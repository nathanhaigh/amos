/*** includes ***/
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <list>
#include "Graph.hh"

using namespace std;

// Node Methods
////////////////

int Graph::degree(Node* p_node) const {
  return p_node->degree();
}

int Graph::out_degree(Node* p_node) const {
  return p_node->out_degree();
}

int Graph::in_degree(Node* p_node) const {
  return p_node->in_degree();
}

Node* Graph::opposite(Node* p_node, Edge* p_edge) {
  return p_edge->opposite(p_node);
}

Node* Graph::target(Edge* p_edge) {
  return p_edge->getTarget();
}

Node* Graph::source(Edge* p_edge) {
  return p_edge->getSource();
}

list< Edge* > Graph::incident_edges(Node* p_node) const {
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


list< Edge* > Graph::out_edges(Node* p_node) const {
  list<Edge *> edges;

  if(directed) {
    EdgeIterator iter = p_node->out_edges_begin();
    for( ; iter != p_node->out_edges_end(); ++iter) {
      edges.push_back(*iter);
    }
  }

  return edges;
}

list< Edge* > Graph::in_edges(Node* p_node) const {
  list<Edge *> edges;

  if(directed) {
    EdgeIterator iter = p_node->in_edges_begin();
    for( ; iter != p_node->in_edges_end(); ++iter) {
      edges.push_back(*iter);
    }
  }

  return edges;
}

list< Node* > Graph::adjacent_nodes(Node* p_node) {
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

list< Node* > Graph::out_adjacent(Node* p_node) {
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

list< Node* > Graph::in_adjacent(Node* p_node) {
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

Node* Graph::new_node(void* p_element) { 
  Node* n = new Node(p_element);
  n->setKey(keys++);
  nodes.push_back(n);
  return n;
}

// Edge Methods
////////////////
Edge* Graph::new_edge(Node* p_n1, Node* p_n2, void* p_element) {
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
void Graph::createDotFile(string p_fileName) {
  ofstream dotOut("test.dot");
  NodeIterator nodeIter = nodes.begin();
  
  dotOut << " digraph " << name << " {" << endl;
  
  dotOut << "  label=\"" << name << "\";" << endl;
  dotOut << "  URL=\"" << name << ".html\";" << endl;
  
  for( ; nodeIter != nodes.end(); ++nodeIter) {
    int key = (*nodeIter)->getKey();
    dotOut << "  " <<  key << " [shape=house,orientation=270,URL=\"";
    dotOut << key << ".html\"];" << endl;
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

