#include "ContigUtils.hh"

using namespace std;
using namespace AMOS;

// Trim 'lefttrim' bases from a contig by trimming the consensus and then
// adjusting the offset and clear range of the tiling reads, or by 
// dropping the read entirely from the multiple alignment
void leftTrimContig(Contig_t & contig, int lefttrim)
{
  string cons = contig.getSeqString();
  string cqual = contig.getQualString();

  cerr << "Left Trimming " << lefttrim << " bases from consensus length of " << cons.length() << endl;

  cons.erase(0, lefttrim);
  cqual.erase(0, lefttrim);

  contig.setSequence(cons,cqual);

  int curread = 0;
  int tilecount = 0;
  vector<Tile_t> & tiling = contig.getReadTiling();

  for (curread = 0; curread < tiling.size(); curread++)
  {
    if (tiling[curread].getRightOffset() >= lefttrim)
    {
      // This read should exist in the new tiling
      tiling[curread].offset -= lefttrim;
      
      if (tiling[curread].offset < 0)
      {
        // fix range and gaps
        int trim = -tiling[curread].offset;
        tiling[curread].offset = 0;

        vector<Pos_t> & gaps = tiling[curread].gaps;
        int curgap = 0;
        int gapstodrop = 0;
        int gapstokeep = 0;

        for (curgap = 0; curgap < gaps.size(); curgap++)
        {
          if (gaps[curgap] + curgap < trim) { gapstodrop++; }
          else
          {
            gaps[gapstokeep] = gaps[curgap]-(trim-gapstodrop);
            gapstokeep++;
          }
        }

        gaps.resize(gapstokeep);

        if (tiling[curread].range.isReverse())
        {
          tiling[curread].range.begin -= (trim - gapstodrop);
        }
        else
        {
          tiling[curread].range.begin += (trim - gapstodrop);
        }
      }

      tiling[tilecount] = tiling[curread];
      tilecount++;
    }
  }

  tiling.resize(tilecount);
}


// Right trim the contig to the new length
void lengthTrimContig(Contig_t & contig, int newlength)
{
  string cons = contig.getSeqString();
  string cqual = contig.getQualString();

  int righttrim = cons.length() - newlength;

  cerr << "Right Trimming " << righttrim << " bases from " << cons.length() << endl;

  cons.resize(newlength);
  cqual.resize(newlength);

  contig.setSequence(cons,cqual);

  int curread = 0;
  int tilecount = 0;
  vector<Tile_t> & tiling = contig.getReadTiling();

  for (curread = 0; curread < tiling.size(); curread++)
  {
    if (tiling[curread].offset < newlength)
    {
      // This read should exist in the new tiling

      if (tiling[curread].getRightOffset() >= newlength)
      {
        // This read extends beyond new width
        // fix range and gaps
        int trim = tiling[curread].getRightOffset() - newlength + 1;
        int last = newlength - 1 - tiling[curread].offset;

        vector<Pos_t> & gaps = tiling[curread].gaps;
        int curgap = 0;
        int gapstodrop = 0;
        int gapstokeep = 0;

        for (curgap = 0; curgap < gaps.size(); curgap++)
        {
          if (gaps[curgap] + curgap > last) { gapstodrop++; }
          else
          {
            gaps[gapstokeep] = gaps[curgap];
            gapstokeep++;
          }
        }

        gaps.resize(gapstokeep);

        if (tiling[curread].range.isReverse())
        {
          tiling[curread].range.end += (trim - gapstodrop);
        }
        else
        {
          tiling[curread].range.end -= (trim - gapstodrop);
        }
      }

      tiling[tilecount] = tiling[curread];
      tilecount++;
    }
  }

  tiling.resize(tilecount);
}

// Trim 'righttrim' bases from a contig by trimming the last righttrim bases
// from the consensus, and then adjusting the clear range of the tiling
// reads, or by dropping them entirely from the contig.
void rightTrimContig(Contig_t & contig, int righttrim)
{
  int clen = contig.getLength();

  lengthTrimContig(contig, clen-righttrim);
}


// Slide the tiling to the right by contigShiftDistance, insert - into
// the consensus
void shiftContigRight(Contig_t & contig, int contigShiftDistance)
{
  string cons = contig.getSeqString();
  string cqual = contig.getQualString();

  cons.insert(cons.begin(), contigShiftDistance, '-');
  cqual.insert(cqual.begin(), contigShiftDistance, 'X');

  contig.setSequence(cons, cqual);

  vector<Tile_t>::iterator ti;
  for (ti =  contig.getReadTiling().begin();
       ti != contig.getReadTiling().end();
       ti++)
  {
    ti->offset += contigShiftDistance;
  }
}

