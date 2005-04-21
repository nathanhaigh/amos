/*** includes ***/
#include <iostream>
#include "SubGraph.hh"

using namespace HASHMAP;
using namespace std;

SubGraph::SubGraph(IGraph& p_parent, string p_name) : Graph(p_name), parent(p_parent) {
  directed = parent.isDirected();
}

bool SubGraph::contains(INode* p_node) {
  hash_map< int, INode* >::iterator n = nodes.find(p_node->getKey());
  if(n != nodes.end()) {
    return ! (p_node->getHidden());
  } else {
    return false;
  }
}

bool SubGraph::contains(IEdge* p_edge) {
  hash_map< int, IEdge* >::iterator e = edges.find(p_edge->getKey());
  if(e != edges.end()) {
    return ! (p_edge->getHidden());
  } else {
    return false;
  }
}

void SubGraph::add_node(INode* p_node) {
  nodes[p_node->getKey()] = p_node;
}

void SubGraph::add_edge(IEdge* p_edge) {
  INode* target = p_edge->getTarget();
  INode* source = p_edge->getSource();
  edges[p_edge->getKey()] = p_edge;
  nodes[source->getKey()] = source;
  nodes[target->getKey()] = target;
}


