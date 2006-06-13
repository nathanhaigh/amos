#ifndef DATA_STORE_HH_
#define DATA_STORE_HH_ 1

#include "foundation_AMOS.hh"
#include <string>
#include <map>
#include <vector>

using std::string;
using std::map;
using std::pair;
using std::vector;

class Insert;

namespace __gnu_cxx
{
  template<> struct hash< long long unsigned >
  {
    size_t operator() (const long long unsigned x) const
    {
      return (x ^ ( x >> 32));
    }
  };
}




class DataStore
{
public:

  DataStore();
  ~DataStore();

  int openBank(const string & bank_name);
  int setContigId(int id);

  string replaceTraceTokens(const string & str,
                            const string & eid,
                            const string & iid);


  char * fetchTrace(const AMOS::Read_t & read, std::vector<int16_t> & positions);

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
  AMOS::BankStream_t scaffold_bank;
  AMOS::BankStream_t feat_bank;

  AMOS::BankStream_t edge_bank;
  AMOS::Bank_t       link_bank;

  std::string m_bankname;

  std::vector <string> m_tracepaths;
  std::string m_tracecache;
  std::string m_tracecmd;
  std::string m_tracecmdpath;
  std::string m_tracedb;
  bool m_tracecmdenabled;
  

  AMOS::ID_t m_contigId;
  AMOS::ID_t m_scaffoldId;
  bool m_loaded;

  AMOS::Contig_t m_contig;

  typedef HASHMAP::hash_map<AMOS::ID_t, pair<AMOS::ID_t, AMOS::FragmentType_t> > MateLookupMap;
  MateLookupMap m_readmatelookup;


  typedef HASHMAP::hash_map<AMOS::ID_t, AMOS::ID_t> IdLookup_t;
  IdLookup_t m_readcontiglookup;
  IdLookup_t m_fragliblookup;
  IdLookup_t m_readfraglookup;      
  IdLookup_t m_contigscafflookup;

  typedef HASHMAP::hash_map<AMOS::ID_t, AMOS::Distribution_t> LibLookup_t;
  LibLookup_t m_libdistributionlookup;

  AMOS::ID_t getPersistantRead(AMOS::ID_t readiid, int errorrate);

  void calculateInserts(vector<AMOS::Tile_t> & tiling, 
                        vector<Insert *> & inserts,
                        bool connectMates,
                        int verbose);

  void mapReadsToScaffold(AMOS::Scaffold_t & scaff,
                          vector<AMOS::Tile_t> & tiling,
                          int verbose);





  typedef  long long unsigned  Mer_t;
  typedef HASHMAP::hash_map<Mer_t, unsigned short> MerTable_t;

  MerTable_t mer_table;
  int        Kmer_Len;

  void Forward_Add_Ch (Mer_t & mer, char ch);
  void Reverse_Add_Ch (Mer_t & mer, char ch);
  void Read_Mers (const char * fname);
  void MerToAscii(Mer_t mer, string & s);
  void Fasta_To_Binary (const string & s, Mer_t & mer);
  unsigned int getMerCoverage(Mer_t fwd_mer, Mer_t rev_mer);


  Mer_t Forward_Mask;



private:

  void indexFrags();
  void indexReads();
  void indexLibraries();
  void indexContigs();
  void indexScaffolds();

  bool m_tracecachecreated;
};


#endif
