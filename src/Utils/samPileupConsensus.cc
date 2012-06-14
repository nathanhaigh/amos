#if HAVE_CONFIG_H
#include "config.h"
#endif
#include <iostream>
#include <getopt.h>
#include "Slice.h"
#include <stdio.h>
#include <stdlib.h>

using namespace std;
//using namespace AMOS;


#define BUFFER_SIZE 1048576
#define QUAL_OFFSET 33 // TODO: actual value
#define endl "\n"


void printHelpText()
{
	// TODO
	
  cerr << 
    "\n"
    ".NAME.\n"
    "samPileupConsensus - Calls consensus for samtools pileup output\n"
    "\n.USAGE.\n"
    "samPileupConsensus [OPTIONS] < pileup > output\n"
    "\n.OPTIONS.\n"
    "-h, -help       Print out help message\n\n"
    
    ".DESCRIPTION.\n\n"
    
    "Uses libSlice to call consensus for samtools pileups and prepends the\n"
    "consensus, quality, and individual base probabilities to the pileups.\n\n"
    
    ".OUTPUT.\n\n"
    
    "   <cons> <qual> (probs) (counts) (quals) (pileup)\n\n"
    
    "   cons, qual - Consensus base (or ambiguity code) and quality score.\n\n"
    
    "   probs - Error probabilities of each being the consensus, subtracted from one.\n"
    "           <A> <C> <G> <T> <gap>\n\n"
    
    "   counts - Total observations of each:\n"
    "            <A> <C> <G> <T> <gap>\n\n"

    "   quals - Total quality of each:\n"
    "           <A> <C> <G> <T> <gap>\n\n"

    "   pileup - The original pileup whose fileds are:\n"
    "            <ref> <pos> <refBase> <depth> <pileup> <pileupQual>\n\n"
    endl;
}

const char* skipFields(const char* string, int fields, const char separator = '\t')
{
	const char *rval = string;
	
	while ( fields && *rval )
	{
		if ( *rval == separator )
		{
			fields--;
		}
		
		rval++;
	}
	
	return rval;
}

void skipIndel(const char* &pos)
{
	int skip = 0;
	
	pos++; // skip '-' or '+'
	
	// parse decimal indel length
	//
	while ( *pos > 47 && *pos < 58 ) // digit character
	{
		skip *= 10;
		skip += *pos - 48;
		pos++;
	}
	
	pos += skip; // skip bases
}

bool GetOptions(int argc, char * const * argv)
{  
	int option_index = 0;
	static struct option long_options[] =
	{
		{"help",        0, 0, 'h'},
		{0, 0, 0, 0}
	};
	
	int c;
	while ((c = getopt_long_only(argc, argv, "", long_options, &option_index))!= -1)
	{
		switch (c)
		{
		case 'h': printHelpText(); exit(0); break;
		
		case '?': 
			cerr << "Error processing options: " << argv[optind-1] << endl; 
			return false;
		};
	}

	return true;
}

//----------------------------------------------
int main(int argc, char * const * argv)
{
	char buffer[BUFFER_SIZE];
	
	GetOptions(argc, argv);
	
	while ( cin.getline(buffer, BUFFER_SIZE - 1) )
	{
		const char* refChar = skipFields(buffer, 2);
		const char* depthChar = skipFields(refChar, 1);
		const char* baseChar = skipFields(depthChar, 1);
		const char* qualChar = skipFields(baseChar, 1);
		int depth = atoi(depthChar);
		int slicePos = 0;
		
//		printf("%s\n", refChar);
//		printf("%s\n", baseChar);
//		printf("%s\n", qualChar);
//		printf("depth: %d\n", depth);
		
		libSlice_Slice *slice = new libSlice_Slice;
		libSlice_Consensus *result = new libSlice_Consensus;
		
		// max # of values is (depth * 2 + 1) if there are deletions neighboring
		// every base; one more needed for null terminator.
		//
		slice->bc = new char [depth * 2 + 2];
		slice->qv = new char [depth * 2 + 2];
		slice->rc = new char [depth * 2 + 2];
		
		while ( *baseChar != '\t' )
		{
			char base = toupper(*baseChar);
			
			switch ( base )
			{
				case '$':
					baseChar++;
					break;
					
				case '^':
					baseChar += 2;
					break;
					
				case '+':
					skipIndel(baseChar);
					break;
					
				case '*':
					qualChar++;
				case '-':
					slice->bc[slicePos] = '-';
					slice->qv[slicePos] = 0; // TODO: deletion probability
					skipIndel(baseChar);
					slice->rc[slicePos] = 0;
					slicePos++;
					break;
				
				case 'N':
					baseChar++;
					qualChar++;
					break;
				
				case '.':
				case ',':
					base = *refChar;
				case 'A':
				case 'C':
				case 'G':
				case 'T':
					slice->bc[slicePos] = base;
					slice->qv[slicePos] = *qualChar - QUAL_OFFSET;
					baseChar++;
					qualChar++;
					slice->rc[slicePos] = 0;
					slicePos++;
					break;

				default:
					cerr
						<< "ERROR: Unknown pileup char '" << base
						<< "' in line:\n" << buffer;
					return 0;
			}
		}
		
		unsigned baseSums[5] = {0, 0, 0, 0, 0};
		unsigned qualSums[5] = {0, 0, 0, 0, 0};
		
		for ( int i = 0; i < slicePos; i++ )
		{
	//		printf("%c\t%d\t%d\n", slice->bc[i], slice->qv[i], slice->rc[i]);
			int index;
			
			switch ( slice->bc[i] )
			{
			case 'A':
				index = 0;
				break;
			case 'C':
				index = 1;
				break;
			case 'G':
				index = 2;
				break;
			case 'T':
				index = 3;
				break;
			case '-':
				index = 4;
				break;
			default:
				cerr << "Bad char " << slice->bc[i] << " in \n" << buffer << endl;
				exit(1);
			}
			
			baseSums[index]++;
			qualSums[index] += slice->qv[i];
		}
		
		slice->bc[slicePos] = '\0';
		slice->qv[slicePos] = '\0';
		slice->rc[slicePos] = '\0';
		slice->dcov = slicePos;
//		slice.c = 'A';
		
//		printf("%s\t%s\t%s\n", slice->bc, slice->qv, slice->rc);
		libSlice_getConsensusParam(slice, result, NULL, 0, 1);
		
		char consensusLight;
		unsigned qualityLight;
		
		delete slice->qv; slice->qv=NULL;
		delete slice->rc; slice->rc=NULL;
		delete slice->bc; slice->bc=NULL;
		
		printf
		(
			"%c\t%d\t%Lg\t%Lg\t%Lg\t%Lg\t%Lg\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%s\n",
			result->consensus,
			result->qvConsensus,
			1 - result->cpA,
			1 - result->cpC,
			1 - result->cpG,
			1 - result->cpT,
			1 - result->cpGap,
			baseSums[0],
			baseSums[1],
			baseSums[2],
			baseSums[3],
			baseSums[4],
			qualSums[0],
			qualSums[1],
			qualSums[2],
			qualSums[3],
			qualSums[4],
			buffer
		);
		
		delete slice;
		delete result;
    }
}

