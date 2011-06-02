//  A. L. Delcher
//
//  File:  make-consensus.cc
//
//  Last Modified:  19 February 2008
//
//  Read layout information for reads in unitigs/contigs and create
//  multialignments and/or consensus sequences for them.


#include  "foundation_AMOS.hh"
#include  "delcher.hh"
#include  "CelMsg.hh"
#include  "align.hh"
#include  "prob.hh"
#include  "fasta.hh"
#include  <iostream>
#include  <vector>
#include  <string>
#include  <algorithm>
#include  <fstream>


using namespace std;
using namespace AMOS;

const int  DEFAULT_ALIGN_WIGGLE = 15;
   // Number of positions left and right of specified location
   // to look for alignments.
const double DEFAULT_ERROR_RATE = 0.06;
const int DEFAULT_MIN_OVERLAP = 5;
const int MAX_LINE = 1000;
const int NEW_SIZE = 1000;


enum Input_Format_t
{ PARTIAL_READ_INPUT, SIMPLE_CONTIG_INPUT, CELERA_MSG_INPUT, BANK_INPUT };
enum Output_Format_t
{ MULTI_ALIGN_OUTPUT, ONLY_FASTA_OUTPUT, CELERA_MSG_OUTPUT,
  TIGR_CONTIG_OUTPUT, AMOS_OUTPUT, BANK_OUTPUT
};


static int Align_Wiggle = DEFAULT_ALIGN_WIGGLE;
  // Change made by Daniela Puiu 01/03/2008
  //   new input parameter -w to specify the alignment wiggle
  //   default value 15; use a smaller value for Solexa data
static bool  Allow_Expels = false;
  // If set true (by -x option) then allow reads with poor alignments to
  // be expelled from multialignments
static string Bank_Name;
  // Name of read bank from which reads are obtained
static bool Use_SeqNames = false;
  //AMP// If set true, will use comment filed in the readbank for seq tags
static bool Do_Contig_Messages = false;
  // If set true (by -c option) then contig messages in the
  // input will be processed
static bool Do_Unitig_Messages = false;
  // If set true (by -u option) then unitig messages in the
  // input will be processed
static double Error_Rate = DEFAULT_ERROR_RATE;
  // Fraction of errors allowed in alignments
static FILE  * Expel_fp = NULL;
  // Pointer to file where expelled reads are output
static FILE *Extra_fp = NULL;
  // Pointer to file of extra fasta sequences to align
static Input_Format_t Input_Format = SIMPLE_CONTIG_INPUT;
  // Type of input
static int Min_Overlap = DEFAULT_MIN_OVERLAP;
  // Minimum number of bases strings in multialignment must overlap
static Output_Format_t Output_Format = CELERA_MSG_OUTPUT;
  // Type of output to produce
static string Tig_File_Name;
  // Name of file containing input contig/unitig messages
static bool byIID = false;
  // Layouts to be processed listed by IID
static ifstream IID_fp;
  // Pointer to file of IIDs
static bool byEID = false;
  // Layouts to be processed listed by EID
static ifstream EID_fp;
  // Pointer to file of EIDs


static bool USE_LayoutClear = false;      // TODO: Fix AMOScmp, then this will be true


static bool By_Lo_Position
  (const Celera_IMP_Sub_Msg_t & a, const Celera_IMP_Sub_Msg_t & b);
static void Get_Strings_And_Offsets
  (vector < char *>&s, vector < char *>&q, vector < Range_t > &clr_list,
   vector < char *>&tag_list, vector < int >&offset,
   const Celera_Message_t & msg, Bank_t & read_bank);
static void Get_Strings_And_Offsets
  (vector < char *>&s, vector < char *>&q, vector < Range_t > &clr_list,
   vector < char *>&tag_list, vector < int >&offset, vector < int >&fid,
   vector < Ordered_Range_t > &pos, vector < Ordered_Range_t > &seg,
   Bank_t & read_bank);
static void Get_Strings_And_Offsets
  (vector < char *>&s, vector < char *>&q, vector < Range_t > &clr_list,
   vector < char *>&tag_list, vector < int >&offset, Layout_t & layout,
   vector < int >&fid, vector < Ordered_Range_t > &pos, Bank_t & read_bank);
static void Output_Unit
  (const string & label, const string & id, int num_reads,
   Gapped_Multi_Alignment_t & gma, Celera_Message_t & msg,
   const vector < char *>&string_list,
   const vector < char *>&qual_list, const vector < Range_t > &clr_list,
   const vector < char *>tag_list, BankStream_t & bank);
static void Parse_Command_Line (int argc, char *argv[]);
static void Sort_By_Low_Pos
  (vector < int >&fid, vector < Ordered_Range_t > &pos,
   vector < Ordered_Range_t > &seg);
static void Usage (const char *command);
static void readIIDFile (ifstream & f, list < ID_t > &l);
static void readEIDFile (ifstream & f, list < string > &l);



