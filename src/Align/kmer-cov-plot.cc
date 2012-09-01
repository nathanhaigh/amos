//  A. L. Delcher
//
//  File:  kmer-cov.cc
//
//  Original:  22 March 2004
//
//  Modified by Eric Biggers: 24 June 2012
//     Rewrote most of the code.
//     - Added support for Jellyfish.
//     - Added support for printing only foward kmer coverage.
//     - Allow FASTQ input as well as FASTA.
//     - Skip over invalid DNA characters (N's, etc.).
//  Modified again 26 July 2012
//     - Add support for tabular output format.
//     - Add support for printing the k-mer coverage of only the specified
//       ranges in the input sequences.
//     - Add support for leaving out the tags and/on the indices in the output.

#include "foundation_AMOS.hh"
#include "delcher.hh"
#include "fasta.hh"
#include "fastq.hh"
#include "RangeIterator.h"
#include <getopt.h>
#include <vector>

using std::cerr;
using std::endl;
using std::make_pair;
using std::max;
using std::string;;
using std::vector;


typedef unsigned long long mer_t;

static mer_t       forward_mask;
static unsigned    kmer_len;
static const char *kmer_file_name;
static long        OPT_ends = -1;
static bool        OPT_tabular = false;
static bool        OPT_display_kmers = false;
static bool        OPT_forward_only = false;
static bool        OPT_sum_only = false;
static bool        OPT_display_tags = true;
static bool        OPT_display_indices = true;
#ifdef WITH_JELLYFISH
static bool OPT_jellyfish = false;
static bool OPT_jellyfish_preload = false;
#endif
static RangeIterator range_iterator;

static void usage(const char *prog_name)
{
    static const char *usage_str = 
"Usage: %s KMER_FILE < (FASTA_FILE | FASTQ_FILE)\n"
"Prints the k-mer coverage beginning at each base in a set of DNA sequences.\n"
"\n"
"The set of DNA sequences is read as a FASTA or FASTQ file from standard input.\n"
"KMER_FILE specifies a FASTA file that contains the k-mer counts for\n"
"the set of DNA sequences.  Each FASTA tag line provides a k-mer count,\n"
"and the following sequence line provides the k-mer.  Alternatively,\n"
"KMER_FILE may specify a Jellyfish hash table if the -j option is given.\n"
"\n"
"The k-mer length is determined automatically from the KMER_FILE.\n"
"\n"
"By default, the tag line of sequence is printed, followed\n"
"by a line for each k-mer in the sequence. The first column is the index\n"
"of the k-mer, starting from 1, and the remaining three columns are:\n"
"     (count(fmer) + count(rmer))   count(fmer)   count(rmer)\n"
"where count(fmer) indicates the count of the forward k-mer and count(rmer)\n"
"indicates the count of the reverse k-mer.\n"
"\n"
"Options:\n"
"  -j, --jellyfish      Use k-mer counts from a Jellyfish hash table.\n"
"                       KMER_FILE specifies a hash table produced by the\n"
"                       `jellyfish count' command.  Only available if\n"
"                       compiled with Jellyfish support.  Note: Jellyfish's\n"
"                       hash table is optimized for memory usage and\n"
"                       multithreaded updating, so it is slower than a\n"
"                       normal hash table for lookups.\n"
"\n"
"  -l, --jellyfish-preload\n"
"                       Access the Jellyfish hash table sequentially before\n"
"                       beginning plotting the k-mer coverage.  This can\n"
"                       greatly improve performance on large Jellyfish\n"
"                       files that have not been pre-loaded into the\n"
"                       kernel's buffer cache, as they would otherwise\n"
"                       be accessed randomly and gradually be loaded into\n"
"                       memory by random page faults.  If you do not have\n"
"                       enough memory to hold the entire hash table in memory,\n"
"                       this option will not be very helpful.\n"
"\n"
"  -r, --ranges=RANGE[,RANGE]...\n"
"                       Only print the k-mer coverages at the positions\n"
"                       specified by the comma-separated list of ranges.\n"
"                       Each range is two numbers separated by a colon.\n"
"                       Positive integral numbers are taken to be base\n"
"                       positions numbered starting at 1.  Negative integral\n"
"                       numbers are taken to be base positions from the\n"
"                       end of the sequence (e.g. -1 is the last base in\n"
"                       the sequence).  Floating-point numbers are handled\n"
"                       similarly, but are first multiplied by the number of\n"
"                       k-mers in the sequence.  Examples:\n"
"                       --ranges=1:10,-10:-1 to print the k-mer coverage of\n"
"                       the first and last 10 k-mers of each sequence;\n"
"                       --ranges=0.45:0.55 to print k-mer coverage of the\n"
"                       the middle 10%% of each sequence's k-mers.  The output\n"
"                       is in ascending order of base position and no position\n"
"                       is printed multiple times.\n"
"\n"
"  -t, --tabular        Output the information as one read per line, as the\n"
"                       FASTA/FASTQ tag followed by the k-mer coverage\n"
"                       values, tab-delimited.  The coverage of invalid\n"
"                       k-mers is marked as -1.  Without -f, the sum of\n"
"                       the coverage of the forward and reverse complement\n"
"                       k-mers is printed.  Cannot be used with\n"
"                       --display-kmers.\n"
"\n"
"  -k, --display-kmers  Display the actual k-mer in addition to the counts.\n"
"                         Cannot be used with --tabular.\n"
"  -f, --forward-only   Display count (and k-mer with -k) for the forward k-mer\n"
"                         only.\n"
"  -s, --sum-only       Display only the combined count of the forward and\n"
"                         reverse complement k-mers.  Default with --tabular.\n"
"  -n, --no-tags        Do not print sequence tags.\n"
"  -N, --no-indices     Do not print the index of each k-mer.  Ignored if\n"
"                         --tabular is given.\n"
"  -h, --help           Print this usage message.\n"
    ;
    printf(usage_str, prog_name);
}

