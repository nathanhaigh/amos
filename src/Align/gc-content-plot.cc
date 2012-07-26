//  File:  gc-content-plot.cc
//
//  Date of First Version:  25 July 2012
//
//  Author: Eric Biggers
//

#include "fasta.hh"
#include "fastq.hh"
#include <getopt.h>

static long OPT_window_size    = 180;
static long OPT_ends           = -1;
static bool OPT_tabular        = false;
static bool OPT_percent        = false;
static bool OPT_include_n      = false;
static double OPT_n_gc_content = 0.5;

static void usage(const char *prog_name)
{
    static const char *usage_str = 
"Usage: %s [OPTIONS] [(FASTA_FILE | FASTQ_FILE)]...\n"
"\n"
"Prints the GC content within a sliding window of configurable size in each\n"
"DNA sequence in each input FASTA_FILE or FASTQ file.  If no FASTA_FILE's or\n"
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
"  -n, --include-n      Include windows that contain unknown sequence (usually\n"
"                         marked as the letter 'N')\n"
"  -N, --n-gc-content=VALUE\n"
"                       Assume that unknown bases (usually marked as the\n"
"                         letter 'N') have a GC-to-AT ratio of VALUE.  This\n"
"                         option implies `--include-n'.  Default: 0.5\n"
"  -t, --tabular        Use a tabular output format.\n" 
"  -e, --ends=LEN       Only print the GC content at the first and last LEN\n"
"                         windows of each read.\n"
"  -h, --help           Print this usage message.\n"
    ;
    printf(usage_str, prog_name);
}

static const char *optstring = "w:pnN:te:h";
static const struct option longopts[] = {
    {"window-size",  required_argument, NULL, 'w'},
    {"percent",      no_argument,       NULL, 'p'},
    {"include-n",    no_argument,       NULL, 'n'},
    {"n-gc-content", required_argument, NULL, 'N'},
    {"tabular",      no_argument,       NULL, 't'},
    {"ends",         required_argument, NULL, 'e'},
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

    for (int i = 0; i < 256; i++)
        gc_count_tab[i] = OPT_n_gc_content;
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

static void gc_content_plot_seq(const std::string &seq)
{
    double gc_count = 0.0;
    long last_bad_idx = -1;
    for (long i = 0; i < seq.size(); i++) {
        unsigned char base = seq[i];
        long window_start_idx = i - OPT_window_size + 1;
        if (is_valid_dna_char_tab[base]) {
            gc_count += gc_count_tab[base];
        } else {
            last_bad_idx = i;
            gc_count = 0.0;
        }
        if (window_start_idx <= last_bad_idx) {
            if (window_start_idx >= 0 && (OPT_ends == -1 || 
                                 window_start_idx < OPT_ends ||
                                 window_start_idx >= seq.size() - OPT_ends)) 
            {
                if (OPT_tabular)
                    fputs("\t-1", stdout);
                else
                    printf("%d\t-1\n", window_start_idx + 1);
            }
        } else {
            if (window_start_idx > 0) {
                char out_of_scope_base = seq[window_start_idx - 1];
                if (is_valid_dna_char_tab[out_of_scope_base])
                    gc_count -= gc_count_tab[out_of_scope_base];
            }
            if (OPT_ends == -1 || window_start_idx < OPT_ends ||
                                  i >= seq.size() - OPT_ends)
            {
                if (OPT_tabular)
                    printf("\t%g", gc_count);
                else
                    printf("%ld\t%g\n", window_start_idx + 1, gc_count);
            }
        }
    }
    if (OPT_tabular)
        putchar('\n');
}

static void gc_content_plot_file(FILE *in)
{
    int c = getc(in);
    ungetc(c, in);
    switch (c) {
    case '>': {
            std::string s, tag;
            while (Fasta_Read(in, s, tag)) {
                putchar('>');
                fputs(tag.c_str(), stdout);
                if (!OPT_tabular)
                    putchar('\n');
                gc_content_plot_seq(s);
            }
            break;
        }
    case '@': {
            std::string s, hdr, q, qualHdr;
            while (Fastq_Read(in, s, hdr, q, qualHdr)) {
                putchar('@');
                fputs(hdr.c_str(), stdout);
                if (!OPT_tabular)
                    putchar('\n');
                gc_content_plot_seq(s);
            }
            break;
        }
    default:
        fprintf(stderr, "\"%s\" has an unexpected(expected a FASTA or FASTQ file)!\n");
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
                std::cerr << "ERROR: '-w' option must be given a positive "
                             "integer value!" << std::endl;
                exit(2);
            }
            break;
        case 'p':
            OPT_percent = true;
            break;
        case 'n':
            OPT_include_n = true;
            break;
        case 'N':
            OPT_n_gc_content = strtod(optarg, &tmp);
            if (tmp == optarg || OPT_n_gc_content < 0.0 || OPT_n_gc_content > 1.0) {
                std::cerr << "ERROR: '-N' option must have a floating-point "
                            "argument in the interval [0.0, 1.0]" << std::endl;
                exit(2);
            }
            OPT_include_n = true;
            break;
        case 't':
            OPT_tabular = true;
            break;
        case 'e':
            OPT_ends = strtol(optarg, &tmp, 10);
            if (tmp == optarg || OPT_ends < 1) {
                std::cerr << "ERROR: '-e' option must be given a positive "
                             "integer value!" << std::endl;
                exit(2);
            }
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
        gc_content_plot_file(stdin);
    } else {
        for (int i = 0; i < argc; i++) {
            FILE *fp;
            if (strcmp(argv[i], "-") == 0)
                fp = stdin;
            else
                fp = File_Open(argv[i], "r", __FILE__, __LINE__);
            gc_content_plot_file(fp);
            fclose(fp);
        }
    }
    return 0;
}