int main (int argc, char *argv[])
{
  Bank_t read_bank (Read_t::NCODE);
  BankStream_t contig_bank (Contig_t::NCODE);
  BankStream_t layout_bank (Layout_t::NCODE);
  Celera_Message_t msg;
  Read_t read;
  FILE *input_fp;
  string label = "Contig";
  string cid;
  vector < char *>string_list, qual_list;
  vector < char *>tag_list;
  vector < int >offset;
  vector < int >ref;
  vector < Range_t > pos, clr_list;
  vector < vector < int > >del_list;
  Gapped_Multi_Alignment_t gma;
  time_t now;
  iostream::fmtflags status;
  int contig_ct, unitig_ct;
  list < string > eid_list;    // if layouts selected by eid
  list < ID_t > iid_list;      // if layouts selected by iid

  try
  {
    now = time (NULL);
    cerr << "Starting on " << ctime (&now) << endl;

    Verbose = 0;

    Parse_Command_Line (argc, argv);

    cerr << "Read bank is " << Bank_Name << endl;
    status = cerr.setf (ios::fixed);
    cerr << "Alignment error rate is " << setprecision (2)
      << Error_Rate << endl;
    cerr.setf (status);
    cerr << "Minimum overlap bases is " << Min_Overlap << endl;

    if (Output_Format == BANK_OUTPUT)
    {
      cerr << "Output will be written to the bank" << endl;
      if (!contig_bank.exists (Bank_Name))
        contig_bank.create (Bank_Name);
      else
        contig_bank.open (Bank_Name);
    }

    if (byIID)
    {
      cerr << "Will only process IIDs in file specified by option -i\n";

      readIIDFile (IID_fp, iid_list);
      cerr << "List size: " << iid_list.size() << endl;

      IID_fp.close ();
    }

    if (byEID)
    {
      cerr << "Will only process EIDs in file specified by option -n\n";
      readEIDFile (EID_fp, eid_list);
      EID_fp.close ();
    }

    gma.setPrintFlag (PRINT_WITH_DIFFS);

    if (Input_Format == CELERA_MSG_INPUT)
    {
      cerr << "Processing ";
      if (Do_Contig_Messages)
        cerr << "contig";
      else
      {
        cerr << "unitig";
        label = "Unitig";
      }
      cerr << " messages from file " << Tig_File_Name << endl;

      input_fp = File_Open (Tig_File_Name.c_str (), "r");
      read_bank.open (Bank_Name, B_READ);

      unitig_ct = contig_ct = 0;
      while (msg.read (input_fp))
      {
        if (msg.getMsgType () == IUM_MSG && Do_Unitig_Messages)
        {
          cerr << "Process unitig " << msg.getAccession () << endl;
          Get_Strings_And_Offsets
            (string_list, qual_list, clr_list, tag_list, offset,
            msg, read_bank);

          try
          {
            cid = msg.getAccession ();
            Multi_Align
              (cid, string_list, offset, Align_Wiggle, Error_Rate,
               Min_Overlap, gma, &ref, &tag_list, Allow_Expels);
          }
          catch (AlignmentException_t)
          {
            cerr << "Failed on unitig " << msg.getAccession () << endl;
            throw;
          }

          Permute (qual_list, ref);
          Permute (clr_list, ref);

          gma.Set_Flipped (clr_list);
          gma.Get_Positions (pos);
          gma.Extract_IMP_Dels (del_list);
          msg.Update_IMPs (pos, ref, del_list);
          if (Allow_Expels)
            msg . Remove_Empty_IMPs ();

          Output_Unit (label, msg.getAccession (),
            msg.getNumFrags (), gma, msg, string_list,
            qual_list, clr_list, tag_list, contig_bank);

            unitig_ct++;
        }
        if (msg.getMsgType () == ICM_MSG && Do_Contig_Messages)
        {
          contig_ct++;
        }
      }
      if (Do_Unitig_Messages)
        cerr << unitig_ct << " IUM messages processed" << endl;
      if (Do_Contig_Messages)
        cerr << contig_ct << " ICM messages processed" << endl;
    } // end of celera input
    else if (Input_Format == BANK_INPUT)
    {
      ID_t layout_id = 0; // we'll have to number the contigs ourselves
      Layout_t layout;
      vector < Ordered_Range_t > pos_list;
      vector < int >frg_id_list;
      list < string >::iterator eidi = eid_list.begin ();
      list < ID_t >::iterator iidi = iid_list.begin ();

      cerr << "Input is being read from the bank " << endl;

      read_bank.open (Bank_Name, B_READ);

      layout_bank.open (Bank_Name);

      msg.setType (IUM_MSG);
      msg.setStatus (UNASSIGNED_UNITIG);

      while (true)
      {
        char sid[256];

        if (byIID)
        {
          if (iidi == iid_list.end ())
          {
            break;
          }

          if (!layout_bank.existsIID (*iidi))
          {
            cerr << "IID " << *iidi << " does not exist *** !\n";
            exit (1);
          }
          layout_bank.fetch (*iidi, layout);
          iidi++;
        }
        else if (byEID)
        {
          if (eidi == eid_list.end ())
            break;
          if (!layout_bank.existsEID (*eidi))
          {
            cerr << "EID " << *eidi << " does not exist!\n";
            exit (1);
          }
          layout_bank.fetch (*eidi, layout);
          eidi++;
        }
        else
        {
          layout_bank >> layout;
          if (layout_bank.eof ())
            break;
        }

        sprintf (sid, "%ld", ++layout_id);
        cid = string (sid);
        ID_t lid = layout.getIID ();
        if (lid == 0)
        {
          lid = layout_id;
        }

        if (Verbose >= 2)
          cerr << "Processing layout: " << cid << endl;

        Get_Strings_And_Offsets
          (string_list, qual_list, clr_list, tag_list, offset,
           layout, frg_id_list, pos_list, read_bank);

        msg.setAccession (cid);
        msg.setIMPs (frg_id_list, pos_list);

        try
        {
          Multi_Align
            (cid, string_list, offset, Align_Wiggle, Error_Rate,
             Min_Overlap, gma, &ref, &tag_list, Allow_Expels);
        }
        catch (...)
        {
          cerr << "Failed on " << lid << "\'th layout/contig" << endl;
          throw;
        }

        Permute (qual_list, ref);
        Permute (clr_list, ref);
        Permute (frg_id_list, ref);

        gma.Set_Flipped (clr_list);
        gma.Get_Positions (pos);
        gma.Extract_IMP_Dels (del_list);
        msg.Update_IMPs (pos, ref, del_list);
        if (Allow_Expels)
          msg . Remove_Empty_IMPs ();

        gma.Set_Consensus_And_Qual (string_list, qual_list);
        msg.setSequence (gma.getConsensusString ());
        msg.setQuality (gma.getQualityString ());
        msg.setUniLen (strlen (gma.getConsensusString ()));

        Output_Unit (label, layout.getEID (),
          msg.getNumFrags (), gma, msg, string_list,
          qual_list, clr_list, tag_list, contig_bank);

        // Cleanup before next layout to make valgrind happy on last layout
        // Otherwise happens in Get_Strings_And_Offsets
        {
          int i, n;
          n = string_list.size ();
          for (i = 0; i < n; i++)
          free (string_list[i]);
          string_list.clear ();

          n = qual_list.size ();
          for (i = 0; i < n; i++)
            free (qual_list[i]);
          qual_list.clear ();

          n = tag_list.size ();
          for (i = 0; i < n; i++)
            free (tag_list[i]);
          tag_list.clear ();
        }

        contig_ct++;
      }                  // while layout
      cerr << "Processed " << layout_id << " layouts" << endl;
    } // end of amos bank processing
    else if (Input_Format == SIMPLE_CONTIG_INPUT
          || Input_Format == PARTIAL_READ_INPUT)
    {
      char line[MAX_LINE];
      vector < Ordered_Range_t > pos_list, seg_list;
      vector < int >frg_id_list;
      int fid;

      input_fp = File_Open (Tig_File_Name.c_str (), "r");
      read_bank.open (Bank_Name, B_READ);

      msg.setType (IUM_MSG);
      msg.setStatus (UNASSIGNED_UNITIG);

      contig_ct = 0;

      while (fgets (line, MAX_LINE, input_fp) != NULL)
      {
        char *p;

        p = strtok (line, " \t\n");
        if (p == NULL)
          continue;

        if (strcmp (p, "C") == 0) // line starts with 'C'
        {
          if (frg_id_list.size () > 0)
          {
            Get_Strings_And_Offsets (string_list, qual_list, clr_list,
                                     tag_list, offset, frg_id_list,
                                     pos_list, seg_list, read_bank);

            msg.setAccession (cid);
            msg.setIMPs (frg_id_list, pos_list);
            try
            {
              Multi_Align
                (cid, string_list, offset, Align_Wiggle, Error_Rate,
                 Min_Overlap, gma, &ref, &tag_list, Allow_Expels);
            }
            catch (AlignmentException_t & e)
            {
              int b = e.b_id ();

              cerr << "Failed on contig " << cid << endl;
              cerr << "Could not align string "
                << tag_list[b] << " subscript " << b << endl;
              throw;
            }
            Permute (qual_list, ref);
            Permute (clr_list, ref);
            Permute (frg_id_list, ref);

            gma.Set_Flipped (clr_list);
            gma.Get_Positions (pos);
            gma.Extract_IMP_Dels (del_list);
            msg.Update_IMPs (pos, ref, del_list);
            if (Allow_Expels)
              msg . Remove_Empty_IMPs ();

            gma.Set_Consensus_And_Qual (string_list, qual_list);
            msg.setSequence (gma.getConsensusString ());
            msg.setQuality (gma.getQualityString ());
            msg.setUniLen (strlen (gma.getConsensusString ()));

            Output_Unit (label, msg.getAccession (),
                         msg.getNumFrags (), gma, msg, string_list,
                         qual_list, clr_list, tag_list, contig_bank);

            contig_ct++;
          }

          frg_id_list.clear ();
          pos_list.clear ();
          seg_list.clear ();

          p = strtok (NULL, " \t\n");
          cid = p;
        } // end of if line starts with 'C'
        else
        {
          Ordered_Range_t ps;
          int a, b;

          fid = strtol (p, NULL, 10);
          p = strtok (NULL, " \t\n");
          a = strtol (p, NULL, 10);
          p = strtok (NULL, " \t\n");
          b = strtol (p, NULL, 10);
          ps.setRange (a, b);
          frg_id_list.push_back (fid);
          pos_list.push_back (ps);
          if (Input_Format == PARTIAL_READ_INPUT)
          {
            p = strtok (NULL, " \t\n");
            a = strtol (p, NULL, 10);
            p = strtok (NULL, " \t\n");
            b = strtol (p, NULL, 10);
            ps.setRange (a, b);
            seg_list.push_back (ps);
          }
        }
      } // end of while fgets

      // Process the last contig here
      if (frg_id_list.size () > 0)
      {
        Get_Strings_And_Offsets (string_list, qual_list, clr_list,
                                 tag_list, offset, frg_id_list, pos_list,
                                 seg_list, read_bank);

        if (Extra_fp != NULL)
        {
          fprintf (stderr,
                   "Fasta sequence option not yet available--Ignored\n");
          fclose (Extra_fp);
        }

        msg.setAccession (cid);
        msg.setIMPs (frg_id_list, pos_list);
        try
        {
          Multi_Align
            (cid, string_list, offset, Align_Wiggle, Error_Rate, Min_Overlap,
             gma, &ref, &tag_list, Allow_Expels);
        }
        catch (AlignmentException_t & e)
        {
          int b = e.b_id ();

          cerr << "Failed on contig " << cid << endl;
          cerr << "Could not align string "
            << tag_list[b] << " subscript " << b << endl;
          throw;
        }

        Permute (qual_list, ref);
        Permute (clr_list, ref);
        Permute (frg_id_list, ref);

        gma.Set_Flipped (clr_list);
        gma.Get_Positions (pos);
        gma.Extract_IMP_Dels (del_list);
        msg.Update_IMPs (pos, ref, del_list);
        if (Allow_Expels)
          msg . Remove_Empty_IMPs ();

        gma.Set_Consensus_And_Qual (string_list, qual_list);
        msg.setSequence (gma.getConsensusString ());
        msg.setQuality (gma.getQualityString ());
        msg.setUniLen (strlen (gma.getConsensusString ()));

        Output_Unit (label, msg.getAccession (),
                     msg.getNumFrags (), gma, msg, string_list,
                     qual_list, clr_list, tag_list, contig_bank);

        contig_ct++;
      } // end of if frg_id_list.size > 0

      cerr << "Processed " << contig_ct << " contigs" << endl;
    } // end of if Input_Format == PARTIAL_READ_INPUT || SIMPLE_CONTIG_INPUT

    if (Input_Format != BANK_INPUT)
      fclose (input_fp);
    if (Expel_fp != NULL)
      fclose (Expel_fp);

    read_bank.close ();
  }
  catch (Exception_t & e)
  {
    cerr << "** AMOS Exception **" << endl;
    cerr << e << endl;
    exit (EXIT_FAILURE);
  }
  catch (std::exception & e)
  {
    cerr << "** Standard Exception **" << endl;
    cerr << e << endl;
    exit (EXIT_FAILURE);
  }

  return 0;
}



