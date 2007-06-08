/*** includes ***/
#include "Unitigger.hh"

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <stdio.h>

using namespace std;
using namespace AMOS;

typedef list< INode* >::iterator nodeListIter;
typedef list< IEdge* >::iterator edgeListIter;

Unitigger::Unitigger() {
  graph = new Graph();

  VERBOSE = false;
  
  colors.push_back("green");
  colors.push_back("red");
  colors.push_back("orange");
  colors.push_back("yellow");
  colors.push_back("dodgerblue");
  colors.push_back("purple");
}


void Unitigger::error(const char* m, const char* c) {
  cerr << m << ' ' << c << endl;
  exit(1);
}


void Unitigger::add_overlap(Overlap* p_olap) {
  
  // convert AMOS adjacency info to Gene's from paper
  // determine overlap dovetail type
  if((p_olap->ahang >= 0) && (p_olap->bhang <= 0)) {
    p_olap->type = 'C';
    p_olap->asuffix = false;
    p_olap->bsuffix = false;

  } else if((p_olap->ahang <= 0) && (p_olap->bhang >= 0)) {
    p_olap->type = 'C';
    p_olap->asuffix = false;
    p_olap->bsuffix = false;
    p_olap->flip();

  } else if(p_olap->ori == 'N') {

    if(p_olap->ahang >= 0) {
      p_olap->type = 'R';
      p_olap->asuffix = true;
      p_olap->bsuffix = false;

    } else {   // ahang is negative
      p_olap->type = 'R';
      p_olap->asuffix = false;
      p_olap->bsuffix = true;
      p_olap->flip();

    }
  } else {   // ori == 'I'

    if(p_olap->ahang >= 0) { // suffix
      p_olap->type = 'S';
      p_olap->asuffix = true;
      p_olap->bsuffix = true;

    } else {   // ahang is negative, prefix
      p_olap->type = 'P';
      p_olap->asuffix = false;
      p_olap->bsuffix = false;

    }
  }

  if(VERBOSE) {
    cout << " Added overlap:" << endl;
    p_olap->print();
  }

  INode* n1 = graph->get_node(p_olap->ridA);
  INode* n2 = graph->get_node(p_olap->ridB);
  
  graph->new_edge(n1, n2, p_olap);
}


void Unitigger::add_overlap(Overlap_t p_olap) {

  Overlap* olap = new Overlap();
  pair <ID_t, ID_t>  reads = p_olap.getReads();

  if(reads.first == reads.second) {
    cerr << "WARNING: Not adding overlap between read " << reads.first << " and itself " << endl;
    return;
  }
  
  olap->ridA = reads.first;
  olap->ridB = reads.second;
  olap->ori = p_olap.getAdjacency();
  olap->ahang = p_olap.getAhang();
  olap->bhang = p_olap.getBhang();

  add_overlap(olap);
}


void Unitigger::add_read(Read_t p_read) {

  Range_t range = p_read.getClearRange();
  
  Read* read = new Read(p_read.getIID(), range.getLength());
  read->range = range;
  if(VERBOSE) read->print();

  INode* n = graph->new_node(read->id, read);
  n->setInterval(read->len);
}


void Unitigger::add_read(Read* p_read) {

  INode* n = graph->new_node(p_read->id, p_read);
  n->setInterval(p_read->len);
}


void Unitigger::output_umd_contigs(IGraph* g, INode* p_node) {
  Read* read = (Read*) p_node->getElement();
  if(VERBOSE) cout << read->id << ' ' << read->start << ' ' << read->end << endl;
  p_node->setFlags(0x1);
  
  list< INode* > nodes = g->adjacent_nodes(p_node);
  INode* node;

  for(nodeListIter iter = nodes.begin(); iter != nodes.end(); ++iter) {
    node = (*iter);
    if(node->getFlags() != 0x1) {
      //      output_contigs(g, node);
    }
  }
}

