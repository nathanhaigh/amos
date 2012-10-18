////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Mike Schatz
//! \date 03/14/2005
//!
//! \brief Dumps RED, FRG, LIB messages associated with a list of reads
//!
////////////////////////////////////////////////////////////////////////////////

#include "foundation_AMOS.hh"
#include <iostream>
#include <cassert>
#include <unistd.h>
#include <algorithm>
#include <set>

using namespace std;
using namespace AMOS;

string OPT_BankName; 
bool   OPT_BankSpy   = false;

bool   OPT_NullMates = false;
bool   OPT_AutoMate  = false;
bool   OPT_ReadEID   = false;
bool   OPT_ReadIID   = false;
bool   OPT_NAMEONLY  = false;

string OPT_EIDInclude;
string OPT_IIDInclude;
string OPT_EIDExclude;
string OPT_IIDExclude;

void ParseArgs(int argc, char ** argv);
void PrintHelp(const char * s);
void PrintUsage(const char * s);

char EXCLUDE  = 0;
char INCLUDE  = 1;
char PRINTED  = 3;
char UNKNOWN  = 4;

typedef HASHMAP::hash_map<AMOS::ID_t, char> idmap;
idmap iidStatus;

typedef set<ID_t> LibSet;
LibSet libraries;

typedef set<ID_t> FrgSet;
FrgSet fragments;

inline void printLibrary(Bank_t & lib_bank, ID_t libid)
{
  if (!OPT_NAMEONLY)
  {
    LibSet::iterator li = libraries.find(libid);
    if (li == libraries.end())
    {
      Library_t lib;
      lib_bank.fetch(libid, lib);

      Message_t msg;
      lib.writeMessage(msg);
      msg.write(cout);

      libraries.insert(libid);
    }
  }
}



int inexcount[4] = {0,0,0,0};

void loadEIDs(Bank_t & red_bank, const string & eidfilename, int value)
{
  if (!eidfilename.empty())
  {
    ifstream eidfile;
    eidfile.open(eidfilename.c_str());

    if (!eidfile) { throw Exception_t("Couldn't open EID File", __LINE__, __FILE__); }

    string eid;
    while (eidfile >> eid)
    {
      ID_t iid = red_bank.lookupIID(eid.c_str());
      if (iid != AMOS::NULL_ID)
      {
        iidStatus[iid] = value;
        inexcount[value]++;
      }
      else
      {
        cerr << "ERROR: EID:" << eid << " not found in bank, skipping" << endl;
      }
    }
  }
}

void loadIIDs(Bank_t & red_bank, const string & iidfilename, int value)
{
  if (!iidfilename.empty())
  {
    ifstream iidfile;
    iidfile.open(iidfilename.c_str());

    if (!iidfile) { throw Exception_t("Couldn't open IID File", __LINE__, __FILE__); }

    ID_t iid;
    while (iidfile >> iid)
    {
      if (iid != AMOS::NULL_ID)
      {
        iidStatus[iid] = value;
        inexcount[value]++;
      }
      else
      {
        cerr << "ERROR: IID:" << iid << " not found in bank, skipping" << endl;
      }
    }
  }
}

int printCount = 0;
inline void printRead(Bank_t & red_bank, ID_t iid)
{
  if (iid)
  {
    if (OPT_ReadEID)
    {
      cout << red_bank.lookupEID(iid) << endl;
    }
    else if (OPT_ReadIID)
    {
      cout << iid << endl;
    }
    else
    {
      Read_t red;
      Message_t msg;
      red_bank.fetch(iid, red);

      if (red.getClearRange().isReverse())
      {
        Range_t rng = red.getClearRange();
        rng.swap();
        red.setClearRange(rng);
      }

      red.writeMessage(msg);
      msg.write(cout);
    }

    iidStatus[iid] = PRINTED;
    printCount++;
  }
}


