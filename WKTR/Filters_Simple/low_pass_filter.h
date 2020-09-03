/***************************************************************************

             WINKAM TM strictly confidential 2019

 ***************************************************************************/
#ifndef LOW_PASS_FILTER_H
#define LOW_PASS_FILTER_H

namespace WKTR // WINKAM library. Digital Signal Processing
{

class Low_Pass_Filter
{
private:
    double m_output = 0.0; // current output value
    double m_coefficient = 0.5; // output[i] = output[i-1] * m_coefficient + input[i] * (1.f - m_coefficient)

public:
    Low_Pass_Filter();
    void set_coefficient(double coefficient) noexcept;
    void set_initial_value(double initial_value) noexcept;
    double filter(double input) noexcept;
};

}

#endif // LOW_PASS_FILTER_H
