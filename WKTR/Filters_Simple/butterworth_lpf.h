/***************************************************************************

             WINKAM TM strictly confidential 2019

 ***************************************************************************/
#ifndef BUTTERWORTH_LPF_H
#define BUTTERWORTH_LPF_H

#include <vector>
#include <deque>

namespace WKTR // WINKAM library. Digital Signal Processing
{

/* Butterworth Low Pass Filter. Order and cut-off frequency are set via set_coefficients(...)
 * Coefficients should be default or can be redefined by using any filter design package.
 * Lower cut-off frequency leads to stronger signal smoothing and to longer delay.
 * Higher order leads to stronger suppression of harmonics with frequency higher than cut-off frequency.
 * 
 * output = vector_A * vector_input + vector_B * vector_output
 */

class Butterworth_LPF
{
private:
    std::vector<double> m_coeffs_A_vector; // A coefficients vector
    std::vector<double> m_coeffs_B_vector; // B coefficients vector

    std::deque<double> m_input_deque; // input values deque
    std::deque<double> m_output_deque; // output values deque

    double m_output; // current output value

    unsigned int m_filter_order = 0; // is calculated using vector A and vector B lengths

public:

    explicit Butterworth_LPF();

    /**
     * Sets filter coefficients
     * @param coeffs_A_vector is vector A' (without gain)
     * @param coeffs_B_vector is vector B
     * @param gain is gain of vector A, so A[i] = A'[i] / gain
     */
    void set_coefficients(const std::vector<double> &coeffs_A_vector, const std::vector<double> &coeffs_B_vector, double gain = 1.) noexcept;

    /**
     * Sets initial signal value before start filtration.
     * Default value is 0.
     * @param initial_value is initial signal value
     */
    void set_initial_value(double initial_value) noexcept;

    /**
     * Filters input signal sample by sample.
     * @param input_value is value of input sample
     * @return filtered output value
     */
    double filter(double input_value) noexcept;

    /**
     * Resets filter to initial state.
     */
    void reset();

    ~Butterworth_LPF();
};

}

#endif // BUTTERWORTH_LPF_H
