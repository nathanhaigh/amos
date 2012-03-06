////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Michael Schatz
//! \date 2011.07.29
//!
//! \brief Compute the ce stat along an assembly
//!
////////////////////////////////////////////////////////////////////////////////

#include "foundation_AMOS.hh"
#include <cmath>
#include <iostream>
#include <cassert>
#include <sstream>
#include <unistd.h>
#include <map>
#include <iomanip>
#include "Feature_AMOS.hh"

#include "DataStore.hh"
#include "Insert.hh"
#include "CoverageStats.hh"

using namespace std;
using namespace AMOS;

typedef HASHMAP::hash_map<ID_t, Tile_t *> SeqTileMap_t;

//=============================================================== Globals ====//
string    OPT_BankName;                 // bank name parameter
bool      OPT_BankSpy = false;          // read or read-only spy
double    OPT_Features = 0.0;
bool      OPT_IIDs = false;
bool      OPT_SCAFFOLD = false;
int       OPT_MIN_LEN = 100;
bool      OPT_BEDPE = false;
bool      OPT_SHOW_DIST = false;
bool      OPT_COMPUTE_MEAN = false;

DataStore * m_datastore;
int m_connectMates = 1;
typedef std::vector<Insert *> InsertList_t;
InsertList_t m_inserts;


int m_coveragePlot = 0;
int m_cestats = 1;

double OPT_CE_mean  = 2244;
double OPT_CE_stdev = 250;
double OPT_CE_range = 8;

void printCEStats(const std::string &id)
{
  typedef map<ID_t, CoverageStats> LibStats;
  LibStats::iterator li;

  LibStats libStats = m_datastore->computeCEStats(m_inserts);

  if ( OPT_Features ) 
    {
      double b = 0;
      double e = 0;
      double sign;

      for (li = libStats.begin(); li != libStats.end(); li++)
        {
          // This are stored as oldvalue newvalue so skip every other one
          for (int i = 1; i < li->second.m_curpos; i+=2)
            {
              if ( b )
                if ( li->second.m_cestat[i] * sign < 0 ||
                     fabs(li->second.m_cestat[i]) < OPT_Features )
                  {
                    if (e-b >= OPT_MIN_LEN)
                    {
                      cout << id << " " << Feature_t::MATEPAIR << " " << b << " "
                           << li->second.m_coverage[i].x() << " ";
                      if  ( sign < 0 )
                        cout << "CE_COMPRESS ";
                      else
                        cout << "CE_STRETCH ";
                      cout << "LIB=" << li->first << endl;
                    }

                    b = e = 0;
                  }

              if ( fabs(li->second.m_cestat[i]) >= OPT_Features )
                {
                  if ( b )
                    e = li->second.m_coverage[i].x();
                  else
                    {
                      b = e = li->second.m_coverage[i].x();
                      sign = li->second.m_cestat[i];
                    }
                }
            }

          if ( b && e )
            {
              if (e-b >= OPT_MIN_LEN)
              {
                cout << id << " " << Feature_t::MATEPAIR << " " << b << " " << e << " ";
                if  ( sign < 0 )
                  cout << "CE_COMPRESS ";
                else
                  cout << "CE_STRETCH ";
                cout << "LIB=" << li->first << endl;
              }
              b = e = 0;
            }
        }
    }
  else
    {
      for (li = libStats.begin(); li != libStats.end(); li++)
        {
          cout << ">" << id  << " lib:" << li->first << endl;
          // This are stored as oldvalue newvalue so skip every other one
          for (int i = 1; i < li->second.m_curpos; i+=2)
            {
              cout << setprecision(10) << li->second.m_coverage[i].x() << " "
                   << setprecision(6)  << li->second.m_cestat[i] << endl;
            }
        }
    }

  vector<Insert *>::iterator i;
              
  for (i =  m_inserts.begin();
               i != m_inserts.end();
               i++)
          {
            delete (*i);
          }
              
          m_inserts.clear();
}


//========================================================== Fuction Decs ====//
//----------------------------------------------------- ParseArgs --------------
//! \brief Sets the global OPT_% values from the command line arguments
//!
//! \return void
//!
void ParseArgs (int argc, char ** argv);


//----------------------------------------------------- PrintHelp --------------
//! \brief Prints help information to cerr
//!
//! \param s The program name, i.e. argv[0]
//! \return void
//!
void PrintHelp (const char * s);


//----------------------------------------------------- PrintUsage -------------
//! \brief Prints usage information to cerr
//!
//! \param s The program name, i.e. argv[0]
//! \return void
//!
void PrintUsage (const char * s);

struct insert
{
  insert(long ll, long rr) : l(ll), r(rr) { }
  int dist() const { return r - l; }

