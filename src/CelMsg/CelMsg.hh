////////////////////////////////////////////////////////////////////////////////
/// 
/// \file CelMsgWGA.hh
/// \author Arthur L. Delcher
/// \date 02/25/2003
///
/// \brief Declarations for routines to manipulate Celera-format messages
///
/// \remark Under development
/// \todo Add other messages
/// \bug
///
////////////////////////////////////////////////////////////////////////////////


#ifndef __CEL_MSG_HH
#define __CEL_MSG_HH

#define __DEBUG


#include  "datatypes_AMOS.hh"
#include  "exceptions_AMOS.hh"
#include  "delcher.hh"
#include  "fasta.hh"
#include  <vector>
#include  <string>

using namespace std;


static const int  MAX_MSG_LINE = 1000;
  // Longest line used for messages
static const char  MAX_CELERA_QUALITY_CHAR = char (48 + 60);
  // Largest quality value allowed is 60



enum  Celera_Message_Type_t
  {NULL_MSG, ADT_MSG, BAT_MSG, CCO_MSG, DST_MSG, FRG_MSG, ICM_MSG,
   IUM_MSG, LKG_MSG};

const char *  Convert_String
    (Celera_Message_Type_t msg_type);


enum  Celera_Msg_Action_t
  {NULL_ACTION, ADD_ACTION, DELETE_ACTION};

void  Convert_Assign
    (Celera_Msg_Action_t & action, char ch);

char  Convert_Char
    (Celera_Msg_Action_t t);


enum  Celera_Msg_Read_Type_t
  {NULL_READ, READ_READ, BACTIG_READ};

void  Convert_Assign
    (Celera_Msg_Read_Type_t & read_type , char ch);

char  Convert_Char
    (Celera_Msg_Read_Type_t t);


enum  Celera_Msg_Link_Type_t
  {NULL_LINK, MATE_LINK, BAC_GUIDE, STS_GUIDE, REREAD_LINK, MAY_JOIN,
   MUST_JOIN};

void  Convert_Assign
    (Celera_Msg_Link_Type_t & link_type , char ch);

char  Convert_Char
    (Celera_Msg_Link_Type_t t);


enum  Celera_Msg_Orientation_Type_t
  {NULL_ORIENT, NORMAL_ORIENT, INNIE_ORIENT, OUTTIE_ORIENT, ANTI_ORIENT};

void  Convert_Assign
    (Celera_Msg_Orientation_Type_t & ori_type , char ch);

char  Convert_Char
    (Celera_Msg_Orientation_Type_t t);


enum  Celera_Placement_Status_t
  {NULL_PLACEMENT, PLACED, UNPLACED};

void  Convert_Assign
    (Celera_Placement_Status_t & p , char ch);

char  Convert_Char
    (Celera_Placement_Status_t p);


enum  Celera_Unitig_Status_t
  {NULL_UNITIG, UNIQUE_UNITIG, CHIMERIC_UNITIG, NOT_REZ_UNITIG, SEP_UNITIG,
   UNASSIGNED_UNITIG};

void  Convert_Assign
    (Celera_Unitig_Status_t & us , char ch);

char  Convert_Char
    (Celera_Unitig_Status_t s);


enum  Celera_Unitig_Type_t
  {NULL_UNI_T, UNIQUE_UNI_T, ROCK_UNI_T, STONE_UNI_T, PEBBLE_UNI_T,
   SINGLE_UNI_T, OTHER_UNI_T};    // Other includes surrogate

void  Convert_Assign
    (Celera_Unitig_Type_t & t , char ch);

char  Convert_Char
    (Celera_Unitig_Type_t t);



class  Celera_ADL_Sub_Msg_t
  {
  private:
   string  who;
   string  time_stamp;
   string  version;
   string  comment;

  public:
   ~ Celera_ADL_Sub_Msg_t
       ()
     {}  // Nothing to do here

   void  clear
       (void);
   const char *  id_string
       (void)
     { return "ADL"; }
   bool  print
       (FILE * fp);
   bool  read_body
       (FILE * fp);
  };