void Unitigger::output_amos_contigs(const string p_bankdir) {
  Message_t msg;
  BankStream_t bank(Layout_t::NCODE);

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

  vector< Contig* >::iterator contig_iter = contigs.begin();
  for( ; contig_iter != contigs.end(); ++contig_iter) {
    Contig* ctg = (*contig_iter);
    Tile_t tile;
    Layout_t amos_ctg;
    vector<Tile_t> tiles;

    // loop over every node in subgraph
    for(INodeIterator nodeIter = ctg->sg->nodes_begin(); nodeIter != ctg->sg->nodes_end(); ++nodeIter) {
      INode* node = (*nodeIter).second;
      Read* read_tile = (Read*) node->getElement();
      
      tile.source = read_tile->id;

      if(read_tile->start < read_tile->end) {

		tile.offset = read_tile->start;
		tile.range = read_tile->range;

      } else {

		tile.range = read_tile->range;
		tile.range.swap();
		tile.offset = read_tile->end;
		
      }
      
      if(VERBOSE) {
		cout << " -> " << read_tile->id << " len " << read_tile->len;
		cout << " (" << read_tile->start << "," << read_tile->end << ") ";
      }

      tiles.push_back(tile);
    }

	if(tiles.size() > 1) {
	  amos_ctg.setTiling(tiles);
  
	  bank << amos_ctg;
	}
  }
  
  bank.close();
}


// void Unitigger::output_amos_layout(const string p_bankdir) {
//   Message_t msg;
//   BankStream_t bank(Layout_t::NCODE);

//   try {
//     if(!bank.exists(p_bankdir)) {
//       bank.create(p_bankdir);
//     } else {
//       bank.open(p_bankdir);
//     }
//     cout << " Writing layouts to bank " << p_bankdir << endl;
//   } catch (Exception_t &e) {
//     cerr << "Exception while writing layouts : " << e << endl;
//     exit(127);
//   }

//   vector< Contig* >::iterator contig_iter = contigs.begin();
//   for( ; contig_iter != contigs.end(); ++contig_iter) {
//     Contig* ctg = (*contig_iter);
//     Tile_t tile;
//     Layout_t layout;
//     vector<Tile_t> tiles;

//     // loop over every node in subgraph
//     for(INodeIterator nodeIter = ctg->sg->nodes_begin(); nodeIter != ctg->sg->nodes_end(); ++nodeIter) {
//       INode* node = (*nodeIter).second;
//       Read* read_tile = (Read*) node->getElement();
      
//       tile.source = read_tile->id;

//       if(read_tile->start < read_tile->end) {

// 		tile.offset = read_tile->start;
// 		tile.range = read_tile->range;

//       } else {

// 		tile.range = read_tile->range;
// 		tile.range.swap();
// 		tile.offset = read_tile->end;
		
//       }
      
//       if(VERBOSE) {
// 		cout << " -> " << read_tile->id << " len " << read_tile->len;
// 		cout << " (" << read_tile->start << "," << read_tile->end << ") ";
//       }

//       tiles.push_back(tile);
//     }

// 	if(tiles.size() > 1) {
// 	  layout.setTiling(tiles);
  
// 	  bank << layout;
// 	}
//   }
  
//   bank.close();
// }


void Unitigger::hide_containment(IGraph* g) {
  IEdge* edge;
  Overlap* olap;
  Read* read;
  INode* node;
  
  // loop over all edges to find containment overlaps
  for(IEdgeIterator edges = g->edges_begin(); edges != g->edges_end(); ++edges) {
    edge = (*edges).second;
    olap = (Overlap *)edge->getElement();
    
    if(olap->type == 'C') {
      node = edge->getTarget();
      read = (Read*) node->getElement();
      if(read->id != olap->ridB) {
	cerr << " Error overlap B id " << olap->ridB << " doesn't match read id " << read->id << endl;
      }

      // only add the node once to the containment queue
      if(!node->getHidden()) {
	//	cout << " add node to containment " << node->getKey() << endl;
	containment.push(edge);
      }
      node->setHidden(true);
    }
  }

  cout << " total contained reads hidden " << containment.size() << endl;
}


