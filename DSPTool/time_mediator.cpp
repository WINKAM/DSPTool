/***************************************************************************

             WINKAM TM strictly confidential 30.04.2020

 ***************************************************************************/
#include "time_mediator.h"

#include <QMessageBox>
#include <ctime>
#include <thread>

#include "plot/base_plot_window.h"
#include "video/video_window.h"
#include "video/speed_convertor.h"
#include "metronom.h"

Time_Mediator::Time_Mediator(QObject* parent) : QObject(parent)
{
    m_current_timestamp = 0;
    m_plot_delta_time = 1000;
    m_video_speed = SPEED_NORMAL;
    m_skip_count = 0;
    m_ptr_metronom = nullptr;
    m_is_playing = false;
    m_is_end = false;
}

void Time_Mediator::set_metronom_ptr(Metronom* ptr_metronom) noexcept
{
    m_ptr_metronom = ptr_metronom;
}

void Time_Mediator::add_ptr_plot_window(Base_Plot_Window* ptr_pw) noexcept
{
    m_ptr_plot_windows.emplace_back(ptr_pw);
}

void Time_Mediator::add_ptr_video_window(Video_Window* ptr_vw) noexcept
{
    pause();
    m_ptr_video_windows.emplace_back(ptr_vw);
}

void Time_Mediator::remove_ptr_video_window(Video_Window *ptr_vw_removed) noexcept
{
    std::vector<Video_Window*> tmp;
    for (auto p : m_ptr_video_windows)
    {
        if (p != ptr_vw_removed)
        {
            tmp.emplace_back(p);
        }
    }

    m_ptr_video_windows = tmp;
}

void Time_Mediator::clear_ptr_plot_windows() noexcept
{
    m_ptr_plot_windows.clear();
}

void Time_Mediator::clear_ptr_video_windows() noexcept
{
    m_ptr_video_windows.clear();
}

// < TIMESTAMP

void Time_Mediator::change_timestamp(uint64_t timestamp) noexcept
{    
    m_mutex.lock();

    m_skip_count = 0;

    if (sender()->objectName()[0] == 'P') // from a plot
    {
        pause();
    }

    if (m_current_timestamp == timestamp)
    {
        m_mutex.unlock();
        return;
    }

    m_current_timestamp = timestamp;

    if (m_ptr_metronom != nullptr)
    {
        m_ptr_metronom->timestamp_change(m_current_timestamp);
    }

    bool is_frame_changed;
    bool is_end_of_videos = change_timestamp_videos(&is_frame_changed);
    bool is_end_of_signals = false;
    is_end_of_signals = change_timestamp_plots();

    m_is_end = is_end_of_videos & is_end_of_signals;

    if (m_is_end)
    {
        pause();
    }

    m_mutex.unlock();
}

void Time_Mediator::change_timestamp_by_metronom(qulonglong timestamp, int skip_count) noexcept
{
    //qDebug() << "change_timestamp_by_metronom" << timestamp;

    if (m_current_timestamp == timestamp || !m_is_playing)
    {
        return;
    }

    m_skip_count = skip_count;

    m_current_timestamp = timestamp;

    //    auto start = clock();

    m_mutex.lock();
    bool is_frame_changed;
    bool is_end_of_videos = change_timestamp_videos(&is_frame_changed);

    bool is_end_of_signals = false;

    is_end_of_signals = change_timestamp_plots();


    //    bool is_end_of_signals = false;
    //    if (is_frame_changed) // для ускорение процесса показа, если не нужно менять кадр, то и на сигнале можно не переходить
    //    {
    //        is_end_of_signals = change_timestamp_plots();
    //    }
    //    else
    //    {
    //        // если нет видео, то переход по сигналам примерно каждые 100 мс
    //        if (m_ptr_video_windows.empty() && m_current_timestamp > m_ptr_plot_windows.front()->get_current_time() + 100)
    //        {
    //            is_end_of_signals = change_timestamp_plots();
    //        }
    //    }

    m_mutex.unlock();

    //    auto end = clock();
    //    double seconds = (double)(end - start) / CLOCKS_PER_SEC;
    //    qDebug() << seconds << " !!!!!";

    m_is_end = is_end_of_videos & is_end_of_signals;

    if (m_is_end)
    {
        pause();
    }
}

bool Time_Mediator::change_timestamp_plots() const noexcept
{   
    bool is_end_of_all = true;
    for (auto p : m_ptr_plot_windows)
    {
        p->update_time1_value(m_current_timestamp);
        if (m_current_timestamp < p->get_log_time())
        {
            is_end_of_all = false;
        }
    }
    return is_end_of_all;
}

