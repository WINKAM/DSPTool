/***************************************************************************

             WINKAM TM strictly confidential 2019

 ***************************************************************************/
#include "butterworth_lpf.h"

namespace WKTR
{

Butterworth_LPF::Butterworth_LPF()
{
    reset();
}

void Butterworth_LPF::set_coefficients(const std::vector<double>& coeffs_A_vector, const std::vector<double>& coeffs_B_vector, double gain) noexcept
{
    m_coeffs_A_vector = coeffs_A_vector;
    m_coeffs_B_vector = coeffs_B_vector;

    m_filter_order = std::max(m_coeffs_A_vector.size() - 1, m_coeffs_B_vector.size());

    // If any coefficient is absent we set it to zero
    while (1)
    {
        if (m_coeffs_A_vector.size() - 1 == m_filter_order && m_coeffs_B_vector.size() == m_filter_order)
        {
            break;
        }

        if (m_coeffs_A_vector.size() - 1 < m_filter_order)
        {
            m_coeffs_A_vector.push_back(0.0);
        }
        if (m_coeffs_B_vector.size() < m_filter_order)
        {
            m_coeffs_B_vector.push_back(0.0);
        }
    }

    if (gain)
    {
        for (auto& coef_A : m_coeffs_A_vector)
        {
            coef_A /= gain;
        }
    }

    reset();

}

void Butterworth_LPF::set_initial_value(double initial_value) noexcept
{
    m_output = initial_value;
    for (unsigned int i = 0; i < m_filter_order; ++i)
    {
        m_input_deque.push_front(initial_value);
        m_output_deque.push_front(initial_value);
    }
}

double Butterworth_LPF::filter(double input_value) noexcept
{
    if (m_filter_order == 0)
    {
        return input_value;
    }

    // calculate filtered output value
    m_output = input_value * m_coeffs_A_vector.at(0);
    for (unsigned int i = 0; i < m_filter_order; ++i)
    {
        m_output += (m_input_deque.at(i) * m_coeffs_A_vector.at(i+1)) + (m_output_deque.at(i) * m_coeffs_B_vector.at(i));
    }

    // update deques
    m_input_deque.push_front(input_value);
    m_input_deque.pop_back();

    m_output_deque.push_front(m_output);
    m_output_deque.pop_back();

    return m_output;
}

void Butterworth_LPF::reset()
{
    m_output = 0.0; // initial output value

    m_input_deque.clear();
    m_output_deque.clear();

    for (unsigned int i = 0; i < m_filter_order; ++i)
    {
        m_input_deque.push_front(0.0);
        m_output_deque.push_front(0.0);
    }
}

Butterworth_LPF::~Butterworth_LPF()
{

}

}