int main (int argc, char ** argv)
{
  int exitcode = EXIT_SUCCESS;
  Read_t red1, red2;
  Fragment_t frg;
  Library_t lib;
  pair<ID_t, ID_t> mtp;

  Bank_t red_bank (Read_t::NCODE);
  Bank_t lib_bank (Library_t::NCODE);
  BankStream_t frg_bank (Fragment_t::NCODE);

  ParseArgs (argc, argv);
  OPT_NAMEONLY = (OPT_ReadEID || OPT_ReadIID);


  try 
  {
    BankMode_t bm = OPT_BankSpy ? B_SPY : B_READ;
    red_bank.open(OPT_BankName, bm);

    cerr << "Bank has " << red_bank.getSize() << " reads" << endl;

    loadEIDs(red_bank, OPT_EIDInclude, INCLUDE); 
    loadIIDs(red_bank, OPT_IIDInclude, INCLUDE); 

    cerr << "Loaded " << inexcount[INCLUDE] << " reads to include" << endl;

    loadEIDs(red_bank, OPT_EIDExclude, EXCLUDE); 
    loadIIDs(red_bank, OPT_IIDExclude, EXCLUDE); 

    cerr << "Loaded " << inexcount[EXCLUDE] << " reads to exclude" << endl;

    bool doInclude = inexcount[INCLUDE] > 0;
    bool doExclude = inexcount[EXCLUDE] > 0;

    // if  doInclude &&  doExclude  => Print {Included} - {Excluded}  ==> Default Exclude, clobbering
    // if  doInclude && !doExclude  => Print {Included}               ==> Default Exclude
    // if !doInclude &&  doExclude  => Print {Everyone} - {Excluded}  ==> Default Include
    // if !doInclude && !doExclude  => Error

    char DEFAULT = EXCLUDE;
    if (doExclude && !doInclude) { DEFAULT = INCLUDE; }

    if (iidStatus.empty())
    {
      cerr << "No reads selected to include/exclude" << endl;
      return 0;
    }

    Message_t msg;

    int nullified = 0;
    int halfmates = 0;
    int mateinclude = 0;
    int mateexclude = 0;
    int clobbered = 0;
    int unmated = 0;

    if (frg_bank.exists(OPT_BankName))
    {
      cerr << "Processing FRG bank..." << endl;

      frg_bank.open(OPT_BankName, bm);
      lib_bank.open(OPT_BankName, bm);

      // Visit each fragment once. Note: mate-pairs may be undefined
      while (frg_bank >> frg)
      {
        mtp = frg.getMatePair();
        idmap::iterator r1 = iidStatus.find(mtp.first);
        idmap::iterator r2 = iidStatus.find(mtp.second);

        const char r1status = (r1 == iidStatus.end()) ? UNKNOWN : r1->second; // { UNKNOWN, INCLUDE, EXCLUDE }
        const char r2status = (r2 == iidStatus.end()) ? UNKNOWN : r2->second; // { UNKNOWN, INCLUDE, EXCLUDE }

        const char r1print = (r1status == UNKNOWN) ? DEFAULT : r1status; // { INCLUDE, EXCLUDE }
        const char r2print = (r2status == UNKNOWN) ? DEFAULT : r2status; // { INCLUDE, EXCLUDE }

        if (0)
        {
          if (r1 != iidStatus.end() || r2 != iidStatus.end())
          {
            cerr << "r1: " << mtp.first  << " r1status: " << (int) r1status << " r1print: " << (int) r1print << endl;
            cerr << "r2: " << mtp.second << " r2status: " << (int) r2status << " r2print: " << (int) r2print << endl;
          }
        }

        if (((mtp.first == AMOS::NULL_ID)  && (mtp.second == AMOS::NULL_ID)) ||
            ((mtp.first == AMOS::NULL_ID)  && (r2print == EXCLUDE)) ||
            ((mtp.second == AMOS::NULL_ID) && (r1print == EXCLUDE)))
            
        {
          // Skip both because of 1 NULLID + Exclude or 2 NULLIDs
          // Otherwise there is at least 1 to print
          // don't mark NULL_ID as EXCLUDE, though, or AutoMate will screw up
        }
        else if (((r1print == EXCLUDE) && (r2print == EXCLUDE)) ||
                 ((r1status == EXCLUDE) && OPT_AutoMate) || // override r2 include
                 ((r2status == EXCLUDE) && OPT_AutoMate))   // override r1 include
        {
          // skipping both reads and frg because of exclusion

          // only mark reads excluded because of mate
          if ((r2status == EXCLUDE) && (r1status != EXCLUDE)) 
          { 
            if (r1status == INCLUDE) { clobbered++; }
            iidStatus[mtp.first] = EXCLUDE; mateexclude++; 
          }

          if ((r1status == EXCLUDE) && (r2status != EXCLUDE)) 
          { 
            if (r2status == INCLUDE) { clobbered++; }
            iidStatus[mtp.second] = EXCLUDE; mateexclude++; 
          }
        }
        else
        {
          // printing at least 1 read

          // status = {UNKNOWN, EXCLUDE, INCLUDE} 
          // print = {INCLUDE, EXCLUDE}

          printLibrary(lib_bank, frg.getLibrary());

          bool printr1 = (r1print == INCLUDE);
          bool printr2 = (r2print == INCLUDE);

          if ((r1status == UNKNOWN) && printr2 && OPT_AutoMate) { printr1 = true; }
          if ((r2status == UNKNOWN) && printr1 && OPT_AutoMate) { printr2 = true; }

          if (!printr1 && !printr2)
          {
            cerr << "INTERNAL ERROR: not printing either!!!" << endl;
            cerr << "r1: " << mtp.first  << " r1status: " << (int) r1status << " DEFAULT: " << (int) DEFAULT << " AutoMate: " << OPT_AutoMate << endl;
            cerr << "r2: " << mtp.second << " r2status: " << (int) r2status << " DEFAULT: " << (int) DEFAULT << " AutoMate: " << OPT_AutoMate << endl;

            return 1;
          }
          else if (printr1 && r1status == EXCLUDE)
          {
            cerr << "INTERNAL ERROR: printr1 but r1status == EXCLUDE" << endl;
            cerr << "r1: " << mtp.first  << " r1status: " << (int) r1status << " DEFAULT: " << (int) DEFAULT << " AutoMate: " << OPT_AutoMate << endl;
            cerr << "r2: " << mtp.second << " r2status: " << (int) r2status << " DEFAULT: " << (int) DEFAULT << " AutoMate: " << OPT_AutoMate << endl;

            return 1;
          }
          else if (printr2 && r2status == EXCLUDE)
          {
            cerr << "INTERNAL ERROR: printr2 but r2status == EXCLUDE" << endl;
            cerr << "r1: " << mtp.first  << " r1status: " << (int) r1status << " DEFAULT: " << (int) DEFAULT << " AutoMate: " << OPT_AutoMate << endl;
            cerr << "r2: " << mtp.second << " r2status: " << (int) r2status << " DEFAULT: " << (int) DEFAULT << " AutoMate: " << OPT_AutoMate << endl;

            return 1;
          }

          bool brokenmate = !(printr1 && printr2);

          if (brokenmate)
          {
            if(OPT_NullMates)
            {
              nullified++;
              frg.setReads(make_pair(NULL_ID, NULL_ID));
            }
            else 
            {
              halfmates++;
            }
          }

          if (!OPT_NAMEONLY)
          {
            frg.writeMessage(msg);
            msg.write(cout);
            fragments.insert(frg.getIID());
          }

          if (printr1)
          {
            printRead(red_bank, mtp.first);

            if (r2status == INCLUDE && OPT_AutoMate && r1status == UNKNOWN)
            {
              mateinclude++;
            }
          }

          if (printr2)
          {
            printRead(red_bank, mtp.second);

            if (r1status == INCLUDE && OPT_AutoMate && r2status == UNKNOWN)
            {
              mateinclude++;
            }
          }
        }
      }

      if (doInclude && OPT_AutoMate) { cerr << "Included " << mateinclude << " extra mates" << endl; }
      if (doExclude && OPT_AutoMate) { cerr << "Excluded " << mateexclude << " extra mates" << endl; }

      if (doInclude && doExclude && OPT_AutoMate) 
       { cerr << "Clobbered " << clobbered   << " mates" << endl; }

      if (OPT_NullMates)          
       { cerr << "Nullified " << nullified << " half mates" << endl; }

      if (!OPT_NullMates && !OPT_AutoMate && halfmates)          
       { cerr << "WARNING: Printed " << halfmates << " half mates" << endl; }

      // iidStatus == PRINTED => Printed
      // iidStatus == EXCLUDE => Exclude listed or by mate
      // iidStatus == INCLUDE => Should be printed, but not in a mate-pair
      // iidStatus == UNKNOWN => Take default action

      cerr << "Printing unmated reads..." << endl;

      IDMap_t::const_iterator ri;
      for (ri =  red_bank.getIDMap().begin();
           ri != red_bank.getIDMap().end();
           ri++)
      {
        idmap::iterator r1 = iidStatus.find(ri->iid);

        if (((r1 == iidStatus.end()) && (DEFAULT == INCLUDE)) || 
            ((r1 != iidStatus.end()) && (r1->second == INCLUDE)))
        {
          if (!OPT_NAMEONLY)
          {
            red_bank.fetch(ri->iid, red1);
            
            frg_bank.seekg(frg_bank.lookupBID(red1.getFragment()));
            frg_bank >> frg;

            printLibrary(lib_bank, frg.getLibrary());

            FrgSet::iterator fi;

            fi = fragments.find(frg.getIID());
            
            if (fi == fragments.end())
            {
              frg.writeMessage(msg);
              msg.write(cout);
              fragments.insert(frg.getIID());
            }
          }

          printRead(red_bank, ri->iid);
          unmated++;
        }
      }

      cerr << "Included " << unmated << " unmated reads" << endl;

      frg_bank.close();
      lib_bank.close();
    }
    else
    {
      cerr << "WARNING: No fragment bank, only printing reads" << endl;

      IDMap_t::const_iterator ri;
      for (ri =  red_bank.getIDMap().begin();
           ri != red_bank.getIDMap().end();
           ri++)
      {
        idmap::iterator r1 = iidStatus.find(ri->iid);

        if (((r1 == iidStatus.end()) && (DEFAULT == INCLUDE)) || 
            ((r1 != iidStatus.end()) && (r1->second == INCLUDE)))
        {
          printRead(red_bank, ri->iid);
          unmated++;
        }
      }
    }

    cerr << endl;
    cerr << "Printed " << printCount << " reads, " 
         << fragments.size() << " fragments, "
         << libraries.size() << " libraries." << endl;

    red_bank.close();
  }
  catch (const Exception_t & e) 
  {
    cerr << "FATAL: " << e . what( ) << endl
         << "  there has been a fatal error, abort" << endl;
    exitcode = EXIT_FAILURE;
  }

  return exitcode;
}




