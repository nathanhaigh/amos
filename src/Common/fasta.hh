//
//  File:  fasta.h
//
//  Last Modified:  25 November 2002
//
//  Routines to manipulate FASTA format files


#ifndef  __FASTA_H_INCLUDED
#define  __FASTA_H_INCLUDED


#include  "delcher.hh"
#include  "WGA_datatypes.hh"
#include  <string>
#include  <vector>


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
bool  Fasta_Qual_Read
    (FILE * fp, string & q, string & hdr);
bool  Fasta_Read
    (FILE * fp, string & s, string & hdr);
void  Reverse_Complement
    (char * s);
void  Reverse_Complement
    (string & s);


#endif
