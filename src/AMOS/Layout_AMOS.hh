////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Adam M Phillippy
//! \date 07/08/2004
//!
//! \brief Header for Layout_t
//!
////////////////////////////////////////////////////////////////////////////////

#ifndef __Layout_AMOS_HH
#define __Layout_AMOS_HH 1

#include "Universal_AMOS.hh"
#include <vector>





namespace AMOS {

//================================================ Layout_t ====================
//! \brief A layout of sequences
//!
//! A Layout_t is simply a collection of sequences and offsets describing how
//! each sequence is positioned in relation to all other sequences. If the
//! layout is in relation to some reference sequence, a vector of alignment
//! gaps is provided for storing the alignment information.
//!
//==============================================================================
class Layout_t : public Universal_t
{
  
protected:

  std::vector<Tile_t> tiles_m;     //!< sequence tiling


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


  //--------------------------------------------------- Layout_t ---------------
  //! \brief Constructs an empty Layout_t object
  //!
  Layout_t ( )
  {

  }


  //--------------------------------------------------- Layout_t ---------------
  //! \brief Copy constructor
  //!
  Layout_t (const Layout_t & source)
  {
    *this = source;
  }


  //--------------------------------------------------- ~Layout_t --------------
  //! \brief Destroys a Layout_t object
  //!
  ~Layout_t ( )
  {

  }


  //--------------------------------------------------- clear ------------------
  virtual void clear ( )
  {
    Universal_t::clear( );
    tiles_m . clear( );
  }


  //--------------------------------------------------- getNCode ---------------
  virtual NCode_t getNCode ( ) const
  {
    return Layout_t::NCODE;
  }


  //--------------------------------------------------- getSpan ----------------
  //! \brief Get the span of the layout
  //!
  //! Returns the difference between min(offset) and max(offset+len). Since the
  //! layout is not necessarily sorted, this method requires linear time.
  //!
  //! \return The span of the layout
  //!
  Size_t getSpan ( ) const;


  //--------------------------------------------------- getTiling --------------
  //! \brief Get the sequence tiling
  //!
  //! \return The vector of underlying sequences
  //!
  const std::vector<Tile_t> & getTiling ( ) const
  {
    return tiles_m;
  }


  //--------------------------------------------------- getTiling --------------
  //! \brief Get the sequence tiling
  //!
  //! \return The vector of underlying sequences
  //!
  std::vector<Tile_t> & getTiling ( )
  {
    return tiles_m;
  }


  //--------------------------------------------------- readMessage ------------
  virtual void readMessage (const Message_t & msg);


  //--------------------------------------------------- setTiling --------------
  //! \brief Set the sequence tiling
  //!
  //! \param tiles The new vector of underlying sequences
  //! \return void
  //!
  void setTiling (const std::vector<Tile_t> & tiles)
  {
    tiles_m = tiles;
  }


  //--------------------------------------------------- writeMessage -----------
  virtual void writeMessage (Message_t & msg) const;

};

} // namespace AMOS

#endif // #ifndef __Layout_AMOS_HH
