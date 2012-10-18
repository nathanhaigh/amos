////////////////////////////////////////////////////////////////////////////////
///
/// \file CelMsgWGA.cc
/// \author Arthur L. Delcher
/// \date 02/25/2003
///
/// \brief Code for routines to manipulate Celera-format messages
///
/// \remark Under development
/// \todo Add other messages
/// \bug
///
////////////////////////////////////////////////////////////////////////////////



#include "CelMsg.hh"
using namespace AMOS;

void  Celera_ADL_Sub_Msg_t :: clear
    (void)

//  Clear all entries in this message

  {
   who . erase ();
   time_stamp . erase ();
   version . erase ();
   comment . erase ();

   return;
  }


bool  Celera_ADL_Sub_Msg_t :: print
    (FILE * fp)

//  Print the contents of this  ADL  submessage to  fp

  {
   fprintf (fp, "{ADL\n");
   fprintf (fp, "who:%s\n", who . c_str ());
   fprintf (fp, "ctm:%s\n", time_stamp . c_str ());
   fprintf (fp, "vsn:%s\n", version . c_str ());
   fprintf (fp, "com:\n%s.\n", comment . c_str ());
   fprintf (fp, "}\n");

   return  false;
  }


bool  Celera_ADL_Sub_Msg_t :: read_body
    (FILE * fp)

//  Read the rest of the body of an ADL submessage

  {
   char  line [MAX_MSG_LINE];
   string  s;
   int  level;

   level = 1;
   while  (level > 0 && fgets (line, MAX_MSG_LINE, fp) != NULL)
     {
      int  len;

      len = strlen (line);
      if  (len >= MAX_MSG_LINE - 1)
	throw IOException_t ("Celera message line too long");
      
      if  (strncmp (line, "who:", 4) == 0)
          who . append (Strip_Trailing (line + 4, '\n'));
      else if  (strncmp (line, "ctm:", 4) == 0)
          time_stamp . append (Strip_Trailing (line + 4, '\n'));
      else if  (strncmp (line, "vsn:", 4) == 0)
          version . append (Strip_Trailing (line + 4, '\n'));
      else if  (strncmp (line, "com:", 4) == 0)
          {
           Read_Comment_String (fp, s);
           comment . append (s);
          }
      else if  (line [0] == '}')
          level --;
        else
          {
           cerr << line << endl;
           throw IOException_t ("Bad line in ADL sub-message");
          }
     }

   return  (level == 0);
  }



void  Celera_IMP_Sub_Msg_t :: clear
    (void)

//  Clear all entries in this message

  {
   read_type = NULL_READ;
   id = 0;
   contained = false;
   source . erase ();
   position . clear ();
   del . clear ();

   return;
  }



bool  Celera_IMP_Sub_Msg_t :: Is_Empty
  (void)

// Return  true  iff both positions are zero

{
  return (position . getBegin () == 0 && position . getEnd () == 0);
}



bool  Celera_IMP_Sub_Msg_t :: print
    (FILE * fp)

//  Print the contents of this  IMP  submessage to  fp

  {
   int  i, n, ct;

   fprintf (fp, "{IMP\n");
   fprintf (fp, "typ:%c\n", Convert_Char (read_type));
   fprintf (fp, "mid:%d\n", id);
   fprintf (fp, "con:%d\n", contained);
   fprintf (fp, "src:\n%s\n.\n", source . c_str ());
   fprintf (fp, "pos:%d,%d\n", position . getBegin (), position . getEnd ());
   
   n = del . size ();
   fprintf (fp, "dln:%d\n", n);
   fprintf (fp, "del:\n");

   ct = 0;
   for  (i = 0;  i < n;  i ++)
     {
      if  (ct > 0)
          fprintf (fp, " ");
      fprintf (fp, "%d", del [i]);
      ct ++;
      if  (i == n - 1 || ct == 20)
          {
           fprintf (fp, "\n");
           ct = 0;
          }
     }

   fprintf (fp, "}\n");

   return  false;
  }


bool  Celera_IMP_Sub_Msg_t :: read_body
    (FILE * fp)

//  Read the rest of the body of an IMP submessage

  {
   char  line [MAX_MSG_LINE];
   int  ct, level, state;

   del . clear ();
   level = 1;
   ct = state = 0;
   while  (level > 0 && fgets (line, MAX_MSG_LINE, fp) != NULL)
     {
      int  len;

      len = strlen (line);
      if  (len >= MAX_MSG_LINE - 1)
          throw IOException_t ("Celera message line too long");
      if  (line [len - 1] == '\n')
          line [-- len] = '\0';
      
      if  (state > 0 && strcmp (line, ".") == 0)
          state = 0;
      else if  (state == 1)
          {
           ct ++;
           if  (ct == 1)
               source = line;
             else
               {
                source . append ("\n");
                source . append (line);
               }
          }
      else if  (state == 2)
          {
           if  (line [0] == '}')
               level --;
             else
               {
                char  * p;
                int  k;

                for  (p = strtok (line, " \t\n");  p != NULL;
                        p = strtok (NULL, " \t\n"))
                  {
                   k = strtol (p, NULL, 10);
                   del . push_back (k);
                  }
               }
          }
      else if  (strncmp (line, "typ:", 4) == 0)
          Convert_Assign (read_type, line [4]);
      else if  (strncmp (line, "mid:", 4) == 0)
          id = int (strtol (line + 4, NULL, 10));
      else if  (strncmp (line, "con:", 4) == 0)
          contained = int (strtol (line + 4, NULL, 10));
      else if  (strncmp (line, "src:", 4) == 0)
          state = 1;
      else if  (strncmp (line, "pos:", 4) == 0)
          {
           uint32_t  start, end;

           if  (sscanf (line + 4, "%u,%u", & start, & end) != 2)
               {
                cerr << line << endl;
                throw IOException_t ("Bad position range in IMP message");
               }
           position . setRange (start, end);
          }
      else if  (strncmp (line, "dln:", 4) == 0)
          ;  // ignore
      else if  (strncmp (line, "del:", 4) == 0)
          state = 2;
      else if  (line [0] == '}')
          level --;
        else
          {
           cerr << line << endl;
           throw IOException_t ("Bad line in IMP sub-message");
          }
     }

   return  (level == 0);
  }



void  Celera_MPS_Sub_Msg_t :: clear
    (void)

//  Clear all entries in this message

  {
   read_type = NULL_READ;
   id = 0;
   source . erase ();
   position . clear ();
   del . clear ();

   return;
  }


bool  Celera_MPS_Sub_Msg_t :: print
    (FILE * fp)

//  Print the contents of this  MPS  submessage to  fp

  {
   int  i, n, ct;

   fprintf (fp, "{MPS\n");
   fprintf (fp, "typ:%c\n", Convert_Char (read_type));
   fprintf (fp, "mid:%d\n", id);
   fprintf (fp, "src:\n%s\n.\n", source . c_str ());
   fprintf (fp, "pos:%d,%d\n", position . getBegin (), position . getEnd ());
   
   n = del . size ();
   fprintf (fp, "dln:%d\n", n);
   fprintf (fp, "del:\n");

   ct = 0;
   for  (i = 0;  i < n;  i ++)
     {
      if  (ct > 0)
          fprintf (fp, " ");
      fprintf (fp, "%d", del [i]);
      ct ++;
      if  (i == n - 1 || ct == 20)
          {
           fprintf (fp, "\n");
           ct = 0;
          }
     }

   fprintf (fp, "}\n");

   return  false;
  }


bool  Celera_MPS_Sub_Msg_t :: read_body
    (FILE * fp)

