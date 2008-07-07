#include "foundation_AMOS.hh"
#include <iostream>
#include <cassert>
#include <unistd.h>
#include <map>
#include <cmath>

#include "DataStore.hh"
#include "Insert.hh"
#include "InsertStats.hh"
#include <stdio.h>
#include "amp.hh"

using namespace std;
using namespace AMOS;
DataStore * m_datastore;



//=============================================================== Globals ====//
string OPT_BankName;                 // bank name parameter
bool   OPT_BankSpy = false;          // read or read-only spy
bool   OPT_Verbose = false;

void ParseArgs (int argc, char ** argv);
void PrintHelp (const char * s);
void PrintUsage (const char * s);


string tableString;

#define DIV(a,b) (b?(double(a)/b):0.0)


static void statsAddRow(string tag, string value)
{
  tableString += "<tr><td>" + tag + "</td><td>" + value + "</td></tr>\n";
}

static void statsAddHeader(string header)
{
  string s = "<b>" + header + "</b>";
  statsAddRow(s, " ");
}


static void statsAddRow(string tag, int value)
{
  char buffer[1024];
  sprintf(buffer, "%d", value);
  statsAddRow(tag, buffer);
}

static void statsAddRow(string tag, double value)
{
  char buffer[1024];
  sprintf(buffer, "%.02f", value);
  statsAddRow(tag, buffer);
}



void computeStatistics()
{
  EventTime_t timer;
  if (OPT_Verbose) { cerr << "Loading AssemblyStats..."; }

  tableString = "<html>\n<head></head>\n<body><table>\n";

  if (m_datastore->scaffold_bank.isOpen())
  {
    vector<int> bases;
    vector<int> spans;
    vector<int> contigs;
    int totalcontigs = 0;
    int totalbases = 0;
    int totalspan = 0;
    int numscaffolds = 0;

    int largescaffolds = 0;
    int largescaffoldspan = 0;
    int largescaffoldThreshold = 2000;

    AMOS::Scaffold_t scf;

    m_datastore->scaffold_bank.seekg(1);
    while (m_datastore->scaffold_bank >> scf)
    {
      int s = scf.getSpan();
      int b = 0;
      vector<Tile_t>::const_iterator ci;
      const vector<Tile_t> & ctiling = scf.getContigTiling();
      for (ci =  ctiling.begin();
           ci != ctiling.end();
           ci++)
      {
        b+=ci->getGappedLength();
      }

      totalcontigs += ctiling.size();
      totalbases += b;
      totalspan += s;

      contigs.push_back(ctiling.size());
      spans.push_back(s);
      bases.push_back(b);

      numscaffolds++;

      if (s >= largescaffoldThreshold)
      {
        largescaffolds++;
        largescaffoldspan += s;
      }
    }

    sort(contigs.begin(), contigs.end(), less<int>());
    sort(bases.begin(),   bases.end(),   less<int>());
    sort(spans.begin(),   spans.end(),   less<int>());

    int n50bases = 0;

    int basesthreshold = totalbases/2;
    int curbases = 0;

    for (n50bases = numscaffolds-1; n50bases >= 0; n50bases--)
    {
      curbases += bases[n50bases];
      if (curbases >= basesthreshold)
      {
        break;
      }
    }


    statsAddHeader("[Scaffolds]");
    statsAddRow("TotalScaffolds", numscaffolds);
    statsAddRow("TotalContigsInScaffolds", totalcontigs);
    statsAddRow("MeanContigsPerScaffold", DIV(totalcontigs, numscaffolds));
    statsAddRow("MinContigsPerScaffold", contigs[0]);
    statsAddRow("MaxContigsPerScaffold", contigs[contigs.size()-1]);
    statsAddRow(" ", " ");

    statsAddRow("TotalBasesInScaffolds", totalbases);
    statsAddRow("MeanBasesInScaffolds", DIV(totalbases, numscaffolds));
    statsAddRow("MaxBasesInScaffolds", bases[numscaffolds-1]);
    statsAddRow("N50ScaffoldBases", bases[n50bases]);
    statsAddRow(" ", " ");


    statsAddRow("TotalSpanOfScaffolds", totalspan);
    statsAddRow("MeanSpanOfScaffolds", DIV(totalspan, numscaffolds));
    statsAddRow("MinScaffoldSpan", spans[0]);
    statsAddRow("MaxScaffoldSpan", spans[numscaffolds-1]);
    statsAddRow("IntraScaffoldGaps", totalcontigs - numscaffolds);
    statsAddRow("2KbScaffolds", largescaffolds);
    statsAddRow("2KbScaffoldSpan", largescaffoldspan);
    statsAddRow("2KbScaffoldPercent", DIV(largescaffoldspan, totalspan) * 100.0);
    statsAddRow("MeanSequenceGapSize", DIV(totalspan-totalbases, totalcontigs-numscaffolds));
    statsAddRow(" ", " ");
  }

  int LARGETHRESHOLD = 10000;

  if (m_datastore->contig_bank.isOpen())
  {
    AMOS::Contig_t ctg;
    vector<int> sizes;
    vector<int> largesizes;
    vector<int> smallsizes;
    int totalsize = 0;
    int totalreads = 0;

    int largecount = 0;
    int largesize = 0;
    int largereads = 0;

    int smallcount = 0;
    int smallsize = 0;
    int smallreads = 0;


    m_datastore->contig_bank.seekg(1);
    m_datastore->contig_bank.setFixedStoreOnly(true);
    while (m_datastore->contig_bank >> ctg)
    {
      int l = ctg.getLength();
      int r = ctg.getReadTiling().size();

      sizes.push_back(l);
      totalsize += l;
      totalreads += r;

      if (l >= LARGETHRESHOLD)
      {
        largecount++;
        largesize += l;
        largereads += r;
      }
      else
      {
        smallcount++;
        smallsize += l;
        smallreads += r;
      }
    }
    m_datastore->contig_bank.setFixedStoreOnly(false);

    sort(sizes.begin( ), sizes.end( ), less<int>( ) );

    int thresh = totalsize / 2;
    int cum = 0;
    int n50 = 0;

    int maxlarge = 0;
    int minlarge = 0;
    int maxsmall = 0;
    int minsmall = 0;

    for (int i = sizes.size()-1; i >= 0; i--)
    {
      int cur = sizes[i];
      cum += cur;

      if (!n50 && cum >= thresh) { n50 = i; }

      if (cur >= LARGETHRESHOLD)
      {
        if (maxlarge == 0){maxlarge = cur;}
        minlarge = cur;
      }
      else
      {
        if (maxsmall == 0){maxsmall = cur;}
        minsmall = cur;
      }
    }


    // Contigs

    statsAddHeader("[Contigs]");
    statsAddRow("TotalContigs",  (int)m_datastore->contig_bank.getSize());
    statsAddRow("TotalBasesInContigs", totalsize);
    statsAddRow("MeanContigSize", DIV(totalsize, sizes.size()));
    statsAddRow("MinContigSize", sizes[0]);
    statsAddRow("MaxContigSize", sizes[sizes.size()-1]);
    statsAddRow("N50ContigBases", sizes[n50]);
    statsAddRow(" ", " ");

    // Big Contigs
    char buffer[1024];
    sprintf(buffer, "%d", LARGETHRESHOLD);
    statsAddHeader(string("[BigContigs_greater_") + buffer + "]");
    statsAddRow("TotalBigContigs", largecount);
    statsAddRow("BigContigLength", largesize);
    statsAddRow("MeanBigContigSize", DIV(largesize,largecount));
    statsAddRow("MinBigContig", minlarge);
    statsAddRow("MaxBigContig", maxlarge);
    statsAddRow("BigContigsPercentBases", DIV(largesize,totalsize)*100.0);
    statsAddRow(" ", " ");

    // Small Contigs
    statsAddHeader("[SmallContigs]");
    statsAddRow("TotalSmallContigs", smallcount);
    statsAddRow("SmallContigLength", smallsize);
    statsAddRow("MeanSmallContigSize", DIV(smallsize, smallcount));
    statsAddRow("MinSmallContig", minsmall);
    statsAddRow("MaxSmallContig", maxsmall);
    statsAddRow("SmallContigsPercentBases", DIV(smallsize,totalsize)*100.0);
    statsAddRow(" ", " ");


    // Reads
    statsAddHeader("[Reads]");
    statsAddRow("TotalReads", (int)m_datastore->read_bank.getSize());
    statsAddRow("ReadsInContigs", totalreads);
    statsAddRow("BigContigReads", largereads);
    statsAddRow("SmallContigReads", smallreads);
    statsAddRow("SingletonReads", (int) m_datastore->read_bank.getSize() - totalreads);
  }

  tableString += "</table>\n";

  if (OPT_Verbose) { cerr << timer.str() << endl; }
}












