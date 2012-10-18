#ifndef __Edge_HH
#define __Edge_HH 1

#include <string>
#include <iostream>
#include <sstream>
#include "INode.hh"
#include "IEdge.hh"

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
class Edge : public IEdge {

public:
  /** object associated with edge */
  void* element;

  /** identitfier, position in array or locator for map */
  int key;

  bool hidden;
  
  std::string color;

  /* nodes that the edge is connected */
  /** source if node is directed */
  INode* n1;

  /** target if node is directed */
  INode* n2;

  /** bit flags */
  unsigned long flags; 

  Edge(void* p_element, bool p_directed =false);

  void* getElement() const { return element; }
  void setElement(void* p_element) { element = p_element; }
  
  int getKey() const { return key; }
  void setKey(int p_key) { key = p_key; }

  void setColor(std::string p_color) { color = p_color; }
  std::string getColor() const { return color; }

  int getHidden() const { return hidden; }
  void setHidden(bool p_hidden) { hidden = p_hidden; }

  unsigned long getFlags() const { return flags; }
  void setFlags(unsigned long p_flags) { flags = p_flags; }
  bool isDirected() const { return flags & DIRECT_MASK; }
  void setDirected(bool p_bit) { flags |= p_bit; }

  bool isWeigthed() const { return flags & WEIGHT_MASK; }
  void setWeighted(bool p_bit) { flags |= p_bit; }

  void reverse();

  INode* getSource() const { return n1; }

  void setSource(INode *p_node) { n1 = p_node; }

  INode* getTarget() const { return n2; }

  void setTarget(INode *p_node) { n2 = p_node; }

  void setNodes(INode *p_node1, INode *p_node2);

  INode** getNodes() const;

  INode* opposite(INode *p_node) const;

};


#endif // #ifndef __Edge_HH
