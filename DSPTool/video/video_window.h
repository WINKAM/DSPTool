#ifndef VIDEO_WINDOW_H
#define VIDEO_WINDOW_H

#include <QWidget>

class QLabel;
class QBoxLayout;
class OpenCV_Player;
class QPushButton;
class QSlider;
class QSpinBox;
class QString;
class QComboBox;
class Log_Labels;
class Label_Item;
class QClipboard;
class QCheckBox;
class QLineEdit;

class Video_Window : public QWidget
{
    Q_OBJECT

private:
    QLabel* m_video_screen;
    QLabel* m_current_time_label;
    QLabel* m_current_frame_label;
    QLabel* m_speed_label;
    QLabel* m_label_information;
    QPushButton* m_play_pause_button;
    QSlider* m_frames_slider;
    QSlider* m_speed_slider;

    QComboBox* m_frame_time_switcher;
    QLineEdit* m_goto_edit;
    QPushButton* m_goto_button;
    QTimer* m_timer_clear_message;

    QString m_video_file_path;

    OpenCV_Player* m_video_controller;

    uint64_t m_timestamp_viewed;

    uint64_t m_start_timestamp;

    int m_count_repeat_q_key;

    //int m_count_no_skip_frames;
    //double m_avg_fps;

    bool m_is_closing;
    bool m_is_play_pause_btn_show_play;

public:
    explicit Video_Window(QWidget *parent = nullptr);
    bool open_video_file(const QString &video_file) noexcept;
    void run_video_player() noexcept;
    uint64_t get_maximum_timestamp() noexcept;
    void set_start_timestamp(uint64_t start_timestamp) noexcept;
    ~Video_Window();

signals:    
    void close_video_signal(const QString &video_file);
    void position_in_ms_changed(uint64_t ms);
    void position_changes_regarding_current(int time_diff);

    void position_in_frame_changed(long frame);
    void video_speed_changed(int value);
    void press_play_button_signal();
    void press_pause_button_signal();

public slots:
    void update_image(QImage *image) noexcept;
    void video_loaded() noexcept;
    void play_video() noexcept;
    void pause_video() noexcept;
    bool recieve_position_in_ms(uint64_t timestamp, int fps = 0) noexcept;
    void update_video_speed(int value) noexcept;

protected:
    void create_gui() noexcept;
    void copy_timestamp() noexcept;
    QBoxLayout* create_video_controls() noexcept;
    void create_hotkeys() noexcept;

    void update_widgets_view(long frame, bool is_no_frame) noexcept;

    void send_speed_changed(int speed) noexcept;
    void send_position_changes_regarding_current(int time_diff);
    void send_timestamp_changed_by_slider() noexcept;
    void send_timestamp_changed(uint64_t timestamp) noexcept;

    void resizeEvent(QResizeEvent *event);
    void closeEvent(QCloseEvent *event);
    bool event(QEvent *event);
};

#endif // VIDEO_WINDOW_H
