// Read a list of short kmers (31 bases or less) and then
// compute what regions of the fasta sequences read from
// stdin are covered by them (or their reverse complement).


#include  "delcher.hh"
#include  "fasta.hh"
#include  <string>
#include  <vector>


#define  DEBUG  0


const int  MAX_LINE = 1000;


typedef  long long unsigned  Mer_t;


static Mer_t  Filled_Mask = Mer_t (1) << (8 * sizeof (Mer_t) - 1);
static Mer_t  Extract_Mask = 0;
static Mer_t  Forward_Mask;
static Mer_t  Hash_Mask;
static int  Hash_Shift = 0;
static Mer_t  * Hash_Table = NULL;
static int  Hash_Table_Size = 0;
static int  Kmer_Len = 0;


static void  Build_Hash_Table
    (const vector <Mer_t> & mer_list);
static unsigned  Char_To_Binary
    (char ch);
static void  Fasta_To_Binary
    (const string & s, Mer_t & mer);
static void  Forward_Add_Ch
    (Mer_t & mer, char ch);
static void  Reverse_Add_Ch
    (Mer_t & mer, char ch);
static int  Hash
    (Mer_t mer);
static bool  Hash_Find
    (Mer_t mer);
static void  Hash_Insert
    (Mer_t mer);
static void  Print_Mer_Coverage
    (const string & s);
static void  Read_Mers
    (const char * fname, vector <Mer_t> & mer_list);



int  main
    (int argc, char * argv [])

  {
   vector <Mer_t>  mer_list;
   string  s, tag;
   int  n;

   Read_Mers (argv [1], mer_list);

   n = mer_list . size ();

   Build_Hash_Table (mer_list);

#if  DEBUG
{
 int  i;

 printf ("Kmer_Len = %d  Hash_Table_Size = %d  Hash_Shift = %d\n",
      Kmer_Len, Hash_Table_Size, Hash_Shift);
 for  (i = 0;  i < n;  i ++)
   printf ("%15llo %8d\n", mer_list [i], Hash (mer_list [i]));
}
#endif

   Forward_Mask = ((long long unsigned) 1 << (2 * Kmer_Len - 2)) - 1;
   Extract_Mask = ((long long unsigned) 1 << (2 * Kmer_Len)) - 1;

#if  DEBUG
{
 printf ("Forward_Mask = %15llo\n", Forward_Mask);
 printf ("Extract_Mask = %15llo\n", Extract_Mask);
}
#endif

   while  (Fasta_Read (stdin, s, tag))
     {
      printf (">%s\n", tag . c_str ());
      Print_Mer_Coverage (s);
     }

   return  0;
  }



static void  Build_Hash_Table
    (const vector <Mer_t> & mer_list)

//  Add entries in  mer_list  to the global
//   Hash_Table .

  {
   int  i, n;

   n = mer_list . size ();
   Hash_Shift = 1;
   for  (i = 2;  i < 3 * n;  i *= 2)
     Hash_Shift ++;
   Hash_Table_Size = i;
   Hash_Mask = Hash_Table_Size - 1;

   Hash_Table = (Mer_t *) Safe_calloc (Hash_Table_Size, sizeof (Mer_t),
        __FILE__, __LINE__);
   for  (i = 0;  i < n;  i ++)
     Hash_Insert (mer_list [i]);

   return;
  }



static unsigned  Char_To_Binary
    (char ch)

//  Return the binary equivalent of  ch .

  {
   switch  (tolower (ch))
     {
      case  'a' :
        return  0;
      case  'c' :
        return  1;
      case  'g' :
        return  2;
      case  't' :
        return  3;
      default :
        sprintf (Clean_Exit_Msg_Line, "Bad char = %c (ASCII %u) in Char_To_Binary",
             ch, unsigned (ch));
        Clean_Exit (Clean_Exit_Msg_Line, __FILE__, __LINE__);
     }
   return  0;
  }



static void  Fasta_To_Binary
    (const string & s, Mer_t & mer)

//  Convert string  s  to its binary equivalent in  mer .

  {
   int  i, n;

   n = s . length ();
   mer = 0;
   for  (i = 0;  i < n;  i ++)
     {
      mer <<= 2;
      mer |= Char_To_Binary (s [i]);
     }

   return;
  }



static void  Forward_Add_Ch
    (Mer_t & mer, char ch)

