#include "foundation_AMOS.hh"
#include "amp.hh"
#include "fasta.hh"
#include "AMOS_Foundation.hh"
#include "ContigUtils.hh"

using namespace AMOS;
using namespace std;

int MINDELTA(1);
int DOSTITCH(0);
int FIXCOLLAPSE(0);
int CLOSEGAPS(0);

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

  bool PERFECT_OVL = 0;

  Bank_t master_contig(Contig_t::NCODE);
  Bank_t master_reads(Read_t::NCODE);
  Bank_t master_scaff(Scaffold_t::NCODE);
  BankStream_t master_feat(Feature_t::NCODE);

  Bank_t patch_contig(Contig_t::NCODE);
  Bank_t patch_reads(Read_t::NCODE);
  Bank_t patch_scaff(Scaffold_t::NCODE);
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
"   -C        Fix collapsed repeats\n"
"   -G        Close gaps between contigs\n"
"   -d <val>  Only fix collapsed repeats if delta >= val\n"
"   -s        Save results to bank\n"
"\n";

    // Instantiate a new TIGR_Foundation object
    tf = new AMOS_Foundation (version, helptext, dependencies, argc, argv);
    tf->disableOptionHelp();

    tf->getOptions()->addOptionResult("d=i", &MINDELTA,       "Be verbose when reporting");
    tf->getOptions()->addOptionResult("s",   &DOSTITCH,       "Be verbose when reporting");
    tf->getOptions()->addOptionResult("C", &FIXCOLLAPSE,    "Be verbose when reporting");
    tf->getOptions()->addOptionResult("G",   &CLOSEGAPS,      "Be verbose when reporting");
    tf->handleStandardOptions();

    list<string> argvv = tf->getOptions()->getAllOtherData();

    if (argvv.size() != 2)
    {
      cerr << "Usage: persistent-fix-contigs [options] master patch" << endl;
      return EXIT_FAILURE;
    }

    string masterbankname = argvv.front(); argvv.pop_front();
    string patchbankname = argvv.front(); argvv.pop_front();

    int mode = B_READ;
    if (DOSTITCH) { mode = B_READ|B_WRITE; }
    master_contig.open(masterbankname, mode);
    master_scaff.open(masterbankname,  mode);
    master_feat.open(masterbankname,   mode);
    master_reads.open(masterbankname,  B_READ);

    patch_contig.open(patchbankname, mode);
    patch_scaff.open(patchbankname,  mode);
    patch_feat.open(patchbankname,   mode);
    patch_reads.open(patchbankname,  B_READ);


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

        int pos = ti->offset;
        if (ti->range.isReverse()) { pos = ti->getRightOffset(); }

        read2contigpos.insert(make_pair(ti->source, ReadPosInfo(c->iid, 
                                                                pos, 
                                                                ti->offset, 
                                                                gappedlen, 
                                                                ti->range.isReverse())));
      }
    }

    if (FIXCOLLAPSE)
    {
      // Now check the master bank to see if there are contigs
      // with reads that were pulled apart in the patch bank

      cerr << "Fixing collapsed repeats" << endl;

      const IDMap_t & contigmap = master_contig.getIDMap();
      for (c = contigmap.begin(); c!= contigmap.end(); c++)
      {
        Contig_t contig;
        master_contig.fetch(c->iid, contig);
        string mcons = contig.getSeqString();
        vector<Tile_t> & tiling = contig.getReadTiling();

        vector<Tile_t>::const_iterator m1;
        vector<Tile_t>::const_iterator m2;

        vector<Tile_t>::const_iterator master1;
        vector<Tile_t>::const_iterator master2;


        ReadPosLookup::const_iterator patch1;
        ReadPosLookup::const_iterator patch2;

        int maxdist = 0;


        sort(tiling.begin(), tiling.end(), TileOrderCmp());
        int tilingsize = tiling.size();

        for (m1 = tiling.begin(); m1 != tiling.end(); m1++)
        {
          int m1pos = m1->offset;
          if (m1->range.isReverse()) { m1pos = m1->getRightOffset(); }

          for (m2 = m1+1; m2 != tiling.end(); m2++)
          {
            int m2pos = m2->offset;
            if (m2->range.isReverse()) { m2pos = m2->getRightOffset(); }

            int dist = abs(m2pos - m1pos);

            ReadPosLookup::const_iterator p1 = read2contigpos.find(m1->source);
            ReadPosLookup::const_iterator p2 = read2contigpos.find(m2->source);

            if (p1 != read2contigpos.end() &&
                p2 != read2contigpos.end() &&
                p1->second.m_contigiid == p2->second.m_contigiid)
            {
              int olddist = abs(p1->second.m_5pos - p2->second.m_5pos);
              int delta = olddist - dist;

              if (delta > maxdist && delta >= MINDELTA)
              {
                if (PERFECT_OVL)
                {
                  Contig_t pcontig;
                  patch_contig.fetch(p1->second.m_contigiid, pcontig);
                  string pcons = pcontig.getSeqString();

                  string mc1 = mcons.substr(m1->offset, m1->getGappedLength());
                  string pc1 = pcons.substr(p1->second.m_offset, p1->second.m_gappedlen);

                  if (m1->range.isReverse()) { Reverse_String(mc1); }
                  if (p1->second.m_rc)       { Reverse_String(pc1); }

                  string mc2 = mcons.substr(m2->offset, m2->getGappedLength());
                  string pc2 = pcons.substr(p2->second.m_offset, p2->second.m_gappedlen);

                  if (m2->range.isReverse()) { Reverse_String(mc2); }
                  if (p2->second.m_rc)       { Reverse_String(pc2); }

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
                    continue;
                  }
                }

                maxdist = delta;

                if (m1->offset < m2->offset)
                {
                  master1 = m1; master2 = m2;
                  patch1  = p1; patch2  = p2;
                }
                else
                {
                  master1 = m2; master2 = m1;
                  patch1  = p2; patch2  = p1;
                }
              }
            }
          }
        }

        if (maxdist >= MINDELTA)
        {
          bool dostitch = true;

          int rc = (patch1->second.m_offset > patch2->second.m_offset) ? 1 : 0;

          if (rc)
          {
             vector<Tile_t>::const_iterator t = master1;
             master1 = master2; master2 = t;

             ReadPosLookup::const_iterator p = patch1;
             patch1 = patch2; patch2 = p;
          }



          string leftstitchread  = master_reads.lookupEID(master1->source);
          string rightstitchread = master_reads.lookupEID(master2->source);


          cout << ">Stitch mcontig: " << contig.getIID() << " pcontig: " << patch1->second.m_contigiid 
               << " delta: " << maxdist << " left: " << leftstitchread << " right: " << rightstitchread 
               << " " << rc << " " << endl;

          if ((patch1->second.m_offset + patch1->second.m_gappedlen) > 
              (patch2->second.m_offset))
          {
            cerr << "patch1 contains patch2!!!" << endl;
            dostitch = false;
          }

          if ((patch2->second.m_offset + patch2->second.m_gappedlen) > 
              (patch1->second.m_offset))
          {
            cerr << "patch2 contains patch1!!!" << endl;
            dostitch = false;
          }

          if (rc)
          {
             if ((patch1->second.m_rc == master1->range.isReverse()) ||
                 (patch2->second.m_rc == master2->range.isReverse()))
             {
               cerr << "Impossible orientations rc1!!!";
               dostitch = false;
             }
          }
          else
          {
             if ((patch1->second.m_rc != master1->range.isReverse()) ||
                 (patch2->second.m_rc != master2->range.isReverse()))
             {
               cerr << "Impossible orientations rc0!!!" << endl;
               cerr << "patch1: " << patch1->second.m_rc << " " << patch1->second.m_offset << endl;
               cerr << "master1: " << (master1->range.isReverse() ? 1 : 0) << " " << master1->offset << endl;

               cerr << "patch2: " << patch2->second.m_rc << " " << patch2->second.m_offset << endl;
               cerr << "master2: " << (master2->range.isReverse() ? 1 : 0) << " " << master2->offset << endl;

               dostitch = false;
             }
          }

          if (dostitch && DOSTITCH)
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

          cout << endl << endl;
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

              if (dostitch && DOSTITCH)
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

                cout << endl << endl;
              }
            }
          }

          if (!madestitch)
          {
            c1 = c2;
          }
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
