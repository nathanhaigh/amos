%module "AMOS"
%{
#include "foundation_AMOS.hh"
using namespace AMOS;
%}

%include "std_string.i"

%apply unsigned int { uint32_t }

%include "inttypes_AMOS.hh"
%include "Bank_AMOS.hh"
%include "Message_AMOS.hh"
%include "Universal_AMOS.hh"
%include "Sequence_AMOS.hh"
%include "Read_AMOS.hh"
%include "Contig_AMOS.hh"


