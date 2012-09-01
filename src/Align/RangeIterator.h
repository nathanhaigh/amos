#ifndef _SEQUENCE_RANGE_H
#define _SEQUENCE_RANGE_H

#include <vector>
#include <string>

struct Range {
    long start;
    long end;
};

// This class is used in the kmer-cov-plot and gc-content-plot programs.  It is
// initialized with a string that is a comma-separated list string ranges, where
// each range is two numbers separated with a colon; each number may be an index
// from the start of a sequence (positive integers),  an index from the end of a
// sequence (negative integers), or a floating point number that is to scaled by
// the length of the sequence to produce the final index into the sequence.
//
// After initialization with load_raw_ranges(), begin_sequence() is called to
// intepret the "raw" ranges in the context of the specified sequence.  Negative
// and floating-point indices are coverted to exact indices.  Overlapping ranges
// are merged together, and ranges that end at a lower index than they start are
// discardeed.  All indices are made such that they do not overflow the sequence
// boundaries.
//
// After calling begin_sequence(), call next() repeatedly until the sequences
// have been exhausted and NULL is returned.
class RangeIterator {
public:
    RangeIterator();
    ~RangeIterator() { }

    bool load_raw_ranges(const char *ranges_str);
    void begin_sequence(const char *seq, long seq_len);
    void begin_sequence(const std::string &seq) {
        begin_sequence(seq.c_str(), (long)seq.length());
    }
    const Range *next()
    {
        if (m_current_range >= m_num_ranges)
            return NULL;
        else
            return &m_ranges[m_current_range++];
    }

private:
    struct RawRangeEndpoint {
    public:
        bool is_double;
        union {
            double double_val;
            long long_val;
        };
        bool load_endpoint(const char *endpoint_str, long edge_val);
        long get_endpoint(long seq_len) const;
    };

    struct RawRange {
        bool is_default;
        RawRangeEndpoint start;
        RawRangeEndpoint end;
    };
    bool add_range(char *range_str);
    bool add_ranges(char *ranges_str);

    size_t                 m_num_ranges;
    size_t                 m_current_range;
    std::vector<Range>     m_ranges;
    std::vector<RawRange>  m_raw_ranges;
};

#endif
