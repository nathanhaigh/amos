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

enum COLORS { green, orange, red, blue, yellow, pink };
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

// need start node and sub-graph
void Unitigger::calc_layout(IEdge* p_edge) {
  INode* target = p_edge->getTarget();
  INode* source = p_edge->getSource();
  Read* read1 = (Read*) source->getElement();
  Read* read2 = (Read*) target->getElement();
  Overlap* olap = (Overlap *)p_edge->getElement();
  
  if(read2->start != -1) {
    Read* tmp = read1;
    read1 = read2;
    read2 = tmp;
  }

  if(VERBOSE) cout << " start layout " << endl;
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
    
  } else {
    cerr << " passed a layout to calulate but both read have start positions " << endl;
  }
  if(VERBOSE) cout << " end layout " << endl;
  
}


void Unitigger::output_amos_contigs(const string p_bankdir) {
  Message_t msg;
  BankStream_t bank(Layout_t::NCODE);
  deque< IEdge* > contig;
  queue< IEdge* > containment;
  int contig_count = 1;
  bool need_suffix;
  bool next_suffix;
  graph->clear_flags();
  INode* first_node;

  try {
    if(!bank.exists(p_bankdir)) {
      bank.create(p_bankdir);
    } else {
      bank.open(p_bankdir);
    }
    cout << " Writing layouts to bank " << p_bankdir << endl;
  } catch (Exception_t &e) {
    cerr << "Exception while writing layouts : " << e << endl;
    exit(127);
  }
  

  for(PairIterator nodeIter = graph->nodes_begin(); nodeIter != graph->nodes_end(); ++nodeIter) {
    INode* start_node = (*nodeIter).second;

    if((start_node->getHidden()) || (start_node->getFlags() == 1)) {
      continue;
    }
  
  for(int i = 0; i < 2; i++) {
    bool backtrack = false;
    cout << " ------------------------------------------------- " << endl;
    need_suffix = ! i;
    INode* cur_node = start_node;
    first_node = start_node;
    cur_node->setFlags(0);
    // first forward
    while(cur_node->getFlags() != 1) {
      cur_node->setFlags(1);
      cur_node->setColor("green");
      IEdge* edge;
      Overlap* ovl;
      int matches = 0;
      IEdge* path;
      int cur_rid = cur_node->getKey();
      
      list< IEdge* > edges =  graph->incident_edges(cur_node);
      for(IEdgeIterator iter = edges.begin(); iter != edges.end(); ++iter) {
	edge = (*iter);
	ovl = (Overlap*) edge->getElement();
	//cout << " current node is " << cur_rid << endl;
	//	cout << " current overlap between " << ovl->rid1 << "and " << ovl->rid2 << " type " << ovl->type;
	//	cout << " a " << ovl->asuffix << " b " << ovl->bsuffix << " need " << need_suffix << endl;
	if(edge->getFlags() == 1) {
	  continue;
	}

 	if(ovl->type == 'C') {
	  containment.push(edge);
 	  edge->setFlags(1);
 	} else {
	  if(ovl->type == 'S') { // next will be prefix
	    if(need_suffix) {
	      matches++;
	      path = edge;
	      next_suffix = false;
	    } else {
	      if(contig.size() > 0) {
		backtrack = true;
		break;
	      }
	    }
	  } else if(ovl->type == 'R') {
	    if((cur_rid == ovl->rid1) && (ovl->asuffix)) { // next will be suffix
	      if(need_suffix) {
		matches++;
		path = edge;
		next_suffix = true;
	      } else {
		if(contig.size() > 0) {
		  backtrack = true;
		  break;
		}
	      }
	    } else if((cur_rid == ovl->rid2) && (ovl->bsuffix)) { // next will be suffix
	      if(need_suffix) {
		matches++;
		path = edge;
		next_suffix = true;
	      } else {
		if(contig.size() > 0) {
		  backtrack = true;
		  break;
		}
	      }	     

	    }
	  }
	  if(ovl->type == 'P') { // next will be suffix
	    if(!need_suffix) {
	      matches++;
	      path = edge;
	      next_suffix = true;
	    } else {
	      if(contig.size() > 0) {
		backtrack = true;
		break;
	      }
	    }
	  } else if(ovl->type == 'R') {
	    if((cur_rid == ovl->rid1) && !(ovl->asuffix)) { // next will be suffix
	      if(!need_suffix) {
		matches++;
		path = edge;
		next_suffix = false;
	      } else {
		if(contig.size() > 0) {
		  backtrack = true;
		  break;
		}
	      }
	    } else if((cur_rid == ovl->rid2) && !(ovl->bsuffix)) { // next will be suffix
	      if(!need_suffix) {
		matches++;
		path = edge;
		next_suffix = false;
	      } else {
		if(contig.size() > 0) {
		  backtrack = true;
		  break;
		}
	      }
	    }
	  }
	}
      }
      
      if(backtrack) {
	cur_node->setFlags(0);
	if(i) 
	  contig.pop_back();
	else 
	  contig.pop_front();
	break;
      }
	
      if(matches == 1) {
	path->setFlags(1);	  
	if(i) 
	  contig.push_back(path);
	else 
	  contig.push_front(path);
	path->setColor("green");
	first_node = cur_node;
	cur_node = path->opposite(cur_node);
	need_suffix = next_suffix;
      } else {
	first_node = cur_node;
      }

    }
    
  }
  
  // now we have a queue of edges for the contig
  // set the first to zero 
  IEdge* calc_edge;
  Tile_t tile;
  Layout_t layout;
  vector<Tile_t> tiles;

  cout << " ** first node is " << first_node->getKey();
  Read* read_tile = (Read*) first_node->getElement();
  read_tile->start = 0;
  read_tile->end = read_tile->len;
  tile.source = read_tile->id;
  tile.offset = read_tile->start;
  tile.range = read_tile->range;
  tiles.push_back(tile);  

  while(! contig.empty()) {
    calc_edge = contig.back();
    contig.pop_back();
    calc_layout(calc_edge);
    first_node = calc_edge->opposite(first_node);
    cout << " -> " << first_node->getKey();
    read_tile = (Read*) first_node->getElement();
    tile.source = read_tile->id;
    
    if(read_tile->start > read_tile->end) {
      tile.offset = read_tile->start;
      tile.range = read_tile->range;
    } else {
      tile.range = read_tile->range;
      tile.range.swap();
      tile.offset = read_tile->end;
    }
    
    tiles.push_back(tile);
  }

  layout.setTiling(tiles);
  cout << endl;
  
  bank << layout;
  }
  
  bank.close();
}
	



