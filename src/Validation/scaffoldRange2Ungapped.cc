//scaffoldRange2Ungapped.cc
//Allison Regier
//The program translates a list of scaffold ranges into the
//equivalent ungapped ranges.
//Actually, it is only an approximation because this program
//makes the assumption that contigs within a scaffold
//do not overlap (or at least that the overlapping regions
//do not contain any gaps).  This may not always be
//a valid assumption.
//The basic approach is to count the number of gaps that occur in contigs before
//the begin coordinate of the gapped range, and the number of gaps that occur in contigs
//that fall between the begin and end coordinates of the range.  We can subtract the number
//of gaps from the gapped range, which will give us the ungapped coordinates.

#include "foundation_AMOS.hh"
#include <map>
#include <getopt.h>
using namespace std;
using namespace AMOS;

#define endl "\n"

map<string, string> globals;

void printHelpText()
{
  cerr <<
	"\n"
	".NAME.\n"
	"scaffoldRange2Ungapped - translates a list of gapped scaffold coordinates to ungapped coordinates\n"
        "\n.USAGE.\n"
        "-h, -help	print out help mesesage\n"
	"-b, -bank      bank where assembly is stored\n"
	"-f  -file      file containing list of gapped coordinates in the following form, one per line:\n"
                        "<scaffold_eid> <begin> <end>\n";
}

bool GetOptions(int argc, char ** argv)
{
	static struct option long_options[] =
	{
		{"help", 0, 0, 'h'},
		{"h", 0, 0, 'h'},
		{"b", 1, 0, 'b'},
		{"bank", 1, 0, 'b'},
		{"file", 1, 0, 'f'},
		{"f", 1, 0, 'f'}
	};
	int option_index = 0;	
	int c;
	while ((c = getopt_long_only(argc, argv, "", long_options, &option_index)) != -1)
	{
		switch (c)
		{
			case 'h':
				printHelpText();
				exit(0);
				break;
			case 'b':
				globals["bank"] = string(optarg);
				cout << globals["bank"] << endl;
				break;
			case 'f':
				globals["file"] = string(optarg);
				cout << globals["file"] << endl;
				break;
		}
	}
	return true;	
}