bool Time_Mediator::change_timestamp_videos(bool* is_frame_changed) const noexcept
{
    *is_frame_changed = false;
    bool is_end_of_all = true;

    std::vector<bool> frame_changed_vector;
    frame_changed_vector.resize(m_ptr_video_windows.size());
    std::fill(frame_changed_vector.begin(), frame_changed_vector.end(), false);

    std::vector<std::thread> threads;

    for (int i = 0; i < m_ptr_video_windows.size(); ++i)
    {
        threads.push_back(std::thread([this, i, &frame_changed_vector]()
        {
            frame_changed_vector[i] = m_ptr_video_windows[i]->recieve_position_in_ms(m_current_timestamp, m_skip_count);
        }));
    }

    for (auto& t : threads)
    {
        t.join();
    }

    int i = 0;
    for (auto p : m_ptr_video_windows)
    {
        if (frame_changed_vector[i])
        {
            *is_frame_changed = true;
        }
        if (m_current_timestamp < p->get_maximum_timestamp())
        {
            is_end_of_all = false;
        }
    }

    //            for (auto p : m_ptr_video_windows)
    //            {
    //                bool frame_changed = p->recieve_position_in_ms(m_current_timestamp, m_skip_count);
    //                if (frame_changed)
    //                {
    //                    *is_frame_changed = true;
    //                }
    //                if (m_current_timestamp < p->get_maximum_timestamp())
    //                {
    //                    is_end_of_all = false;
    //                }
    //            }
    return is_end_of_all;
}

void Time_Mediator::goto_regarding_current_time(int time_diff) noexcept
{
    if (m_current_timestamp == 0 && time_diff < 0)
    {
        return;
    }

    m_mutex.lock();

    pause();

    m_mutex.unlock();

    uint64_t timestamp = std::max(0l, static_cast<int64_t>(m_current_timestamp) + time_diff);

    change_timestamp(timestamp);
}

uint64_t Time_Mediator::get_current_timestamp() const noexcept
{
    return m_current_timestamp;
}

// TIMESTAMP />

// < PLOTS DELTA TIME (TIME WINDOW WIDTH)
void Time_Mediator::change_plot_delta_time_slot(unsigned int delta_time) noexcept
{
    if (m_plot_delta_time == delta_time)
    {
        return;
    }

    m_plot_delta_time = delta_time;
    change_plot_delta_time();
}

unsigned int Time_Mediator::get_plot_delta_time() const noexcept
{
    return m_plot_delta_time;
}

void Time_Mediator::change_plot_delta_time() const noexcept
{
    for (auto p : m_ptr_plot_windows)
    {
        p->update_deltaT_value(m_plot_delta_time);
    }
}

// PLOTS DELTA TIME (TIME WINDOW WIDTH) />

// < VIDEO_SPEED
void Time_Mediator::change_video_speed_request(int speed) noexcept
{
    if (m_video_speed == speed)
    {
        return;
    }

    m_skip_count = 0;

    m_video_speed = speed;

    change_playback_speed();
}

void Time_Mediator::video_speed_down_request() noexcept
{
    if (m_video_speed > SPEED_MIN)
    {
        m_skip_count = 0;
        --m_video_speed;
        change_playback_speed();
    }
}

void Time_Mediator::video_speed_up_request() noexcept
{
    if (m_video_speed < SPEED_MAX)
    {
        m_skip_count = 0;
        ++m_video_speed;
        change_playback_speed();
    }
}

void Time_Mediator::change_playback_speed() const noexcept
{
    qDebug().noquote() << "set playback" << SPEED_LABELS[m_video_speed];

    if (m_ptr_metronom != nullptr)
    {
        m_ptr_metronom->change_speed(m_video_speed);
    }

    for (auto p : m_ptr_video_windows)
    {
        //qDebug() << "MED:";
        p->update_video_speed(m_video_speed);
    }

    for (auto p : m_ptr_plot_windows)
    {
        //qDebug() << "MED:";
        p->set_speed(m_video_speed);
    }
}

int Time_Mediator::get_video_speed() const noexcept
{
    return m_video_speed;
}

// VIDEO_SPEED >

// < PLAY / PAUSE

void Time_Mediator::play_pause() noexcept
{
    if (m_is_playing)
    {
        pause();
    }
    else
    {
        play();
    }
}

void Time_Mediator::play() noexcept
{
    if (m_is_playing)
    {
        return;
    }

    m_is_playing = true;

    for (auto p : m_ptr_video_windows)
    {
        p->play_video();
    }

    if (m_ptr_metronom != nullptr)
    {
        m_ptr_metronom->play();
    }
}

void Time_Mediator::pause() noexcept
{
    if (!m_is_playing)
    {
        return;
    }

    m_skip_count = false;

    if (m_ptr_metronom != nullptr)
    {
        m_ptr_metronom->pause();
    }

    m_is_playing = false;

    for (auto p : m_ptr_video_windows)
    {
        p->pause_video();
    }
}

bool Time_Mediator::is_playing() const noexcept
{
    return m_is_playing;
}

// PLAY / PAUSE >


void Time_Mediator::mutex_lock() noexcept
{
    m_mutex.lock();
}

bool Time_Mediator::mutex_try_lock() noexcept
{
    return m_mutex.try_lock();
}

void Time_Mediator::mutex_unlock() noexcept
{
    m_mutex.unlock();
}
