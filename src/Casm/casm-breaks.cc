////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Adam M Phillippy
//! \date 06/08/2005
//!
//! \brief Layout the qry seqs and report alternate paths through the ref
//!
////////////////////////////////////////////////////////////////////////////////

#include "foundation_AMOS.hh"
#include "delta.hh"
#include "delta.cc"
#include "amp.hh"
#include <climits>
#include <list>
#include <sstream>
#include <fstream>
using namespace std;
using namespace AMOS;
using namespace HASHMAP;


//=============================================================== Options ====//
string   OPT_AlignName;                  // alignment name parameter
string   OPT_BankName;                   // bank name parameter

string   OPT_FeaturesName;               // features output file
string   OPT_PathsName;                  // paths output file
int      OPT_Fuzzy          = 3;         // fuzzy equals tolerance
int      OPT_Seed           = -1;        // random seed
int      OPT_Redundancy     = 3;         // needed concurring signatures
float    OPT_MinIdentity    = 0.0;       // min identity to tile
int      OPT_MaxTrim        = 500;       // max ignorable trim length

const long int MAXIMAL      = LONG_MAX;  // maximum integer


//============================================================= Constants ====//
struct Signature_t
//!< mis-alignment signature
{
  long int fc;                        // foward support
  long int rc;                        // reverse support
  list<const DeltaNode_t *> qry;      // supporting queries

  vector<const DeltaNode_t *> ref;    // reference ids
  vector<long int> pos;               // reference alignment positions
  vector<long int> gap;               // query alignment gaps/overlaps

  // [sR]---[eR]   [sR]---[eR]   [sR]---[eR]
  //   [nodeR]       [nodeR]       [nodeR]
  //           [gap]         [gap]

  Signature_t() { fc = rc = 0; }
};


struct Placement_t
//!< placement of a read on the reference
{
  ID_t iid;                           // iid of the read
  const DeltaNode_t * read;           // read alignment info
  const DeltaEdgelet_t * chain;       // head of the alignment chain
  Placement_t * mate;                 // mated read's placement

  Placement_t() { iid = NULL_ID; read = NULL; chain = NULL; mate = NULL; }
};


struct Mapping_t
//!< placements of reads on the reference
{
  vector<Placement_t *> reads;
  list<Signature_t> sigs;

  Mapping_t() { }
  ~Mapping_t()
  {
    vector<Placement_t *>::iterator ppi;
    for ( ppi = reads.begin(); ppi != reads.end(); ++ ppi )
      delete (*ppi);
  }
};


struct EdgeletCmp_t
//!< Compares query lo coord
{
  bool operator() (const DeltaEdgelet_t * a, const DeltaEdgelet_t * b) const
  { return ( a->loQ < b->loQ ); }
};


//========================================================== Fuction Decs ====//

//------------------------------------------------------- CleanSignatures ----//
void CleanSignatures (Mapping_t & mapping);


//----------------------------------------------------- CombineSignatures ----//
//! \brief Combine like signatures
//!
//! \pre sigs vector is populated
//! \post Like signatures have been collapsed
//! \param mapping The mapping containing the list of signatures
//! \return void
//!
void CombineSignatures (Mapping_t & mapping);


//----------------------------------------------------- CompareSignatures ----//
//! \brief Compares two signatures for equality
//!
//! Returns 1 if signatures match in foward direction, -1 if match in reverse,
//! or 0 if they don't match.
//!
//! \param a First signature
//! \param b Second signature
//! \return 1,-1 for match, 0 for no match
//!
int CompareSignatures (const Signature_t & a, const Signature_t & b);


//--------------------------------------------------------- GetAlignments ----//
//! \brief Read in the alignment info
//!
//! \param graph The empty alignment graph
//! \return void
//!
void GetAlignments (DeltaGraph_t & graph);


