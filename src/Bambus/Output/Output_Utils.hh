#ifndef OUTPUT_UTILS_HH_
#define OUTPUT_UTILS_HH_

#include <string>

#include "Bank_AMOS.hh"
#include "BankStream_AMOS.hh"

namespace Bundler
{
   enum outputType{AGP, DOT, MOTIFS, BAMBUS};

   void outputResults(
            const std::string &bank,
            AMOS::Bank_t &contig_bank,
            AMOS::Bank_t &edge_bank,
            AMOS::BankStream_t &motif_stream,
            AMOS::BankStream_t &scf_stream,
            outputType type,
            const std::string &outputPrefix,
            int32_t debug);
}
#endif
