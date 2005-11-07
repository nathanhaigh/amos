///////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Adam M Phillippy
//! \date 11/03/2005
//!
//! \brief Simulates a 454 pyro sequencing machine
//!
////////////////////////////////////////////////////////////////////////////////

#include "inttypes_AMOS.hh"
#include "fasta.hh"
#include "alloc.hh"
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iostream>
#include <unistd.h>
#include <sstream>

#include <set>
#include <list>
#include <deque>

using namespace std;
using namespace AMOS;

const int MAXLINE = 1024;
const int WIDTH   = 80;


//=============================================================== Options ====//
FILE *   OPT_Fasta         = NULL;
Size_t   OPT_Rounds        = 40;
Size_t   OPT_Reads         = 1000;
Size_t   OPT_Homos         = 15;
int      OPT_Seed          = -1;
bool     OPT_OFlow         = false;
string   OPT_Flow ("acgt");


//============================================================= Constants ====//
const char DELIM_CH = '$';
const char NULL_CH  = '\0';
const char WILD_CH  = 'n';

typedef deque<char> CharDeque_t;
typedef multiset<char> CharMultiset_t;

class Flow_t : private CharDeque_t
{
private:

  Size_t cnt;

public:

  friend bool Isomorphic (const Flow_t & a, const Flow_t & b);

  Flow_t()
    : CharDeque_t()
  { reset(); }

  Flow_t (const string & s)
    : CharDeque_t (s.begin(), s.end())
  { reset(); }

  void append (char c)
  {
    push_back (c);
    reset();
  }

  void clear()
  {
    CharDeque_t::clear();
    reset();
  }

  Size_t count()
  { return cnt; }

  Size_t round()
  { return cnt / size(); }

  void reset()
  { cnt = 0; }

  size_type size() const
  { return CharDeque_t::size(); }

  string order() const
  { return string (begin(), end()); }

  char base() const
  { return ( empty() ? NULL_CH : front() ); }

  void rotate()
  {
    if ( !empty() )
      {
        push_back (front());
        pop_front();
      }
    ++ cnt;
  }
};


//========================================================== Fuction Decs ====//
//------------------------------------------------------------ Isomorphic ----//
bool Isomorphic (const Flow_t & a, const Flow_t & b)
{
  if ( a.size() == b.size() )
    {
      Flow_t t (b);
      for ( t.reset(); t.round() == 0; t.rotate() )
        if ( (CharDeque_t)a == (CharDeque_t)t )
          return true;
    }
  return false;
}


//-------------------------------------------------------------- Matching ----//
inline bool Matching (char a, char b)
{
  return ( a == b || a == WILD_CH || b == WILD_CH );
}

//------------------------------------------------------------- ValidBase ----//
inline bool ValidBase (char a)
{
  return ( a == 'a' || a == 'c' || a == 'g' || a == 't' );
}

//---------------------------------------------------------- OptimizeFlow ----//
void OptimizeFlow (const char * seq);


//--------------------------------------------------------------- Permute ----//
void Permute (CharMultiset_t s, Flow_t f, vector<Flow_t> & flows);


//-------------------------------------------------------------- PickSeqs ----//
void PickSeqs (const char * seq, vector<const char *> & frags);


//------------------------------------------------------------- ReadInput ----//
void ReadInput (char * & seq);


//---------------------------------------------------------------- RunSim ----//
void RunSim (const vector<const char *> & frags);


//------------------------------------------------------------- ParseArgs ----//
void ParseArgs (int argc, char ** argv);
  
  
//------------------------------------------------------------- PrintHelp ----//
void PrintHelp (const char * s);
  
  
//------------------------------------------------------------ PrintUsage ----//
void PrintUsage (const char * s);
 


//========================================================= Function Defs ====//
int main (int argc, char ** argv)
{
  char * seq = NULL;
  vector<const char *> frags;

  ParseArgs (argc, argv);
  srand (OPT_Seed);

  ReadInput (seq);

  if ( OPT_OFlow )
    OptimizeFlow (seq);

  PickSeqs (seq, frags);

  RunSim (frags);

  return EXIT_SUCCESS;
}


//---------------------------------------------------------- OptimizeFlow ----//
void OptimizeFlow (const char * seq)
{
  vector<Flow_t> flows;
  Size_t hom;
  const char * p;

  Permute (CharMultiset_t (OPT_Flow.begin(), OPT_Flow.end()), Flow_t(), flows);

  for ( vector<Flow_t>::iterator i = flows.begin(); i != flows.end(); ++ i )
    {
      cout << i->order() << " - ";

      p = seq;
      for ( i->reset(); *p != NULL_CH; i->rotate() )
        while ( Matching (*p, Complement (i->base())) || *p == DELIM_CH )
          ++ p;

      cout << i->count() << endl;
    }
}


//--------------------------------------------------------------- Permute ----//
void Permute (CharMultiset_t s, Flow_t f, vector<Flow_t> & flows)
{
  if ( s.empty() )
    {
      for ( vector<Flow_t>::const_iterator
              i = flows.begin(); i != flows.end(); ++ i )
        if ( Isomorphic (*i, f) )
          return;

      flows.push_back (f);
      return;
    }

  for (CharMultiset_t::iterator i = s.begin(); i != s.end(); ++ i )
    {
      Flow_t nf (f);
      CharMultiset_t ns (s);

      nf.append (*i);
      ns.erase (ns.find(*i));

      Permute (ns, nf, flows);
    }
}


