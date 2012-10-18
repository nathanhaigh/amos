#ifndef CompositeNode_HH
#define CompositeNode_HH 1

#include <map>
#include "Graph.hh"
#include "Node.hh"

using namespace std;

/**
 * The <b>CompositeNode</b> class
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
class CompositeNode : public Node {

public:

  Graph* graph;

  CompositeNode(void* p_element) : Node(p_element) {
    graph = new Graph();
  }

  Graph* getGraph() const { return graph; }
  void setGraph(Graph* p_graph) { graph = p_graph; }


  
};

#endif // #ifndef CompositeNode_HH