//  Read the rest of the body of an MPS submessage

  {
   char  line [MAX_MSG_LINE];
   int  ct, dln, level, state;

   del . clear ();
   level = 1;
   ct = state = 0;
   while  (level > 0 && fgets (line, MAX_MSG_LINE, fp) != NULL)
     {
      int  len;

      len = strlen (line);
      if  (len >= MAX_MSG_LINE - 1)
          throw IOException_t ("Celera message line too long");
      if  (line [len - 1] == '\n')
          line [-- len] = '\0';
      
      if  (state > 0 && strcmp (line, ".") == 0)
          state = 0;
      else if  (state == 1)
          {
           ct ++;
           if  (ct == 1)
               source = line;
             else
               {
                source . append ("\n");
                source . append (line);
               }
          }
      else if  (line [0] == '}')
          level --;
      else if  (state == 2)
          {
           char  * p;
           int  k;

           for  (p = strtok (line, " \t\n");  p != NULL;
                   p = strtok (NULL, " \t\n"))
             {
              k = strtol (p, NULL, 10);
              del . push_back (k);
             }
          }
      else if  (strncmp (line, "typ:", 4) == 0)
          Convert_Assign (read_type, line [4]);
      else if  (strncmp (line, "mid:", 4) == 0)
          id = int (strtol (line + 4, NULL, 10));
      else if  (strncmp (line, "src:", 4) == 0)
          state = 1;
      else if  (strncmp (line, "pos:", 4) == 0)
          {
           uint32_t  start, end;

           if  (sscanf (line + 4, "%u,%u", & start, & end) != 2)
               {
                cerr << line << endl;
                throw IOException_t ("Bad position range in MPS sub-message");
               }
           position . setRange (start, end);
          }
      else if  (strncmp (line, "dln:", 4) == 0)
          dln = int (strtol (line + 4, NULL, 10));
      else if  (strncmp (line, "del:", 4) == 0)
          state = 2;
        else
          {
           cerr << line << endl;
           throw IOException_t ("Bad line in MPS sub-message");
          }
     }

   assert (size_t (dln) == del . size ());

   return  (level == 0);
  }



void  Celera_UPS_Sub_Msg_t :: clear
    (void)

//  Clear all entries in this message

  {
   unitig_type = NULL_UNI_T;
   id = 0;
   position . clear ();
   del . clear ();

   return;
  }


bool  Celera_UPS_Sub_Msg_t :: print
    (FILE * fp)

//  Print the contents of this  UPS  submessage to  fp

  {
   int  i, n, ct;

   fprintf (fp, "{UPS\n");
   fprintf (fp, "typ:%c\n", Convert_Char (unitig_type));
   fprintf (fp, "lid:%d\n", id);
   fprintf (fp, "pos:%d,%d\n", position . getBegin (), position . getEnd ());
   
   n = del . size ();
   fprintf (fp, "dln:%d\n", n);
   fprintf (fp, "del:\n");

   ct = 0;
   for  (i = 0;  i < n;  i ++)
     {
      if  (ct > 0)
          fprintf (fp, " ");
      fprintf (fp, "%d", del [i]);
      ct ++;
      if  (i == n - 1 || ct == 20)
          {
           fprintf (fp, "\n");
           ct = 0;
          }
     }

   fprintf (fp, "}\n");

   return  false;
  }


bool  Celera_UPS_Sub_Msg_t :: read_body
    (FILE * fp)

//  Read the rest of the body of an UPS submessage

  {
   char  line [MAX_MSG_LINE];
   int  ct, dln, level, state;

   del . clear ();
   level = 1;
   ct = state = 0;
   while  (level > 0 && fgets (line, MAX_MSG_LINE, fp) != NULL)
     {
      int  len;

      len = strlen (line);
      if  (len >= MAX_MSG_LINE - 1)
          throw IOException_t ("Celera message line too long");
      if  (line [len - 1] == '\n')
          line [-- len] = '\0';
      
      if  (line [0] == '}')
          level --;
      else if  (state == 2)
          {
           char  * p;
           int  k;

           for  (p = strtok (line, " \t\n");  p != NULL;
                   p = strtok (NULL, " \t\n"))
             {
              k = strtol (p, NULL, 10);
              del . push_back (k);
             }
          }
      else if  (strncmp (line, "typ:", 4) == 0)
          Convert_Assign (unitig_type, line [4]);
      else if  (strncmp (line, "lid:", 4) == 0)
          id = int (strtol (line + 4, NULL, 10));
      else if  (strncmp (line, "pos:", 4) == 0)
          {
           uint32_t  start, end;

           if  (sscanf (line + 4, "%u,%u", & start, & end) != 2)
               {
                cerr << line << endl;
                throw IOException_t ("Bad position range in UPS sub-message");
               }
           position . setRange (start, end);
          }
      else if  (strncmp (line, "dln:", 4) == 0)
          dln = int (strtol (line + 4, NULL, 10));
      else if  (strncmp (line, "del:", 4) == 0)
          state = 2;
        else
          {
           cerr << line << endl;
           throw IOException_t ("Bad line in UPS sub-message");
          }
     }

   assert (size_t (dln) == del . size ());

   return  (level == 0);
  }



const string &  Celera_Msg_Body_t :: getAccession
    (void)
  {
   sprintf (Clean_Exit_Msg_Line, "getAccession illegal for %s message",
            id_string ());
   throw IOException_t (Clean_Exit_Msg_Line);
  }


const Range_t &  Celera_Msg_Body_t :: getClearRange
    (void)
  {
   sprintf (Clean_Exit_Msg_Line, "getClearRange illegal for %s message",
            id_string ());
   throw IOException_t (Clean_Exit_Msg_Line);
  }


const string &  Celera_Msg_Body_t :: getConsensus
    (void)
  {
   sprintf (Clean_Exit_Msg_Line, "getConsensus illegal for %s message",
            id_string ());
   throw IOException_t (Clean_Exit_Msg_Line);
  }


vector <Celera_IMP_Sub_Msg_t> &  Celera_Msg_Body_t :: getIMPList
    (void)

  {
   sprintf (Clean_Exit_Msg_Line, "getIMPList illegal for %s message",
            id_string ());
   throw IOException_t (Clean_Exit_Msg_Line);
  }


const vector <Celera_MPS_Sub_Msg_t> &  Celera_Msg_Body_t :: getMPSList
    (void)

  {
   sprintf (Clean_Exit_Msg_Line, "getMPSList illegal for %s message",
            id_string ());
   throw IOException_t (Clean_Exit_Msg_Line);
  }


int  Celera_Msg_Body_t :: getNumFrags
    (void)

  {
   sprintf (Clean_Exit_Msg_Line, "getNumFrags illegal for %s message",
            id_string ());
   throw IOException_t (Clean_Exit_Msg_Line);
  }


int  Celera_Msg_Body_t :: getNumUnitigs
    (void)

  {
   sprintf (Clean_Exit_Msg_Line, "getNumUnitigs illegal for %s message",
            id_string ());
   throw IOException_t (Clean_Exit_Msg_Line);
  }


const string &  Celera_Msg_Body_t :: getQuality
    (void)
  {
   sprintf (Clean_Exit_Msg_Line, "getQuality illegal for %s message",
            id_string ());
   throw IOException_t (Clean_Exit_Msg_Line);
  }


const string &  Celera_Msg_Body_t :: getSequence
    (void)
  {
   sprintf (Clean_Exit_Msg_Line, "getSequence illegal for %s message",
            id_string ());
   throw IOException_t (Clean_Exit_Msg_Line);
  }


const string &  Celera_Msg_Body_t :: getSource
    (void)
  {
   sprintf (Clean_Exit_Msg_Line, "getAccession illegal for %s message",
            id_string ());
   throw IOException_t (Clean_Exit_Msg_Line);
  }


const vector <Celera_UPS_Sub_Msg_t> &  Celera_Msg_Body_t :: getUPSList
    (void)

  {
   sprintf (Clean_Exit_Msg_Line, "getUPSList illegal for %s message",
            id_string ());
   throw IOException_t (Clean_Exit_Msg_Line);
  }