bool By_Lo_Position
  (const Celera_IMP_Sub_Msg_t & a, const Celera_IMP_Sub_Msg_t & b)
//  Return true iff the region in  a  comes before the region in  b
//  in the uni/contig that contains them.  Used for sorting.
{
  Ordered_Range_t position;
  int start, stop;
  int a_lo, a_hi, b_lo, b_hi;

  position = a.getPosition ();
  start = position.getBegin ();
  stop = position.getEnd ();
  a_lo = Min (start, stop);
  a_hi = Max (start, stop);

  position = b.getPosition ();
  start = position.getBegin ();
  stop = position.getEnd ();
  b_lo = Min (start, stop);
  b_hi = Max (start, stop);

  if (a_lo < b_lo)
    return true;
  else if (a_lo == b_lo && a_hi < b_hi)
    return true;

  return false;
}



static void Get_Strings_And_Offsets
  (vector < char *>&s, vector < char *>&q, vector < Range_t > &clr_list,
   vector < char *>&tag_list, vector < int >&offset,
   const Celera_Message_t & msg, Bank_t & read_bank)
//  Populate  s  and  offset  with reads and their unitig positions
//  for the unitig in  msg  with reads coming from  read_bank.
//  Put the clear-ranges for the strings in  clr_list  with
//  coordinates swapped if the string is reverse-complemented.
//   read_bank  must already be opened.  Put the corresponding quality-value
//  strings for the reads into  q .  Put the identifying tags for the reads
//  in  tag_list .
{
  vector < Celera_IMP_Sub_Msg_t > frgs = msg.getIMPList ();
  Read_t read;
  Ordered_Range_t position;
  int prev_offset;
  int i, n;

  n = s.size ();
  for (i = 0; i < n; i++)
    free (s[i]);
  s.clear ();

  n = q.size ();
  for (i = 0; i < n; i++)
    free (q[i]);
  q.clear ();

  n = tag_list.size ();
  for (i = 0; i < n; i++)
    free (tag_list[i]);
  tag_list.clear ();

  clr_list.clear ();
  offset.clear ();

  sort (frgs.begin (), frgs.end (), By_Lo_Position);

  prev_offset = 0;
  n = msg.getNumFrags ();
  for (i = 0; i < n; i++)
  {
    char *tmp, tag_buff[100];
    string seq;
    string qual;
    Range_t clear;
    int this_offset;
    int a, b, j, len, qlen;

    position = frgs[i].getPosition ();
    a = position.getBegin ();
    b = position.getEnd ();

    read_bank.fetch (frgs[i].getId (), read);

    if (Use_SeqNames)
      tag_list.push_back (strdup (read.getEID ().c_str ()));
    else
    {
      sprintf (tag_buff, "%u", read.getIID ());
      tag_list.push_back (strdup (tag_buff));
    }

    clear = read.getClearRange ();
    if (Verbose > 2)
      cerr << read;
    seq = read.getSeqString (clear);
    qual = read.getQualString (clear);
    if (b < a)
    {
      Reverse_Complement (seq);
      reverse (qual.begin (), qual.end ());
      clear.swap ();
    }
    clr_list.push_back (clear);

    len = seq.length ();
    tmp = strdup (seq.c_str ());
    for (j = 0; j < len; j++)
      tmp[j] = tolower (tmp[j]);
    s.push_back (tmp);

    qlen = qual.length ();
    if (len != qlen)
    {
      sprintf
        (Clean_Exit_Msg_Line,
         "ERROR:  Sequence length (%d) != quality length (%d) for read %d\n",
         len, qlen, read.getIID ());
      Clean_Exit (Clean_Exit_Msg_Line, __FILE__, __LINE__);
    }
    tmp = strdup (qual.c_str ());
    q.push_back (tmp);

    this_offset = Min (a, b);
    offset.push_back (this_offset - prev_offset);
    prev_offset = this_offset;
  }

  return;
}



