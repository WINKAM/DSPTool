/***************************************************************************

WINKAM TM strictly confidential 13.11.2017

***************************************************************************/
#include "alg_temp_labels.h"

void Alg_Temp_Labels::clear_all_labels() noexcept
{
    m_alg_labels.clear();
}

void Alg_Temp_Labels::add_label(double time, const QString &color, double width, const QString &description) noexcept
{
    Alg_Label label;
    label.m_time = time;
    label.m_color = color;
    label.m_width = width;
    label.m_description = description;
    m_alg_labels.emplace_back(label);
}

std::vector<Alg_Temp_Labels::Alg_Label>* Alg_Temp_Labels::get_alg_temp_labels() noexcept
{
    return &m_alg_labels;
}
