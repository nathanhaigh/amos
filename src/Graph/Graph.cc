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

bool Graph::isDirected() {
  return directed;
}


bool Graph::contains(INode* p_node) {
  return ! (p_node->getHidden());
}

bool Graph::contains(IEdge* p_edge) {
  return ! (p_edge->getHidden());
}

// INode Methods
////////////////
// TODO: need to fix degree for subgraphes

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

list< IEdge* > Graph::incident_edges(INode* p_node) {
  list<IEdge *> edges;
  IEdge* edge;
  IEdgeIterator iter = p_node->out_edges_begin();

  for( ; iter != p_node->out_edges_end(); ++iter) {
    edge = (*iter).second;
    if(contains(edge)) {
      edges.push_back(edge);
    }
  }

  if(directed) {
    iter = p_node->in_edges_begin();
    for( ; iter != p_node->in_edges_end(); ++iter) {
      edge = (*iter).second;
      if(contains(edge)) {
	edges.push_back(edge);
      }
    }
  }

  return edges;
}


list< IEdge* > Graph::out_edges(INode* p_node) const {
  list<IEdge *> edges;
  IEdge* edge;

  if(directed) {
    IEdgeIterator iter = p_node->out_edges_begin();
    for( ; iter != p_node->out_edges_end(); ++iter) {
      edge = (*iter).second;
      edges.push_back(edge);
    }
  }

  return edges;
}

list< IEdge* > Graph::in_edges(INode* p_node) const {
  list<IEdge *> edges;
  IEdge* edge;

  if(directed) {
    IEdgeIterator iter = p_node->in_edges_begin();
    for( ; iter != p_node->in_edges_end(); ++iter) {
      edge = (*iter).second;
      edges.push_back(edge);
    }
  }

  return edges;
}

list< INode* > Graph::adjacent_nodes(INode* p_node) {
  list< INode* > nodes;
  IEdge* edge;
  IEdgeIterator iter = p_node->out_edges_begin();

  for( ; iter != p_node->out_edges_end(); ++iter) {
    edge = (*iter).second;
    if(contains(edge)) {
      nodes.push_back(opposite(p_node, edge));
    }
  }

  if(directed) {
    iter = p_node->in_edges_begin();
    for( ; iter != p_node->in_edges_end(); ++iter) {
      edge = (*iter).second;
      if(contains(edge)) {
	nodes.push_back(opposite(p_node, edge));
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
      edge = (*iter).second;
      nodes.push_back(opposite(p_node, edge));
    }
  }

  return nodes;
}

list< INode* > Graph::in_adjacent(INode* p_node) {
  list< INode* > nodes;
  //  IEdge* edge;
  IEdgeIterator iter = p_node->out_edges_begin();

  if(directed) {
    iter = p_node->in_edges_begin();
    for( ; iter != p_node->in_edges_end(); ++iter) {
      nodes.push_back(opposite(p_node, (*iter).second));
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


// CompositeNode* Graph::reduce(IEdge* p_e) {
//   // create composite node
//   CompositeNode* cn = new CompositeNode();

//   // need to add nodes to composite
//   INode* source = p_e->getSource();
//   INode* target = p_e->getTarget();

//   //

  

// }


INode* Graph::get_node(int p_key) {
  return nodes[p_key];
}

IEdge* Graph::get_edge(int p_key) {
  return edges[p_key];
}

// IEdge Methods
////////////////
IEdge* Graph::new_edge(INode* p_n1, INode* p_n2, void* p_element) {
  IEdge* e = (IEdge *) new Edge(p_element, directed);

  e->setSource(p_n1);
  e->setTarget(p_n2);
  e->setKey(keys++);

  edges[e->getKey()] = e;

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
    (*edgeIter).second->setFlags(0x0);
  }
}

void Graph::clear_flags() {
  clear_node_flags();
  clear_edge_flags();

}

void Graph::clear_node_flags() {
  for(INodeIterator nodeIter = nodes.begin(); nodeIter != nodes.end(); ++nodeIter) {
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
  for(INodeIterator nodeIter = nodes.begin(); nodeIter != nodes.end(); ++nodeIter) {
    n = (*nodeIter).second;
    if(contains(n)) {
      int key = (*nodeIter).first;
      dotOut << "  " <<  key << " [shape=house,orientation=270";
      dotOut << ", color=\"" << n->getColor() << "\"";
      dotOut << ",URL=\"" << key << ".html\"];" << endl;
    }
  }

  IEdgeIterator edgeIter = edges.begin();  
  INode* n1;
  INode* n2;
  IEdge* e;

  for( ; edgeIter != edges.end(); ++edgeIter) {
    e = (*edgeIter).second;
    if(contains(e)) {
      n1 = e->getSource();
      n2 = e->getTarget();
      
      dotOut << "  " << n1->getKey() << " -> " << n2->getKey();
      dotOut << " [label=\"" << e->getKey() << "\"";
      dotOut << ", color=\"" << e->getColor() << "\"";
      dotOut << "]; " << endl;

    }
  }

  cout << " end of create dot file " << endl;
  dotOut << endl;
  dotOut << "} " << endl;
}

