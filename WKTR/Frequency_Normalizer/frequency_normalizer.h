/***************************************************************************

             WINKAM TM strictly confidential 04.06.2018

 ***************************************************************************/
//v.1.2.0
#ifndef FREQUENCY_NORMALIZER_H
#define FREQUENCY_NORMALIZER_H

#include <unordered_set>
#include <deque>

namespace WKTR
{

/* This class gets samples of signal with determined input frequency
 * and returns only samples corresponded to set output frequency,
 * with considering extremums
 */

class Frequency_Normalizer
{
private:
    std::unordered_set<int> m_remove_indexes_set; // indexes into periodic array to be removed
    std::deque<double> m_samples_prev_deque; // deque of previous samples for searching extremums

    double m_last_sample_corrected; // value of last sample corrected to extremum

    unsigned int m_input_freq;
    unsigned int m_output_freq;
    int m_processed_period; // number of samples in each of which fix number of samples are removed
    int m_counter; // determine index into each m_processed_period sector

    bool m_if_use_not_extremums; // flag enabled using not extremums for sample correction
    bool m_is_last_sample_uncorrected; // flag indicated that sample at last iteration has not been corrected to prev or post

public:
    /**
     * Constructor
     * @param input_freq is input signal frequency in Hz
     * @param output_freq is output signal frequency in Hz
     * @param if_use_not_extremums is flag enabled using not extremums for sample correction
     */
    explicit Frequency_Normalizer(unsigned int input_freq, unsigned int output_freq, bool if_use_not_extremums = false);

    ~Frequency_Normalizer();

    /**
     * Sets input and output frequencies, corrects to allowable values
     * @param input_freq is input signal frequency in Hz
     * @param output_freq is output signal frequency in Hz
     */
    void set_frequencies(unsigned int input_freq, unsigned int output_freq) noexcept;

    /**
     * Gets input and output frequencies
     * @return pair of input and output frequencies in Hz
     */
    std::pair<unsigned int, unsigned int> get_frequencies() const noexcept;

    /**
     * Sets flag enabled using not_extremums for sample correction
     * @param if_use_not_extremums true if enabled
     */
    void set_if_use_not_extremums_flag(bool if_use_not_extremums) noexcept;

    /**
     * Gets value of flag enabled using not_extremums for sample correction
     * @return flag enabled using not_extremums
     */
    bool get_if_use_not_extremums_flag() const noexcept;

    /**
     * Processes samples and returns samples corrensponded to output frequency
     * @param input_sample is value of signal at any axis
     * @param is_accepted is pointer to flag allowed using current sample in corresponding to frequency
     * @return output sample value (in accordance to output frequency)
     */
    double process_sample(const double input_sample, bool* is_accepted) noexcept;

    /**
     * Resets all internal class fields to default values.
     * Use it in case of system restart
     */
    void reset() noexcept;

};

}

#endif // FREQUENCY_NORMALIZER_H
