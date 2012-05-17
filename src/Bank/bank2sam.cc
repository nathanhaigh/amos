//  O. K. Tørresen
//
//  File:  bank2sam.cc
//
//  Last Modified:  10 May 2012
//
//  This program takes an AMOS bank directory and dumps its reads 
//  as SAM formatted text to stdout.

extern "C" {
#include <getopt.h>
}

#include "foundation_AMOS.hh"
#include "amp.hh"
#include "fasta.hh"
#include <map>
#include <sstream>
//#include <Contig_AMOS.hh>



using namespace AMOS;
using namespace std;

struct config {
  string        bank;
  bool			scaffolds;
  bool			use_eids;
};
config globals;

class Sam_entry
{
	private:
		string read_id; //QNAME
		int flag; //FLAG
		string r_name; //RNAME
		int pos_1; 		//POS 1-based, first position that map to the reference
		int mapqual; //MAPQ
		string cigar; //CIGAR
		string r_next; //RNEXT
		int p_next;  //PNEXT
		int t_len;  //TLEN
		string seq; //SEQ
		string qual; //QUAL
		
		bool rc; //If true, this read is reversed compared to contig direction, and 
				 //I presume it is the second read of a fragment
				
		int end_pos; // The last position that map to the reference
		
	public:
		Sam_entry()
		{
		}
		
		
		void setSam(int ctgoffset, Read_t read, Tile_t tile, Contig_t contig, string contigeid, string cons, string rnext, int pnext, int tlen)
		{
		
			// Set the entries we can 
			r_name = contigeid;
			//pos_1 = contig.gap2ungap(tile.offset);
			r_next = rnext;
			p_next = pnext;
			//Tlen goes from the start of the first read, to the end of the second.
			t_len = tlen;
			
			
			if (!globals.scaffolds) 
			{
				pos_1 = contig.gap2ungap(tile.offset);
				end_pos = tile.getRightOffset();
			}
			else
			{
				pos_1 = contig.gap2ungap(tile.offset) + ctgoffset;
				end_pos = tile.getRightOffset() + ctgoffset;
			}
			stringstream cigarstream;
			Range_t range = tile.range;
			rc = (range.begin > range.end);
		
			// render the sequence
			  
			//read_bank.fetch(read_id, read);
		
			string fullseq = read.getSeqString();
			string qualstr = read.getQualString();
		
			if (rc) { range.swap(); }
			string sequence = read.getSeqString(range);
			if (rc) { Reverse_Complement(sequence); }
		
			Pos_t gapcount = 0;
		
			vector<Pos_t>::const_iterator g;
			for (g  = tile.gaps.begin();
				g != tile.gaps.end();
				g++)
			{
				sequence.insert(*g+gapcount, "-", 1);
				gapcount++;
			}
		
			if (globals.use_eids) 
				read_id = read.getEID();
			//This uses IID as query template name
			else
			{
				stringstream out;
				out << tile.source;
				read_id = out.str();				
		    }
		
			flag = 0;
			// I'm not sure how I would set it to something else. Based on CIGAR?
			mapqual = 255;
		
			if (rc)
			{
				flag = 0x0010;
			}

		
			// now generate cigar string
		  
			int leftclip = range.begin;
			int rightclip = fullseq.length() - range.end;
		  
			if (rc)
			{
				int t = leftclip;
				leftclip = rightclip;
				rightclip = t;
			}
		
			if (leftclip)
			{
				cigarstream << leftclip << 'S';
			}
		  
			int pos = 0;
			int end = sequence.length();
		
			while (pos < end)
			{
				bool seqgap  = (sequence[pos] == '-');
				//bool consgap = (cons[pos+tile->offset] == '-');
				bool consgap = (cons[pos+tile.offset] == '-');
		
				int  len = 1;
				pos++;
		
				char type    = 'M'; // match
		
				if (seqgap && consgap) { type = 'P'; } // pad 
				else if (seqgap)       { type = 'D'; } // deletion 
				else if (consgap)      { type = 'I'; } // insertion

				while ((pos < end) &&
					((sequence[pos] == '-') == seqgap) &&
					((cons[pos+tile.offset] == '-') == consgap))
				{
					pos++;
					len++;
				}
				cigarstream << len << type;
			}
		
			if (rightclip)
			{
				cigarstream << rightclip << 'S';
			}
		  
		  	cigar = cigarstream.str();
		  
			// now sequence and qual
		
			if (rc) 
			{ 
				Reverse_Complement(fullseq); 
				reverse(qualstr.begin(), qualstr.end());
			}
		
			const int SAM_QUAL_BASE = 33;
		
			for (int i = 0; i < qualstr.length(); i++)
			{
				qualstr[i] = Char2Qual(qualstr[i]) + SAM_QUAL_BASE;
			}
			
			seq = fullseq;
			qual = qualstr;
		}

		
		//Default destructor.
		~Sam_entry()
		{
		}
		
		//Prints the content of the SAM entry to stdout
		void printSam()
		{
			cout << read_id << "\t" << flag << "\t" << r_name << "\t" << pos_1 << "\t" 
				 << mapqual << "\t" << cigar << "\t" << r_next << "\t" << p_next
				 << "\t" << t_len << "\t" << seq << "\t" << qual << endl;
				 
		}
		