//-------------------------------------------------------------- GetReads ----//
//! \brief Read in the read and mate info, if any
//!
//! \param graph The filled alignment graph
//! \param mapping The empty read mapping
//! \return void
//!
void GetReads (const DeltaGraph_t & graph, Mapping_t & mapping);


//-------------------------------------------------------- OutputFeatures ----//
void OutputFeatures (const Mapping_t & mapping);


//----------------------------------------------------------- OutputPaths ----//
void OutputPaths (const Mapping_t & mapping);


//----------------------------------------------------------- PlaceUnique ----//
//! \brief Place reads with an unambiguous mapping to the reference
//!
//! \param mapping The read mapping
//! \return void
//!
void PlaceUnique (Mapping_t & mapping);


//---------------------------------------------------------- PlaceRepeats ----//
//! \brief Place reads with an ambiguous mapping to the reference
//!
//! \param mapping The read mapping
//! \return void
//!
void PlaceRepeats (Mapping_t & mapping);


//------------------------------------------------------ RecordSignatures ----//
//! \brief Reads the alignment graph and records the alignment signatures
//!
//! \param graph Alignment graph
//! \param mapping The mapping containing the empty signature list
//! \pre Alignment graph has been filtered and cleaned
//! \post Populates the sigs list, one per aligned sequence
//! \return void
//!
void RecordSignatures (const DeltaGraph_t & graph, Mapping_t & mapping);


//------------------------------------------------------------- ParseArgs ----//
//! \brief Sets the global OPT_% values from the command line arguments
//!
//! \param argc
//! \param argv
//! \return void
//!
void ParseArgs (int argc, char ** argv);


//------------------------------------------------------------- PrintHelp ----//
//! \brief Prints help information to cout
//!
//! \param s The program name
//! \return void
//!
void PrintHelp (const char * s);


//------------------------------------------------------------ PrintUsage ----//
//! \brief Prints usage information to cout
//!
//! \param s The program name
//! \return void
//!
void PrintUsage (const char * s);


//========================================================== Inline Funcs ====//

//========================================================= Function Defs ====//
int main (int argc, char ** argv)
{
  DeltaGraph_t graph;
  Mapping_t mapping;

  //-- COMMAND
  ParseArgs (argc, argv);          // parse the command line arguments
  srand (OPT_Seed);

  //-- GET/CLEAN ALIGNMENTS
  cerr << "Building alignment graph\n";
  GetAlignments (graph);

  //-- GET READS
  cerr << "Fetching read and mate information\n";
  GetReads (graph, mapping);

  //-- PLACE UNAMBIGUOUS READS
  cerr << "Placing unique reads\n";
  PlaceUnique (mapping);

  //-- PLACE READS IN REPEATS
  cerr << "Placing repetitive reads\n";
  PlaceRepeats (mapping);


  //-- GENERATE SIGNATURES
  RecordSignatures (graph, mapping);

  //-- COMBINE SIGNATURES
  CombineSignatures (mapping);

  CleanSignatures (mapping);

  //-- OUTPUT
  cerr << "Outputting read paths\n";
  if ( ! OPT_FeaturesName.empty() )
    OutputFeatures (mapping);

  if ( ! OPT_PathsName.empty() )
    OutputPaths (mapping);


  return EXIT_SUCCESS;
}


//------------------------------------------------------- CleanSignatures ----//
void CleanSignatures (Mapping_t & mapping)
{
  list<Signature_t>::iterator i (mapping.sigs.begin());

  while ( i != mapping.sigs.end() )
    {
      //-- Remove signatures with insufficient support
      if ( i->qry.size() < OPT_Redundancy )
        i = mapping.sigs.erase (i);
      else
        ++ i;
    }
}