int main(int argc, char **argv)
{
	if (! GetOptions(argc, argv))
	{
		cerr << "Command line parsing failed" << endl;
		printHelpText();
		exit(1);
	}

	if (globals.find("bank") == globals.end())
	{
		cerr << "A bank must be specified" << endl;
		exit(1);
	}
	
	//Open scaffold bank
	Bank_t scaffold_bank (Scaffold_t::NCODE);

	if (! scaffold_bank.exists(globals["bank"]))
	{
		cerr << "No scaffold account found in bank " << globals["bank"] << endl;
		exit(1);
	}
	try
	{
		scaffold_bank.open(globals["bank"], B_READ);
	}
	catch (Exception_t & e)
	{
		cerr << "Failed to open scaffold account in bank " << globals["bank"]
		     << ": " << endl << e << endl;
		exit(1);
	}

	//Open contig bank
	Bank_t contig_bank (Contig_t::NCODE);
	if (! contig_bank.exists(globals["bank"]))
	{
		cerr << "No contig account found in bank " << globals["bank"] << endl;
		exit(1);
	}
	try
	{
		contig_bank.open(globals["bank"], B_READ);
	}
	catch (Exception_t & e)
	{
		cerr << "Failed to open contig account in bank " << globals["bank"]
		     << ": " << endl << e << endl;
		exit(1);
	}

	//Open input file.  One entry per line, in the form:
	//<scaffold_eid> <begin> <end>
	ifstream rangeFile;
	rangeFile.open(globals["file"].c_str(), ifstream::in);
	if (! rangeFile.is_open())
	{
		cerr << "Failed to open input range file " << globals["file"] << endl;
		exit(1);
	}
	string scaffEID;
	rangeFile >> scaffEID;
	while (rangeFile) //for each entry in the file
	{
		int rangeBegin;
		int rangeEnd;
		rangeFile >> rangeBegin;
		rangeFile >> rangeEnd;
	
		int total1 = 0; //total number of gaps in the scaffold before the begin coordinate
		int total2 = 0; //total number of gaps in the scaffold after the begin coordinate but
                                //before the end coordinate
		Scaffold_t scaffold; 
		scaffold_bank.fetch(scaffEID, scaffold);
		vector<Tile_t> tiles = scaffold.getContigTiling();
		//Look through all of the contigs in the scaffold.
		for (vector<Tile_t>::iterator ti = tiles.begin(); ti != tiles.end(); ti++)
		{
			AMOS::ID_t contigIID = ti->source;
			int offset = ti->offset;
			Contig_t contig;
			contig_bank.fetch(contigIID, contig);
			int contigRangeBegin = ti->range.getLo();
			int contigRangeEnd = ti->range.getHi();
			if (offset + contigRangeEnd <= rangeBegin)
			{
				//The contig is completely before our range on the scaffold
				//Therefore, all gaps in this contig are before the begin coordinate
				total1 = total1 + contig.getLength() - contig.getUngappedLength();
			}
			else if (offset <= rangeBegin &&
				 offset + contigRangeEnd > rangeBegin &&
				 offset + contigRangeEnd <= rangeEnd)
			{
				//The contig is partially below and partially inside our range on the scaffold
				//Some of the gaps in the contig are before the begin coordinate, and some are
				//between the begin and end coordinates.  We need to separate them out.
				Pos_t ungappedBeginOfRange = contig.gap2ungap(rangeBegin - offset - 1);
				int gapsBelowRange = rangeBegin - offset - 1 - ungappedBeginOfRange;
				total1 = total1 + gapsBelowRange;
				int gapsWithinRange = contig.getLength() - contig.getUngappedLength() - gapsBelowRange;
				total2 = total2 + gapsWithinRange;
			}
			else if (offset > rangeBegin &&
				 offset + contigRangeEnd <= rangeEnd)
			{
				//The contig is completely inside our range on the scaffold
				//All gaps in this contig are between the begin and end coordinates
				total2 = total2 + contig.getLength() - contig.getUngappedLength();
			}
			else if (offset <= rangeEnd &&
				 offset > rangeBegin &&
				 offset + contigRangeEnd > rangeEnd)
			{
				//The contig is partially inside and partially above our range
				//Some gaps in the contig fall beteen the begin and end coordinates
				//We need to separate them out.  The gaps that fall after our end
				//coordinate do not concern us.
				Pos_t ungappedEndOfRange = contig.gap2ungap(rangeEnd - offset - 1);
				int gapsWithinRange = rangeEnd - offset - 1 - ungappedEndOfRange;
				total2 = total2 + gapsWithinRange;
			}
			else if (offset <= rangeBegin &&
				 offset + contigRangeEnd > rangeEnd)
			{
				//Our range is completely inside the contig
				//Some gaps in the contig will come before our begin coordinate.
				//Some will come between the begin and end coordinates.  Some
				//will come after the end coordinate, so we need to separate them all out.
				Pos_t ungappedBeginOfRange = contig.gap2ungap(rangeBegin - offset - 1);
				int gapsBelowRange = rangeBegin - offset - 1 - ungappedBeginOfRange;
				total1 = total1 + gapsBelowRange;
				Pos_t ungappedEndOfRange = contig.gap2ungap(rangeEnd - offset - 1);
				int gapsWithinRange = rangeEnd - offset - 1 - ungappedEndOfRange - gapsBelowRange;
				total2 = total2 + gapsWithinRange;
			}
			else
			{
				//Do nothing, because the contig comes after our range in the scaffold.
				//All gaps in this contig come after our range in the scaffold.
			}
		}
		rangeBegin = rangeBegin - total1; //Adjust the begin coordinate based on gaps that come before it
		rangeEnd = rangeEnd - total1 - total2; //Adjust the end coordinate based on gaps that come before it
		cout << scaffEID << " " << rangeBegin << " " << rangeEnd << endl;
		rangeFile >> scaffEID; //Read in the next entry
	}
}
