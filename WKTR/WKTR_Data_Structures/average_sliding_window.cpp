/***************************************************************************

             WINKAM TM strictly confidential 2019

 ***************************************************************************/
#include "average_sliding_window.h"

#include <iostream>

namespace WKTR
{

Average_Sliding_Window::Average_Sliding_Window(unsigned int averaging_sample_number)
{
    reset(averaging_sample_number);
}

double Average_Sliding_Window::process_sample(double input_value) noexcept
{
    m_sum += input_value;
    m_input_deque.emplace(input_value);

    // remove oldest speed
    if (m_input_deque.size() > m_averaging_sample_number)
    {
        m_sum -= m_input_deque.front();
        m_input_deque.pop();
    }

    return get_current_average();
}

double Average_Sliding_Window::get_current_average() const noexcept
{
    return m_sum / m_averaging_sample_number;
}

unsigned int Average_Sliding_Window::get_averaging_sample_number() const noexcept
{
    return m_averaging_sample_number;
}

unsigned int Average_Sliding_Window::get_queue_size() const noexcept
{
    return m_input_deque.size();
}

double Average_Sliding_Window::get_oldest() const noexcept
{
    if (!m_input_deque.empty())
    {
        return m_input_deque.front();
    }
    return 0.;
}

void Average_Sliding_Window::reset(unsigned int averaging_sample_number) noexcept
{
    m_averaging_sample_number = averaging_sample_number;
    if (m_averaging_sample_number == 0)
    {
        std::cerr << "Warning! Average_XY_Speed::reset() You try to set zero value in averaging_sample_number. It will be changed to 1"
                  << std::endl;

        m_averaging_sample_number = 1;
    }
    m_sum = 0.;
    std::queue<double>().swap(m_input_deque);
}

Average_Sliding_Window::~Average_Sliding_Window()
{
}

}
