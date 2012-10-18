#include "foundation_AMOS.hh"
#include "amp.hh"

/*
 * Encode and store a 3 read contig. Write the object directly
 * into a bank, or into a message stream printed to stdout. 
 *
 * Original Reads: (Clear range indicated with [])
 * seq1: A[GCACCGGT]AAA
 * seq2: ACG[GATCAACT]TTAC
 * seq3: [ATAATT]CG
 *
 *
 * Multiple alignment:
 * 
 *                 0000000000011
 * Offset:         0123456789012
 *  
 * Consensus:      ACCGAT-CAACTT
 * 
 * seq1 <          ACCGGTGC.....
 * seq2 >          ...GAT-CAACT.
 * seq3 >          ....AT--AA-TT
 *
 */


using namespace AMOS;
using namespace std;

int main (int argc, char ** argv)
{
  if (argc != 2)
  {
    cerr << "Usage: bank-tutorial bankname/-" << endl
         << endl
         << "  Creates a 3 read contig and writes it directly into a binary bank" << endl
         << "  named bankname, or write to stdout if - is specified." << endl;
    return EXIT_FAILURE;
  }

  Message_t msg;
  BankStream_t contig_bank(Contig_t::NCODE);
  BankStream_t read_bank(Read_t::NCODE);
  string bank_name = argv[1];

  // Write to stdout if the bank_name is "-", otherwise write the binary
  // objects directly to a bank named bank_name.
  bool printmsg = (bank_name == "-");

  Read_t read; 
  Contig_t contig;
  Tile_t tile;
  vector<Tile_t> tiling;

  if (!printmsg)
  {
    cerr << "Creating " << bank_name << " at " << Date() << endl;
  }

  try
  {
    if (!printmsg)
    {
      // Create the banks, alternatively use bank.open() to open an existing bank
      read_bank.create(bank_name);
      contig_bank.create(bank_name);
    }

    // Internal IDs must be numeric (IID)
    read.setIID(1);

    // External IDs can be any string without newline (EID)
    read.setEID("seq1");

    // Quality values are stored as (numeric value) - '0'
    // For example 0 => '0', 10 => ':', 20 => 'D' , 30 => 'N', Max: 60
    // Here I set the quality values to be 1,2,3,...,12.
    read.setSequence("AGCACCGGTAAA", "123456789:;<");

    // Clear ranges are stored [s,e) with inclusive start, 
    // and exclusive end coordinates (0-based). In this case, base 1 (0-based)
    // is the first good base, and base 9 (0-based) is the first bad base.
    // The length of the clear range is therefore e-s.
    read.setClearRange(Range_t(1,9));

    // The objects can be written directly to a binary bank, 
    // or written to a message file to be loaded with bank-transact
    if (printmsg) { read.writeMessage(msg); msg.write(cout); }
    else          { read_bank << read; }
      

    // Objects can be reused, but be careful to reset every value or call obj.clear()!
    read.setIID(2);
    read.setEID("seq2");
    read.setSequence("ACGGATCAACTTTAC", "123456789:;<=>?");
    read.setClearRange(Range_t(3,11));
    if (printmsg) { read.writeMessage(msg); msg.write(cout); }
    else          { read_bank << read; }

    read.setIID(3);
    read.setEID("seq3");
    read.setSequence("ATAATTCG", "12345678");
    read.setClearRange(Range_t(0,6));
    if (printmsg) { read.writeMessage(msg); msg.write(cout); }
    else          { read_bank << read; }

    contig.setIID(1);
    contig.setEID("contig1");

    // Note consensus sequences are stored gapped, quality values
    // are required at each position (including gaps)
    contig.setSequence("ACCGAT-CAACTT", "XXXXXXXXXXXXX");

    // The read -> contig mapping is stored in tile objects.
    // The source field indicates the iid of the read
    tile.source = 1;
    
    // The offset is the 0-based position where the leftmost base of the read
    // tiles the leftmost base of the consensus
    tile.offset = 0;

    // Range indicates the actual range of the read used in the contig
    // If the numbers are reversed, it indicates the read was reverse
    // complemented, and is stored (e,s]. 
    tile.range = Range_t(9,1); 

    // There can be any number of reads, so they are stored in a vector
    tiling.push_back(tile);

    tile.source = 2;
    tile.offset = 3;
    tile.range = Range_t(3,11);
    
    // This indicates there should be 1 gap inserted into the read after 3
    // bases of the aligned clear range. Gaps for reversed reads are still stored
    // left to right (contig orientation).
    tile.gaps.push_back(3);
    tiling.push_back(tile);

    tile.gaps.clear();
    tile.source = 3;
    tile.offset = 4;
    tile.range = Range_t(0,6);

    // Adjacent gaps are stored with the same position.
    // The gap at 4 is still relative to the ungapped aligned clear range
    tile.gaps.push_back(2);
    tile.gaps.push_back(2);
    tile.gaps.push_back(4);
    tiling.push_back(tile);

    contig.setReadTiling(tiling);

    if (printmsg) { contig.writeMessage(msg); msg.write(cout); }
    else          { contig_bank << contig; }

    if (!printmsg)
    {
      // You should now be able to open the bank with bankViewer
      // and see the multiple alignment described above.
      read_bank.close();
      contig_bank.close();
    }
  }
  catch (Exception_t & e)
  {
    cerr << "ERROR: -- Fatal AMOS Exception --\n" << e;
    return EXIT_FAILURE;
  }

  if (!printmsg) { cerr << "End: " << Date() << endl; }
  return EXIT_SUCCESS;
}
