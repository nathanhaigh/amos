/*** includes ***/
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <vector>
#include <queue>

#include "Unitigger.hh"
#include "Read.hh"
#include "Overlap.hh"

using namespace std;
using namespace AMOS;

static bool VERBOSE = false;

Unitigger::Unitigger() {
  if(VERBOSE) cout << " construct new unitigger" << endl;
  graph = new Graph();
}


void Unitigger::error(const char* m, const char* c) {
  cerr << m << ' ' << c << endl;
  exit(1);
}


void Unitigger::add_overlap(Overlap_t p_olap) {
  Overlap* olap = new Overlap();
  pair <ID_t, ID_t>  reads = p_olap.getReads();
  
  olap->rid1 = reads.first;
  olap->rid2 = reads.second;
  olap->ori = p_olap.getAdjacency();
  olap->ahang = p_olap.getAhang();
  olap->bhang = p_olap.getBhang();

  // determine overlap dovetail type
  if((olap->ahang >= 0) && (olap->bhang <= 0)) {
    olap->type = 'C';
    olap->flip = false;
    olap->asuffix = false;
    olap->bsuffix = false;
  } else if((olap->ahang <= 0) && (olap->bhang >= 0)) {
    olap->type = 'C';
    olap->asuffix = false;
    olap->bsuffix = false;
    olap->flip = true;
  } else if(olap->ori == 'N') {
    if(olap->ahang >= 0) {
      olap->type = 'R';
      olap->flip = false;
      olap->asuffix = true;
      olap->bsuffix = false;
    } else {   // ahang is negative
      olap->type = 'R';
      olap->flip = true;
      olap->asuffix = false;
      olap->bsuffix = true;
    }
  } else {   // ori == 'I'
    if(olap->ahang >= 0) { // suffix
      olap->type = 'S';
      olap->flip = false;
      olap->asuffix = true;
      olap->bsuffix = true;
    } else {   // ahang is negative, prefix
      olap->type = 'P';
      olap->flip = false;
      olap->asuffix = false;
      olap->bsuffix = false;
    }
  }

  if(VERBOSE) {
    cout << " Added overlap:" << endl;
    cout << "\trids " << olap->rid1 << " " << olap->rid2 << endl;
    cout << "\t type flip " << olap->type << " " << olap->flip << endl;
  }

  INode* n1 = graph->get_node(olap->rid1);
  INode* n2 = graph->get_node(olap->rid2);
  
  graph->new_edge(n1, n2, olap);
}


void Unitigger::add_read(Read_t p_read) {
  if(VERBOSE) cout << " add read " << p_read.getIID() << endl;
  Range_t range = p_read.getClearRange();
  
  Read* read = new Read(p_read.getIID(), range.getLength());
  read->range = range;

  INode* n = graph->new_node(read->id, read);
  if(VERBOSE) cout << " add node " <<  endl;
  n->setInterval(read->len);
}


///////////////// read in UMD reads ///////////////
void Unitigger::inputReads(const char* p_file, IGraph* g) {
  ifstream reads(p_file);
  if(!reads) error("cannot open reads file");

  int rid;
  int len;
  INode* n;
  Read* read;

  while(reads >> rid >> len) {
    if(VERBOSE) cout << " Create INode " << rid << " length " << len << endl;
    read = new Read(rid, len);
    
    n = g->new_node(rid, read);
    n->setInterval(len);
  }
}


////////////////////////////// read in UMD overlap ////////////
void Unitigger::inputOverlaps(const char* p_file, IGraph* g) {
  ifstream olaps(p_file);
  if(!olaps) error("cannot open overlaps file");

  Overlap* olap = new Overlap();

  while(olaps >> olap->rid1 >> olap->rid2 >> olap->ori >> olap->ahang \
	>> olap->bhang >> olap->alen >> olap->blen >> olap->alin_score \
	>> olap->errors >> olap->percent) {
    if(VERBOSE) cout << " Create edge between " << olap->rid1 << " and " << olap->rid2 << endl;
    INode* n1 = g->get_node(olap->rid1);
    INode* n2 = g->get_node(olap->rid2);
    g->new_edge(n1, n2, olap);
  }

}


