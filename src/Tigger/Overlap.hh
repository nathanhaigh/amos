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

  int ridA;  // A
  int ridB;  // B
  bool asuffix;
  bool bsuffix;
  char ori;  // I or N
  char type; // C, R, P, or S
  int ahang, bhang;

  void flip() {
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