void Unitigger::add_containment() {
  IEdge* edge;
  INode* con_node;
  INode* node;
  Overlap* ovl;
  int count = 0;


  while(!containment.empty()) {
    int size = containment.size();
    int pass_count = 0;
    
    cout << "this pass containment size is " << size << endl;
    
    for(int i = size; i > 0; i--) {
      edge = containment.front();
      containment.pop();
      ovl = (Overlap*) edge->getElement();
      con_node = edge->getTarget();
      
      if(con_node->getKey() == ovl->ridA) {
		node = con_node;
		con_node = edge->getSource();
      } else {
		node = edge->getSource();
      }
      
      vector< Contig* >::iterator contig_iter = contigs.begin();
      bool added = false;

      //      cout << " start looking for contig that contains read " << node->getKey() << endl;
      //
      // TODO : for a large number of contigs, 
      // finding the right contig can be very, very slow
      for( ; contig_iter != contigs.end(); ++contig_iter) {
		Contig* c = (*contig_iter);
		if(c->sg->contains(node)) {
		  pass_count++;
		  c->sg->add_edge(edge);
		  con_node->setNodeHidden(false);
		  edge->setHidden(false);
		  edge->setColor("purple");
		  con_node->setColor("purple");
		  added = true;
		  break;
		}
      }
      
      // TODO: fix if read contains self
      if(!added) {
		//cout << " not added  " << con_node->getKey() << " " << node->getKey() << endl;
		containment.push(edge);
      }
    } // for containment size
	
    cout << pass_count << " containment reads added back on this pass " << endl;
    count += pass_count;
    cout << " sub-total contained reads unhidden " << count << endl;

    if(containment.size() == size) { // we didn't remove any nodes BAD
      cout << " containment loop found " << endl;
      //      cout << " queue contains: " << endl;
      //      cout << " [ read1, read2, overlap ]" << endl;
      while(!containment.empty()) {
	IEdge* e2 = containment.front();
	containment.pop();
	int read1 = edge->getTarget()->getKey();
	int read2 = edge->getSource()->getKey();
	//	cout << "[ " << read1 << ", " << read2 << ", " << e2->getKey() << " ]" << endl;
      }
    }

  } 
  cout << " total contained reads unhidden " << count << endl;
}

