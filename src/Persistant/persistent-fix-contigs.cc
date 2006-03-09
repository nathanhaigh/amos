#include "foundation_AMOS.hh"
#include "amp.hh"
#include "fasta.hh"
#include "AMOS_Foundation.hh"
#include "ContigUtils.hh"

using namespace AMOS;
using namespace std;

int MINDELTA(0);
int DOSTITCH(0);

class ReadPosInfo
{
public:
  ReadPosInfo(ID_t contigiid, int pos, int offset, int gappedlen, bool rc)
    : m_contigiid(contigiid), m_5pos(pos), m_offset(offset), m_gappedlen(gappedlen), m_rc(rc) {}

  ID_t m_contigiid;
  int m_5pos;
  int m_offset;
  int m_gappedlen;
  bool m_rc;
};


typedef map<ID_t, ReadPosInfo > ReadPosLookup;
ReadPosLookup read2contigpos;


int main (int argc, char ** argv)
{
  int retval = 0;
  AMOS_Foundation * tf = NULL;

  Bank_t master_contig(Contig_t::NCODE);
  Bank_t master_reads(Read_t::NCODE);
  Bank_t master_scaff(Scaffold_t::NCODE);
  BankStream_t master_feat(Feature_t::NCODE);

  Bank_t patch_contig(Contig_t::NCODE);
  Bank_t patch_reads(Read_t::NCODE);
  BankStream_t patch_feat(Feature_t::NCODE);

  try
  {
    string version =  "Version 1.0";
    string dependencies = "";
    string helptext = 
"Fixes contigs in one assembly based on contigs in another\n"
"\n"
"   Usage: persistent-fix-contigs [options] master patch\n"
"\n"
"   Options\n"
"   -------------------\n"
"   -d <val>  Only fix if delta >= val\n"
"   -s        Do the stitching\n"
"\n";

    // Instantiate a new TIGR_Foundation object
    tf = new AMOS_Foundation (version, helptext, dependencies, argc, argv);
    tf->disableOptionHelp();

    tf->getOptions()->addOptionResult("d=i", &MINDELTA,       "Be verbose when reporting");
    tf->getOptions()->addOptionResult("s",   &DOSTITCH,       "Be verbose when reporting");
    tf->handleStandardOptions();

    list<string> argvv = tf->getOptions()->getAllOtherData();

    if (argvv.size() != 2)
    {
      cerr << "Usage: persistent-fix-contigs [options] master patch" << endl;
      return EXIT_FAILURE;
    }

    string masterbankname = argvv.front(); argvv.pop_front();
    string patchbankname = argvv.front(); argvv.pop_front();

    master_contig.open(masterbankname, B_READ|B_WRITE);
    master_reads.open(masterbankname, B_READ|B_WRITE);
    master_scaff.open(masterbankname, B_READ|B_WRITE);
    master_feat.open(masterbankname, B_READ|B_WRITE);

    patch_contig.open(patchbankname, B_READ);
    patch_reads.open(patchbankname, B_READ);
    patch_feat.open(patchbankname, B_READ);


    // Record the position of every read in the patch bank
    IDMap_t::const_iterator c;
    const IDMap_t & patchmap = patch_contig.getIDMap();
    for (c = patchmap.begin(); c!= patchmap.end(); c++)
    {
      Contig_t contig;
      patch_contig.fetch(c->iid, contig);

      vector<Tile_t> & tiling = contig.getReadTiling();
      vector<Tile_t>::const_iterator ti;
      for (ti = tiling.begin(); ti != tiling.end(); ti++)
      {
        int gappedlen = ti->getGappedLength();

        int t1pos = ti->offset;
        if (ti->range.isReverse()) { t1pos = t1pos + gappedlen; }

        read2contigpos.insert(make_pair(ti->source, ReadPosInfo(c->iid, t1pos, ti->offset, gappedlen, ti->range.isReverse())));
      }
    }

    // Now check the master bank to see if there are contigs
    // with reads that were pulled apart in the patch bank


    const IDMap_t & contigmap = master_contig.getIDMap();
    for (c = contigmap.begin(); c!= contigmap.end(); c++)
    {
      Contig_t contig;
      master_contig.fetch(c->iid, contig);

      string mcons = contig.getSeqString();

      vector<Tile_t> & tiling = contig.getReadTiling();
      vector<Tile_t>::const_iterator t1;
      vector<Tile_t>::const_iterator t2;

      int maxdist = 0;

      ReadPosLookup::const_iterator maxa;
      ReadPosLookup::const_iterator maxb;

      vector<Tile_t>::const_iterator maxt1;
      vector<Tile_t>::const_iterator maxt2;


      int tilingsize = tiling.size();

      for (t1 = tiling.begin(); t1 != tiling.end(); t1++)
      {
        int t1pos = t1->offset;
        if (t1->range.isReverse()) { t1pos = t1pos + t1->getGappedLength() - 1; }

        for (t2 = t1+1; t2 != tiling.end(); t2++)
        {
          int t2pos = t2->offset;
          if (t2->range.isReverse()) { t2pos = t2pos + t2->getGappedLength() - 1; }

          int dist = abs(t2pos - t1pos);
          int olddist = 0;
          int delta = 0;
          int absdelta = 0;

          ReadPosLookup::const_iterator r1 = read2contigpos.find(t1->source);
          ReadPosLookup::const_iterator r2 = read2contigpos.find(t2->source);

          if (r1 != read2contigpos.end() &&
              r2 != read2contigpos.end() &&
              r1->second.m_contigiid == r2->second.m_contigiid)
          {
            olddist = abs(r1->second.m_5pos - r2->second.m_5pos);
            delta = olddist - dist;

            if (delta > maxdist && delta > MINDELTA)
            {
              Contig_t pcontig;
              patch_contig.fetch(r1->second.m_contigiid, pcontig);
              string pcons = pcontig.getSeqString();

              string mc1 = mcons.substr(t1->offset, t1->getGappedLength());
              string pc1 = pcons.substr(r1->second.m_offset, r1->second.m_gappedlen);

              if (t1->range.isReverse()) { Reverse_String(mc1); }
              if (r1->second.m_rc)       { Reverse_String(pc1); }

              string mc2 = mcons.substr(t2->offset, t2->getGappedLength());
              string pc2 = pcons.substr(r2->second.m_offset, r2->second.m_gappedlen);

              if (t2->range.isReverse()) { Reverse_String(mc2); }
              if (r2->second.m_rc)       { Reverse_String(pc2); }

              if (mc1 != pc1 || mc2 != pc2)
              {
                cerr << "Contig " << contig.getIID() << " within dist (" << delta 
                     << "), but consensus mismatch!!!" << endl;

                cerr << "mc1: " << mc1 << endl;
                cerr << "pc1: " << pc1 << endl;

                cerr << endl;
                cerr << "mc2: " << mc2 << endl;
                cerr << "pc2: " << pc2 << endl;
                cerr << endl;
                cerr << endl;
              }
              else
              {
                maxdist = delta;

                if (t1->offset < t2->offset)
                {
                  maxt1 = t1; maxt2 = t2;
                  maxa = r1; maxb = r2;
                }
                else
                {
                  maxt1 = t2; maxt2 = t1;
                  maxa = r2;  maxb = r2;
                }
              }
            }
          }
        }
      }

      if (maxdist >= MINDELTA)
      {
        string leftstitchread  = master_reads.lookupEID(maxt1->source);
        string rightstitchread = master_reads.lookupEID(maxt2->source);

        int rc = (maxa->second.m_offset > maxb->second.m_offset) ? 1 : 0;

        cout << ">Stitch mcontig: " << contig.getIID() << " pcontig: " << maxa->second.m_contigiid 
             << " delta: " << maxdist << " left: " << leftstitchread << " right: " << rightstitchread 
             << " " << rc << " " << endl;

        if (DOSTITCH)
        {
          if (rc) { reverseContig(master_contig, master_scaff, master_feat, contig.getIID()); }

          set<ID_t> masterreads, patchreads;
          Range_t stitchRegion;

          stitchContigs(master_contig, master_reads, master_scaff, master_feat,
                        patch_contig,  patch_reads,  patch_feat,
                        leftstitchread, rightstitchread, 
                        masterreads, patchreads, stitchRegion,
                        contig.getIID(), 0, maxa->second.m_contigiid,
                        false);
        }
      }
    }
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

  cerr << "End: " << Date() << endl;
  return retval;
}
