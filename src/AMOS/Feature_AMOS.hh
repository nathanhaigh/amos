////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Adam M Phillippy
//! \date 04/19/2005
//!
//! \brief Header for Feature_t
//!
////////////////////////////////////////////////////////////////////////////////

#ifndef __Feature_AMOS_HH
#define __Feature_AMOS_HH 1

#include "Universal_AMOS.hh"




namespace AMOS {

typedef char FeatureType_t;

//====================================================== Feature_t =============
//! \brief Notes remarkable features of other objects
//!
//! Allows a range from some other Universal object to be flagged as a feature.
//! One example would be a location on a contig where a repeat is known to
//! exist. Multiple repeat features with the same consensus could then be
//! grouped together with the Group object to represent a repeat class,
//!
//==============================================================================
class Feature_t : public Universal_t
{

private:

  std::pair<ID_t, NCode_t> source_m;      //!< source of the feature
  Range_t range_m;                        //!< feature extent
  FeatureType_t type_m;                   //!< feature type


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

  static const FeatureType_t NULL_FEATURE = 0;
  static const FeatureType_t REPEAT       = 'R';
  static const FeatureType_t UNITIG       = 'U';
  static const FeatureType_t JOIN         = 'J';
  static const FeatureType_t ORF          = 'O';
  static const FeatureType_t FIX          = 'F';

  // forensics features
  static const FeatureType_t COVERAGE     = 'C';
  static const FeatureType_t POLYMORPHISM = 'P';
  static const FeatureType_t BREAKPOINT   = 'B';
  static const FeatureType_t KMER         = 'K';
  static const FeatureType_t MATEPAIR     = 'M';
  static const FeatureType_t MISASSEMBLY  = 'A';


  //--------------------------------------------------------- Feature_t --------
  //! \brief Constructs an empty Feature_t object
  //!
  //! Sets all members to zero or NULL
  //!
  Feature_t ( )
  {
    source_m . first = NULL_ID;
    source_m . second = NULL_NCODE;
    type_m = NULL_FEATURE;
  }


  //--------------------------------------------------------- Feature_t --------
  //! \brief Copy constructor
  //!
  Feature_t (const Feature_t & source)
  {
    *this = source;
  }


  //--------------------------------------------------------- ~Feature_t -------
  //! \brief Destroys a Feature_t object
  //!
  ~Feature_t ( )
  {

  }


  //--------------------------------------------------- clear ------------------
  virtual void clear ( )
  {
    source_m . first = NULL_ID;
    source_m . second = NULL_NCODE;
    range_m . clear( );
    type_m = NULL_FEATURE;
  }


  //--------------------------------------------------- getNCode ---------------
  virtual NCode_t getNCode ( ) const
  {
    return Feature_t::NCODE;
  }


  //--------------------------------------------------- getRange ---------------
  //! \brief Get the feature range
  //!
  //! \return The feature range
  //!
  Range_t getRange ( ) const
  {
    return range_m;
  }


  //--------------------------------------------------- getSource --------------
  //! \brief Get the feature source IID and type
  //!
  //! This method only returns the IID of the object which this feature refers
  //! to. The entire source object can be obtained by fetching the returned IID
  //! from the specified bank type.
  //!
  //! \return The IID of the feature source and the NCode type indentifier
  //!
  std::pair<ID_t, NCode_t> getSource ( ) const
  {
    return source_m;
  }


  //--------------------------------------------------- getType ----------------
  //! \brief Get the feature type
  //!
  //! \return The feature type
  //!
  FeatureType_t getType ( ) const
  {
    return type_m;
  }


  //--------------------------------------------------- setRange ---------------
  //! \brief Set the feature range
  //!
  //! \param range The new feature range
  //! \return void
  //!
  void setRange (Range_t range)
  {
    range_m = range;
  }


  //--------------------------------------------------- setSource --------------
  //! \brief Set the feature source IID and type
  //!
  //! \param source The new IID and NCode type identifier of the feature source
  //! \return void
  //!
  void setSource (std::pair<ID_t, NCode_t> source)
  {
    source_m = source;
  }


  //--------------------------------------------------- setType ----------------
  //! \brief Set the feature type
  //!
  //! \param type The new feature type
  //! \return void
  //!
  void setType (FeatureType_t type)
  {
    type_m = type;
  }


  //--------------------------------------------------- readMessage ------------
  virtual void readMessage (const Message_t & msg);


  //--------------------------------------------------- writeMessage -----------
  virtual void writeMessage (Message_t & msg) const;

};

} // namespace AMOS

#endif // #ifndef __Feature_AMOS_HH
