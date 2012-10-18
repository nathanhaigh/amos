#ifndef SubGraph_HH
#define SubGraph_HH 1

#include <iostream>
#include <sstream>
#include <list>
#include "IGraph.hh"
#include "Graph.hh"
#include "Node.hh"
#include "Edge.hh"

class IEdge;
class INode;

class Edge;
class Node;


/**
 * The <b>SubGraph</b> class
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
class SubGraph : public Graph {
  
public:
  
  IGraph& parent;

  SubGraph(IGraph& p_parent, std::string name="noname");

  virtual bool contains(IEdge* p_edge);
  virtual bool contains(INode* p_node);

  virtual void add_node(INode* p_node);

  // adds nodes as well
  virtual void add_edge(IEdge* p_edge);

};


#endif // #ifndef SubGraph_HH
