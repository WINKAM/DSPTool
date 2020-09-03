/***************************************************************************

             WINKAM TM strictly confidential 07.05.2020

 ***************************************************************************/
#include "metronom.h"
#include <thread>
#include <QDebug>
#include <QApplication>
#include <QDateTime>
#include "time_mediator.h"
#include "video/speed_convertor.h"

Metronom::Metronom(QObject *parent): QObject(parent)
{
    m_timestamp = 0;
    m_timestamp_last_emit = 0;
    m_is_running = false;
    m_is_proc_loop_started = true;
    m_interval_init = 17;
    m_interval = m_interval_init;
    m_avg_real_interval = m_interval_init;
    m_skip_counter = 0;
    m_speed = SPEED_NORMAL;
}

void Metronom::run() noexcept
{
    while(m_is_proc_loop_started)
    {
        if (m_is_running)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(3));

            if (m_is_running) // пока спали, is_running могло стать false
            {
                uint64_t timestamp = QDateTime::currentDateTime().toMSecsSinceEpoch();
                int delta_time = timestamp - m_timestamp_last_change;

                if (delta_time >= m_interval)
                {
                    int skip = delta_time / m_interval;

                    m_timestamp_last_change = timestamp - (delta_time % m_interval);

                    m_timestamp += skip * m_interval_init;

                    //qDebug() << m_timestamp << "!!!!" << timestamp << "!!!!!!" << m_skip_counter;

                    if (Time_Mediator::get_instance().mutex_try_lock())
                    {
                        //qDebug() << m_skip_counter << "skip";
                        if (m_timestamp_last_emit != 0)
                        {
                            m_avg_real_interval = 0.95 * m_avg_real_interval + 0.05 * (timestamp - m_timestamp_last_emit);
                        }

                        emit next_step_got(m_timestamp, static_cast<int>(0.5 + 1000. / m_avg_real_interval));
                        Time_Mediator::get_instance().mutex_unlock();
                        m_skip_counter = 0;

                        m_timestamp_last_emit = timestamp;
                    }
                    else
                    {
                        m_skip_counter += skip;
                        //if (m_skip_counter > 6 && m_speed <= SPEED_NORMAL && m_speed >= SPEED_MIN) // если скорость повышенная, то пусть пропускает сколько нужно, иначе много пропускать не надо
                        {
                            //qDebug() << "problem with video playback; try to reduce number of showed signals on Time signal plot or slow down playback speed";
                            //emit need_slower(m_speed - 1);
                        }
                        //qDebug() << "skip" << m_skip_counter;
                    }
                }
            }
        }
        else
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        //QApplication::processEvents();
    }
    qDebug() << "Metronom stopped";
}

void Metronom::timestamp_change(uint64_t timestamp) noexcept
{
    if (m_timestamp == timestamp)
    {
        return;
    }

    bool was_running = m_is_running;

    if (m_is_running)
    {
        pause();
    }

    m_timestamp = timestamp;
    m_timestamp_last_change = QDateTime::currentDateTime().toMSecsSinceEpoch();
    m_timestamp_last_emit = 0;

    if (was_running)
    {
        play();
    }
}

void Metronom::play() noexcept
{
    m_is_running = true;
    m_timestamp_last_change = QDateTime::currentDateTime().toMSecsSinceEpoch();
    m_timestamp_last_emit = 0;
}

void Metronom::pause() noexcept
{
    m_is_running = false;
    m_timestamp_last_emit = 0;
}

void Metronom::release() noexcept
{
    m_is_proc_loop_started = false;
}

void Metronom::change_speed(int speed) noexcept
{
    m_speed = speed;
    m_interval = m_interval_init * SPEED_INTERVAL_COEFS[speed];
}
