#ifndef CONTIG_HH
#define CONTIG_HH 1

#include "SubGraph.hh"

/**
 * The <b>Contig</b> class
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
class Contig {

public:

  SubGraph* sg;
  int start_node;

};

#endif // #ifndef CONTIG_HH
