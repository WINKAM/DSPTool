/***************************************************************************

             WINKAM TM strictly confidential 21.06.2018

 ***************************************************************************/
//v.1.2.0
#include "frequency_normalizer_3d.h"
#include "frequency_normalizer.h"

namespace WKTR
{

Frequency_Normalizer_3d::Frequency_Normalizer_3d(unsigned int input_freq, unsigned int output_freq, bool if_use_not_extremums)
{
    m_x_norm = new Frequency_Normalizer(input_freq, output_freq, if_use_not_extremums);
    m_y_norm = new Frequency_Normalizer(input_freq, output_freq, if_use_not_extremums);
    m_z_norm = new Frequency_Normalizer(input_freq, output_freq, if_use_not_extremums);
}

void Frequency_Normalizer_3d::set_frequencies(unsigned int input_freq, unsigned int output_freq) noexcept
{
    m_x_norm->set_frequencies(input_freq, output_freq);
    m_y_norm->set_frequencies(input_freq, output_freq);
    m_z_norm->set_frequencies(input_freq, output_freq);
}

std::pair<unsigned int, unsigned int> Frequency_Normalizer_3d::get_frequencies() const noexcept
{
    return m_x_norm->get_frequencies(); // frequencies are the same for all axes
}

WKTR::Sample_3d Frequency_Normalizer_3d::process_sample(const WKTR::Sample_3d& input_sample) noexcept
{
    WKTR::Sample_3d output;
    bool is_accepted;

    output.m_x = m_x_norm->process_sample(input_sample.m_x, &is_accepted);
    output.m_y = m_y_norm->process_sample(input_sample.m_y, &is_accepted);
    output.m_z = m_z_norm->process_sample(input_sample.m_z, &is_accepted);

    // is_accepted is the same after any axis normalizer
    output.m_time = is_accepted ? input_sample.m_time : UNDEFINED_TIME_U32;

    return output;
}

void Frequency_Normalizer_3d::reset() noexcept
{
    m_x_norm->reset();
    m_y_norm->reset();
    m_z_norm->reset();
}

Frequency_Normalizer_3d::~Frequency_Normalizer_3d()
{
    delete m_x_norm;
    delete m_y_norm;
    delete m_z_norm;
}


}