void  Celera_Msg_Body_t :: setAccession
    (const char * s)
  {
   sprintf (Clean_Exit_Msg_Line, "setAccession illegal for %s message",
            id_string ());
   throw IOException_t (Clean_Exit_Msg_Line);
  }


void  Celera_Msg_Body_t :: setAccession
    (const string & s)
  {
   sprintf (Clean_Exit_Msg_Line, "setAccession illegal for %s message",
            id_string ());
   throw IOException_t (Clean_Exit_Msg_Line);
  }


void  Celera_Msg_Body_t :: setClearRange
    (Pos_t a, Pos_t b)
  {
   sprintf (Clean_Exit_Msg_Line, "setClearRange illegal for %s message",
            id_string ());
   throw IOException_t (Clean_Exit_Msg_Line);
  }


void  Celera_Msg_Body_t :: setIMPs
    (const vector <int> & f, const vector <Ordered_Range_t> & p)
  {
   sprintf (Clean_Exit_Msg_Line, "setIMPs illegal for %s message",
            id_string ());
   throw IOException_t (Clean_Exit_Msg_Line);
  }


void  Celera_Msg_Body_t :: setQuality
    (const char * q)
  {
   sprintf (Clean_Exit_Msg_Line, "setQuality illegal for %s message",
            id_string ());
   throw IOException_t (Clean_Exit_Msg_Line);
  }


void  Celera_Msg_Body_t :: setQuality
    (const string & q)
  {
   sprintf (Clean_Exit_Msg_Line, "setQuality illegal for %s message",
            id_string ());
   throw IOException_t (Clean_Exit_Msg_Line);
  }


void  Celera_Msg_Body_t :: setSequence
    (const char * s)
  {
   sprintf (Clean_Exit_Msg_Line, "setSequence illegal for %s message",
            id_string ());
   throw IOException_t (Clean_Exit_Msg_Line);
  }


void  Celera_Msg_Body_t :: setSequence
    (const string & s)
  {
   sprintf (Clean_Exit_Msg_Line, "setSequence illegal for %s message",
            id_string ());
   throw IOException_t (Clean_Exit_Msg_Line);
  }


void  Celera_Msg_Body_t :: setStatus
    (Celera_Unitig_Status_t s)
  {
   sprintf (Clean_Exit_Msg_Line, "setStatus illegal for %s message",
            id_string ());
   throw IOException_t (Clean_Exit_Msg_Line);
  }


void  Celera_Msg_Body_t :: setUniLen
    (int n)
  {
   sprintf (Clean_Exit_Msg_Line, "setUniLen illegal for %s message",
            id_string ());
   throw IOException_t (Clean_Exit_Msg_Line);
  }


void  Celera_Msg_Body_t :: Remove_Empty_IMPs
  (void)
{
  sprintf (Clean_Exit_Msg_Line, "Remove_Empty_IMPs illegal for %s message",
           id_string ());
  throw IOException_t (Clean_Exit_Msg_Line);
}


void  Celera_Msg_Body_t :: Update_IMPs
    (const vector <Range_t> & pos, const vector <int> & ref,
     const vector < vector <int> > & del_list)
  {
   sprintf (Clean_Exit_Msg_Line, "Update_IMPs illegal for %s message",
            id_string ());
   throw IOException_t (Clean_Exit_Msg_Line);
  }



void  Celera_ADT_Msg_Body_t :: clear
    (void)

//  Clear all entries in this message

  {
   adl_list . clear ();

   return;
  }



bool  Celera_ADT_Msg_Body_t :: print_body
    (FILE * fp)

//  Print the body of this  ADT  message to  fp

  {
   vector <Celera_ADL_Sub_Msg_t> :: iterator  p;

   for  (p = adl_list . begin ();  p != adl_list . end ();  p ++)
     p -> print (fp);

   fprintf (fp, ".\n");

   return  true;
  }



bool  Celera_ADT_Msg_Body_t :: read_body
    (FILE * fp)

//  Read the rest of the body of an ADT message

  {
   Celera_ADL_Sub_Msg_t  adt_msg;
   char  line [MAX_MSG_LINE];

   adl_list . clear ();

   while  (fgets (line, MAX_MSG_LINE, fp) != NULL)
     if  (strcmp (line, "{ADL\n") == 0)
         {
          adt_msg . clear ();
          adt_msg . read_body (fp);
          adl_list . push_back (adt_msg);
         }
     else if  (strcmp (line, ".\n") == 0)
         {
          char  * result;

          result = fgets (line, MAX_MSG_LINE, fp);
          if  (result == NULL || strcmp (line, "}\n") != 0)
              {
               sprintf (Clean_Exit_Msg_Line, "Unexpected end of ADT message");
               throw IOException_t (Clean_Exit_Msg_Line);
              }

          return  true;
         }
       else
         return  false;

   return  false;
  }



void  Celera_BAT_Msg_Body_t :: clear
    (void)

//  Clear all entries in this message

  {
   batch_name . erase ();
   time_stamp . erase ();
   accession . erase ();
   comment . erase ();

   return;
  }



bool  Celera_BAT_Msg_Body_t :: print_body
    (FILE * fp)

//  Print the body of this  BAT  message to  fp

  {
   fprintf (fp, "bna:%s\n", batch_name . c_str ());
   fprintf (fp, "crt:%s\n", time_stamp . c_str ());
   fprintf (fp, "acc:%s\n", accession . c_str ());
   fprintf (fp, "com:\n%s.\n", comment . c_str ());

   return  true;
  }



bool  Celera_BAT_Msg_Body_t :: read_body
    (FILE * fp)

//  Read the rest of the body of an BAT message

  {
   char  line [MAX_MSG_LINE];
   string  s;
   int  level;

   level = 1;
   while  (level > 0 && fgets (line, MAX_MSG_LINE, fp) != NULL)
     if  (strncmp (line, "bna:", 4) == 0)
          batch_name . append (Strip_Trailing (line + 4, '\n'));
     else if  (strncmp (line, "crt:", 4) == 0)
          time_stamp . append (Strip_Trailing (line + 4, '\n'));
     else if  (strncmp (line, "acc:", 4) == 0)
          accession . append (Strip_Trailing (line + 4, '\n'));
     else if  (strncmp (line, "com:", 4) == 0)
          {
           Read_Comment_String (fp, s);
           comment . append (s);
          }
     else if  (line [0] == '}')
         level --;
        else
          {
           cerr << line << endl;
           throw IOException_t ("Bad line in BAT message");
          }

   return  (level == 0);
  }



void  Celera_CCO_Msg_Body_t :: clear
    (void)

//  Clear all entries in this message

  {
   accession . erase ();
   placed = NULL_PLACEMENT;
   len = 0;
   consensus . erase ();
   quality . erase ();
   forced = false;
   num_frags = 0;
   num_unitigs = 0;
   mps_list . clear ();
   ups_list . clear ();

   return;
  }



bool  Celera_CCO_Msg_Body_t :: print_body
    (FILE * fp)

//  Print the body of this  CCO  message to  fp

  {
   vector <Celera_MPS_Sub_Msg_t> :: iterator  p;
   vector <Celera_UPS_Sub_Msg_t> :: iterator  q;

   fprintf (fp, "acc:%s\n", accession . c_str ());
   fprintf (fp, "pla:%c\n", Convert_Char (placed));
   fprintf (fp, "len:%d\n", len);
   fprintf (fp, "cns:\n");
   Fasta_Print (fp, consensus . c_str (), NULL);
   fprintf (fp, ".\n");
   fprintf (fp, "qlt:\n");
   Fasta_Print (fp, quality . c_str (), NULL);
   fprintf (fp, ".\n");
   fprintf (fp, "for:%c\n", forced ? '1' : '0');
   fprintf (fp, "npc:%d\n", num_frags);
   fprintf (fp, "nou:%d\n", num_unitigs);

   for  (p = mps_list . begin ();  p != mps_list . end ();  p ++)
     p -> print (fp);

   for  (q = ups_list . begin ();  q != ups_list . end ();  q ++)
     q -> print (fp);

   return  false;
  }



