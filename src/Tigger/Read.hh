#ifndef Read_HH
#define Read_HH 1

#include "foundation_AMOS.hh"
#include <iostream>

/**
 * The <b>Read</b> class
 *
 *
 * <p>Copyright &copy; 2004, The Institute for Genomic Research (TIGR).
 * <br>All rights reserved.
 *
 * @author  Dan Sommer
 *
 * <pre>
 * $RCSfile$
 * $Revision$
 * $Date$
 * $Author$
 * </pre>
 */
class Read {
public:
  int id;    // read ID
  int len;   // length of read clear range
  int start; // start position of read in layout or contig
  int end;   // end position of read in layout or contig
  AMOS::Range_t range;

  Read(int p_id, int p_len, int p_start = -1, int p_end = -1) 
    : id(p_id), len(p_len), start(p_start), end(p_end) {  }

  void print() {
    std::cout << " read " << id << " len " << len << " LAY (" << start << ", " << end << ")" << std::endl;
  }

};

#endif // #ifndef Read_HH
