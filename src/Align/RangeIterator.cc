//
// See RangeIterator.h for documentation
//
#include "RangeIterator.h"
#include <string.h>
#include <stdlib.h>


RangeIterator::RangeIterator()
{
    RawRange full_range;
    full_range.is_default = true;
    full_range.start.is_double = false;
    full_range.start.long_val = 1;
    full_range.end.is_double = false;
    full_range.end.long_val = -1;
    m_raw_ranges.push_back(full_range);
    m_ranges.resize(1);
}

bool RangeIterator::load_raw_ranges(const char *ranges_str)
{
    if (m_raw_ranges[0].is_default)
        m_raw_ranges.resize(0);
    char copy[strlen(ranges_str) + 1];
    strcpy(copy, ranges_str);
    if (!add_ranges(copy))
        return false;
    m_ranges.resize(m_raw_ranges.size());
    return true;
}


bool RangeIterator::add_ranges(char *ranges_str)
{
    char *ptr = ranges_str;
    char *tok;

    while ((tok = strtok(ptr, ","))) {
        ptr = NULL;
        if (!add_range(tok))
            return false;
    }
    return true;
}

bool RangeIterator::add_range(char *range_str)
{
    char *start_str;
    char *end_str;
    char *colon;
    RawRange range;
    colon = strchr(range_str, ':');
    if (!colon || strrchr(range_str, ':') != colon)
        return false;
    *colon = '\0';
    start_str = range_str;
    end_str = colon + 1;
    if (!range.start.load_endpoint(start_str, 1))
        return false;
    if (!range.end.load_endpoint(end_str, -1))
        return false;
    if (range.start.is_double && range.end.is_double &&
        range.start.double_val > range.end.double_val)
            return false;
    else if (!range.end.is_double && !range.end.is_double
            && (range.start.long_val < 0) == (range.end.long_val < 0)
            && range.start.long_val > range.end.long_val)
        return false;

    range.is_default = false;
    m_raw_ranges.push_back(range);
    return true;
}

bool RangeIterator::RawRangeEndpoint::load_endpoint(const char *endpoint_str,
                                                    long edge_val)
{
    if (*endpoint_str == '\0') {
        long_val = edge_val;
        is_double = false;
    } else {
        char *p;
        if (strchr(endpoint_str, '.')) {
            double_val = strtod(endpoint_str, &p);
            if (double_val < -1.0 || double_val > 1.0)
                return false;
            if (double_val < 0.0)
                double_val += 1.0;
            is_double = true;
        } else {
            long_val = strtol(endpoint_str, &p, 10);
            if (long_val == 0)
                return false;
            is_double = false;
        }
        if (p == endpoint_str || *p != '\0')
            return false;
    }
    return true;
}


static int compare_ranges(const void *p1, const void *p2)
{
    const Range *range_1 = (const Range *)p1;
    const Range *range_2 = (const Range *)p2;
    if (range_1->start < range_2->start)
        return -1;
    else if (range_1->start > range_2->start)
        return 1;
    else
        return 0;
}


long RangeIterator::RawRangeEndpoint::get_endpoint(long seq_len) const
{
    long ret;
    if (is_double) {
        ret = (long)((seq_len - 1) * double_val + 1);
    } else {
        if (long_val < 0)
            ret = seq_len + 1 + long_val;
        else
            ret = long_val;
    }
    if (ret < 1)
        ret = 1;
    else if (ret > seq_len)
        ret = seq_len;
    return ret;
}

void RangeIterator::begin_sequence(const char *seq, long seq_len)
{
    size_t num_ranges;
    size_t i, j;

    m_current_range = 0;
    m_num_ranges = 0;

    if (seq_len < 1)
        return;
    num_ranges = m_raw_ranges.size();
    j = 0;
    for (i = 0; i < num_ranges; i++) {
        m_ranges[j].start = m_raw_ranges[i].start.get_endpoint(seq_len);
        m_ranges[j].end = m_raw_ranges[i].end.get_endpoint(seq_len);
        if (m_ranges[j].end >= m_ranges[j].start)
            j++;
    }
    num_ranges = j;
    if (num_ranges == 0)
        return;

    qsort(&m_ranges[0], num_ranges, sizeof(m_ranges[0]), compare_ranges);
    j = 0;
    for (i = 1; i < num_ranges; i++) {
        long start, end;
        if (m_ranges[j].end + 1 < m_ranges[i].start) {
            j++;
            start = m_ranges[i].start;
            end = m_ranges[i].end;
        } else {
            // Merge ranges
            start = m_ranges[j].start;
            end = std::max(m_ranges[i].end, m_ranges[j].end);
        }
        m_ranges[j].start = start;
        m_ranges[j].end = end;
    }
    m_num_ranges = j + 1;
}