void Unitigger::hide_containment(IGraph* g) {
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

void Unitigger::add_containment(IGraph* g) {
  IEdge* edge;
  Overlap* olap;
  Read* read;
  INode* node;
  int count = 0;
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
      if(node->getHidden()) {
	count++;
	edge->setHidden(false);
	edge->setColor("purple");
	node->setNodeHidden(false);
	node->setColor("purple");
      }
    }
  }
  
  cout << " Added back in " << count << " containment edges " << endl;
}


void Unitigger::remove_transitive_overlaps(IGraph *g) {
  queue< INode* > q;
  queue< INode* > children;
  map< int, IEdge* > parents; // parent mapping
  queue< IEdge* > trans;

  graph->clear_flags();

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
		  int pkey = grand_node->getParent();
		  int gkey = grand_node->getKey();
		  int nkey = node2->getKey();
		  bool suffix1;
		  bool suffix2;

		  if(VERBOSE) {
		    cout << " found transitive link between ";
		    cout << grand_node->getParent() << " ";
		    cout << grand_node->getKey() << " " << node2->getKey() << endl;
		  }

		  Overlap* o1 = (Overlap *)grand_edge->getElement();
		  Overlap* o2 = (Overlap *)parents[grand_node->getKey()]->getElement();
		  Overlap* o3 = (Overlap *)parents[node2->getKey()]->getElement();

		  if(o2->rid1 == pkey) {
		    suffix1 = o2->asuffix;
		  } else if(o2->rid2 == pkey) {
		    suffix1 = o2->bsuffix;
		  }

		  if(o3->rid1 == pkey) {
		    suffix2 = o3->asuffix;
		  } else if(o3->rid2 == pkey) {
		    suffix2 = o3->bsuffix;
		  }

		  if(suffix1 != suffix2) {
		    trans.push(grand_edge);
		  }

		  if(o1->rid1 == gkey) {
		    suffix1 = o1->asuffix;
		  } else if(o1->rid2 == gkey) {
		    suffix1 = o1->bsuffix;
		  }

		  if(o2->rid1 == gkey) {
		    suffix2 = o2->asuffix;
		  } else if(o2->rid2 == gkey) {
		    suffix2 = o2->bsuffix;
		  }

		  if(suffix1 != suffix2) {
		    trans.push(parents[node2->getKey()]);
		  }


		  if(o1->rid1 == nkey) {
		    suffix1 = o1->asuffix;
		  } else if(o1->rid2 == nkey) {
		    suffix1 = o1->bsuffix;
		  }

		  if(o3->rid1 == nkey) {
		    suffix2 = o3->asuffix;
		  } else if(o3->rid2 == nkey) {
		    suffix2 = o3->bsuffix;
		  }
		  
		  if(suffix1 != suffix2) {
		    trans.push(parents[grand_node->getKey()]);
		  }
		  
		  
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
  remove_transitive_overlaps((IGraph*) graph);


  //
  // Step 3. Chunking
  //
  // Calculate reads coordinates
  // for each composite run calc layout
  // add_containment((IGraph*) graph);  
  //  calc_layout(graph, root_node);
  //  graph->clear_flags();

  if(VERBOSE) cout << " end calc_contigs " << endl;
  
}

