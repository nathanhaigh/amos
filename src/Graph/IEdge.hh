#ifndef __IEdge_HH
#define __IEdge_HH 1

#include <list>
#include <string>

#include "INode.hh"

class IEdge;
class INode;

/**
 * The <b>IEdge</b> class
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
class IEdge {

public:

  virtual ~IEdge( ) { }

  virtual void setColor(std::string p_color) = 0;
  virtual std::string getColor() const = 0;

  virtual   void* getElement() const = 0;
  virtual   void setElement(void* p_element) = 0;
  
  virtual   int getKey() const = 0;
  virtual   void setKey(int p_key) = 0;

  virtual   unsigned long getFlags() const = 0;
  virtual   void setFlags(unsigned long p_flags) = 0;

  virtual int getHidden() const = 0;
  virtual void setHidden(bool p_hidden) = 0;

  virtual   bool isDirected() const = 0;
  virtual   void setDirected(bool p_bit) = 0;

  virtual   bool isWeigthed() const = 0;
  virtual   void setWeighted(bool p_bit) = 0;

  virtual   void reverse() = 0;

  virtual   INode* getSource() const = 0;

  virtual   void setSource(INode *p_node) = 0;

  virtual   INode* getTarget() const = 0;

  virtual   void setTarget(INode *p_node) = 0;

  virtual   void setNodes(INode *p_node1, INode *p_node2) = 0;

  virtual   INode** getNodes() const = 0;
  virtual   INode* opposite(INode *p_node) const = 0;

};


#endif // #ifndef __IEdge_HH
