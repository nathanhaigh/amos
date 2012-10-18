//  A. L. Delcher
//
//  File:  simple-overlap.hh
//
//  Last Modified:  Thu May 13 17:39:09 EDT 2004
//
//  Declarations for  simple-overlap.cc


#ifndef  __SIMPLE_OVERLAP_HH_INCLUDED
#define  __SIMPLE_OVERLAP_HH_INCLUDED


#include  "foundation_AMOS.hh"
#include  "delcher.hh"
#include  "align.hh"
#include  "fasta.hh"
#include  <iostream>
#include  <vector>
#include  <string>
#include  <algorithm>


using namespace std;
using namespace AMOS;


const double  DEFAULT_ERROR_RATE = 0.06;
  // Default fraction of errors allowed in overlap alignment
const int  DEFAULT_MIN_OVERLAP_LEN = 40;
  // Default minimum number of bases by which two sequences must overlap
const int  MAX_LINE = 1000;
const int  NEW_SIZE = 1000;


static void  Get_Strings_From_Bank
    (vector <char *> & s, vector <char *> & q,
     vector <Range_t> & clr_list, vector <ID_t> & id_list,
     vector <char * > & tag_list, BankStream_t & read_bank);
static void  Output
    (FILE * fp, const Simple_Overlap_t & olap);
static void  Parse_Command_Line
    (int argc, char * argv []);
static void  Read_Fasta_Strings
    (vector <char *> & s, vector <ID_t> & id_list,
     vector <char *> & tag_list, const string & fn);
static void  Usage
    (const char * command);

#endif


