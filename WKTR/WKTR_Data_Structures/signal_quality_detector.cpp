/***************************************************************************

             WINKAM TM strictly confidential 2019

 ***************************************************************************/
//v.1.0.0
#include "signal_quality_detector.h"

namespace WKTR
{

#define TIME_LAST_DIFFS_BUFFER_SIZE 20  // size of sample timestamp differences buffer
#define TIME_NO_SIGNAL_MIN 70           // ms, min time difference between two samples to set state "no signal"
#define PERIOD_REDUCTION_COEF 0.5       // coefficient of signal period reduction for state "signal compression" detection

Signal_Quality_Detector::Signal_Quality_Detector()
{
    reset();
}

void Signal_Quality_Detector::process_sample(uint32_t input_sample_time) noexcept
{
    int dt = input_sample_time - m_time_last_sample;

    // detect state "no signal"
    if(m_time_last_diffs_buffer.size() == TIME_LAST_DIFFS_BUFFER_SIZE)
    {
        m_time_last_diffs_buffer.pop_front();
    }
    m_time_last_diffs_buffer.emplace_back(dt);

    if (dt > TIME_NO_SIGNAL_MIN)
    {
        m_signal_quality_state = Signal_Quality_State::NO_SIGNAL;
    }
    else // detect state "compressed signal"
    {
        double time_diff_avg = 0.;
        for (auto &time_last_diff : m_time_last_diffs_buffer)
        {
            time_diff_avg += time_last_diff;
        }
        time_diff_avg /= m_time_last_diffs_buffer.size();

        if (time_diff_avg < PERIOD_REDUCTION_COEF * 1000 / m_signal_freq)
        {
            m_signal_quality_state = Signal_Quality_State::COMPRESS;
        }
        else
        {
            m_signal_quality_state = Signal_Quality_State::NORMAL;
        }
    }

    m_time_last_sample = input_sample_time;
}

void Signal_Quality_Detector::set_signal_frequency(int signal_freq) noexcept
{
    m_signal_freq = signal_freq;
}

void Signal_Quality_Detector::set_initial_timestamp(uint64_t initial_timestamp) noexcept
{
    m_time_last_sample = initial_timestamp;
}

Signal_Quality_State Signal_Quality_Detector::get_signal_quality_state() const noexcept
{
    return m_signal_quality_state;
}

void Signal_Quality_Detector::reset() noexcept
{
    m_time_last_diffs_buffer.clear();
    m_time_last_sample = 0;
    m_signal_freq = 0;
    m_signal_quality_state = Signal_Quality_State::NORMAL;
}

Signal_Quality_Detector::~Signal_Quality_Detector()
{

}

}