  long l;
  long r;
};

bool sortl(const struct insert & i, const struct insert & j)
{
  return i.l < j.l;
}

struct sortr
{
  bool operator() (const struct insert & i, const struct insert & j)
  {
    return i.r > j.r;
  }
};

double Z(double sum, int n)
{
  if (n == 0) { return 0; }
  return ((sum/n) - OPT_CE_mean) / (OPT_CE_stdev / sqrt(n));
}

void printZ(const string & label, long pos, double sum, int n)
{
  cout << label << "\t" << pos << "\t"
       << setiosflags(ios::fixed) << setprecision(3) << Z(sum, n) << "\t" 
       << n << endl;
}


void computeCE(const string & id, vector<insert> & inserts)
{
  sort(inserts.begin(), inserts.end(), sortl);

  priority_queue<insert, vector<insert>, sortr> ends;

  double sum = 0;
  int n = inserts.size();

  if (OPT_COMPUTE_MEAN)
  {
    for (int i = 0; i < n; i++)
    {
      sum += inserts[i].dist();
    }

    double cmean = sum/n;
    cerr << "computed mean: " << cmean << " n: " << n;

    sum = 0;

    for (int i = 0; i < n; i++)
    {
      double diff = inserts[i].dist() - cmean;
      sum += diff * diff;
    }

    double cstdev = sqrt(sum / n);
    cerr << " computed stdev: " << cstdev << endl;
  }


  sum = 0;

  cout << ">" << id << " ce" << endl;
  printZ(id, 1, sum, 0);

  for (int i = 0; i < n; i++)
  {
    while ((!ends.empty()) && (ends.top().r <= inserts[i].l))
    {
      insert e = ends.top();

      printZ(id, e.r, sum, ends.size());

      ends.pop();
      sum -= e.dist();

      printZ(id, e.r, sum, ends.size());
    }

    printZ(id, inserts[i].l, sum, ends.size());
    //cout << inserts[i].l << "\t" << inserts[i].r << "\t" << inserts[i].dist() << endl;

    ends.push(inserts[i]);
    sum += inserts[i].dist();

    printZ(id, inserts[i].l, sum, ends.size());
  }

  while (!ends.empty())
  {
    insert e = ends.top();
    printZ(id, e.r, sum, ends.size());

    ends.pop();
    sum -= e.dist();

    printZ(id, e.r, sum, ends.size());
  }

  inserts.clear();

  if (sum != 0)
  {
    cerr << "ERROR: sum != 0" << endl;
  }
}



