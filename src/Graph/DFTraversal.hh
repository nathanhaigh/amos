#ifndef DFTraversal_HH
#define DFTraversal_HH 1

#include <iostream>
#include <sstream>
#include "IGraph.hh"


/**
 * The <b>DFtraversal</b> class
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
class DFTraversal {

  IGraph* g;
  IGraph* subg;

public:
  DFTraversal(IGraph* p_graph) : g(p_graph) {
    
  }

  void traverse(INode* n) const;
};


#endif // #ifndef DFTraversal_HH
