#ifndef Overlap_HH
#define Overlap_HH 1

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

  int rid1, rid2;
  char ori;
  int ahang, bhang;
  int alen, blen;
  int alin_score, errors;
  float percent;
};

#endif // #ifndef Overlap_HH
