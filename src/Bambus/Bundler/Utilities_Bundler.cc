#include <time.h>

#include "Utilities_Bundler.hh"

#include "Library_AMOS.hh"
#include "Fragment_AMOS.hh"
#include "Overlap_AMOS.hh"
#include "Contig_AMOS.hh"
#include "ContigLink_AMOS.hh"
#include "ContigEdge_AMOS.hh"

namespace Bundler
{
// internal-only definitions   
AMOS::Bank_t *global_edge_bank = NULL;

void outputEdge(std::ofstream &stream, AMOS::Bank_t &edge_bank, AMOS::ID_t iid, const char *start, const char *end, const char *direction);
void outputEdges(std::ostream &stream, AMOS::Bank_t &edge_bank, AMOS::ID_t currCtg, int32_t edgeTypes[], int32_t debug);
void outputLibrary(const std::string &outputPrefix, int32_t debug);
void outputEvidenceXML(const std::string &bank, AMOS::Bank_t &contig_bank, const std::string &outputPrefix, int32_t debug);
void outputOutXML(AMOS::Bank_t &edge_bank, const std::vector<AMOS::Scaffold_t> &scaffs, const std::string &outputPrefix, int32_t debug);

void outputAGP(AMOS::Bank_t &contig_bank, const std::vector<AMOS::Scaffold_t> &scaffs, const std::string& outputPrefix, int32_t debug);
void outputDOT(AMOS::Bank_t &contig_bank, AMOS::Bank_t &edge_bank,
               std::vector<AMOS::Scaffold_t> &scaffs, 
               HASHMAP::hash_map<AMOS::ID_t, contigOrientation, HASHMAP::hash<AMOS::ID_t>, HASHMAP::equal_to<AMOS::ID_t> >& ctg2ort,
               const std::string &outputPrefix,
               int32_t debug);
void outputBAMBUS(const std::string &bank, 
               AMOS::Bank_t &contig_bank, 
               AMOS::Bank_t &edge_bank, 
               const std::vector<AMOS::Scaffold_t> &scaffs, 
               const std::string &outputPrefix, 
               int32_t debug);


struct EdgeOrderCmp
{
  bool operator () (const AMOS::ID_t & a, const AMOS::ID_t & b)
  {
      assert(global_edge_bank);
      AMOS::ContigEdge_t ctgA, ctgB;
      global_edge_bank->fetch(a, ctgA);
      global_edge_bank->fetch(b, ctgB);

      if (ctgA.getContigs().first != ctgB.getContigs().first) {
         if (ctgA.getContigs().first < ctgB.getContigs().first) return true;
         if (ctgA.getContigs().first > ctgB.getContigs().first) return false;
      }
      if (ctgA.getContigs().second != ctgB.getContigs().second) {
         if (ctgA.getContigs().second < ctgB.getContigs().second) return true;
         if (ctgA.getContigs().second > ctgB.getContigs().second) return false;
      }
      
      if (!isBadEdge(ctgA)) return true;

      return false;
  }
};

double computeArrivalRate(const std::vector<AMOS::Contig_t> &contigs) {
   double result = 0;
   int32_t numFragments = 0;
   
   for (std::vector<AMOS::Contig_t>::const_iterator i = contigs.begin(); i < contigs.end(); i++) {
      // compute global arrival rate
      result += i->getAvgRho();
      numFragments += i->getReadTiling().size();
   }
   if (result == 0) {
      return 0;
   }

   return numFragments / result;
}

#ifdef AMOS_HAVE_BOOST
void buildGraph(
            Graph &g, 
            AMOS::BankStream_t &node_stream, AMOS::BankStream_t &edge_stream, 
            AMOS::IBankable_t *node, AMOS::IBankable_t *edge,
            int32_t redundancy) {
   // build boost-based graph   
   HASHMAP::hash_map<AMOS::ID_t, Vertex, HASHMAP::hash<AMOS::ID_t>, HASHMAP::equal_to<AMOS::ID_t> > nodeToDescriptor;
   VertexName vertexNames = get(boost::vertex_name, g);
   EdgeWeight edgeWeights = get(boost::edge_weight, g);
   
   // build boost-based graph
   // add node 0, it represents links outside of the set of contings (for example to singletons)
   nodeToDescriptor[0] = boost::add_vertex(g);

   while (node_stream >> (*node)) {
      assert(node->getIID() != 0);
      nodeToDescriptor[node->getIID()] = boost::add_vertex(g);
      vertexNames[nodeToDescriptor[node->getIID()] ] = node->getIID();
   }
   node_stream.seekg(0,AMOS::BankStream_t::BEGIN);

   while (edge_stream >> (*edge)) {
      AMOS::ID_t firstNode, secondNode;
      double_t weight = 0;
      if (dynamic_cast<AMOS::ContigEdge_t *>(edge) != NULL) {
         AMOS::ContigEdge_t* cte = dynamic_cast<AMOS::ContigEdge_t *>(edge);
         if (cte->getContigLinks().size() < redundancy || cte->getContigLinks().size() == 0) {
            continue;
         }
         
         firstNode = cte->getContigs().first;
         secondNode = cte->getContigs().second;
         weight = ((double)1 / cte->getContigLinks().size());
      }
      else if (dynamic_cast<AMOS::Overlap_t *>(edge) != NULL) {
         AMOS::Overlap_t* overlap = dynamic_cast<AMOS::Overlap_t*>(edge);
         firstNode = overlap->getReads().first;
         secondNode = overlap->getReads().second;
         weight = 1;
      }
      else {
         std::cerr << "buildGraph(): error, unknown type of edge bank " << edge_stream.getType() << " skipping record" << std::endl;
         continue;
      }
      std::pair<Edge, bool> e = boost::add_edge(nodeToDescriptor[firstNode], nodeToDescriptor[secondNode], g);
      edgeWeights[e.first] = weight;
   }
   edge_stream.seekg(0,AMOS::BankStream_t::BEGIN);

   std::cerr << "Built graph of " << boost::num_vertices(g) << " vertices and " << boost::num_edges(g) << " edges" << std::endl;
}
#else
void buildGraph(
            Graph &g, 
            AMOS::BankStream_t &node_stream, AMOS::BankStream_t &edge_stream, 
            AMOS::IBankable_t *node, AMOS::IBankable_t *edge,
            int32_t redundancy) {
   std::cerr << "Unable to build graph. Boost library is required. Please double check your installation and recompile AMOS" << std::endl;
}
#endif //AMOS_HAVE_BOOST

AMOS::ID_t getEdgeDestination(const AMOS::ID_t &edgeSrc, const AMOS::ContigEdge_t &cte) {
   if (cte.getContigs().second == edgeSrc) {
      return cte.getContigs().first;
   }
   else {
      return cte.getContigs().second;
   }
}

contigOrientation getOrientation(const AMOS::ID_t &contig, contigOrientation &myOrient, const AMOS::ContigEdge_t &cte) {
   assert(myOrient != NONE);

   switch (cte.getAdjacency()) {
      case AMOS::ContigEdge_t::NORMAL:
      case AMOS::ContigEdge_t::ANTINORMAL:
         // incase of normal or anti-normal, our directions match
         return myOrient;
         break;
      case AMOS::ContigEdge_t::INNIE:
      case AMOS::ContigEdge_t::OUTIE:
         if (myOrient == FWD) { return REV;} else { return FWD;}
         break;
      default:
         std::cerr << "Unknown orientation " << cte.getAdjacency() << std::endl;
         exit(1);
   };
}

bool isBadEdge(const AMOS::ContigEdge_t &cte) {
   if (cte.getStatus() != GOOD_EDGE && cte.getStatus() != NULL_STATUS) {
      return true;
   }
   
   return false;
}

void outputAGP(AMOS::Bank_t &contig_bank, const std::vector<AMOS::Scaffold_t> &scaffs, const std::string &outputPrefix, int32_t debug) {
   AMOS::Contig_t ctg;

   std::string outputFile = outputPrefix + ".agp";
   std::ofstream stream;
   stream.open(outputFile.c_str(), std::ios::out);

   stream << "# DESCRIPTION: WGS SCAFFOLDS GENERATED FROM AMOS BANK" << std::endl;

   for(std::vector<AMOS::Scaffold_t>::const_iterator itScf = scaffs.begin(); itScf < scaffs.end(); itScf++) {
      int32_t counter = 0;
      int32_t lastEnd = 0;

      for (std::vector<AMOS::Tile_t>::const_iterator tileIt = itScf->getContigTiling().begin(); tileIt < itScf->getContigTiling().end(); tileIt++) {
         // output the gap
         if (lastEnd != 0 && (tileIt->offset - lastEnd != 0)) {
            stream << itScf->getIID();
            stream << "\t" << lastEnd << "\t" << tileIt->offset;
            stream << "\t" << counter;
            stream << "\t" << (tileIt->offset - lastEnd + 1);
            stream << "\tfragment\tyes";
            
            stream << std::endl;
            counter++;
         }

         std::string sign = "+";
         if (tileIt->range.isReverse()) { sign = "-"; }

         stream << itScf->getIID();
         stream << "\t" << (tileIt->offset+1) << "\t" << (tileIt->offset+tileIt->range.getLength());
         stream << "\t" << counter << "\tW";

         int32_t outputID = 0;
         if (tileIt->source <= contig_bank.getMaxIID()) {
            contig_bank.fetch(tileIt->source, ctg);
            if (ctg.getEID().size() != 0) {
               stream << "\t" << ctg.getEID();
               outputID = 1;
            }
         }
         if (outputID == 0) stream << "\t" << tileIt->source;

         stream << "\t" << tileIt->range.getLo() << "\t" << tileIt->range.getHi();
         stream << "\t" << sign;
         stream << std::endl;
         
         counter++;
         lastEnd = tileIt->offset+tileIt->range.getLength()+1;
      }
   }
   
   stream.close();
}

void outputEdge(std::ostream &stream, AMOS::Bank_t &edge_bank, AMOS::ID_t iid, const char *start, const char *end, const char *direction) {
   AMOS::ContigEdge_t outputEdge;
   edge_bank.fetch(iid, outputEdge);

   stream << outputEdge.getContigs().first << ":" << start << "->" << outputEdge.getContigs().second << ":" << end;
   stream << " [ label =\"" << outputEdge.getIID() << " orientation=" << outputEdge.getAdjacency();

   if (isBadEdge(outputEdge)) {
      stream << " (UNUSED) ";
   }
   stream << "\" dir=\"" << direction << "\" fontsize = 8 solid color=\"black\" ]\n";   
}

void outputEdges(std::ostream &stream, AMOS::Bank_t &edge_bank, AMOS::ID_t currCtg, int32_t edgeTypes[], int32_t debug) {
   if (currCtg != 0) {
      if (edgeTypes[3] != 0) {
         outputEdge(stream, edge_bank, edgeTypes[3], "w", "w", "back");
      }
      if (edgeTypes[2] != 0) {
         outputEdge(stream, edge_bank, edgeTypes[2], "e", "e", "forward");
      } 
      if (edgeTypes[1] != 0) {
         outputEdge(stream, edge_bank, edgeTypes[1], "w", "e", "back");
      }
      if (edgeTypes[0] != 0) {
         outputEdge(stream, edge_bank, edgeTypes[0], "e", "w", "forward");
      } else if (debug >= 3) {
         std::cerr << "WARNING: NO GOOD EDGE BETWEEN CONTIGS" << std::endl;
      }
   }
}

void outputDOT(AMOS::Bank_t &contig_bank, AMOS::Bank_t &edge_bank,
               std::vector<AMOS::Scaffold_t> &scaffs, 
               HASHMAP::hash_map<AMOS::ID_t, contigOrientation, HASHMAP::hash<AMOS::ID_t>, HASHMAP::equal_to<AMOS::ID_t> >& ctg2ort,
               const std::string &outputPrefix,
               int32_t debug)
{
   AMOS::Contig_t ctg;

   std::string outputFile = outputPrefix + ".dot";
   std::ofstream stream;
   stream.open(outputFile.c_str(), std::ios::out);

   stream << "digraph ROOT {" << std::endl;
   stream << "  rankdir = LR" << std::endl;
   stream << "  rotate = 90" << std::endl;
   stream << "  ranksep = 0.01" << std::endl;
   stream << "  nodesep = 0.01" << std::endl;
   stream << "  fontsize = 8" << std::endl;
   stream << "  margin = \".01,.01\"" << std::endl;
   stream << "  ratio = fill" << std::endl;
   stream << "  size = \"11,8.5\"" << std::endl;
   
   HASHMAP::hash_map<AMOS::ID_t, int32_t, HASHMAP::hash<AMOS::ID_t>, HASHMAP::equal_to<AMOS::ID_t> > visited;
   for(std::vector<AMOS::Scaffold_t>::iterator itScf = scaffs.begin(); itScf < scaffs.end(); itScf++) {
      stream << "subgraph cluster_" << itScf->getIID() << "{" << std::endl;
      stream << "\tlabel = \"" << itScf->getIID() << "\"" << std::endl;

      for (std::vector<AMOS::Tile_t>::const_iterator tileIt = itScf->getContigTiling().begin(); tileIt < itScf->getContigTiling().end(); tileIt++) {
         int32_t angle = -90;
         if (tileIt->range.isReverse()) { angle = 90; }

         int32_t outputID = 0;
         stream << "\t" << tileIt->source << " [label=\"" ;
         if (tileIt->source <= contig_bank.getMaxIID()) {
            contig_bank.fetch(tileIt->source, ctg);
            if (ctg.getEID().size() != 0) { 
	       stream << ctg.getEID();
               outputID = 1;
            }
         }
         if (outputID == 0) stream << tileIt->source;

         stream << " position=(" << tileIt->offset << "," << tileIt->offset + tileIt->range.getLength() - 1;
         stream << ") length=" << tileIt->range.getLength() << "\" height=0.2, fontsize=8, shape=\"house\", ";
         stream << "orientation=" << angle << " ]" << std::endl;         
      }
      AMOS::ID_t currCtg = 0;
      AMOS::ID_t secondCtg = 0;
      int32_t edgeTypes[4];
      
      // sort before output
      global_edge_bank = &edge_bank;
      sort(itScf->getContigEdges().begin(), itScf->getContigEdges().end(), EdgeOrderCmp());
      global_edge_bank = NULL;
      
      for (std::vector<AMOS::ID_t>::const_iterator edgeIt = itScf->getContigEdges().begin(); edgeIt < itScf->getContigEdges().end(); edgeIt++) {
         AMOS::ContigEdge_t cte;
         edge_bank.fetch(*edgeIt, cte);

         if (currCtg != cte.getContigs().first || secondCtg != cte.getContigs().second) {
            outputEdges(stream, edge_bank, currCtg, edgeTypes, debug);

            currCtg = cte.getContigs().first;
            secondCtg = cte.getContigs().second;
            memset(edgeTypes, 0, sizeof(int32_t)*4);
         }
         if (visited[cte.getIID()] == 0) {
            // output the edges based on the orient of the contigs
            contigOrientation edgeFirstCtgOrient = ctg2ort[cte.getContigs().first];
            contigOrientation edgeSecondCtgOrient = ctg2ort[cte.getContigs().second];

            contigOrientation first = FWD;
            if (cte.getAdjacency() == AMOS::ContigEdge_t::ANTINORMAL || cte.getAdjacency() == AMOS::ContigEdge_t::OUTIE) {
               first = REV;
            }
            contigOrientation secondCtgOrient = getOrientation(cte.getContigs().first, first, cte);

            if (debug >= 3) {
               std::cerr << "****The firstOrient is " << edgeFirstCtgOrient << " and second is " << edgeSecondCtgOrient << std::endl;
               std::cerr << "The edge type is " << cte.getAdjacency() << " and we initialized first to be " << first << std::endl;
               std::cerr << "****The orient we get are " << first << " and second is " << secondCtgOrient << std::endl;
            }
            if (first == edgeFirstCtgOrient && secondCtgOrient == edgeSecondCtgOrient) {
               edgeTypes[0] = cte.getIID();
               
               if (debug >= 3) { std::cerr << "*** MAIN EDGE BETWEEN " << cte.getContigs().first << " AND " << cte.getContigs().second << " IS " << cte.getAdjacency() <<std::endl; }
            }
            else if (first != edgeFirstCtgOrient && secondCtgOrient == edgeSecondCtgOrient) {
               if (debug >= 3) { std::cerr << "*** D, S EDGE BETWEEN " << cte.getContigs().first << " AND " << cte.getContigs().second << " IS " << cte.getAdjacency() <<std::endl;}
               edgeTypes[1] = cte.getIID();
            }
            else if (first == edgeFirstCtgOrient && secondCtgOrient != edgeSecondCtgOrient) {
               if (debug >= 3) { std::cerr << "*** S, D EDGE BETWEEN " << cte.getContigs().first << " AND " << cte.getContigs().second << " IS " << cte.getAdjacency() <<std::endl; }
               edgeTypes[2] = cte.getIID();
            }
            else {
               if (debug >= 3) { std::cerr << "*** D, D EDGE BETWEEN " << cte.getContigs().first << " AND " << cte.getContigs().second << " IS " << cte.getAdjacency() <<std::endl; }
               edgeTypes[3] = cte.getIID();
            }
         } else {
               std::cerr << "IN SCAFFOLD " << itScf->getIID() << " ENCOUNTERED EDGE " << cte.getIID() << "THAT IVE ALREADY SEEN!!" << std::endl;
               assert(0);
         }
         visited[cte.getIID()] = 1;
      }
      outputEdges(stream, edge_bank, currCtg, edgeTypes, debug);
      stream << "}" << std::endl;
   }
   stream  << "}" << std::endl;
   stream.close();
}

void outputLibrary(const std::string &bank, const std::string &outputPrefix, int32_t debug) {
   AMOS::BankStream_t library_stream (AMOS::Library_t::NCODE);
   if (!library_stream.exists(bank)){
      std::cerr << "No library account found in bank " << bank << std::endl;
      exit(1);
   }
   try {
      library_stream.open(bank, AMOS::B_READ);
   } catch (AMOS::Exception_t & e) {
      std::cerr << "Failed to open library account in bank " << bank << ": " << std::endl << e << std::endl;
      exit(1);
   }
   
   std::string outputFile = outputPrefix + ".library";
   std::ofstream stream;
   stream.open(outputFile.c_str(), std::ios::out);

   AMOS::Library_t lib;
   while (library_stream >> lib) {
      stream << lib.getEID() << " " << lib.getIID() << std::endl;
   }
   
   stream.close();
   library_stream.close();
}

void outputEvidenceXML(const std::string &bank, AMOS::Bank_t &contig_bank, const std::string &outputPrefix, int32_t debug) {
   AMOS::BankStream_t library_stream (AMOS::Library_t::NCODE);
   if (!library_stream.exists(bank)){
      std::cerr << "No library account found in bank " << bank << std::endl;
      exit(1);
   }
   try {
      library_stream.open(bank, AMOS::B_READ);
   } catch (AMOS::Exception_t & e) {
      std::cerr << "Failed to open library account in bank " << bank << ": " << std::endl << e << std::endl;
      exit(1);
   }
   AMOS::BankStream_t frag_stream (AMOS::Fragment_t::NCODE);
   if (!frag_stream.exists(bank)){
      std::cerr << "No library account found in bank " << bank << std::endl;
      exit(1);
   }
   try {
      frag_stream.open(bank, AMOS::B_READ);
   } catch (AMOS::Exception_t & e) {
      std::cerr << "Failed to open library account in bank " << bank << ": " << std::endl << e << std::endl;
      exit(1);
   }

   std::string outputFile = outputPrefix + ".evidence.xml";
   std::ofstream stream;
   stream.open(outputFile.c_str(), std::ios::out);

   time_t t;
   time(&t);
   stream << "<?xml version=\"1.0\" ?>" << std::endl;
   stream << std::endl;
   stream << "<EVIDENCE ID=\"" + outputPrefix << "\"" << std::endl;
   stream << "\t\tDATE=\"" << ctime(&t) << "\"" << std::endl;
   stream << "\t\tPROJECT=\"catXML\"" << std::endl;
   stream << "\t\tPARAMETERS=\"" << outputFile << "\"" << std::endl;
   stream << ">" << std::endl;

   // output libraries
   // build a map for library to fragments
   AMOS::Fragment_t frg;
   HASHMAP::hash_map<AMOS::ID_t, std::vector<AMOS::Fragment_t>, HASHMAP::hash<AMOS::ID_t>, HASHMAP::equal_to<AMOS::ID_t> > lib2frg;
   while (frag_stream >> frg) {
      lib2frg[frg.getLibrary()].push_back(frg);
   }
   
   AMOS::Library_t lib;
   while (library_stream >> lib) {
      double min = lib.getDistribution().mean - (3*lib.getDistribution().sd);
      double max = lib.getDistribution().mean + (3*lib.getDistribution().sd);
      stream << "\t<LIBRARY ID=\"" << lib.getIID() << "\" NAME=\"" << lib.getEID() << "\" MIN=\"" << min << "\" MAX=\"" << max << "\">" << std::endl;
      
      for (std::vector<AMOS::Fragment_t>::const_iterator i = lib2frg[lib.getIID()].begin(); i < lib2frg[lib.getIID()].end(); i++) {
         stream << "\t\t<INSERT ID=\"ins_" << i->getIID() << "\" NAME=\"" << i->getEID() << "\">" << std::endl;
         stream << "\t\t\t<SEQUENCE ID=\"seq_" << i->getMatePair().first << "\" NAME=\"" << i->getEID() << "_L\"></SEQUENCE>" << std::endl;
         stream << "\t\t\t<SEQUENCE ID=\"seq_" << i->getMatePair().second << "\" NAME=\"" << i->getEID() << "_R\"></SEQUENCE>" << std::endl;
         stream << "\t\t</INSERT>" << std::endl;
      }
      
      stream << "\t</LIBRARY>" << std::endl;
   }
   frag_stream.close();
   library_stream.close();
   
   // output contigs
   for (AMOS::IDMap_t::const_iterator ci = contig_bank.getIDMap().begin(); ci; ci++) {
      AMOS::Contig_t ctg;
      contig_bank.fetch(ci->iid, ctg);
         
      stream << "\t<CONTIG ID=\"contig_" << ctg.getIID() << "\" NAME=\"" << ctg.getEID() << "\" LEN=\"" << ctg.getLength() << "\">" << std::endl;
      const std::vector<AMOS::Tile_t> &tiling = ctg.getReadTiling();
      for (std::vector<AMOS::Tile_t>::const_iterator  i = tiling.begin(); i < tiling.end(); i++) {
         stream << "\t\t<SEQUENCE ID=\"seq_" << i->source 
                << "\" ORI=\"" << (i->range.isReverse() ? "EB" : "BE") << "\" ASM_LEND=\"" << i->range.getLo() << "\" ASM_REND=\"" << i->range.getHi() 
                << "\"></SEQUENCE>" << std::endl;
      }
      stream << "\t</CONTIG>" << std::endl;
   }
   
   stream << "</EVIDENCE>" << std::endl;
   stream.close();   
}

void outputOutXML(AMOS::Bank_t &edge_bank, const std::vector<AMOS::Scaffold_t> &scaffs, const std::string &outputPrefix, int32_t debug) {
   std::string outputFile = outputPrefix + ".out.xml";
   std::ofstream stream;
   stream.open(outputFile.c_str(), std::ios::out);

   stream << "<GROUPING>" << std::endl;
   for(std::vector<AMOS::Scaffold_t>::const_iterator itScf = scaffs.begin(); itScf < scaffs.end(); itScf++) {
      stream << "\t<SCAFFOLD ID = \"scaff_" << itScf->getIID() << "\">"  << std::endl;
      
      // output the contigs
      for (std::vector<AMOS::Tile_t>::const_iterator tileIt = itScf->getContigTiling().begin(); tileIt < itScf->getContigTiling().end(); tileIt++) {
         std::string ori = "BE";
         if (tileIt->range.isReverse()) { ori = "EB"; }

         stream << "\t\t<CONTIG ID=\"contig_" << tileIt->source << "\"" << std::endl;
         stream << "\tX=\"" << tileIt->offset << "\"" << std::endl;
         stream << "\tORI=\"" << ori << "\"" << std::endl;
         stream << "></CONTIG>" << std::endl;
      }

      // output the links in the scaffold
      for (std::vector<AMOS::ID_t>::const_iterator edgeIt = itScf->getContigEdges().begin(); edgeIt < itScf->getContigEdges().end(); edgeIt++) {
         AMOS::ContigEdge_t cte;
         edge_bank.fetch(*edgeIt, cte);


         if (!isBadEdge(cte)) {
            std::vector<AMOS::ID_t>::const_iterator linkIt = cte.getContigLinks().begin();
            for (; linkIt < cte.getContigLinks().end(); linkIt++) {
               stream << "\t\t<LINK ID=\"ins_" << *linkIt << "\"" << std::endl;
               stream << "\tVALID=\"VALID\"" << std::endl;
               stream << "\tTAG=\"T\"" << std::endl;
               stream << "></LINK>" << std::endl;
            }
         }
      }
      stream << "\t</SCAFFOLD>" << std::endl;
   }
   
   // output the bad edges
   stream << "\t<UNUSED>" << std::endl;
   for (AMOS::IDMap_t::const_iterator ci = edge_bank.getIDMap().begin(); ci; ci++) {
      AMOS::ContigEdge_t cte;
      edge_bank.fetch(ci->iid, cte);

      std::string status;
      if (!isBadEdge(cte)) {
         continue;
      }
      
      switch (cte.getStatus()) {
         case BAD_LOW:
         case BAD_RPT:
            status = "UNSEEN";
            break;
         case BAD_DST:
            status = "DST";
            break;
         case BAD_ORI:
            status = "ORI";
            break;
         default:
            assert(0);
            break;
      }
      
      for (std::vector<AMOS::ID_t>::const_iterator linkIt = cte.getContigLinks().begin(); linkIt < cte.getContigLinks().end(); linkIt++) {
         stream << "\t\t<LINK ID=\"ins_" << *linkIt << "\"" << std::endl;
         stream << "\t\t\tVALID=\"" << status << "\"" << std::endl;
         stream << "\t\t\tTAG=\"T\"" << std::endl;
         stream << "\t\t></LINK>" << std::endl;
      }
   }

   stream << "\t</UNUSED>" << std::endl;
   stream << "</GROUPING>" << std::endl;
   stream.close();
}

void outputBAMBUS(
         const std::string &bank, 
         AMOS::Bank_t &contig_bank, 
         AMOS::Bank_t &edge_bank, 
         const std::vector<AMOS::Scaffold_t> &scaffs, 
         const std::string &outputPrefix, 
         int32_t debug) 
{
   outputLibrary(bank, outputPrefix, debug);
   outputEvidenceXML(bank, contig_bank, outputPrefix, debug);
   outputOutXML(edge_bank, scaffs, outputPrefix, debug);
}

void outputResults(
         const std::string &bank,
         AMOS::Bank_t &contig_bank,
         AMOS::Bank_t &edge_bank, 
         std::vector<AMOS::Scaffold_t> &scaffs, 
         HASHMAP::hash_map<AMOS::ID_t, contigOrientation, HASHMAP::hash<AMOS::ID_t>, HASHMAP::equal_to<AMOS::ID_t> >& ctg2ort,
         outputType type,
         const std::string &outputPrefix,
         int32_t debug) 
{
   switch (type) {
      case AGP:
         outputAGP(contig_bank, scaffs, outputPrefix, debug);
         break;
      case DOT:
         outputDOT(contig_bank, edge_bank, scaffs, ctg2ort, outputPrefix, debug);
         break;
      case BAMBUS:
         outputBAMBUS(bank, contig_bank, edge_bank, scaffs, outputPrefix, debug);
         break;
   }
 }
 
}
