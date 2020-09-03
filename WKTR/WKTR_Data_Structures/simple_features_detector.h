/***************************************************************************

             WINKAM TM strictly confidential 2019

 ***************************************************************************/
//v.1.2.0
#ifndef SIMPLE_FEATURES_DETECTOR_H
#define SIMPLE_FEATURES_DETECTOR_H

#include <vector>
#include <array>
#include <deque>

namespace WKTR // WINKAM TM Digital Signal Processing library
{

// Simple Feature Detection is set of structs and classes to detect signal features such as extremum, thunder, etc.

#define WKTR_MIN(min, value) if (value < min) {min = value;}
#define WKTR_MAX(max, value) if (value > max) {max = value;}
#define WKTR_MIN_MAX(min, max, value) if (value < min) {min = value;} else if (value > max) {max = value;}

// Struct Thunder store data about sudden change in signal (zigzag with minimum and maximum extremes)
struct Thunder
{
    std::pair<uint32_t, double> m_first_extremum;  // timestamp and value of first extremum
    std::pair<uint32_t, double> m_second_extremum; // timestamp and value of second extremum
};

// Class Thunder_Detector detects sudden change in signal (zigzag with minimum and maximum extremes)
class Thunder_Detector
{
private:
    std::deque<std::pair<uint32_t, double>> m_min_deque; // deque of mimimums (timestamp and value)
    std::deque<std::pair<uint32_t, double>> m_max_deque; // deque of mimimums (timestamp and value)

    std::pair<uint32_t, double> m_sample_1; // prev sample
    std::pair<uint32_t, double> m_sample_2; // prev-prev sample

    uint32_t m_min_time_width; // mimimum time between first and second extremes
    uint32_t m_max_time_width; // maximum time between first and second extremes

    double m_min_value_height; //  mimimum difference between values of first and second extremes
    double m_max_value_height; //  maximum difference between values of first and second extremes

    unsigned int m_deque_size; // number of maximum extremes in deques, recommended value is 3

private:
    /**
     * Check previous extemes from deque to find extremum suitable to new found
     * @param deque is deque of previous extremes
     * @return suitable extrmum ; if there is not such extremum, then return {UNDEFINED_TIME_U32, 0.}
     */
    std::pair<uint32_t, double> find_min_max(const std::deque<std::pair<uint32_t, double>>& deque) const noexcept;

public:
    /**
     * Constructor
     * @param max_time_widht    is maximum time between first and second extremes
     * @param max_value_height  is maximum difference between values of first and second extremes
     * @param min_time_widht    is mimimum time between first and second extremes
     * @param min_value_height  is mimimum difference between values of first and second extremes
     * @param deque_size        is number of maximum extremes in deques, recommended value is 3
     */
    Thunder_Detector(uint32_t max_time_widht, double max_value_height, uint32_t min_time_widht = 0, double min_value_height = 0., unsigned int deque_size = 3);

    /**
     * Process signal to detect thunder
     * @param value is input signal value
     * @param time is input signal timestamp
     * @return Thunder detected
     */
    Thunder detect_thunder(double value, uint32_t time) noexcept;

    /**
     * Resets all internal class fields
     * @param max_time_widht    is maximum time between first and second extremes
     * @param max_value_height  is maximum difference between values of first and second extremes
     * @param min_time_widht    is mimimum time between first and second extremes
     * @param min_value_height  is mimimum difference between values of first and second extremes
     * @param deque_size        is number of maximum extremes in deques, recommended value is 3
     */
    void reset(uint32_t max_time_widht, double max_value_height, uint32_t min_time_widht = 0, double min_value_height = 0., unsigned int deque_size = 3) noexcept;

    /**
     * Resets algorithm to initial state
     * Use it in case of system restart
     */
    void reset() noexcept;

    ~Thunder_Detector();
};

class Simple_Features_Detector
{
public:


    template<typename T>
    static bool is_min_strict_lr(const T& prev, const T& cur, const T& next) noexcept;