static const char *optstring = "jlr:tkfsnNh";
static const struct option longopts[] = {
    {"jellyfish",         no_argument, NULL, 'j'},
    {"jellyfish-preload", no_argument, NULL, 'l'},
    {"ranges",            required_argument, NULL, 'r'},
    {"tabular",           no_argument, NULL, 't'},
    {"display-kmers",     no_argument, NULL, 'k'},
    {"forward-only",      no_argument, NULL, 'f'},
    {"sum-only",          no_argument, NULL, 's'},
    {"no-tags",           no_argument, NULL, 'n'},
    {"no-indices",        no_argument, NULL, 'N'},
    {"help",              no_argument, NULL, 'h'},
    {0, 0, 0, 0},
};

static void parse_command_line(int argc, char *argv[])
{
    int c;
    while ((c = getopt_long(argc, argv, optstring, longopts, NULL)) != -1) {
        switch (c) {
        case 'j':
#ifdef WITH_JELLYFISH
            OPT_jellyfish = true;
#else
            cerr << "ERROR: Jellyfish support not enabled" << endl;
            exit(2);
#endif
            break;
        case 'l':
#ifdef WITH_JELLYFISH
            OPT_jellyfish_preload = true;
#else
            cerr << "ERROR: Jellyfish support not enabled" << endl;
            exit(2);
#endif
            break;
        case 'r':
            if (!range_iterator.load_raw_ranges(optarg)) {
                cerr << "ERROR: invalid range string \"" << optarg
                     << "\"" << endl;
                exit(2);
            }
            break;
        case 't':
            OPT_tabular = true;
            break;
        case 'k':
            OPT_display_kmers = true;
            break;
        case 'f':
            OPT_forward_only = true;
            break;
        case 's':
            OPT_sum_only = true;
            break;
        case 'n':
            OPT_display_tags = false;
            break;
        case 'N':
            OPT_display_indices = false;
            break;
        case 'h':
        default:
            usage(argv[0]);
            exit((c == 'h') ? 0 : 2);
        }
    }

    if (OPT_display_kmers && OPT_tabular) {
        cerr << "ERROR: Cannot mix options -k and -t" << endl;
        exit(2);
    }
    if (OPT_tabular && !OPT_forward_only)
        OPT_sum_only = true;

    argc -= optind - 1;
    argv += optind - 1;
    if (argc != 2) {
        usage(argv[0]);
        exit(2);
    }
    kmer_file_name = argv[1];
}

