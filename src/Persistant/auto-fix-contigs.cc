#include "foundation_AMOS.hh"
#include "amp.hh"
#include "fasta.hh"
#include "AMOS_Foundation.hh"
#include "ContigUtils.hh"
#include "PersistentUtils.hh"
#include "DataStore.hh"
#include <sstream>
#include "delta.hh"

using namespace AMOS;
using namespace std;

int SAVERESULTS(0);
int COLLAPSEFIX(0);
int MARKLOCATIONS(0);
int END_TRIM(500);

int VERBOSE(0);

typedef map<ID_t, CoverageStats> LibCE;


struct AlignmentInfo_t
{
  int sR;
  int eR;
  int sQ;
  int eQ;

  bool prefix;
  bool suffix;

  AlignmentInfo_t(int _sR, int _eR, int _sQ, int _eQ, int lenR, int lenQ) 
   : sR(_sR), eR(_eR), sQ(_sQ), eQ(_eQ) 
  {
    prefix = (sR - 1)    < END_TRIM;
    suffix = (lenR - eR) < END_TRIM;
  }

  bool qrc() { return eQ < sQ; }
};

struct AlignmentInfoCmp
{
  bool operator() (const AlignmentInfo_t & a, const AlignmentInfo_t & b)
  {
    return a.sR < b.sR;
  }
};

pair<double, double> ceextreme(LibCE & ce, Range_t & rng, Contig_t ctg)
{
  double maxlo = 0, maxhi = 0;

  int CERANGE = 10000;

  int start = ctg.ungap2gap(rng.begin) - CERANGE; 
  int end   = ctg.ungap2gap(rng.end)   + CERANGE;

  for (LibCE::iterator li = ce.begin(); li != ce.end(); li++)
  {
    for (int i = 1; i < li->second.m_curpos; i+=2)
    {
      int x = (int)li->second.m_coverage[i].x();

      if ((start <= x) && (x <= end))
      {
        double ce = li->second.m_cestat[i];
        
        if (ce > maxhi) { maxhi = ce; }
        if (ce < maxlo) { maxlo = ce; }
      }
    }
  }

  return make_pair(maxlo, maxhi);
}


