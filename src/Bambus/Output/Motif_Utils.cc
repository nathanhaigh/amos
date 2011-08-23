#include <string>
#include <map>
#include <set>
#include <queue>
#include <iterator>
#include <iostream>
#include <string>

#include "fasta.hh"
#include "datatypes_AMOS.hh"
#include "Bank_AMOS.hh"
#include "BankStream_AMOS.hh"

#include "Contig_AMOS.hh"
#include "ContigEdge_AMOS.hh"
#include "Scaffold_AMOS.hh"
#include "Motif_AMOS.hh"

#include "Utilities_Bundler.hh"

#include "Motif_Utils.hh"
#include "Position.hh"

#ifdef AMOS_HAVE_BOOST
#include <boost/config.hpp>
#include <boost/utility.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#endif //AMOS_HAVE_BOOST

#include "Bank_AMOS.hh"
#include "BankStream_AMOS.hh"

using namespace std;
using namespace HASHMAP;
using namespace AMOS;

#ifdef AMOS_HAVE_BOOST
/* give access to common Boost data types through our interface */
// Vertex properties
typedef boost::property<boost::vertex_name_t, uint32_t> VertexProperty;
// Adjacency graph using list data structure
typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS, VertexProperty> Graph;
// Accessors
typedef boost::graph_traits<Graph>::vertex_descriptor Vertex;
typedef boost::graph_traits<Graph>::edge_descriptor Edge;
typedef boost::graph_traits<Graph>::vertex_iterator VertexIterator;
typedef boost::graph_traits<Graph>::out_edge_iterator EdgeIterator;

// Properties
typedef boost::property_map<Graph, boost::vertex_name_t>::type VertexName;
#endif //AMOS_HAVE_BOOST

#ifdef AMOS_HAVE_BOOST
Vertex computeSource(Graph g); 
Position traverseRecursive(
                                Bank_t &contig_bank,
                                Graph g,
                                ID_t current,
                                hash_map<ID_t, Tile_t, hash<ID_t>, equal_to<ID_t> >& nodeToTile,
                                hash_map<ID_t, Vertex, hash<ID_t>, equal_to<ID_t> >& nodeToDescriptor,
                                hash_map<ID_t, uint32_t, hash<ID_t>, equal_to<ID_t> > &visited,
                                hash_map<ID_t, vector<Position>, hash<ID_t>, equal_to<ID_t> > &paths,
                                hash_map<ID_t, Position, hash<ID_t>, equal_to<ID_t> > &longest,
                                vector<Position> &edits,
                                hash_map<ID_t, string, hash<ID_t>, equal_to<ID_t> > &seq,
                                hash_map<ID_t, string, hash<ID_t>, equal_to<ID_t> > &seqNames);
Position translateSetToPaths(
   Motif_t &scf,
   Bank_t &motif_bank, Bank_t &contig_bank, Bank_t &edge_bank,
   vector<Position>& edits, hash_map<ID_t, string, hash<ID_t>, equal_to<ID_t> > &seq,
   hash_map<ID_t, string, hash<ID_t>, equal_to<ID_t> > &seqNames);

Vertex computeSource(Graph g) {
	Vertex source;
	uint32_t counter = 0;
        hash_map<Vertex, bool, hash<Vertex>, equal_to<Vertex> > edgeIncoming;

	pair<VertexIterator, VertexIterator> i;
	for (i = boost::vertices(g); i.first != i.second; ++i.first) {
		Vertex first = *i.first;
		if (boost::in_degree(first, g) == 0) {
			if (counter != 0) {
				cerr << "Error: multiple source nodes for graph" << endl;
				exit(1);
			}
			else {
				counter++;
				source = first;
			}
		}
	}
	if (counter == 0) {
		cerr << "Error: no source node for graph" << endl;
		exit(1);
	} else {
		return (source);
	}
}