    template<typename T>
    static bool is_min_nonstrict_lr(const T& prev, const T& cur, const T& next) noexcept;

    template<typename T>
    static bool is_min_nonstrict_l(const T& prev, const T& cur, const T& next) noexcept;

    template<typename T>
    static bool is_min_nonstrict_r(const T& prev, const T& cur, const T& next) noexcept;

    template<typename T>
    static bool is_max_strict_lr(const T& prev, const T& cur, const T& next) noexcept;

    template<typename T>
    static bool is_max_nonstrict_lr(const T& prev, const T& cur, const T& next) noexcept;

    template<typename T>
    static bool is_max_nonstrict_l(const T& prev, const T& cur, const T& next) noexcept;

    template<typename T>
    static bool is_max_nonstrict_r(const T& prev, const T& cur, const T& next) noexcept;

    template<typename T>
    static bool is_extremum_nonstrict_r(const T& prev, const T& cur, const T& next) noexcept;

    template<typename T>
    static bool is_extremum_nonstrict_lr(const T& prev, const T& cur, const T& next) noexcept;

    template<typename T>
    static bool is_extremum_strict(const T& prev, const T& cur, const T& next) noexcept;


    static inline double diff_2(double a, double b) noexcept;

    static bool is_bend(const std::array<double, 5>& data_array
                        , double step, double diff_max, double diff_extended_max
                        , double next_to_prev_min, double avg_after_min) noexcept;

};

double Simple_Features_Detector::diff_2(double a, double b) noexcept
{
    return (a - b) * (a - b);
}

inline double normalize_min_max(double value, double min, double max) noexcept
{
    if (value >= max)
    {
        return 1.0;
    }
    if (value <= min)
    {
        return 0.0;
    }
    return (value - min) / (max - min);
}

template<typename T>
bool Simple_Features_Detector::is_min_strict_lr(const T& prev, const T& cur, const T& next) noexcept
{
    return cur < prev && cur <  next;
}

template<typename T>
bool Simple_Features_Detector::is_min_nonstrict_lr(const T& prev, const T& cur, const T& next) noexcept
{
    return cur <= prev && cur <=  next;
}

template<typename T>
bool Simple_Features_Detector::is_min_nonstrict_l(const T& prev, const T& cur, const T& next) noexcept
{
    return cur <= prev && cur <  next;
}

template<typename T>
bool Simple_Features_Detector::is_min_nonstrict_r(const T& prev, const T& cur, const T& next) noexcept
{
    return cur < prev && cur <=  next;
}

template<typename T>
bool Simple_Features_Detector::is_max_strict_lr(const T& prev, const T& cur, const T& next) noexcept
{
    return cur > prev && cur >  next;
}

template<typename T>
bool Simple_Features_Detector::is_max_nonstrict_lr(const T& prev, const T& cur, const T& next) noexcept
{
    return cur >= prev && cur >= next;
}

template<typename T>
bool Simple_Features_Detector::is_max_nonstrict_l(const T& prev, const T& cur, const T& next) noexcept
{
    return cur >= prev && cur >  next;
}

template<typename T>
bool Simple_Features_Detector::is_max_nonstrict_r(const T& prev, const T& cur, const T& next) noexcept
{
    return cur > prev && cur >=  next;
}

template<typename T>
bool Simple_Features_Detector::is_extremum_nonstrict_r(const T& prev, const T& cur, const T& next) noexcept
{
    return (cur > prev && cur >=  next) || (cur < prev && cur <=  next);
}

template<typename T>
bool Simple_Features_Detector::is_extremum_nonstrict_lr(const T& prev, const T& cur, const T& next) noexcept
{
    return (cur >= prev && cur >=  next) || (cur <= prev && cur <=  next);
}

template<typename T>
bool Simple_Features_Detector::is_extremum_strict(const T& prev, const T& cur, const T& next) noexcept
{
    return (cur > prev && cur >  next) || (cur < prev && cur <  next);
}


}

#endif // SIMPLE_FEATURES_DETECTOR_H