//----------------------------------------------------- CombineSignatures ----//
void CombineSignatures (Mapping_t & mapping)
{
  int cmp;
  list<Signature_t>::iterator i, j;

  //-- Walk through the list and combine everything that's equal
  for ( i = mapping.sigs.begin(); i != mapping.sigs.end(); ++ i )
    {
      j = i; ++ j;
      while ( j != mapping.sigs.end() )
        {
          cmp = CompareSignatures (*i, *j);

          if ( cmp )
            {
              if ( cmp > 0 ) // forward match
                {
                  i->fc += j->fc;
                  i->rc += j->rc;
                }
              else // reverse match
                {
                  i->fc += j->rc;
                  i->rc += j->fc;
                }

              i->qry.splice (i->qry.end(), j->qry);
              j = mapping.sigs.erase (j);
            }
          else
            ++ j;
        }
    }
}


//----------------------------------------------------- CompareSignatures ----//
int CompareSignatures (const Signature_t & a, const Signature_t & b)
{
  //-- ** BEWARE ** of overflow for MAXIMAL values, use subtraction only!
  int match = 1;
  vector<long int>::const_iterator i, j;
  vector<long int>::const_reverse_iterator k;

  //-- Check the forward pos vector
  for ( i  = a.pos.begin(), j  = b.pos.begin();
        i != a.pos.end() && j != b.pos.end(); ++ i, ++ j )
    if ( labs(*i) - OPT_Fuzzy > labs(*j)
         ||
         labs(*j) - OPT_Fuzzy > labs(*i) )
      break;

  //-- If not the same forward positions, try reverse
  if ( i != a.pos.end() || j != b.pos.end() )
    {
      for ( i  = a.pos.begin(), k  = b.pos.rbegin();
            i != a.pos.end() && k != b.pos.rend(); ++ i, ++ k )
        if ( labs(*i) - OPT_Fuzzy > labs(*k)
             ||
             labs(*k) - OPT_Fuzzy > labs(*i) )
          break;
      
      //-- Different or same reverse positions?
      match = ( i != a.pos.end() || k != b.pos.rend() ) ? 0 : -1;
    }

  //-- 'match' is now either 1 for forward, -1 for reverse or 0 for no match

  //-- Check the gap vector
  if ( match == 1 )
    {
      for ( i  = a.gap.begin(), j  = b.gap.begin();
            i != a.gap.end() && j != b.gap.end(); ++ i, ++ j )
        if ( labs(*i) - OPT_Fuzzy > labs(*j)
             ||
             labs(*j) - OPT_Fuzzy > labs(*i) )
          break;

      //-- If not the same forward gaps
      if ( i != a.gap.end() || j != b.gap.end() )
        match = 0;
    }
  else if ( match == -1 )
    {
      for ( i  = a.gap.begin(), k  = b.gap.rbegin();
            i != a.gap.end() && k != b.gap.rend(); ++ i, ++ k )
        if ( labs(*i) - OPT_Fuzzy > labs(*k)
             ||
             labs(*k) - OPT_Fuzzy > labs(*i) )
          break;

      //-- If not the same reverse gaps
      if ( i != a.gap.end() || k != b.gap.rend() )
        match = 0;
    }


  //-- If not the same references
  if ( match != 0  &&  a.ref != b.ref )
    match = 0;
  
  return match;
}



//--------------------------------------------------------- GetAlignments ----//
void GetAlignments (DeltaGraph_t & graph)
{
  //-- Short and sweet thanks to delta.hh
  cerr << "  build\n";
  graph.build (OPT_AlignName, false);
  cerr << "  flag score\n";
  graph.flagScore (0, OPT_MinIdentity);
  cerr << "  flag QLIS\n";
  graph.flagQLIS();
  cerr << "  clean\n";
  graph.clean();
}