// Add contigGrowDistance gaps to the consensus
void widenContig(Contig_t & contig, int contigGrowDistance)
{
  string cons = contig.getSeqString();
  string cqual = contig.getQualString();

  cons.append(contigGrowDistance, '-');
  cqual.append(contigGrowDistance, 'X');

  contig.setSequence(cons, cqual);
}






ID_t lookupIID(Bank_t & bnk, const string & eid)
{
  ID_t retval = bnk.lookupIID(eid);

  if (!retval)
  {
    cerr << "EID " << eid << " not found!" << endl;
    throw Exception_t("Invalid eid", __LINE__, __FILE__);
  }

  return retval;
}

void checkClr(vector<Tile_t>::iterator & mti, vector<Tile_t>::iterator & pti, const string & side)
{
  if (mti->range != pti->range)
  {
    cerr << "Clear range of " << side << " stitch read differs!!!" << endl;
    cerr << "master: " << mti->range.begin << "," << mti->range.end << endl;
    cerr << "patch: "  << pti->range.begin << "," << pti->range.end << endl;

    throw Exception_t("ERROR", __LINE__, __FILE__);
  }
}

void checkCons(const string & mc, const string & pc, const string & side)
{
  if (mc != pc)
  {
    cerr << side << " stitch consensus mismatch!" << endl;
    cerr << "pc: " << pc << endl;
    cerr << "mc: " << mc << endl;

    throw Exception_t("Stitch consensus mismatch", __LINE__, __FILE__);
  }

  cerr << "  " << side << " stitch consensus match (len = " << mc.length() << ")" << endl;
}


void reverseContig(AMOS::Bank_t & contig_bank,
                   AMOS::Bank_t & scaffold_bank,
                   AMOS::BankStream_t & feat_bank,
                   AMOS::ID_t contigiid)
{
  Contig_t contig;
  contig_bank.fetch(contigiid, contig);
  contig.reverseComplement();
  contig_bank.replace(contigiid, contig);

  int contiglen = contig.getLength();

  try 
  {
    // try to reverse the contig in the scaffold as well
    Scaffold_t scaffold;
    bool done = false;

    AMOS::IDMap_t::const_iterator ci;
    for (ci = scaffold_bank.getIDMap().begin();
         ci && !done;
         ci++)
    {
      scaffold_bank.fetch(ci->iid, scaffold);

      vector<Tile_t> & tiling = scaffold.getContigTiling();
      vector<Tile_t>::iterator ti;

      for (ti =  tiling.begin();
           ti != tiling.end();
           ti++)
      {
        if (ti->source == contigiid)
        {
          ti->range.swap();

          scaffold_bank.replace(scaffold.getIID(), scaffold);
          cout << "Updated scaffold i" << scaffold.getIID() << endl;
          done = true;
          break;
        }
      }
    }
  }
  catch (Exception_t & e)
  {
    cerr << "WARNING: Scaffold not updated!" << endl;
    cerr << e << endl;
  }

  try
  {
    Feature_t feat;

    cout << "Reversing features:";

    feat_bank.seekg(1);
    while (feat_bank >> feat)
    {
      ID_t bid = feat_bank.tellg() - 1;
      if ((feat.getSource().second == Contig_t::NCODE) &&
          (feat.getSource().first == contigiid))
      {
        Range_t rng = feat.getRange();
        rng.begin = contiglen - rng.begin - 1;
        rng.end   = contiglen - rng.end - 1;
        feat.setRange(rng);

        feat_bank.replaceByBID(bid, feat);
        cout << ".";
      }
    }

    cout << endl;
  }
  catch (Exception_t & e)
  {
    cerr << "WARNING: Features not updated!" << endl;
    cerr << e << endl;
  }
}