int main (int argc, char ** argv)
{
  int retval = 0;
  AMOS_Foundation * tf = NULL;

  DataStore reference;
  DataStore query;

  try
  {
    string version =  "Version 1.0";
    string dependencies = "";
    string helptext = 
"Fixes contigs in one assembly based on alignment to contigs in another.\n"
"The correct contig is selected based on the ce-statistic\n"
"\n"
"   Usage: auto-fix-contigs [options] reference query delta.rq\n"
"\n"
"   Options\n"
"   -------------------\n"
"   -s        Save results to bank\n"
"   -m        Just mark locations of potential events\n";
"   -v        Be Verbose\n";
"\n";

    // Instantiate a new TIGR_Foundation object
    tf = new AMOS_Foundation (version, helptext, dependencies, argc, argv);
    tf->disableOptionHelp();

    tf->getOptions()->addOptionResult("s",   &SAVERESULTS);
    tf->getOptions()->addOptionResult("m",   &MARKLOCATIONS);
    tf->getOptions()->addOptionResult("v",   &VERBOSE);
    tf->handleStandardOptions();

    list<string> argvv = tf->getOptions()->getAllOtherData();

    if (argvv.size() != 3)
    {
      cerr << "Usage: auto-fix-contigs [options] reference query delta" << endl;
      return EXIT_FAILURE;
    }

    string referencebankname = argvv.front(); argvv.pop_front();
    string querybankname  = argvv.front(); argvv.pop_front();
    string deltafile      = argvv.front(); argvv.pop_front();

    reference.openBank(referencebankname);
    query.openBank(querybankname);

    int mode = B_READ;
    if (SAVERESULTS) 
    { 
      mode = B_READ|B_WRITE; 
      reference.contig_bank.close();
      reference.contig_bank.open(referencebankname, mode);

      reference.scaffold_bank.close();
      reference.scaffold_bank.open(referencebankname, mode);
    }

    reference.feat_bank.close();
    reference.feat_bank.open(referencebankname, B_READ|B_WRITE);

    DeltaReader_t deltareader;
    deltareader.open(deltafile);

    cout << endl << "Analyzing Alignments" << endl;

    while (deltareader.readNextHeadersOnly())
    {
      const DeltaRecord_t & record = deltareader.getRecord();

      typedef vector<AlignmentInfo_t> AlignVector;
      AlignVector aligns;

      vector<DeltaAlignment_t>::const_iterator ai;
      for (ai = record.aligns.begin();
           ai != record.aligns.end();
           ai++)
      {
        AlignmentInfo_t a(ai->sR, ai->eR, ai->sQ, ai->eQ, record.lenR, record.lenQ);
        aligns.push_back(a);
      }

      if (aligns.size() > 1)
      {
        sort(aligns.begin(), aligns.end(), AlignmentInfoCmp());

        cout << ">" << record.idR << "\t" << record.idQ << endl;

        AlignVector::iterator cur, prev;

        Contig_t refctg, qryctg;
        vector<Insert *> refinserts, qryinserts;

        LibCE refce, qryce;

        reference.fetchContigEID(record.idR, refctg);
        reference.calculateInserts(refctg.getReadTiling(), refinserts, 1, 0);
        refce = reference.computeCEStats(refinserts);

        query.fetchContigEID(record.idQ, qryctg);
        query.calculateInserts(qryctg.getReadTiling(), qryinserts, 1, 0);
        qryce = query.computeCEStats(qryinserts);


        for (prev=aligns.end(), cur = aligns.begin(); 
             cur != aligns.end(); 
             prev=cur, cur++)
        {
          cout << cur->sR << "\t" << cur->eR << " | " << cur->sQ << "\t" << cur->eQ << "\t"; 

          if      (cur->prefix && cur->suffix) { cout << "[FULL] "; }
          else if (cur->prefix)                { cout << "[PREF] "; }
          else if (cur->suffix)                { cout << "[SUFF] "; }
          else                                 { cout << "[INTL] "; }

          if (prev != aligns.end())
          {
            if (cur->qrc() != prev->qrc()) { cout << "Inconsistent qdir" << endl; continue; }

            Range_t ref(min(prev->eR, cur->sR), max(prev->eR, cur->sR));
            Range_t qry(prev->eQ, cur->sQ);

            bool qrc = false;

            if (cur->qrc()) { qry.swap(); qrc = true; }

            int refdst   = ref.end - ref.begin;
            int qrydst   = qry.end - qry.begin;
            int delta = refdst - qrydst;

            pair<double,double> refextreme = ceextreme(refce, ref, refctg);
            pair<double,double> qryextreme = ceextreme(qryce, qry, qryctg);

            cout << refdst << " " << qrydst << " : " << delta;
            cout << " | " << refextreme.first << "," << refextreme.second 
                 << " | " << qryextreme.first << "," << qryextreme.second;

            if (refextreme.first < qryextreme.first)
            {
              cout << " Confirmed collapse in reference!: " << ref.begin << "," << ref.end << endl;

              ReadPosLookup refpos, querypos;
              recordReadPositions(refctg.getIID(), refctg.getReadTiling(), refpos);
              recordReadPositions(qryctg.getIID(), qryctg.getReadTiling(), querypos);

              ReadPosLookup::iterator ri, rlow, rhigh, qi, qlow, qhigh;
              rlow = refpos.end();
              rhigh = refpos.end();

              for (ri = refpos.begin(); ri != refpos.end(); ri++)
              {
                if ((ri->second.getRightOffset() < ref.begin) &&
                    ((rlow == refpos.end()) || (ri->second.getRightOffset() > rlow->second.getRightOffset())))
                {
                  qi = querypos.find(ri->first);
                  if ((qi != querypos.end()) && (qi->second.getRightOffset() < qry.begin)) { rlow = ri; qlow=qi; }
                }

                if ((ri->second.m_offset > ref.end) &&
                    ((rhigh == refpos.end()) || (ri->second.m_offset < rhigh->second.m_offset)))
                {
                  qi = querypos.find(ri->first);
                  if ((qi != querypos.end()) && (qi->second.m_offset > qry.end)) { rhigh = ri; qhigh=qi; }
                }
              }

              bool foundlow = false;
              bool foundhi = false;
              if (rlow != refpos.end())
              {
                foundlow = true;
                cout << "Found rlow: " << rlow->first 
                     << " [" << rlow->second.m_offset << ", " << rlow->second.getRightOffset() << "]" << endl;
              }

              if (rhigh != refpos.end())
              {
                foundhi = true;
                cout << "Found rhigh: " << rhigh->first 
                     << " [" << rhigh->second.m_offset << ", " << rhigh->second.getRightOffset() << "]" << endl;
              }

              if (foundlow && foundhi)
              {
                cout << "Found low and hi" << endl;
              }
              else
              {
                cout << "Didn't find both" << endl;
              }

              if (SAVERESULTS && foundlow && foundhi)
              {
                set<ID_t> masterreads, patchreads;
                Range_t stitchRegion;

                bool PERFECT_OVL = false;
                bool SAVE_TO_NEW_REF_CONTIG = false;

                stitchContigs(reference.contig_bank, reference.read_bank, reference.scaffold_bank, reference.feat_bank,
                              query.contig_bank,     query.read_bank,     query.feat_bank,
                              reference.read_bank.lookupEID(rlow->first), 
                              reference.read_bank.lookupEID(rhigh->first),
                              masterreads, patchreads, stitchRegion,
                              refctg.getIID(), 0, qryctg.getIID(),
                              SAVE_TO_NEW_REF_CONTIG, PERFECT_OVL);
              }
              else
              {
                Feature_t feat;
                feat.setRange(ref);
                feat.setSource(make_pair(refctg.getIID(), Contig_t::NCODE));
                feat.setType('F');

                stringstream comment;
                comment << "Potential Collapsed Repeat delta=" << delta;
                comment << " patch: " << querybankname;
                feat.setComment(comment.str());

                reference.feat_bank << feat;
              }

              COLLAPSEFIX++;
            }
          }

          cout << endl;
        }

        cout << endl;
      }
    }

    cout << "@Summary:"
         << " reference " << referencebankname
         << " query "     << querybankname 
         << " Corrected " << COLLAPSEFIX << " collapsed repeats"  << endl;
  }
  catch (Exception_t & e)
  {
    cerr << "ERROR: -- Fatal AMOS Exception --\n" << e;
    retval = 1;
  }
  catch (const ExitProgramNormally & e)
  {
    retval = 0;
  }
  catch (const amosException & e)
  {
    cerr << e << endl;
    retval = 100;
  }

  try
  {
    if (tf) delete tf;
  }
  catch (const amosException & e)
  {
    cerr << "amosException while deleting tf: " << e << endl;
    retval = 105;
  }

  return retval;
}
