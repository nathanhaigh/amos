////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Adam M Phillippy
//! \date 06/18/2003
//!
//! \brief Header for Matepair_t
//!
////////////////////////////////////////////////////////////////////////////////

#ifndef __Matepair_AMOS_HH
#define __Matepair_AMOS_HH 1

#include "Universal_AMOS.hh"




namespace AMOS {

typedef char MateType_t;

//================================================ Matepair_t ==================
//! \brief Matepair information for two reads
//!
//! Pairs two reads based on their positional relationship within the same
//! fragment. In the case of end sequencing, the two reads come from the ends
//! of the insert fragment. In the case of transposon sequencing, the two reads
//! come from the N and S primers.
//!
//==============================================================================
class Matepair_t : public Universal_t
{

public:

  static const MateType_t NULL_MATE;
  static const MateType_t OTHER;
  static const MateType_t END;
  static const MateType_t TRANSPOSON;


private:

  std::pair<ID_t, ID_t> reads_m;        //!< the pair of read IIDs
  MateType_t type_m;                    //!< type of link


protected:

  //--------------------------------------------------- readRecord -------------
  virtual void readRecord (std::istream & fix, std::istream & var);


  //--------------------------------------------------- writeRecord ------------
  virtual void writeRecord (std::ostream & fix, std::ostream & var) const;

  
public:

  static const NCode_t NCODE;
  //!< The NCode type identifier for this object


  //--------------------------------------------------- Matepair_t -------------
  //! \brief Constructs an empty Matepair_t object
  //!
  //! Sets all members to zero or NULL
  //!
  Matepair_t ( )
  {
    reads_m . first = reads_m . second = NULL_ID;
    type_m = NULL_MATE;
  }


  //--------------------------------------------------- Matepair_t -------------
  //! \brief Copy constructor
  //!
  Matepair_t (const Matepair_t & source)
  {
    *this = source;
  }


  //--------------------------------------------------- ~Matepair_t ------------
  //! \brief Destroys a Matepair_t object
  //!
  ~Matepair_t ( )
  {

  }


  //--------------------------------------------------- clear ------------------
  virtual void clear ( )
  {
    Universal_t::clear( );
    reads_m . first = reads_m . second = NULL_ID;
    type_m = NULL_MATE;
  }


  //--------------------------------------------------- flip -------------------
  //! \brief Flip the order of the mates
  //!
  //! Reverses the order of the read IIDs.
  //!
  //! \return void
  //!
  void flip ( )
  {
    reads_m = std::make_pair (reads_m . second, reads_m . first);
  }


  //--------------------------------------------------- getReads ---------------
  //! \brief Get the pair of read IIDs joined by this matepair
  //!
  //! The first read in the matepair is the Forward or North read and the second
  //! read is the Reverse or South read.
  //!
  //! \return The first and second read IIDs joined by this matepair
  //!
  std::pair<ID_t, ID_t> getReads ( ) const
  {
    return reads_m;
  }


  //--------------------------------------------------- getNCode ---------------
  virtual NCode_t getNCode ( ) const
  {
    return Matepair_t::NCODE;
  }


  //--------------------------------------------------- getType ----------------
  //! \brief Get the link type
  //!
  //! Describes the type of link this object represents.
  //!
  //! \return The type of link
  //!
  MateType_t getType ( ) const
  {
    return type_m;
  }


  //--------------------------------------------------- readMessage ------------
  virtual void readMessage (const Message_t & msg);


  //--------------------------------------------------- setReads ---------------
  //! \brief Set the read IID pair for this matepair
  //!
  //! Only returns the IIDs of the reads. It is up to the user to keep track
  //! of where the actual read objects are stored. The first read in the
  //! matepair is the Forward or North read and the second read is the Reverse
  //! or South read.
  //!
  //! \param reads The new pair of read IIDs
  //! \return void
  //!
  void setReads (std::pair<ID_t, ID_t> reads)
  {
    reads_m = reads;
  }


  //--------------------------------------------------- setType ----------------
  //! \brief Set the linking type
  //!
  //! \param type The new link type
  //! \pre type is one of [XMOPAS] or NULL
  //! \throws ArgumentException_t
  //! \return void
  //!
  void setType (MateType_t type);


  //--------------------------------------------------- writeMessage -----------
  virtual void writeMessage (Message_t & msg) const;

};

} // namespace AMOS

#endif // #ifndef __Matepair_AMOS_HH