//------------------------------------------------------------- ParseArgs ----//
void ParseArgs (int argc, char ** argv)
{
  int ch, errflg = 0;
  optarg = NULL;

  while ( !errflg && ((ch = getopt (argc, argv, "hvsLlMNI:i:X:x:")) != EOF) )
  {
    switch (ch)
    {
      case 'h': PrintHelp (argv[0]);        exit (EXIT_SUCCESS); break;
      case 'v': PrintBankVersion (argv[0]); exit (EXIT_SUCCESS); break;
      case 's': OPT_BankSpy = true;          break;

      case 'M': OPT_AutoMate = true;         break;
      case 'N': OPT_NullMates = true;        break;

      case 'I': OPT_EIDInclude = optarg;     break;
      case 'i': OPT_IIDInclude = optarg;     break;

      case 'X': OPT_EIDExclude = optarg;     break;
      case 'x': OPT_IIDExclude = optarg;     break;

      case 'L': OPT_ReadEID = true;          break;
      case 'l': OPT_ReadIID = true;          break;

      default: errflg ++; 
    };
  }

  if ( errflg > 0 || optind != argc - 1 )
  {
    PrintUsage (argv[0]);
    cerr << "Try '" << argv[0] << " -h' for more information.\n";
    exit (EXIT_FAILURE);
  }

  OPT_BankName = argv [optind ++];
}




