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


  void * fetchTrace(const AMOS::Read_t & read);

  void fetchRead(AMOS::ID_t readid, AMOS::Read_t & read);
  void fetchFrag(AMOS::ID_t fragid, AMOS::Fragment_t & frag);

  AMOS::Distribution_t getLibrarySize(AMOS::ID_t libid);
  AMOS::ID_t getLibrary(AMOS::ID_t readid);
  AMOS::ID_t lookupContigId(AMOS::ID_t readid);

  AMOS::BankStream_t contig_bank;
  AMOS::BankStream_t read_bank;
  AMOS::BankStream_t frag_bank;
  AMOS::BankStream_t lib_bank;
  AMOS::BankStream_t mate_bank;
  AMOS::BankStream_t scaffold_bank;

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


  typedef map<AMOS::ID_t, AMOS::ID_t> IdLookup_t;
  IdLookup_t m_readcontiglookup;
  IdLookup_t m_fragliblookup;
  IdLookup_t m_readfraglookup;      

  typedef map<AMOS::ID_t, AMOS::Distribution_t> LibLookup_t;
  LibLookup_t m_libdistributionlookup;

private:

  void indexMates();
  void indexFrags();
  void indexReads();
  void indexLibraries();
  void indexContigs();
};


#endif
