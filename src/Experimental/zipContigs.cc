#include "foundation_AMOS.hh"
#include "amp.hh"
#include "fasta.hh"
#include "AMOS_Foundation.hh"

using namespace AMOS;
using namespace std;

int m_alignStart(0);
string m_queryGaps("");
string m_referenceGaps("");
int m_promoteAlignmentGaps(1);
int m_reverseQuery(0);
int m_ungappedCoordinates(0);
int m_gapCoordinateOffset(0);
int m_referenceConsensus(0);


typedef map<int, int> GapMap;
GapMap m_referenceGapMap;
GapMap m_queryGapMap;

int getGapCount(bool isReference, int gpos, int upos)
{
  int retval = 0;
  GapMap::iterator i;
  
  int location = (m_ungappedCoordinates) ? upos : gpos;

  if (isReference)
  {
    i = m_referenceGapMap.find(location);

    if (i != m_referenceGapMap.end())
    {
      retval = i->second;
      i->second = 0;
    }
  }
  else
  {
    i = m_queryGapMap.find(location);

    if (i != m_queryGapMap.end())
    {
      retval = i->second;
      i->second = 0;
    }
  }

  return retval;
}

void addGap(int gappos, GapMap & gapmap)
{
  gappos -= m_gapCoordinateOffset;
  gappos++;

  cerr << "Adding " << gappos << " (0-based) to gapmap" << endl;

  GapMap::iterator gi = gapmap.find(gappos);

  if (gi != gapmap.end())
  {
    gi->second++;
  }
  else
  {
    gapmap[gappos] = 1;
  }
}


void loadGaps(const string & gaplist, GapMap & gapmap)
{
  // Split the gaplist on ',', load each into the gapmap

  std::string::size_type index = 0;
  std::string::size_type splitIndex;
  int gappos;

  while ((splitIndex = gaplist.find(',', index)) != gaplist.npos)
  {
    gappos = atoi(gaplist.substr(index, splitIndex - index).c_str());
    addGap(gappos, gapmap);
    index = splitIndex + 1;
  }

  gappos = atoi(gaplist.substr(index, 
                               gaplist.length() - index + 1).c_str());
  addGap(gappos, gapmap);
}







