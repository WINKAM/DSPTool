/***************************************************************************

             WINKAM TM strictly confidential 21.06.2018

 ***************************************************************************/
//v.1.2.0
#ifndef FREQUENCY_NORMALIZER_3D_H
#define FREQUENCY_NORMALIZER_3D_H

#include "sample.h"
#include <utility>

namespace WKTR
{

class Frequency_Normalizer;

class Frequency_Normalizer_3d
{
private:
    Frequency_Normalizer* m_x_norm;
    Frequency_Normalizer* m_y_norm;
    Frequency_Normalizer* m_z_norm;

public:
    /**
     * Constructor
     * @param input_freq is input signal frequency in Hz
     * @param output_freq is output signal frequency in Hz
     * @param if_use_not_extremums is flag enabled using not extremums for sample correction
     */
    explicit Frequency_Normalizer_3d(unsigned int input_freq, unsigned int output_freq, bool if_use_not_extremums);

    ~Frequency_Normalizer_3d();

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
     * Processes samples and returns samples corrensponded to output frequency
     * @param input_sample is input signal sample
     * @return output sample (in accordance to output frequency); timestamp == WKTR::UNDEFINED_TIME_32 if sample is not accepted
     */
    WKTR::Sample_3d process_sample(const WKTR::Sample_3d& input_sample) noexcept;

    /**
     * Resets all internal class fields to default values.
     * Use it in case of system restart
     */
    void reset() noexcept;
};

}

#endif // FREQUENCY_NORMALIZER_3D_H