// TODO: refactor 
// TODO: better handle two distinct overlaps between reads
void Unitigger::hide_transitive_overlaps(IGraph* g) {
  queue< INode* > q; // queue of gray nodes
  queue< INode* > children;
  map< int, IEdge* > parents; // parent mapping
  queue< IEdge* > trans; // trans edges that were found

  graph->clear_flags();

  // loop over all nodes 
  for(INodeIterator nodeIter = g->nodes_begin(); nodeIter != g->nodes_end(); ++nodeIter) {
    INode* root_node = (*nodeIter).second;
    int depth = 1;

    // look for one that isn't hidden and hasn't been visited by BFS
    // use as root for BFS
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
	for(edgeListIter iter = inc_edges.begin(); iter != inc_edges.end(); ++iter) {
	  cur_edge = (*iter);
	  child = cur_edge->opposite(cur_node);

	  if(child == NULL) {
	    cout << " ERROR null node in graph " << endl;
	    exit(1);
	  }
	  
	  if(child->getFlags() == 0) { // hasn't  been visited
	    child->setDepth(depth + 1);
	    child->setFlags(1); // gray
	    parents[child->getKey()] = cur_edge;
	    q.push(child);  // push onto gray queue

	    child->setParent(cur_node->getKey());
	    children.push(child); // this nodes children
	    //	    cout << " " << child->getKey();


	  } else if(child->getFlags() == 1) {
	    if(child->getParent() == -1) {
	      child->setParent(cur_node->getKey());
	      children.push(child);
	      //	      cout << " " << child->getKey();
	    } else {
	      //	      cout << " " << child->getKey() << "(dup)";
	    }

	    parents[child->getKey()] = cur_edge;

	  } else { // flags should be 2/black
	    //	    cout << " " << child->getKey() << "(done)";

	  }
	}


	// look for transitive edges
	//cout << endl << " start looking for 3 cycles for children of node " << cur_node->getKey() << endl;
	while(! children.empty()) {
	  IEdge* grand_edge;
	  INode* grand_node = children.front();
	  children.pop();
	  INode* node2;
	  
	  list< IEdge* > grand = g->incident_edges(grand_node);
	  for(edgeListIter iter = grand.begin(); iter != grand.end(); ++iter) {
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
		  cout << pkey << " ";
		  cout << gkey << " " << nkey << endl;
		}
		
		Overlap* o1 = (Overlap *)grand_edge->getElement();
		Overlap* o2 = (Overlap *)parents[grand_node->getKey()]->getElement();
		Overlap* o3 = (Overlap *)parents[node2->getKey()]->getElement();

		if(o2->ridA == pkey) {
		  suffix1 = o2->asuffix;
		} else if(o2->ridB == pkey) {
		  suffix1 = o2->bsuffix;
		}
		
		if(o3->ridA == pkey) {
		  suffix2 = o3->asuffix;
		} else if(o3->ridB == pkey) {
		  suffix2 = o3->bsuffix;
		}
		
		if(suffix1 != suffix2) {
		  trans.push(grand_edge);
		}
		
		if(o1->ridA == gkey) {
		  suffix1 = o1->asuffix;
		} else if(o1->ridB == gkey) {
		  suffix1 = o1->bsuffix;
		}
		
		if(o2->ridA == gkey) {
		  suffix2 = o2->asuffix;
		} else if(o2->ridB == gkey) {
		  suffix2 = o2->bsuffix;
		}
		
		if(suffix1 != suffix2) {
		  trans.push(parents[node2->getKey()]);
		}
		
		
		if(o1->ridA == nkey) {
		  suffix1 = o1->asuffix;
		} else if(o1->ridB == nkey) {
		  suffix1 = o1->bsuffix;
		}
		
		if(o3->ridA == nkey) {
		  suffix2 = o3->asuffix;
		} else if(o3->ridB == nkey) {
		  suffix2 = o3->bsuffix;
		}
		
		if(suffix1 != suffix2) {
		  trans.push(parents[grand_node->getKey()]);
		}
				
		
	      }
	    } // end transitive check for node2
	    
	  }
	  grand_node->setParent(-1);
	}
      }
      
      int count = 0;
      // hide transitive edges
      while(! trans.empty()) {
	if(! trans.front()->getHidden()) {
	  count++;
	}
	trans.front()->setHidden(true);
	trans.pop();
      }
      
    }

  }


  g->clear_flags();
}


bool Unitigger::isSuffix(Read* read, Overlap* ovl) {
  if(read->id == ovl->ridA) return ovl->asuffix;
  if(read->id == ovl->ridB) return ovl->bsuffix;
  // TODO: should through exception?
  return false;
}


void Unitigger::find_chunks() {
  INode* node;
  int count = 0;
  
  graph->clear_flags();

  for(INodeIterator nodes = graph->nodes_begin(); nodes != graph->nodes_end(); ++nodes) {
    node = (*nodes).second;
    if((node->getFlags() != 1) && (! node->getHidden())) {
      contigs.push_back(walk(node));
    }
  }

  cout << " number of contigs " << contigs.size() << endl;
}

//
// 1. add edge and node to subgraph if valid
// 2. return null if no more edges
//
IEdge* Unitigger::walk_edge(IEdge* e, INode* n, Contig* ctg) {
  SubGraph* sg = ctg->sg;
  Read* read = (Read*) n->getElement();
  Overlap* in_ovl = (Overlap*) e->getElement();
  bool needSuffix = isSuffix(read, in_ovl);
  IEdge* path;
  int imatch = 0;
  int omatch = 0;
  bool cur = e->getHidden();
  
  e->setHidden(true);

  // loop through all edges, checking if we can walk
  // from this node
  list< IEdge* > edges =  graph->incident_edges(n);
  for(edgeListIter iter = edges.begin(); iter != edges.end(); ++iter) {
    IEdge* oedge = (*iter);
    Overlap* out_ovl = (Overlap*) oedge->getElement();

    if(needSuffix != isSuffix(read, out_ovl)) {
      omatch++;
      path = oedge;
    } else {
      imatch++;
    }

  }

  e->setHidden(cur);

  if(imatch == 0) {
    sg->add_edge(e);
    e->setFlags(1);
    n->setFlags(1);
    ctg->start_node = n->getKey();
  }
  
  if((omatch == 1) && (imatch == 0)) {
    return path;
  } else {
    return NULL;
  }
}


