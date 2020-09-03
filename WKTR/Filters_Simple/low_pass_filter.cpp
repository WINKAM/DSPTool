/***************************************************************************

             WINKAM TM strictly confidential 2019

 ***************************************************************************/
#include "low_pass_filter.h"

namespace WKTR
{

Low_Pass_Filter::Low_Pass_Filter()
{
}

void Low_Pass_Filter::set_coefficient(double coefficient) noexcept
{
    m_coefficient = coefficient;
}

void Low_Pass_Filter::set_initial_value(double initial_value) noexcept
{
    m_output = initial_value;
}

double Low_Pass_Filter::filter(double input) noexcept
{
    m_output = m_output * m_coefficient + input * (1. - m_coefficient);
    return m_output;
}

}
