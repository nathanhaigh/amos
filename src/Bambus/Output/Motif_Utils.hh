#ifndef MOTIF_UTILS_HH_
#define MOTIF_UTILS_HH_

#include <string>

#include "Bank_AMOS.hh"
#include "BankStream_AMOS.hh"
#include "Motif_AMOS.hh"

namespace Bundler
{
   std::string outputMotif(std::string &name, AMOS::Motif_t &scf, AMOS::Bank_t &motif_bank, AMOS::Bank_t &contig_bank, AMOS::Bank_t &edge_bank);
   std::string outputMotif(std::string &name, AMOS::Motif_t &scf, AMOS::Bank_t &motif_bank, AMOS::Bank_t &contig_bank, AMOS::Bank_t &edge_bank, bool print);
}
#endif