Contig* Unitigger::walk(INode* p_node) {
  Contig* ctg = new Contig();
  ctg->sg = new SubGraph(*graph, "Contig");
  ctg->start_node = p_node->getKey();
  IEdge* edge;
  IEdge* prefix;
  IEdge* suffix;
  Overlap* ovl;
  Read* read = (Read*) p_node->getElement();
  int smatch = 0;
  int pmatch = 0;
  

  // loop through all edges, checking if we can walk
  // from this node
  list< IEdge* > edges =  graph->incident_edges(p_node);
  for(edgeListIter iter = edges.begin(); iter != edges.end(); ++iter) {
    edge = (*iter);
    ovl = (Overlap*) edge->getElement();

    if(isSuffix(read, ovl)) {
      smatch++;
      suffix = edge;
    } else {
      pmatch++;
      prefix = edge;
    }

  }

  ctg->sg->add_node(p_node);
  p_node->setFlags(1);

  INode* node2;
  if(pmatch == 1) { // we can walk off the prefix overlap
    node2 = p_node;
    edge = prefix;
	
    while(edge != NULL) {
      node2 = edge->opposite(node2);
      if(node2 != p_node) {
		//	cout << " walk edge " << edge->getKey() << " with node " << node2->getKey() << endl;
		edge = walk_edge(edge, node2, ctg);
      } else {
		edge = NULL;
		smatch = 0;
      }
    }
  }


  if(smatch == 1) { // we can walk off the suffix overlap
    node2 = p_node; // start node
    edge = suffix; // start at suffix edge
	
    while(edge != NULL) {
      node2 = edge->opposite(node2);
      if(node2 != p_node) {
		//	cout << " walk edge " << edge->getKey() << " with node " << node2->getKey() << endl;
		edge = walk_edge(edge, node2, ctg);
      } else {
		edge = NULL;
      }
    }
  }

  return ctg;
}


void Unitigger::layout_contig(Contig* ctg) {
  INode* first_node = ctg->sg->get_node(ctg->start_node);
  Read* read = (Read*) first_node->getElement();
  IEdge* edge;
  Overlap* ovl;
  int count = 0;
  queue< INode* > q;

  read->start = 0;
  read->end = read->len;

  ctg->sg->clear_flags();

  list< IEdge* > edges = ctg->sg->incident_edges(first_node);

  // loop over edges
  // find no containment edge
  // there should be only one or there is a problem
  // check overlap on edge
  for(edgeListIter iter = edges.begin(); iter != edges.end(); ++iter) {
    edge = (*iter);
    ovl = (Overlap*) edge->getElement();

    if(ovl->type != 'C') {
      count++;
      if(isSuffix(read, ovl)) {
		read->start = 0;
		read->end = (read->len);
      } else {
		read->start = (read->len);
		read->end = 0;
      }
    }

  }

  if(count > 1) {
    cerr << " *******Error: First read had more than one non-containment overlap for layout " << endl;
  }

  // now that the first read is set 
  // go through whole contig

  INode* cur_node;
  IEdge* cur_edge;
  q.push(first_node);

  while(!q.empty()) {
    cur_node = q.front();
    q.pop();
    cur_node->setFlags(2); // black
	
    // go over each child and mark/queue
    list< IEdge* > inc_edges = ctg->sg->incident_edges(cur_node);
    for(edgeListIter iter = inc_edges.begin(); iter != inc_edges.end(); ++iter) {
      cur_edge = (*iter);
      INode* child = cur_edge->opposite(cur_node);
      if(child->getFlags() == 0) { // hasn't  been visited
		child->setFlags(1); // gray
		q.push(child);
		layout_read(cur_edge, child);
      }
    }
  }

}


