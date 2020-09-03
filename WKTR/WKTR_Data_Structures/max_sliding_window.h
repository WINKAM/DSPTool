/***************************************************************************

             WINKAM TM strictly confidential 2019

 ***************************************************************************/
#ifndef MAX_SLIDING_WINDOW_H
#define MAX_SLIDING_WINDOW_H

#include <deque>
#include <stdint.h>

namespace WKTR // WINKAM library. Digital Signal Processing
{

/* Class Max_Sliding_Window computes maximum input signal value in sliding window
 */
class Max_Sliding_Window
{
private:
    std::deque<std::pair<uint64_t, double>> m_input_deque; // deque of input samples
    uint64_t m_time_window_size; // size of sliding window in milliseconds

public:
    /**
     * Constructor
     * @param time_window_size is size of sliding window in milliseconds
     */
    Max_Sliding_Window(uint64_t time_window_size);

    /**
     * Computes maximum value in sliding window
     * @param time  is input sample timestamp in milliseconds
     * @param value is input sample value
     * @return maximum in current window: first is timestamp of maximum; second is magnitude of maximum
     */
    std::pair<uint64_t, double> process_sample(uint64_t time, double value) noexcept;


    /**
     * Resets algorithm to initial state and sets window size
     * @param time_window_size is size of sliding window in milliseconds
     */
    void reset(uint64_t time_window_size) noexcept;

    ~Max_Sliding_Window();
};

}

#endif // MAX_SLIDING_WINDOW_H
