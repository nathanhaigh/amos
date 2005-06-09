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
#include <map>
#include <sstream>
//#include <algorithm>


const std::string NUCMER_STRING = "NUCMER";
//!< String identifier for data produced by NUCmer

const std::string PROMER_STRING = "PROMER";
//!< String identifier for data produced by PROmer

typedef unsigned char DataType_t;
const DataType_t NULL_DATA = 0;
const DataType_t NUCMER_DATA = 1;
const DataType_t PROMER_DATA = 2;

typedef unsigned char Dir_t;
const Dir_t FORWARD_DIR = 0;
const Dir_t REVERSE_DIR = 1;


//===================================================== DeltaAlignment_t =======
struct DeltaAlignment_t
//!< A single delta encoded alignment region
{

  unsigned long int sR;
  //!< Start coordinate in the reference

  unsigned long int eR;
  //!< End coordinate in the reference

  unsigned long int sQ;
  //!< Start coordinate in the reference

  unsigned long int eQ;
  //!< End coordinate in the reference

  unsigned long int idyc;
  //!< number of mismatches in the alignment

  unsigned long int simc;
  //!< number of similarity scores < 1 in the alignment

  unsigned long int stpc;
  //!< number of stop codons in the alignment

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
struct DeltaRecord_t
//!< A delta record representing the alignments between two sequences
{

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
  inline bool readNext (bool isdeltas = true)
  {
    return readNextRecord (isdeltas);
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


//===================================================== DeltaEdgelet_t =========
struct DeltaEdgelet_t
//!< A piece of a delta graph edge, a single alignment
{
  unsigned char isGOOD : 1;   // meets the requirements
  unsigned char isQLIS : 1;   // is part of the query's LIS
  unsigned char isRLIS : 1;   // is part of the reference's LIS
  unsigned char isGLIS : 1;   // is part of the reference/query LIS
  unsigned char dirR   : 1;   // reference match direction
  unsigned char dirQ   : 1;   // query match direction

  float idy, sim, stp;                    // percent identity [0 - 1]
  unsigned long int idyc, simc, stpc;     // idy, sim, stp counts
  unsigned long int loQ, hiQ, loR, hiR;   // alignment bounds

  std::string delta;                           // delta information

  DeltaEdgelet_t ( )
  {
    isGOOD = true;
    isQLIS = isRLIS = isGLIS = false;
    dirR = dirQ = FORWARD_DIR;
    idy = sim = stp = 0;
    idyc = simc = stpc = 0;
    loQ = hiQ = loR = hiR = 0;
  }
};

struct DeltaNode_t;

//===================================================== DeltaEdge_t ============
struct DeltaEdge_t
//!< A delta graph edge, alignments between a single reference and query
{
  DeltaNode_t * refnode;      // the adjacent reference node
  DeltaNode_t * qrynode;      // the adjacent query node
  std::vector<DeltaEdgelet_t *> edgelets;  // the set of individual alignments

  DeltaEdge_t ( )
  {
    refnode = qrynode = NULL;
  }

  ~DeltaEdge_t ( )
  {
    std::vector<DeltaEdgelet_t *>::iterator i;
    for ( i = edgelets . begin( ); i != edgelets . end( ); ++ i )
      delete (*i);
  }

  void build (const DeltaRecord_t & rec);
};


//===================================================== DeltaNode_t ============
struct DeltaNode_t
//!< A delta graph node, contains the sequence information
{
  const std::string * id;               // the id of the sequence
  unsigned long int len;           // the length of the sequence
  std::vector<DeltaEdge_t *> edges;     // the set of related edges

  DeltaNode_t ( )
  {
    id = NULL;
    len = 0;
  }

  // DeltaGraph_t will take care of destructing the edges
};


//===================================================== DeltaGraph_t ===========
//! \brief A graph of sequences (nodes) and their alignments (edges)
//!
//!  A bipartite graph with two partite sets, R and Q, where R is the set of
//!  reference sequences and Q is the set of query sequences. These nodes are
//!  named "DeltaNode_t". We connect a node in R to a node in Q if an alignment
//!  is present between the two sequences. The group of all alignments between
//!  the two is named "DeltaEdge_t" and a single alignment between the two is
//!  named a "DeltaEdgelet_t". Alignment coordinates reference the forward
//!  strand and are stored lo before hi.
//!
//==============================================================================
class DeltaGraph_t
{
public:

  //-- The reference and query delta graph nodes (1 node per sequence)
  std::map<std::string, DeltaNode_t> refnodes;
  std::map<std::string, DeltaNode_t> qrynodes;

  std::string refpath;
  std::string qrypath;
  DataType_t datatype;

  DeltaGraph_t ( )
  {
    datatype = NULL_DATA;
  }

  ~DeltaGraph_t ( )
  {
    clear( );
  }

  void clear ( )
  {
    //-- Make sure the edges only get destructed once
    std::map<std::string, DeltaNode_t>::iterator i;
    std::vector<DeltaEdge_t *>::iterator j;
    for ( i = refnodes . begin( ); i != refnodes . end( ); ++ i )
      for ( j  = i -> second . edges . begin( );
            j != i -> second . edges . end( ); ++ j )
        delete (*j);

    refnodes.clear( );
    qrynodes.clear( );
    refpath.erase( );
    qrypath.erase( );
    datatype = NULL_DATA;
  }

  void build (const std::string & deltapath, bool isdeltas = true);

  void clean ( );

  long int getNodeCount ( );
  long int getEdgeCount ( );
  long int getEdgeletCount ( );

  void flagGLIS (float epsilon = -1);
  void flagScore (long int minlen, float minidy);
  void flagQLIS (float epsilon = -1, float maxolap = 100.0);
  void flagRLIS (float epsilon = -1, float maxolap = 100.0);
  void flagUNIQ (float minuniq);
};

#endif // #ifndef __DELTA_HH