static void Get_Strings_And_Offsets
  (vector < char *>&s, vector < char *>&q, vector < Range_t > &clr_list,
   vector < char *>&tag_list, vector < int >&offset, vector < int >&fid,
   vector < Ordered_Range_t > &pos, vector < Ordered_Range_t > &seg,
   Bank_t & read_bank)
//  Populate  s  and  offset  with reads and their contig positions
//  for the contig with read-ids in  fid  and  consensus positions
//  in  pos .  Put the corresponding quality-value strings for the reads
//  into  q .  Put the clear-ranges for the strings in  clr_list  with
//  coordinates swapped if the string is reverse-complemented.
//  Put the identifying tags for the reads
//  in  tag_list .  Get reads and qualities from  read_bank.
//   read_bank  must already be opened.  If  seg  is not empty, used
//  the values in it to determine what segment of each read to use.
{
  Read_t read;
  int prev_offset;
  bool partial_reads;
  int i, n;

  n = s.size ();
  for (i = 0; i < n; i++)
    free (s[i]);
  s.clear ();

  n = q.size ();
  for (i = 0; i < n; i++)
    free (q[i]);
  q.clear ();

  n = tag_list.size ();
  for (i = 0; i < n; i++)
    free (tag_list[i]);
  tag_list.clear ();

  clr_list.clear ();
  offset.clear ();
  partial_reads = (seg.size () > 0);

  Sort_By_Low_Pos (fid, pos, seg);
  if (Verbose > 2)
  {
    int i, n;

    fprintf (stderr, "After Sort_By_Low_Pos:\n");
    n = fid.size ();
    for (i = 0; i < n; i++)
      fprintf (stderr, "%8d  %6d %6d\n", fid[i],
               pos[i].getBegin (), pos[i].getEnd ());
  }

  prev_offset = 0;
  n = fid.size ();
  for (i = 0; i < n; i++)
  {
    char *tmp, tag_buff[100];
    string seq;
    string qual;
    Range_t clear;
    int this_offset;
    int a, b, j, len, qlen;

    a = pos[i].getBegin ();
    b = pos[i].getEnd ();

    read_bank.fetch (fid[i], read);

    if (Use_SeqNames)
      tag_list.push_back (strdup (read.getEID ().c_str ()));
    else
    {
      sprintf (tag_buff, "%u", read.getIID ());
      tag_list.push_back (strdup (tag_buff));
    }

    clear = read.getClearRange ();
    if (Verbose > 2)
      cerr << read;
    seq = read.getSeqString (clear);
    qual = read.getQualString (clear);
    if (partial_reads)
    {
      int lo, hi;

      lo = seg[i].getBegin ();
      hi = seg[i].getEnd ();
      seq = seq.substr (lo, hi - lo);
      qual = qual.substr (lo, hi - lo);
    }
    if (b < a)
    {
      Reverse_Complement (seq);
      reverse (qual.begin (), qual.end ());
      clear.swap ();
    }

    clr_list.push_back (clear);

    len = seq.length ();
    tmp = strdup (seq.c_str ());
    for (j = 0; j < len; j++)
      tmp[j] = tolower (tmp[j]);
    s.push_back (tmp);

    qlen = qual.length ();
    if (len != qlen)
    {
      sprintf
        (Clean_Exit_Msg_Line,
         "ERROR:  Sequence length (%d) != quality length (%d) for read %d\n",
         len, qlen, fid[i]);
      Clean_Exit (Clean_Exit_Msg_Line, __FILE__, __LINE__);
    }
    tmp = strdup (qual.c_str ());
    q.push_back (tmp);

    this_offset = Min (a, b);
    offset.push_back (this_offset - prev_offset);
    prev_offset = this_offset;
  }

  return;
}