//-------------------------------------------------------------- GetReads ----//
void GetReads (const DeltaGraph_t & graph, Mapping_t & mapping)
{
  //-- If a bank exists, try and populate the read/mate info from it
  if ( ! OPT_BankName.empty() )
    {
      Index_t mtpx, libx;
      Bank_t red_bank (Read_t::NCODE);

      //-- Mate info
      cerr << "  getting mate info from bank\n";
      try {
        mtpx.buildReadMate (OPT_BankName);
        libx.buildReadLibrary (OPT_BankName);
      } catch (const Exception_t & e) {
        cerr << "WARNING: " << e.what() << endl
             << "  could not collect mate-pair information from bank\n";
      }

      //-- Read info
      cerr << "  getting read info from bank\n";
      try {
        red_bank.open (OPT_BankName, B_READ);

        //-- Check if alignment file uses IIDs or EIDs
        bool isIID = true;
        if ( ! graph.qrynodes.empty() )
          {
            ID_t tempid;
            string tempstr (*(graph.qrynodes.begin()->second.id));
            istringstream ss (tempstr);
            ss >> tempid;
            if ( ! ss || ! red_bank.existsIID (tempid) )
              {
                if ( red_bank.existsEID (tempstr) )
                  isIID = false;
                else
                  AMOS_THROW ("Query IDs do not match the read bank");
              }
          }
        if ( isIID )
          cerr << "  looks like alignment uses IIDs\n";
        else
          cerr << "  looks like alignment uses EIDs\n";

        //-- Iterating through the IDMap will be quicker than streaming
        ID_t r1, r2;
        string s1, s2;
        Placement_t * p1, * p2;
        ostringstream ss;
        Index_t::iterator ii;
        map<string, DeltaNode_t>::const_iterator mi;

        IDMap_t::const_iterator idmi;
        const IDMap_t & idmap = red_bank.getIDMap();
        IDMap_t seen (idmap.getBuckets());
        for ( idmi = idmap.begin(); idmi != idmap.end(); ++ idmi )
          {
            if ( idmi->iid == NULL_ID )
              AMOS_THROW ("All read objects must have an IID");

            //-- If we've seen it already, don't do it again
            if ( seen.exists (idmi->iid) )
              continue;

            r1 = idmi->iid;
            r2 = mtpx.lookup (r1);

            seen.insert (r1);
            seen.insert (r2);

            //-- Translate from IID to alignment ID
            if ( isIID )
              {
                ss.str("");
                ss << r1;
                s1 = ss.str();
                ss.str("");
                ss << r2;
                s2 = ss.str();
              }
            else
              {
                s1 = idmi->eid;
                s2 = idmap.lookupEID (r2);
              }

            //-- Add the read to the mapping
            p1 = new Placement_t();
            p1->iid = r1;
            mi = graph.qrynodes.find (s1.c_str());
            if ( mi != graph.qrynodes.end() )
              p1->read = &(mi->second);
            mapping.reads.push_back (p1);

            //-- Add it's mate to the mapping
            if ( r2 != NULL_ID )
              {
                p2 = new Placement_t();
                p2->iid = r2;
                mi = graph.qrynodes.find (s2.c_str());
                if ( mi != graph.qrynodes.end() )
                  p2->read = &(mi->second);
                mapping.reads.push_back (p2);

                //-- Link the mates
                p1->mate = p2;
                p2->mate = p1;
              }
          }

        red_bank.close();
      }
      catch (const Exception_t & e) {
        cerr << "WARNING: " << e.what() << endl
             << "  could not collect read information from bank\n";
      }
    }

  //-- If no bank reads, populate from alignment info
  if ( mapping.reads.empty() )
    {
      cerr << "  getting partial list of reads from alignment graph\n";

      map<string, DeltaNode_t>::const_iterator mi;
      for ( mi = graph.qrynodes.begin(); mi != graph.qrynodes.end(); ++ mi )
        {
          Placement_t * pp = new Placement_t();
          pp->read = &(mi->second);
          mapping.reads.push_back (pp);
        }
    }
}


