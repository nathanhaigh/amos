////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Adam M Phillippy
//! \date 03/26/2003
//!
//! \brief Class declarations for the handling of delta alignment files
//!
//! \see delta.cc
////////////////////////////////////////////////////////////////////////////////

#ifndef __DELTA_HH
#define __DELTA_HH

#include <cassert>
#include <string>
#include <vector>
#include <fstream>
#include <cstdlib>
#include <iostream>




const std::string NUCMER_STRING = "NUCMER";
//!< String identifier for data produced by NUCmer

const std::string PROMER_STRING = "PROMER";
//!< String identifier for data produced by PROmer





//===================================================== DeltaAlignment_t =======
//! \brief A single delta encoded alignment region
//!
//==============================================================================
struct DeltaAlignment_t {

  unsigned long int sR;
  //!< Start coordinate in the reference

  unsigned long int eR;
  //!< End coordinate in the reference

  unsigned long int sQ;
  //!< Start coordinate in the reference

  unsigned long int eQ;
  //!< End coordinate in the reference

  float idy;
  //!< Percent identity [0 - 100]

  float sim;
  //!< Percent similarity [0 - 100]

  float stp;
  //!< Percent stop codon [0 - 100]

  std::vector<long int> deltas;
  //!< Delta encoded alignment informaiton

  DeltaAlignment_t ( )
  {
    clear ( );
  }

  void clear ( )
  {
    sR = eR = sQ = eQ = 0;
    idy = sim = stp = 0;
    deltas.clear ( );
  }
};




//===================================================== DeltaRecord_t ==========
//! \brief A delta record representing the alignments between two sequences
//!
//! \see DeltaAlignment_t
//==============================================================================
struct DeltaRecord_t {

  std::string idR;
  //!< Reference contig ID

  std::string idQ;
  //!< Query contig ID

  unsigned long int lenR;
  //!> Length of the reference contig

  unsigned long int lenQ;
  //!> Length of the query contig

  std::vector<DeltaAlignment_t> aligns;
  //!> Alignments between the two contigs

  DeltaRecord_t ( )
  {
    clear ( );
  }

  void clear ( )
  {
    idR.erase ( );
    idQ.erase ( );
    lenR = lenQ = 0;
    aligns.clear ( );
  }
};





//===================================================== DeltaReader_t ==========
//! \brief Delta encoded file reader class
//!
//! Handles the input of delta encoded alignment information for various MUMmer
//! utilities. Very basic functionality, can be expanded as necessary...
//!
//! \see DeltaRecord_t
//==============================================================================
class DeltaReader_t {

private:

  std::string delta_path_m;
  //!< The name of the delta input file

  std::ifstream delta_stream_m;
  //!< The delta file input stream

  std::string data_type_m;
  //!< The type of data this delta file represents

  std::string reference_path_m;
  //!< The name of the reference file

  std::string query_path_m;
  //!< The name of the query file
  
  DeltaRecord_t record_m;
  //!< The current delta information record

  bool is_record_m;
  //!< There is a valid record in record_m

  bool is_open_m;
  //!< Delta stream is open


  //--------------------------------------------------- readNextAlignment ------
  //! \brief Reads in a delta encoded alignment
  //!
  //! \param align read info into this structure
  //! \param read_deltas read delta information yes/no
  //! \pre delta_stream is positioned at the beginning of an alignment header
  //! \return void
  //!
  void readNextAlignment (DeltaAlignment_t & align, const bool read_deltas);


  //--------------------------------------------------- readNextRecord ---------
  //! \brief Reads in the next delta record from the delta file
  //!
  //! \param read_deltas read delta information yes/no
  //! \pre delta file must be open
  //! \return true on success, false on EOF
  //!
  bool readNextRecord (const bool read_deltas);


  //--------------------------------------------------- checkStream ------------
  //! \brief Check stream status and abort program if an error has occured
  //!
  //! \return void
  //!
  void checkStream ( )
  {
    if ( !delta_stream_m.good ( ) )
      {
	std::cerr << "ERROR: Could not parse delta file, "
		  << delta_path_m << std::endl;
	exit (-1);
      }
  }


public:
  //--------------------------------------------------- DeltaReader_t ----------
  //! \brief Default constructor
  //!
  //! \return void
  //!
  DeltaReader_t ( )
  {
    is_record_m = false;
    is_open_m = false;
  }


  //--------------------------------------------------- ~DeltaReader_t ---------
  //! \brief Default destructor
  //!
  //! \return void
  //!
  ~DeltaReader_t ( )
  {
    close ( );
  }


  //--------------------------------------------------- open -------------------
  //! \brief Opens a delta file by path
  //!
  //! \param delta file to open
  //! \return void
  //!
  void open (const std::string & delta_path);


  //--------------------------------------------------- close ------------------
  //! \brief Closes any open delta file and resets class members
  //!
  //! \return void
  //!
  void close ( )
  {
    delta_path_m.erase ( );
    delta_stream_m.close ( );
    data_type_m.erase ( );
    reference_path_m.erase ( );
    query_path_m.erase ( );
    record_m.clear ( );
    is_record_m = false;
    is_open_m = false;
  }


  //--------------------------------------------------- readNext --------------
  //! \brief Reads in the next delta record from the delta file
  //!
  //! \param read_deltas read delta information yes/no
  //! \pre delta file must be open
  //! \return true on success, false on EOF
  //!
  inline bool readNext ( )
  {
    return readNextRecord (true);
  }



  //--------------------------------------------------- readNextHeadersOnly ----
  //! \brief Reads in the next delta record from the delta file
  //!
  //! Only reads the alignment header information, does not read in the delta
  //! information and leaves each alignment structure's delta vector empty.
  //!
  //! \pre delta file must be open
  //! \return true on success, false on EOF
  //!
  inline bool readNextHeadersOnly ( )
  {
    return readNextRecord (false);
  }


  //--------------------------------------------------- getRecord --------------
  //! \brief Returns a reference to the current delta record
  //!
  //! \pre readNext( ) was successfully called in advance
  //! \return true on success, false on failure or end of file
  //!
  const DeltaRecord_t & getRecord ( ) const
  {
    assert (is_record_m);
    return record_m;
  }


  //--------------------------------------------------- getDeltaPath -----------
  //! \brief Get the path of the current delta file
  //!
  //! \pre delta file is open
  //! \return the path of the delta file
  //!
  const std::string & getDeltaPath ( ) const
  {
    assert (is_open_m);
    return delta_path_m;
  }


  //--------------------------------------------------- getDataType ------------
  //! \brief Get the data type of the current delta file
  //!
  //! \pre delta file is open
  //! \return the data type of the current file
  //!
  const std::string & getDataType ( ) const
  {
    assert (is_open_m);
    return data_type_m;
  }


  //--------------------------------------------------- getReferencePath -------
  //! \brief Get the path of the MUMmer reference file
  //!
  //! \pre delta file is open
  //! \return the path of the MUMmer reference file
  //!
  const std::string & getReferencePath ( ) const
  {
    assert (is_open_m);
    return reference_path_m;
  }


  //--------------------------------------------------- getQueryPath -----------
  //! \brief Get the path of the MUMmer query file
  //!
  //! \pre delta file is open
  //! \return the path of the MUMmer query file
  //!
  const std::string & getQueryPath ( ) const
  {
    assert (is_open_m);
    return query_path_m;
  }
};

#endif // #ifndef __DELTA_HH
