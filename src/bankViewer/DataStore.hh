#ifndef DATA_STORE_HH_
#define DATA_STORE_HH_ 1

#include "foundation_AMOS.hh"
#include <string>

class DataStore
{
public:

  DataStore();

  int openBank(const string & bank_name);
  int setContigId(int id);

  AMOS::Distribution_t getLibrarySize(AMOS::ID_t readid);

  AMOS::Bank_t contig_bank;
  AMOS::Bank_t read_bank;
  AMOS::Bank_t frag_bank;
  AMOS::Bank_t lib_bank;
  AMOS::BankStream_t mate_bank;

  std::string m_db;
  std::string m_bankname;

  int m_contigId;
  bool m_loaded;

  AMOS::Contig_t m_contig;
};


#endif
