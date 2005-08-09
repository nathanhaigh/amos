//  A. L. Delcher
//
//  File:  fasta.h
//
//  Last Modified:  25 November 2002
//
//  Routines to manipulate FASTA format files


#ifndef  __FASTA_HH
#define  __FASTA_HH


#include  "delcher.hh"
#include  <string>
#include  <vector>
#include  <cstring>


const int  DEFAULT_FASTA_WIDTH = 60;
  // Max number of characters to print on a FASTA data line


char  Complement
    (char ch);
void  Fasta_Print
    (FILE * fp, const char * s, const char * hdr = NULL,
     int fasta_width = DEFAULT_FASTA_WIDTH);
void  Fasta_Print_N
    (FILE * fp, const char * s, int n, const char * hdr = NULL,
     int fasta_width = DEFAULT_FASTA_WIDTH);
void  Fasta_Print_Skip
    (FILE * fp, const char * s, const char * skip, const char * hdr = NULL,
     int fasta_width = DEFAULT_FASTA_WIDTH);
bool  Fasta_Qual_Read
    (FILE * fp, std::string & q, std::string & hdr);
bool  Fasta_Read
    (FILE * fp, std::string & s, std::string & hdr);
void  Reverse_Complement
    (char * s);
void  Reverse_Complement
    (std::string & s);
void  Reverse_String
    (char * s);
void  Reverse_String
    (std::string & s);

int  Fasta_Read_String  (FILE *, char * &, long int &, char [], int);


#endif // #ifndef __FASTA_HH
