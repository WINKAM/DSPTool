/***************************************************************************

             WINKAM TM strictly confidential 2019

 ***************************************************************************/
//v.1.0.0
#ifndef SIGNAL_QUALITY_DETECTOR_H
#define SIGNAL_QUALITY_DETECTOR_H

#include <memory>
#include <deque>
#include "sample.h"

namespace WKTR
{

enum class Signal_Quality_State {NORMAL = 0, NO_SIGNAL = 1, COMPRESS = 2};

// Class Signal_Quality_Detector processes accelerometer signal and defines its quality:
// detects "no signal" and "compressed signal". Returns "good signal" otherwise

class Signal_Quality_Detector
{
private:
    std::deque<int> m_time_last_diffs_buffer;  // buffer to store last sample timestamps differences

    uint32_t m_time_last_sample; // last input signal sample timestamp, ms

    int m_signal_freq; // input signal frequency, Hz
    Signal_Quality_State m_signal_quality_state; // current state of input signal (see signal quality states)

public:
    /**
     * Constructor
     */
    Signal_Quality_Detector();    

    /**
     * Processes raw input accel signal and defines its quality
     * @param input_sample is input sample timestamp
     */
    void process_sample(uint32_t input_sample_time) noexcept;

    /**
     * Sets input signal frequency
     * @param signal_freq is input signal frequency in Hz, by default 100 Hz
     */
    void set_signal_frequency(int signal_freq = 100) noexcept;

    /**
     * Sets timestamp of first input signal sample
     * @param initial_timestamp is first input signal sample time, in ms
     */
    void set_initial_timestamp(uint64_t initial_timestamp = 0) noexcept;

    /**
     * Gets detected signal quality state
     * @return input signal quality state (see signal quality states)
     */
    Signal_Quality_State get_signal_quality_state() const noexcept;

    /**
     * Resets algo to initial state
     * Use it in case of system restart
     */
    void reset() noexcept;

    /**
     * Destructor
     */
    ~Signal_Quality_Detector();
};

}

#endif // SIGNAL_QUALITY_DETECTOR_H