void updateStitchFeatures(BankStream_t & master_feat,
                          BankStream_t & patch_feat,
                          Range_t & stitchRegion,
                          ID_t masteriid,
                          bool masterrc,
                          ID_t master2iid,
                          bool master2rc,
                          ID_t newmasteriid,
                          ID_t patchiid,
                          const string & leftstitchread,
                          const string & rightstitchread,
                          int rightmasterbaseoffset,
                          int oldmasterlen,
                          int newmasterlen,
                          int oldmaster2len,
                          int patchleftoffset,
                          int patchrightoffset,
                          bool savetonew)
{
  // Update the features, and add a new one about the join
  cerr << "Updating features: ";
  vector<Feature_t> newfeatures;

  Feature_t feat;
  feat.setRange(stitchRegion);
  feat.setSource(make_pair(newmasteriid, Contig_t::NCODE));
  feat.setType(Feature_t::JOIN);

  char buffer[16];
  snprintf(buffer, 16, "%d", patchiid);
  feat.setComment((string) "Stitch from patch i:" + buffer + " left: " + leftstitchread + " right: " + rightstitchread);
  newfeatures.push_back(feat);

  cerr << "n";

  master_feat.seekg(1);
  while (master_feat >> feat)
  {
    ID_t bid = master_feat.tellg()-1;
    bool change = false;
    bool remove = false;

    if (feat.getSource().second == Contig_t::NCODE)
    {
      if (feat.getSource().first == masteriid)
      {
        change = true; // This ensures new iid gets set
        feat.setSource(make_pair(newmasteriid, Contig_t::NCODE));

        //cout << "master: " << feat.getRange().getLo() << "," << feat.getRange().getHi() << endl;

        if (masterrc && master2iid)
        {
          cerr << "unsupported rc master feature" << endl;
        }
        else
        {
          if (feat.getRange().getHi() < stitchRegion.begin)
          {
            // before patch, keep the coord unchanged
            cout << "m";
          }
          else if (!master2iid && (feat.getRange().getLo() > rightmasterbaseoffset))
          {
            // past patch region, adjust coords
            Range_t rng = feat.getRange();
            rng.begin = newmasterlen - (oldmasterlen - rng.begin);
            rng.end   = newmasterlen - (oldmasterlen - rng.end);
            feat.setRange(rng);

            cout << "u";
          }
          else
          {
            // in patch   or past patch region, but in master2
            remove = true;
            cout << "r";
          }
        }
      }
      else if (master2iid && feat.getSource().first == master2iid)
      {
        change = true;
        feat.setSource(make_pair(newmasteriid, Contig_t::NCODE));

        if (master2rc)
        {
          cerr << "unsupported rc master2 feature" << endl;
        }
        else
        {
          if (feat.getRange().getLo() > rightmasterbaseoffset)
          {
            // after patch region, adjust coordinates
            Range_t rng = feat.getRange();
            rng.begin = newmasterlen - (oldmaster2len - rng.begin);
            rng.end   = newmasterlen - (oldmaster2len - rng.end);
            feat.setRange(rng);
            cout << "2";
          }
          else
          {
            // before patch region, clobber feature
            remove = true;
            cout << "r";
          }
        }
      }
    }

    if (remove)
    {
      if (!savetonew)
      {
        master_feat.removeByBID(bid);
      }
    }
    else if (change)
    {
      if (savetonew)
      {
        newfeatures.push_back(feat);
      }
      else
      {
        master_feat.replaceByBID(bid, feat);
      }
    }
  }

  patch_feat.seekg(1);
  while (patch_feat >> feat)
  {
    if ((feat.getSource().second == Contig_t::NCODE) &&
        (feat.getSource().first  == patchiid))
    {
      Range_t rng = feat.getRange();

      // Check for contained features
      if ((rng.getLo() >= patchleftoffset) &&
          (rng.getHi() <= patchrightoffset))
      {
        rng.begin = stitchRegion.begin + (rng.begin - patchleftoffset);
        rng.end   = stitchRegion.begin + (rng.end   - patchleftoffset);

        feat.setRange(rng);
        feat.setSource(make_pair(newmasteriid, Contig_t::NCODE));

        newfeatures.push_back(feat);

        cout << "p";
      }
    }
  }
  cout << endl;

  for (int i = 0; i < newfeatures.size(); i++)
  {
    master_feat << newfeatures[i];
  }
}