bool  Celera_CCO_Msg_Body_t :: read_body
    (FILE * fp)

//  Read the rest of the body of an CCO message

  {
   char  line [MAX_MSG_LINE];
   int  level, state;
   int  line_ct = 0;

   consensus . erase ();
   quality . erase ();

   level = 1;
   state = 0;
   while  (level > 0 && fgets (line, MAX_MSG_LINE, fp) != NULL)
     {
      int  len;

      line_ct ++;
      len = strlen (line);
      if  (len >= MAX_MSG_LINE - 1)
          throw IOException_t ("Celera message line too long");
      if  (line [len - 1] == '\n')
          line [-- len] = '\0';
      
      if  (state > 0 && strcmp (line, ".") == 0)
          state = 0;
      else if  (state == 1)
          consensus . append (line);
      else if  (state == 2)
          quality . append (line);
      else if  (strncmp (line, "acc:", 4) == 0)
          accession . append (line + 4);
      else if  (strncmp (line, "pla:", 4) == 0)
          Convert_Assign (placed, line [4]);
      else if  (strncmp (line, "len:", 4) == 0)
          len = int (strtol (line + 4, NULL, 10));
      else if  (strncmp (line, "cns:", 4) == 0)
          state = 1;
      else if  (strncmp (line, "qlt:", 4) == 0)
          state = 2;
      else if  (strncmp (line, "for:", 4) == 0)
          forced = (line [4] != '0');
      else if  (strncmp (line, "npc:", 4) == 0)
          num_frags = int (strtol (line + 4, NULL, 10));
      else if  (strncmp (line, "nou:", 4) == 0)
          {
           Celera_MPS_Sub_Msg_t  mps_message;
           Celera_UPS_Sub_Msg_t  ups_message;
           int  i;

           num_unitigs = int (strtol (line + 4, NULL, 10));

           mps_list . clear ();
           for  (i = 0;  i < num_frags;  i ++)
             {
              char  * p;

              p = fgets (line, MAX_MSG_LINE, fp);
              if  (p == NULL || strcmp (line, "{MPS\n") != 0)
                  {
                   cerr << line;
                   throw IOException_t ("Line should have been \"{MPS\"\n");
                  }
              mps_message . read_body (fp);
              mps_list . push_back (mps_message);
             }

           ups_list . clear ();
           for  (i = 0;  i < num_unitigs;  i ++)
             {
              if  (fgets (line, MAX_MSG_LINE, fp) == NULL
                     || strcmp (line, "{UPS\n") != 0)
                  {
                   cerr << line;
                   throw IOException_t ("Line should have been \"{UPS\"\n");
                  }
              ups_message . read_body (fp);
              ups_list . push_back (ups_message);
             }
          }
      else if  (line [0] == '}')
          level --;
        else
          {
           cerr << line << endl;
           throw IOException_t ("Bad line in CCO message");
          }
     }

   return  (level == 0);
  }



void  Celera_DST_Msg_Body_t :: clear
    (void)

//  Clear all entries in this message

  {
   action = NULL_ACTION;  
   accession . erase ();
   mean = 0.0;
   std_dev = 0.0;

   return;
  }



bool  Celera_DST_Msg_Body_t :: print_body
    (FILE * fp)

//  Print the body of this  DST  message to  fp

  {
   fprintf (fp, "act:%c\n", Convert_Char (action));
   fprintf (fp, "acc:%s\n", accession . c_str ());
   fprintf (fp, "mea:%.2f\n", mean);
   fprintf (fp, "std:%.2f\n", std_dev);

   return  true;
  }



bool  Celera_DST_Msg_Body_t :: read_body
    (FILE * fp)

//  Read the rest of the body of an DST message

  {
   char  line [MAX_MSG_LINE];
   int  level;

   level = 1;
   while  (level > 0 && fgets (line, MAX_MSG_LINE, fp) != NULL)
     if  (strncmp (line, "act:", 4) == 0)
          Convert_Assign (action, line [4]);
     else if  (strncmp (line, "acc:", 4) == 0)
          accession . append (Strip_Trailing (line + 4, '\n'));
     else if  (strncmp (line, "mea:", 4) == 0)
          mean = strtod (line + 4, NULL);
     else if  (strncmp (line, "std:", 4) == 0)
          std_dev = strtod (line + 4, NULL);
     else if  (line [0] == '{')
         level ++;
     else if  (line [0] == '}')
         level --;

   return  (level == 0);
  }



void  Celera_FRG_Msg_Body_t :: clear
    (void)

//  Clear all entries in this message

  {
   action = NULL_ACTION;  
   accession . erase ();
   read_type = NULL_READ;
   source . erase ();
   time_stamp . erase ();
   sequence . erase ();
   quality . erase ();
   clear_range . setRange (0, 0);

   return;
  }



bool  Celera_FRG_Msg_Body_t :: print_body
    (FILE * fp)

//  Print the body of this  FRG  message to  fp

  {
   fprintf (fp, "act:%c\n", Convert_Char (action));
   fprintf (fp, "acc:%s\n", accession . c_str ());
   fprintf (fp, "typ:%c\n", Convert_Char (read_type));
   fprintf (fp, "src:\n%s.\n", source . c_str ());
   fprintf (fp, "etm:%s\n", time_stamp . c_str ());
   fprintf (fp, "seq:\n");
   Fasta_Print (fp, sequence . c_str (), NULL);
   fprintf (fp, ".\n");
   fprintf (fp, "qlt:\n");
   Fasta_Print (fp, quality . c_str (), NULL);
   fprintf (fp, ".\n");
   fprintf (fp, "clr:%d,%d\n", int (clear_range . getBegin ()),
        int (clear_range . getEnd ()));

   return  true;
  }



bool  Celera_FRG_Msg_Body_t :: read_body
    (FILE * fp)

//  Read the rest of the body of an FRG message

  {
   char  line [MAX_MSG_LINE];
   string  s;
   int  level;

   level = 1;
   while  (level > 0 && fgets (line, MAX_MSG_LINE, fp) != NULL)
     {
      int  len;

      len = strlen (line);
      if  (len >= MAX_MSG_LINE - 1)
          throw IOException_t ("Celera message line too long");
      
      if  (strncmp (line, "act:", 4) == 0)
          Convert_Assign (action, line [4]);
      else if  (strncmp (line, "acc:", 4) == 0)
          accession . append (Strip_Trailing (line + 4, '\n'));
      else if  (strncmp (line, "typ:", 4) == 0)
          Convert_Assign (read_type, line [4]);
      else if  (strncmp (line, "src:", 4) == 0)
          {
           Read_Comment_String (fp, s);
           source . append (s);
          }
      else if  (strncmp (line, "etm:", 4) == 0)
          time_stamp . append (Strip_Trailing (line + 4, '\n'));
      else if  (strncmp (line, "seq:", 4) == 0)
          {
           Read_Sequence_String (fp, s);
           sequence . append (s);
          }
      else if  (strncmp (line, "qlt:", 4) == 0)
          {
           Read_Sequence_String (fp, s);
           quality . append (s);
          }
      else if  (strncmp (line, "clr:", 4) == 0)
          {
           uint32_t  lo, hi;

           if  (sscanf (line + 4, "%u,%u", & lo, & hi) != 2
                  || hi < lo)
               {
                cerr << line << endl;
                throw IOException_t ("Bad clear range in FRG message");
               }
           clear_range . setRange (lo, hi);
          }
      else if  (line [0] == '}')
          level --;
        else
          {
           cerr << line << endl;
           throw IOException_t ("Bad line in FRG message");
          }
     }

   return  (level == 0);
  }



