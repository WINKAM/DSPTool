/***************************************************************************

             WINKAM TM strictly confidential 2019

 ***************************************************************************/
#include "max_sliding_window.h"

namespace WKTR
{

Max_Sliding_Window::Max_Sliding_Window(uint64_t time_window_size)
{
    reset(time_window_size);
}

std::pair<uint64_t, double> Max_Sliding_Window::process_sample(uint64_t time, double value) noexcept
{
    // 1. remove old elements
    while (!m_input_deque.empty() && time > m_input_deque.back().first + m_time_window_size)
    {
        m_input_deque.pop_back();
    }

        // 2. remove elements that are smaller than new input sample (such elements will not be able to become maximum in window)
    while (!m_input_deque.empty() && value > m_input_deque.front().second)
    {
        m_input_deque.pop_front();
    }

    m_input_deque.emplace_front(std::pair<uint64_t, double>{time, value});

    return m_input_deque.back();
}

void Max_Sliding_Window::reset(uint64_t time_window_size) noexcept
{
    m_time_window_size = time_window_size;
    m_input_deque.clear();
}

Max_Sliding_Window::~Max_Sliding_Window()
{

}

}