static unsigned char char_to_bin_tab[256];
static char char_complement_tab[256];
static unsigned char bin_to_char_tab[4];

static void init_tables()
{
    for (unsigned i = 0; i < 256; i++)
        char_to_bin_tab[i] = 4;
    char_to_bin_tab['a'] = 0;
    char_to_bin_tab['A'] = 0;
    char_to_bin_tab['c'] = 1;
    char_to_bin_tab['C'] = 1;
    char_to_bin_tab['g'] = 2;
    char_to_bin_tab['G'] = 2;
    char_to_bin_tab['t'] = 3;
    char_to_bin_tab['T'] = 3;

    bin_to_char_tab[0] = 'A';
    bin_to_char_tab[1] = 'C';
    bin_to_char_tab[2] = 'G';
    bin_to_char_tab[3] = 'T';

    for (unsigned i = 0; i < 256; i++)
        char_complement_tab[i] = i;
    char_complement_tab['a'] = 't';
    char_complement_tab['A'] = 'T';
    char_complement_tab['c'] = 'g';
    char_complement_tab['C'] = 'G';
    char_complement_tab['g'] = 'c';
    char_complement_tab['G'] = 'C';
    char_complement_tab['t'] = 'a';
    char_complement_tab['T'] = 'A';
}

static inline char char_complement(char c)
{
    return char_complement_tab[(unsigned char)c];
}

static inline unsigned char char_to_bin(char c)
{
    return char_to_bin_tab[(unsigned char)c];
}

static inline bool is_dna_char(char c)
{
    return char_to_bin(c) < 4;
}

static inline unsigned char bin_complement(unsigned char c)
{
    return c ^ 3;
}

// Add the Watson-Crick complement of @c to @mer on the left, sliding one
// character off the right end of @mer.
static inline mer_t reverse_add_char(mer_t mer, char c)
{
    mer >>= 2;
    mer |= ((mer_t)(bin_complement(char_to_bin(c))) << (2 * kmer_len - 2));
    return mer;
}

// Add @c to @mer on the right, sliding one character off the left end of mer.
static inline mer_t forward_add_char(mer_t mer, char c)
{
    mer &= forward_mask;
    mer <<= 2;
    mer |= char_to_bin(c);
    return mer;
}

// Returns the binary equivalent of the kmer @s of length no greater than 31,
// given in ASCII format.
static mer_t ascii_to_mer(const char *s)
{
    mer_t mer = 0;
    while (*s) {
        if (!is_dna_char(*s)) {
            fprintf(stderr, "ERROR: in file \"%s\": \"%s\" is not a valid "
                            "k-mer!\n", kmer_file_name, s);
            exit(1);
        }
        mer <<= 2;
        mer |= char_to_bin(*s++);
    }
    return mer;
}

// Interface for a class that maps kmers to counts.
class mer_table_t {

protected:
    unsigned m_kmer_len;
    void read_kmers(const char *kmer_file_name);

public:
    // Look up a kmer and return its count.
    virtual unsigned long operator[](mer_t mer) const = 0;

    // Insert a (kmer, count) pair into the map.
    virtual void insert(mer_t mer, unsigned long count) { }

    // Returns the length of the kmers in the map.
    virtual unsigned get_kmer_len() const { 
        return m_kmer_len; 
    }
};

