/*** includes ***/
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <list>
#include "Graph.hh"

using namespace std;

Graph::Graph(string p_name) : name(p_name) { 
  keys = 0;
  directed = false;
}

// INode Methods
////////////////

int Graph::degree(INode* p_node) const {
  return p_node->degree();
}

int Graph::out_degree(INode* p_node) const {
  return p_node->out_degree();
}

int Graph::in_degree(INode* p_node) const {
  return p_node->in_degree();
}

INode* Graph::opposite(INode* p_node, IEdge* p_edge) {
  return p_edge->opposite(p_node);
}

INode* Graph::target(IEdge* p_edge) {
  return p_edge->getTarget();
}

INode* Graph::source(IEdge* p_edge) {
  return p_edge->getSource();
}

list< IEdge* > Graph::incident_edges(INode* p_node) const {
  list<IEdge *> edges;
  IEdgeIterator iter = p_node->out_edges_begin();

  for( ; iter != p_node->out_edges_end(); ++iter) {
    if(! (*iter)->getHidden()) {
      edges.push_back(*iter);
    }
  }

  if(directed) {
    iter = p_node->in_edges_begin();
    for( ; iter != p_node->in_edges_end(); ++iter) {
      if(! (*iter)->getHidden()) {
	edges.push_back(*iter);
      }
    }
  }

  return edges;
}


list< IEdge* > Graph::out_edges(INode* p_node) const {
  list<IEdge *> edges;

  if(directed) {
    IEdgeIterator iter = p_node->out_edges_begin();
    for( ; iter != p_node->out_edges_end(); ++iter) {
      edges.push_back(*iter);
    }
  }

  return edges;
}

list< IEdge* > Graph::in_edges(INode* p_node) const {
  list<IEdge *> edges;

  if(directed) {
    IEdgeIterator iter = p_node->in_edges_begin();
    for( ; iter != p_node->in_edges_end(); ++iter) {
      edges.push_back(*iter);
    }
  }

  return edges;
}

list< INode* > Graph::adjacent_nodes(INode* p_node) {
  list< INode* > nodes;
  IEdge* edge;
  IEdgeIterator iter = p_node->out_edges_begin();

  for( ; iter != p_node->out_edges_end(); ++iter) {
    if(! (*iter)->getHidden()) {
      nodes.push_back(opposite(p_node, (*iter)));
    }
  }

  if(directed) {
    iter = p_node->in_edges_begin();
    for( ; iter != p_node->in_edges_end(); ++iter) {
      if(! (*iter)->getHidden()) {
	nodes.push_back(opposite(p_node, (*iter)));
      }
    }
  }

  return nodes;
}

list< INode* > Graph::out_adjacent(INode* p_node) {
  list< INode* > nodes;
  IEdge* edge;
  IEdgeIterator iter = p_node->out_edges_begin();

  if(directed) {
    for( ; iter != p_node->out_edges_end(); ++iter) {
      nodes.push_back(opposite(p_node, (*iter)));
    }
  }

  return nodes;
}

list< INode* > Graph::in_adjacent(INode* p_node) {
  list< INode* > nodes;
  IEdge* edge;
  IEdgeIterator iter = p_node->out_edges_begin();

  if(directed) {
    iter = p_node->in_edges_begin();
    for( ; iter != p_node->in_edges_end(); ++iter) {
      nodes.push_back(opposite(p_node, (*iter)));
    }
  }

  return nodes;
}

INode* Graph::new_node(void* p_element) { 
  return new_node(keys++, p_element);
}

INode* Graph::new_node(int p_key, void* p_element) { 
  INode* n = new Node(p_element);
  n->setKey(p_key);
  nodes[n->getKey()] = n;
  return n;
}

INode* Graph::get_node(int p_key) {
  return nodes[p_key];
}

// IEdge Methods
////////////////
IEdge* Graph::new_edge(INode* p_n1, INode* p_n2, void* p_element) {
  IEdge* e = (IEdge *) new Edge(p_element, directed);
  edges.push_back(e);
  
  e->setSource(p_n1);
  e->setTarget(p_n2);
  e->setKey(keys++);

  p_n1->add_oedge(e);

  if(directed) {
    p_n2->add_iedge(e);
  } else {
    p_n2->add_oedge(e);
  }
  
  return e;
}

void Graph::clear_edge_flags() {
  IEdgeIterator edgeIter = edges.begin();  
  for( ; edgeIter != edges.end(); ++edgeIter) {
    (*edgeIter)->setFlags(0x0);
  }
}

void Graph::clear_flags() {
  for(PairIterator nodeIter = nodes.begin(); nodeIter != nodes.end(); ++nodeIter) {
    (*nodeIter).second->setFlags(0x0);
  }

  clear_edge_flags();

}

/**
 *
 */
void Graph::create_dot_file(const char* p_filename) {
  ofstream dotOut(p_filename);
  
  dotOut << " digraph " << name << " {" << endl;
  
  dotOut << "  label=\"" << name << "\";" << endl;
  dotOut << "  URL=\"" << name << ".html\";" << endl;
  
  INode* n;
  for(PairIterator nodeIter = nodes.begin(); nodeIter != nodes.end(); ++nodeIter) {
    n = (*nodeIter).second;
    if(! n->getHidden()) {
      int key = (*nodeIter).first;
      dotOut << "  " <<  key << " [shape=house,orientation=270";
      dotOut << ", color=" << n->getColor();
      dotOut << ",URL=\"" << key << ".html\"];" << endl;
    }
  }

  IEdgeIterator edgeIter = edges.begin();  
  INode* n1;
  INode* n2;
  IEdge* e;

  for( ; edgeIter != edges.end(); ++edgeIter) {
    e = (*edgeIter);
    if(! e->getHidden() ) {
      n1 = e->getSource();
      n2 = e->getTarget();
      
      dotOut << "  " << n1->getKey() << " -> " << n2->getKey();
      dotOut << " [label=\"" << e->getKey() << "\"";
      dotOut << ", color=" << e->getColor();
      dotOut << "]; " << endl;

    }
  }

  cout << " end of create dot file " << endl;
  dotOut << endl;
  dotOut << "} " << endl;
}