//--------------------------------------------------------------- PickSeq ----//
void PickSeqs (const char * seq, vector<const char *> & frags)
{
  Pos_t p;
  Size_t i, size;

  size = strlen (seq);

  for ( i = 0; i < OPT_Reads; ++ i )
    {
      p = 0;

      while ( seq[p] == DELIM_CH )
        p = (Pos_t)((double)size * rand() / (RAND_MAX + 1.0));

      frags.push_back (seq + p);
    }
}


//------------------------------------------------------------- ReadInput ----//
void ReadInput (char * & seq)
{
  long beg, end, size;
  string rec, tag;
  char * p;

  beg = ftell (OPT_Fasta);
  fseek (OPT_Fasta, 0, SEEK_END);
  end = ftell (OPT_Fasta);
  fseek (OPT_Fasta, 0, SEEK_SET);
  size = (end - beg) * 2;

  p = seq = (char *) SafeRealloc (seq, size);

  while ( Fasta_Read (OPT_Fasta, rec, tag) )
    {
      if ( rec.empty() )
        continue;

      *p = DELIM_CH;
      strcpy (++ p, rec.c_str());
      for ( ; *p != NULL_CH; ++ p )
        {
          *p = tolower (*p);
          if ( ! ValidBase (*p) && *p != WILD_CH )
            {
              cerr << "WARNING: Converting '" << *p
                   << "' to '" << WILD_CH << "'\n";
              *p = WILD_CH;
            }
        }

      Reverse_Complement (rec);

      *p = DELIM_CH;
      strcpy (++ p, rec.c_str());
      for ( ; *p != NULL_CH; ++ p )
        {
          *p = tolower (*p);
          if ( ! ValidBase (*p) && *p != WILD_CH )
            {
              cerr << "WARNING: Converting '" << *p
                   << "' to '" << WILD_CH << "'\n";
              *p = WILD_CH;
            }
        }
    }
}


//---------------------------------------------------------------- RunSim ----//
void RunSim (const vector<const char *> & frags)
{
  Flow_t flow (OPT_Flow);
  Pos_t hom, uid;
  const char * p;
  int cnt;

  uid = 0;
  for ( vector<const char *>::const_iterator
          i = frags.begin(); i != frags.end(); ++ i )
    {
      p = *i;

      printf (">%d\n", ++ uid);

      int cnt = 0;
      for ( flow.reset(); flow.round() < OPT_Rounds; flow.rotate() )
        for ( hom = 0;
              Matching (*p, Complement (flow.base())) && hom < OPT_Homos;
              ++ hom )
          {
            if ( cnt == WIDTH )
              {
                putchar ('\n');
                cnt = 0;
              }
            putchar (*p);
            ++ cnt;
            ++ p;
          }

      putchar ('\n');
    }
}


//------------------------------------------------------------- ParseArgs ----//
void ParseArgs (int argc, char ** argv)
{
  int ch, errflg = 0;
  optarg = NULL;
         
  while ( !errflg  &&  ((ch = getopt (argc, argv, "f:Fhi:p:r:s:")) != EOF) )
    switch (ch)
      {
      case 'f':
        OPT_Flow = optarg;
        break;
        
      case 'F':
        OPT_OFlow = true;
        break;

      case 'h':
        PrintHelp (argv[0]);
        exit (EXIT_SUCCESS);
        break;

      case 'i':
        OPT_Rounds = atoi (optarg);
        break;

      case 'p':
        OPT_Homos = atoi (optarg);
        break;

      case 'r':
        OPT_Reads = atol (optarg);
        break;

      case 's':
        OPT_Seed = atoi (optarg);
        break;

      default:
        errflg ++;
      }

  if ( optind != argc - 1 )
    errflg ++;

  for ( string::iterator i = OPT_Flow.begin(); i != OPT_Flow.end(); ++ i )
    {
      *i = tolower (*i);
      if ( ! ValidBase (*i) )
        {
          cerr << "ERROR: Only a,c,g,t allowed in flow\n";
          errflg ++;
        }
    }

  if ( errflg > 0 )
    {
      PrintUsage (argv[0]);
      cerr << "Try '" << argv[0] << " -h' for more information." << endl;
      exit (EXIT_FAILURE);
    }

  if ( (OPT_Fasta = fopen (argv[optind ++], "r")) == NULL )
    {
      cerr << "ERROR: Could not open FastA file " << argv[optind-1] << endl;
      exit (EXIT_FAILURE);
    }

  if ( OPT_Seed < 0 )
    OPT_Seed = time (NULL);
}


//------------------------------------------------------------- PrintHelp ----//
void PrintHelp (const char * s)
{
  PrintUsage (s);
  cerr
    << "-f string     Nucleotide flow order for one round, default "
    << OPT_Flow << endl
    << "-F            Calculate and use the optimal flow permutation for -f\n"
    << "-h            Display help information\n"
    << "-i int        Number round iterations to simulate, default "
    << OPT_Rounds << endl
    << "-p int        Number of sequence-able homopolymer bases per flow,\n"
    << "              default "
    << OPT_Homos << endl
    << "-r int        Number of reads to sequence, default "
    << OPT_Reads << endl
    << "-s uint       Set random generator seed, default seed is generated\n"
    << "              by the system clock\n"
    << endl;
  cerr
    << "Simulates a 454 pyro sequencing machine.\n"
    << endl;
  return;
}
 
 
//------------------------------------------------------------ PrintUsage ----//
void PrintUsage (const char * s)
{
  cerr << "\nUSAGE:" << s << " [options] <fasta>\n"
       << endl;
  return;
}