// Read kmers and counts from file kmer_file_name and insert them into the mer
// table. Input format is a multi-fasta file.  The tag line for each kmer must
// give its count.  For example:
//
// >12
// ACTTCGTC
// >7
// TTTTAGTC
void mer_table_t::read_kmers(const char *kmer_file_name)
{
    string s, tag;
    FILE *fp = File_Open(kmer_file_name, "r", __FILE__, __LINE__);
    char *p;
    unsigned long mer_count;
    while (Fasta_Read(fp, s, tag)) {
        if (m_kmer_len == 0) {
            m_kmer_len = s.length();
        } else if (m_kmer_len != s.length()) {
            fprintf(stderr, "ERROR: The k-mer \"%s\" has length %u "
                            "(expected %u)!\n",
                            s.c_str(), s.length(), m_kmer_len);
            exit(1);
        }
        mer_t mer = ascii_to_mer(s.c_str());
        mer_count = strtoul(tag.c_str(), &p, 10);
        if (p == tag.c_str()) {
            fprintf(stderr, "ERROR: k-mer count \"%s\" is invalid!\n",
                    tag.c_str());
            exit(1);
        }
        this->insert(mer, mer_count);
    }
    fclose(fp);
}

typedef HASHMAP::hash_map <mer_t, unsigned long, HASHMAP::hash <unsigned long> >
        hash_map;

// mer_table_t implementation that uses std::hash_map.
class hash_map_mer_table_t : public mer_table_t {

private:
    hash_map map;

public:
    hash_map_mer_table_t(const char *filename) {
        this->read_kmers(filename);
    }

    unsigned long operator[](mer_t mer) const {
        hash_map::const_iterator it = map.find(mer);
        if (it != map.end())
            return it->second;
        else
            return 0;
    }

    void insert(mer_t mer, unsigned long count) {
        map.insert(make_pair(mer, count));
    }
};

#ifdef WITH_JELLYFISH

#include <jellyfish/compacted_hash.hpp>
#include <jellyfish/mer_counting.hpp>

// Assert that jellyfish is using the expected kmer representation
static void jellyfish_check()
{
    const char *test_kmer = "AGTCCTTGCTTAGCATGCCG";
    mer_t mer1 = ascii_to_mer(test_kmer);
    mer_t mer2 = jellyfish::parse_dna::mer_string_to_binary(test_kmer, 
                                                        strlen(test_kmer));
    if (mer1 != mer2) {
        cerr << "Jellyfish doesn't seem to be using the expected kmer "
                    "representation!  Aborting." << endl;
        exit(1);
    }
}

// mer_table_t implementation that uses one of the hash tables from Jellyfish.
template <typename jellyfish_hash_t>
class jellyfish_mer_table_t : public mer_table_t {

private:
    jellyfish_hash_t map;

public:
    jellyfish_mer_table_t(mapped_file &file) :
        map(file) { jellyfish_check(); }

    unsigned long operator[](mer_t mer) const {
        return map[mer];
    }

    unsigned get_kmer_len() const {
        return map.get_mer_len();
    }
};
#endif // WITH_JELLYFISH


static void print_kmers(const char *kmer, bool forward_only)
{
    fwrite(kmer, 1, kmer_len, stdout);
    if (!forward_only) {
        unsigned long len = kmer_len;
        const char *p = kmer + len - 1;
        putchar('\t');
        while (len--)
            putchar(char_complement(*p--));
    }
}

