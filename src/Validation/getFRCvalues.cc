#include "getFRCvalues.hh"

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

using namespace std;
using namespace AMOS;

#define DIV(a,b) (b?(double(a)/b):0.0)

void FeatureStats::computeStats() {
	
	open_bank();

	AMOS::Feature_t feat;
	AMOS::Contig_t ctg;
	
	int num_ctg = (int)ctg_bank->getSize();
	int cum;
	int cur;
	int totallength;
	double thresh;
	double contig_coverage = 0.0;
	
    multimap<int,int> ctgset; // associative container for reads and their lookahead value
	multimap<int,int>::iterator ctgset_it;
	multimap<int,int>::reverse_iterator ctgset_rit;
	int n50;
	int max_iid;
	vector<int> ctg_feats(num_ctg+1, 0); // position 0 is not used!
	vector<int> ctg_feats_C(num_ctg+1, 0); 
	vector<int> ctg_feats_P(num_ctg+1, 0);
	vector<int> ctg_feats_B(num_ctg+1, 0); 
	vector<int> ctg_feats_K(num_ctg+1, 0);
	vector<int> ctg_feats_M(num_ctg+1, 0);
	vector<int> ctg_feats_A(num_ctg+1, 0); 
	
	set<int> iid_set;
	set<int>::iterator iid_it;
	
	int totalfeatures = 0;
	int totalfeatures_C = 0;
	int totalfeatures_P = 0;
	int totalfeatures_B = 0;
	int totalfeatures_K = 0;
	int totalfeatures_M = 0;
	int totalfeatures_A = 0;
	
	feat_bank->seekg(1);
	
	//   static const FeatureType_t NULL_FEATURE = 0;
	//   static const FeatureType_t REPEAT       = 'R';
	//   static const FeatureType_t UNITIG       = 'U';
	//   static const FeatureType_t JOIN         = 'J';
	//   static const FeatureType_t ORF          = 'O';
	//   static const FeatureType_t FIX          = 'F';
	
	//cout << "Total contigs: " << num_ctg << endl; 
	
	max_iid = 0;
	while ((*feat_bank) >> feat)
	{
		//feat.getSource().second; // type of the object which this feature refers to 
		//cout << feat.getSource().first << endl;
		iid_set.insert(feat.getSource().first);
		if(feat.getSource().first > max_iid) {
			max_iid = feat.getSource().first;
		} 
		ctg_feats.resize(max_iid+1, 0); // position 0 is not used!
		ctg_feats_C.resize(max_iid+1, 0); 
		ctg_feats_P.resize(max_iid+1, 0);
		ctg_feats_B.resize(max_iid+1, 0); 
		ctg_feats_K.resize(max_iid+1, 0);
		ctg_feats_M.resize(max_iid+1, 0);
		ctg_feats_A.resize(max_iid+1, 0);
		
		if(feat.getType() != 'A') { // all features except MISASSEMBLY			
			ctg_feats[feat.getSource().first] += 1; 
			totalfeatures += 1;
		}
		if(feat.getType() == 'C') { // COVERAGE
			ctg_feats_C[feat.getSource().first] += 1;
			totalfeatures_C += 1;
		}
		if(feat.getType() == 'P') { // POLYMORPHISM
			ctg_feats_P[feat.getSource().first] += 1;
			totalfeatures_P += 1;
		}
		if(feat.getType() == 'B') { // BREAKPOINT
			ctg_feats_B[feat.getSource().first] += 1;
			totalfeatures_B += 1;
		}
		if(feat.getType() == 'K') { // KMER
			ctg_feats_K[feat.getSource().first] += 1;
			totalfeatures_K += 1;
		}
		if(feat.getType() == 'M') { // MATEPAIR
			ctg_feats_M[feat.getSource().first] += 1;
			totalfeatures_M += 1;
		}
		if(feat.getType() == 'A') { // MISASSEMBLY
			ctg_feats_A[feat.getSource().first] += 1;
			totalfeatures_A += 1;
		}
		//cout << "Obj: " << feat.getSource().second << ", IID: " << feat.getSource().first << "Type: " << feat.getType() << endl;
	}
	
	// store contigs into a map associative container (length,ctg_iid)
	//for (int j = 1; j < ctg_feats.size(); j++) {
	for ( iid_it=iid_set.begin() ; iid_it != iid_set.end(); iid_it++ ) {
		if(ctg_bank->existsIID(*iid_it)) {
			ctg_bank->fetch((*iid_it), ctg);
			ctgset.insert(pair<int,int>(ctg.getLength(),(*iid_it)));
		}
	}
	
	// at this point ctgset contains the contigs ordered by length (ascending order)
	
	/*
	for (ctgset_rit = ctgset.rbegin(); ctgset_rit != ctgset.rend(); ctgset_rit++) {
		cout << "Contig: " << (*ctgset_rit).second << " has length " << (*ctgset_rit).first << endl;
	}
	*/

	cout << "Computing the Feature-Response curves..." << endl;
	
	// Compute Feature-Response for all features
	cout << "- FRC" << endl;
	out.open("FRC.txt");
	if (!out.is_open()){
		cerr << "Could not open input feat_all.txt" << endl;
		exit(1);
	}
	for (int k = 1; k < 100; k++) {
		double factor = ((double)k/100.0);
		thresh = totalfeatures * factor;	
		cum = 0;
		totallength = 0;
		n50 = -1;
		for (ctgset_rit = ctgset.rbegin(); ctgset_rit != ctgset.rend(); ctgset_rit++) {
			//cur = (*ctgset_rit).first;
			cur = ctg_feats[(*ctgset_rit).second];
			cum += cur;
			//ctg_bank->fetch((*ctgset_rit).second, ctg);
			totallength += (*ctgset_rit).first;

			if (n50 == -1 && cum >= thresh) { 
				n50 = (*ctgset_rit).first; 
				break;
			}
		}
		contig_coverage = DIV(totallength,genome_size);
		out << thresh << " " << n50 << " " << contig_coverage*100.0 << endl;
	}
	out.close();
	
	// Compute Feature-Response for COVERAGE features
	cout << "- FRC-coverage" << endl;
	out.open("FRC_coverage.txt");
	if (!out.is_open()){
		cerr << "Could not open input feat_coverage.txt" << endl;
		exit(1);
	}
	for (int k = 1; k < 100; k++) {
		double factor = ((double)k/100.0);
		thresh = totalfeatures_C * factor;	
		cum = 0;
		totallength = 0;
		n50 = -1;
		for (ctgset_rit = ctgset.rbegin(); ctgset_rit != ctgset.rend(); ctgset_rit++) {
			//cur = (*ctgset_rit).first;
			cur = ctg_feats_C[(*ctgset_rit).second];
			cum += cur;
			//ctg_bank->fetch((*ctgset_rit).second, ctg);
			totallength += (*ctgset_rit).first;

			if (n50 == -1 && cum >= thresh) { 
				n50 = (*ctgset_rit).first; 
				break;
			}
		}
		contig_coverage = DIV(totallength,genome_size);
		out << thresh << " " << n50 << " " << contig_coverage*100.0 << endl;
	}
	out.close();
	
	// Compute Feature-Response for POLYMORPHISM features
	cout << "- FRC-polymorphism" << endl;
	out.open("FRC_polymorphism.txt");
	if (!out.is_open()){
		cerr << "Could not open input feat_polymorphism.txt" << endl;
		exit(1);
	}
	for (int k = 1; k < 100; k++) {
		double factor = ((double)k/100.0);
		thresh = totalfeatures_P * factor;	
		cum = 0;
		totallength = 0;
		n50 = -1;
		for (ctgset_rit = ctgset.rbegin(); ctgset_rit != ctgset.rend(); ctgset_rit++) {
			//cur = (*ctgset_rit).first;
			cur = ctg_feats_P[(*ctgset_rit).second];
			cum += cur;
			//ctg_bank->fetch((*ctgset_rit).second, ctg);
			totallength += (*ctgset_rit).first;

			if (n50 == -1 && cum >= thresh) { 
				n50 = (*ctgset_rit).first; 
				break;
			}
		}
		contig_coverage = DIV(totallength,genome_size);
		out << thresh << " " << n50 << " " << contig_coverage*100.0 << endl;
	}
	out.close();
	
	// Compute Feature-Response for BREAKPOINT features
	cout << "- FRC-breakpoint" << endl;
	out.open("FRC_breakpoint.txt");
	if (!out.is_open()){
		cerr << "Could not open input feat_breakpoint.txt" << endl;
		exit(1);
	}
	for (int k = 1; k < 100; k++) {
		double factor = ((double)k/100.0);
		thresh = totalfeatures_B * factor;	
		cum = 0;
		totallength = 0;
		n50 = -1;
		for (ctgset_rit = ctgset.rbegin(); ctgset_rit != ctgset.rend(); ctgset_rit++) {
			//cur = (*ctgset_rit).first;
			cur = ctg_feats_B[(*ctgset_rit).second];
			cum += cur;
			//ctg_bank->fetch((*ctgset_rit).second, ctg);
			totallength += (*ctgset_rit).first;

			if (n50 == -1 && cum >= thresh) { 
				n50 = (*ctgset_rit).first; 
				break;
			}
		}
		contig_coverage = DIV(totallength,genome_size);
		out << thresh << " " << n50 << " " << contig_coverage*100.0 << endl;
	}
	out.close();
	
	// Compute Feature-Response for KMER features
	cout << "- FRC-kmer" << endl;
	out.open("FRC_kmer.txt");
	if (!out.is_open()){
		cerr << "Could not open input feat_kmer.txt" << endl;
		exit(1);
	}
	for (int k = 1; k < 100; k++) {
		double factor = ((double)k/100.0);
		thresh = totalfeatures_K * factor;	
		cum = 0;
		totallength = 0;
		n50 = -1;
		for (ctgset_rit = ctgset.rbegin(); ctgset_rit != ctgset.rend(); ctgset_rit++) {
			//cur = (*ctgset_rit).first;
			cur = ctg_feats_K[(*ctgset_rit).second];
			cum += cur;
			//ctg_bank->fetch((*ctgset_rit).second, ctg);
			totallength += (*ctgset_rit).first;

			if (n50 == -1 && cum >= thresh) { 
				n50 = (*ctgset_rit).first; 
				break;
			}
		}
		contig_coverage = DIV(totallength,genome_size);
		out << thresh << " " << n50 << " " << contig_coverage*100.0 << endl;
	}
	out.close();
	
	// Compute Feature-Response for MATEPAIR features
	cout << "- FRC-matepair" << endl;
	out.open("FRC_matepair.txt");
	if (!out.is_open()){
		cerr << "Could not open input feat_matepair.txt" << endl;
		exit(1);
	}
	for (int k = 1; k < 100; k++) {
		double factor = ((double)k/100.0);
		thresh = totalfeatures_M * factor;	
		cum = 0;
		totallength = 0;
		n50 = -1;
		for (ctgset_rit = ctgset.rbegin(); ctgset_rit != ctgset.rend(); ctgset_rit++) {
			//cur = (*ctgset_rit).first;
			cur = ctg_feats_M[(*ctgset_rit).second];
			cum += cur;
			//ctg_bank->fetch((*ctgset_rit).second, ctg);
			totallength += (*ctgset_rit).first;

			if (n50 == -1 && cum >= thresh) { 
				n50 = (*ctgset_rit).first; 
				break;
			}
		}
		contig_coverage = DIV(totallength,genome_size);
		out << thresh << " " << n50 << " " << contig_coverage*100.0 << endl;
	}
	out.close();
	
	// Compute Feature-Response for MISASSEMBLY features
	cout << "- FRC-misassembly" << endl;
	out.open("FRC_misassembly.txt");
	if (!out.is_open()){
		cerr << "Could not open input feat_misassembly.txt" << endl;
		exit(1);
	}
	for (int k = 1; k < 100; k++) {
		double factor = ((double)k/100.0);
		thresh = totalfeatures_A * factor;	
		cum = 0;
		totallength = 0;
		n50 = -1;
		for (ctgset_rit = ctgset.rbegin(); ctgset_rit != ctgset.rend(); ctgset_rit++) {
			//cur = (*ctgset_rit).first;
			cur = ctg_feats_A[(*ctgset_rit).second];
			cum += cur;
			//ctg_bank->fetch((*ctgset_rit).second, ctg);
			totallength += (*ctgset_rit).first;

			if (n50 == -1 && cum >= thresh) { 
				n50 = (*ctgset_rit).first; 
				break;
			}
		}
		contig_coverage = DIV(totallength,genome_size);
		out << thresh << " " << n50 << " " << contig_coverage*100.0 << endl;
	}
	out.close();
			
	close_bank();
	
	cout << "done!" << endl;
}


