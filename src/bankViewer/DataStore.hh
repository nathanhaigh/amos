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


  char * fetchTrace(const AMOS::Read_t & read);

  void fetchScaffold(AMOS::ID_t scaffid, AMOS::Scaffold_t & scaff);
  void fetchContig(AMOS::ID_t contigid, AMOS::Contig_t & contig);
  void fetchFrag(AMOS::ID_t fragid, AMOS::Fragment_t & frag);
  void fetchRead(AMOS::ID_t readid, AMOS::Read_t & read);

  AMOS::Distribution_t getLibrarySize(AMOS::ID_t libid);
  AMOS::ID_t getLibrary(AMOS::ID_t readid);
  AMOS::ID_t lookupContigId(AMOS::ID_t readid);
  AMOS::ID_t lookupScaffoldId(AMOS::ID_t readid);

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

  AMOS::ID_t m_contigId;
  AMOS::ID_t m_scaffoldId;
  bool m_loaded;

  AMOS::Contig_t m_contig;

  typedef HASHMAP::hash_map<AMOS::ID_t, pair<AMOS::ID_t, AMOS::MateType_t> > MateLookupMap;
  MateLookupMap m_readmatelookup;


  typedef HASHMAP::hash_map<AMOS::ID_t, AMOS::ID_t> IdLookup_t;
  IdLookup_t m_readcontiglookup;
  IdLookup_t m_fragliblookup;
  IdLookup_t m_readfraglookup;      
  IdLookup_t m_contigscafflookup;

  typedef HASHMAP::hash_map<AMOS::ID_t, AMOS::Distribution_t> LibLookup_t;
  LibLookup_t m_libdistributionlookup;

private:

  void indexMates();
  void indexFrags();
  void indexReads();
  void indexLibraries();
  void indexContigs();
  void indexScaffolds();
};


#endif