class  Celera_IMP_Sub_Msg_t
  {
  private:
   Celera_Msg_Read_Type_t  read_type;
   int  id;
   int  contained;
   string  source;
   AMOS::Ordered_Range_t  position;
   vector <int>  del;

  public:
   ~ Celera_IMP_Sub_Msg_t
       ()
     {}  // Nothing to do here

   int  getId
       (void)  const
     { return id; }
   AMOS::Ordered_Range_t  getPosition
       (void)  const
     { return position; }

   void  setContained
       (int c)
     { contained = c; }
   void  setDel
       (const vector <int> v)
     { del = v; }
   void  setID
       (int i)
     { id = i; }
   void  setPosition
       (AMOS::Pos_t s, AMOS::Pos_t e)
     { position . setRange (s, e); }
   void  setPosition
       (AMOS::Ordered_Range_t p)
     { position = p; }
   void  setReadType
       (Celera_Msg_Read_Type_t t)
     { read_type = t; }
   void  setSource
       (const char * s)
     { source = s; }

   void  clear
       (void);
   const char *  id_string
       (void)
     { return "IMP"; }
   bool  Is_Empty
     (void);
   bool  print
       (FILE * fp);
   bool  read_body
       (FILE * fp);
  };



class  Celera_MPS_Sub_Msg_t
  {
  private:
   Celera_Msg_Read_Type_t  read_type;
   int  id;
   string  source;
   AMOS::Ordered_Range_t  position;
   vector <int>  del;

  public:
   ~ Celera_MPS_Sub_Msg_t
       ()
     {}  // Nothing to do here

   int  getId
       (void)  const
     { return id; }
   AMOS::Ordered_Range_t  getPosition
       (void)  const
     { return position; }
   const vector <int> & getDel
       (void)  const
     { return del; }

   void  setDel
       (const vector <int> v)
     { del = v; }
   void  setID
       (int i)
     { id = i; }
   void  setPosition
       (AMOS::Pos_t s, AMOS::Pos_t e)
     { position . setRange (s, e); }
   void  setPosition
       (AMOS::Ordered_Range_t p)
     { position = p; }
   void  setReadType
       (Celera_Msg_Read_Type_t t)
     { read_type = t; }
   void  setSource
       (const char * s)
     { source = s; }

   void  clear
       (void);
   const char *  id_string
       (void)
     { return "MPS"; }
   bool  print
       (FILE * fp);
   bool  read_body
       (FILE * fp);
  };



class  Celera_UPS_Sub_Msg_t
  {
  private:
   Celera_Unitig_Type_t  unitig_type;
   int  id;
   AMOS::Ordered_Range_t  position;
   vector <int>  del;

  public:
   ~ Celera_UPS_Sub_Msg_t
       ()
     {}  // Nothing to do here

   Celera_Unitig_Type_t  getUnitigType
       (void)  const
     { return unitig_type; }
   int  getId
       (void)  const
     { return id; }
   AMOS::Ordered_Range_t  getPosition
       (void)  const
     { return position; }
   const vector <int> & getDel
       (void)  const
     { return del; }

   void  setDel
       (const vector <int> v)
     { del = v; }
   void  setID
       (int i)
     { id = i; }
   void  setPosition
       (AMOS::Pos_t s, AMOS::Pos_t e)
     { position . setRange (s, e); }
   void  setPosition
       (AMOS::Ordered_Range_t p)
     { position = p; }
   void  setUnitigType
       (Celera_Unitig_Type_t t)
     { unitig_type = t; }

   void  clear
       (void);
   const char *  id_string
       (void)
     { return "UPS"; }
   bool  print
       (FILE * fp);
   bool  read_body
       (FILE * fp);
  };



