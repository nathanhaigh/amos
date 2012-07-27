//  File:  gc-content-plot.cc
//
//  Date of First Version:  25 July 2012
//
//  Author: Eric Biggers
//

#include "fasta.hh"
#include "fastq.hh"
#include "RangeIterator.h"
#include <getopt.h>

using std::string;
using std::cerr;
using std::endl;

static long   OPT_window_size     = 180;
static bool   OPT_tabular         = false;
static bool   OPT_percent         = false;
static bool   OPT_include_n       = false;
static double OPT_n_gc_content    = 0.5;
static bool   OPT_display_tags    = true;
static bool   OPT_display_indices = true;
static RangeIterator range_iterator;

static void usage(const char *prog_name)
{
    static const char *usage_str = 
"Usage: %s [OPTIONS] [(FASTA_FILE | FASTQ_FILE)]...\n"
"\n"
"Prints the GC content within a sliding window of configurable size in each\n"
"DNA sequence in each input FASTA_FILE or FASTQ_FILE.  If no FASTA_FILE's or\n"
"FASTQ_FILE's are specified, a FASTA or FASTQ file is read from standard input.\n"
"\n"
"Without the `--include-n' option, windows containing unknown base pairs are\n"
"marked as having a GC content of -1.\n"
"\n"
"In the default output format (`--tabular' not given), the first column is the\n"
"index of the start of the window, where the bases in each sequence are\n"
"numbered beginning at 1.  The number in the second column is the GC content\n"
"of that window, either as an absolute number (default) or as a percent\n"
"(with `--percent').\n"
"\n"
"Options:\n"
"  -w, --window-size=SIZE\n"
"                       Use a sliding window of size SIZE to calculate GC\n"
"                         content.  Default: 180\n"
"  -p, --percent        Print GC contents as percentages rather than base counts.\n"
"  -i, --include-n      Include windows that contain unknown sequence (usually\n"
"                         marked as the letter 'N')\n"
"  -g, --n-gc-content=VALUE\n"
"                       Assume that unknown bases (usually marked as the\n"
"                         letter 'N') have a GC-to-AT ratio of VALUE.  This\n"
"                         option implies `--include-n'.  Default: 0.5\n"
"  -t, --tabular        Use a tabular output format.\n" 
"  -r, --ranges=RANGE[,RANGE]...\n"
"                       Only print the GC content at the windows beginning at\n"
"                       the positions specified by the comma-separated list of\n"
"                       ranges.  Each range is two numbers separated by a colon.\n"
"                       Positive integral numbers are taken to be base\n"
"                       positions numbered starting at 1.  Negative integral\n"
"                       numbers are taken to be base positions from the\n"
"                       end of the sequence (e.g. -1 is the last base in\n"
"                       the sequence).  Floating-point numbers are handled\n"
"                       similarly, but are first multiplied by the number of\n"
"                       GC windows in the sequence.  Examples:\n"
"                       --ranges=1:10,-10:-1 to print the GC content of\n"
"                       the first and last 10 windows of each sequence;\n"
"                       --ranges=0.45:0.55 to print GC content of the\n"
"                       the middle 10%% of each sequence's windows.  The output\n"
"                       is in ascending order of base position and no position\n"
"                       is printed multiple times.\n"
"\n"
"  -n, --no-tags        Do not print sequence tags.\n"
"  -N, --no-indices     Do not print the index of each GC window.  Ignored if\n"
"                         --tabular is given.\n"
"  -h, --help           Print this usage message.\n"
    ;
    printf(usage_str, prog_name);
}

static const char *optstring = "w:pig:tr:nNh";
static const struct option longopts[] = {
    {"window-size",  required_argument, NULL, 'w'},
    {"percent",      no_argument,       NULL, 'p'},
    {"include-n",    no_argument,       NULL, 'i'},
    {"n-gc-content", required_argument, NULL, 'g'},
    {"tabular",      no_argument,       NULL, 't'},
    {"ranges",       required_argument, NULL, 'r'},
    {"no-tags",      no_argument,       NULL, 'n'},
    {"no-indices",   no_argument,       NULL, 'N'},
    {"help",         no_argument,       NULL, 'h'},
    {NULL, 0, NULL, 0}
};

static double gc_count_tab[256];
static bool is_valid_dna_char_tab[256];

static void init_tables()
{
    for (int i = 0; i < 256; i++)
        is_valid_dna_char_tab[i] = OPT_include_n;
    is_valid_dna_char_tab['a'] = true;
    is_valid_dna_char_tab['A'] = true;
    is_valid_dna_char_tab['c'] = true;
    is_valid_dna_char_tab['C'] = true;
    is_valid_dna_char_tab['g'] = true;
    is_valid_dna_char_tab['G'] = true;
    is_valid_dna_char_tab['t'] = true;
    is_valid_dna_char_tab['T'] = true;

    double n_gc_content;
    if (OPT_include_n)
        n_gc_content = OPT_n_gc_content;
    else
        n_gc_content = 0.0;
    for (int i = 0; i < 256; i++)
        gc_count_tab[i] = n_gc_content;
    gc_count_tab['a'] = 0.0;
    gc_count_tab['A'] = 0.0;
    gc_count_tab['t'] = 0.0;
    gc_count_tab['T'] = 0.0;
    gc_count_tab['c'] = 1.0;
    gc_count_tab['C'] = 1.0;
    gc_count_tab['g'] = 1.0;
    gc_count_tab['G'] = 1.0;
    if (OPT_percent)
        for (int i = 0; i < 256; i++)
            gc_count_tab[i] = gc_count_tab[i] / OPT_window_size * 100.0;
}

