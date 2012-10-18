%module "AMOS"

%{
#include "utility_AMOS.hh"
#include "inttypes_AMOS.hh"
#include "exceptions_AMOS.hh"
#include "datatypes_AMOS.hh"
#include "databanks_AMOS.hh"
#include "messages_AMOS.hh"
#include "universals_AMOS.hh"
#include "BankStream_AMOS.hh"
%}


%include "std_string.i"
//%include "embed.i"

%apply unsigned int { uint32_t }


%include "inttypes_AMOS.hh"
%include "Message_AMOS.hh"
%include "datatypes_AMOS.hh"
%include "databanks_AMOS.hh"
%include "IDMap_AMOS.hh"
%include "Bank_AMOS.hh"
%include "BankStream_AMOS.hh"
%include "Universal_AMOS.hh"
%include "Sequence_AMOS.hh"
%include "Read_AMOS.hh"
%include "Link_AMOS.hh"
%include "Layout_AMOS.hh"
%include "Contig_AMOS.hh"

// Redefine nested struct
  struct HashTriple_t
  {
    uint8_t  c;               //!< reference counter
    uint32_t   iid;               //!< internal AMOS ID
    uint32_t   bid;               //!< bank index
    std::string eid;          //!< external ID

    //------------------------------------------------- HashTriple_t -----------
    //! \brief Constructs a HashTriple
    //!
    HashTriple_t (uint32_t iid_p, const std::string & eid_p, uint32_t bid_p)
      : c (0), iid (iid_p), eid (eid_p), bid (bid_p)
    { }

    //------------------------------------------------- ~HashTriple_t ----------
    //! \brief Destroys a HashTriple
    //!
    ~HashTriple_t ( )
    { }

    //------------------------------------------------- operator= --------------
    //! \brief Deep copy of the HashTriple
    //!
    HashTriple_t & operator= (const HashTriple_t & s);

  };

%nestedworkaround AMOS::IDMap_t::HashTriple_t;

%{
typedef AMOS::IDMap_t::HashTriple_t HashTriple_t;
%}


// make clean; make CXXFLAGS=-fPIC
// /fs/szdevel/treangen/bin/swig -Wall -python -c++ AMOS.i
//  g++ -shared  -fPIC -c AMOS_wrap.cxx -I/fs/sz-user-supported/Linux-x86_64/include/python2.6
//  g++ -shared -fPIC  AMOS_wrap.o -L. -lAMOS -o _AMOS.so

//ignore BankStream_AMOS::triples_m
//ignore Bank_AMOS::getIDMap()
//ignore BankStream_AMOS::BankStream_t::clean()
//ignore BankStream_AMOS::BankStream_t::concat()
//ignore BankStream_AMOS::BankStream_t::open(const std::string & dir, BankMode_t mode, Size_t version, bool inPlace)