//-------------------------------------------------------- OutputFeatures ----//
void OutputFeatures (const Mapping_t & mapping)
{
  ofstream out (OPT_FeaturesName.c_str());
  if ( !out )
    cerr << "WARNING: could not open " << OPT_FeaturesName << endl;

  list<Signature_t>::const_iterator si;
  vector<long int>::const_iterator pi;
  vector<const DeltaNode_t *>::const_iterator ri;
  vector<long int>::const_iterator gi;
  long int s, e;

  for ( si = mapping.sigs.begin(); si != mapping.sigs.end(); ++ si )
    {
      ostringstream ss;

      ss << si->qry.size() << " ("
         << si->fc << ',' << si->rc << ") ";

      ri = si->ref.begin();
      gi = si->gap.begin();
      for ( pi = si->pos.begin(); pi != si->pos.end(); ++ pi, ++ ri )
        {
          if ( labs (*pi) == MAXIMAL )
            ss << 'B';
          else
            ss << labs (*pi);

          if ( *pi < *(++ pi) )
            ss << " <-[" << *((*ri)->id) << "]- ";
          else
            ss << " -[" << *((*ri)->id) << "]-> ";

          if ( labs (*pi) == MAXIMAL )
            ss << 'E';
          else
            ss << labs (*pi);

          if ( gi != si->gap.end() )
            ss << " |" << *(gi++) << "| ";
        }
      ss << '\n';

      ri = si->ref.begin();
      for ( pi = si->pos.begin(); pi != si->pos.end(); ++ pi, ++ ri )
        {
          if ( labs (*pi) != MAXIMAL )
            {
              s = e = labs (*pi);
              
              if ( *pi > *(pi+1) ) ++ s;
              else ++ e;

              out << "{FEA\n"
                  << "typ:B\n"
                  << "src:" << *((*ri)->id) << ",CTG\n"
                  << "com:" << ss.str()
                  << "clr:" << s << ',' << e << '\n'
                  << "}\n";
            }

          ++ pi;

          if ( labs (*pi) != MAXIMAL )
            {
              s = e = labs (*pi);

              if ( *pi > *(pi-1) ) ++ s;
              else ++ e;

              out << "{FEA\n"
                  << "typ:B\n"
                  << "src:" << *((*ri)->id) << ",CTG\n"
                  << "com:" << ss.str()
                  << "clr:" << s << ',' << e << '\n'
                  << "}\n";
            }
        }
    }

  out.close();
}


//----------------------------------------------------------- OutputPaths ----//
void OutputPaths (const Mapping_t & mapping)
{
  ofstream out (OPT_PathsName.c_str());
  if ( !out )
    cerr << "WARNING: could not open " << OPT_PathsName << endl;

  list<Signature_t>::const_iterator si;
  list<const DeltaNode_t *>::const_iterator qi;
  vector<long int>::const_iterator pi;
  vector<const DeltaNode_t *>::const_iterator ri;
  vector<long int>::const_iterator gi;

  for ( si = mapping.sigs.begin(); si != mapping.sigs.end(); ++ si )
    {
      out << "> "
          << si->qry.size() << " ("
          << si->fc << ',' << si->rc << ") ";

      ri = si->ref.begin();
      gi = si->gap.begin();
      for ( pi = si->pos.begin(); pi != si->pos.end(); ++ pi, ++ ri )
        {
          if ( labs (*pi) == MAXIMAL )
            out << 'B';
          else
            out << labs (*pi);

          if ( *pi < *(++ pi) )
            out << " <-[" << *((*ri)->id) << "]- ";
          else
            out << " -[" << *((*ri)->id) << "]-> ";

          if ( labs (*pi) == MAXIMAL )
            out << 'E';
          else
            out << labs (*pi);

          if ( gi != si->gap.end() )
            out << " |" << *(gi++) << "| ";
        }
      out << '\n';

      for ( qi = si->qry.begin(); qi != si->qry.end(); ++ qi )
        out << *((*qi)->id) << '\n';
      out << '\n';
    }

  out.close();
}