// need start node and sub-graph
void Unitigger::calc_layout(IGraph* g, INode* p_node) {
  list< IEdge* > edges;
  IEdge* edge;
  Read* read1 = (Read*) p_node->getElement();

  if(VERBOSE) cout << " start layout " << endl;
  // 
  // calculate layout for each neighbor
  edges = g->incident_edges(p_node);
  for(IEdgeIterator iter = edges.begin(); iter != edges.end(); ++iter) {
    edge = (*iter);
    Overlap* olap = (Overlap *)edge->getElement();
    INode* node = g->opposite(p_node, edge); 
    Read* read2 = (Read*) node->getElement();


    if(read2->start == -1) {
      if(VERBOSE) cout << " layout read " << read1->id << " and " << read2->id << endl;

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
	  if(VERBOSE) cout << " normal rd2.s = rd1.s + ahang " <<  endl;
	  if(VERBOSE) cout << read1->start << " + " << ahang << endl;
	  read2->start = read1->start + ahang;
	  read2->end = read2->start + len;
	} else if(olap->ori == 'I') {
	  if(VERBOSE) cout << " innie rd2.e = rd1.s + ahang " <<  endl;
	  if(VERBOSE) cout << read1->start << " + " << ahang << endl;
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

	  if(VERBOSE) cout << " normal rev rd2.s = rd1.s - ahang " <<  endl;
	  if(VERBOSE) cout << read1->start << " - " << olap->ahang << endl;
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


	  if(VERBOSE) cout << " read 1 is B and is innie " << endl;
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
      if(VERBOSE) cout << " read layout already calculated " << endl;
    }
    
  }

  if(VERBOSE) cout << " end layout " << endl;
  
}


void Unitigger::output_contigs(IGraph* g, INode* p_node) {
  Read* read = (Read*) p_node->getElement();
  if(VERBOSE) cout << read->id << ' ' << read->start << ' ' << read->end << endl;
  p_node->setFlags(0x1);
  
  list< INode* > nodes = g->adjacent_nodes(p_node);
  INode* node;

  for(INodeIterator iter = nodes.begin(); iter != nodes.end(); ++iter) {
    node = (*iter);
    if(node->getFlags() != 0x1) {
      output_contigs(g, node);
    }
  }

}


void Unitigger::output_amos_contigs(const string p_bankdir) {
  Message_t msg;
  Layout_t layout;
  vector<Tile_t> tiles;
  BankStream_t bank(Layout_t::NCODE);

  try {
    if(!bank.exists(p_bankdir)) {
      bank.create(p_bankdir);
    } else {
      bank.open(p_bankdir);
    }
    cout << " Writing layouts to bank " << p_bankdir << endl;
    
    INode* node;
    Read* read;

    int count = 0;
    for(PairIterator iter = graph->nodes_begin(); iter != graph->nodes_end(); ++iter) {
      count++;
      Tile_t tile;
      node = (*iter).second;
      read = (Read*) node->getElement();
      tile.source = read->id;
      
      if(read->start > read->end) {
	tile.offset = read->start;
	tile.range = read->range;
      } else {
	tile.range = read->range;
	tile.range.swap();
	tile.offset = read->end;
      }
    
      tiles.push_back(tile);
    }

    cout << " tiles in layout " << count << endl;
    layout.setTiling(tiles);
    
    bank << layout;

    bank.close();

  } catch (Exception_t &e) {
    cerr << "Exception while writing layouts : " << e << endl;
  }
}


