#ifndef STATS_COUNTER_H
#define STATS_COUNTER_H

#include <algorithm>  // std::copy/sort
#include <iomanip>    // std::setprecision
#include <map>        // std::map
#include <ostream>    // std::ostream
#include <vector>     // std::vector
#include <math.h>     // sqrt
#include <stddef.h>   // size_t

#ifndef _WIN32
#define PLUS_MINUS " ± "
#else
// Multibyte symbols work better with wide-character strings on Windows.
// Not worth the trouble here.
#define PLUS_MINUS " +- "
#endif

template <typename T>
class stats_counter {
public:
    explicit stats_counter(size_t size) { values_.reserve(size); }
    void add(T value) { values_.push_back(value); }
    void print_result(std::ostream& os, const char* unit);

private:
    std::vector<T> values_;
};

template <typename T>
void stats_counter<T>::print_result(std::ostream& os, const char* unit)
{
    if (values_.size() < 2) {
        os << "*** Too few inputs" << std::endl;
        return;
    }

    std::vector<T> values(values_.size());
    std::copy(values_.begin(), values_.end(), values.begin());
    std::sort(values.begin(), values.end());
    int count = static_cast<int>(values.size());

    std::map<T, size_t> counts;
    T sum = 0;
    T sum_sqr = 0;
    T min = values_[0];
    T max = values_[0];
    for (auto value : values_) {
        ++counts[value];
        sum += value;
        sum_sqr += value * value;
        if (min > value) {
            min = value;
        }
        if (max < value) {
            max = value;
        }
    }
    if (count > 2 && counts.size() > 1) {
        os << "Ignoring one biggest value: " << max << std::endl;
        --count;
        values.resize(count);
        sum -= max;
        sum_sqr -= max * max;
    }

    double median = (count % 2 == 0)
                        ? static_cast<double>(values[count / 2 - 1] +
                                              values[count / 2]) /
                              2
                        : static_cast<double>(values[count / 2]);
    double mean = static_cast<double>(sum) / count;
    double stddev = sqrt(
        (static_cast<double>(sum_sqr) - count * mean * mean) / (count - 1));
    auto orig_precision = os.precision();
    os << "Average " << mean << PLUS_MINUS << std::setprecision(3) << stddev
       << std::setprecision(orig_precision) << ' ' << unit
       << ", min: " << min << ", max: " << max << ", median: " << median
       << std::endl;
    if (counts.size() <= 1) {
        return;
    }
    os << "Distribution:\n";
    size_t output_count = 0;
    for (auto& [value, vcount] : counts) {
        if (++output_count > 5) {
            os << " ...\n";
            break;
        }
        os << ' ' << value << ": " << vcount << '\n';
    }
}

#endif // STATS_COUNTER_H
