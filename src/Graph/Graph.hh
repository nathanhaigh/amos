#ifndef Graph_HH
#define Graph_HH 1

#include <iostream>
#include <sstream>
#include <map>
#include <list>
#include "IGraph.hh"
#include "Node.hh"
#include "Edge.hh"

class IEdge;
class INode;

class Edge;
class Node;

typedef list< IEdge* >::iterator IEdgeIterator;
typedef list< INode* >::iterator INodeIterator;
typedef map< int, INode* >::iterator PairIterator;

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
class Graph : public IGraph {
  
public:
  
  /** <code> directed </code> is the graph directed */
  bool directed;

  /** <code> simple </code> is the graph simple, no loops or multi-edges */
  bool simple;
  
  /** map of edges */
  list< IEdge* > edges;

  /** map of nodes */
  map< int, INode* > nodes;
  
  /** <code> name </code> of graph */
  string name;

  /** */
  int keys;
  
  Graph(string p_name="noname");

  /**
   * output dot file for the graph
   */
  void create_dot_file(const char* p_filename);
  
  /**
   * create new INode
   */
  INode* new_node(int key, void* p_element = NULL);
  INode* new_node(void* p_element = NULL);

  INode* get_node(int p_key);

  int num_nodes() { return nodes.size(); }

  PairIterator nodes_begin() { return nodes.begin(); }
  PairIterator nodes_end() { return nodes.end(); }
  
  void clear_flags();
  int degree(INode* p_node) const;
  int out_degree(INode* p_node) const;
  int in_degree(INode* p_node) const;
  
  list< IEdge* > incident_edges(INode* p_node) const;
  list< IEdge* > in_edges(INode* p_node) const;
  list< IEdge* > out_edges(INode* p_node) const;

  INode* aNode() { return (nodes.begin())->second; }

  /**
   * create new IEdge
   */
  IEdge* new_edge(INode* p_n1, INode* p_n2, void* p_element = NULL);
  
  int num_edges() { return edges.size(); }
  IEdgeIterator edges_begin() { return edges.begin(); }
  IEdgeIterator edges_end() { return edges.end(); }

  INode* opposite(INode* p_node, IEdge* p_edge);
  INode* source(IEdge* p_edge);
  INode* target(IEdge* p_edge);

  list< INode* > adjacent_nodes(INode* p_node);
  list< INode* > out_adjacent(INode* p_node);
  list< INode* > in_adjacent(INode* p_node);
};


#endif // #ifndef Graph_HH
