/*** includes ***/
#include "Edge.hh"

using namespace std;

Edge::Edge(void* p_element, bool p_directed) : element(p_element) {
  flags = 0;
  //  flags |= p_directed;
  hidden = false;
  color = "black";
}

void Edge::reverse() {
  INode* tmp = n1;
  n1 = n2;
  n2 = tmp;
}

void Edge::setNodes(INode *p_node1, INode *p_node2) {
  n1 = p_node1;
  n2 = p_node2;
}

INode** Edge::getNodes() const {
  INode** nodes = new INode*[2];
  nodes[0] = n1;
  nodes[1] = n2;
  
  return nodes;
}

INode* Edge::opposite(INode *p_node) const {
  if(n1 == p_node) {
    return n2;
  } else if(n2 == p_node) {
    return n1;
  } else { 
    // TODO throw exception
    return NULL;
  }
}



