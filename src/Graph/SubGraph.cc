/*** includes ***/
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <list>
#include "SubGraph.hh"

using namespace std;

// Node Methods
////////////////

inline int SubGraph::degree(Node* p_node) const {
  return p_node->degree();
}

inline int SubGraph::out_degree(Node* p_node) const {
  return p_node->out_degree();
}

inline int SubGraph::in_degree(Node* p_node) const {
  return p_node->in_degree();
}

Node* SubGraph::opposite(Node* p_node, Edge* p_edge) {
  return p_edge->opposite(p_node);
}

Node* SubGraph::target(Edge* p_edge) {
  return p_edge->target();
}

Node* SubGraph::source(Edge* p_edge) {
  return p_edge->source();
}

list< Edge* > SubGraph::incident_edges(Node* p_node) const {
  list<Edge *> edges;
  EdgeIterator iter = p_node->out_edges_begin();

  for( ; iter != p_node->out_edges_end(); ++iter) {
    edges.push_back(*iter);
  }

  if(directed) {
    iter = p_node->in_edges_begin();
    for( ; iter != p_node->in_edges_end(); ++iter) {
      edges.push_back(*iter);
    }
  }

  return edges;
}


list< Edge* > SubGraph::out_edges(Node* p_node) const {
  list<Edge *> edges;

  if(directed) {
    EdgeIterator iter = p_node->out_edges_begin();
    for( ; iter != p_node->out_edges_end(); ++iter) {
      edges.push_back(*iter);
    }
  }

  return edges;
}

list< Edge* > SubGraph::in_edges(Node* p_node) const {
  list<Edge *> edges;

  if(directed) {
    EdgeIterator iter = p_node->in_edges_begin();
    for( ; iter != p_node->in_edges_end(); ++iter) {
      edges.push_back(*iter);
    }
  }

  return edges;
}

list< Node* > SubGraph::adjacent_nodes(Node* p_node) {
  list< Node* > nodes;
  Edge* edge;
  EdgeIterator iter = p_node->out_edges_begin();

  for( ; iter != p_node->out_edges_end(); ++iter) {
    nodes.push_back(opposite(p_node, (*iter)));
  }

  if(directed) {
    iter = p_node->in_edges_begin();
    for( ; iter != p_node->in_edges_end(); ++iter) {
      nodes.push_back(opposite(p_node, (*iter)));
    }
  }

  return nodes;
}

list< Node* > SubGraph::out_adjacent(Node* p_node) {
  list< Node* > nodes;
  Edge* edge;
  EdgeIterator iter = p_node->out_edges_begin();

  if(directed) {
    for( ; iter != p_node->out_edges_end(); ++iter) {
      nodes.push_back(opposite(p_node, (*iter)));
    }
  }

  return nodes;
}

list< Node* > SubGraph::in_adjacent(Node* p_node) {
  list< Node* > nodes;
  Edge* edge;
  EdgeIterator iter = p_node->out_edges_begin();

  if(directed) {
    iter = p_node->in_edges_begin();
    for( ; iter != p_node->in_edges_end(); ++iter) {
      nodes.push_back(opposite(p_node, (*iter)));
    }
  }

  return nodes;
}

Node* SubGraph::new_node(void* p_element = NULL) { 
  Node* n = new Node(p_element);
  n->setKey(keys++);
  nodes.push_back(n);
  return n;
}

// Edge Methods
////////////////
Edge* SubGraph::new_edge(Node* p_n1, Node* p_n2, void* p_element =NULL) {
  Edge* e = new Edge(p_element, directed);
  edges.push_back(e);
  
  e->setNodes(p_n1, p_n2);
  e->setKey(keys++);

  p_n1->add_oedge(e);

  if(directed) {
    p_n2->add_iedge(e);
  } else {
    p_n2->add_oedge(e);
  }
  
  return e;
}

/**
 *
 */
void SubGraph::createDotFile(string p_fileName) {
  ofstream dotOut("test.dot");
  NodeIterator nodeIter = nodes.begin();
  
  dotOut << " digraph " << name << " {" << endl;
  
  dotOut << "  label=\"" << name << "\";" << endl;
  dotOut << "  URL=\"" << name << ".html\";" << endl;
  
  for( ; nodeIter != nodes.end(); ++nodeIter) {
    dotOut << "  " << (*nodeIter)->getKey() << " [shape=house,orientation=270,URL=\"";
    dotOut << (*nodeIter)->getKey() + ".html\"];" << endl;
  }

  EdgeIterator edgeIter = edges.begin();  
  Node* n1;
  Node* n2;
  Edge* e;

  for( ; edgeIter != edges.end(); ++edgeIter) {
    e = (*edgeIter);
    n1 = e->getSource();
    n2 = e->getTarget();

    dotOut << "  " << n1->getKey() << " -> " << n2->getKey() << " [label=\"" << e->getKey() << "\"]; " << endl;

  }

  dotOut << endl;
  dotOut << "} " << endl;
}