void stitchContigs(Bank_t & master_contig, 
                   Bank_t & master_reads,
                   Bank_t & master_scaff,
                   BankStream_t & master_feat,
                   Bank_t & patch_contig,
                   Bank_t & patch_reads,
                   BankStream_t & patch_feat,
                   const string & leftstitchread,
                   const string & rightstitchread,
                   set<ID_t> & masterreads,
                   set<ID_t> & patchreads,
                   Range_t & stitchRegion,
                   ID_t masteriid,
                   ID_t master2iid,
                   ID_t patchiid,
                   bool savetonew,
                   bool perfectovl)
{
  Contig_t master, master2, patch;

  ID_t mleftiid = 0,  pleftiid = 0; 
  ID_t mrightiid = 0, prightiid = 0;

  if (!leftstitchread.empty())
  {
    mleftiid = lookupIID(master_reads, leftstitchread);
    pleftiid = lookupIID(patch_reads, leftstitchread);
  }

  if (!rightstitchread.empty())
  {
    mrightiid = lookupIID(master_reads, rightstitchread);
    prightiid = lookupIID(patch_reads, rightstitchread);
  }

  master_contig.fetch(masteriid, master);
  if (master2iid) { master_contig.fetch(master2iid, master2); }
  patch_contig.fetch(patchiid, patch);

  int oldmasterlen = master.getLength();
  int oldmaster2len = 0;
  int rightmasterbaseoffset = 0;
  int patchleftoffset = 0;
  int patchrightoffset = 0;

  // master
  string mcons = master.getSeqString();
  string mqual = master.getQualString();

  // patch
  string pcons = patch.getSeqString();
  string pqual = patch.getQualString();

  vector<Tile_t> & ptiling = patch.getReadTiling();
  sort(ptiling.begin(), ptiling.end(), TileOrderCmp());
  vector<Tile_t>::iterator pti = ptiling.begin();


  // new
  string newcons;
  string newqual;
  vector<Tile_t> newtiling;

  int masterbaseoffset = 0;
  int patchbaseoffset = 0;


  stitchRegion.begin = 0;
  stitchRegion.end   = 0;

  sort(master.getReadTiling().begin(), master.getReadTiling().end(), TileOrderCmp());
  vector<Tile_t>::iterator mti = master.getReadTiling().begin();


  if (mleftiid)
  {
    // Find the left read in the master
    vector<Tile_t> & mtiling = master.getReadTiling();

    cout << "Handling Left" << endl;

    int rightmost = 0;
    while (mti != mtiling.end())
    {
      if (mti->offset + mti->getGappedLength() > rightmost)
      {
        rightmost = mti->offset + mti->getGappedLength();
      }

      if (mti->source == mleftiid)
      {
        cout << "  Found left stitch read in master, offset:" << mti->offset << endl;
        masterbaseoffset = mti->offset;
        break;
      }

      newtiling.push_back(*mti);
      mti++;
    }

    if (mti == mtiling.end())
    {
      cout << "ERROR: Didn't find left stitch read in master!!!" << endl;
      throw Exception_t("ERROR", __LINE__, __FILE__);
    }

    cout << "  Left Consensus = mcons[0, " << masterbaseoffset - 1 << "]" << endl;

    newcons = mcons.substr(0, masterbaseoffset);
    newqual = mqual.substr(0, masterbaseoffset);

    stitchRegion.begin = masterbaseoffset;

    // Find the left read in the patch
    while (pti != ptiling.end())
    {
      if (pti->source == pleftiid)
      {
        patchleftoffset = pti->offset;
        patchbaseoffset = pti->offset;
        cout << "  Found left stitch read in patch, offset: " << pti->offset << endl;

        checkClr(mti, pti, "left");
        break;
      }

      pti++;
    }

    if (pti == ptiling.end())
    {
      cout << "Didn't find left read in patch!" << endl;
      throw Exception_t("ERROR", __LINE__, __FILE__);
    }

    if (perfectovl)
    {
      // Check that at least the consensi across the left stitch read are identical
      string pc = pcons.substr(patchbaseoffset,  rightmost-masterbaseoffset);
      string mc = mcons.substr(masterbaseoffset, rightmost-masterbaseoffset);
      checkCons(mc, pc, "Left");
    }
  }

  cout << endl;
  cout << "Handling Patch Region" << endl;

  // pti points at first read to patch
  // stitch the reads into the master

  int porigoffset = 0;
  int rightmost = 0;
  int lentocheck = 0;
  while (pti != ptiling.end())
  {
    if (pti->offset + pti->getGappedLength() > rightmost)
    {
      rightmost = pti->offset + pti->getGappedLength();
    }

    porigoffset = pti->offset;
    pti->offset = pti->offset - patchbaseoffset + masterbaseoffset;
    pti->source = lookupIID(master_reads, patch_reads.lookupEID(pti->source));
    newtiling.push_back(*pti);

    if (pti->source == mrightiid) // right read was converted, so use mrightiid
    {
      cout << "  Found right stitch read in patch, offset: " << porigoffset << endl;

      if (rightmost > porigoffset + pti->getGappedLength())
      {
        cout << "  WARNING: Read in patch extends beyond right stitch read" << endl;
      }

      lentocheck = rightmost - porigoffset;
      rightmost = porigoffset + pti->getGappedLength();
      patchrightoffset = rightmost;

      break;
    }

    patchreads.insert(lookupIID(master_reads, patch_reads.lookupEID(pti->source)));
    pti++;
  }

  if (prightiid && (pti == ptiling.end()))
  {
    cout << "Didn't find right read in patch" << endl;
    throw Exception_t("ERROR", __LINE__, __FILE__);
  }

  int len = rightmost - patchbaseoffset;
  cout << "  Patch Consensus[" << newcons.length() 
       << "].append pcons[" << patchbaseoffset << "," << len << "]" << endl;

  newcons.append(pcons, patchbaseoffset, len);
  newqual.append(pqual, patchbaseoffset, len);

  stitchRegion.end = newcons.size() - 1;


  if (prightiid)
  {
    cout << endl;
    cout << "Handling Right" << endl;

    // handle right flank from master
    bool stitchreads = false;
    patchbaseoffset = pti->offset; // pti must exist

    vector<Tile_t> & mtiling = (master2iid) ? master2.getReadTiling() : master.getReadTiling();

    if (master2iid)
    {
      oldmaster2len = master2.getLength();
      while (mti != master.getReadTiling().end())
      {
        masterreads.insert(mti->source);
        mti++;
      }

      sort(mtiling.begin(), mtiling.end(), TileOrderCmp());
      mti = mtiling.begin();

      mcons = master2.getSeqString();
      mqual = master2.getQualString();
    }

    while (mti != mtiling.end())
    {
      if (stitchreads)
      {
        mti->offset = mti->offset - masterbaseoffset + patchbaseoffset;
        newtiling.push_back(*mti);
      }

      // mrightiid is the last read from patch, don't stitch it
      if (mti->source == mrightiid)
      {
        stitchreads = true;
        masterbaseoffset = mti->offset;

        int roffset = mti->offset + mti->getGappedLength();
        cout << "  Found right stitch read in master, offset: " << mti->offset << endl;

        rightmasterbaseoffset = roffset;

        checkClr(mti, pti, "right");

        if (perfectovl)
        {
          // Ensure the consensi across the right stitch read are identical
          string mc = mcons.substr(masterbaseoffset, lentocheck);
          string pc = pcons.substr(porigoffset, lentocheck);
          checkCons(mc, pc, "Right");
        }

        len = mcons.length() - roffset;
        cout << "  Right Consensus [" << newcons.length() << "].append mcons[" << roffset << "," << len << "]" << endl;
        newcons.append(mcons, roffset, len);
        newqual.append(mcons, roffset, len);
      }

      if (!stitchreads)
      {
        masterreads.insert(mti->source);
      }

      mti++;
    }

    if (!stitchreads)
    {
      cerr << "ERROR: Didn't find right stitch read in master" << endl;
      throw Exception_t("ERROR", __LINE__, __FILE__);
    }
  }

  int newmasterlen = newcons.length();

  master.setReadTiling(newtiling);
  master.setSequence(newcons, newqual);


  if (savetonew)
  {
    master.setIID(master_contig.getMaxIID()+1);
    master.setEID(master.getEID() + "_stitched");
    master_contig.append(master);

    cout << "Saved new contig i:" << master.getIID() << " e:" << master.getEID() << endl;
  }
  else
  {
    master_contig.replace(master.getIID(), master);
    cout << "Updated contig i:" << master.getIID() << " e:" << master.getEID() << endl;

    if (master2iid) 
    { 
      cout << "Removed contig i:" << master2iid << " e:" << master_contig.lookupEID(master2iid) << endl;
      master_contig.remove(master2iid); 
    }
  }

  bool masterrc = false;
  bool master2rc = false;

  if (oldmasterlen != newmasterlen || master2iid)
  {
    Scaffold_t masterscaff;
    Scaffold_t master2scaff;

    bool foundmaster = false;
    bool foundmaster2 = false;

    cout << endl;
    cout << "Updating scaffold" << endl;

    vector<Tile_t>::iterator ci;

    const IDMap_t & scaffmap = master_scaff.getIDMap();
    IDMap_t::const_iterator si;
    for (si = scaffmap.begin(); si != scaffmap.end(); si++)
    {
      Scaffold_t scaff;
      master_scaff.fetch(si->iid, scaff);

      for (ci = scaff.getContigTiling().begin();
           ci != scaff.getContigTiling().end();
           ci++)
      {
        if (ci->source == masteriid)
        {
          masterscaff = scaff;
          foundmaster = true;
          cout << "  Found masterscaffold i:" << masterscaff.getIID() << endl;
        }
        else if (ci->source == master2iid)
        {
          master2scaff = scaff;
          foundmaster2 = true;
          cout << "  Found master2scaffold i:" << master2scaff.getIID() << endl;
        }
      }

      if (foundmaster && (!master2iid || foundmaster2))
      {
        break;
      }
    }

    if (foundmaster)
    {
      cout << "  Updating scaffold information" << endl;
      vector<Tile_t> newcontigs;

      sort(masterscaff.getContigTiling().begin(), masterscaff.getContigTiling().end(), TileOrderCmp());

      // masterscaff must contain masteriid

      for (ci =  masterscaff.getContigTiling().begin();
           ci != masterscaff.getContigTiling().end();
           ci++)
      {
        if (ci->source == masteriid)
        {
          break;
        }

        newcontigs.push_back(*ci);
      }

      int offsetadjust = newmasterlen - oldmasterlen;

      if (ci->range.isReverse())
      {
        masterrc = true;
        ci->range.setBegin(ci->range.getBegin() + offsetadjust);
      }
      else
      {
        masterrc = false;
        ci->range.setEnd(ci->range.getEnd() + offsetadjust);
      }

      ci->source = master.getIID();
      int newmasterright = ci->getRightOffset();

      cout << "  Found mastercontig, adjusting downstream offsets by " << offsetadjust << endl;

      newcontigs.push_back(*ci);
      ci++;

      if (master2iid && foundmaster2)
      {
        vector<Tile_t> & ctiling = (master2scaff.getIID() == masterscaff.getIID()) ? masterscaff.getContigTiling() : master2scaff.getContigTiling();

        if (master2scaff.getIID() != masterscaff.getIID())
        {
          sort(ctiling.begin(), ctiling.end(), TileOrderCmp());
          ci = ctiling.begin();
        }

        bool copycontigs = false;
        while (ci != ctiling.end())
        {
          if (copycontigs)
          {
            ci->offset += offsetadjust;
            newcontigs.push_back(*ci);
          }

          if (ci->source == master2iid)
          {
            offsetadjust = newmasterright - ci->getRightOffset();
            copycontigs = true;
            master2rc = ci->range.isReverse();
          }

          ci++;
        }
      }
      else
      {
        // Just adjust the downstream offsets
        while (ci != masterscaff.getContigTiling().end())
        {
          ci->offset += offsetadjust;
          newcontigs.push_back(*ci);
          ci++;
        }
      }

      masterscaff.setContigTiling(newcontigs);

      if (savetonew)
      {
        masterscaff.setIID(master_scaff.getMaxIID()+1);
        masterscaff.setEID(masterscaff.getEID() + "_stitched");
        master_scaff.append(masterscaff);

        cout << "Saved new scaffold i:" << masterscaff.getIID() << " e:" << masterscaff.getEID() << endl;
      }
      else
      {
        master_scaff.replace(masterscaff.getIID(), masterscaff);
        cout << "Updated scaffold i:" << masterscaff.getIID() << " e:" << masterscaff.getEID() << endl;

        if (master2iid && master2scaff.getIID() && (masterscaff.getIID() != master2scaff.getIID())) 
        { 
          cout << "Removed scaffold i:" << master2scaff.getIID() << " e:" << master2scaff.getEID() << endl;
          master_scaff.remove(master2scaff.getIID()); 
        }
      }
    }
    else if (foundmaster2)
    {
      cerr << "Error master not in scaffold, but master2 is" << endl;
      exit(EXIT_FAILURE);
    }
  }

  updateStitchFeatures(master_feat,
                       patch_feat,
                       stitchRegion,
                       masteriid,
                       masterrc,
                       master2iid,
                       master2rc,
                       master.getIID(),
                       patchiid,
                       leftstitchread,
                       rightstitchread,
                       rightmasterbaseoffset,
                       oldmasterlen,
                       newmasterlen,
                       oldmaster2len,
                       patchleftoffset,
                       patchrightoffset,
                       savetonew);
}
