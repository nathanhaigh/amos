#ifndef Node_HH
#define Node_HH 1

#include <iostream>
#include <sstream>
#include <list>
#include "INode.hh"
#include "IEdge.hh"

// Declarations need to Node
class IEdge;

using namespace std;

typedef list< IEdge* >::iterator IEdgeIterator;
//typedef list< Node* >::iterator NodeIterator;
//typedef map< int, Node* >::iterator PairIterator;


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
class Node : public INode {
public:
  // element reference
  void* element;

  // 
  list< IEdge* > oedges;
  list< IEdge* > iedges;

  // identitfier or position
  int key;

  // interval
  int interval;

  unsigned long flags;

  Node(void* p_element) : element(p_element) {
    flags = 0;
  }

  /* Setters & Getters */
  void* getElement() const { return element; }
  void setElement(void* p_element) { element = p_element; }
  
  int getKey() const { return key; }
  void setKey(int p_key) { key = p_key; }

  int getInterval() const { return interval; }
  void setInterval(int p_interval) { interval = p_interval; }

  unsigned long getFlags() const { return flags; }
  void setFlags(unsigned long p_flags) { flags = p_flags; }

  void add_edge(IEdge* p_edge) { oedges.push_back(p_edge); }

  void add_oedge(IEdge* p_edge) { oedges.push_back(p_edge); }

  void add_iedge(IEdge* p_edge) { iedges.push_back(p_edge); }


  int degree() { return (oedges.size() + iedges.size()); }
  int out_degree() { return oedges.size(); }
  int in_degree() { return iedges.size(); }

  IEdgeIterator out_edges_begin() { return oedges.begin(); }
  IEdgeIterator out_edges_end() { return oedges.end(); }

  IEdgeIterator in_edges_begin() { return iedges.begin(); }
  IEdgeIterator in_edges_end() { return iedges.begin(); }
  
};

#endif // #ifndef Node_HH
