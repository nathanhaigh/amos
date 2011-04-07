//  Daniel Sommer
//
//  File:  fastq.h
//
//  Last Modified:  April 2011
//
//  Routines to manipulate FASTQ format files


#ifndef  __FASTQ_HH
#define  __FASTQ_HH


#include  "delcher.hh"
#include  <string>
#include  <vector>
#include  <cstring>


const int  DEFAULT_FASTQ_WIDTH = 60;
  // Max number of characters to print on a FASTA data line


bool  Fastq_Read(FILE * fp, std::string & s, std::string & hdr, std::string & q, std::string & qualHdr);

#endif // #ifndef __FASTQ_HH
