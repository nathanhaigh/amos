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

#include "Motif_Utils.hh"
#include "Motif_Sequence.hh"
#include "Position.hh"

using namespace std;
using namespace HASHMAP;
using namespace AMOS;
using namespace Output;

HASHMAP::hash_map<AMOS::ID_t, std::string, HASHMAP::hash<AMOS::ID_t>, HASHMAP::equal_to<AMOS::ID_t> > *motifSeq = NULL;
HASHMAP::hash_map<AMOS::ID_t, std::string, HASHMAP::hash<AMOS::ID_t>, HASHMAP::equal_to<AMOS::ID_t> > *motifName = NULL;

void getMotifsFromBank(AMOS::Bank_t &motif_bank, AMOS::Bank_t &contig_bank, AMOS::Bank_t &edge_bank) {
   // get motif sequence
   AMOS::Motif_t mtf;
   AMOS::Scaffold_t scf;
   std::string name;
   motifSeq = new HASHMAP::hash_map<AMOS::ID_t, std::string, HASHMAP::hash<AMOS::ID_t>, HASHMAP::equal_to<AMOS::ID_t> >();
   motifName = new HASHMAP::hash_map<AMOS::ID_t, std::string, HASHMAP::hash<AMOS::ID_t>, HASHMAP::equal_to<AMOS::ID_t> >();

   for (AMOS::IDMap_t::const_iterator ci = motif_bank.getIDMap().begin(); ci; ci++) {
      motif_bank.fetch(ci->iid, mtf);
      (*motifSeq)[ci->iid] = Output::outputMotif(name, mtf, motif_bank, contig_bank, edge_bank, false);
      (*motifName)[ci->iid] = name;
   }
}

#ifdef AMOS_HAVE_BOOST
Position traverseRecursive(
                                Bank_t &contig_bank,
                                Graph &g,
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

Position traverseRecursive(
				Bank_t &contig_bank,
				Graph &g,
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
        	for (vector<Position>::iterator i = myPaths.begin(); i < myPaths.end(); i++) {
        		Position merged = p.merge(*i, edits);
        		paths[current].push_back(merged);
        		if (myLongestPath.getSequence().size() == 0 || merged.getUngappedLength() > myLongestPath.getUngappedLength()) {
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

	buildMotifGraph(g, nodeToDescriptor, nodeToTile, scf, edge_bank);
	VertexName vertexNames = get(boost::vertex_name, g);

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
    	}
    }
    Position p = traverseSet(scf, contig_bank, edge_bank, edits, paths, longest, seq, seqNames);
    result = result.merge(p, edits);

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

namespace Output
{
std::string getTileSequence(AMOS::Bank_t &contig_bank, AMOS::Bank_t &motif_bank, AMOS::Bank_t &edge_bank, AMOS::ID_t max, AMOS::ID_t source, AMOS::Range_t range) {
   std::string eid;
   return getTileSequence(contig_bank, motif_bank, edge_bank, max, source, range, eid);
}

std::string getTileSequence(AMOS::Bank_t &contig_bank, AMOS::Bank_t &motif_bank, AMOS::Bank_t &edge_bank, AMOS::ID_t max, AMOS::ID_t source, AMOS::Range_t range, std::string &eid) {
   if (motifName == NULL) {
      getMotifsFromBank(motif_bank, contig_bank, edge_bank);
   }

   if (source > max) {
#ifdef AMOS_HAVE_BOOST
      if (motifSeq->find(source) == motifSeq->end()) {
         cerr << "Error: unknown contig id " << source << endl;
         exit(1);
      }
      eid = (*motifName)[source];
      return (*motifSeq)[source];
#else
   cerr << "Error: the boost library cannot be found. Will not be outputting tile " << source << endl;
   return "";
#endif
   } else {
      AMOS::Contig_t ctg;
      contig_bank.fetch(source, ctg);
      eid = ctg.getEID();
      return ctg.getSeqString(range);
   }
}

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
        header << result.getName() << "_" << it->getName() << "_"
                        << (it->getEditType() == Position::REPLACE ? "REPLACE" : "EDIT") << "_RANGE_("
                        << (it->getStart()-result.getStart()) << "_"
                        << (it->getEnd()-result.getStart()) << ")";

        if (print) Fasta_Print(stdout, it->getUngappedSequence().c_str(), header.str().c_str());
    }

    name = result.getName();
    return result.getSequence();
#else
    cerr << "Error: the boost library cannot be found. Will not be outputting motif " << scf.getEID() << " consisting of " << scf.getContigTiling().size() << " nodes" << endl;

   return "";
#endif
}

string outputMotif(string &name, Motif_t &scf, Bank_t &motif_bank, Bank_t &contig_bank, Bank_t &edge_bank) {
   return outputMotif(name, scf, motif_bank, contig_bank, edge_bank, true);
}
}