// Print the kmer_len-mer coverage in the sequence @seq at the k-mers starting
// at 1-based indices in the range [@start, @end].
static void kmer_cov_plot_range(const char *seq, long start, long end,
                                const mer_table_t &mer_table,
                                bool is_first_range)
{
    mer_t fmer = 0;
    mer_t rmer = 0;

    long next_good_pos = 1;

    long first_kmer_end_pos = start + kmer_len - 1;
    for (long i = start; i < first_kmer_end_pos; i++) {
        unsigned char base = seq[i - 1];
        if (is_dna_char(base)) { 
            fmer = forward_add_char(fmer, base);
            rmer = reverse_add_char(rmer, base);
        } else {
            next_good_pos = i + 1;
        }
    }
    for (long i = start; i <= end; i++) {

        long kmer_end_pos  = i + kmer_len - 1;

        unsigned char base = seq[kmer_end_pos - 1];

        if (is_dna_char(base)) { 
            fmer = forward_add_char(fmer, base);
            rmer = reverse_add_char(rmer, base);
        } else {
            next_good_pos = kmer_end_pos + 1;
        }

        long fcount;
        long rcount;
        long count;

        if (i < next_good_pos) {
            fcount = -1;
            rcount = -1;
            count = -1;
        } else {
            fcount = mer_table[fmer];
            if (!OPT_forward_only) {
                rcount = mer_table[rmer];
                count = fcount + rcount;
            }
        }

        if (OPT_tabular) {
            if (OPT_display_tags || !is_first_range || i != start)
                putchar('\t');
        } else {
            if (OPT_display_indices)
                printf("%ld\t", i);
        }

        if (OPT_forward_only)
            printf("%ld", fcount);
        else if (OPT_sum_only)
            printf("%ld", count);
        else
            printf("%ld\t%ld\t%ld", count, fcount, rcount);

        if (!OPT_tabular) {
            if (OPT_display_kmers) {
                putchar('\t');
                print_kmers(&seq[i - 1], OPT_forward_only);
            }
            putchar('\n');
        }
    }
}

static void kmer_cov_plot_seq(const string &seq, const mer_table_t &mer_table)
{
    range_iterator.begin_sequence(seq.c_str(), seq.length() - (long)kmer_len + 1);
    const Range *range;
    bool is_first_range = true;
    while ((range = range_iterator.next())) {
        kmer_cov_plot_range(&seq[0], range->start, range->end,
                            mer_table, is_first_range);
        is_first_range = false;
    }
    if (OPT_tabular)
        putchar('\n');
}


int main(int argc, char *argv[])
{
    parse_command_line(argc, argv);
    init_tables();

    mer_table_t *mer_table;
#ifdef WITH_JELLYFISH
    if (OPT_jellyfish) {

        mapped_file dbf(kmer_file_name);
        if (OPT_jellyfish_preload)
            dbf.load();
        if (strncmp(dbf.base(), jellyfish::compacted_hash::file_type, 8) == 0) {
            mer_table = new jellyfish_mer_table_t<hash_query_t>(dbf);
        } else if (strncmp(dbf.base(), jellyfish::raw_hash::file_type, 8) == 0) {
            mer_table = new jellyfish_mer_table_t<raw_inv_hash_query_t>(dbf);
        } else {
            cerr << kmer_file_name 
                      << " is not a valid jellyfish hash table"
                      << endl;
            exit(1);
        }
    } else {
#endif
        mer_table = new hash_map_mer_table_t(kmer_file_name);
#ifdef WITH_JELLYFISH
    }
#endif
    kmer_len = mer_table->get_kmer_len();
    if (kmer_len <= 0 || kmer_len > 31) {
        cerr << "ERROR: Cannot use kmer length of " << kmer_len << "!" << endl;
        exit(1);
    }
    forward_mask = ((mer_t)1 << (2 * kmer_len - 2)) - 1;

    int c = getchar();
    switch (c) {
    case '>': {
            string s, tag;
            ungetc(c, stdin);
            while (Fasta_Read(stdin, s, tag)) {
                if (OPT_display_tags) {
                    putchar('>');
                    fputs(tag.c_str(), stdout);
                    if (!OPT_tabular)
                        putchar('\n');
                }
                kmer_cov_plot_seq(s, *mer_table);
            }
            break;
        }
    case '@': {
            string s, hdr, q, qualHdr;
            ungetc(c, stdin);
            while (Fastq_Read(stdin, s, hdr, q, qualHdr)) {
                if (OPT_display_tags) {
                    putchar('@');
                    fputs(hdr.c_str(), stdout);
                    if (!OPT_tabular)
                        putchar('\n');
                }
                kmer_cov_plot_seq(s, *mer_table);
            }
            break;
        }
    default:
        cerr << "ERROR: Unrecognized file type on stdin (expected FASTA or "
                     "FASTQ file)!" << endl;
        return 1;
    }
    return 0;
}
