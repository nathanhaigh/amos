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


36,37c36,37
< const char     MAX_QUALITY = '0' + 63;   //!< max quality score definition
< const char     NULL_CHAR   = '\0';       //!< null char
---
> const char     MAX_QUALITY = (63);   //!< max quality score definition
> const char     NULL_CHAR2   = 0;       //!< null char
39c39
< const std::string  NULL_STRING = &NULL_CHAR; //!< null string
---
> const std::string  NULL_STRING = &NULL_CHAR2; //!< null string
