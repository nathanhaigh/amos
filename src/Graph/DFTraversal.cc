/*** includes ***/
#include "IGraph.hh"
#include "DFTraversal.hh"

using namespace std;

void DFTraversal::traverse(Node* p_node) const {
  list< Node* > nodes = g.adjacent_nodes(p_node);
  Node* node;

  for(NodeIterator iter = nodes.begin(); iter != nodes.end(); ++iter) {
    node = (*iter);
    traverse(node);
  }
  
}