//----------------------------------------------------------- PlaceUnique ----//
void PlaceUnique (Mapping_t & mapping)
{
  vector<Placement_t *>::iterator ppi;

  for ( ppi = mapping.reads.begin(); ppi != mapping.reads.end(); ++ ppi )
    {
      //-- Skip if no alignments or already placed
      if ( (*ppi)->read == NULL || (*ppi)->chain != NULL )
        continue;
      
      //-- If unambiguous mapping, place the read
      if ( (*ppi)->read->chains.size() == 1 )
        {
          (*ppi)->chain = (*ppi)->read->chains.front();
        }
    }
}


//---------------------------------------------------------- PlaceRepeats ----//
void PlaceRepeats (Mapping_t & mapping)
{
  vector<Placement_t *>::iterator ppi;

  for ( ppi = mapping.reads.begin(); ppi != mapping.reads.end(); ++ ppi )
    {
      //-- Skip if no alignments or already placed
      if ( (*ppi)->read == NULL || (*ppi)->chain != NULL )
        continue;
      
      //-- If ambiguous mapping, place the read
      if ( (*ppi)->read->chains.size() != 1 )
        {
          (*ppi)->chain = (*ppi)->read->chains.front();
        }
    }
}


//------------------------------------------------------ RecordSignatures ----//
//
// Signatures will be generated as group of integer vectors, such that for the
// set of alignments of a query to the reference will be represented as:
//
// [sR]---[eR]   [sR]---[eR]   [sR]---[eR]
//   [nodeR]       [nodeR]       [nodeR]
//           [gap]         [gap]
//
// where the [sR] [eR] pairs are the start and end of each query alignment in
// the reference, and the corresponding [nodeR] is the reference in which the
// alignment occurs. The [gap] is the distance (negative for overlap) between
// adjacent alignments in the query. The [sR] [eR] pairs are ordered by the
// alignment's query position, lo to hi, and the [nodeR], [gap] values are
// similarly ordered. The [sR] [eR] values are also given a sign to represent
// represent the direction of the match in the reference (i.e. the alignment
// slope), +- for forward, -+ for reverse. If the first or last position is
// maximal, that is cannot be extended without reaching the end of the query,
// it is marked with the MAXIMAL value to indicate the break as incidental.
//
void RecordSignatures (const DeltaGraph_t & graph, Mapping_t & mapping)
{
  long int qlen;
  const string * qid;
  vector<DeltaEdgelet_t *> edgelets;

  map<string, DeltaNode_t>::const_iterator mi;
  vector<DeltaEdge_t *>::const_iterator ei;
  vector<DeltaEdgelet_t *>::iterator eli;

  for ( mi = graph.qrynodes.begin(); mi != graph.qrynodes.end(); ++ mi )
    {
      qid = mi->second.id;
      qlen = mi->second.len;

      //-- Collect the alignments for this query
      //   graph should be clean, so no need to worry about bad alignments
      edgelets.clear();
      for ( ei  = (mi->second).edges.begin();
            ei != (mi->second).edges.end(); ++ ei )
        for ( eli  = (*ei)->edgelets.begin();
              eli != (*ei)->edgelets.end(); ++ eli )
          edgelets.push_back (*eli);

      assert ( !edgelets.empty() );

      //-- Sort by loQ
      sort (edgelets.begin(), edgelets.end(), EdgeletCmp_t());

      int slope;
      long int pos;
      vector<DeltaEdgelet_t *>::iterator first = edgelets.begin();
      vector<DeltaEdgelet_t *>::iterator last = -- edgelets.end();

      //-- If alignment is clean, we don't need a signature from it
      if ( first == last &&
           (*first)->loQ <= 1 + OPT_MaxTrim &&
           (*first)->hiQ >= qlen - OPT_MaxTrim )
        continue;
 
      //-- Make a signature
      mapping.sigs.push_back (Signature_t());
      Signature_t & sig = mapping.sigs.back();
      sig.qry.push_back (&(mi->second));
      sig.fc ++;

      for ( eli = edgelets.begin(); eli != edgelets.end(); ++ eli )
        {
          slope = ((*eli)->dirR == (*eli)->dirQ) ? 1 : -1;

          //-- [sR]
          if ( eli == first  &&  (*eli)->loQ <= 1 + OPT_MaxTrim )
            pos = MAXIMAL * slope;
          else
            pos = (slope == 1) ? (*eli)->loR * slope : (*eli)->hiR * slope;
          sig.pos.push_back (pos);

          //-- [nodeR]
          sig.ref.push_back ((*eli)->edge->refnode);

          //-- [eR]
          if ( eli == last  &&  (*eli)->hiQ >= qlen - OPT_MaxTrim )
            pos = MAXIMAL * -slope;
          else
            pos = (slope == 1) ? (*eli)->hiR * -slope : (*eli)->loR * -slope;
          sig.pos.push_back (pos);

          //-- [gap]
          if ( eli != last )
            sig.gap.push_back ((*(eli + 1))->loQ - (*eli)->hiQ - 1);
        }
    }
}