class  Celera_Msg_Body_t
  {
  public:
   virtual  ~ Celera_Msg_Body_t
       ()
     {}  // Ensure derived destructor is called

   virtual  const string &  getAccession
       (void);
   virtual  const AMOS::Range_t &  getClearRange
       (void);
   virtual  const string &  getConsensus
       (void);
   virtual  vector <Celera_IMP_Sub_Msg_t> &  getIMPList
       (void);
   virtual  const vector <Celera_MPS_Sub_Msg_t> &  getMPSList
       (void);
   virtual  const vector <Celera_UPS_Sub_Msg_t> &  getUPSList
       (void);
   virtual  int  getNumFrags
       (void);
   virtual  int  getNumUnitigs
       (void);
   virtual  const string &  getQuality
       (void);
   virtual  const string &  getSequence
       (void);
   virtual  const string &  getSource
       (void);

   virtual  void  setAccession
       (const char * s);
   virtual  void  setAccession
       (const string & s);
   virtual  void  setClearRange
       (AMOS::Pos_t a, AMOS::Pos_t b);
   virtual  void  setIMPs
       (const vector <int> & f, const vector <AMOS::Ordered_Range_t> & p);
   virtual  void  setQuality
       (const char * q);
   virtual  void  setQuality
       (const string & q);
   virtual  void  setSequence
       (const char * s);
   virtual  void  setSequence
       (const string & s);
   virtual  void  setStatus
       (Celera_Unitig_Status_t s);
   virtual  void  setUniLen
       (int n);

   virtual  void  clear
       (void) = 0;
   virtual  const char *  id_string
       (void) = 0;
   virtual  bool  print_body
       (FILE * fp) = 0;
   virtual  bool  read_body
       (FILE * fp) = 0;
   virtual  void  Remove_Empty_IMPs
     (void);
   virtual  void  Update_IMPs
       (const vector <AMOS::Range_t> & pos, const vector <int> & ref,
        const vector < vector <int> > & del_list);
  };


class  Celera_ADT_Msg_Body_t  :  public Celera_Msg_Body_t
  {
  private:
   vector <Celera_ADL_Sub_Msg_t>  adl_list;

  public:
   ~ Celera_ADT_Msg_Body_t
       ()
     {}  // Nothing to do here

   void  clear
       (void);
   const char *  id_string
       (void)
     { return "ADT"; }

   bool  print_body
       (FILE * fp);
   bool  read_body
       (FILE * fp);
  };


class  Celera_BAT_Msg_Body_t  :  public Celera_Msg_Body_t
  {
  private:
   string  batch_name;
   string  time_stamp;
   string  accession;
   string  comment;
  public:
   ~ Celera_BAT_Msg_Body_t
       ()
     {}  // Nothing to do here

   void  clear
       (void);
   const char *  id_string
       (void)
     { return "BAT"; }
   bool  print_body
       (FILE * fp);
   bool  read_body
       (FILE * fp);
  };


class  Celera_CCO_Msg_Body_t  :  public Celera_Msg_Body_t
  {
  private:
   string  accession;
   Celera_Placement_Status_t  placed;
   int  len;
   string  consensus;
   string  quality;
   bool  forced;
   int  num_frags;
   int  num_unitigs;
   vector <Celera_MPS_Sub_Msg_t>  mps_list;
   vector <Celera_UPS_Sub_Msg_t>  ups_list;

  public:
   ~ Celera_CCO_Msg_Body_t
       ()
     {}  // Nothing to do here

   const string &  getAccession
       (void)
     { return  accession; }
   Celera_Placement_Status_t  getPlaced
       (void)
     { return  placed; }
   int  getLen
       (void)
     { return  len; }
   const string &  getConsensus
       (void)
     { return  consensus; }
   const string &  getQuality
       (void)
     { return  quality; }
   bool  getForced
       (void)
     { return  forced; }
   int  getNumFrags
       (void)
     { return  num_frags; }
   int  getNumUnitigs
       (void)
     { return  num_unitigs; }
   const vector <Celera_MPS_Sub_Msg_t> &  getMPSList
       (void)
     { return  mps_list; }
   const vector <Celera_UPS_Sub_Msg_t> &  getUPSList
       (void)
     { return  ups_list; }

   void  clear
       (void);
   const char *  id_string
       (void)
     { return "CCO"; }
   bool  print_body
       (FILE * fp);
   bool  read_body
       (FILE * fp);
  };


