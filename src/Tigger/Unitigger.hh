#ifndef Unitigger_HH
#define Unitigger_HH 1

#include "foundation_AMOS.hh"
#include <vector>
#include <queue>
#include "IGraph.hh"
#include "Graph.hh"
#include "SubGraph.hh"
#include "Read.hh"
#include "Overlap.hh"
#include "Contig.hh"

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
  
  vector<string> colors;
  vector< Contig* > contigs;
  queue< IEdge* > containment;

  void Unitigger::error(const char* m, const char* c = "");

  void Unitigger::add_read(Read* p_read);
  void Unitigger::add_read(AMOS::Read_t p_read);

  void Unitigger::add_overlap(Overlap* p_olap);
  void Unitigger::add_overlap(AMOS::Overlap_t p_olap);

  void Unitigger::layout_contig(Contig* contig);
  void Unitigger::layout_read(IEdge* p_edge, INode* p_node);

  void Unitigger::output_umd_contigs(IGraph* g, INode* p_node);
  void Unitigger::output_amos_contigs(const string p_bankdir);

  void Unitigger::hide_transitive_overlaps(IGraph *g);

  void Unitigger::hide_containment(IGraph* g);
  void Unitigger::add_containment();

  void Unitigger::calc_contigs();

  void Unitigger::find_chunks();

  void Unitigger::output_contig_graphs();

  Contig* Unitigger::walk(INode* p_node);
  IEdge* Unitigger::walk_edge(IEdge* e, INode* n, Contig* ctg);

  bool Unitigger::isSuffix(Read* read, Overlap* ovl);


  Unitigger();
  //~Unitiggger();
};


#endif // #ifndef Unitigger_HH