void FeatureStats::open_bank() {

	feat_bank = new BankStream_t(Feature_t::NCODE);
	ctg_bank = new Bank_t(Contig_t::NCODE);	

	try { // try to open the bank
		if(feat_bank->exists(bankdir)) {
			//cout << "Opening feature bank ... " << bankdir << endl;
			feat_bank->open(bankdir, B_READ);
			ctg_bank->open(bankdir, B_READ);

			if(feat_bank->empty()) {
				cerr << "feature bank is empty" << endl;
				return;
			}	
			if(ctg_bank->empty()) {
				cerr << "contig bank is empty" << endl;
				return;
			}		
		}	
	} catch (Exception_t &e) {
		cerr << "Exception in opening the bank: " << e << endl;
		close_bank();
	}
}

void FeatureStats::close_bank() {
	
	try { // try to pen the bank
		if(feat_bank->exists(bankdir)) {
			//cout << "Closing feature bank ... " << endl;
			feat_bank->close();
			//cout << "bank "<< bankdir << " closed !" << endl;
		}
		if(ctg_bank->exists(bankdir)) {
			//cout << "Closing contig bank ... " << endl;
			ctg_bank->close();
			//cout << "bank "<< bankdir << " closed !" << endl;
		}
	} catch (Exception_t &e) {
		cerr << "Exception in closing the bank: " << e << endl;
	}
}