class  Celera_DST_Msg_Body_t  :  public Celera_Msg_Body_t
  {
  private:
   Celera_Msg_Action_t  action;
   string  accession;
   double  mean;
   double  std_dev;

  public:
   ~ Celera_DST_Msg_Body_t
       ()
     {}  // Nothing to do here

   void  clear
       (void);
   const char *  id_string
       (void)
     { return "DST"; }
   bool  print_body
       (FILE * fp);
   bool  read_body
       (FILE * fp);
  };


class  Celera_FRG_Msg_Body_t  :  public Celera_Msg_Body_t
  {
  private:
   Celera_Msg_Action_t  action;
   string  accession;
   Celera_Msg_Read_Type_t  read_type;
   string  source;
   string  time_stamp;
   string  sequence;
   string  quality;
   AMOS::Range_t  clear_range;

  public:
   ~ Celera_FRG_Msg_Body_t
       ()
     {}  // Nothing to do here

   Celera_Msg_Action_t  getAction
       (void)
     { return  action; }
   const string &  getAccession
       (void)
     { return  accession; }
   Celera_Msg_Read_Type_t  getReadType
       (void)
     { return  read_type; }
   const string &  getSource
       (void)
     { return  source; }
   const string &  getTimeStamp
       (void)
     { return  time_stamp; }
   const string &  getQuality
       (void)
     { return  quality; }
   const string &  getSequence
       (void)
     { return  sequence; }
   const AMOS::Range_t &  getClearRange
       (void)
     { return  clear_range; }

   void  setClearRange
       (AMOS::Pos_t a, AMOS::Pos_t b)
     { clear_range . setRange (a, b); }
   void  setQuality
       (const char * q)
     { quality = q; }
   void  setQuality
       (const string & q)
     { quality = q; }
   void  setSequence
       (const char * s)
     { sequence = s; }
   void  setSequence
       (const string & s)
     { sequence = s; }

   void  clear
       (void);
   const char *  id_string
       (void)
     { return "FRG"; }
   bool  print_body
       (FILE * fp);
   bool  read_body
       (FILE * fp);
  };


class  Celera_ICM_Msg_Body_t  :  public Celera_Msg_Body_t
  {
  private:
   string  content;
  public:
   ~ Celera_ICM_Msg_Body_t
       ()
     {}  // Nothing to do here

   void  clear
       (void);
   const char *  id_string
       (void)
     { return "ICM"; }
   bool  print_body
       (FILE * fp);
   bool  read_body
       (FILE * fp);
  };


class  Celera_IUM_Msg_Body_t  :  public Celera_Msg_Body_t
  {
  private:
   string  accession;
   string  source;
   float  cover_stat;
   Celera_Unitig_Status_t  status;
   int  a_branch_point;
   int  b_branch_point;
   int  uni_len;
   string  consensus;
   string  quality;
   bool  forced;
   int  num_frags;
   vector <Celera_IMP_Sub_Msg_t>  imp_list;

  public:
   ~ Celera_IUM_Msg_Body_t
       ()
     {}  // Nothing to do here

   const string &  getAccession
       (void)
     { return  accession; }
   const string &  getSource
       (void)
     { return  source; }
   float  getCoverStat
       (void)
     { return  cover_stat; }
   Celera_Unitig_Status_t  getStatus
       (void)
     { return  status; }
   int  getABranchPoint
       (void)
     { return  a_branch_point; }
   int  getBBranchPoint
       (void)
     { return  b_branch_point; }
   int  getUniLen
       (void)
     { return  uni_len; }
   const string &  getConsensus
       (void)
     { return  consensus; }
   const string &  getQuality
       (void)
     { return  quality; }
   bool  getForced
       (void)
     { return  forced; }
   int  getNumFrags
       (void)
     { return  num_frags; }
   vector <Celera_IMP_Sub_Msg_t> &  getIMPList
       (void)
     { return  imp_list; }

   void  setAccession
       (const char * s)
     { accession = s; }
   void  setAccession
       (const string & s)
     { accession = s; }
   void  setIMPs
       (const vector <int> & f, const vector <AMOS::Ordered_Range_t> & p);
   void  setQuality
       (const char * q)
     { quality = q; }
   void  setSequence
       (const char * s)
     { consensus = s; }
   void  setStatus
       (Celera_Unitig_Status_t s)
     { status = s; }
   void  setUniLen
       (int n)
     { uni_len = n; }

   void  clear
       (void);
   const char *  id_string
       (void)
     { return "IUM"; }
   bool  print_body
       (FILE * fp);
   bool  read_body
       (FILE * fp);
   void  Remove_Empty_IMPs
     (void);
   void  Update_IMPs
       (const vector <AMOS::Range_t> & pos, const vector <int> & ref,
        const vector < vector <int> > & del_list);
  };


