%module "AMOS"
%{
#include "utility_AMOS.hh"
#include "inttypes_AMOS.hh"
#include "exceptions_AMOS.hh"
#include "datatypes_AMOS.hh"
#include "databanks_AMOS.hh"
#include "messages_AMOS.hh"
#include "universals_AMOS.hh"
%}

%include "std_string.i"

%apply unsigned int { uint32_t }

%include "../inttypes_AMOS.hh"
%include "../Bank_AMOS.hh"
%include "../Message_AMOS.hh"
%include "../Universal_AMOS.hh"
%include "../Sequence_AMOS.hh"
%include "../Read_AMOS.hh"
%include "../Contig_AMOS.hh"


// make clean; make CXXFLAGS=-fPIC
// swig -perl5 -c++ AMOS.i
// g++ -shared -fPIC -I/usr/lib64/perl5/5.8.0/x86_64-linux-thread-multi/CORE AMOS_wrap.cxx libAMOS.a -o AMOS.so 