void  Celera_ICM_Msg_Body_t :: clear
    (void)

//  Clear all entries in this message

  {
   content . erase ();

   return;
  }



bool  Celera_ICM_Msg_Body_t :: print_body
    (FILE * fp)

//  Print the body of this  ICM  message to  fp

  {
   // Stub.  Print nothing

   return  false;
  }



bool  Celera_ICM_Msg_Body_t :: read_body
    (FILE * fp)

//  Read the rest of the body of an ICM message

  {
   char  line [MAX_MSG_LINE];
   int  level;

   // Stub.  Just skips over message

   level = 1;
   while  (level > 0 && fgets (line, MAX_MSG_LINE, fp) != NULL)
     if  (line [0] == '{')
         level ++;
     else if  (line [0] == '}')
         level --;

   return  (level == 0);
  }



void  Celera_IUM_Msg_Body_t :: clear
    (void)

//  Clear all entries in this message

  {
   accession . erase ();
   source . erase ();
   cover_stat = 0.0;
   status = NULL_UNITIG;
   a_branch_point = b_branch_point = 0;
   uni_len = 0;
   consensus . erase ();
   quality . erase ();
   forced = false;
   num_frags = 0;
   imp_list . clear ();

   return;
  }



bool  Celera_IUM_Msg_Body_t :: print_body
    (FILE * fp)

//  Print the body of this  IUM  message to  fp

  {
   vector <Celera_IMP_Sub_Msg_t> :: iterator  p;

   fprintf (fp, "acc:%s\n", accession . c_str ());
   fprintf (fp, "src:\n%s\n.\n", source . c_str ());
   fprintf (fp, "cov:%.3f\n", cover_stat);
   fprintf (fp, "sta:%c\n", Convert_Char (status));
   fprintf (fp, "abp:%d\n", a_branch_point);
   fprintf (fp, "bbp:%d\n", b_branch_point);
   fprintf (fp, "len:%d\n", uni_len);
   fprintf (fp, "cns:\n");
   Fasta_Print (fp, consensus . c_str (), NULL);
   fprintf (fp, ".\n");
   fprintf (fp, "qlt:\n");
   Fasta_Print (fp, quality . c_str (), NULL);
   fprintf (fp, ".\n");
   fprintf (fp, "for:%c\n", forced ? '1' : '0');
   fprintf (fp, "nfr:%d\n", num_frags);

   for  (p = imp_list . begin ();  p != imp_list . end ();  p ++)
     p -> print (fp);

   return  true;
  }



bool  Celera_IUM_Msg_Body_t :: read_body
    (FILE * fp)

//  Read the rest of the body of an IUM message

  {
   char  line [MAX_MSG_LINE];
   int  ct, level, state;

   level = 1;
   ct = state = 0;
   while  (level > 0 && fgets (line, MAX_MSG_LINE, fp) != NULL)
     {
      int  len;

      len = strlen (line);
      if  (len >= MAX_MSG_LINE - 1)
          throw IOException_t ("Celera message line too long");
      if  (line [len - 1] == '\n')
          line [-- len] = '\0';
      
      if  (state > 0 && strcmp (line, ".") == 0)
          state = 0;
      else if  (state == 1)
          {
           ct ++;
           if  (ct == 1)
               source = line;
             else
               {
                source . append ("\n");
                source . append (line);
               }
          }
      else if  (state == 2)
          consensus . append (line);
      else if  (state == 3)
          quality . append (line);
      else if  (strncmp (line, "acc:", 4) == 0)
          accession . append (line + 4);
      else if  (strncmp (line, "cov:", 4) == 0)
          cover_stat = float (strtod (line + 4, NULL));
      else if  (strncmp (line, "sta:", 4) == 0)
          Convert_Assign (status, line [4]);
      else if  (strncmp (line, "abp:", 4) == 0)
          a_branch_point = int (strtol (line + 4, NULL, 10));
      else if  (strncmp (line, "bbp:", 4) == 0)
          b_branch_point = int (strtol (line + 4, NULL, 10));
      else if  (strncmp (line, "len:", 4) == 0)
          uni_len = int (strtol (line + 4, NULL, 10));
      else if  (strncmp (line, "src:", 4) == 0)
          state = 1;
      else if  (strncmp (line, "cns:", 4) == 0)
          state = 2;
      else if  (strncmp (line, "qlt:", 4) == 0)
          state = 3;
      else if  (strncmp (line, "for:", 4) == 0)
          forced = (line [4] != '0');
      else if  (strncmp (line, "nfr:", 4) == 0)
          {
           int  i;
           Celera_IMP_Sub_Msg_t  sub_message;
           bool  success;
           

           num_frags = int (strtol (line + 4, NULL, 10));
           imp_list . clear ();
           for  (i = 0;  i < num_frags;  i ++)
             {
              line [0] = '\0';
              if  (fgets (line, MAX_MSG_LINE, fp) == NULL
                     || strcmp (line, "{IMP\n") != 0)
                  {
                   cerr << line;
                   throw IOException_t ("Line should have been \"{IMP\"\n");
                  }
              success = sub_message . read_body (fp);
              if  (! success)
                  {
                   cerr << "accession = " << accession << "  i = " << i << endl;
                   throw IOException_t ("Failed to read IMP submessage");
                  }
              imp_list . push_back (sub_message);
             }
          }
      else if  (line [0] == '}')
          level --;
        else
          {
           cerr << line << endl;
           throw IOException_t ("Bad line in IUM message");
          }
     }

   return  (level == 0);
  }



void  Celera_IUM_Msg_Body_t :: Remove_Empty_IMPs
  (void)

// Remove all  IMP  submessages with positions that are both zero from
// this message.

{
  vector <Celera_IMP_Sub_Msg_t>  new_list;
  Celera_IMP_Sub_Msg_t  m;
  int  remove_ct = 0;
  int  i;

  for (i = 0; i < num_frags; i ++)
    if (imp_list [i] . Is_Empty ())
      remove_ct ++;

  if (remove_ct == 0)
    return;   // nothing to remove

  for (i = 0; i < num_frags; i ++)
    if (! imp_list [i] . Is_Empty ())
      new_list . push_back (imp_list [i]);
  
  imp_list = new_list;
  num_frags -= remove_ct;

  return;
}



void  Celera_IUM_Msg_Body_t :: setIMPs
    (const vector <int> & f, const vector <Ordered_Range_t> & p)

//  Make the  IMP  submessages of this message be the reads with IDs
//  in  f  and consensus positions in  p .

  {
   Celera_IMP_Sub_Msg_t  m;
   int  i, n;

   m . setReadType (READ_READ);
   m . setContained (0);
   m . setSource ("");

   imp_list . clear ();

   n = f . size ();
   for  (i = 0;  i < n;  i ++)
     {
      m . setID (f [i]);
      m . setPosition (p [i]);
      imp_list . push_back (m);
     }
   num_frags = n;

   return;
  }



void  Celera_IUM_Msg_Body_t :: Update_IMPs
    (const vector <Range_t> & pos, const vector <int> & ref,
     const vector < vector <int> > & del_list)

//  Adjust the IMP position entries in this message body
//  to the values in  pos .   ref [i]  is the subscript of the IMP
//  entry that corresponds to  pos [i] .  Only applies to IUM and
//  ICM messages.

  {
   int  i, n;

   n = pos . size ();
   if  (n != int (ref . size ())
          || n != int (imp_list . size ())
          || n != int (del_list . size ()))
       {
        sprintf (Clean_Exit_Msg_Line,
             "Bad sizes in Update_IMPs  n = %d\n"
             "   refsize = %d  listsize = %d  delsize = %d",
             n, int (ref . size ()), int (imp_list . size ()),
             int (del_list . size ()));
        throw IOException_t (Clean_Exit_Msg_Line);
       }

   for  (i = 0;  i < n;  i ++)
     {
      int  a, b, j;

      j = ref [i];
      a = imp_list [j] . getPosition () . getBegin ();
      b = imp_list [j] . getPosition () . getEnd ();
      if  (a <= b)
          imp_list [j] . setPosition (pos [i] . getBegin (), pos [i] . getEnd ());
        else
          imp_list [j] . setPosition (pos [i] . getEnd (), pos [i] . getBegin ());
      imp_list [j] . setDel (del_list [i]);
     }

   return;
  }