// Print the GC content at OPT_window_size-sized windows in the sequence @seq in
// the windows starting at 1-based indices in the range [@start, @end].
static void gc_content_plot_range(const char *seq, long start, long end,
                                  bool is_first_range)
{
    double gc_count = 0.0;

    long next_good_pos = 1;

    long first_window_end_pos = start + OPT_window_size - 1;
    for (long i = start; i < first_window_end_pos; i++) {
        unsigned char base = seq[i - 1];
        if (is_valid_dna_char_tab[base]) {
            gc_count += gc_count_tab[base];
        } else {
            next_good_pos = i + 1;
            gc_count = 0.0;
        }
    }

    for (long i = start; i <= end; i++) {

        long window_end_pos = i + OPT_window_size - 1;
        unsigned char base = seq[window_end_pos - 1];

        if (is_valid_dna_char_tab[base]) {
            gc_count += gc_count_tab[base];
        } else {
            next_good_pos = window_end_pos + 1;
            gc_count = 0.0;
        }

        double output_gc_count;

        if (i < next_good_pos) {
            output_gc_count = -1.0;
        } else {
            output_gc_count = gc_count;
            gc_count -= gc_count_tab[seq[i - 1]];
        }

        if (OPT_tabular) {
            if (OPT_display_tags || !is_first_range || i != start)
                putchar('\t');
        } else {
            if (OPT_display_indices)
                printf("%ld\t", i);
        }
        printf("%g", output_gc_count);
        if (!OPT_tabular)
            putchar('\n');
    }
}

// Print the GC content at OPT_window_size-sized windows in the sequence @seq in
// subranges specified by the @range_iterator object.
static void gc_content_plot_seq(const string &seq, RangeIterator &range_iterator)
{
    range_iterator.begin_sequence(seq.c_str(), seq.length() - OPT_window_size + 1);
    const Range *range;
    bool is_first_range = true;
    while ((range = range_iterator.next())) {
        gc_content_plot_range(&seq[0], range->start, range->end,
                              is_first_range);
        is_first_range = false;
    }
    if (OPT_tabular)
        putchar('\n');
}

static void gc_content_plot_file(FILE *in, const char *filename)
{
    int c = getc(in);
    ungetc(c, in);
    switch (c) {
    case '>': {
            string s, tag;
            while (Fasta_Read(in, s, tag)) {
                if (OPT_display_tags) {
                    putchar('>');
                    fputs(tag.c_str(), stdout);
                    if (!OPT_tabular)
                        putchar('\n');
                }
                gc_content_plot_seq(s, range_iterator);
            }
            break;
        }
    case '@': {
            string s, hdr, q, qualHdr;
            while (Fastq_Read(in, s, hdr, q, qualHdr)) {
                if (OPT_display_tags) {
                    putchar('@');
                    fputs(hdr.c_str(), stdout);
                    if (!OPT_tabular)
                        putchar('\n');
                }
                gc_content_plot_seq(s, range_iterator);
            }
            break;
        }
    default:
        fprintf(stderr, "ERROR: \"%s\": Unrecognized file type (expected "
                        "FASTA or FASTQ file)\n", filename);
        exit(1);
    }
}

int main(int argc, char *argv[])
{
    int c;
    char *tmp;
    while ((c = getopt_long(argc, argv, optstring, longopts, NULL)) != -1) {
        switch (c) {
        case 'w':
            OPT_window_size = strtol(optarg, &tmp, 10);
            if (tmp == optarg || OPT_window_size < 1) {
                cerr << "ERROR: the '-w' option must be given a positive "
                        "integer value!" << endl;
                exit(2);
            }
            break;
        case 'p':
            OPT_percent = true;
            break;
        case 'i':
            OPT_include_n = true;
            break;
        case 'g':
            OPT_n_gc_content = strtod(optarg, &tmp);
            if (tmp == optarg || OPT_n_gc_content < 0.0 || OPT_n_gc_content > 1.0) {
                cerr << "ERROR: the '-N' option must have a floating-point "
                        "argument in the interval [0.0, 1.0]" << endl;
                exit(2);
            }
            OPT_include_n = true;
            break;
        case 't':
            OPT_tabular = true;
            break;
        case 'r':
            if (!range_iterator.load_raw_ranges(optarg)) {
                cerr << "ERROR: invalid range string \"" << optarg
                     << "\"" << endl;
                exit(2);
            }
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
            exit((c == 'h') ? 0 : 1);
        }
    }

    argc -= optind;
    argv += optind;
    init_tables();
    if (argc == 0) {
        gc_content_plot_file(stdin, "stdin");
    } else {
        for (int i = 0; i < argc; i++) {
            FILE *fp;
            const char *filename;
            if (strcmp(argv[i], "-") == 0) {
                fp = stdin;
                filename = "stdin";
            } else {
                filename = argv[i];
                fp = File_Open(filename, "r", __FILE__, __LINE__);
            }
            gc_content_plot_file(fp, filename);
            fclose(fp);
        }
    }
    return 0;
}
