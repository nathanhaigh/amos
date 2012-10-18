////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Sergey Koren 
//! \date 04/11/11
//!
//! \brief Header for Motif_t
//!
////////////////////////////////////////////////////////////////////////////////

#ifndef __Motif_AMOS_HH
#define __Motif_AMOS_HH 1

#include "Universal_AMOS.hh"
#include "Scaffold_AMOS.hh"
#include <vector>




namespace AMOS {

//================================================ Motif_t ==================
//! \brief A ordered, oriented and positioned list of contigs
//!
//! A list of contigs that are ordered, oriented and positioned in
//! relation to one another.
//!
//==============================================================================
class Motif_t : public Scaffold_t
{

private:
   ID_t scf_m;

protected:

  //--------------------------------------------------- readRecord -------------
  virtual void readRecord (std::istream & fix, std::istream & var);


  //--------------------------------------------------- readRecordFix ----------
  virtual void readRecordFix (std::istream & fix);


  //--------------------------------------------------- writeRecord ------------
  virtual void writeRecord (std::ostream & fix, std::ostream & var) const;


public:

  static const NCode_t NCODE;
  //!< The NCode type identifier for this object


  //--------------------------------------------------- Motif_t -------------
  //! \brief Constructs an empty Motif_t object
  //!
  Motif_t ( )
  {
     scf_m = NULL_ID;
  }


  //--------------------------------------------------- Motif_t -------------
  //! \brief Copy constructor
  //!
  Motif_t (const Motif_t & source)
  {
    *this = source;
  }


  //--------------------------------------------------- ~Motif_t ------------
  //! \brief Destroys a Motif_t object
  //!
  ~Motif_t ( )
  {

  }

  void setScf(ID_t &id) {
     scf_m = id;
  }

  ID_t getScf() {
     return scf_m;
  }

  //--------------------------------------------------- clear ------------------
  virtual void clear ( )
  {
    Scaffold_t::clear( );
    scf_m = NULL_ID;
  }


  //--------------------------------------------------- getNCode ---------------
  virtual NCode_t getNCode ( ) const
  {
    return Motif_t::NCODE;
  }


  //--------------------------------------------------- readMessage ------------
  virtual void readMessage (const Message_t & msg);


  //--------------------------------------------------- writeMessage -----------
  virtual void writeMessage (Message_t & msg) const;

};

} // namespace AMOS

#endif // #ifndef __Motif_AMOS_HH
