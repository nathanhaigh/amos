#ifndef Unitigger_HH
#define Unitigger_HH 1

#include "IGraph.hh"

using namespace std;

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

  void Unitigger::error(const char* m, const char* c = "");
  void Unitigger::inputReads(const char* p_file, IGraph* g);
  void Unitigger::inputOverlaps(const char* p_file, IGraph* g);
  void Unitigger::calc_layout(IGraph* g, INode* p_node);
  void Unitigger::output_contig(IGraph* g, INode* p_node);

  Unitigger() { }

};


#endif // #ifndef Unitigger_HH
