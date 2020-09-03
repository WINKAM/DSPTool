/***************************************************************************

             WINKAM TM strictly confidential 2019

 ***************************************************************************/
//v.1.2.0
#include "simple_features_detector.h"
#include "sample.h"
#include <cmath>

namespace WKTR // WINKAM TM Digital Signal Processing library
{

Thunder_Detector::Thunder_Detector(uint32_t max_time_widht, double max_value_height, uint32_t min_time_widht, double min_value_height, unsigned int deque_size)
{
    reset(max_time_widht, max_value_height, min_time_widht, min_value_height, deque_size);
}

Thunder Thunder_Detector::detect_thunder(double value, uint32_t time) noexcept
{
    Thunder result = {{UNDEFINED_TIME_U32, 0.}, {UNDEFINED_TIME_U32, 0.}};

    // detect mimimum
    if (Simple_Features_Detector::is_min_nonstrict_lr(m_sample_2.second, m_sample_1.second, value))
    {
        if (m_min_deque.size() == m_deque_size)
        {
            m_min_deque.pop_front();
        }
        m_min_deque.emplace_back(m_sample_1);

        // find max height thunder
        result.m_first_extremum = find_min_max(m_max_deque);
    }
    // detect maximum
    else if (Simple_Features_Detector::is_max_nonstrict_lr(m_sample_2.second, m_sample_1.second, value))
    {
        if (m_max_deque.size() == m_deque_size)
        {
            m_max_deque.pop_front();
        }
        m_max_deque.emplace_back(m_sample_1);

        // find max height thunder
        result.m_first_extremum = find_min_max(m_min_deque);
    }

    // if first extremum was found, then prev sample is second extremum
    if (result.m_first_extremum.first != UNDEFINED_TIME_U32)
    {
        result.m_second_extremum = m_sample_1;
    }

    m_sample_2 = m_sample_1;
    m_sample_1 = {time, value};

    return result;
}

std::pair<uint32_t, double> Thunder_Detector::find_min_max(const std::deque<std::pair<uint32_t, double>>& deque) const noexcept
{
    std::pair<uint32_t, double> result = {UNDEFINED_TIME_U32, 0.};

    for (const std::pair<uint32_t, double>& min_or_max : deque)
    {
        // check time between extremes, difference of values and select pair of extremum with the maximum difference
        if (m_sample_1.first - min_or_max.first >= m_min_time_width
                && m_sample_1.first - min_or_max.first <= m_max_time_width
                && std::abs(m_sample_1.second - min_or_max.second) >= m_min_value_height
                && std::abs(m_sample_1.second - min_or_max.second) <= m_max_value_height
                && (std::abs(m_sample_1.second - min_or_max.second) > std::abs(m_sample_1.second - result.second)
                    || result.first == UNDEFINED_TIME_U32)
                )
        {
            result.first = min_or_max.first;
            result.second = min_or_max.second;
        }
    }

    return result;
}

void Thunder_Detector::reset(uint32_t max_time_widht, double max_value_height, uint32_t min_time_widht, double min_value_height, unsigned int deque_size) noexcept
{
    m_max_time_width = max_time_widht;
    m_max_value_height = max_value_height;
    m_min_time_width = min_time_widht;
    m_min_value_height = min_value_height;
    m_deque_size = deque_size;
    reset();
}

void Thunder_Detector::reset() noexcept
{
    m_min_deque.clear();
    m_max_deque.clear();
    m_sample_1 = m_sample_2 = {0, 0.};
}

Thunder_Detector::~Thunder_Detector()
{

}

bool Simple_Features_Detector::is_bend(const std::array<double, 5>& data_array, double step
                                       , double diff_max, double diff_extended_max
                                       , double next_to_prev_min, double avg_after_min) noexcept
{
    double avg_delta_prev = (data_array[2]- data_array[0]) / (step + 1);
    double avg_delta_post = (data_array[4] - data_array[2]) / (step + 1);

    double delta_prev = data_array[2]- data_array[1];
    double delta_post = data_array[3] - data_array[2];

    return (    ((std::abs(delta_prev - avg_delta_prev) < diff_max
                  && std::abs(delta_post - avg_delta_post) < diff_extended_max)
                 ||
                 (std::abs(delta_prev - avg_delta_prev) < diff_extended_max
                  && std::abs(delta_post - avg_delta_post) < diff_max))
                && std::abs(delta_prev - avg_delta_prev) > 0.
                && std::abs(delta_post - avg_delta_post) > 0.
                && avg_delta_post / avg_delta_prev > next_to_prev_min
                && avg_delta_post > avg_after_min);
}

}

