#ifndef FEATURE_CURVE_HH
#define FEATURE_CURVE_HH 1

/*
 * Routines for computing the Feature-Response Curve 
 * on a set of assembled contigs (in an AMOS databank)
 * 
 * Copyright 2011 New York University.
 *
 * Authors :
 * 
 * 2011 : Giuseppe Narzisi
 * Email: narzisi@nyu.edu 
*/

#include "foundation_AMOS.hh"
#include <iostream>
#include <fstream>
#include <math.h>
#include <unistd.h>
#include <vector>
#include <set>

class FeatureStats {

public:
	
	std::string bankdir;	
	
	std::ofstream out;

	bool VERBOSE;
	
	int genome_size;
	
	AMOS::BankStream_t* feat_bank;
	AMOS::Bank_t* ctg_bank;
			
	FeatureStats() { 
		VERBOSE = false; 
		genome_size = 1;
	}
		
	~FeatureStats() { }
	
	void computeStats();
	
	void parse_command_line(int argc, char* argv[]);
	
	void open_bank();

	void close_bank();
};

#endif