//  Add  ch  to  mer  on the right, sliding one character
//  off the left end of  mer .

  {
   mer &= Forward_Mask;
   mer <<= 2;
   mer |= Char_To_Binary (ch);

   return;
  }



static int  Hash
    (Mer_t mer)

//  Convert  mer  to a subscript in the global  Hash_Table .

  {
   unsigned  result = 0;

   while  (mer > 0)
     {
      result ^= mer & Hash_Mask;
      mer >>= Hash_Shift;
     }

   return  int (result);
  }



static bool  Hash_Find
    (Mer_t mer)

//  Return  true  iff  mer  occurs in the global  Hash_Table .

  {
   int  i;

   i = Hash (mer);

   while  (Hash_Table [i] & Filled_Mask)
     {
      if  ((Hash_Table [i] & Extract_Mask) == mer)
          return  true;
      i ++;
      if  (i == Hash_Table_Size)
          i = 0;
     }

   return  false;
  }



static void  Hash_Insert
    (Mer_t mer)

//  Insert  mer  into the global  Hash_Table .

  {
   int  i;

   i = Hash (mer);

   while  (Hash_Table [i] & Filled_Mask)
     {
      i ++;
      if  (i == Hash_Table_Size)
          i = 0;
     }

   Hash_Table [i] = (mer | Filled_Mask);

   return;
  }



static void  Print_Mer_Coverage
    (const string & s)

//  Print regions in string  s  that are covered
//  by mers (or their reverse-complements) in
//  the global hash table.

  {
   Mer_t  fwd_mer = 0, rev_mer = 0;
   int  lo, hi, total = 0;
   int  i, j, n;

   n = s . length ();

   if  (n < Kmer_Len)
       {
        printf ("Total %d of %d (%.1f%%)\n", 0, n, Percent (0, n));
        return;
       }

   for  (i = 0;  i < Kmer_Len - 1;  i ++)
     {
      Forward_Add_Ch (fwd_mer, s [i]);
      Reverse_Add_Ch (rev_mer, s [i]);
     }

   lo = 0;
   hi = -1;
   for  (j = 0;  i < n;  i ++, j ++)
     {
      Forward_Add_Ch (fwd_mer, s [i]);
      Reverse_Add_Ch (rev_mer, s [i]);

#if  DEBUG
{
 printf ("%4d  %c  %15llo  %15llo  %c  %c\n",
      i, s [i], fwd_mer, rev_mer,
      Hash_Find (fwd_mer) ? 'T' : 'F',
      Hash_Find (rev_mer) ? 'T' : 'F');
}
#endif

      if  (Hash_Find (fwd_mer) || Hash_Find (rev_mer))
          {
           if  (hi < j)
               {
                if  (hi > 0)
                    {
                     printf ("%8d %8d %8d\n", lo, hi, hi - lo);
                     total += hi - lo;
                    }
                lo = j;
               }
           hi = i + 1;
          }
     }

   if  (hi > 0)
       {
        total += hi - lo;
        printf ("%8d %8d %8d\n", lo, hi, hi - lo);
       }

   printf ("Total %d of %d (%.1f%%)\n", total, n, Percent (total, n));

   return;
  }



static void  Read_Mers
    (const char * fname, vector <Mer_t> & mer_list)

//  Read kmers from file name  fname  and save them
//  in binary form in  mer_list .  Input format is
//  a multi-fasta file.  Mers are assumed to contain only
//  ACGT's

  {
   FILE  * fp;
   string  s, tag;
   Mer_t  mer;

   fp = File_Open (fname, "r", __FILE__, __LINE__);

   mer_list . clear ();

   while  (Fasta_Read (fp, s, tag))
     {
      if  (Kmer_Len == 0)
          Kmer_Len = s . length ();
      else if  (Kmer_Len != int (s . length ()))
          {
           sprintf (Clean_Exit_Msg_Line, "New kmer \"%s\" length is %d instead of %d",
                s . c_str (), s . length (), Kmer_Len);
           Clean_Exit (Clean_Exit_Msg_Line, __FILE__, __LINE__);
          }
      Fasta_To_Binary (s, mer);
      mer_list . push_back (mer);
     }

   return;
  }



static void  Reverse_Add_Ch
    (Mer_t & mer, char ch)

//  Add the Watson-Crick complement of  ch  to  mer  on the left,
//  sliding one character off the right end of  mer .

  {
   mer >>= 2;
   mer |= ((long long unsigned) (3 ^ Char_To_Binary (ch)) << (2 * Kmer_Len - 2));

   return;
  }



