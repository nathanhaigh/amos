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

  std::vector<std::string> colors;
  std::vector< Contig* > contigs;
  std::queue< IEdge* > containment;
  //  HASHMAP::hash_map< int, IEdge* > 
  bool VERBOSE;
  bool SINGLE;

  void error(const char* m, const char* c = "");

  void add_read(Read* p_read);
  void add_read(AMOS::Read_t p_read);

  void add_overlap(Overlap* p_olap);
  void add_overlap(AMOS::Overlap_t p_olap);

  void layout_contig(Contig* contig);
  void layout_read(IEdge* p_edge, INode* p_node);

  void output_umd_contigs(IGraph* g, INode* p_node);
  void output_amos_contigs(const std::string p_bankdir);

  void hide_transitive_overlaps(IGraph *g);

  void hide_containment(IGraph* g);
  void add_containment();

  void calc_contigs();

  void find_chunks();

  void output_contig_graphs();

  Contig* walk(INode* p_node);
  IEdge* walk_edge(IEdge* e, INode* n, Contig* ctg);

  bool isSuffix(Read* read, Overlap* ovl);


  Unitigger();
  //~Unitiggger();
};


#endif // #ifndef Unitigger_HH
