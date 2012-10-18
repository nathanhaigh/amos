//  O. K. Tørresen
//
//  File:  extractScaffold.cc
//
//  Last Modified:  10 May 2012
//
//  This program takes an AMOS bank directory and extract the selected
//  scaffold into a new AMOS bank.


extern "C" {
#include <getopt.h>
}

#include "foundation_AMOS.hh"
#include "amp.hh"
#include "fasta.hh"
#include "Contig_AMOS.hh"

//#include <iostream>
#include <vector>




using namespace AMOS;
using namespace std;

struct config {
  string        bank;
  string        scaffoldid;
  string        newbank;
  bool			listscaff;
};
config globals;

// For Hawkeye to work, the scaffolds and contigs need to start from IID 1, 
// this is not the case for reads and fragments, so they are transferred with
// original IID.
//int minSeqID = 1;
int minCtgID = 1;
int minScfID = 1;

//==============================================================================//
// Function Prototypes
//==============================================================================//
void extract_scaffold(string old_bank, string scaffoldid, string new_bank);
void list_scaffolds(string old_bank);

//==============================================================================//
// Documentation
//==============================================================================//
void PrintHelp()
{
    cerr << "\n"
         << ".USAGE."
         << "  extractScaffold -b bank -s scaffoldiid -n newbank \n "
         << ".DESCRIPTION.\n"
		 << " Extracts selected scaffold and associated contigs, \n"
		 << " reads, fragments, and libraries and writes them to new bank \n"
         << ".OPTIONS. \n"
         << "  -b <bank> - The bank to be operated on. \n"
         << "  -s <scaffold id> - The IID of the scaffold the be extracted .\n"
         << "  -n <new bank> - The name of the new bank consisting of the.\n"
         << " scaffold, its contigs, reads, fragments and libraries. \n"
         << "  -l - list the scaffolds in the given bank to stdout. The order is \n"
         << " IID EID number_of_contigs number_of_bases span.\n"
         << ".KEYWORDS. \n"
         << "  extracter, amos format\n"
         << endl;
}

//----------------------------------------------------- GetOptions -----------//
//! \brief Sets the global OPT_% values from the command line arguments
//!
//! \return void
//!
//----------------------------------------------------------------------- ----//

static int help_flag;
static int listed_flag;

bool GetOptions (int argc, char ** argv)
{
	globals.listscaff = false;
	
  while (1)
    {
      int ch, option_index = 0;
      static struct option long_options[] = {
        {"help",      no_argument,      &help_flag, 1},
        {"list",      no_argument,      &listed_flag, 1},
        {"bank",      required_argument,         0, 'b'},
        {"scaffoldiid", required_argument,       0, 's'},
        {"newbank",   required_argument,         0, 'n'},
        {"listscaff", no_argument, 				 0, 'l'},
        {0,           0,                         0, 0}
      };
      
      ch = getopt_long(argc, argv, "hlb:s:n:", long_options, &option_index);
      if (ch == -1)
        break;

      string qualName;
      

      switch (ch)
        {
        case 0:
          /* If this option set a flag, do nothing else now. */
          if (long_options[option_index].flag != 0)
            break;
        case 'b':
          globals.bank = string(optarg);
          break;
        case 's':
          globals.scaffoldid = string(optarg);
          break;
        case 'n':
          globals.newbank = string(optarg);
          break;
        case 'l':
          globals.listscaff = true;
          break;	
       case 'h':
          PrintHelp();
          return (EXIT_SUCCESS);
        case '?':
          break;
        }
    }
  if (help_flag){
    PrintHelp();
    return (EXIT_SUCCESS);
  } 

  return true;
}

int main(int argc, char ** argv)
{
// TODO:
// Change: Use old IID for libraries. Done.
// Change: Fix mate pairing, losing that now. Mates are not added to tiling! Done.
// Add: Print all scaffolds, their IIDs, EIDs, and span. (Maybe contigs/reads too.) Done.
// Add: Transfer contig links and edges too.

  // Handle all initial parsing of hashmaps and temp files via options parsing
  if (! GetOptions (argc, argv)) {
    cerr << "Command line parsing failed" << endl;
    PrintHelp();
    exit(1);
  }
  
  if (globals.bank.length() > 0) {
  	if (globals.scaffoldid.length() > 0 && globals.newbank.length() > 0)
  		extract_scaffold(globals.bank, globals.scaffoldid, globals.newbank);
  	else if (globals.listscaff)
  		list_scaffolds(globals.bank);
  }
  else {
  	cerr << "Invalid combinations of options." << endl;
  	PrintHelp();
  }
}

