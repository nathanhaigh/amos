#ifndef DATA_STORE_HH_
#define DATA_STORE_HH_ 1

#include "foundation_AMOS.hh"
#include <string>
#include <map>

using std::string;
using std::map;
using std::pair;

class DataStore
{
public:

  DataStore();

  int openBank(const string & bank_name);
  int setContigId(int id);

  void loadMates();
  void loadLibraries();
  void loadContigs();

  void * fetchTrace(const AMOS::Read_t & read);

  AMOS::Distribution_t getLibrarySize(AMOS::ID_t readid);
  AMOS::ID_t getLibrary(AMOS::ID_t readid);
  AMOS::ID_t lookupContigId(AMOS::ID_t readid);

  AMOS::BankStream_t contig_bank;
  AMOS::Bank_t       read_bank;
  AMOS::Bank_t       frag_bank;
  AMOS::BankStream_t lib_bank;
  AMOS::BankStream_t mate_bank;

  AMOS::BankStream_t edge_bank;
  AMOS::Bank_t       link_bank;

  std::string m_db;
  std::string m_bankname;

  std::vector <string> m_chromodbs;
  std::vector <string> m_chromopaths;

  int m_contigId;
  bool m_loaded;

  AMOS::Contig_t m_contig;

  typedef map<AMOS::ID_t, pair<AMOS::ID_t, AMOS::MateType_t> > MateLookupMap;

  MateLookupMap m_readmatelookup;
  map<AMOS::ID_t, AMOS::Distribution_t> m_libdistributionlookup;
  map<AMOS::ID_t, AMOS::ID_t> m_readcontiglookup;
};


#endif
