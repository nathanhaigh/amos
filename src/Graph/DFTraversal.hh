#ifndef DFTraversal_HH
#define DFTraversal_HH 1

#include <iostream>
#include <sstream>
#include "IGraph.hh"

using namespace std;


/**
 * The <b>DFtraversal</b> class
 *
 *
 * <p>Copyright &copy; 2003, The Institute for Genomic Research (TIGR).
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
class DFtraversal {

  IGraph* g;
  IGraph* subg;

public:
  DFTraversal(IGraph* p_graph) : g(p_graph) {
    
  }

  void traverse(INode* n);
};


#endif // #ifndef DFTraversal_HH
