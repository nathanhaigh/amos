#ifndef Graph_HH
#define Graph_HH 1

#include <iostream>
#include <sstream>
#include "Edge.hh"
#include "Node.hh"

class Edge;
class Node;

typedef list< Edge* >::iterator EdgeIterator;
typedef list< Node* >::iterator NodeIterator;

using namespace std;


/**
 * The <b>Graph</b> class
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
class Graph {
  
public:
  
  /** <code> directed </code> is the graph directed */
  bool directed;

  /** <code> simple </code> is the graph simple, no loops or multi-edges */
  bool simple;
  
  /** linked list of edges */
  list< Edge* > edges;

  /** linked list of nodes */
  list< Node* > nodes;
  
  /** <code> name </code> of graph */
  string name;

  /** */
  int keys;
  
  Graph(string p_name="noname") : name(p_name) { 
    keys = 0;
    directed = false;
  }

  /**
   * output dot file for the graph
   */
  void createDotFile(string p_name);
  
  /**
   * create new Node
   */
  Node* new_node(void* p_element = NULL);

  int num_nodes() { return nodes.size(); }

  NodeIterator nodes_begin() { return nodes.begin(); }
  NodeIterator nodes_end() { return nodes.end(); }
  

  int degree(Node* p_node) const;
  int out_degree(Node* p_node) const;
  int in_degree(Node* p_node) const;
  
  list< Edge* > incident_edges(Node* p_node) const;
  list< Edge* > in_edges(Node* p_node) const;
  list< Edge* > out_edges(Node* p_node) const;

  Node aNode() { return *(nodes.begin()); }

  /**
   * create new Edge
   */
  Edge* new_edge(Node* p_n1, Node* p_n2, void* p_element = NULL);
  
  int num_edges() { return edges.size(); }
  EdgeIterator edges_begin() { return edges.begin(); }
  EdgeIterator edges_end() { return edges.end(); }

  Node* opposite(Node* p_node, Edge* p_edge);
  Node* source(Edge* p_edge);
  Node* target(Edge* p_edge);

  list< Node* > adjacent_nodes(Node* p_node);
  list< Node* > out_adjacent(Node* p_node);
  list< Node* > in_adjacent(Node* p_node);
};


#endif // #ifndef Graph_HH
