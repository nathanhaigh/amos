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
#include  "inttypes_AMOS.hh"
#include  <string>
#include  <vector>
#include  <cstring>


const int  DEFAULT_FASTQ_WIDTH = 60;
  // Max number of characters to print on a FASTA data line

enum FastqQualType {ILLUMINA, SANGER};
const std::string FASTQ_QUALITY_NAMES[] = {"ILLUMINA", "SANGER"};
const uint32_t FASTQ_QUALITY_COUNT = 2;
const FastqQualType FASTQ_DEFAULT_QUALITY_TYPE = SANGER;

bool  Fastq_Read(FILE * fp, std::string & s, std::string & hdr, std::string & q, std::string & qualHdr, FastqQualType qualType = FASTQ_DEFAULT_QUALITY_TYPE);

#endif // #ifndef __FASTQ_HH