struct cmpTile:public binary_function < Tile_t, Tile_t, bool >
{
  bool operator  () (Tile_t x, Tile_t y)
  {
    if (x.offset < y.offset)
      return true;
    if (x.offset == y.offset && x.range.getLength () < y.range.getLength ())
      return true;
    return false;
  }
};


// MP - using layout info to populate fid & pos in addition to the other work
// done in this function
static void Get_Strings_And_Offsets
  (vector < char *>&s, vector < char *>&q, vector < Range_t > &clr_list,
   vector < char *>&tag_list, vector < int >&offset, Layout_t & layout,
   vector < int >&fid, vector < Ordered_Range_t > &pos, Bank_t & read_bank)
//  Populate  s  and  offset  with reads and their contig positions
//  for the contig with read-ids in  fid  and  consensus positions
//  in  pos .  Put the corresponding quality-value strings for the reads
//  into  q .  Put the clear-ranges for the strings in  clr_list  with
//  coordinates swapped if the string is reverse-complemented.
//  Put the identifying tags for the reads
//  in  tag_list .  Get reads and qualities from  read_bank.
//   read_bank  must already be opened.  If  seg  is not empty, used
//  the values in it to determine what segment of each read to use.
{
  Read_t read;
  int prev_offset;
  bool partial_reads;
  int i, n;

  n = s.size ();
  for (i = 0; i < n; i++)
    free (s[i]);
  s.clear ();

  n = q.size ();
  for (i = 0; i < n; i++)
    free (q[i]);
  q.clear ();

  n = tag_list.size ();
  for (i = 0; i < n; i++)
    free (tag_list[i]);
  tag_list.clear ();

  clr_list.clear ();
  offset.clear ();
  fid.clear ();

  sort (layout.getTiling ().begin (), layout.getTiling ().end (), cmpTile ());

  prev_offset = 0;

  for (vector < Tile_t >::iterator ti = layout.getTiling ().begin ();
       ti != layout.getTiling ().end (); ti++)
  {
    char *tmp, tag_buff[100];
    string seq;
    string qual;
    Range_t clear;
    int this_offset;
    int a, b, j, len, qlen;

    if (ti->range.getBegin () < ti->range.getEnd ())
    { // forward match
      a = ti->offset;
      b = ti->offset + ti->range.getLength ();
    }
    else
    {
      a = ti->offset + ti->range.getLength ();
      b = ti->offset;
    }

    pos.push_back (Ordered_Range_t (a, b));
    fid.push_back (ti->source);


    read_bank.fetch (ti->source, read);

    if (Verbose > 3)
    {
      cerr << "Loading e" << read.getEID () << " i" << read.
        getIID () << "... ";
    }

    if (Use_SeqNames)
      tag_list.push_back (strdup (read.getEID ().c_str ()));
    else
    {
      sprintf (tag_buff, "%u", read.getIID ());
      tag_list.push_back (strdup (tag_buff));
    }

    if (USE_LayoutClear)
    {
      clear = ti->range;
      if (clear.isReverse ())
      {
        clear.swap ();
      }
    }
    else
    {
      // Grab clear range from read bank
      clear = read.getClearRange ();
    }

    seq = read.getSeqString (clear);
    qual = read.getQualString (clear);

    if (b < a)
    {
      Reverse_Complement (seq);
      reverse (qual.begin (), qual.end ());
      clear.swap ();
    }

    clr_list.push_back (clear);

    len = seq.length ();
    tmp = strdup (seq.c_str ());
    for (j = 0; j < len; j++)
      tmp[j] = tolower (tmp[j]);
    s.push_back (tmp);

    qlen = qual.length ();
    if (len != qlen)
    {
      sprintf
        (Clean_Exit_Msg_Line,
         "ERROR:  Sequence length (%d) != quality length (%d) for read %d\n",
         len, qlen, ti->source);
      Clean_Exit (Clean_Exit_Msg_Line, __FILE__, __LINE__);
    }
    tmp = strdup (qual.c_str ());
    q.push_back (tmp);

    this_offset = Min (a, b);
    offset.push_back (this_offset - prev_offset);

    prev_offset = this_offset;

    if (Verbose > 3)
    {
      cerr << "ok." << endl;
    }
  }

  return;
} // end of Get_Strings_And_Offsets