void extract_scaffold(string old_bank, string scaffoldid, string new_bank) {

  Bank_t scaffold_bank(Scaffold_t::NCODE);
  Bank_t contig_bank(Contig_t::NCODE);
  //Bank_t contiglink_bank(ContigLink_t::NCODE);
  //Bank_t contigedge_bank(ContigEdge_t::NCODE);
  Bank_t read_bank(Read_t::NCODE);
  Bank_t fragment_bank(Fragment_t::NCODE);
  //Bank_t library_bank(Library_t::NCODE);
  BankStream_t library_bank(Library_t::NCODE);

  Bank_t newscaffold_bank(Scaffold_t::NCODE);
  Bank_t newcontig_bank(Contig_t::NCODE);
  //Bank_t newcontiglink_bank(ContigLink_t::NCODE);
  //Bank_t newcontigedge_bank(ContigEdge_t::NCODE);
  Bank_t newread_bank(Read_t::NCODE);
  Bank_t newfragment_bank(Fragment_t::NCODE);
  //Bank_t newlibrary_bank(Library_t::NCODE);
  BankStream_t newlibrary_bank(Library_t::NCODE);

  cerr << "Extracting scaffold " << scaffoldid << " from " << old_bank
       << " into " << new_bank << " at " << Date() << endl;

//  try
//  {
  	scaffold_bank.open(old_bank, B_READ);
  	contig_bank.open(old_bank,   B_READ);
    //contiglink_bank.open(old_bank,   B_READ);
    //contigedge_bank.open(old_bank,   B_READ);
    read_bank.open(old_bank,     B_READ);
    fragment_bank.open(old_bank, B_READ);
    library_bank.open(old_bank,  B_READ);
    


	if(!newscaffold_bank.exists(new_bank)){newscaffold_bank.create(new_bank);}
    if(!newcontig_bank.exists(new_bank))  {newcontig_bank.create(new_bank);  }
    //if(!newcontiglink_bank.exists(new_bank))  {newcontiglink_bank.create(new_bank);  }
    //if(!newcontigedge_bank.exists(new_bank))  {newcontigedge_bank.create(new_bank);  }
    if(!newread_bank.exists(new_bank))    {newread_bank.create(new_bank);    }
    if(!newfragment_bank.exists(new_bank)){newfragment_bank.create(new_bank);}
    if(!newlibrary_bank.exists(new_bank)) {newlibrary_bank.create(new_bank); }
    

	newscaffold_bank.open(new_bank);
    newcontig_bank.open(new_bank);
    //newcontiglink_bank.open(new_bank);
    //newcontigedge_bank.open(new_bank);
    newread_bank.open(new_bank);
    newfragment_bank.open(new_bank);
    newlibrary_bank.open(new_bank);

	Scaffold_t scaffold;
    Contig_t contig;
    //ContigLink_t contig_link;
    //ContigEdge_t contig_edge;
    Read_t read;
    Fragment_t fragment;	
    Library_t library;
    
    // I'll just stream all the libraries to the new bank at once
    // since they all are probably needed in the new bank, and it doesn't matter 
    // much if they all are included anyway.
    while (library_bank >> library)
    {
    	newlibrary_bank << library;
    }	
    
    scaffold_bank.fetch(atoi(scaffoldid.c_str()), scaffold);

	vector<Tile_t> & contigs = scaffold.getContigTiling();
	vector<Tile_t> new_contigs;
  	vector<Tile_t>::const_iterator ci;
	//vector<ID_t> & contig_edges = scaffold.getContigEdges();
  	//vector<ID_t>::const_iterator ce;


  	sort(contigs.begin(), contigs.end(), TileOrderCmp());
  	

	for (ci = contigs.begin(); ci != contigs.end(); ci++)
	{
		Tile_t ctg_tile = *ci;
  		contig_bank.fetch(ci->source, contig);
    	
		const std::vector<Tile_t> & tiling = contig.getReadTiling();
		
    	vector<Tile_t> new_reads;
    	std::vector<Tile_t>::const_iterator ti;
    	for (ti =  tiling.begin();
         ti != tiling.end();
         ti++)
    	{
    		Tile_t read_tile = *ti;

      		read_bank.fetch(ti->source, read);
      		ID_t read_id = read.getIID() ;
      		
      		
      		if (!newread_bank.existsIID(read_id))
      		{

      			//Got errors because several reads was member of fragment 0, which does not exist. That's because these 'reads' are unitigs.
      			if (read.getFragment() != 0) {
        			fragment_bank.fetch(read.getFragment(), fragment);

        			if (!newfragment_bank.existsIID(fragment.getIID()))
        			{

          				ID_t mateid = 0;

          				if (fragment.getMatePair().first == read_id)
          				{
            				mateid = fragment.getMatePair().second;
         				}
          				else if (fragment.getMatePair().second == read_id)
          				{
            				mateid = fragment.getMatePair().first; 
          				}
						
						// Paired read
          				if (mateid != 0)
          				{
            				Read_t mate;
            				read_bank.fetch(mateid, mate);

            				newread_bank.append(mate);
          				}

        			}
					// Update the new bank with fragment and read
					newfragment_bank.append(fragment);
        			newread_bank.append(read);
        			
      			} 
      			else {
      				// is a unitig
      				newread_bank.append(read);
      			}

    		}	
    	}
 

    	contig.setIID(minCtgID++);
    	newcontig_bank.append(contig);
    	ctg_tile.source = contig.getIID();
    	new_contigs.push_back(ctg_tile);
    	
  	} 

    scaffold.setIID(minScfID++);
    scaffold.setContigTiling(new_contigs);
    newscaffold_bank.append(scaffold);

//  }
  //catch (Exception_t & e)
  //{
  //  cerr << "ERROR: -- Fatal AMOS Exception --\n" << e;
  //  return EXIT_FAILURE;
  //}
  
  	scaffold_bank.close();
  	contig_bank.close();
    //contiglink_bank.close();
    //contigedge_bank.close();
    read_bank.close();
    fragment_bank.close();
    library_bank.close();
    
	newscaffold_bank.close();
    newcontig_bank.close();
    //newcontiglink_bank.close();
    //newcontigedge_bank.close();
    newread_bank.close();
    newfragment_bank.close();
    newlibrary_bank.close();  
  
  
  cerr << "End: " << Date() << endl;
  //return EXIT_SUCCESS;
} 