//========================================================= Function Defs ====//
int main (int argc, char ** argv)
{
  int exitcode = EXIT_SUCCESS;
  Read_t red;
  Fragment_t frg;
  Library_t lib;

  Scaffold_t scaff;
  Contig_t contig;

  //-- Parse the command line arguments
  ParseArgs (argc, argv);

  //-- BEGIN: MAIN EXCEPTION CATCH
  try {

    if (OPT_BEDPE)
    {
      cerr << "processing bedpe: " << OPT_BankName << endl;

      FILE * bedpe = fopen(OPT_BankName.c_str(), "r");

      if (!bedpe)
      {
        cerr << "ERROR: Couldn't open bedpe:" << OPT_BankName << endl;
        exit(1);
      }

      char templateid [1024];
      char seq1 [1024];
      char seq2 [1024];
      long s1, e1, s2, e2;
      char ds1 [1024], ds2[1024];

      string lastref;
      string curref;
      string ref2;

      char d1, d2;

      long t;
      char u;

      cerr << "expected mean: " << OPT_CE_mean << " expected stdev: " << OPT_CE_stdev << endl;
      cerr << "acceptable range: " << OPT_CE_mean - OPT_CE_range * OPT_CE_stdev << " - " << OPT_CE_mean + OPT_CE_range * OPT_CE_stdev << endl;

      vector<insert> inserts;

      while (fscanf(bedpe, "%s%s%ld%ld%s%s%ld%ld%s", 
                    templateid, seq1, &s1, &e1, ds1, seq2, &s2, &e2, ds2) == 9)
      {
        curref = seq1;

        if (curref != lastref)
        {
          if (lastref != "")
          {
            computeCE(lastref, inserts);
          }

          lastref = curref;

          if (OPT_SHOW_DIST)
          {
            cout << ">" << curref << endl;
          }
        }

        ref2 = seq2;

        if (ref2 == curref)
        {
          d1 = ds1[0];
          d2 = ds2[0];

          if (d2 == '+')
          {
            t = s2; s2 = s1; s1 = t;
            t = e2; e2 = e1; e1 = t;
            u = d2; d2 = d1; d1 = u;
          }

          if ((d1 == '+') && (d2 == '-'))
          {
            int dist = e2 - s1;

            if (abs(OPT_CE_mean - dist) <= (OPT_CE_range * OPT_CE_stdev))
            {
              if (OPT_SHOW_DIST)
              {
                cout << templateid << "\t" 
                     << seq1 << "\t" << s1 << "\t" << e1 << "\t" << d1 << "\t|\t"
                     << seq2 << "\t" << s2 << "\t" << e2 << "\t" << d2 << "\t|\t"
                     << dist << endl;
              }

              insert i(s1, e2);
              inserts.push_back(i);
            }
          }
        }
      }

      fclose(bedpe);

      if (lastref != "")
      {
        computeCE(lastref, inserts);
      }

    }
    else
    {
      m_datastore = new DataStore();
      m_datastore->openBank(OPT_BankName);

    if ( OPT_SCAFFOLD && m_datastore->scaffold_bank.isOpen() )
      {
        cerr << "Processing scaffolds... ";
        int scaffcount = 0;
        m_datastore->scaffold_bank.seekg(1);
        while (m_datastore->scaffold_bank >> scaff)
        {
          scaffcount++;
          vector <Tile_t> rtiling;
              
          m_datastore->mapReadsToScaffold(scaff, rtiling, 1);
          m_datastore->calculateInserts(rtiling, m_inserts, m_connectMates, 1);
              
          if ( OPT_IIDs )
          {
            stringstream oss;
            oss << scaff.getIID();
            printCEStats(oss.str());
          }
          else
            printCEStats(scaff.getEID());
              
        }
      }
      else if ( m_datastore->contig_bank.isOpen() )
      {
        cerr << "Processing contigs... ";
        int contigcount = 0;
        m_datastore->contig_bank.seekg(1);
        while (m_datastore->contig_bank >> contig)
        {
          contigcount++;

          m_datastore->calculateInserts(contig.getReadTiling(), m_inserts, m_connectMates, 1);            

          if ( OPT_IIDs )
          {
            stringstream oss;
            oss << contig.getIID();
            printCEStats(oss.str());
          }
          else
            printCEStats(contig.getEID());
        }
      }
    }
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

  while ( !errflg && ((ch = getopt (argc, argv, "hif:svSl:Bm:d:r:R")) != EOF) )
    switch (ch)
      {
      case 'h': PrintHelp (argv[0]); exit (EXIT_SUCCESS); break;

      case 'B': OPT_BEDPE = true; break; 
      case 'm': OPT_CE_mean = atof(optarg); break;
      case 'd': OPT_CE_stdev = atof(optarg); break;
      case 'r': OPT_CE_range = atoi(optarg); break;
      case 'i': OPT_IIDs = true; break; 
      case 'S': OPT_SCAFFOLD = true; break; 
      case 'f': OPT_Features = atof(optarg); break; 
      case 'l': OPT_MIN_LEN = atoi(optarg); break;
      case 's': OPT_BankSpy = true; break; 
      case 'R': OPT_COMPUTE_MEAN = true; break; 

      case 'v': PrintBankVersion (argv[0]); exit (EXIT_SUCCESS); break;


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
    << "Print the compression-expansion (CE) statistic value at the beginning and end \n"
    << "of each insert across each contig separated by library. If scaffold data is \n"
    << "available and -S is specifed, compute along scaffolds.\n"
    << "\n"
    << "General Options\n"
    << "  -h       Display help information\n"
    << "  -v       Display the compatible bank version\n"
    << "  -s       Disregard bank locks and write permissions (spy mode)\n"
    << "  -R       Recompute mean and stdev from data\n"
    << "\n"
    << "Bank Options\n"
    << "  -i       Dump scaffold/contig IIDs instead of EIDs\n"
    << "  -f float Only output CE features outside float deviations\n"
    << "  -l len   Only output features at least this length (default: " << OPT_MIN_LEN << ")\n"
    << "  -S       Consider scaffolds instead of contigs\n"
    << "\n"
    << "BEDPE Options\n"
    << "  -B       The input is a BEDPE file, not an AMOS bank\n"
    << "  -m mean  Use this as the library mean (default: " << OPT_CE_mean << ")\n"
    << "  -d std   Use this as the library stdev (default: " << OPT_CE_stdev << ")\n"
    << "  -r rng   Only use pairs that are within rng deviations from the mean (default: " << OPT_CE_range << ")\n"
    << endl;
}




//------------------------------------------------------------ PrintUsage ----//
void PrintUsage (const char * s)
{
  cerr
    << "\nUSAGE: " << s << "  [options]  <bank path>\n\n";
  return;
}
