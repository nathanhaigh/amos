//
//  File:  fasta.h
//
//  Last Modified:  25 November 2002
//
//  Routines to manipulate FASTA format files


#ifndef  __FASTA_H_INCLUDED
#define  __FASTA_H_INCLUDED


#include  "delcher.hh"
#include  <string>


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
void  Reverse_Complement
    (string & s);


#endif