void hide_containment(IGraph* g) {
  IEdge* edge;
  Overlap* olap;
  Read* read;
  INode* node;
  
  // loop over all edges
  for(IEdgeIterator edges = g->edges_begin(); edges != g->edges_end(); ++edges) {
    edge = (*edges);
    olap = (Overlap *)edge->getElement();
    
    if(olap->type == 'C') {
      int tid = olap->rid1;      
      if(olap->flip) { tid = olap->rid2; }

      node = edge->getSource();
      read = (Read*) node->getElement();

      if(read->id != tid) {
	  node = edge->getTarget();
      }
      
      node->setHidden(true);
    }
  }
}


void Unitigger::remove_transitive_overlaps(IGraph *g) {
  queue< INode* > q;
  queue< INode* > children;
  map< int, IEdge* > parents; // parent mapping
  queue< IEdge* > trans;

  for(PairIterator nodeIter = g->nodes_begin(); nodeIter != g->nodes_end(); ++nodeIter) {
    INode* root_node = (*nodeIter).second;
    int depth = 1;

    if((! root_node->getHidden()) && root_node->getDepth() == 0) {
      root_node->setDepth(depth);
      root_node->setFlags(1);
      q.push(root_node);
      
      INode* cur_node;
      INode* child;
      IEdge* cur_edge;
      list < IEdge* > inc_edges;
      
      while(!q.empty()) {
	cur_node = q.front();
	q.pop();
	depth = cur_node->getDepth();
	cur_node->setFlags(2); // black
	
	// go over each child and mark/queue
	inc_edges = g->incident_edges(cur_node);
	for(IEdgeIterator iter = inc_edges.begin(); iter != inc_edges.end(); ++iter) {
	  cur_edge = (*iter);
	  child = cur_edge->opposite(cur_node);
	  if(child->getFlags() == 0) { // hasn't  been visited
	    child->setDepth(depth + 1);
	    child->setFlags(1); // gray
	    parents[child->getKey()] = cur_edge;
	    q.push(child);
	    children.push(child);
	    child->setParent(cur_node->getKey());
	  } else if(child->getFlags() == 1) {
	    children.push(child);
	    child->setParent(cur_node->getKey());
	    parents[child->getKey()] = cur_edge;
	  }
	}


	// look for transitive edges
	while(! children.empty()) {
	  IEdge* grand_edge;
	  INode* grand_node = children.front();
	  children.pop();
	  INode* node2;

	  list< IEdge* > grand = g->incident_edges(grand_node);
	    for(IEdgeIterator iter = grand.begin(); iter != grand.end(); ++iter) {
	      grand_edge = (*iter);
	      node2 = grand_edge->opposite(grand_node);

	      if(node2->getFlags() != 2) {
		// check for transitive link
		if(node2->getParent() == grand_node->getParent()) {
		  if(VERBOSE) {
		    cout << " found transitive link between ";
		    cout << grand_node->getParent() << " ";
		    cout << grand_node->getKey() << " " << node2->getKey() << endl;
		  }
		  trans.push(grand_edge);
		}
	      }
	
	    }
	    grand_node->setParent(-1);
	}
      }
      
      // hide transitive edges
      while(! trans.empty()) {
	trans.front()->setHidden(true);
	trans.pop();
      }
      
    }

  }

  g->clear_flags();
}

void Unitigger::calc_contigs() {

  //
  // Step 1. handle containment edges
  hide_containment((IGraph*) graph);

  //
  // Step 2. transitive edges
  graph->clear_flags();
  remove_transitive_overlaps((IGraph*) graph);


  //
  // Step 3. Chunking
  //
  // Calculate reads coordinates
  // for each composite run calc layout
  INode* root_node = graph->aNode();
  Read* start_read = (Read *) root_node->getElement();
  start_read->start = 0;
  start_read->end = start_read->len - 1;
  
  calc_layout(graph, root_node);
  graph->clear_flags();

  if(VERBOSE) cout << " end calc_contigs " << endl;
  
}