Position traverseRecursive(
				Bank_t &contig_bank,
				Graph g,
				ID_t current,
				hash_map<ID_t, Tile_t, hash<ID_t>, equal_to<ID_t> >& nodeToTile,
				hash_map<ID_t, Vertex, hash<ID_t>, equal_to<ID_t> >& nodeToDescriptor,
				hash_map<ID_t, uint32_t, hash<ID_t>, equal_to<ID_t> > &visited,
				hash_map<ID_t, vector<Position>, hash<ID_t>, equal_to<ID_t> > &paths,
				hash_map<ID_t, Position, hash<ID_t>, equal_to<ID_t> > &longest,
				vector<Position> &edits,
                                hash_map<ID_t, string, hash<ID_t>, equal_to<ID_t> > &seq,
				hash_map<ID_t, string, hash<ID_t>, equal_to<ID_t> > &seqNames) {
        // need to get offset and range for the sequence
        Tile_t tile = nodeToTile[current];
        Position p;

	if (tile.source_type == Contig_t::NCODE) {
           Contig_t ctg;
	   contig_bank.fetch(current, ctg);
           string gapped = ctg.getSeqString(tile.range);
           p = Position(ctg.getEID(), tile.offset, tile.offset + tile.range.getLength(), gapped);
        } else if (seq.find(tile.source) != seq.end()) {
           // get pre-made sequence
           p = Position(seqNames[tile.source], tile.offset, tile.offset + tile.range.getLength(), seq[tile.source]);
        } else {
           fprintf(stderr, "Error: no sequence known for tile %d\n", tile.source);
           assert(0);
        }

	VertexName vertexNames = get(boost::vertex_name, g);

	if (visited.find(current) == visited.end()) {
		visited[current] = 1;

		// get edges
        EdgeIterator out_i, out_end;
        if (boost::out_degree(nodeToDescriptor[current], g) == 0) {
			paths[current].push_back(p);
			longest[current] = p;
        } else {
        	vector<Position> myPaths;
        	for (tie(out_i, out_end) = boost::out_edges(nodeToDescriptor[current], g); out_i != out_end; ++out_i) {
				myPaths.push_back(traverseRecursive(contig_bank, g, vertexNames[boost::target(*out_i, g)], nodeToTile, nodeToDescriptor, visited, paths, longest, edits, seq, seqNames));
			}

        	Position myLongestPath;
        	for (vector<Position>::const_iterator i = myPaths.begin(); i < myPaths.end(); i++) {
        		Position merged = p.merge(*i, edits);
        		paths[current].push_back(merged);

        		if (myLongestPath.getSequence().size() == 0 || merged.getLength() > myLongestPath.getLength()) {
        			myLongestPath = merged;
        		}
        	}
        	longest[current] = myLongestPath;
		}
	}
	return longest[current];
}

Position traverseSet(Motif_t &scf, Bank_t & contig_bank, Bank_t &edge_bank, vector<Position> &edits,
		hash_map<ID_t, vector<Position>, hash<AMOS::ID_t>, equal_to<AMOS::ID_t> > &paths,
		hash_map<AMOS::ID_t, Position, hash<AMOS::ID_t>, equal_to<AMOS::ID_t> > &longest,
                hash_map<ID_t, string, hash<ID_t>, equal_to<ID_t> > &seq,
		hash_map<ID_t, string, hash<ID_t>, equal_to<ID_t> > &seqNames) {
	// do a DFS traversal to find if we can skip the node
	hash_map<ID_t, uint32_t, hash<ID_t>, equal_to<ID_t> > visited;
	Graph g;
	hash_map<ID_t, Vertex, hash<ID_t>, equal_to<ID_t> > nodeToDescriptor;
	hash_map<ID_t, Tile_t, hash<ID_t>, equal_to<ID_t> > nodeToTile;
	VertexName vertexNames = get(boost::vertex_name, g);

	// build boost-based graph
    for (vector<Tile_t>::const_iterator tileIt = scf.getContigTiling().begin(); tileIt < scf.getContigTiling().end(); tileIt++) {
    	nodeToDescriptor[tileIt->source] = boost::add_vertex(g);
    	vertexNames[nodeToDescriptor[tileIt->source] ] = tileIt->source;
    	nodeToTile[tileIt->source] = *tileIt;
    }

    for (vector<ID_t>::const_iterator edgeIt = scf.getContigEdges().begin(); edgeIt < scf.getContigEdges().end(); edgeIt++) {
    	ContigEdge_t cte;
    	edge_bank.fetch((*edgeIt), cte);
        if (Bundler::isBadEdge(cte)) { 
           continue;
        }
        if (nodeToDescriptor.find(cte.getContigs().first) == nodeToDescriptor.end() || nodeToDescriptor.find(cte.getContigs().second) == nodeToDescriptor.end()) {
           continue;
        }
    	pair<Edge, bool> e = boost::add_edge(nodeToDescriptor[cte.getContigs().first], nodeToDescriptor[cte.getContigs().second], g);
    }

	return traverseRecursive(contig_bank, g, vertexNames[computeSource(g)], nodeToTile, nodeToDescriptor, visited, paths, longest, edits, seq, seqNames);
}

