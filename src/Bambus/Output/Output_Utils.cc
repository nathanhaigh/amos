#include <iostream>

#include "Output_Utils.hh"
#include "Utilities_Bundler.hh"

#include "universals_AMOS.hh"
#include "Library_AMOS.hh"
#include "Fragment_AMOS.hh"
#include "Overlap_AMOS.hh"
#include "Contig_AMOS.hh"
#include "ContigLink_AMOS.hh"
#include "ContigEdge_AMOS.hh"
#include "Scaffold_AMOS.hh"
#include "Motif_AMOS.hh"

namespace Bundler
{
AMOS::Bank_t *global_edge_bank = NULL;

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

AMOS::ID_t translateCLKtoFRG(AMOS::Bank_t &link_bank, AMOS::ID_t linkID);
void outputEdge(std::ofstream &stream, AMOS::Bank_t &edge_bank, AMOS::ID_t iid, const char *start, const char *end, const char *direction);
void outputEdges(std::ostream &stream, AMOS::Bank_t &edge_bank, AMOS::ID_t currCtg, int32_t edgeTypes[], int32_t debug);

void outputLibrary(const std::string &outputPrefix, int32_t debug);
void outputEvidenceXML(const std::string &bank, AMOS::Bank_t &contig_bank, const std::string &outputPrefix, int32_t debug);
void outputOutXML(const std::string &bank, AMOS::Bank_t &edge_bank, AMOS::BankStream_t &scf_stream, const std::string &outputPrefix, int32_t debug);

void outputAGP(AMOS::Bank_t &contig_bank, AMOS::BankStream_t &scf_stream, const std::string& outputPrefix, int32_t debug);
void outputDOT(AMOS::Bank_t &contig_bank, AMOS::Bank_t &edge_bank,
               AMOS::Scaffold_t *node,
               AMOS::BankStream_t &scf_stream,
               const std::string &outputPrefix,
               int32_t debug);
void outputDOT(AMOS::Bank_t &contig_bank, AMOS::Bank_t &edge_bank,
               AMOS::Scaffold_t *node,
               AMOS::BankStream_t &scf_stream,
               const std::string &outputPrefix,
               bool allEdges, int32_t debug);
void outputMotifs(AMOS::Bank_t &contig_bank, AMOS::Bank_t &edge_bank,
               AMOS::BankStream_t &motif_stream,
               const std::string &outputPrefix,
               int32_t debug);
void outputBAMBUS(const std::string &bank,
               AMOS::Bank_t &contig_bank,
               AMOS::Bank_t &edge_bank,
               AMOS::BankStream_t &scf_stream,
               const std::string &outputPrefix,
               int32_t debug);

void outputAGP(AMOS::Bank_t &contig_bank, AMOS::BankStream_t &scf_stream, const std::string &outputPrefix, int32_t debug) {
   AMOS::Contig_t ctg;

   std::string outputFile = outputPrefix + ".agp";
   std::ofstream stream;
   stream.open(outputFile.c_str(), std::ios::out);

   stream << "# DESCRIPTION: WGS SCAFFOLDS GENERATED FROM AMOS BANK" << std::endl;

   AMOS::Scaffold_t scf;
   AMOS::ID_t maxIID = contig_bank.getMaxIID();

   while (scf_stream >> scf) {
      int32_t counter = 0;
      int32_t lastEnd = 0;

      for (std::vector<AMOS::Tile_t>::const_iterator tileIt = scf.getContigTiling().begin(); tileIt < scf.getContigTiling().end(); tileIt++) {
         // output the gap
         if (lastEnd != 0 && (tileIt->offset - lastEnd != 0)) {
            stream << scf.getIID();
            stream << "\t" << lastEnd << "\t" << tileIt->offset;
            stream << "\t" << counter;
            stream << "\t" << (tileIt->offset - lastEnd + 1);
            stream << "\tfragment\tyes";
            
            stream << std::endl;
            counter++;
         }

         std::string sign = "+";
         if (tileIt->range.isReverse()) { sign = "-"; }

         stream << scf.getIID();
         stream << "\t" << (tileIt->offset+1) << "\t" << (tileIt->offset+tileIt->range.getLength());
         stream << "\t" << counter << "\tW";

         int32_t outputID = 0;
         if (tileIt->source <= maxIID) {
            contig_bank.fetch(tileIt->source, ctg);
            if (ctg.getEID().size() != 0) {
               stream << "\t" << ctg.getEID();
               outputID = 1;
            }
         }
         if (outputID == 0) stream << "\t" << tileIt->source;

         stream << "\t" << tileIt->range.getLo()+1 << "\t" << tileIt->range.getHi();
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
   stream << " [ label=\"" << outputEdge.getIID() << " orientation=" << outputEdge.getAdjacency() << " weight=" << outputEdge.getContigLinks().size() << " distance=" << outputEdge.getSize() << " stdev=" << outputEdge.getSD();

   if (outputEdge.getStatus() == BAD_TRNS) {
      stream << " (REDUNDANT) ";
   }
   else if (isBadEdge(outputEdge)) {
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
               AMOS::Scaffold_t *node, AMOS::BankStream_t &scf_stream,
               const std::string &outputPrefix,
               int32_t debug)
{
   outputDOT(contig_bank, edge_bank, node, scf_stream, outputPrefix, false, debug);
}

void outputDOT(AMOS::Bank_t &contig_bank, AMOS::Bank_t &edge_bank,
               AMOS::Scaffold_t *node, AMOS::BankStream_t &scf_stream,
               const std::string &outputPrefix,
               bool allEdges,
               int32_t debug)
{
   AMOS::Contig_t ctg;
   AMOS::ID_t maxIID = contig_bank.getMaxIID();

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
   while (scf_stream >> (*node)) {
      AMOS::ID_t scfID = AMOS::NULL_ID;
      if (dynamic_cast<AMOS::Scaffold_t *>(node) != NULL) {
         scfID = node->getIID();
      } else if (dynamic_cast<AMOS::Motif_t *>(node) != NULL) {
         scfID = (dynamic_cast<AMOS::Motif_t *>(node))->getScf();
      } else {
         std::cerr << "Error: Unknown object type passed to output " << node->getNCode() << std::endl;
         continue;
      }

      HASHMAP::hash_map<AMOS::ID_t, int32_t, HASHMAP::hash<AMOS::ID_t>, HASHMAP::equal_to<AMOS::ID_t> > ctginscf;
      HASHMAP::hash_map<AMOS::ID_t, contigOrientation, HASHMAP::hash<AMOS::ID_t>, HASHMAP::equal_to<AMOS::ID_t> > ctgori;
      
      stream << "subgraph cluster_" << scfID << "{" << std::endl;
      stream << "\tlabel = \"" << scfID << "\"" << std::endl;

      for (std::vector<AMOS::Tile_t>::const_iterator tileIt = node->getContigTiling().begin(); tileIt < node->getContigTiling().end(); tileIt++) {
         ctginscf[tileIt->source] = 1;
         ctgori[tileIt->source] = (tileIt->range.isReverse() ? REV : FWD);

         int32_t angle = -90;
         if (tileIt->range.isReverse()) { angle = 90; }

         int32_t outputID = 0;
         stream << "\t" << tileIt->source << " [label=\"" ;
         if (tileIt->source <= maxIID) {
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
      sort(node->getContigEdges().begin(), node->getContigEdges().end(), EdgeOrderCmp());
      global_edge_bank = NULL;
      
      for (std::vector<AMOS::ID_t>::const_iterator edgeIt = node->getContigEdges().begin(); edgeIt < node->getContigEdges().end(); edgeIt++) {
         AMOS::ContigEdge_t cte;
         edge_bank.fetch(*edgeIt, cte);

         if (allEdges == false && (ctginscf[cte.getContigs().first] != 1 || ctginscf[cte.getContigs().second] != 1)) {
            continue;
         }

         if (currCtg != cte.getContigs().first || secondCtg != cte.getContigs().second) {
            outputEdges(stream, edge_bank, currCtg, edgeTypes, debug);

            currCtg = cte.getContigs().first;
            secondCtg = cte.getContigs().second;
            memset(edgeTypes, 0, sizeof(int32_t)*4);
         }
         if (visited[cte.getIID()] == 0) {
            // output the edges based on the orient of the contigs
            contigOrientation edgeFirstCtgOrient = ctgori[cte.getContigs().first];
            contigOrientation edgeSecondCtgOrient = ctgori[cte.getContigs().second];

            contigOrientation first = FWD;
            if (cte.getAdjacency() == AMOS::ContigEdge_t::ANTINORMAL || cte.getAdjacency() == AMOS::ContigEdge_t::OUTIE) {
               first = REV;
            }
            contigOrientation secondCtgOrient = getOrientation(first, cte);

            if (debug >= 3) {
               std::cerr << "****The firstOrient is " << edgeFirstCtgOrient << " and second is " << edgeSecondCtgOrient << std::endl;
               std::cerr << "The edge type is " << cte.getAdjacency() << " and we initialized first to be " << first << std::endl;
               std::cerr << "****The orient we get are " << first << " and second is " << secondCtgOrient << std::endl;
            }
            if (first == edgeFirstCtgOrient && secondCtgOrient == edgeSecondCtgOrient) {
               edgeTypes[0] = (edgeTypes[0] == 0 || !isBadEdge(cte.getIID(), edge_bank) ? cte.getIID() : edgeTypes[0]);
 
               if (debug >= 3) { std::cerr << "*** MAIN EDGE BETWEEN " << cte.getContigs().first << " AND " << cte.getContigs().second << " IS " << cte.getAdjacency() <<std::endl; }
            }
            else if (first != edgeFirstCtgOrient && secondCtgOrient == edgeSecondCtgOrient) {
               if (debug >= 3) { std::cerr << "*** D, S EDGE BETWEEN " << cte.getContigs().first << " AND " << cte.getContigs().second << " IS " << cte.getAdjacency() <<std::endl;}
               edgeTypes[1] = (edgeTypes[1] == 0 || !isBadEdge(cte.getIID(), edge_bank) ? cte.getIID() : edgeTypes[1]);
            }
            else if (first == edgeFirstCtgOrient && secondCtgOrient != edgeSecondCtgOrient) {
               if (debug >= 3) { std::cerr << "*** S, D EDGE BETWEEN " << cte.getContigs().first << " AND " << cte.getContigs().second << " IS " << cte.getAdjacency() <<std::endl; }
               edgeTypes[2] = (edgeTypes[2] == 0 || !isBadEdge(cte.getIID(), edge_bank) ? cte.getIID() : edgeTypes[2]);
            }
            else {
               if (debug >= 3) { std::cerr << "*** D, D EDGE BETWEEN " << cte.getContigs().first << " AND " << cte.getContigs().second << " IS " << cte.getAdjacency() <<std::endl; }
               edgeTypes[3] = (edgeTypes[3] == 0 || !isBadEdge(cte.getIID(), edge_bank) ? cte.getIID() : edgeTypes[3]);
            }
         } else {
               std::cerr << "IN SCAFFOLD " << scfID << " ENCOUNTERED EDGE " << cte.getIID() << "THAT IVE ALREADY SEEN!!" << std::endl;
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

void outputMotifs(AMOS::Bank_t &contig_bank, AMOS::Bank_t &edge_bank,
               AMOS::BankStream_t &motif_stream,
               const std::string &outputPrefix,
               int32_t debug)
{
   if (motif_stream.getSize() == 0) {
      return;
   }

   // output the dot file of motifs
   AMOS::Motif_t *motif = new AMOS::Motif_t();
   outputDOT(contig_bank, edge_bank, motif, motif_stream, outputPrefix + ".noreduce", true, debug);
   motif_stream.seekg(0,AMOS::BankStream_t::BEGIN);

   // output the set of motifs
   std::string outputFile = outputPrefix + ".sets"; 
   std::ofstream stream;
   stream.open(outputFile.c_str(), std::ios::out);

   while (motif_stream >> (*motif)) {
      stream << motif->getEID() << " :";
      for (std::vector<AMOS::Tile_t>::const_iterator tileIt = motif->getContigTiling().begin(); tileIt < motif->getContigTiling().end(); tileIt++) {
         stream << " " << tileIt->source; 
      }
      stream << std::endl;
   }
   stream.close();
   delete motif;
}

AMOS::ID_t translateCLKtoFRG(AMOS::Bank_t &link_bank, AMOS::ID_t linkID) {
   // for fragment-based links, translate their ID to the source
   AMOS::ContigLink_t clk;
   link_bank.fetch(linkID, clk);

   std::pair<AMOS::ID_t, AMOS::NCode_t> source = clk.getSource();
   AMOS::ID_t iid = linkID;

   if (source.second == AMOS::Fragment_t::NCODE) {
     iid = source.first;
   }

   return iid;
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
      std::cerr << "No fragment account found in bank " << bank << std::endl;
      exit(1);
   }
   try {
      frag_stream.open(bank, AMOS::B_READ);
   } catch (AMOS::Exception_t & e) {
      std::cerr << "Failed to open fragment account in bank " << bank << ": " << std::endl << e << std::endl;
      exit(1);
   }
   AMOS::BankStream_t link_stream (AMOS::ContigLink_t::NCODE);
   if (!link_stream.exists(bank)){
      std::cerr << "No contig link account found in bank " << bank << std::endl;
      exit(1);
   }
   try {
      link_stream.open(bank, AMOS::B_READ);
   } catch (AMOS::Exception_t & e) {
      std::cerr << "Failed to open contig link account in bank " << bank << ": " << std::endl << e << std::endl;
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
                << "\" ORI=\"" << (i->range.isReverse() ? "EB" : "BE") << "\" ASM_LEND=\"" << i->offset << "\" ASM_REND=\"" << i->getRightOffset() 
                << "\"></SEQUENCE>" << std::endl;
      }
      stream << "\t</CONTIG>" << std::endl;
   }
  
   // here we output any CTE between contigs that are not the result of link data
   AMOS::ContigLink_t clk;
   while (link_stream >> clk) {
      if (clk.getSource().second != AMOS::Fragment_t::NCODE) {
         stream << "\t<LINK ID=\"link_" << clk.getIID() << "\" SIZE=\"" << clk.getSize() << "\"TYPE=\"" << clk.getType() << "\">" << std::endl;
         std::string oriCtgA = "BE";
         std::string oriCtgB = "BE";
         switch (clk.getAdjacency()) {
            case AMOS::Link_t::NORMAL:
               oriCtgA = oriCtgB = "BE";
               break;
            case AMOS::Link_t::ANTINORMAL:
               oriCtgA = oriCtgB = "EB";
               break;
            case AMOS::Link_t::OUTIE:
               oriCtgA = "EB";
               oriCtgB = "BE";
               break;
            case AMOS::Link_t::INNIE:
               oriCtgA = "BE";
               oriCtgB = "EB";
               break;
         };
         stream << "\t\t<CONTIG ID=\"" << clk.getContigs().first << "\"ORI=\"" << oriCtgA << "\">" << std::endl;
         stream << "\t\t<CONTIG ID=\"" << clk.getContigs().second << "\"ORI=\"" << oriCtgB << "\">" << std::endl;
      }
   }

   stream << "</EVIDENCE>" << std::endl;
   stream.close();   
   link_stream.close();
}

void outputOutXML(const std::string &bank, AMOS::Bank_t &edge_bank, AMOS::BankStream_t &scf_stream, const std::string &outputPrefix, int32_t debug) {
   AMOS::Bank_t link_bank (AMOS::ContigLink_t::NCODE);
   if (!link_bank.exists(bank)){
      std::cerr << "No contig link account found in bank " << bank << std::endl;
      exit(1);
   }
   try {
      link_bank.open(bank, AMOS::B_READ);
   } catch (AMOS::Exception_t & e) {
      std::cerr << "Failed to open contig link account in bank " << bank << ": " << std::endl << e << std::endl;
      exit(1);
   }

   HASHMAP::hash_map<AMOS::ID_t, int, HASHMAP::hash<AMOS::ID_t>, HASHMAP::equal_to<AMOS::ID_t> > outputLinks; 
   std::string outputFile = outputPrefix + ".out.xml";
   std::ofstream stream;
   stream.open(outputFile.c_str(), std::ios::out);

   stream << "<GROUPING>" << std::endl;
   AMOS::Scaffold_t scf;
   while (scf_stream >> scf) {
      stream << "\t<SCAFFOLD ID = \"scaff_" << scf.getIID() << "\">"  << std::endl;
      
      // output the contigs
      for (std::vector<AMOS::Tile_t>::const_iterator tileIt = scf.getContigTiling().begin(); tileIt < scf.getContigTiling().end(); tileIt++) {
         std::string ori = "BE";
         if (tileIt->range.isReverse()) { ori = "EB"; }

         stream << "\t\t<CONTIG ID=\"contig_" << tileIt->source << "\"" << std::endl;
         stream << "\tX=\"" << tileIt->offset << "\"" << std::endl;
         stream << "\tORI=\"" << ori << "\"" << std::endl;
         stream << "></CONTIG>" << std::endl;
      }

      // output the links in the scaffold
      for (std::vector<AMOS::ID_t>::const_iterator edgeIt = scf.getContigEdges().begin(); edgeIt < scf.getContigEdges().end(); edgeIt++) {
         AMOS::ContigEdge_t cte;
         edge_bank.fetch(*edgeIt, cte);


         if (!isBadEdge(cte)) {
            std::vector<AMOS::ID_t>::const_iterator linkIt = cte.getContigLinks().begin();
            for (; linkIt < cte.getContigLinks().end(); linkIt++) {
               stream << "\t\t<LINK ID=\"ins_" << translateCLKtoFRG(link_bank, *linkIt) << "\"" << std::endl;
               stream << "\tVALID=\"VALID\"" << std::endl;
               stream << "\tTAG=\"T\"" << std::endl;
               stream << "></LINK>" << std::endl;

               // store list of links weve output
               outputLinks[(*linkIt)] = 1;
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
         case BAD_THRESH:
         case BAD_RPT:
            status = "UNSEEN";
            break;
         case BAD_SKIP:
         case BAD_SCF:
            status = "UNUSED";
            break;
         case BAD_DST:
            status = "LEN";
            break;
         case BAD_ORI:
            status = "ORI";
            break;
         case BAD_TRNS:
            status = "TRANSITIVE";
            break;
         default:
            assert(0);
            break;
      }
      
      for (std::vector<AMOS::ID_t>::const_iterator linkIt = cte.getContigLinks().begin(); linkIt < cte.getContigLinks().end(); linkIt++) {
         stream << "\t\t<LINK ID=\"ins_" << translateCLKtoFRG(link_bank, *linkIt) << "\"" << std::endl;
         stream << "\t\t\tVALID=\"" << status << "\"" << std::endl;
         stream << "\t\t\tTAG=\"T\"" << std::endl;
         stream << "\t\t></LINK>" << std::endl;

         outputLinks[(*linkIt)] = 1;
      }
   }

   // finally make sure all links have been output
   for (AMOS::IDMap_t::const_iterator ci = link_bank.getIDMap().begin(); ci; ci++) {

      if (outputLinks[ci->iid] != 1) {
         stream << "\t\t<LINK ID=\"ins_" << translateCLKtoFRG(link_bank, ci->iid) << "\"" << std::endl;
         stream << "\t\t\tVALID=\"" << "UNUSED" << "\"" << std::endl;
         stream << "\t\t\tTAG=\"T\"" << std::endl;
         stream << "\t\t></LINK>" << std::endl;
      }
   }

   stream << "\t</UNUSED>" << std::endl;
   stream << "</GROUPING>" << std::endl;
   stream.close();
 
   link_bank.close();
}

void outputBAMBUS(
         const std::string &bank, 
         AMOS::Bank_t &contig_bank, 
         AMOS::Bank_t &edge_bank, 
         AMOS::BankStream_t &scf_stream,
         const std::string &outputPrefix, 
         int32_t debug) 
{
   outputLibrary(bank, outputPrefix, debug);
   outputEvidenceXML(bank, contig_bank, outputPrefix, debug);
   outputOutXML(bank, edge_bank, scf_stream, outputPrefix, debug);
}

void outputResults(
         const std::string &bank,
         AMOS::Bank_t &contig_bank,
         AMOS::Bank_t &edge_bank, 
         AMOS::BankStream_t &motif_stream,
         AMOS::BankStream_t &scf_stream,
         outputType type,
         const std::string &outputPrefix,
         int32_t debug) 
{
   AMOS::Scaffold_t * scf = new AMOS::Scaffold_t();

   if (scf_stream.getSize() == 0) {
      return;
   }

   switch (type) {
      case AGP:
         outputAGP(contig_bank, scf_stream, outputPrefix, debug);
         break;
      case DOT:
         outputDOT(contig_bank, edge_bank, scf, scf_stream, outputPrefix, debug);
         break;
      case MOTIFS:
         outputMotifs(contig_bank, edge_bank, motif_stream, outputPrefix, debug);
         break;
      case BAMBUS:
         outputBAMBUS(bank, contig_bank, edge_bank, scf_stream, outputPrefix, debug);
         break;
   }
   delete scf;
 }
}