static void Output_Unit
  (const string & label, const string & id, int num_reads,
   Gapped_Multi_Alignment_t & gma, Celera_Message_t & msg,
   const vector < char *>&string_list,
   const vector < char *>&qual_list, const vector < Range_t > &clr_list,
   vector < char *>tag_list, BankStream_t & bank)
//  Output the consensus/multialignment unit in  msg  and  gma  with
//   id  and  num_reads  in the format specified in global  Output_Format .
//   label  is the separator in  MULTI_ALIGN_OUTPUT .   string_list  and
//   qual_list  hold the sequence and quality values of the component
//  reads.   clr_list  holds the clear ranges of the fragments from
//  which the reads came (in reverse order if the read has been
//  reverse-complemented).   tag_list  holds the IIDs of the reads.
{
  Contig_t act;
  Message_t amg;
  int  expel_ct = 0;

  if (Allow_Expels)
    expel_ct = gma . Print_Empty_Aligns
    (Expel_fp, tag_list, msg . getAccession () . c_str ());

  act.setEID (id);
  switch (Output_Format)
  {
    case MULTI_ALIGN_OUTPUT:
      cout << endl << endl << label << " #" << msg.getAccession ()
        << "   " << num_reads << " reads" << endl;
      gma.Consensus_To_Lower ();
      gma.Print (stdout, string_list, 60, &tag_list);
      break;
    case ONLY_FASTA_OUTPUT:
      cout << ">" << msg.getAccession () << endl;
      Fasta_Print_Skip (stdout, gma.getConsensusString (), "-", NULL);
      break;
    case CELERA_MSG_OUTPUT:
      gma.Set_Consensus_And_Qual (string_list, qual_list);
      msg.setSequence (gma.getConsensusString ());
      msg.setQuality (gma.getQualityString ());
      msg.setUniLen (strlen (gma.getConsensusString ()));
      msg.print (stdout);
      break;
    case TIGR_CONTIG_OUTPUT:
      gma.TA_Print (stdout, string_list, clr_list, 60, &tag_list,
                    msg.getAccession ());
      break;
    case AMOS_OUTPUT:
      gma.Make_AMOS_Contig (clr_list, tag_list, act);
      act.writeMessage (amg);
      amg.write (cout);
      break;
    case BANK_OUTPUT:
      gma.Make_AMOS_Contig (clr_list, tag_list, act);
      bank << act;
      break;
    default:
      sprintf
        (Clean_Exit_Msg_Line,
         "ERROR:  Bad output type = %d\n", int (Output_Format));
      Clean_Exit (Clean_Exit_Msg_Line, __FILE__, __LINE__);
  }

  return;
}