void  Celera_LKG_Msg_Body_t :: clear
    (void)

//  Clear all entries in this message

  {
   action = NULL_ACTION;
   link_type = NULL_LINK;
   accession1 . erase ();
   accession2 . erase ();
   time_stamp . erase ();
   dst_ref . erase ();
   ori = NULL_ORIENT;

   return;
  }



bool  Celera_LKG_Msg_Body_t :: print_body
    (FILE * fp)

//  Print the body of this  LKG  message to  fp

  {
   fprintf (fp, "act:%c\n", Convert_Char (action));
   fprintf (fp, "typ:%c\n", Convert_Char (link_type));
   fprintf (fp, "fg1:%s\n", accession1 . c_str ());
   fprintf (fp, "fg2:%s\n", accession2 . c_str ());
   fprintf (fp, "etm:%s\n", time_stamp . c_str ());
   fprintf (fp, "dst:%s\n", dst_ref . c_str ());
   fprintf (fp, "ori:%c\n", Convert_Char (ori));

   return  false;
  }



bool  Celera_LKG_Msg_Body_t :: read_body
    (FILE * fp)

//  Read the rest of the body of an LKG message

  {
   char  line [MAX_MSG_LINE];
   int  level;

   // Stub.  Just skips over message

   level = 1;
   while  (level > 0 && fgets (line, MAX_MSG_LINE, fp) != NULL)
     if  (strncmp (line, "act:", 4) == 0)
         Convert_Assign (action, line [4]);
     else if  (strncmp (line, "typ:", 4) == 0)
         Convert_Assign (link_type, line [4]);
     else if  (strncmp (line, "fg1:", 4) == 0)
         accession1 . append (Strip_Trailing (line + 4, '\n'));
     else if  (strncmp (line, "fg2:", 4) == 0)
         accession2 . append (Strip_Trailing (line + 4, '\n'));
     else if  (strncmp (line, "etm:", 4) == 0)
         time_stamp . append (Strip_Trailing (line + 4, '\n'));
     else if  (strncmp (line, "dst:", 4) == 0)
         dst_ref . append (Strip_Trailing (line + 4, '\n'));
     else if  (strncmp (line, "ori:", 4) == 0)
         Convert_Assign (ori, line [4]);
     else if  (line [0] == '{')
         level ++;
     else if  (line [0] == '}')
         level --;

   return  (level == 0);
  }



const string &  Celera_Message_t :: getAccession
   (void)  const

//  Return the  accession  field of this message if it's valid

  {
   switch  (msg_type)
     {
      case  CCO_MSG :
      case  FRG_MSG :
      case  IUM_MSG :
        return  body -> getAccession ();
      default :
        throw IOException_t ("Message type does not have accession field");
     }
  }



const Range_t &  Celera_Message_t :: getClearRange
   (void)  const

//  Return the  range  field of this message if it's valid

  {
   switch  (msg_type)
     {
      case  FRG_MSG :
        return  body -> getClearRange ();
      default :
        throw IOException_t ("Message type does not have range field");
     }
  }



const string &  Celera_Message_t :: getConsensus
    (void)  const

//  Return the  consensus  field of this message if it's valid

  {
   switch  (msg_type)
     {
      case  CCO_MSG :
      case  IUM_MSG :
        return  body -> getConsensus ();
      default :
        throw IOException_t ("Message type does not have quality field");
     }
  }



vector <Celera_IMP_Sub_Msg_t> &  Celera_Message_t :: getIMPList
    (void)  const

//  Return the  imp_list  field of this message if it's valid

  {
   switch  (msg_type)
     {
      case  IUM_MSG :
        return  body -> getIMPList ();
      default :
        throw IOException_t ("Message type does not have IMP-list field");
     }
  }


    
const vector <Celera_MPS_Sub_Msg_t> &  Celera_Message_t :: getMPSList
    (void)  const

//  Return the  mps_list  field of this message if it's valid

  {
   switch  (msg_type)
     {
      case  CCO_MSG :
        return  body -> getMPSList ();
      default :
        throw IOException_t ("Message type does not have MPS-list field");
     }
  }


    
int  Celera_Message_t :: getNumFrags
    (void)  const

//  Return the  num_frags  field of this message if it's valid

  {
   switch  (msg_type)
     {
      case  CCO_MSG :
      case  IUM_MSG :
        return  body -> getNumFrags ();
      default :
        throw IOException_t ("Message type does not have num-frags field");
     }
  }



int  Celera_Message_t :: getNumUnitigs
    (void)  const

//  Return the  num_unitigs  field of this message if it's valid

  {
   switch  (msg_type)
     {
      case  CCO_MSG :
        return  body -> getNumUnitigs ();
      default :
        throw IOException_t ("Message type does not have num-unitigs field");
     }
  }



const string &  Celera_Message_t :: getQuality
    (void)  const

//  Return the  quality  field of this message if it's valid

  {
   switch  (msg_type)
     {
      case  CCO_MSG :
      case  FRG_MSG :
        return  body -> getQuality ();
      default :
        throw IOException_t ("Message type does not have quality field");
     }
  }



const string &  Celera_Message_t :: getSequence
    (void)  const

//  Return the  sequence  field of this message if it's valid

  {
   switch  (msg_type)
     {
      case  FRG_MSG :
        return  body -> getSequence ();
      default :
        throw IOException_t ("Message type does not have sequence field");
     }
  }



const string &  Celera_Message_t :: getSource
    (void)  const

//  Return the  source  field of this message if it's valid

  {
   switch  (msg_type)
     {
      case  FRG_MSG :
        return  body -> getSource ();
      default :
        throw IOException_t ("Message type does not have source field");
     }
  }



const vector <Celera_UPS_Sub_Msg_t> &  Celera_Message_t :: getUPSList
    (void)  const

//  Return the  ups_list  field of this message if it's valid

  {
   switch  (msg_type)
     {
      case  CCO_MSG :
        return  body -> getUPSList ();
      default :
        throw IOException_t ("Message type does not have MPS-list field");
     }
  }


    
void  Celera_Message_t :: setAccession
    (const char * s)

//  Set the  accession  field of this message to  s  if it's valid

  {
   body -> setAccession (s);
  }



void  Celera_Message_t :: setAccession
    (const string & s)

//  Set the  accession  field of this message to  s  if it's valid

  {
   body -> setAccession (s);
  }



void  Celera_Message_t :: setClearRange
    (Pos_t a, Pos_t b)

//  Set the  clear_range  field of this message to  s  if it's valid

  {
   body -> setClearRange (a, b);
  }


void  Celera_Message_t :: setIMPs
    (const vector <int> & f, const vector <Ordered_Range_t> & p)

//  Set the  IMP  submessages of this message to the read IDs in  f
//  and the positions in   p .

  {
   body -> setIMPs (f, p);
  }



void  Celera_Message_t :: setQuality
    (const char * q)

//  Set the  quality  field of this message to  q  if it's valid

  {
   body -> setQuality (q);
  }



void  Celera_Message_t :: setQuality
    (const string & q)

//  Set the  quality  field of this message to  q  if it's valid

  {
   body -> setQuality (q);
  }



void  Celera_Message_t :: setSequence
    (const char * s)

//  Set the  sequence  field of this message to  s  if it's valid

  {
   body -> setSequence (s);
  }



