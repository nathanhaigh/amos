////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Adam M Phillippy
//! \date 06/18/2003
//!
//! \brief Header for Read_t
//!
////////////////////////////////////////////////////////////////////////////////

#ifndef __Read_AMOS_HH
#define __Read_AMOS_HH 1

#include "Sequence_AMOS.hh"
#include <vector>




namespace AMOS {

typedef char ReadType_t;

//================================================ Read_t ======================
//! \brief A DNA sequencing read
//!
//! Derived from a Sequence_t type, this class is for the efficient storage of
//! DNA sequencing reads. Retains all the functionality of a Sequence with
//! added fields for clear ranges, mate pairs, etc.
//!
//==============================================================================
class Read_t : public Sequence_t
{

private:

  Range_t clear_m;                 //!< generic clear range
  ID_t frag_m;                     //!< the IID of the parent fragment
  Range_t qclear_m;                //!< the quality score clear range
  ReadType_t type_m;               //!< the read type
  Range_t vclear_m;                //!< the vector clear range
  std::vector<int16_t> bcp_m;      //!< base call positions
  Pos_t pos_m;                     //!< position on parent fragment


protected:

  //--------------------------------------------------- readRecord -------------
  virtual void readRecord (std::istream & fix, std::istream & var);

  virtual void readRecordFix (std::istream & fix);


  //--------------------------------------------------- writeRecord ------------
  virtual void writeRecord (std::ostream & fix, std::ostream & var) const;


public:

  static const NCode_t NCODE;
  //!< The NCode type identifier for this object

  static const ReadType_t NULL_READ  = 0;
  static const ReadType_t OTHER      = 'X';
  static const ReadType_t END        = 'E';
  static const ReadType_t CONTIG     = 'C';
  static const ReadType_t BAC        = 'B';
  static const ReadType_t WALK       = 'W';
  static const ReadType_t TRANSPOSON = 'T';


  //--------------------------------------------------- Read_t -----------------
  //! \brief Constructs an empty Read_t object
  //!
  //! Sets all members to NULL
  //!
  Read_t ( )
  {
    frag_m = NULL_ID;
    type_m = NULL_READ;
    pos_m = 0;
  }


  //--------------------------------------------------- Read_t -----------------
  //! \brief Copy constructor
  //!
  Read_t (const Read_t & source)
  {
    *this = source;
  }


  //--------------------------------------------------- ~Read_t ----------------
  //! \brief Destroys a Read_t object
  //!
  ~Read_t ( )
  {

  }


  //--------------------------------------------------- clear ------------------
  virtual void clear ( );


  //--------------------------------------------------- getBasePositions -------
  //! \brief Get the base call positions
  //!
  //! If the vector is empty, then no base call positions are provided.
  //! Otherwise, the vector should have a size equal to the sequence length
  //! representing the base call positions as specified by the base-caller.
  //!
  //! \return The vector of base call positions
  //!
  const std::vector<int16_t> & getBasePositions ( ) const
  {
    return bcp_m;
  }


  //--------------------------------------------------- getBasePositions -------
  //! \brief Get the base call positions
  //!
  //! If the vector is empty, then no base call positions are provided.
  //! Otherwise, the vector should have a size equal to the sequence length
  //! representing the base call positions as specified by the base-caller.
  //!
  //! \return The vector of base call positions
  //!
  std::vector<int16_t> & getBasePositions ( )
  {
    return bcp_m;
  }


  //--------------------------------------------------- getClearRange ----------
  //! \brief Get the generic clear range
  //!
  //! \return The generic clear range
  //!
  Range_t getClearRange ( )
  {
    return clear_m;
  }


  //--------------------------------------------------- getFragment ------------
  //! \brief Get the parent fragment IID
  //!
  //! \return The parent fragment IID
  //!
  ID_t getFragment ( ) const
  {
    return frag_m;
  }


  //--------------------------------------------------- getFragmentPosition ----
  //! \brief Get the approximate position on the parent fragment
  //!
  //! Get this read's 1-based position on the parent fragment, positive if
  //! counting from left and oriented forward, negative if counting from right
  //! and reverse oriented, 0 if unknown.
  //!
  //! \return The position of this read on the parent fragment
  //!
  Pos_t getFragmentPosition ( )
  {
    return pos_m;
  }


  //--------------------------------------------------- getNCode ---------------
  virtual NCode_t getNCode ( ) const
  {
    return Read_t::NCODE;
  }


  //--------------------------------------------------- getQualityClearRange ---
  //! \brief Get the quality score clear range
  //!
  //! \return The quality score clear range
  //!
  Range_t getQualityClearRange ( )
  {
    return qclear_m;
  }


  //--------------------------------------------------- getType ----------------
  //! \brief Get the type of read
  //!
  //! \return The type of read
  //!
  ReadType_t getType ( ) const
  {
    return type_m;
  }


  //--------------------------------------------------- getVectorClearRange ----
  //! \brief Get the vector sequence clear range
  //!
  //! \return The vector sequence clear range
  //!
  Range_t getVectorClearRange ( )
  {
    return vclear_m;
  }


  //--------------------------------------------------- readMessage ------------
  virtual void readMessage (const Message_t & msg);


  //--------------------------------------------------- setBasePositions -------
  //! \brief Set the base call positions
  //!
  //! \param bcp The new vector of base call positions
  //! \return void
  //!
  void setBasePositions (const std::vector<int16_t> & bcp)
  {
    bcp_m = bcp;
  }


  //--------------------------------------------------- setClearRange ----------
  //! \brief Set the generic clear range
  //!
  //! \param clear The new generic clear range
  //! \return void
  //!
  void setClearRange (Range_t clear)
  {
    clear_m = clear;
  }


  //--------------------------------------------------- setFragment ------------
  //! \brief Set the parent fragment IID
  //!
  //! \param frag The new parent fragment IID
  //! \return void
  //!
  void setFragment (ID_t frag)
  {
    frag_m = frag;
  }


  //--------------------------------------------------- setFragmentPosition ----
  //! \brief Set the approximate position on the parent fragment
  //!
  //! Set this read's 1-based position on the parent fragment, positive if
  //! counting from left and oriented forward, negative if counting from right
  //! and reverse oriented, 0 if unknown.
  //!
  //! \param pos The approximate position of this read on the parent fragment
  //! \return void
  //!
  void setFragmentPosition (Pos_t pos)
  {
    pos_m = pos;
  }


  //--------------------------------------------------- setQualityClearRange ---
  //! \brief Set the quality score clear range
  //!
  //! \param qclear The new quality score clear range
  //! \return void
  //!
  void setQualityClearRange (Range_t qclear)
  {
    qclear_m = qclear;
  }


  //--------------------------------------------------- setType ----------------
  //! \brief Set the type of read
  //!
  //! \param type The new type of read
  //! \pre type is one of [XECBW]
  //! \throws ArgumentException_t
  //! \return void
  //!
  void setType (ReadType_t type);


  //--------------------------------------------------- setVectorClearRange ----
  //! \brief Set the vector sequence clear range
  //!
  //! \param vclear The new vector sequence clear range
  //! \return void
  //!
  void setVectorClearRange (Range_t vclear)
  {
    vclear_m = vclear;
  }


  //--------------------------------------------------- writeMessage -----------
  virtual void writeMessage (Message_t & msg) const;

};


//================================================ Helper Functions ============
//----------------------------------------------------- operator<< -------------
//! \brief Dump read object to ostream
//!
std::ostream & operator<< (std::ostream & o, Read_t & read);

} // namespace AMOS

#endif // #ifndef __Read_AMOS_HH