// need start node and sub-graph
void Unitigger::layout_read(IEdge* p_edge, INode* p_node) {
  Overlap* olap = (Overlap*) p_edge->getElement();
  // read that should be set
  Read* lay_read = (Read*) p_node->getElement(); 
  // read to layout
  Read* set_read = (Read*) p_edge->opposite(p_node)->getElement();

  if(VERBOSE) {
    cout << " layout read " << lay_read->id << " against " << set_read->id << endl;
    set_read->print();
    olap->print();
  }
  
  int ahang = olap->ahang;
  int len = lay_read->len;

  if(set_read->id == olap->ridA) { // need to layout B given A
    if(set_read->start > set_read->end) { // invert coordinates
      ahang = - ahang;
      len = - len;
    }

    if(olap->type == 'C') {

      if(olap->ori == 'N') {

	lay_read->start = set_read->start + ahang;
	lay_read->end = lay_read->start + len;

      } else {	// innie

	lay_read->end = set_read->start + ahang;
	lay_read->start = lay_read->end + len;
      }


    } else if(olap->type == 'R') {

      lay_read->start = set_read->start + ahang;
      lay_read->end = lay_read->start + len;

    } else if((olap->type == 'P') || (olap->type == 'S')) {

      lay_read->end = set_read->start + ahang;
      lay_read->start = lay_read->end + len;
    }

  } else {  // need to layout A given B
    if((olap->ori == 'N') && (set_read->start > set_read->end)) { // invert coordinates
      ahang = - ahang;
      len = - len;
    } else if ((olap->ori == 'I') && (set_read->end > set_read->start)) { // invert coordinates
      ahang = - ahang;
      len = - len;
    }

    if(olap->type == 'C') {

      if(olap->ori == 'N') {
	lay_read->start = set_read->start - ahang;
	lay_read->end = lay_read->start + len;
      } else {	// innie
	lay_read->start = set_read->end - ahang;
	lay_read->start = lay_read->end + len;
      }

    } else if(olap->type == 'R') {

      lay_read->start = set_read->start - ahang;
      lay_read->end = lay_read->start + len;

    } else if((olap->type == 'P') || (olap->type == 'S')) {

      lay_read->start = set_read->end - ahang;
      lay_read->end = lay_read->start + len;
    }
  }

  if(lay_read->start < 0) {
    //cout << " negative start read " << lay_read->start << endl;
    lay_read->end -= lay_read->start;
    lay_read->start = 0;
  } else if(lay_read->end < 0) {
    // cout << " negative end read " << lay_read->end << endl;
    lay_read->start -= lay_read->end;
    lay_read->end = 0;
  }


  // check start
  if((lay_read->start < 0) || (lay_read->end < 0)) {
    cerr << " ***** layout read error: negative coordinates for " << lay_read->id << endl;
  }

  if(VERBOSE) {
    cout << " done " << lay_read->start << ", " << lay_read->end << endl;
    lay_read->print();
  }
  
}

void Unitigger::output_contig_graphs() {
  char buffer[30];

  vector< Contig* >::iterator contig_iter = contigs.begin();
  for(int i = 0; contig_iter != contigs.end(); ++contig_iter) {
    Contig* c = (*contig_iter);
    sprintf(buffer, "Contig-%d.dot", i++);
    if(c->sg->num_nodes() > 1) {
      c->sg->create_dot_file(buffer);
    }
  }
}


void Unitigger::calc_contigs() {

  //
  // Step 1. handle containment edges
  //
  hide_containment((IGraph*) graph);

  //
  // Step 2. transitive edges
  //
  hide_transitive_overlaps((IGraph*) graph);


  //
  // Step 3. Chunking
  //
  find_chunks();

  add_containment();

  //
  // Step 4. Layout
  //
  vector< Contig* >::iterator contig_iter = contigs.begin();
  contig_iter = contigs.begin();
  for( ; contig_iter != contigs.end(); ++contig_iter) {
    layout_contig((*contig_iter));
  }

}

