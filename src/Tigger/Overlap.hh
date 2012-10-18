#ifndef Overlap_HH
#define Overlap_HH 1

#include <iostream>

/**
 * The <b>Overlap</b> class
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
class Overlap {

public:

  int ridA, ridB;
  // ID of read A and B respectively

  bool asuffix, bsuffix;
  // asuffix: means that the 3' end (the suffix) of read A leads into read B
  // bsuffix: same thing as asuffix but for read B
  // Example:
  //   A: ---------->
  //          |||||||
  //   B:     ---------->
  //   asuffix is 1
  //   bsuffix is 0

  char ori;
  // Read orientation:
  //   N: normal  ---> --->
  //   I: innie   ---> <---

  char type;
  // Type of overlap: C, R, P or S
  //
  //   C: contained
  //      A: ---------------
  //             |||||||
  //      B:     -------
  //
  //   R: regular dovetail
  //      A: ---------->
  //             |||||||
  //      B:     ---------->
  //
  //   P: prefix dovetail
  //      A:     <----------
  //             |||||||
  //      B: ---------->
  //
  //   S: suffix dovetail
  //      A: ---------->
  //             |||||||
  //      B:     <----------
  //

  int ahang, bhang;
  // ahang: number of bp by which read A sticks out compared to B. Negative
  // values mean that A sticks in. Whether A or B are reverse-complemented or
  // not does not change anything.
  // bhang: same thing as ahang, but for read B
  //
  // Examples:
  //
  // A:  ACCAGCAAGTTGCCAATATTTTCTTAAATCGTCCATTTAAAAAAGATTTC
  // B:  ACCAGCAAGTTGCCAATATTTTCTTAAATCGTCCATTTAAAAAAGATTTCTGGGAAAAAA
  // ahang:0,   bhang:10
  //
  // A:  CTCGGAATCAACCAGCAAGTTGCTTAAATCGTCCATTTAAAAAAGATTTCTGGGAAAAAA
  // B:       AATCAACCAGCAAGTTGCTTAAATCGTCCATTTAAAAAAGATTTC
  // ahang:5,   bhang:-10

  void flip() {
    // Flip read A and read B: A becomes B and B becomes A
    bool tsuffix = asuffix;
    int ridT = ridA;
    ridA = ridB;
    asuffix = bsuffix;
    ridB = ridT;
    bsuffix = tsuffix;
    if((type == 'C') && (ori == 'I')) {
      int thang = ahang;
      ahang = bhang;
      bhang = thang;
    } else {
      bhang = -bhang;
      ahang = -ahang;
    }
  }

  void print() {
    std::cout << " read A " << ridA << " overlaps B " << ridB << " ori " << ori << std::endl;
    std::cout << " type " << type << " ahang " << ahang << " bhang " << bhang << std::endl;
  }


};

#endif // #ifndef Overlap_HH
