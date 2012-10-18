#include "foundation_AMOS.hh"
#include "amp.hh"
#include "fasta.hh"


using namespace AMOS;
using namespace std;


int main(int argc, char ** argv)
{
  if (argc != 4)
  {
    cerr << "Usage: extractContig bankname contigiid newbank" << endl;
    cerr << "Extracts selected contig and associated reads, fragments, and libraries" << endl
         << "and writes them to new bank" << endl;

    return EXIT_FAILURE;
  }

  Bank_t contig_bank(Contig_t::NCODE);
  Bank_t read_bank(Read_t::NCODE);
  Bank_t fragment_bank(Fragment_t::NCODE);
  Bank_t library_bank(Library_t::NCODE);

  Bank_t newcontig_bank(Contig_t::NCODE);
  Bank_t newread_bank(Read_t::NCODE);
  Bank_t newfragment_bank(Fragment_t::NCODE);
  Bank_t newlibrary_bank(Library_t::NCODE);

  string bank_name = argv[1];
  string contigid  = argv[2];
  string new_name  = argv[3];

  cerr << "Extracting contig " << contigid << " from " << bank_name
       << " into " << new_name << " at " << Date() << endl;

  try
  {
    contig_bank.open(bank_name,   B_READ);
    read_bank.open(bank_name,     B_READ);
    fragment_bank.open(bank_name, B_READ);
    library_bank.open(bank_name,  B_READ);

    if(!newcontig_bank.exists(new_name))  {newcontig_bank.create(new_name);  }
    if(!newread_bank.exists(new_name))    {newread_bank.create(new_name);    }
    if(!newfragment_bank.exists(new_name)){newfragment_bank.create(new_name);}
    if(!newlibrary_bank.exists(new_name)) {newlibrary_bank.create(new_name); }

    newcontig_bank.open(new_name);
    newread_bank.open(new_name);
    newfragment_bank.open(new_name);
    newlibrary_bank.open(new_name);

    Contig_t contig;
    Read_t read;
    Fragment_t fragment;
    Library_t library;

    contig_bank.fetch(atoi(contigid.c_str()), contig);

    const std::vector<Tile_t> & tiling = contig.getReadTiling();

    std::vector<Tile_t>::const_iterator ti;
    for (ti =  tiling.begin();
         ti != tiling.end();
         ti++)
    {
      read_bank.fetch(ti->source, read);

      if (!newread_bank.existsIID(read.getIID()))
      {
        fragment_bank.fetch(read.getFragment(), fragment);

        if (!newfragment_bank.existsIID(fragment.getIID()))
        {
          library_bank.fetch(fragment.getLibrary(), library);

          if (!newlibrary_bank.existsIID(library.getIID()))
          {
            newlibrary_bank.append(library);
          }

          newfragment_bank.append(fragment);

          ID_t mateid = 0;

          if (fragment.getMatePair().first == read.getIID())
          {
            mateid = fragment.getMatePair().second;
          }
          else if (fragment.getMatePair().second == read.getIID())
          {
            mateid = fragment.getMatePair().first;
          }

          if (mateid != 0)
          {
            Read_t mate;
            read_bank.fetch(mateid, mate);

            newread_bank.append(mate);
          }
        }

        newread_bank.append(read);
      }
    }

    if (!newcontig_bank.existsIID(contig.getIID()))
    {
      newcontig_bank.append(contig);
    }
  }
  catch (Exception_t & e)
  {
    cerr << "ERROR: -- Fatal AMOS Exception --\n" << e;
    return EXIT_FAILURE;
  }

  cerr << "End: " << Date() << endl;
  return EXIT_SUCCESS;
}

