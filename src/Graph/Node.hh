#ifndef Node_HH
#define Node_HH 1

#include <string>
#include "INode.hh"
#include "IEdge.hh"

// Declarations need to Node
class IEdge;

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
  HASHMAP::hash_map< int, IEdge* > oedges;
  HASHMAP::hash_map< int, IEdge* > iedges;

  // identitfier or position
  int key;

  bool hidden;

  // interval
  int interval;

  int depth;

  int parent;

  std::string color;

  unsigned long flags;

  Node(void* p_element) : element(p_element) {
    flags = 0;
    depth = 0;
    parent = -1;
    hidden = false;
    color = "black";
  }

  /* Setters & Getters */
  void* getElement() const { return element; }
  void setElement(void* p_element) { element = p_element; }

  void setColor(const std::string p_color) { color = p_color; }
  std::string getColor() const { return color; }
  
  int getKey() const { return key; }
  void setKey(int p_key) { key = p_key; }

  int getDepth() const { return depth; }
  void setDepth(int p_depth) { depth = p_depth; }

  int getParent() const { return parent; }
  void setParent(int p_parent) { parent = p_parent; }

  int getHidden() const { return hidden; }

  void setHidden(bool p_hidden) { 
    IEdge* edge;
    hidden = p_hidden;
    for(IEdgeIterator edges = oedges.begin(); edges != oedges.end(); edges++) {
      edge = (*edges).second;
      edge->setHidden(p_hidden);
    }
  }

  void setNodeHidden(bool p_hidden) {
    hidden = p_hidden;
  }

  int getInterval() const { return interval; }
  void setInterval(int p_interval) { interval = p_interval; }

  unsigned long getFlags() const { return flags; }
  void setFlags(unsigned long p_flags) { flags = p_flags; }

  void add_edge(IEdge* p_edge) { oedges[p_edge->getKey()] = p_edge; }

  void add_oedge(IEdge* p_edge) { oedges[p_edge->getKey()] = p_edge; }

  void add_iedge(IEdge* p_edge) { iedges[p_edge->getKey()] = p_edge; }


  int degree() { return (oedges.size() + iedges.size()); }
  int out_degree() { return oedges.size(); }
  int in_degree() { return iedges.size(); }

  IEdgeIterator out_edges_begin() { return oedges.begin(); }
  IEdgeIterator out_edges_end() { return oedges.end(); }

  IEdgeIterator in_edges_begin() { return iedges.begin(); }
  IEdgeIterator in_edges_end() { return iedges.begin(); }
  
};

#endif // #ifndef Node_HH