//------------------------------------------------------------- ParseArgs ----//
void ParseArgs (int argc, char ** argv)
{
  int ch, errflg = 0;
  optarg = NULL;

  while ( !errflg  &&
  ((ch = getopt (argc, argv, "b:c:f:F:hi:P:s:t:")) != EOF) )
    switch (ch)
      {
      case 'b':
        OPT_BankName = optarg;
        break;

      case 'c':
        OPT_Redundancy = atoi (optarg);
        break;

      case 'f':
        OPT_Fuzzy = atoi (optarg);
        break;

      case 'F':
        OPT_FeaturesName = optarg;
        break;

      case 'h':
        PrintHelp (argv[0]);
        exit (EXIT_SUCCESS);
        break;

      case 'i':
	OPT_MinIdentity = atof (optarg);
	break;

      case 'P':
        OPT_PathsName = optarg;
        break;

      case 's':
	OPT_Seed = atoi (optarg);
	break;

      case 't':
        OPT_MaxTrim = atoi (optarg);
        break;

      default:
        errflg ++;
      }

  if ( errflg > 0 || optind != argc - 1 )
    {
      PrintUsage (argv[0]);
      cout << "Try '" << argv[0] << " -h' for more information.\n";
      exit (EXIT_FAILURE);
    }

  if ( OPT_Seed < 0 )
    OPT_Seed = time (NULL);

  OPT_AlignName = argv [optind ++];
}


//------------------------------------------------------------- PrintHelp ----//
void PrintHelp (const char * s)
{
  cout
    << ".NAME.\n\n"
    << s << "\n\n"
    << "Comparative assembly layout\n\n";

  PrintUsage (s);

  cout
    << ".OPTIONS.\n\n"
    << "-b string     Bank for mate/read info and layout/scaffold output\n"
    << "-c int        Number of concurring signatures required, default "
    << OPT_Redundancy << endl
    << "-f uint       Set the fuzzy equals tolerance for break positions,\n"
    << "              default " << OPT_Fuzzy << endl
    << "-F file       Output read paths to file as AMOS features\n"
    << "-h            Display help information\n"
    << "-i float      Set the minimum alignment identity, default "
    << OPT_MinIdentity << endl
    << "-P file       Output read paths to file in wacky format\n"
    << "-s uint       Set random generator seed to unsigned int, default\n"
    << "              seed is generated by the system clock\n"
    << "-t uint       Set maximum ignorable trim length, default "
    << OPT_MaxTrim << endl
    << endl;

  cout
    << ".DESCRIPTION.\n\n"
    << s << " in development\n"
    << endl;

  cout
    << ".KEYWORDS.\n\n"
    << "assembly, validation, comparison\n\n";

  return;
}


//------------------------------------------------------------ PrintUsage ----//
void PrintUsage (const char * s)
{
  cout
    << ".USAGE.\n\n"
    << s << " <deltafile>\n\n";

  return;
}