static void Parse_Command_Line (int argc, char *argv[])
//  Get options and parameters from command line with  argc
//  arguments in  argv [0 .. (argc - 1)] .
{
  bool errflg = false;
  int ch;

  optarg = NULL;

  while (!errflg
      && ((ch = getopt (argc, argv, "aAbBcCe:E:fhi:Ln:o:PsSTuv:w:x:")) != EOF))
    switch (ch)
    {
      case 'a':
        Output_Format = MULTI_ALIGN_OUTPUT;
        break;

      case 'A':
        Output_Format = AMOS_OUTPUT;
        break;

      case 'b':
        Input_Format = BANK_INPUT;
        break;

      case 'B':
        Output_Format = BANK_OUTPUT;
        break;

      case 'c':
        Do_Contig_Messages = true;
        Do_Unitig_Messages = false;
        break;

      case 'C':
        Input_Format = CELERA_MSG_INPUT;
        break;

      case 'e':
        Error_Rate = strtod (optarg, NULL);
        break;

      case 'E':
        Extra_fp = File_Open (optarg, "r", __FILE__, __LINE__);
        break;

      case 'f':
        Output_Format = ONLY_FASTA_OUTPUT;
        break;

      case 'h':
        errflg = true;
        break;

      case 'i':
        byIID = true;
        IID_fp.open (optarg, ifstream::in);
        if (!IID_fp.is_open ())
        {
          cerr << "Couldn't open " << optarg << endl;
          exit (1);
        }
        break;

      case 'L':
        USE_LayoutClear = true;
        break;

      case 'n':
        byEID = true;
        EID_fp.open (optarg, ifstream::in);
        if (!EID_fp.is_open ())
        {
          cerr << "Couldn't open " << optarg << endl;
          exit (1);
        }
        break;

      case 'o':
        Min_Overlap = strtol (optarg, NULL, 10);
        break;

      case 'P':
        Input_Format = PARTIAL_READ_INPUT;
        break;

      case 's':
        Use_SeqNames = true;
        break;

      case 'S':
        Input_Format = SIMPLE_CONTIG_INPUT;
        break;

      case 'T':
        Output_Format = TIGR_CONTIG_OUTPUT;
        break;

      case 'u':
        Do_Unitig_Messages = true;
        Do_Contig_Messages = false;
        break;

      case 'v':
        Verbose = strtol (optarg, NULL, 10);
        break;

      case 'w':
        Align_Wiggle = strtol (optarg, NULL, 10);
        break;

      case 'x':
        Allow_Expels = true;
        Expel_fp = File_Open (optarg, "w", __FILE__, __LINE__);
        fprintf (Expel_fp, "# expelled reads:  read-id  contig-id\n");
        cerr << "Expelled reads listed in file " << optarg << endl;
        break;

      case '?':
        fprintf (stderr, "Unrecognized option -%c\n", optopt);

      default:
        errflg = true;
    }

    if (Input_Format == CELERA_MSG_INPUT
        && !Do_Unitig_Messages && !Do_Contig_Messages)
    {
      fprintf (stderr, "\nERROR:  Must specify either -u or -c\n\n");
      errflg = true;
    }

    if (errflg)
    {
      Usage (argv[0]);
      exit (EXIT_FAILURE);
    }

    if ((Input_Format == BANK_INPUT && optind > argc - 1) ||
        (Input_Format != BANK_INPUT && optind > argc - 2))
    {
      Usage (argv[0]);
      exit (EXIT_FAILURE);
    }

    if (Input_Format != BANK_INPUT)
      Tig_File_Name = argv[optind++];
    Bank_Name = argv[optind++];

  return;
}