//========================================================= Function Defs ====//
int main (int argc, char ** argv)
{
  int exitcode = EXIT_SUCCESS;


  Scaffold_t scaff;
  Contig_t contig;

  //-- Parse the command line arguments
  ParseArgs (argc, argv);

  //-- BEGIN: MAIN EXCEPTION CATCH
  try 
  {

    m_datastore = new DataStore();
    m_datastore->openBank(OPT_BankName);

    computeStatistics();
    cout << tableString << endl;
  }
  catch (const Exception_t & e) {
    cerr << "FATAL: " << e . what( ) << endl
         << "  there has been a fatal error, abort" << endl;
    exitcode = EXIT_FAILURE;
  }
  //-- END: MAIN EXCEPTION CATCH

  return exitcode;
}




//------------------------------------------------------------- ParseArgs ----//
void ParseArgs (int argc, char ** argv)
{
  int ch, errflg = 0;
  optarg = NULL;

  while ( !errflg && ((ch = getopt (argc, argv, "hsvf:")) != EOF) )
    switch (ch)
      {
      case 'h': PrintHelp (argv[0]); exit (EXIT_SUCCESS); break;

      default:
        errflg ++;
      }

  if (errflg > 0 || optind != argc - 1)
  {
    PrintUsage (argv[0]);
    cerr << "Try '" << argv[0] << " -h' for more information.\n";
    exit (EXIT_FAILURE);
  }

  OPT_BankName = argv [optind ++];
}




//------------------------------------------------------------- PrintHelp ----//
void PrintHelp (const char * s)
{
  PrintUsage (s);
  cerr
    << "-h            Display help information\n"
    << endl;
  cerr
    << "Compute statistics on an assembly stored in a bank\n";
  return;
}




//------------------------------------------------------------ PrintUsage ----//
void PrintUsage (const char * s)
{
  cerr
    << "\nUSAGE: " << s << "  [options]  <bank path>\n\n";
  return;
}
