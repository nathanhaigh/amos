/*** includes ***/
#include <foundation_AMOS.hh>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include "IGraph.hh"
#include "Graph.hh"
#include "Read.hh"
#include "Overlap.hh"
#include "Unitigger.hh"

using namespace std;

void Unitigger::error(const char* m, const char* c) {
  cerr << m << ' ' << c << '\n';
  exit(1);
}

///////////////// read in nodes ///////////////
void Unitigger::inputReads(const char* p_file, IGraph* g) {
  ifstream reads(p_file);
  if(!reads) error("cannot open reads file");

  int rid;
  int len;
  INode* n;
  Read* read;

  while(reads >> rid >> len) {
    cout << " Create INode " << rid << " length " << len << endl;
    read = new Read(rid, len);
    
    n = g->new_node(rid, read);
    n->setInterval(len);
  }
}

////////////////////////////// edges ////////////
void Unitigger::inputOverlaps(const char* p_file, IGraph* g) {
  ifstream olaps(p_file);
  if(!olaps) error("cannot open overlaps file");

  Overlap* olap = new Overlap();

  while(olaps >> olap->rid1 >> olap->rid2 >> olap->ori >> olap->ahang \
	>> olap->bhang >> olap->alen >> olap->blen >> olap->alin_score \
	>> olap->errors >> olap->percent) {
    cout << " Create edge between " << olap->rid1 << " and " << olap->rid2 << endl;
    INode* n1 = g->get_node(olap->rid1);
    INode* n2 = g->get_node(olap->rid2);
    g->new_edge(n1, n2, olap);
    olap = new Overlap();
  }

}

// need start node and sub-graph
void Unitigger::calc_layout(IGraph* g, INode* p_node) {
  list< IEdge* > edges;
  IEdge* edge;
  Read* read1 = (Read*) p_node->getElement();

  cout << " start layout " << endl;
  // 
  // calculate layout for each neighbor
  edges = g->incident_edges(p_node);
  for(IEdgeIterator iter = edges.begin(); iter != edges.end(); ++iter) {
    edge = (*iter);
    Overlap* olap = (Overlap *)edge->getElement();
    INode* node = g->opposite(p_node, edge); 
    Read* read2 = (Read*) node->getElement();


    if(read2->start == -1) {
      cout << " layout read " << read1->id << " and " << read2->id << endl;

      if(read1->id == olap->rid1) { // 1 = A
	int ahang, len;
	
	if(read1->start > read1->end) {
	  ahang = - olap->ahang;
	  len = - read2->len;
	} else {
	  ahang = olap->ahang;
	  len = read2->len;
	}

	if(olap->ori == 'N') {
	  cout << " normal rd2.s = rd1.s + ahang " <<  endl;
	  cout << read1->start << " + " << ahang << endl;
	  read2->start = read1->start + ahang;
	  read2->end = read2->start + len;
	} else if(olap->ori == 'I') {
	  cout << " innie rd2.e = rd1.s + ahang " <<  endl;
	  cout << read1->start << " + " << ahang << endl;
	  read2->end = read1->start + ahang;
	  read2->start = read2->end + len;
	} else {
	  error(" invalid orientation of overlap ");
	}

      } else if(read1->id == olap->rid2) { // 1 = B
	int ahang, len;

	if(olap->ori == 'N') {
	  if(read1->start > read1->end) {
	    ahang = - olap->ahang;
	    len = - read2->len;
	  } else {
	    ahang = olap->ahang;
	    len = read2->len;
	  }

	  cout << " normal rev rd2.s = rd1.s - ahang " <<  endl;
	  cout << read1->start << " - " << olap->ahang << endl;
	  read2->start = read1->start - ahang;
	  read2->end = read2->start + len;
	} else if(olap->ori == 'I') {

	  if(read1->end > read1->start) {
	    ahang = - olap->ahang;
	    len = - read2->len;
	  } else {
	    ahang = olap->ahang;
	    len = read2->len;
	  }


	  cout << " read 1 is B and is innie " << endl;
	  read2->start = read1->end - ahang;
	  read2->end = read2->start + len;
	} else {
	  error(" invalid orientation of overlap ");
	}
      } else {
	  error(" invalid overlap between reads ");
      }	
      
      // recurse
      calc_layout(g, node);
    } else {
      cout << " read layout already calculated " << endl;
    }
    
  }

  cout << " end layout " << endl;
  
}

void Unitigger::output_contig(IGraph* g, INode* p_node) {
  Read* read = (Read*) p_node->getElement();
  cout << read->id << ' ' << read->start << ' ' << read->end << endl;
  p_node->setFlags(0x1);
  
  
  list< INode* > nodes = g->adjacent_nodes(p_node);
  INode* node;

  for(INodeIterator iter = nodes.begin(); iter != nodes.end(); ++iter) {
    node = (*iter);
    if(node->getFlags() != 0x1) {
      output_contig(g, node);
    }
  }

}

//void Unitigger::containment(IGraph* g) {
  // Do DFS 
//}


int main(int argc, char* argv[]) {
  Unitigger uni;

  if(argc != 3) uni.error(" wrong number of arguments");
  
  //
  // Input
  //
  // create graph and import nodes and edges
  Graph* g = new Graph("name");

  uni.inputReads(argv[1], (IGraph*) g);

  uni.inputOverlaps(argv[2], (IGraph*) g);

  //
  // Step 1. handle containment edges
  

  //
  // Step 2. transitive edges

  //
  // Step 3. Chunking

  //
  // Calculate reads coordinates
  // for each composite run calc layout
  INode* start_node = g->get_node(57);
  Read* start_read = (Read *) start_node->getElement();
  start_read->start = 0;
  start_read->end = start_read->len - 1;
  
  uni.calc_layout(g, start_node);

  //
  // output contigs
  cout << "C 0" << endl;
  uni.output_contig(g, start_node);

  cout << " start depth first search " << endl;
  

  g->create_dot_file("te");
  
}

