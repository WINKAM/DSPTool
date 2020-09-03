/***************************************************************************

             WINKAM TM strictly confidential 30.04.2020

 ***************************************************************************/
#ifndef TIME_MEDIATOR_H
#define TIME_MEDIATOR_H

#include <QObject>
#include <cstdint>
#include <vector>
#include <mutex>

class Base_Plot_Window;
class Video_Window;
class Metronom;

class Time_Mediator : public QObject
{
    Q_OBJECT

public:
    static Time_Mediator& get_instance()
    {
        static Time_Mediator instance;
        return instance;
    }

    void mutex_lock() noexcept;
    bool mutex_try_lock() noexcept;
    void mutex_unlock() noexcept;

    void set_metronom_ptr(Metronom* ptr_metronom) noexcept;

    void add_ptr_plot_window(Base_Plot_Window* ptr_pw) noexcept;
    void add_ptr_video_window(Video_Window* ptr_vw) noexcept;

    void remove_ptr_video_window(Video_Window* ptr_vw_removed) noexcept;

    void clear_ptr_plot_windows() noexcept;
    void clear_ptr_video_windows() noexcept;

    unsigned int get_plot_delta_time() const noexcept;

    uint64_t get_current_timestamp() const noexcept;

    int get_video_speed() const noexcept;

    bool is_playing() const noexcept;

public slots:
    void change_plot_delta_time_slot(unsigned int delta_time) noexcept;
    void change_timestamp(uint64_t timestamp) noexcept;
    void change_timestamp_by_metronom(qulonglong timestamp, int skip_count) noexcept;

    void change_video_speed_request(int speed) noexcept;
    void video_speed_down_request() noexcept;
    void video_speed_up_request() noexcept;

    void goto_regarding_current_time(int time_diff) noexcept;

    void play() noexcept;
    void pause() noexcept;
    void play_pause() noexcept;

protected:
    void change_plot_delta_time() const noexcept;
    bool change_timestamp_plots() const noexcept;
    bool change_timestamp_videos(bool *is_frame_changed) const noexcept;
    void change_playback_speed() const noexcept;

private:
    explicit Time_Mediator(QObject* parent = nullptr);
    ~Time_Mediator()= default;
    Time_Mediator(const Time_Mediator&)= delete;
    Time_Mediator& operator=(const Time_Mediator&)= delete;

    std::vector<Base_Plot_Window*> m_ptr_plot_windows;
    std::vector<Video_Window*> m_ptr_video_windows;
    Metronom* m_ptr_metronom;

    std::mutex m_mutex;

    uint64_t m_current_timestamp;
    unsigned int m_plot_delta_time;
    int m_video_speed;
    int m_skip_count;
    bool m_is_playing;
    bool m_is_end;
};


#endif // TIME_MEDIATOR_H
