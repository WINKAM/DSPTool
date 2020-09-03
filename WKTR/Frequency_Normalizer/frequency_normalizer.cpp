/***************************************************************************

             WINKAM TM strictly confidential 04.06.2018

 ***************************************************************************/
//v.1.2.0
#include <cmath>
#include "frequency_normalizer.h"
#include "simple_features_detector.h"

namespace WKTR
{

#define BUFFER_SIZE 4 // previous samples stored
#define MIDDLE_SAMPLE_INDEX 1 // in dependence with BUFFER_SIZE
#define EXTREMUM_NONE 0
#define EXTREMUM_MIN  1
#define EXTREMUM_MAX  2

Frequency_Normalizer::Frequency_Normalizer(unsigned int input_freq, unsigned int output_freq, bool if_use_not_extremums)
{
    reset();
    set_frequencies(input_freq, output_freq);
    m_if_use_not_extremums = if_use_not_extremums;
}

void Frequency_Normalizer::set_frequencies(unsigned int input_freq, unsigned int output_freq) noexcept
{
    m_input_freq  = input_freq;
    m_output_freq = output_freq;

    reset();

    // check input and output frequencies and correct it
    if (m_input_freq == 0)
    {
        m_input_freq = 5;
    }

    if (m_output_freq == 0)
    {
        m_output_freq = 5;
    }

    if (m_input_freq / 5 != std::floor((4 + m_input_freq) / 5))
    {
        m_input_freq = 5 * (m_input_freq / 5);
    }

    if (m_output_freq / 5 != std::floor((4 + m_output_freq) / 5))
    {
        m_output_freq = 5 * (m_output_freq / 5);
    }

    if (m_output_freq > m_input_freq)
    {
        m_output_freq = m_input_freq;
    }

    // We remove samples_to_remove samples in each set of m_processed_period points
    m_processed_period = m_input_freq / 5;
    int samples_to_remove = (m_input_freq - m_output_freq) / 5;
    if (samples_to_remove)
    {
        double increment = m_processed_period * 1. / samples_to_remove / 2.;
        double tmp_sum = 0.;
        int cycle_counter = 0;
        while (int(tmp_sum) < m_processed_period)
        {
            tmp_sum += increment;
            if (cycle_counter / 2 != floor((1 + cycle_counter) / 2))
            {
                m_remove_indexes_set.emplace(int(tmp_sum) - 1);
            }
            ++cycle_counter;
        }

//                for (auto& index : m_remove_indexes_set)
//                {
//                    qDebug() << index;
//                }
//                qDebug() << "------------------------";
    }
}

std::pair<unsigned int, unsigned int> Frequency_Normalizer::get_frequencies() const noexcept
{
    return {m_input_freq, m_output_freq};
}

void Frequency_Normalizer::set_if_use_not_extremums_flag(bool if_use_not_extremums) noexcept
{
    m_if_use_not_extremums = if_use_not_extremums;
}

bool Frequency_Normalizer::get_if_use_not_extremums_flag() const noexcept
{
    return m_if_use_not_extremums;
}

double Frequency_Normalizer::process_sample(const double input_sample, bool* is_accepted) noexcept
{
    *is_accepted = false;
    double res = 0.;

    // We use special counter increment and decrement, because m_samples_prev_deque is cyclical samples set
    int counter_minus_1 = (m_counter - 1 >= 0) ? (m_counter - 1) : m_processed_period - 1;
    int counter_plus_1  = (m_counter + 1 < m_processed_period) ? (m_counter + 1) : 0;

    // We use only samples if their indexes are not contained into m_remove_indexes_set
    if (m_remove_indexes_set.find(m_counter) == m_remove_indexes_set.end() && m_samples_prev_deque.size() == BUFFER_SIZE)
    {
        *is_accepted = true;
        //res = input_sample;

        double middle_sample = m_samples_prev_deque.at(MIDDLE_SAMPLE_INDEX);
        double middle_sample_minus_1 = m_samples_prev_deque.at(MIDDLE_SAMPLE_INDEX - 1);
        double middle_sample_plus_1 = m_samples_prev_deque.at(MIDDLE_SAMPLE_INDEX + 1);
        double middle_sample_plus_2 = m_samples_prev_deque.at(MIDDLE_SAMPLE_INDEX + 2);

        /********************************************************************************
         ATTENTION! middle_sample_minus_1 is in the FUTURE relatively middle index;
                    counter_minus_1 is in the PAST relatively m_counter index
                    (similarly for _plus_1)
        ********************************************************************************/

        bool are_all_extremums = Simple_Features_Detector::is_extremum_nonstrict_lr(middle_sample_plus_2,
                                                                                    middle_sample_plus_1,
                                                                                    middle_sample)
                                && Simple_Features_Detector::is_extremum_nonstrict_lr(middle_sample_plus_1,
                                                                                      middle_sample,
                                                                                      middle_sample_minus_1)
                                && Simple_Features_Detector::is_extremum_nonstrict_lr(middle_sample,
                                                                                      middle_sample_minus_1,
                                                                          input_sample);
        bool are_all_not_extremums = !(Simple_Features_Detector::is_extremum_nonstrict_lr(middle_sample_plus_2,
                                                                                        middle_sample_plus_1,
                                                                                        middle_sample))
                                    && !(Simple_Features_Detector::is_extremum_nonstrict_lr(middle_sample_plus_1,
                                                                                          middle_sample,
                                                                                          middle_sample_minus_1))
                                    && !(Simple_Features_Detector::is_extremum_nonstrict_lr(middle_sample,
                                                                                          middle_sample_minus_1,
                                                                                          input_sample));

        // We select sample to output (middle, middle-1 (prev) or middle+1 (post))

        if (m_if_use_not_extremums && are_all_not_extremums)
        {
            // We select min from prev or post samples if all of them are negative and not extremums
            if (middle_sample <= 0
                    && middle_sample_minus_1 <= 0
                    && middle_sample_plus_1 <= 0)
            {
                if (middle_sample_minus_1 < middle_sample_plus_1
                        && (middle_sample_minus_1 != m_last_sample_corrected || m_is_last_sample_uncorrected)
                        && m_remove_indexes_set.find(counter_plus_1) != m_remove_indexes_set.end())
                {
                    res = middle_sample_minus_1;
                    m_last_sample_corrected = res;
                    m_is_last_sample_uncorrected = false;
                }

                if (middle_sample_plus_1 < middle_sample_minus_1
                        && (middle_sample_plus_1 != m_last_sample_corrected || m_is_last_sample_uncorrected)
                        && m_remove_indexes_set.find(counter_minus_1) != m_remove_indexes_set.end())
                {
                    res = middle_sample_plus_1;
                    m_last_sample_corrected = res;
                    m_is_last_sample_uncorrected = false;
                }
            }

            // We select max from prev or post samples if all of them are positive and not extremums
            if (middle_sample >= 0
                    && middle_sample_minus_1 >= 0
                    && middle_sample_plus_1 >= 0)
            {
                if (middle_sample_minus_1 > middle_sample_plus_1
                        && (middle_sample_minus_1 != m_last_sample_corrected || m_is_last_sample_uncorrected)
                        && m_remove_indexes_set.find(counter_plus_1) != m_remove_indexes_set.end())
                {
                    res = middle_sample_minus_1;
                    m_last_sample_corrected = res;
                    m_is_last_sample_uncorrected = false;
                }

                if (middle_sample_plus_1 > middle_sample_minus_1
                        && (middle_sample_plus_1 != m_last_sample_corrected || m_is_last_sample_uncorrected)
                        && m_remove_indexes_set.find(counter_minus_1) != m_remove_indexes_set.end())
                {
                    res = middle_sample_plus_1;
                    m_last_sample_corrected = res;
                    m_is_last_sample_uncorrected = false;
                }
            }
        }
        else
        {
            if (are_all_extremums) // middle, middle-1 and middle+1 - all extremums
            {
                // check is middle sample global extremum, use it as result in this case
                if(  (middle_sample > middle_sample_plus_2
                      && middle_sample > middle_sample_plus_1
                      && middle_sample > middle_sample_minus_1
                      && middle_sample > input_sample)
                     ||
                     (middle_sample < middle_sample_plus_2
                      && middle_sample < middle_sample_plus_1
                      && middle_sample < middle_sample_minus_1
                      && middle_sample < input_sample))
                {
                    res = middle_sample;
                    m_is_last_sample_uncorrected = true;
                }
                else
                {
                    res = middle_sample;

                    // select greatest of prev and post samples
                    if (middle_sample < middle_sample_plus_1
                            && middle_sample < middle_sample_minus_1) // middle sample is minimum, select largest from prev and post
                    {
                        if (middle_sample_plus_1 > middle_sample_minus_1
                                && (middle_sample_plus_1 != m_last_sample_corrected || m_is_last_sample_uncorrected)
                                && m_remove_indexes_set.find(counter_minus_1) != m_remove_indexes_set.end())
                        {
                            res = middle_sample_plus_1;
                            m_last_sample_corrected = res;
                            m_is_last_sample_uncorrected = false;
                        }

                        if (middle_sample_minus_1 > middle_sample_plus_1
                                && (middle_sample_minus_1 != m_last_sample_corrected || m_is_last_sample_uncorrected)
                                && m_remove_indexes_set.find(counter_plus_1) != m_remove_indexes_set.end())
                        {
                            res = middle_sample_minus_1;
                            m_last_sample_corrected = res;
                            m_is_last_sample_uncorrected = false;
                        }
                    }

                    if (middle_sample > middle_sample_plus_1
                            && middle_sample > middle_sample_minus_1) // middle sample is maximum, select least from prev and post
                    {
                        if (middle_sample_plus_1 < middle_sample_minus_1
                                && (middle_sample_plus_1 != m_last_sample_corrected || m_is_last_sample_uncorrected)
                                && m_remove_indexes_set.find(counter_minus_1) != m_remove_indexes_set.end())
                        {
                            res = middle_sample_plus_1;
                            m_last_sample_corrected = res;
                            m_is_last_sample_uncorrected = false;
                        }

                        if (middle_sample_minus_1 < middle_sample_plus_1
                                && (middle_sample_minus_1 != m_last_sample_corrected || m_is_last_sample_uncorrected)
                                && m_remove_indexes_set.find(counter_plus_1) != m_remove_indexes_set.end())
                        {
                            res = middle_sample_minus_1;
                            m_last_sample_corrected = res;
                            m_is_last_sample_uncorrected = false;
                        }
                    }

                    // res has not changed
                    if (res == middle_sample)
                    {
                        m_is_last_sample_uncorrected = true;
                    }
                }
            }
            else
            {
                // middle sample is not extremum, next sample is extremum
                if (m_remove_indexes_set.find(counter_plus_1) != m_remove_indexes_set.end()
                        && (middle_sample_plus_1 != m_last_sample_corrected || m_is_last_sample_uncorrected)
                        && Simple_Features_Detector::is_extremum_nonstrict_lr(input_sample,
                                                                              middle_sample_minus_1,
                                                                              m_samples_prev_deque.at(MIDDLE_SAMPLE_INDEX))
                        && !Simple_Features_Detector::is_extremum_nonstrict_lr(middle_sample_minus_1,
                                                                               m_samples_prev_deque.at(MIDDLE_SAMPLE_INDEX),
                                                                               middle_sample_plus_1)
                        )
                {
                    res = middle_sample_minus_1;
                    m_last_sample_corrected = res;
                    m_is_last_sample_uncorrected = false;
                }
                else
                {
                    // middle sample is not extremum, prev sample is extremum
                    if (m_remove_indexes_set.find(counter_minus_1) != m_remove_indexes_set.end()
                            && (middle_sample_plus_1 != m_last_sample_corrected || m_is_last_sample_uncorrected)
                            && Simple_Features_Detector::is_extremum_nonstrict_lr(middle_sample_plus_2,
                                                                                  middle_sample_plus_1,
                                                                                  m_samples_prev_deque.at(MIDDLE_SAMPLE_INDEX))
                            && !Simple_Features_Detector::is_extremum_nonstrict_lr(middle_sample_minus_1,
                                                                                   m_samples_prev_deque.at(MIDDLE_SAMPLE_INDEX),
                                                                                   middle_sample_plus_1)
                            )
                    {
                        res = middle_sample_plus_1;
                        m_last_sample_corrected = res;
                        m_is_last_sample_uncorrected = false;
                    }

                    // use middle sample otherwise
                    else
                    {
                        res = m_samples_prev_deque.at(MIDDLE_SAMPLE_INDEX);
                        m_is_last_sample_uncorrected = true;
                    }
                }
            }
        }
    }

    // update and reset m_counter to match m_processed_period
    if (m_samples_prev_deque.size() > MIDDLE_SAMPLE_INDEX)
    {
        if (++m_counter == m_processed_period)
        {
            m_counter = 0;
        }
    }

    // cyclical last samples set (deque)
    if (m_samples_prev_deque.size() >= BUFFER_SIZE)
    {
        m_samples_prev_deque.pop_back();
    }
    m_samples_prev_deque.emplace_front(input_sample);

    return res;
}

void Frequency_Normalizer::reset() noexcept
{
    m_remove_indexes_set.clear();
    m_samples_prev_deque.clear();

    m_is_last_sample_uncorrected = true;

    m_processed_period = 1;
    m_counter = 0;
}

Frequency_Normalizer::~Frequency_Normalizer()
{

}

}