void FeatureStats::parse_command_line(int argc, char* argv[]) {
	bool errflg = false;
	int ch;

	optarg = NULL;

	while (!errflg && ((ch = getopt(argc, argv, "g:r:l:b:hgvs")) != EOF)) {
		switch  (ch) {
			case 'b' :
			bankdir = optarg;
			break;
			
			case 'g' :
			genome_size = atoi(optarg);
			break;

			case 'v':
			VERBOSE = true;
			break;

			case '?' :
			cerr << "Unrecognized option - " <<  optopt << endl;

			default :
			errflg = true;
		}
	}

	if(bankdir.empty()) {
		cerr << " Need to provide a bankdir name " << endl;
		errflg = true;
	}

	if(errflg) {
		cerr << " Usage: assembler -g <genome size> -b <AMOS Bank> " << endl;
		
		cerr << "Output:" << endl;
		cerr << "\tThe Feature-Response curve (FRC) is saved in file 'FRC.txt', while" << endl;
		cerr << "\tFRCs for each feature type are saved respectively in:" << endl;
		cerr <<	"\t'FRC_coverage.txt', 'FRC_polymorphism.txt', 'FRC_breakpoint.txt'," << endl; 
		cerr <<	"\t'FRC_kmer.txt', 'FRC_matepair.txt' and 'FRC_misassembly.txt'" << endl;

		cerr << "Output file format:" << endl;
		cerr << "Each file contains the FRCs in 3-columns format" << endl;
		cerr << "\t- column 1 = feature threshold T;" << endl;
		cerr << "\t- column 2 = contigs' N50 associated to the threshold T in column 1;" << endl;
		cerr <<	"\t- column 3 = cumulative size of the contigs whose number of features is <= T;" << endl;
		cerr << endl;
				
		exit(EXIT_FAILURE);
	}
	
	return;
}

int main(int argc, char** argv) {
	
	FeatureStats* fs = new FeatureStats();
	fs->parse_command_line(argc, argv);

	if(fs->VERBOSE) {
		cout << " AMOS mode " << endl;
		cout << " use bank: " << fs->bankdir << endl;
	}
	
	fs->computeStats();
}