int main (int argc, char ** argv)
{

  int retval = 0;
  AMOS_Foundation * tf = NULL;

  try
  {
    string version =  "Version 1.0";
    string dependencies = "";
    string helptext = 
"Zip together a query contig onto a reference contig. The beginning\n"
"of the alignment is specified with -O, and gaps can be inserted into both\n"
"contigs files using -r and -q.\n"
"\n"
"   Usage: zipContigs [options] bnk qryeid refeid joineid\n"
"\n"
"   bnk     Name of bank to find contigs\n"
"   qryeid  Eid of query (typically smaller than reference)\n"
"   refeid  Eid of reference contig\n"
"   joineid Eid to assign to zipped contig\n"
"\n"
"   Options\n"
"   ----------------\n"
"   -R Query should be reversed before merging\n"
"   -1 Coordinates of gaps are 1-based\n"
"   -U Coordinates of gaps to insert are ungapped\n"
"   -p Automatically promote preexisting gaps\n"
"   --refcons Use the consensus of the reference for merged slices\n"
"             (Uses the query consensus by default)\n"
"\n"
"   -q <csl> Comma separated list of positions to insert gaps in the query\n"
"   -r <csl> Comma separated list of positions to insert gaps in the reference\n"
"\n"
"   -O <offset> Specify alignment offset from query to reference\n"
"               (negative means reference left flanks query (always 0-based))\n";

    // Instantiate a new TIGR_Foundation object
    tf = new AMOS_Foundation (version, helptext, dependencies, argc, argv);
    tf->disableOptionHelp();
    tf->getOptions()->addOptionResult("O|alignmentoffset=i", &m_alignStart,
                                      "Specify alignment offset from query to reference\n           negative means reference left flanks query (always 0-based)");

    tf->getOptions()->addOptionResult("q|querygaps=s", &m_queryGaps,
                                      "Specify gaps in query slices");

    tf->getOptions()->addOptionResult("r|referencegaps=s", &m_referenceGaps,
                                      "Specify gaps in reference slices");

    tf->getOptions()->addOptionResult("p|promoteAlignmentGaps!", &m_promoteAlignmentGaps,
                                      "Toggle if alignment gaps should automatically be promoted");

    tf->getOptions()->addOptionResult("R|reversequery!", &m_reverseQuery,
                                      "Query should be reversed before merging");

    tf->getOptions()->addOptionResult("U|ungapped!", &m_ungappedCoordinates,
                                      "Coordinates are all ungapped");

    tf->getOptions()->addOptionResult("1", &m_gapCoordinateOffset,
                                      "Coordinates of gaps are 1-based");

    tf->getOptions()->addOptionResult("refcons", &m_referenceConsensus,
                                      "Use the consensus of the reference for the merged slices");

    tf->handleStandardOptions();

    list<string> argvv = tf->getOptions()->getAllOtherData();

    if (argvv.size() != 4)
    {
      cerr << "Usage: zipcontigs [options] bankname refeid qryeid joineid" << endl;
      return EXIT_FAILURE;
    }

    string bankname = argvv.front(); argvv.pop_front();
    string qryeid = argvv.front(); argvv.pop_front();
    string refeid = argvv.front(); argvv.pop_front();
    string jeid   = argvv.front(); argvv.pop_front();

    Bank_t contig_bank(Contig_t::NCODE);

    cerr << "Processing " << bankname << endl;

    contig_bank.open(bankname, B_READ|B_WRITE);
    ID_t jiid     = contig_bank.getMaxIID()+1;

    Contig_t rcontig, qcontig;

    contig_bank.fetch(refeid, rcontig);
    contig_bank.fetch(qryeid, qcontig);

    cerr << "Ref: " << refeid << " len: " << rcontig.getLength() << endl;
    cerr << "Qry: " << qryeid << " len: " << qcontig.getLength() << endl;

    cerr << " m_alignStart(0)=" <<           m_alignStart << endl;
    cerr << " m_queryGaps(\"\")=" <<         m_queryGaps<< endl;
    cerr << " m_referenceGaps(\"\")=" <<     m_referenceGaps<< endl;
    cerr << " m_promoteAlignmentGaps(0)=" << m_promoteAlignmentGaps<< endl;
    cerr << " m_reverseQuery(0)=" <<         m_reverseQuery<< endl;
    cerr << " m_ungappedCoordinates(0)=" <<  m_ungappedCoordinates<< endl;
    cerr << " m_gapCoordinateOffset(0)=" <<  m_gapCoordinateOffset<< endl;
    cerr << " m_referenceConsensus(0)=" <<   m_referenceConsensus<< endl;

    if (!m_queryGaps.empty())
    {
      cerr << "Loading Query Gaps" 
           << (m_reverseQuery ? "(reversed)" : "") << endl;
      loadGaps(m_queryGaps, m_queryGapMap);
    }

    if (!m_referenceGaps.empty())
    {
      cerr << "Loading Reference Gaps" << endl;
      loadGaps(m_referenceGaps, m_referenceGapMap);
    }


    if (m_reverseQuery)
    {
      cerr << "Reversing query\n";
      qcontig.reverseComplement();
    }

    string rcons(rcontig.getSeqString());
    string qcons(qcontig.getSeqString());

    Pos_t rgindex = 0;
    Pos_t qgindex = 0;

    if (m_alignStart < 0) 
    { 
      if (m_ungappedCoordinates) 
      { 
        m_alignStart = -rcontig.ungap2gap(-m_alignStart); 
      }

      rgindex = -m_alignStart; 
    }
    else if (m_alignStart > 0)
    { 
      if (m_ungappedCoordinates) 
      { 
        m_alignStart = qcontig.ungap2gap(m_alignStart); 
      }

      qgindex = m_alignStart; 
    }

    vector <Pos_t> rinsert;
    vector <Pos_t> qinsert;

    int rlen = rcons.length();
    int qlen = qcons.length();

   
    while (rgindex < rlen && qgindex < qlen) 
    {
      int ruindex = rcontig.gap2ungap(rgindex);
      int quindex = qcontig.gap2ungap(qgindex);

      int rgaps = getGapCount(true, rgindex, ruindex);
      int qgaps = getGapCount(false, qgindex, quindex);
      
      char r = rcons[rgindex];
      char q = qcons[qgindex];

      if (rgaps && qgaps)
      {
        cerr << "Error: gap specified at same location r:" << rgindex
             << " q: " << qgindex << endl;

        throw "err";
      }

      if (rgaps || qgaps)
      {
        while (rgaps)
        {
          if (m_promoteAlignmentGaps && q == '-' && r == '-')
          {
            // Let gaps zip together, doesn't help rgaps
            rgindex++;
            qgindex++;
            cerr << "-";
          }
          else if (m_promoteAlignmentGaps && q == '-' && r != '-')
          {
            // extra gap to promote
            qgindex++;
            cerr << 'r';
            rinsert.push_back(rgindex);
          }
          else if (m_promoteAlignmentGaps && r == '-' && q != '-')
          {
            // count this preexisting gap towards rgaps
            rgindex++;
            qgindex++;
            rgaps--;
          }
          else
          {
            // ordinary gap to insert
            cerr << 'R';
            qgindex++;
            rgaps--;
            rinsert.push_back(rgindex);
          }

          q = qcons[qgindex];
          r = rcons[rgindex];
        }

        while (qgaps)
        {
          if (m_promoteAlignmentGaps && q == '-' && r == '-')
          {
            // Let gaps zip together, doesn't help rgaps
            rgindex++;
            qgindex++;
            cerr << "-";
          }
          else if (m_promoteAlignmentGaps && r == '-' && q != '-')
          {
            // extra gap to promote
            rgindex++;
            cerr << 'q';
            qinsert.push_back(qgindex);
          }
          else if (m_promoteAlignmentGaps && q == '-' && r != '-')
          {
            // count this preexisting gap towards qgaps
            rgindex++;
            qgindex++;
            qgaps--;
          }
          else
          {
            // ordinary gap to insert
            cerr << 'Q';
            rgindex++;
            qgaps--;
            qinsert.push_back(qgindex);
          }

          q = qcons[qgindex];
          r = rcons[rgindex];
        }
      }
      else if (r == '-' && q == '-')
      {
        // Let gaps zip together
        rgindex++;
        qgindex++;
        cerr << "-";
      }
      else if (m_promoteAlignmentGaps && r == '-')
      {
        qinsert.push_back(qgindex);
        rgindex++;
        cerr << "q";
      }
      else if (m_promoteAlignmentGaps && q == '-')
      {
        rinsert.push_back(rgindex);
        qgindex++;
        cerr << "r";
      }
      else
      {
        rgindex++;
        qgindex++;
        cerr << ".";
      }
    }

    cerr << endl;


    // Apply Gaps

    cerr << "Applying " << rinsert.size() << " reference gaps:";
    vector <Pos_t>::reverse_iterator gi;
    for (gi = rinsert.rbegin(); gi != rinsert.rend(); gi++)
    {
      cerr << " " << *gi;
      rcontig.insertGapColumn(*gi);
    }
    cerr << endl;

    cerr << "Applying " << qinsert.size() << " query gaps:";
    for (gi = qinsert.rbegin(); gi != qinsert.rend(); gi++)
    {
      cerr << " " << *gi;
      qcontig.insertGapColumn(*gi);
    }
    cerr << endl;

    // Merge Tilings into rcontig
    vector<Tile_t> & rtiling = rcontig.getReadTiling();
    vector<Tile_t> & qtiling = qcontig.getReadTiling();
    vector<Tile_t>::iterator ti;

    if (m_alignStart > 0)
    {
      cerr << "Shifting reference reads by " << m_alignStart << endl;
      for (ti = rtiling.begin(); ti != rtiling.end(); ti++)
      {
        ti->offset += m_alignStart;
      }
    }

    cerr << "Merging " << qtiling.size() << " query reads" << endl;

    // shift query reads if reference left flanks
    int shift = 0;
    if (m_alignStart < 0) 
    { 
      shift = -m_alignStart; 
      cerr << "And shift query by " << shift << endl;
    }

    for (ti =  qtiling.begin(); ti != qtiling.end(); ti++)
    {
      ti->offset += shift;
      rtiling.push_back(*ti);
    }

    cerr << "Concatenating consensus: ";

    // Concatenate new consensus
           rcons  = rcontig.getSeqString();
    string rqual  = rcontig.getQualString();
           rlen   = rcons.length();
    int    rstart = 0;
    int    rend   = rlen-1;

           qcons  = qcontig.getSeqString();
    string qqual  = qcontig.getQualString();
           qlen   = qcons.length();
    int    qstart = 0;
    int    qend   = qlen-1;

    string cons;
    string qual;

    int alignstart = abs(m_alignStart);
    int alignend   = 0;
    int aligndist  = 0;


    // left flank

    if (m_alignStart > 0)
    {
      // query left flank
      cerr << "1Q(" << alignstart << ")";
      cons = qcons.substr(0, alignstart);
      qual = qqual.substr(0, alignstart);

      qstart = alignstart;
      rend += alignstart;
    }
    else if (m_alignStart < 0)
    {
      //reference left flank
      cerr << "1R(" << alignstart << ")";
      cons = rcons.substr(0, alignstart);
      qual = rqual.substr(0, alignstart);

      //cerr << "\"" << rcons.substr(0, alignstart) << "\"";

      rstart = alignstart;
      qend += alignstart;
    }

    // overlap

    // {r,q}start is relative to the sequence
    // alignstart, alignend, {r,q}end are global coordinates
    alignend = (rend < qend) ? rend : qend;

    // cerr << endl;
    // cerr << "alignstart: " << alignstart << "alignend: " << alignend << endl;
    // cerr << "rstart: " << rstart << " rend: " << rend << " rlen: " << rlen << endl;
    // cerr << "qstart: " << qstart << " qend: " << qend << " qlen: " << qlen << endl;

    aligndist = alignend-alignstart+1;

    if (aligndist <= 0)
    {
      AMOS_THROW_ARGUMENT("No overlap between Contigs!");
    }

    if (m_referenceConsensus)
    {
      cerr << "2R(" << aligndist << ")";

      cons += rcons.substr(rstart, aligndist);
      qual += rqual.substr(rstart, aligndist);
    }
    else
    {
      cerr << "2Q(" << aligndist << ")";

      cons += qcons.substr(qstart, aligndist);
      qual += qqual.substr(qstart, aligndist);

      //cerr << "\"" << qcons.substr(qstart, aligndist) << "\"";
    }


    // right flank

    if (rend > alignend)
    {
      //reference right flank
      int dist = rend - alignend;
      cerr << "3R(" << dist << ")";

      cons += rcons.substr(rlen - dist, dist);
      qual += rqual.substr(rlen - dist, dist);
    }
    else if (qend > alignend)
    {
      // query right flank
      int dist = qend - alignend;
      cerr << "3Q(" << dist << ")";

      //cerr << "\"" << qcons.substr(qlen-dist, dist) << "\"";

      cons += qcons.substr(qlen - dist, dist);
      qual += qqual.substr(qlen - dist, dist);
    }

    cerr << endl;

    rcontig.setSequence(cons, qual);

    // save to bank, rcontig gets new name
    rcontig.setIID(jiid);
    rcontig.setEID(jeid);

    cerr << "Appending iid: " << jiid << " eid: " << jeid << endl;
    contig_bank.append(rcontig);
    contig_bank.close();
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