Position translateSetToPaths(Motif_t &scf, Bank_t &motif_bank, Bank_t &contig_bank, Bank_t &edge_bank, vector<Position>& edits, hash_map<ID_t, string, hash<ID_t>, equal_to<ID_t> > &seq, hash_map<ID_t, string, hash<ID_t>, equal_to<ID_t> > &seqNames) {
	Position result;
	hash_map<ID_t, vector<Position>, hash<ID_t>, equal_to<ID_t> > paths;
	hash_map<ID_t, Position, hash<ID_t>, equal_to<ID_t> > longest;

    for (std::vector<Tile_t>::const_iterator tileIt = scf.getContigTiling().begin(); tileIt < scf.getContigTiling().end(); tileIt++) {
    	if (tileIt->source_type == Motif_t::NCODE) {
    		Motif_t subScf;
    		motif_bank.fetch(tileIt->source, subScf);
    		Position subResult = translateSetToPaths(subScf, motif_bank, contig_bank, edge_bank, edits, seq, seqNames);
                seq[tileIt->source] = subResult.getSequence();
                seqNames[tileIt->source]  = subScf.getEID();
                result = result.merge(subResult, edits);
    	}
    }
    result = result.merge(traverseSet(scf, contig_bank, edge_bank, edits, paths, longest, seq, seqNames), edits);

	// now that we have built up the paths, add the replacements to the edit list
    for (hash_map<ID_t, vector<Position>, hash<ID_t>, equal_to<ID_t> >::iterator it = paths.begin(); it != paths.end(); it++) {
    	for (vector<Position>::iterator ctg = it->second.begin(); ctg < it->second.end(); ctg++) {
    		if ((*ctg) != (longest[it->first])) {
    			ctg->setEditType(Position::REPLACE);
				edits.push_back(*ctg);
			}
		}
	}

	return result;
}
#endif

namespace Bundler
{
string outputMotif(string &name, Motif_t &scf, Bank_t &motif_bank, Bank_t &contig_bank, Bank_t &edge_bank, bool print) {
#ifdef AMOS_HAVE_BOOST
        vector<Position> edits;
        hash_map<ID_t, string, hash<ID_t>, equal_to<ID_t> > seq;
        hash_map<ID_t, string, hash<ID_t>, equal_to<ID_t> > seqNames;
        Position result = translateSetToPaths(scf, motif_bank, contig_bank, edge_bank, edits, seq, seqNames);
        // print the main sequence
        if (print) Fasta_Print(stdout, result.getUngappedSequence().c_str(), result.getName().c_str());

    // output the edits
    for (vector<Position>::const_iterator it = edits.begin(); it < edits.end(); it++) {
        stringstream header;
        header << it->getName() << " "
                        << (it->getEditType() == Position::REPLACE ? "REPLACE" : "EDIT") << " RANGE ("
                        << (it->getStart()-result.getStart()) << ", "
                        << (it->getEnd()-result.getStart()) << ")";

        if (print) Fasta_Print(stdout, it->getUngappedSequence().c_str(), header.str().c_str());
    }

    name = result.getName();
    return result.getSequence();
#else
    cerr << "Error: the boost library cannot be found. Will not be outputting motif " << scf.getEID() << " consisting of " << scf.getContigTiling().size() << endl;

   return "";
#endif
}

string outputMotif(string &name, Motif_t &scf, Bank_t &motif_bank, Bank_t &contig_bank, Bank_t &edge_bank) {
   return outputMotif(name, scf, motif_bank, contig_bank, edge_bank, true);
}
}