static void Sort_By_Low_Pos
  (vector < int >&fid, vector < Ordered_Range_t > &pos,
   vector < Ordered_Range_t > &seg)
//  Sort the entries in  fid  and  pos  (and  seg  if it's not empty)
//  into ascending order based on the lower of two values in each
//   pos  entry.  Use an insertion sort since we expect the values are
//  likely to be nearly sorted already.
{
  vector < int >low;
  bool use_seg;
  Ordered_Range_t save_pos, save_seg;
  int save_fid, save_low;
  int n, i;

  n = pos.size ();
  if (n == 0)
    return;

  use_seg = (seg.size () > 0);

  for (i = 0; i < n; i++)
    low.push_back (Min (pos[i].getBegin (), pos[i].getEnd ()));

  for (i = 1; i < n; i++)
  {
    int j;

    if (low[i - 1] <= low[i])
      continue; // already in order

    save_low = low[i];
    save_pos = pos[i];
    save_fid = fid[i];
    if (use_seg)
      save_seg = seg[i];

    for (j = i; j > 0 && low[j - 1] > save_low; j--)
    {
      low[j] = low[j - 1];
      pos[j] = pos[j - 1];
      fid[j] = fid[j - 1];
      if (use_seg)
        seg[j] = seg[j - 1];
    }

    low[j] = save_low;
    pos[j] = save_pos;
    fid[j] = save_fid;
    if (use_seg)
      seg[j] = save_seg;
  }

  return;
}



static void Usage (const char *command)
//  Print to stderr description of options and command line for
//  this program.   command  is the command that was used to
//  invoke it.
{
  fprintf (stderr,
         "USAGE:  %s  <tig-file> <bank-name>\n"
         "\n"
         "Read layout information from <tig-file> describing positions\n"
         "of reads, and create multialignments and/or consensus sequences\n"
         "for them.  Read sequences are obtained from <bank-name>\n"
         "\n"
         "Options:\n"
         "  -a       Output alignments instead of consensus messages\n"
         "  -A       Output an AMOS message file\n"
         "  -b       Input from AMOS bank\n"
         "  -B       Output to an AMOS bank\n"
         "  -c       Process contig messages\n"
         "  -C       Input is Celera msg format, i.e., a .cgb or .cgw file\n"
         "  -e <x>   Set alignment error rate to <x>, e.g.,  -e 0.05  for 5%% error\n"
         "  -E <fn>  Get extra sequences to align from fasta file <fn>\n"
         "  -f       Output consensus only in FASTA format\n"
         "  -h       Print this usage message\n"
         "  -i <fn>  File containing list of IIDs to be processed\n"
         "  -L       Use clear range in layout rather than readbank (-b only)\n"
         "  -n <fn>  File containing list of EIDs (names) to be processed\n"
         "  -o <n>   Set minimum overlap bases to <n>\n"
         "  -P       Input is simple contig format, i.e., UMD format\n"
         "              using partial reads\n"
         "  -s       Output EID seqnames for reads instead of IID ints\n"
         "  -S       Input is simple contig format, i.e., UMD format\n"
         "  -T       Output in TIGR Assembler contig format\n"
         "  -u       Process unitig messages\n"
         "  -v <n>   Set verbose level to <n>.  Higher produces more output\n"
         "  -w <n>   Set alignment wiggle to <n>. Default is 15. Use a smaller value for Solexa reads (Example: -w 2)\n"
         "  -x <fn>  Expel poor aligning reads from contigs and list them in file <fn>\n"
         "\n", command);

  return;
}

static void
readIIDFile (ifstream & f, list < ID_t > &l)
  // Reads IIDs from a file
{
  ID_t iid;

  while (f >> iid)
    l.push_back (iid);
}

static void
readEIDFile (ifstream & f, list < string > &l)
  // Reads EIDs from a file
{
  string eid;

  while (f >> eid)
    l.push_back (eid);
}
