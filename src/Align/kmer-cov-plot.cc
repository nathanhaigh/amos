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

#include "foundation_AMOS.hh"
#include "delcher.hh"
#include "fasta.hh"
#include "fastq.hh"
#include <getopt.h>

typedef unsigned long long mer_t;

static mer_t       filled_mask = (mer_t)1 << (8 * sizeof(mer_t) - 1);
static mer_t       forward_mask;
static unsigned    kmer_len = 0;
static const char *kmer_file_name;
static bool        OPT_display_kmers = false;
static bool        OPT_forward_only = false;

/* Map a DNA character, upper case or lower case, to the binary code
 * { A => 0, C => 1, G => 2, C => 3 }.
 *
 * C99 designated initializers do not exist in C++, too bad...
 */
static const unsigned char char_to_binary[256] = {
        4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
        4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
        4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
        4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
        4, /* A */ 0, 4, /* C */ 1, 4, 4, 4, /* G */ 2,
                4, 4, 4, 4, 4, 4, /* N */ 4, 4,
        4, 4, 4, 4, /* T */ 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
        4, /* a */ 0, 4, /* c */ 1, 4, 4, 4, /* g */ 2,
                4, 4, 4, 4, 4, 4, /* n */ 4, 4,
        4, 4, 4, 4, /* t */ 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
        4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
        4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
        4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
        4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
        4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
        4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
        4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
        4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
};


static const unsigned char binary_to_char[4] = {
    'A', 'C', 'G', 'T',
};

static inline bool is_dna_char(char c)
{
    return char_to_binary[(unsigned char)c] < 4;
}

// Add the Watson-Crick complement of @c to @mer on the left, sliding one
// character off the right end of @mer.
static inline mer_t reverse_add_char(mer_t mer, unsigned char c)
{
    mer >>= 2;
    mer |= ((mer_t)(3 ^ char_to_binary[c]) << (2 * kmer_len - 2));
    return mer;
}

// Add @c to @mer on the right, sliding one character off the left end of mer.
static inline mer_t forward_add_char(mer_t mer, unsigned char c)
{
    mer &= forward_mask;
    mer <<= 2;
    mer |= char_to_binary[c];
    return mer;
}

// Returns the binary equivalent of the kmer @s of length no greater than 31,
// given in ASCII format.
static inline mer_t ascii_to_mer(const char *s)
{
    mer_t mer = 0;
    while (*s) {
        mer <<= 2;
        mer |= char_to_binary[*s++];
    }
    return mer;
}

// Translates the kmer @mer into ASCII format, writing the result into @s.  @s
// must point to an array of length at least (kmer_len + 1) bytes.
static inline void mer_to_ascii(mer_t mer, char *s)
{
    char *p = s + kmer_len;
    *p-- = '\0';
    while (p >= s) {
        *p-- = binary_to_char[mer & 0x3];
        mer >>= 2;
    }
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

    // Return the number of entries in the map.
    virtual size_t size() const = 0;

    // Returns the length of the kmers in the map.
    virtual unsigned long get_kmer_len() const { 
        return m_kmer_len; 
    }
};

// Read kmers and counts from file kmer_file_name and insert them into the mer
// table. Input format is a multi-fasta file.  Mers are assumed to contain only
// ACGT's. The tag line for each kmer must give its count.  For example:
//
// >12
// ACTTCGTC
// >7
// TTTTAGTC
void mer_table_t::read_kmers(const char *kmer_file_name)
{
    std::string s, tag;
    FILE *fp = File_Open(kmer_file_name, "r", __FILE__, __LINE__);
    while (Fasta_Read(fp, s, tag)) {
        if (m_kmer_len == 0) {
            m_kmer_len = s.length();
        } else if (m_kmer_len != s.length()) {
            sprintf(Clean_Exit_Msg_Line,
                "The k-mer \"%s\" has length %u (expected %u)!",
                s.c_str(), s.length(), m_kmer_len);
            Clean_Exit(Clean_Exit_Msg_Line, __FILE__, __LINE__);
        }
        mer_t mer = ascii_to_mer(s.c_str());
        unsigned long mer_count = strtoul(tag.c_str(), NULL, 10);
        this->insert(mer, mer_count);
    }
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
        map.insert(std::make_pair(mer, count));
    }

    size_t size() const {
        return map.size();
    }
};

