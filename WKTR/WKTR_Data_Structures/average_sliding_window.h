/***************************************************************************

             WINKAM TM strictly confidential 2019

 ***************************************************************************/
//v.1.0.1
#ifndef AVERAGE_SLIDING_WINDOW_H
#define AVERAGE_SLIDING_WINDOW_H

#include <queue>

namespace WKTR // WINKAM library. Digital Signal Processing
{

/* Class Average_Sliding_Window calculates average (arithmetic mean) in sliding window
 */
class Average_Sliding_Window
{
private:
    std::queue<double> m_input_deque;               // queue of input samples
    double m_sum;                                   // current sum
    unsigned int m_averaging_sample_number;         // window size (averaging sample number)

public:
    /**
     * Constructor
     * @param averaging_sample_number is width of sliding window
     */
    Average_Sliding_Window(unsigned int averaging_sample_number);

    /**
     * Calculates average value
     * @param sample is input sample value
     * @return average value in sliding window
     */
    double process_sample(double input_value) noexcept;

    /**
     * Gets current average value
     * @return average value in sliding window
     */
    double get_current_average() const noexcept;

    /**
     * Gets sliding window size
     * @return number of sample on which averaging is performed
     */
    unsigned int get_averaging_sample_number() const noexcept;

    /**
     * Gets current size of input queue
     * @return input queue size
     */
    unsigned int get_queue_size() const noexcept;

    /**
     * Resets algorithm to initial state and sets width of sliding window
     * @param averaging_sample_number is width of sliding window
     */
    void reset(unsigned int averaging_sample_number) noexcept;

    double get_oldest() const noexcept;

    ~Average_Sliding_Window();
};

}

#endif // AVERAGE_SLIDING_WINDOW_H
