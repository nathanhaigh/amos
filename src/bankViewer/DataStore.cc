#include "DataStore.hh"

using namespace AMOS;

DataStore::DataStore()
  : contig_bank(Contig_t::NCODE),
    read_bank(Read_t::NCODE),
    frag_bank(Fragment_t::NCODE),
    lib_bank(Library_t::NCODE),
    mate_bank(Matepair_t::NCODE)
{
  m_contigId = 1;
  m_loaded = false;

}

int DataStore::openBank(const string & bankname)
{
  int retval = 0;

  try
  {
    read_bank.open(bankname);
    contig_bank.open(bankname);
    mate_bank.open(bankname);
    frag_bank.open(bankname);
    lib_bank.open(bankname);

    m_bankname = bankname;
  }
  catch (Exception_t & e)
  {
    cerr << "ERROR: -- Fatal AMOS Exception --\n" << e;
    retval = 1;
  }

  return retval;
}

int DataStore::setContigId(int id)
{
  int retval = 0;

  try
  {
    contig_bank.fetch(id, m_contig);
    m_contigId = id;
    m_loaded = true;
  }
  catch (AMOS::Exception_t & e)
  {
    cerr << "ERROR: -- Fatal AMOS Exception --\n" << e;
    retval = 1;
  }

  return retval;
}

AMOS::Distribution_t DataStore::getLibrarySize(ID_t readid)
{
  try
  {
    Read_t read;
    read_bank.fetch(readid, read);

    Fragment_t frag;
    frag_bank.fetch(read.getFragment(), frag);

    Library_t lib;
    lib_bank.fetch(frag.getLibrary(), lib);

    return lib.getDistribution();
  }
  catch (AMOS::Exception_t & e)
  {
    cerr << "ERROR: -- Fatal AMOS Exception --\n" << e;
  }

  return Distribution_t();
}

