#ifndef __Edge_HH
#define __Edge_HH 1

#include <iostream>
#include <sstream>
#include "Node.hh"

using namespace std;

class Edge;

typedef list< Edge* >::iterator EdgeIterator;

const unsigned long DIRECT_MASK = 0x1;
const unsigned long WEIGHT_MASK = 0x2;

/**
 * The <b>Edge</b> class
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
class Edge {

public:
  /** object associated with edge */
  void* element;

  /** identitfier, position in array or locator for map */
  int key;

  /* nodes that the edge is connected */
  /** source if node is directed */
  Node* n1;

  /** target if node is directed */
  Node* n2;

  /** bit flags */
  unsigned long flags; 

  Edge(void* p_element, bool p_directed =false) : element(p_element) {
    flags |= p_directed;
  }

  void* getElement() const { return element; }
  void setElement(void* p_element) { element = p_element; }
  
  int getKey() const { return key; }
  void setKey(int p_key) { key = p_key; }

  unsigned long getFlags() const { return key; }
  void setFlags(unsigned long p_flags) { flags = p_flags; }
  bool isDirected() const { return flags & DIRECT_MASK; }
  void setDirected(bool p_bit) { flags |= p_bit; }

  bool isWeigthed() const { return flags & WEIGHT_MASK; }
  void setWeighted(bool p_bit) { flags |= p_bit; }

  void reverse() {
    Node* tmp = n1;
    n1 = n2;
    n2 = tmp;
  }

  Node* getSource() const { return n1; }

  void setSource(Node *p_node) { n1 = p_node; }

  Node* getTarget() const { return n2; }

  void setTarget(Node *p_node) { n2 = p_node; }

  void setNodes(Node *p_node1, Node *p_node2) {
    n1 = p_node1;
    n2 = p_node2;
  }

  Node** getNodes() const {
    Node** nodes = new Node*[2];
    nodes[0] = n1;
    nodes[1] = n2;
    
    return nodes;
  }

  Node* opposite(Node *p_node) const {
    if(n1 == p_node) {
      return n2;
    } else if(n2 == p_node) {
      return n1;
    } else { 
      // TODO throw exception
      return NULL;
    }
  }

};


#endif // #ifndef __Edge_HH
