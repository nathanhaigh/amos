#ifndef Unitigger_HH
#define Unitigger_HH 1

#include "foundation_AMOS.hh"
#include <vector>
#include "IGraph.hh"
#include "Graph.hh"



/**
 * The <b>Unitigger</b> class
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
class Unitigger {
  
public:
  
  /** <code> g </code> overlap graph */
  Graph* graph;

  bool VERBOSE;
  
  void Unitigger::error(const char* m, const char* c = "");
  void Unitigger::add_read(AMOS::Read_t p_read);
  void Unitigger::add_overlap(AMOS::Overlap_t p_olap);
  void Unitigger::inputReads(const char* p_file, IGraph* g);
  void Unitigger::inputOverlaps(const char* p_file, IGraph* g);
  void Unitigger::calc_layout(IEdge* p_edge);
  void Unitigger::output_contigs(IGraph* g, INode* p_node);
  void Unitigger::output_amos_contigs(const string p_bankdir);
  void Unitigger::remove_transitive_overlaps(IGraph *g);
  void Unitigger::hide_containment(IGraph* g);
  void Unitigger::add_containment(IGraph* g);
  void Unitigger::calc_contigs();


  Unitigger();
  //~Unitiggger();
};


#endif // #ifndef Unitigger_HH