void PrintHelp (const char * s)
{
  PrintUsage (s);
  cerr
    << "Print RED, FRG & LIB messages of selected reads from an AMOS Bank\n\n"
    << "-h            Display help information\n"
    << "-s            Disregard bank locks and write permissions (spy mode)\n"
    << "-v            Display the compatible bank version\n"
    << "-i IIDFile    Specify file containing list of iid's to print\n"
    << "-I EIDFile    Specify file containing list of eid's to print\n"
    << "-L            Just output read names that would be printed by eid\n"
    << "-l            Just output read names that would be printed by iid\n"
    << "-M            Automatically consider mates\n"
    << "-N            Nullify mates in FRG if not printing both mate-pairs\n"
    << "-x IIDFile    File of list reads iids to exclude\n"
    << "-X EIDFile    File of list reads eids to exclude\n"
    << "\n"
    << "Notes:\n"
    << ". If there is no fragment bank, then it only prints read messages\n"
    << "\n"
    << ". By default, reads are considered independently so you can\n"
    << "  print a read, but not its mate. The -N option removes references\n"
    << "  to unprinted mate-pairs in the FRG messages.\n"
    << "\n"
    << ". In most cases, you should use -M to automatically select mates.\n"
    << "\n"
    << "You must specify included and/or excluded reads. Precedence is:\n"
    << "1) If a read is excluded by -X or -x, it will not be printed\n"
    << "  (-M) Mates of excluded reads will not be printed either\n"
    << "\n"
    << "2) If a read is included by -I or -i, it will be printed except if excluded\n"
    << "  (-M) Mates of included reads will be printed, except if either is excluded\n"
    << "       (Included Reads or mates may be clobbered by excluded reads or mates)\n"
    << "\n"
    << "3) If no reads are included, all reads except those excluded will be printed\n"
    << "\n"
    << "Exclusion takes precedence over inclusion!\n";

  return;
}




void PrintUsage (const char * s)
{
  cerr
    << "\nUSAGE: " << s << " [options] <bank path>\n\n";
  return;
}
