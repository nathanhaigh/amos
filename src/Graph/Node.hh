#ifndef Node_HH
#define Node_HH 1

#include <iostream>
#include <sstream>
#include <list>
#include "Graph.hh"

// Declarations need to Node
class Node;
class Edge;

using namespace std;

typedef list< Edge* >::iterator EdgeIterator;
typedef list< Node* >::iterator NodeIterator;


/**
 * The <b>Node</b> class
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
class Node {
public:
  // element reference
  void* element;

  // 
  list< Edge* > oedges;
  list< Edge* > iedges;

  // identitfier or position
  int key;

  unsigned long flags;

  Node(void* p_element) : element(p_element) { }

  /* Setters & Getters */
  void* getElement() const { return element; }
  void setElement(void* p_element) { element = p_element; }
  
  int getKey() const { return key; }
  void setKey(int p_key) { key = p_key; }

  unsigned long getFlags() const { return flags; }
  void setFlags(unsigned long p_flags) { flags = p_flags; }

  void add_edge(Edge* p_edge) { oedges.push_back(p_edge); }

  void add_oedge(Edge* p_edge) { oedges.push_back(p_edge); }

  void add_iedge(Edge* p_edge) { iedges.push_back(p_edge); }


  int degree() { return (oedges.size() + iedges.size()); }
  int out_degree() { return oedges.size(); }
  int in_degree() { return iedges.size(); }

  EdgeIterator out_edges_begin() { return oedges.begin(); }
  EdgeIterator out_edges_end() { return oedges.end(); }

  EdgeIterator in_edges_begin() { return iedges.begin(); }
  EdgeIterator in_edges_end() { return iedges.begin(); }
  
};

#endif // #ifndef Node_HH
