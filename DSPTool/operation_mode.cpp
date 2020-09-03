/***************************************************************************

             WINKAM TM strictly confidential 13.04.2020

 ***************************************************************************/
#include "operation_mode.h"

#include <QDebug>
#include <QApplication>

//void Operation_Mode::set_plot_zoom(int plot_id, int a, int b, int c, int d) noexcept
//{
//    m_plot_zooms[plot_id] = {a, b, c, d};
//}

//std::vector<int> Operation_Mode::get_plot_zoom(int plot_id) const noexcept
//{
//    auto iter = m_plot_zooms.find(plot_id);
//    if (iter == m_plot_zooms.end())
//    {
//        return {};
//    }
//    return iter->second;
//}

void Operation_Mode::set_episode_name(const QString& file_name) noexcept
{
    m_log_name = file_name;
}

void Operation_Mode::set_begin_reading_data(const QString& file_name) noexcept
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    m_is_reading_data = true;

    if (!file_name.isEmpty())
    {
        m_log_name = file_name;
    }
    //qDebug() << m_log_name;
}

void Operation_Mode::set_end_reading_data() noexcept
{
    QApplication::restoreOverrideCursor();
    m_is_reading_data = false;
}

bool Operation_Mode::is_reading_data() const noexcept
{
    return m_is_reading_data;
}

QString Operation_Mode::get_log_name() const noexcept
{
    return m_log_name;
}

void Operation_Mode::set_only_low_freq_opening(bool is_only_low_freq_opening) noexcept
{
    m_is_only_low_freq_opening = is_only_low_freq_opening;
}

bool Operation_Mode::is_only_low_freq_opening() const noexcept
{
    return m_is_only_low_freq_opening;
}
