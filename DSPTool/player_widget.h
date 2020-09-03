/***************************************************************************

             WINKAM TM strictly confidential 28.04.2020

 ***************************************************************************/
#ifndef PLAYER_WIDGET_H
#define PLAYER_WIDGET_H

#include <QWidget>

class QLabel;
class QSlider;
class QSpinBox;
class QLineEdit;
class QComboBox;
class QPushButton;
class QCloseEvent;

class Player_Widget : public QWidget
{
    Q_OBJECT
public:
    explicit Player_Widget(QWidget *parent = nullptr);
    ~Player_Widget();

signals:

public slots:

protected:
    void create_widgets() noexcept;
    void create_shortcuts() noexcept;
    void set_window_state() noexcept;
    void save_config() noexcept;
    void closeEvent(QCloseEvent *event);

private:
    QLabel* m_current_time_label;
    QSlider* m_time_slider;
    //QSlider* m_time_shift_slider;
    QSlider* m_speed_slider;
    QSpinBox* m_timestamp_spinbox;
    QSpinBox* m_frame_spinbox;
    QLineEdit* m_label_tags_lineedit;
    QComboBox* m_label_object_combobox;
    QComboBox* m_label_event_combobox;
    QComboBox* m_label_subtype_combobox;
    QPushButton* m_goto_first_label_btn;
    QPushButton* m_goto_last_label_btn;
    QPushButton* m_goto_prev_label_btn;
    QPushButton* m_goto_next_label_btn;

    QPushButton* m_play_pause_btn;
    QPushButton* m_stop_btn;

    bool m_is_close_blocked;
};

#endif // PLAYER_WIDGET_H