#ifdef WITH_JELLYFISH

#include <jellyfish/compacted_hash.hpp>
#include <jellyfish/mer_counting.hpp>
static bool OPT_jellyfish = false;

// Assert that jellyfish is using the expected kmer representation
static void jellyfish_check()
{
    const char *test_kmer = "AGTCCTTGCTTAGCATGCCG";
    mer_t mer1 = ascii_to_mer(test_kmer);
    mer_t mer2 = jellyfish::parse_dna::mer_string_to_binary(test_kmer, 
                                                        strlen(test_kmer));
    if (mer1 != mer2) {
        std::cerr << "Jellyfish doesn't seem to be using the expected kmer "
                    "representation!  Aborting." << std::endl;
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

    size_t size() const {
        return map.get_size();
    }

    size_t get_kmer_len() const {
        return map.get_mer_len();
    }
};
#endif // WITH_JELLYFISH


static void usage(const char *prog_name)
{
    static const char *usage_str = 
"Usage: %s KMER_FILE < (FASTA_FILE | FASTQ_FILE)\n"
"Prints the k-mer coverage at each base in a read set or genome.\n"
"\n"
"The read set or genome is read as a FASTA or FASTQ file from standard input.\n"
"KMER_FILE specifies a FASTA file that contains the k-mer counts for\n"
"the read set or genome.  Each FASTA tag line provides a k-mer count,\n"
"and the following sequence line provides the k-mer.  Alternatively,\n"
"KMER_FILE may specify a Jellyfish hash table if the -j option is given.\n"
"\n"
"Options:\n"
"  -K, --display-kmers  Display the actual kmer in addtion to the counts.\n"
"  -j, --jellyfish      Use kmer counts from a jellyfish hash table.\n"
"                         KMER_FILE specifies a hash table produced by the\n"
"                         `jellyfish count' command.  Only available if\n"
"                         compiled with jellyfish support.  Note: jellyfish's\n"
"                         hash table is optimized for memory usage and\n"
"                         multithreaded updating, so it is slower than a\n"
"                         normal hash table for normal lookups.\n"
"  -f, --forward-only   Display count (and kmer with -K) for the forward kmer\n"
"                         only (default is to show forward, reverse complement,\n"
"                         and max coverage, and both forward and reverse kmers\n"
"                         with -K)\n"
"  -h, --help           Print this usage message.\n"
    ;
    fprintf(stderr, usage_str, prog_name);
}

static const struct option longopts[] = {
    { "display-kmers", no_argument, NULL, 'K'},
    { "jellyfish",     no_argument, NULL, 'j'},
    { "forward-only",  no_argument, NULL, 'f'},
    { "help",          no_argument, NULL, 'h'},
    { 0, 0, 0, 0},
};

static void parse_command_line(int argc, char *argv[])
{
    int c;
    while ((c = getopt_long(argc, argv, "hKjf", longopts, NULL)) != -1) {
        switch (c) {
        case 'K':
            OPT_display_kmers = true;
            break;

        case 'j':
#ifdef WITH_JELLYFISH
            OPT_jellyfish = true;
#else
            std::cerr << "ERROR: Jellyfish support not enabled" << std::endl;
            exit(1);
#endif
            break;
        case 'f':
            OPT_forward_only = true;
            break;
        case 'h':
        default:
            usage(argv[0]);
            exit((c == 'h') ? 0 : 1);
        }
    }

    argc -= optind - 1;
    argv += optind - 1;
    if (argc != 2) {
        usage(argv[0]);
        exit(1);
    }
    kmer_file_name = argv[1];
}

static void print_kmer_coverage(const std::string & s,
                                const mer_table_t & mer_table)
{
    unsigned n = s.length();
    unsigned i, j;

    if (n < kmer_len) {
        for (i = 0; i < n; i++)
            printf("%d 0 0\n", i);
        return;
    }

    mer_t fwd_mer = 0;
    mer_t rev_mer = 0;
    unsigned skip_until = kmer_len - 1;

    for (i = 0; ; ) {
        while (1) {
            /* Continue adding bases to the mer until a full k-mer, with no
	     * intervening invalid k-mers, has been finished. */
            if (i == n)
                return;
            if (is_dna_char(s[i])) {
                fwd_mer = forward_add_char(fwd_mer, s[i]);
                rev_mer = reverse_add_char(rev_mer, s[i]);
            } else {
                skip_until = i + kmer_len;
            }
            i++;
            if (i > skip_until)
                break;
            if (i >= kmer_len)
                printf("%u\t(invalid)\n", i - kmer_len + 1);
        }
        skip_until = i;

        unsigned long fcount = mer_table[fwd_mer];

	/* Print the k-mer coverage at the current base in the sequence. */
        if (OPT_forward_only) {
            printf("%u\t%lu", i - kmer_len + 1, fcount);
        } else {
            unsigned long rcount = mer_table[rev_mer];
            unsigned long mcount = std::max(fcount, rcount);
            printf("%u\t%lu\t%lu\t%lu", i - kmer_len + 1, mcount, 
                   fcount, rcount);
        }

        if (OPT_display_kmers) {
            char mer[kmer_len + 1];
            mer_to_ascii(fwd_mer, mer);

            putchar('\t');
            fputs(mer, stdout);
            if (!OPT_forward_only) {
                mer_to_ascii(rev_mer, mer);
                putchar('\t');
                fputs(mer, stdout);
            }
        }
        putchar('\n');
        fflush(stdout);
    }
}



int main(int argc, char *argv[])
{
    parse_command_line(argc, argv);

    mer_table_t *mer_table;
#ifdef WITH_JELLYFISH
    if (OPT_jellyfish) {
        mapped_file dbf(kmer_file_name);
        if (strncmp(dbf.base(), jellyfish::compacted_hash::file_type, 8) == 0) {
            mer_table = new jellyfish_mer_table_t<hash_query_t>(dbf);
        } else if (strncmp(dbf.base(), jellyfish::raw_hash::file_type, 8) == 0) {
            mer_table = new jellyfish_mer_table_t<raw_inv_hash_query_t>(dbf);
        } else {
            std::cerr << kmer_file_name 
                      << " is not a valid jellyfish hash table"
                      << std::endl;
            exit(1);
        }
    } else {
#endif
        mer_table = new hash_map_mer_table_t(kmer_file_name);
#ifdef WITH_JELLYFISH
    }
#endif
    kmer_len = mer_table->get_kmer_len();
    if (kmer_len == 0) {
        std::cerr << "Cannot use kmer length of 0!" << std::endl;
        exit(1);
    }
    forward_mask = ((mer_t)1 << (2 * kmer_len - 2)) - 1;

    int c = getchar();
    switch (c) {
    case '>': {
            std::string s, tag;
            ungetc(c, stdin);
            while (Fasta_Read(stdin, s, tag)) {
                std::cout << ">" << tag << std::endl;
                print_kmer_coverage(s, *mer_table);
            }
        }
    case '@': {
            std::string s, hdr, q, qualHdr;
            ungetc(c, stdin);
            while (Fastq_Read(stdin, s, hdr, q, qualHdr)) {
                std::cout << ">" << hdr << std::endl;
                print_kmer_coverage(s, *mer_table);
            }
        }
    default:
        std::cerr << "Unrecognized file type on stdin (expected FASTA or "
                     "FASTQ file)!" << std::endl;
        return 1;
    }
    return 0;
}