class  Celera_LKG_Msg_Body_t  :  public Celera_Msg_Body_t
  {
  private:
   Celera_Msg_Action_t  action;
   Celera_Msg_Link_Type_t  link_type;
   string  accession1, accession2;
   string  time_stamp;
   string  dst_ref;
   Celera_Msg_Orientation_Type_t  ori;
   

  public:
   ~ Celera_LKG_Msg_Body_t
       ()
     {}  // Nothing to do here

   void  clear
       (void);
   const char *  id_string
       (void)
     { return "LKG"; }
   bool  print_body
       (FILE * fp);
   bool  read_body
       (FILE * fp);
  };



class  Celera_Message_t
  {
  protected:
   static const int  MAX_MSG_LINE = 1000;
     // length of longest possible message input line

   Celera_Message_Type_t  msg_type;
   Celera_Msg_Body_t  * body;

  public:
   Celera_Message_t
       ()
     {
      msg_type = NULL_MSG;
      body = NULL;
     }

   ~ Celera_Message_t
       ()
     {
      clear ();
     }

   void  clear
       (void)
     {
      delete  body;
      body = NULL;
      msg_type = NULL_MSG;

      return;
     }
       
   const Celera_Message_Type_t &  getMsgType
       (void)  const
     { return  msg_type; }
   const Celera_Msg_Body_t *  getBody
       (void)  const
     { return  body; }

   const string &  getAccession
      (void)  const;
   const AMOS::Range_t &  getClearRange
      (void)  const;
   const string &  getConsensus
       (void) const;
   vector <Celera_IMP_Sub_Msg_t> &  getIMPList
       (void)  const;
   const vector <Celera_MPS_Sub_Msg_t> &  getMPSList
       (void)  const;
   const vector <Celera_UPS_Sub_Msg_t> &  getUPSList
       (void)  const;
   int  getNumFrags
       (void)  const;
   int  getNumUnitigs
       (void)  const;
   const string &  getQuality
      (void)  const;
   const string &  getSequence
      (void)  const;
   const string &  getSource
      (void)  const;

   void  setAccession
      (const char * s);
   void  setAccession
      (const string & s);
   void  setClearRange
       (AMOS::Pos_t a, AMOS::Pos_t b);
   void  setIMPs
       (const vector <int> & f, const vector <AMOS::Ordered_Range_t> & p);
   void  setQuality
      (const char * q);
   void  setQuality
      (const string & q);
   void  setSequence
      (const char * s);
   void  setSequence
      (const string & s);
   void  setStatus
       (Celera_Unitig_Status_t s);
   void  setType
      (Celera_Message_Type_t t);
   void  setUniLen
      (int n);

   const char *  id_string
       (void)  const
     {
      if  (body == NULL)
          return  "NULL";
        else
          return  body -> id_string ();
     }
   void  print
       (FILE * fp, const char * seq = NULL, const char * qv = NULL);
   bool  read
       (FILE * fp);
   void  Remove_Empty_IMPs
     (void);
   void  Update_IMPs
       (const vector <AMOS::Range_t> & pos, const vector <int> & ref,
        const vector < vector <int> > & del_list);
  };



void  Read_Comment_String
    (FILE * fp, string & s);
void  Read_Sequence_String
    (FILE * fp, string & s);



#endif  // #ifndef __CEL_MSG_HH