		//If this read has a mate, this information might not be available at construction
		//This is used to update these fields. The read_id (QNAME) is set equal to the mate.
		void updateMate(int f, string qname, string rnext, int pnext, int tlen)
		{
			flag = f;
			read_id = qname;
			r_next = rnext;
			p_next = pnext;
			t_len = tlen;
		}
		
		bool getRC()
		{
			return rc;
		}
		
		string getRNAME()
		{
			return r_name;
		}
		
		string getQNAME()
		{
			//To be able to visualize in Tablet for example, the pairs need
			//to be named the same. 
			return read_id;
		}
		
		int getPos()
		{
			return pos_1;
		}
		
		int getEndPos()
		{
			return end_pos;
		}

		
		
};

//Reads that have a mate, but where some of the information from the
//mate is missing (like PNEXT, RNEXT), will be saved in this map
//until the mate is found and that information found.
map<ID_t, Sam_entry> mated_reads;
		
//Index_t read2contig;

//==============================================================================//
// Documentation
//==============================================================================//
void PrintHelp ()
{

          
  cerr << ".DESCRIPTION.\n"
       << "  This program takes an AMOS bank directory and dumps its reads \n"
       << "  as SAM formatted text to stdout.\n\n"
       << ".OPTIONS.\n"
       << "  -h          Display help information\n"
       << "  -b <bank>   The bank to be operated on. \n"
       << "  -c          Use contigs as reference\n"
       << "  -s          Use scaffolds as reference\n"
       << "  -i          Use IIDs as query template name (EIDs is default.) \n"
       << ".KEYWORDS.\n"
       << "  converters, bank, contigs\n\n"
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
	globals.scaffolds = false;
	globals.use_eids = true;
	
  while (1)
    {
      int ch, option_index = 0;
      static struct option long_options[] = {
        {"help",      no_argument,      &help_flag, 1},
        {"list",      no_argument,      &listed_flag, 1},
        {"bank",      required_argument,         0, 'b'},
        {"scaffold", no_argument,       0, 'c'},
        {"iids", 	no_argument,      	 0, 'i'},
        {"contigs",   no_argument,         0, 's'},
        {0,           0,                         0, 0}
      };
      
      ch = getopt_long(argc, argv, "hb:cis", long_options, &option_index);
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
        case 'c':
          globals.scaffolds = false;
          break;
        case 'i':
          globals.use_eids = false;
          break;
        case 's':
          globals.scaffolds = true;
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

//c_offset is the contig's offset on a scaffold, if we want scaffolds as RNAME
void printContig(int c_offset, string scaffeid, Contig_t & contig, Bank_t & read_bank, Bank_t & frag_bank)
{
  Read_t read;
  ID_t read_id, mate_id = 0;
  string mate_rname, mate_qname;
  Fragment_t fragment;
  //bool mate_rc;
  int mate_pos;
  int tlen = 0;
  int r_flag = 0, m_flag = 0;
  map<ID_t, Sam_entry>::iterator mate_it;
  std::vector<Tile_t> & tiling = contig.getReadTiling();
  sort(tiling.begin(), tiling.end(), TileOrderCmp());

	string contigeid;
	//If using scaffolds as RNAME
	if (globals.scaffolds)
		contigeid = scaffeid;
	else
	{
		// get the contig name
		contigeid = contig.getEID();
		if (contigeid.empty())
		{
			stringstream out;
			out << contig.getIID();
			contigeid = out.str();
		}
	}


    const string cons = contig.getSeqString();

    // convert each read in the contig
    vector<Tile_t>::const_iterator ti;
    for (ti =  tiling.begin();
         ti != tiling.end();
         ti++)
    {
		read_id = ti->source;
	  	Sam_entry sam_e;
      
    	read_bank.fetch(read_id, read);
    	//If a read has a fragment IID == 0, it's a unitig.
		if (read.getFragment() != 0) 
		{
      			
			frag_bank.fetch(read.getFragment(), fragment);
			if (fragment.getMatePair().first == read_id)
			{
				mate_id = fragment.getMatePair().second;
			}
			else if (fragment.getMatePair().second == read_id)
			{
				mate_id = fragment.getMatePair().first; 
			}
					
			// Not paired read, create SAM entry and print
			if (mate_id == 0)
			{
				sam_e.setSam(c_offset, read, *ti, contig, contigeid, cons, "*", 0, 0);
				sam_e.printSam();
			}
			// Mated read
			else 
			{
				// See if the mate of the read has already been found
				mate_it = mated_reads.find(mate_id);
			
				// Found the mate, create SAM entry and update and print both
				if (mate_it != mated_reads.end()) 
				{
					sam_e.setSam(c_offset, read, *ti, contig, contigeid, cons, "*", 0, 0);
					
					//need contigeid, if the mate is reversed or not and pos
					//to calculate tlen if contigeid is the same
					mate_rname = (*mate_it).second.getRNAME();
					mate_pos = (*mate_it).second.getPos();
					mate_qname = (*mate_it).second.getQNAME();
					// on the same reference
					if (mate_rname == contigeid) 
					{
						// If mate is reversed, I assume it's the second read
						// of a template, and the furthest from the start
						// of the contig
						if ((*mate_it).second.getRC()) 
						{
							tlen = (*mate_it).second.getEndPos() - sam_e.getPos();
							m_flag = 0x1 + 0x2 + 0x10 + 0x80;
							r_flag = 0x1 + 0x2 + 0x20 + 0x40;
							//Update the read with the query template name of mate
							sam_e.updateMate(r_flag, mate_qname, "=", mate_pos, tlen);
							(*mate_it).second.updateMate(m_flag, mate_qname, "=", sam_e.getPos(), -tlen);
							sam_e.printSam();
							(*mate_it).second.printSam();
						}
						else 
						{	
							//sam_e is the furthest from the start of the contig
							tlen = sam_e.getEndPos() - mate_pos;
							r_flag = 0x1 + 0x2 + 0x10 + 0x80;
							m_flag = 0x1 + 0x2 + 0x20 + 0x40;
							//Update the read with the query template name of mate
							sam_e.updateMate(r_flag, mate_qname, "=", mate_pos, -tlen);
							(*mate_it).second.updateMate(m_flag, mate_qname, "=", sam_e.getPos(), tlen);
							(*mate_it).second.printSam();
							sam_e.printSam();
						}	
						mated_reads.erase(mate_it);
					}
					// not on same reference
					else
					{
						if ((*mate_it).second.getRC()) 
						{
							m_flag = 0x1 + 0x2 + 0x10 + 0x80;
							r_flag = 0x1 + 0x2 + 0x20 + 0x40;
						}
						else 
						{
							r_flag = 0x1 + 0x2 + 0x10 + 0x80;
							m_flag = 0x1 + 0x2 + 0x20 + 0x40;
						}
						sam_e.updateMate(r_flag, mate_qname, mate_rname, mate_pos, 0);
						(*mate_it).second.updateMate(m_flag, mate_qname, contigeid, sam_e.getPos(), 0);
						sam_e.printSam();
						(*mate_it).second.printSam();
						mated_reads.erase(mate_it);
					}
				}
				// Paired read, create SAM entry and save
				else
				{
					sam_e.setSam(c_offset, read, *ti, contig, contigeid, cons, "*", 0, 0);
					mated_reads[read_id] = sam_e;
				}
			}
		}
		// Adding unitig as read.
        else
        {
        	sam_e.setSam(c_offset, read, *ti, contig, contigeid, cons, "*", 0, 0);
			sam_e.printSam();
        }
    }	
}

int main (int argc, char ** argv)
{

//TODO:
//First, get insert sizes and mates out. Done, works mostly at least.	
//Then also get scaffolds as reference. Done.
	GetOptions (argc, argv);
  
	if (globals.bank.length() > 0) {
		cerr << "Processing " << globals.bank << " at " << Date() << endl;
  
		Bank_t read_bank(Read_t::NCODE);
		Bank_t frag_bank(Fragment_t::NCODE);
		read_bank.open(globals.bank, B_READ);
		frag_bank.open(globals.bank, B_READ);
    	Contig_t contig;
    	
    	//Use contigs as reference (RNAME)
    	if(!globals.scaffolds) 
    	{
    		BankStream_t contig_bank(Contig_t::NCODE);
    		contig_bank.open(globals.bank, B_READ);
			
			while (contig_bank >> contig)
    		{
    			printContig(0, "", contig, read_bank, frag_bank);
    		}
    	}
    	// Use scaffolds as reference (RNAME)
    	else
		{
			BankStream_t scaffold_bank(Scaffold_t::NCODE);
			Bank_t contig_bank(Contig_t::NCODE);
	
			// Opens the scaffold and contig banks
			scaffold_bank.open(globals.bank, B_READ);
			contig_bank.open(globals.bank, B_READ);
			
			Scaffold_t scaffold;
			
			while (scaffold_bank >> scaffold)
			{
				string scaffeid = scaffold.getEID();
				if (scaffeid.empty())
				{
					stringstream out;
					out << scaffold.getIID();
					scaffeid = out.str();
				}
				// Might run into problems with negative gaps...
				vector<Tile_t> & contigs = scaffold.getContigTiling();
  				vector<Tile_t>::const_iterator ci;
  				sort(contigs.begin(), contigs.end(), TileOrderCmp());

  				for (ci = contigs.begin(); ci != contigs.end(); ci++)
  				{
  					contig_bank.fetch(ci->source, contig);
  					printContig(ci->offset, scaffeid, contig, read_bank, frag_bank);
  				}
  			}
  			scaffold_bank.close();
  			contig_bank.close();
		}
		// Need to check whether all reads have been printed or not
		while (!mated_reads.empty())
		{
			mated_reads.begin()->second.printSam();
			mated_reads.erase(mated_reads.begin());
		}

    	read_bank.close();
    	
    	frag_bank.close();
    }
	
	cerr << "End: " << Date() << endl;
  	return EXIT_SUCCESS;
}
