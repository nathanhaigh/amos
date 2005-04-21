#ifndef INode_HH
#define INode_HH 1

#include <string>
#include "utility_AMOS.hh"
#include "IEdge.hh"


class IEdge;

typedef HASHMAP::hash_map< int, IEdge* >::iterator IEdgeIterator;

/**
 * The <b>INode</b> class is a interface
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
class INode {
public:

  virtual ~INode( ) { }

  virtual void setColor(const std::string p_color) = 0;
  virtual std::string getColor() const = 0;

  virtual void* getElement() const = 0;
  virtual void setElement(void* p_element) = 0;
  
  virtual int getKey() const = 0;
  virtual void setKey(int p_key) = 0;

  virtual int getDepth() const = 0;
  virtual void setDepth(int p_depth) = 0;

  virtual int getParent() const = 0;
  virtual void setParent(int p_parent) = 0;

  virtual int getInterval() const = 0;
  virtual void setInterval(int p_interval) = 0;

  virtual int getHidden() const = 0;
  virtual void setHidden(bool p_hidden) = 0;
  virtual void setNodeHidden(bool p_hidden) = 0;
  virtual unsigned long getFlags() const = 0;
  virtual void setFlags(unsigned long p_flags) = 0;

  virtual void add_edge(IEdge* p_edge) = 0;
  virtual void add_oedge(IEdge* p_edge) = 0;
  virtual void add_iedge(IEdge* p_edge) = 0;

  virtual int degree() = 0;
  virtual int out_degree() = 0;
  virtual int in_degree() = 0;

  virtual IEdgeIterator out_edges_begin() = 0;
  virtual IEdgeIterator out_edges_end() = 0;

  virtual IEdgeIterator in_edges_begin() = 0;
  virtual IEdgeIterator in_edges_end() = 0;
  
};

#endif // #ifndef INode_HH