void  Celera_Message_t :: setSequence
    (const string & s)

//  Set the  sequence  field of this message to  s  if it's valid

  {
   body -> setSequence (s);
  }



void  Celera_Message_t :: setStatus
    (Celera_Unitig_Status_t s)

//  Set the  status  field of this message to  s  if it's valid

  {
   body -> setStatus (s);
  }



void  Celera_Message_t :: setType
    (Celera_Message_Type_t t)

//  Set the type of this message to  t

  {
   if  (msg_type == t)
       {
        body -> clear ();
        return;
       }

   if  (msg_type != NULL_MSG)
       delete  body;

   msg_type = t;
   switch  (t)
     {
      case  ADT_MSG :
        body = new Celera_ADT_Msg_Body_t;
        break;
      case  BAT_MSG :
        body = new Celera_BAT_Msg_Body_t;
        break;
      case  DST_MSG :
        body = new Celera_DST_Msg_Body_t;
        break;
      case  FRG_MSG :
        body = new Celera_FRG_Msg_Body_t;
        break;
      case  ICM_MSG :
        body = new Celera_ICM_Msg_Body_t;
        break;
      case  IUM_MSG :
        body = new Celera_IUM_Msg_Body_t;
        break;
      case  LKG_MSG :
        body = new Celera_LKG_Msg_Body_t;
        break;
      default :
        sprintf (Clean_Exit_Msg_Line,
             "Celera message setType for type %s not supported",
             Convert_String (t));
        Clean_Exit (Clean_Exit_Msg_Line, __FILE__, __LINE__);
     }

   body -> clear ();
  }



void  Celera_Message_t :: setUniLen
   (int n)

//  Set the  uni_len  field of this message to  n  if it's valid

  {
   body -> setUniLen (n);
  }



void  Celera_Message_t :: print
    (FILE * fp, const char * seq, const char * qv)

//  Print this message to  fp .  If  seq  and  qv  are not
//   NULL , print them as the sequence and quality if this is
//  an  IUM  message.

  {
   fprintf (fp, "{%s\n", Convert_String (msg_type));

   body -> print_body (fp);    // add next

   fprintf (fp, "}\n");

   return;
  }



bool  Celera_Message_t :: read
    (FILE * fp)

//  Read the next message from  fp  and return a pointer to it.
//  Return  true  if successful;  false , otherwise.

  {
   char  line [MAX_MSG_LINE];
   int  len, line_ct = 0;

   while  (fgets (line, MAX_MSG_LINE, fp) != NULL)
     {
      line_ct ++;
      if  (line [0] == '{')
          break;
     }

   if  (line_ct == 0 || line [0] != '{')
       return  false;

   len = strlen (line);
   if  (len >= MAX_MSG_LINE - 1)
       throw IOException_t ("Celera message line too long");

   clear ();

   line [4] = '\0';
   if  (strcmp (line + 1, "ADT") == 0)
       {
        if  (msg_type == ADT_MSG)
            body -> clear ();
          else
            {
             clear ();
             msg_type = ADT_MSG;
             body = new Celera_ADT_Msg_Body_t;
            }
       }
   else if  (strcmp (line + 1, "BAT") == 0)
       {
        if  (msg_type == BAT_MSG)
            body -> clear ();
          else
            {
             clear ();
             msg_type = BAT_MSG;
             body = new Celera_BAT_Msg_Body_t;
            }
       }
   else if  (strcmp (line + 1, "CCO") == 0)
       {
        if  (msg_type == CCO_MSG)
            body -> clear ();
          else
            {
             clear ();
             msg_type = CCO_MSG;
             body = new Celera_CCO_Msg_Body_t;
            }
       }
   else if  (strcmp (line + 1, "DST") == 0)
       {
        if  (msg_type == DST_MSG)
            body -> clear ();
          else
            {
             clear ();
             msg_type = DST_MSG;
             body = new Celera_DST_Msg_Body_t;
            }
       }
   else if  (strcmp (line + 1, "FRG") == 0)
       {
        if  (msg_type == FRG_MSG)
            body -> clear ();
          else
            {
             clear ();
             msg_type = FRG_MSG;
             body = new Celera_FRG_Msg_Body_t;
            }
       }
   else if  (strcmp (line + 1, "ICM") == 0)
       {
        if  (msg_type == ICM_MSG)
            body -> clear ();
          else
            {
             clear ();
             msg_type = ICM_MSG;
             body = new Celera_ICM_Msg_Body_t;
            }
       }
   else if  (strcmp (line + 1, "IUM") == 0)
       {
        if  (msg_type == IUM_MSG)
            body -> clear ();
          else
            {
             clear ();
             msg_type = IUM_MSG;
             body = new Celera_IUM_Msg_Body_t;
            }
       }
   else if  (strcmp (line + 1, "LKG") == 0)
       {
        if  (msg_type == LKG_MSG)
            body -> clear ();
          else
            {
             clear ();
             msg_type = LKG_MSG;
             body = new Celera_LKG_Msg_Body_t;
            }
       }
     else
       {
        strcat (line, " <== Unrecognized message type");
        throw IOException_t (line);
       }

   return  body -> read_body (fp);
  }



void  Celera_Message_t :: Remove_Empty_IMPs
  (void)

//  Remove IMP entries with both values equal to zero in the body
//  of this message.  Only applies to IUM and ICM messages.

  {
   switch  (msg_type)
     {
      case  IUM_MSG :
      case  ICM_MSG :
        body -> Remove_Empty_IMPs ();
        return;
      default :
        sprintf (Clean_Exit_Msg_Line, "Attempt to Update_IMPs for %s message",
             Convert_String (msg_type));
        throw IOException_t (Clean_Exit_Msg_Line);
     }
  }



void  Celera_Message_t :: Update_IMPs
    (const vector <Range_t> & pos, const vector <int> & ref,
     const vector < vector <int> > & del_list)

//  Adjust the IMP position entries in the body of this message
//  to the values in  pos .   ref [i]  is the subscript of the IMP
//  entry that corresponds to  pos [i] .  Only applies to IUM and
//  ICM messages.

  {
   switch  (msg_type)
     {
      case  IUM_MSG :
      case  ICM_MSG :
        body -> Update_IMPs (pos, ref, del_list);
        return;
      default :
        sprintf (Clean_Exit_Msg_Line, "Attempt to Update_IMPs for %s message",
             Convert_String (msg_type));
        throw IOException_t (Clean_Exit_Msg_Line);
     }
  }



const char *  Convert_String
    (Celera_Message_Type_t msg_type)

//  Return a printable version of  msg_type

  {
   switch  (msg_type)
     {
      case  NULL_MSG :
        return  "NULL";
      case  ADT_MSG :
        return  "ADT";
      case  BAT_MSG :
        return  "BAT";
      case  CCO_MSG :
        return  "CCO";
      case  DST_MSG :
        return  "DST";
      case  FRG_MSG :
        return  "FRG";
      case  ICM_MSG :
        return  "ICM";
      case  IUM_MSG :
        return  "IUM";
      case  LKG_MSG :
        return  "LKG";
      default :
        throw IOException_t ("Invalid message type");
     }
  }


void  Convert_Assign
    (Celera_Msg_Action_t & action, char ch)

//  Set  action  to the message action encoded by character  ch .

  {
   switch  (ch)
     {
      case  'A' :
        action = ADD_ACTION;
        break;
      case  'D' :
        action = DELETE_ACTION;
        break;
      default :
        sprintf (Clean_Exit_Msg_Line, "Illegal action code = %c", ch);
        throw IOException_t (Clean_Exit_Msg_Line);
     }

   return;
  }


char  Convert_Char
    (Celera_Msg_Action_t t)

