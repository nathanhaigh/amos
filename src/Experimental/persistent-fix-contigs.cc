#include "foundation_AMOS.hh"
#include "amp.hh"
#include "fasta.hh"
#include "AMOS_Foundation.hh"
#include "ContigUtils.hh"
#include "PersistentUtils.hh"
#include <sstream>

using namespace AMOS;
using namespace std;

int SAVERESULTS(0);
int FIXCOLLAPSE(0);
int CLOSEGAPS(0);
int SCANALL(0);

int GAPSCLOSED(0);
int COLLAPSEFIX(0);
int MARKLOCATIONS(0);

int VERBOSE(0);

Bank_t master_contig(Contig_t::NCODE);
Bank_t master_reads(Read_t::NCODE);
Bank_t master_scaff(Scaffold_t::NCODE);
BankStream_t master_feat(Feature_t::NCODE);

Bank_t patch_contig(Contig_t::NCODE);
Bank_t patch_reads(Read_t::NCODE);
Bank_t patch_scaff(Scaffold_t::NCODE);
BankStream_t patch_feat(Feature_t::NCODE);

ReadPosLookup read2contigpos;

int main (int argc, char ** argv)
{
  int retval = 0;
  AMOS_Foundation * tf = NULL;

  bool PERFECT_OVL = 0;


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
"   -F <val>  Fix collapsed repeats if delta >= val\n"
"   -G        Close gaps between contigs\n"
"   -s        Save results to bank\n"
"   -S        Do full scan, not just adjacent reads\n"
"   -m        Just mark locations of potential events\n";
"   -v        Be Verbose\n";
"\n";

    // Instantiate a new TIGR_Foundation object
    tf = new AMOS_Foundation (version, helptext, dependencies, argc, argv);
    tf->disableOptionHelp();

    tf->getOptions()->addOptionResult("s",   &SAVERESULTS);
    tf->getOptions()->addOptionResult("F=i", &FIXCOLLAPSE);
    tf->getOptions()->addOptionResult("G",   &CLOSEGAPS);
    tf->getOptions()->addOptionResult("S",   &SCANALL);
    tf->getOptions()->addOptionResult("m",   &MARKLOCATIONS);
    tf->getOptions()->addOptionResult("v",   &VERBOSE);
    tf->handleStandardOptions();

    list<string> argvv = tf->getOptions()->getAllOtherData();

    if (argvv.size() != 2)
    {
      cerr << "Usage: persistent-fix-contigs [options] master patch" << endl;
      return EXIT_FAILURE;
    }

    if (!FIXCOLLAPSE && !CLOSEGAPS)
    {
      cerr << "Nothing to do, exiting." << endl;
      return EXIT_FAILURE;
    }

    string masterbankname = argvv.front(); argvv.pop_front();
    string patchbankname = argvv.front(); argvv.pop_front();

    int mode = B_READ;
    if (SAVERESULTS) { mode = B_READ|B_WRITE; }
    master_contig.open(masterbankname, mode);
    master_scaff.open(masterbankname,  mode);
    master_feat.open(masterbankname,   B_READ|B_WRITE);
    master_reads.open(masterbankname,  B_READ);

    patch_contig.open(patchbankname, mode);
    patch_scaff.open(patchbankname,  mode);
    patch_feat.open(patchbankname,   mode);
    patch_reads.open(patchbankname,  B_READ);

    recordContigPositions(patch_contig, read2contigpos);

    if (FIXCOLLAPSE)
    {
      // Now check the master bank to see if there are contigs
      // with reads that were pulled apart in the patch bank

      cout << "Fixing collapsed repeats (>=" << FIXCOLLAPSE << ") in " << masterbankname << " with " << patchbankname << endl;

      // current
      vector<Tile_t>::iterator m1, m2;

      // best master, patch
      vector<Tile_t>::iterator master1, master2;
      ReadPosLookup::iterator patch1, patch2;
      int masterdist, patchdist;

      // Check every contig for potential collapse
      const IDMap_t & contigmap = master_contig.getIDMap();
      IDMap_t::const_iterator c;
      for (c = contigmap.begin(); c!= contigmap.end(); c++)
      {
        Contig_t contig;
        master_contig.fetch(c->iid, contig);
        string mcons = contig.getSeqString();
        vector<Tile_t> & tiling = contig.getReadTiling();

        sort(tiling.begin(), tiling.end(), TileOrderCmp());
        int tilingsize = tiling.size();

        // do a quick pass to see if there are any large collapses
        bool bigjump = false;
        for (int i = 0; i+1 < tilingsize; i++)
        {
          ReadPosLookup::iterator p1 = read2contigpos.find(tiling[i].source);
          if (p1 == read2contigpos.end()) { continue; } // read[i] not in any patch contig

          // find the first read from the same patch contig
          int j;
          bool foundp2 = false;
          ReadPosLookup::iterator p2;

          for (j = i+1; j < tilingsize; j++)
          {
            p2 = read2contigpos.find(tiling[j].source);
            if (p2 == read2contigpos.end()) { continue; }

            foundp2 = (p2->second.m_contigiid == p1->second.m_contigiid);

            if (VERBOSE) { cout << master_reads.lookupEID(tiling[i].source) << ","  
                                << master_reads.lookupEID(tiling[j].source) << " "
                                << foundp2 << endl; }

            if (foundp2) { break; }
          }

          if (foundp2)
          {
            int consistent;
            int delta, mdist, pdist;

            computeDelta(&tiling[i], &p1->second, &tiling[j], &p2->second,
                         consistent, delta, mdist, pdist);

            if (!consistent)
            {
              if (VERBOSE) { cerr << "Inconsistent orientations detected. master: " << c->iid << " patch: " << p1->second.m_contigiid << endl; }

              if (VERBOSE) 
              { 
                bool m1rc = tiling[i].range.isReverse();
                bool m2rc = tiling[j].range.isReverse();

                bool masterinnie = m1rc ^ m2rc;
                bool patchinnie = p1->second.rc() ^ p2->second.rc();

                cout << "Inconsistent orientations detected "
                     << " masterinnie:" << masterinnie << " m1rc: " << m1rc << " m2rc: " << m2rc
                     << " patchinnie: " << patchinnie  << " p1rc: " << p1->second.m_rc << " p2rc: " << p2->second.m_rc << endl; 
              } 
            }
            else
            {
              if (VERBOSE) { cout << tiling[i].source 
                                  << " " << tiling[j].source 
                                  << " mdist: " << mdist
                                  << " pdist: " << pdist
                                  << " delta: " << delta << endl; }

              if (delta >= FIXCOLLAPSE)
              {
                bigjump = true;
                break;
              }
            }
          }
        }

        if (bigjump || SCANALL)
        {
          int maxdelta = 0;

          for (m1 = tiling.begin(); m1 != tiling.end(); m1++)
          {
            ReadPosLookup::iterator p1 = read2contigpos.find(m1->source);
            if (p1 == read2contigpos.end()) { continue; }

            for (m2 = m1+1; m2 != tiling.end(); m2++)
            {
              ReadPosLookup::iterator p2 = read2contigpos.find(m2->source);
              if (p2 == read2contigpos.end()) { continue; }

              if (p1->second.contig() == p2->second.contig())
              {
                int delta, mdist, pdist;
                int consistent;

                computeDelta(&*m1, &p1->second, &*m2, &p2->second,
                             consistent, delta, mdist, pdist);

                if (!consistent)
                {
 //                 cerr << "Full Scan Inconsistent orientations detected. master: " << c->iid << " patch: " << p1->second.m_contigiid << endl;
                }
                else
                {
                  if (delta >= FIXCOLLAPSE && delta > maxdelta)
                  {
                    bool savedelta = true;

                    if (PERFECT_OVL)
                    {
                      Contig_t pcontig;
                      patch_contig.fetch(p1->second.m_contigiid, pcontig);
                      string pcons = pcontig.getSeqString();

                      string mc1 = mcons.substr(m1->offset, m1->getGappedLength());
                      string pc1 = pcons.substr(p1->second.m_offset, p1->second.m_gappedlen);

                      bool m1rc = m1->range.isReverse();
                      bool m2rc = m2->range.isReverse();

                      if (m1rc)            { Reverse_String(mc1); }
                      if (p1->second.m_rc) { Reverse_String(pc1); }

                      string mc2 = mcons.substr(m2->offset, m2->getGappedLength());
                      string pc2 = pcons.substr(p2->second.m_offset, p2->second.m_gappedlen);

                      if (m2rc)            { Reverse_String(mc2); }
                      if (p2->second.m_rc) { Reverse_String(pc2); }

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
                        savedelta = false;
                      }
                    }

                    if (savedelta)
                    {
                      maxdelta = delta;

                      master1 = m1; patch1 = p1;
                      master2 = m2; patch2 = p2;

                      masterdist = mdist;
                      patchdist  = pdist;
                    }
                  }
                }
              }
            }
          }

          if (maxdelta >= FIXCOLLAPSE)
          {
            bool dostitch = true;

            bool rc = 0; // (master1->range.isReverse() != patch1->second.m_rc);

            if (rc) { swap(master1, master2); swap(patch1, patch2); }

            string leftstitchread  = master_reads.lookupEID(master1->source);
            string rightstitchread = master_reads.lookupEID(master2->source);

            cout << ">Stitch mcontig: " << contig.getIID() << " (" << contig.getReadTiling().size() << ") pcontig: " << patch1->second.m_contigiid 
                 << " delta: " << maxdelta << " left: " << leftstitchread << " right: " << rightstitchread 
                 << " " << rc << endl;

            cout <<     "master1: " << master1->offset << "," << master1->getRightOffset() << (master1->range.isReverse() ? "-" : "+") 
                 << "\t | patch1: " << patch1->second.m_offset << "," << patch1->second.getRightOffset() << (patch1->second.m_rc ? "-" : "+") << endl

                 <<     "master2: " << master2->offset << "," << master2->getRightOffset() << (master2->range.isReverse() ? "-" : "+")
                 << "\t | patch2: " << patch2->second.m_offset << "," << patch2->second.getRightOffset() << (patch2->second.m_rc ? "-" : "+") << endl

                 << "masterdist: " << masterdist << "\t | patchdist: " << patchdist << endl;

            //cout << "p2pos: "  << patch2->second.pos(master2->range.isReverse()) << " p1pos: " << patch1->second.pos(master1->range.isReverse()) << endl;

            if (dostitch)
            {
              COLLAPSEFIX++;

              if (SAVERESULTS)
              {
                if (rc) { reverseContig(master_contig, master_scaff, master_feat, contig.getIID()); }

                set<ID_t> masterreads, patchreads;
                Range_t stitchRegion;

                stitchContigs(master_contig, master_reads, master_scaff, master_feat,
                              patch_contig,  patch_reads,  patch_feat,
                              leftstitchread, rightstitchread, 
                              masterreads, patchreads, stitchRegion,
                              contig.getIID(), 0, patch1->second.m_contigiid,
                              false, PERFECT_OVL);
              }
              else if (MARKLOCATIONS)
              {
                Feature_t feat;
                feat.setRange(Range_t(master1->offset, master2->getRightOffset()));
                feat.setSource(make_pair(contig.getIID(), Contig_t::NCODE));
                feat.setType('F');

                stringstream comment;
                comment << "Potential Collapsed Repeat delta=" << maxdelta;
                comment << " patch: " << patchbankname;

                feat.setComment(comment.str());

                master_feat << feat;
              }
            }

            cout << endl << endl;
          }
        }
      }
    }

    if (CLOSEGAPS)
    {
      map <ID_t, bool> patchrc;
      cerr << "Closing Gaps" << endl;

      const IDMap_t & scaffmap = master_scaff.getIDMap();
      IDMap_t::const_iterator s;
      for (s = scaffmap.begin(); s != scaffmap.end(); s++)
      {
        Scaffold_t scaff;
        master_scaff.fetch(s->iid, scaff);

        vector<Tile_t> & ctiling = scaff.getContigTiling();
        int l = ctiling.size();

        // Check if there are potential gaps to close
        if (l < 2) { continue; }

        sort(ctiling.begin(), ctiling.end(), TileOrderCmp());

        Contig_t c1, c2;

        master_contig.fetch(ctiling[0].source, c1);

        for (int i = 1; i < l; i++)
        {
          master_contig.fetch(ctiling[i].source, c2);

          set<ID_t> c1contigs;
          set<ID_t> c2contigs;
          set<ID_t> joincontigs;
          set<ID_t>::iterator si;

          vector<Tile_t>::iterator ti;
          for (ti = c1.getReadTiling().begin(); ti != c1.getReadTiling().end(); ti++)
          {
            ReadPosLookup::iterator r = read2contigpos.find(ti->source);

            if (r != read2contigpos.end())
            {
              c1contigs.insert(r->second.m_contigiid);
            }
          }

          for (ti = c2.getReadTiling().begin(); ti != c2.getReadTiling().end(); ti++)
          {
            ReadPosLookup::iterator r = read2contigpos.find(ti->source);
            if (r != read2contigpos.end())
            {
              if (c1contigs.find(r->second.m_contigiid) != c1contigs.end())
              {
                joincontigs.insert(r->second.m_contigiid);
                c2contigs.insert(r->second.m_contigiid);
              }
            }
          }

          bool madestitch = false;

          if (!joincontigs.empty())
          {
            cout << ">Gap " << c1.getIID() << " " << c2.getIID()
                 << " " << joincontigs.size() << " patches:";

            for (si = joincontigs.begin(); si != joincontigs.end(); si++)
            {
              cout << " " << *si;
            }
            cout << endl;

            if (joincontigs.size() == 1)
            {
              ID_t joiniid = *(joincontigs.begin());

              vector<Tile_t>::iterator master1, master2;
              ReadPosLookup::iterator  patch1, patch2;
              vector<Tile_t> & c1tiling = c1.getReadTiling();
              sort(c1tiling.begin(), c1tiling.end(), TileOrderCmp());
              reverse(c1tiling.begin(), c1tiling.end());

              for (master1 = c1tiling.begin(); master1 != c1tiling.end(); master1++)
              {
                patch1 = read2contigpos.find(master1->source);

                if (patch1 != read2contigpos.end() && patch1->second.m_contigiid == joiniid)
                {
                  break;
                }
              }

              vector<Tile_t> & c2tiling = c2.getReadTiling();
              sort(c2tiling.begin(), c2tiling.end(), TileOrderCmp());
              for (master2 = c2tiling.begin(); master2 != c2tiling.end(); master2++)
              {
                patch2 = read2contigpos.find(master2->source);

                if (patch2 != read2contigpos.end() && patch2->second.m_contigiid == joiniid)
                {
                  break;
                }
              }

              int rc = (patch1->second.m_offset > patch2->second.m_offset) ? 1 : 0;
              int rc1 = (master1->range.isReverse() != patch1->second.m_rc);
              int rc2 = (master2->range.isReverse() != patch2->second.m_rc);

              string leftstitchread  = master_reads.lookupEID(master1->source);
              string rightstitchread = master_reads.lookupEID(master2->source);

              ID_t patchiid = patch1->second.m_contigiid;

              cout << ">Stitch mcontig: " << c1.getIID() << " mcontig2:" << c2.getIID() << " pcontig: " << patchiid
                   << " left: " << leftstitchread << " right: " << rightstitchread 
                   << " rc:" << rc << " rc1:"  << rc1 << " rc2: " << rc2 << endl;

              map <ID_t, bool>::iterator prc = patchrc.find(patchiid);

              if (prc != patchrc.end())
              {
                if (prc->second)
                {
                  rc = !rc; rc1 = !rc1; rc2 = !rc2; 
                  cout << "Patch was previously flipped: rc:" << rc << " rc1: " << rc1 << " rc2: " << rc2 <<  endl;
                }
              }

              bool dostitch = true;

              if ((patch1->second.m_offset + patch1->second.m_gappedlen) > 
                  (patch2->second.m_offset + patch2->second.m_gappedlen))
              {
                cerr << "patch1 contains patch2!!!" << endl;
 //               dostitch = false;
              }

              if ((patch2->second.m_offset + patch2->second.m_gappedlen) > 
                  (patch1->second.m_offset + patch1->second.m_gappedlen))
              {
                cerr << "patch2 contains patch1!!!" << endl;
   //             dostitch = false;
              }

              if (dostitch)
              {
                GAPSCLOSED++;
                
                if  (SAVERESULTS)
                {
                  if (rc)
                  {
                    cerr << "patchrc, reversing patch, rc1, rc2" << endl;
                    reverseContig(patch_contig, patch_scaff, patch_feat, patchiid);
                    rc1 = !rc1;
                    rc2 = !rc2;

                    if (prc != patchrc.end())
                    {
                      prc->second = !prc->second;
                    }
                    else
                    {
                      patchrc.insert(make_pair(patchiid, true));
                    } 
                  }

                  if (rc1) { reverseContig(master_contig, master_scaff, master_feat, c1.getIID()); }
                  if (rc2) { reverseContig(master_contig, master_scaff, master_feat, c2.getIID()); }

                  set<ID_t> masterreads, patchreads;
                  Range_t stitchRegion;

                  try
                  {
                    stitchContigs(master_contig, master_reads, master_scaff, master_feat,
                                  patch_contig,  patch_reads,  patch_feat,
                                  leftstitchread, rightstitchread, 
                                  masterreads, patchreads, stitchRegion,
                                  c1.getIID(), c2.getIID(), patch1->second.m_contigiid,
                                  false, PERFECT_OVL);

                    master_contig.fetch(c1.getIID(), c1);
                    madestitch = true;
                  }
                  catch (Exception_t & e)
                  {
                    cerr << "Stitch failed: " << e << endl;
                    madestitch = false;
                  }
                }
              }

              cout << endl << endl;
            }
          }

          if (!madestitch)
          {
            c1 = c2;
          }
        }
      }
    }

    cout << "@Summary:"
         << " Master "    << masterbankname
         << " Patch "     << patchbankname 
         << " Closed "    << GAPSCLOSED << " gaps" 
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
