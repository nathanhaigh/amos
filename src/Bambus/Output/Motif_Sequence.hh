#ifndef MOTIF_SEQUENCE_HH_
#define MOTIF_SEQUENCE_HH_

#include <string>

#include "Bank_AMOS.hh"
#include "BankStream_AMOS.hh"
#include "Motif_AMOS.hh"

namespace Output 
{
   std::string outputMotif(std::string &name, AMOS::Motif_t &scf, AMOS::Bank_t &motif_bank, AMOS::Bank_t &contig_bank, AMOS::Bank_t &edge_bank);
   std::string outputMotif(std::string &name, AMOS::Motif_t &scf, AMOS::Bank_t &motif_bank, AMOS::Bank_t &contig_bank, AMOS::Bank_t &edge_bank, bool print);

   std::string getTileSequence(AMOS::Bank_t &contig_bank, AMOS::Bank_t &motif_bank, AMOS::Bank_t &edge_bank, AMOS::ID_t max, AMOS::ID_t source, AMOS::Range_t range); 
   std::string getTileSequence(AMOS::Bank_t &contig_bank, AMOS::Bank_t &motif_bank, AMOS::Bank_t &edge_bank, AMOS::ID_t max, AMOS::ID_t source, AMOS::Range_t range, std::string &eid);
}
#endif