// Lists all scaffolds in a bank
// use with extractScaffold -b e_coli_ca_mp_bogart.bnk -l | sort -k5,5 -n
// for example, to get the largest scaffold (by span)
void list_scaffolds(string old_bank) 
{
	cerr << "Listing all scaffolds in the bank " << old_bank << " to stdout:" << endl;
	
	
	BankStream_t scaffold_bank(Scaffold_t::NCODE);
	Bank_t contig_bank(Contig_t::NCODE);
	
	//Opens the scaffold and contig banks
	scaffold_bank.open(old_bank, B_READ);
	contig_bank.open(old_bank, B_READ);
	
	Scaffold_t scaffold;
	
	//As long as there's scaffolds in the scaffold bank
	while (scaffold_bank >> scaffold)
	{
		vector<Tile_t> & contigs = scaffold.getContigTiling();
  		vector<Tile_t>::const_iterator ci;

  		sort(contigs.begin(), contigs.end(), TileOrderCmp());

		
  		int bases = 0, span = 0, numcontigs = contigs.size();

		// Finds the amount of bases in contigs (including gaps) and total span of the scaffold
  		for (ci = contigs.begin(); ci != contigs.end(); ci++)
  		{
    		bases += ci->getGappedLength();
    		int right = ci->getRightOffset();

    		if (right > span) { span = right; }
  		}
	
    	cout << scaffold.getIID() << "\t" << scaffold.getEID() << "\t" << numcontigs << "\t" << bases << "\t" << span << endl;
    }
	

	contig_bank.close();
	scaffold_bank.close();
	
}