//  Return a printable version of  t

  {
   switch  (t)
     {
      case  ADD_ACTION :
        return  'A';
      case  DELETE_ACTION :
        return  'D';
      default :
        throw IOException_t ("Invalid action type");
     }
  }



void  Convert_Assign
    (Celera_Msg_Read_Type_t & read_type , char ch)

//  Set  read_type  to the read type encoded by character  ch .

  {
   switch  (ch)
     {
      case  'R' :
        read_type = READ_READ;
        break;
      case  'B' :
        read_type = BACTIG_READ;
        break;
      default :
        sprintf (Clean_Exit_Msg_Line, "Illegal read type code = %c", ch);
        throw IOException_t (Clean_Exit_Msg_Line);
     }

   return;
  }


char  Convert_Char
    (Celera_Msg_Read_Type_t t)

//  Return a printable version of  t

  {
   switch  (t)
     {
      case  READ_READ :
        return  'R';
      case  BACTIG_READ :
        return  'B';
      default :
        throw IOException_t ("Invalid read type");
     }
  }



void  Convert_Assign
    (Celera_Msg_Link_Type_t & link_type , char ch)

//  Set  link_type  to the read type encoded by character  ch .

  {
   switch  (ch)
     {
      case  'M' :
        link_type = MATE_LINK;
        break;
      case  'B' :
        link_type = BAC_GUIDE;
        break;
      case  'S' :
        link_type = STS_GUIDE;
        break;
      case  'R' :
        link_type = REREAD_LINK;
        break;
      case  'Y' :
        link_type = MAY_JOIN;
        break;
      case  'T' :
        link_type = MUST_JOIN;
        break;
      default :
        sprintf (Clean_Exit_Msg_Line, "Illegal link type code = %c", ch);
        throw IOException_t (Clean_Exit_Msg_Line);
     }

   return;
  }


char  Convert_Char
    (Celera_Msg_Link_Type_t t)

//  Return a printable version of  t

  {
   switch  (t)
     {
      case  MATE_LINK :
        return  'M';
      case  BAC_GUIDE :
        return  'B';
      case  STS_GUIDE :
        return  'S';
      case  REREAD_LINK :
        return  'R';
      case  MAY_JOIN :
        return  'Y';
      case  MUST_JOIN :
        return  'T';
      default :
        throw IOException_t ("Invalid link type");
     }
  }



void  Convert_Assign
    (Celera_Msg_Orientation_Type_t & ori_type , char ch)

//  Set  ori_type  to the read type encoded by character  ch .

  {
   switch  (ch)
     {
      case  'N' :
        ori_type = NORMAL_ORIENT;
        break;
      case  'I' :
        ori_type = INNIE_ORIENT;
        break;
      case  'O' :
        ori_type = OUTTIE_ORIENT;
        break;
      case  'A' :
        ori_type = ANTI_ORIENT;
        break;
      default :
        sprintf (Clean_Exit_Msg_Line, "Illegal orientation type code = %c", ch);
        throw IOException_t (Clean_Exit_Msg_Line);
     }

   return;
  }


char  Convert_Char
    (Celera_Msg_Orientation_Type_t t)

//  Return a printable version of  t

  {
   switch  (t)
     {
      case  NORMAL_ORIENT :
        return  'N';
      case  INNIE_ORIENT :
        return  'I';
      case  OUTTIE_ORIENT :
        return  'O';
      case  ANTI_ORIENT :
        return  'A';
      default :
        throw IOException_t ("Invalid orientation type");
     }
  }



void  Convert_Assign
    (Celera_Placement_Status_t & p, char ch)

//  Set  p  to the placement status encoded by character  ch .

  {
   switch  (ch)
     {
      case  'P' :
        p = PLACED;
        break;
      case  'U' :
        p = UNPLACED;
        break;
      default :
        sprintf (Clean_Exit_Msg_Line, "Illegal placement status code = %c", ch);
        throw IOException_t (Clean_Exit_Msg_Line);
     }

   return;
  }


char  Convert_Char
    (Celera_Placement_Status_t p)

//  Return a printable version of  p

  {
   switch  (p)
     {
      case  PLACED :
        return  'P';
      case  UNPLACED :
        return  'U';
      default :
        throw IOException_t ("Invalid placement status type");
     }
  }



void  Convert_Assign
    (Celera_Unitig_Status_t & us , char ch)

//  Set  us  to the unitig status encoded by character  ch .

  {
   switch  (ch)
     {
      case  'C' :
        us = CHIMERIC_UNITIG;
        break;
      case  'N' :
        us = NOT_REZ_UNITIG;
        break;
      case  'S' :
        us = SEP_UNITIG;
        break;
      case  'U' :
        us = UNIQUE_UNITIG;
        break;
      case  'X' :
        us = UNASSIGNED_UNITIG;
        break;
      default :
        sprintf (Clean_Exit_Msg_Line, "Illegal unitig status code = %c", ch);
        throw IOException_t (Clean_Exit_Msg_Line);
     }

   return;
  }


char  Convert_Char
    (Celera_Unitig_Status_t s)

//  Return a printable version of  s

  {
   switch  (s)
     {
      case  UNIQUE_UNITIG :
        return  'U';
      case  CHIMERIC_UNITIG :
        return  'C';
      case  NOT_REZ_UNITIG :
        return  'N';
      case  SEP_UNITIG :
        return  'S';
      case  UNASSIGNED_UNITIG :
        return  'X';
      default :
        throw IOException_t ("Invalid unitig status type");
     }
  }



void  Convert_Assign
    (Celera_Unitig_Type_t & t , char ch)

//  Set  t  to the unitig type encoded by character  ch .

  {
   switch  (ch)
     {
      case  'U' :
        t = UNIQUE_UNI_T;
        break;
      case  'R' :
        t = ROCK_UNI_T;
        break;
      case  'S' :
        t = STONE_UNI_T;
        break;
      case  'P' :
        t = PEBBLE_UNI_T;
        break;
      case  's' :
        t = SINGLE_UNI_T;
        break;
      case  'X' :
        t = OTHER_UNI_T;
        break;
      default :
        sprintf (Clean_Exit_Msg_Line, "Illegal unitig type code = %c", ch);
        throw IOException_t (Clean_Exit_Msg_Line);
     }

   return;
  }


char  Convert_Char
    (Celera_Unitig_Type_t t)

//  Return a printable version of  t

  {
   switch  (t)
     {
      case  UNIQUE_UNI_T :
        return  'U';
      case  ROCK_UNI_T :
        return  'R';
      case  STONE_UNI_T :
        return  'S';
      case  PEBBLE_UNI_T :
        return  'P';
      case  SINGLE_UNI_T :
        return  's';
      case  OTHER_UNI_T :
        return  'X';
      default :
        throw IOException_t ("Invalid unitig type type");
     }
  }



void  Read_Comment_String
    (FILE * fp, string & s)

//  Read lines from  fp  until a line containing just a
//  period at the beginning is encountered.  Append the
//  lines (including the newline characters but not the period-line)
//  and put them into  s .

  {
   char  line [MAX_MSG_LINE];

   s . erase ();

   while  (fgets (line, MAX_MSG_LINE, fp) != NULL)
     {
      if  (strcmp (line, ".\n") == 0)
          return;

      s . append (line);
     }

   throw IOException_t ("Unexpected file error reading comment");

   return;
  }



void  Read_Sequence_String
    (FILE * fp, string & s)

//  Read lines from  fp  until a line containing just a
//  period at the beginning is encountered.  Append the
//  lines (without the period-line and discarding newlines)
//  and put them into  s .

  {
   char  line [MAX_MSG_LINE];

   s . erase ();

   while  (fgets (line, MAX_MSG_LINE, fp) != NULL)
     {
      if  (strcmp (line, ".\n") == 0)
          return;

      s . append (Strip_Trailing (line, '\n'));
     }

   throw IOException_t ("Unexpected file error reading comment");

   return;
  }



