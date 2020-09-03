#include "video_window.h"

#include <QDebug>
#include <QVBoxLayout>
#include <QLabel>
#include <QResizeEvent>
#include <QPushButton>
#include <QSlider>
#include <QSpinBox>
#include <QTimer>
#include <QComboBox>
#include <QCheckBox>
#include <QLineEdit>
#include "../utils.h"
#include "opencv_player.h"
#include "settings_manager.h"
#include <QGuiApplication>
#include <QScreen>
#include <QShortcut>
#include <QClipboard>
#include <QMessageBox>
#include <QTimer>
#include <QDateTime>
#include <ctime>
#include <QMenuBar>

#include "time_mediator.h"
#include "speed_convertor.h"
#include "main_menu_mediator.h"

Video_Window::Video_Window(QWidget *parent) : QWidget(parent)
{
    create_gui();
    create_hotkeys();

    m_video_controller = new OpenCV_Player();

    m_count_repeat_q_key = 0;

    //qDebug() << width << height;

    int position_x = Settings_Manager::get_instance()->get_window_parameters("Video_Window", "position_x");
    int position_y = Settings_Manager::get_instance()->get_window_parameters("Video_Window", "position_y");

    move(position_x, position_y);

    m_is_closing = false;
    m_is_play_pause_btn_show_play = true;
    m_start_timestamp = 0;
    //m_count_no_skip_frames = 0;
    //m_avg_fps = 25.;
    setWindowFlag(Qt::Dialog);
}

void Video_Window::create_gui() noexcept
{
    QVBoxLayout *central_layout = new QVBoxLayout(this);
    central_layout->setContentsMargins(0, 0, 0, 0);
    central_layout->setAlignment(Qt::AlignTop);

    central_layout->addLayout(create_video_controls());

    setMinimumHeight(540 + 36); // 36 is height of layout with controls
    setMinimumWidth(960);

    qDebug() << "Video_Window::create_gui";

    central_layout->setMenuBar(Main_Menu_Mediator::get_instance().create_menu_bar(this));
}

QBoxLayout* Video_Window::create_video_controls() noexcept
{
    QVBoxLayout* result = new QVBoxLayout();
    result->setContentsMargins(0, 0, 0, 0);
    result->setSpacing(0);

    m_video_screen = new QLabel();
    m_video_screen->setStyleSheet("QWidget { background-color : black}");
    result->addWidget(m_video_screen);
    m_video_screen->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    m_current_frame_label = new QLabel("F# 1");
    m_current_frame_label->setStyleSheet("QWidget {background-color : #00ffffff; color : orange; font: 14pt;}");
    m_current_frame_label->setAlignment(Qt::AlignRight);

    m_speed_label = new QLabel(SPEED_LABELS[Time_Mediator::get_instance().get_video_speed()] /* +  "; FPS: " + QString::number(static_cast<int>(m_avg_fps + 0.5))*/);
    m_speed_label->setStyleSheet("QWidget {background-color : #00ffffff; color : orange; font: 14pt;}");
    m_speed_label->setAlignment(Qt::AlignTop);

    QLabel* copyright = new QLabel("WINKAM\u2122 STRICTLY CONFIDENTIAL. FOR INTERNAL USE ONLY");
    copyright->setStyleSheet("QWidget {background-color : #00ffffff; color : white; font: 14pt;}");
    copyright->setAlignment(Qt::AlignBottom);

    m_current_time_label = new QLabel("00:00:00.000     0 ms");
    m_timestamp_viewed = 0;
    m_current_time_label->setStyleSheet("QWidget {background-color : #00ffffff; color : orange; font: 14pt;}");
    m_current_time_label->setAlignment(Qt::AlignRight);

    m_label_information = new QLabel("");
    m_label_information->setStyleSheet("QWidget {background-color : #00ffffff; color : orange; font: 14pt;}");
    m_label_information->setAlignment(Qt::AlignRight);

    m_timer_clear_message = new QTimer(this);
    m_timer_clear_message->setInterval(1000);
    m_timer_clear_message->setSingleShot(true);
    QObject::connect(m_timer_clear_message, &QTimer::timeout, [=] () {m_label_information->clear();});

    QVBoxLayout* vbox = new QVBoxLayout();
    vbox->addWidget(m_current_frame_label);
    vbox->addWidget(m_current_time_label);
    vbox->addWidget(m_label_information);
    vbox->setAlignment(Qt::AlignTop);

    QHBoxLayout* hbox = new QHBoxLayout();
    hbox->addWidget(m_speed_label);
    hbox->addStretch();
    hbox->addLayout(vbox);

    QHBoxLayout* hbox2 = new QHBoxLayout();
    hbox2->setAlignment(Qt::AlignRight);
    hbox2->addWidget(copyright);

    QVBoxLayout* vbox_base = new QVBoxLayout(m_video_screen);
    vbox_base->addLayout(hbox);
    vbox->setAlignment(Qt::AlignTop);
    vbox_base->addLayout(hbox2);

    QHBoxLayout* controls_layout = new QHBoxLayout();
    controls_layout->setContentsMargins(3, 3, 3, 3);
    controls_layout->setSpacing(6);
    controls_layout->setAlignment(Qt::AlignLeft);
    result->addLayout(controls_layout);

    m_play_pause_button = new QPushButton();
    m_play_pause_button->setToolTip("play/pause (Space)");
    m_play_pause_button->setIcon(QIcon(":/image/play.png"));
    m_play_pause_button->setMinimumHeight(30);
    m_play_pause_button->setMinimumWidth(30);
    controls_layout->addWidget(m_play_pause_button);
    QObject::connect(m_play_pause_button, &QPushButton::clicked, [this]()
    {
        if (m_is_play_pause_btn_show_play)
        {
            emit press_play_button_signal();
        }
        else
        {
            emit press_pause_button_signal();
        }
    });

    //    QPushButton* stop_button = new QPushButton();
    //    stop_button->setIcon(QIcon(":/image/stop.png"));
    //    stop_button->setMinimumHeight(30);
    //    stop_button->setMinimumWidth(30);
    //    controls_layout->addWidget(stop_button);

    m_speed_slider = new QSlider();
    m_speed_slider->setOrientation(Qt::Horizontal);
    m_speed_slider->setMinimum(SPEED_MIN);
    m_speed_slider->setMaximum(SPEED_MAX);
    m_speed_slider->setValue(Time_Mediator::get_instance().get_video_speed());
    m_speed_slider->setMaximumWidth(70);
    m_speed_slider->setToolTip("speed up (X) or slow down (Z) playback rate");
    m_speed_slider->setFocusPolicy(Qt::FocusPolicy::NoFocus);
    QObject::connect(m_speed_slider, &QSlider::valueChanged, this, &Video_Window::send_speed_changed);

    controls_layout->addSpacing(6);
    controls_layout->addWidget(new QLabel("Pl.speed:"));
    controls_layout->addWidget(m_speed_slider);
    controls_layout->addSpacing(6);

    m_frames_slider = new QSlider();
    m_frames_slider->setOrientation(Qt::Horizontal);
    m_frames_slider->setMinimum(1);
    m_frames_slider->setFocusPolicy(Qt::FocusPolicy::NoFocus);
    QObject::connect(m_frames_slider, &QSlider::valueChanged, this, &Video_Window::send_timestamp_changed_by_slider);
    QObject::connect(m_frames_slider, &QSlider::sliderPressed, [this]()
    {
        if (!m_is_play_pause_btn_show_play)
        {
            emit press_pause_button_signal();
        }
    });


    QPushButton* copy_timestamp_btn = new QPushButton("CpTm");
    //copy_timestamp_btn->setStyleSheet("font-size: 12px;");
    copy_timestamp_btn->setMaximumWidth(50);
    copy_timestamp_btn->setToolTip("copy timestamp (P)");

    QObject::connect(copy_timestamp_btn, &QPushButton::clicked, [this]()
    {
        copy_timestamp();
    });

    controls_layout->addWidget(m_frames_slider);
    controls_layout->addWidget(copy_timestamp_btn);
    controls_layout->addSpacing(12);

    m_frame_time_switcher = new QComboBox();
    m_frame_time_switcher->addItems({"tmstmp#", "frame#"});
    m_frame_time_switcher->setCurrentIndex(0);
    m_frame_time_switcher->setFocusPolicy(Qt::FocusPolicy::NoFocus);
    controls_layout->addWidget(m_frame_time_switcher);

    m_goto_edit = new QLineEdit(QString::number(0));
    m_goto_edit->setText("0");
    m_goto_edit->setToolTip("go-to video frame# OR timestamp# (Enter)");
    m_goto_edit->setFixedWidth(120);
    m_goto_edit->setValidator(new QRegExpValidator(QRegExp("[+-]?\\d*[\\.]?\\d+")));
    controls_layout->addSpacing(-6);
    controls_layout->addWidget(m_goto_edit);

    m_goto_button = new QPushButton("Go-to");
    m_goto_button->setToolTip("go-to video frame# OR timestamp# (Enter)");
    m_goto_button->setFixedWidth(50);
    m_goto_button->setMinimumHeight(30);
    QObject::connect(m_goto_button, &QPushButton::clicked, [this]()
    {
        if (!m_is_play_pause_btn_show_play)
        {
            m_play_pause_button->click();
        }

        uint64 timestamp;
        if (m_frame_time_switcher->currentIndex() == 0) // time
        {
            timestamp = m_goto_edit->text().toULongLong();
        }
        else // frame
        {
            timestamp = m_start_timestamp + (m_goto_edit->text().toLong() - 1) / m_video_controller->get_frame_rate() * 1000;
            if (timestamp <= 0)
            {
                m_goto_edit->setText("1"); // frames from 1
                timestamp = 0;
            }
        }

        send_timestamp_changed(timestamp);
    });

    controls_layout->addWidget(m_goto_button);

    return result;
}

void Video_Window::create_hotkeys() noexcept
{
    // Через обычные QShortcut сделаны клавиши, не связанные с перемоткой, потому что для них не нужно особой обработки на случай "зажать и держать".
    QObject::connect(new QShortcut(QKeySequence(Qt::Key_Z), this), &QShortcut::activated, [=] ()
    {
        m_speed_slider->setValue(m_speed_slider->value() - 1);
    });
    QObject::connect(new QShortcut(QKeySequence(Qt::Key_X), this), &QShortcut::activated, [=] ()
    {
        m_speed_slider->setValue(m_speed_slider->value() + 1);
    });

    QObject::connect(new QShortcut(QKeySequence(Qt::Key_Space), this), SIGNAL(activated()), m_play_pause_button, SLOT(click()));

    QObject::connect(new QShortcut(QKeySequence(Qt::Key_P), this), &QShortcut::activated, [=] ()
    {
        copy_timestamp();
    });
}

bool Video_Window::event(QEvent *event)
{
    if (event->type() == QEvent::KeyRelease)
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent* >(event);

        if (keyEvent->key() == Qt::Key_Q || keyEvent->key() == 1049) // 1049 - Й
        {
            if (!keyEvent->isAutoRepeat())
            {
                m_count_repeat_q_key = 0;
            }
            else
            {
                if (++m_count_repeat_q_key == 10)
                {
                    m_count_repeat_q_key = 0;
                }
            }

            if (m_count_repeat_q_key == 0)
            {
                if (QGuiApplication::keyboardModifiers() & Qt::ShiftModifier)
                {
                    send_position_changes_regarding_current(-1 * static_cast<int>(1000. / m_video_controller->get_frame_rate() * 10));
                }
                else
                {
                    send_position_changes_regarding_current(-1 * static_cast<int>(1000. / m_video_controller->get_frame_rate() + 0.5));
                }
            }
            return true;
        }
        if (keyEvent->key() == Qt::Key_W || keyEvent->key() == 1062) // 1062 - Ц
        {
            if (QGuiApplication::keyboardModifiers() & Qt::ShiftModifier)
            {
                send_position_changes_regarding_current(static_cast<int>(1000. / m_video_controller->get_frame_rate() * 10));
            }
            else
            {
                send_position_changes_regarding_current(static_cast<int>(1000. / m_video_controller->get_frame_rate() + 0.5));
            }
            return true;
        }
        if ((keyEvent->key() == Qt::Key_S || keyEvent->key() == 1067) && !keyEvent->isAutoRepeat()) // 1067 - Ы
        {
            if (QGuiApplication::keyboardModifiers() & Qt::ShiftModifier)
            {
                send_position_changes_regarding_current(10000); // 10 sec
            }
            else
            {
                send_position_changes_regarding_current(1000); // 1 sec
            }
            return true;
        }
        if ((keyEvent->key() == Qt::Key_A || keyEvent->key() == 1060) && !keyEvent->isAutoRepeat()) // 1060 - Ф
        {
            if (QGuiApplication::keyboardModifiers() & Qt::ShiftModifier)
            {
                send_position_changes_regarding_current(-10000); // 10 sec
            }
            else
            {
                send_position_changes_regarding_current(-1000); // 1 sec
            }
            return true;
        }
        if (keyEvent->key() == Qt::Key_S && !keyEvent->isAutoRepeat())
        {
            send_position_changes_regarding_current(1000); // 1 sec
            return true;
        }

        if (keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter)
        {
            m_goto_button->click();
            setFocus();
            return true;
        }
    }
    //    else if (event->type() == QEvent::KeyPress)
    //    {
    //        QKeyEvent *keyEvent = static_cast<QKeyEvent* >(event);
    //        if (keyEvent->key() == Qt::Key_Q || keyEvent->key() == 1049) // 1049 - Й
    //        {
    //            if (!keyEvent->isAutoRepeat())
    //            {
    //                m_is_hold_key = true;
    //            }
    //            return true;
    //        }
    //    }
    return QWidget::event(event);
}

void Video_Window::copy_timestamp() noexcept
{
    m_timer_clear_message->stop();

    m_label_information->setText("Timestamp is copied to clipboard");
    qDebug().noquote() << "timestamp" << QString::number(m_timestamp_viewed) << "ms is copied to clipboard";
    QGuiApplication::clipboard()->setText(QString::number(m_timestamp_viewed));

    m_timer_clear_message->start();
}

bool Video_Window::open_video_file(const QString &video_file) noexcept
{
    m_video_file_path = video_file;
    setWindowTitle(QString("DSP Tool v.") + APP_VERSION + " Video " + video_file.split("/").back());

    qDebug() << "Open video" << video_file;

    if (m_video_controller->open(video_file))
    {
        auto ptr_img = m_video_controller->get_frame();
        update_image(ptr_img);
        return true;
    }
    else
    {
        qDebug() << "Error: cannot open video file" << video_file;
    }

    return false;
}

void Video_Window::run_video_player() noexcept
{
    show();
}

void Video_Window::set_start_timestamp(uint64_t start_timestamp) noexcept
{
    m_start_timestamp = start_timestamp;
}

uint64_t Video_Window::get_maximum_timestamp() noexcept
{
    return m_start_timestamp + static_cast<uint64_t>((m_video_controller->get_video_frames_count() - 1) * 1000. / m_video_controller->get_frame_rate());
}

void Video_Window::video_loaded() noexcept
{
    m_frames_slider->setMaximum(m_video_controller->get_video_frames_count());
    m_play_pause_button->setIcon(QIcon(":/image/play.png"));
    m_video_screen->setScaledContents(true);
}

void Video_Window::play_video() noexcept
{
    if (m_is_play_pause_btn_show_play)
    {
        m_play_pause_button->setIcon(QIcon(":/image/pause.png"));
        m_is_play_pause_btn_show_play = false;
    }
}

void Video_Window::pause_video() noexcept
{
    if (!m_is_play_pause_btn_show_play)
    {
        m_play_pause_button->setIcon(QIcon(":/image/play.png"));
        m_is_play_pause_btn_show_play = true;
    }
}

void Video_Window::update_image(QImage *image) noexcept
{
    //m_video_screen->setScaledContents(true);
    m_video_screen->setPixmap(QPixmap::fromImage(*image));
}

void Video_Window::update_widgets_view(long frame, bool is_no_frame) noexcept
{
    if (m_frames_slider->value() != frame && !is_no_frame)
    {
        QObject::disconnect(m_frames_slider, &QSlider::valueChanged, this, &Video_Window::send_timestamp_changed_by_slider);
        m_frames_slider->setValue(frame);
        QObject::connect(m_frames_slider, &QSlider::valueChanged, this, &Video_Window::send_timestamp_changed_by_slider);
    }

    if (is_no_frame)
    {
        m_current_frame_label->setText("NO VIDEO AT THIS MOMENT");
    }
    else
    {
        m_current_frame_label->setText("F# " + QString::number(frame));
    }

    uint64_t timestamp = Time_Mediator::get_instance().get_current_timestamp();
    m_current_time_label->setText(UTILS::time_to_string(timestamp) + "     " + QString::number(timestamp) + " ms");

    m_timestamp_viewed = timestamp;
    m_label_information->clear();
}

bool Video_Window::recieve_position_in_ms(uint64_t timestamp, int fps) noexcept
{
    if (this->isMinimized())
    {
        return true;
    }

    long frame = 1 + (0.5 + (timestamp * 1. - m_start_timestamp) * m_video_controller->get_frame_rate() / 1000);

    bool is_no_frame = (frame < 1) || (frame > m_video_controller->get_video_frames_count());

    //auto start = clock();

    int code_goto_result = is_no_frame ? 0 : m_video_controller->goto_frame_id(frame);

    //auto end = clock();
    //double seconds = (double)(end - start) / CLOCKS_PER_SEC;
    //qDebug() << seconds << "goto frame !!!!!" << windowTitle();

    if (code_goto_result == -1)
    {
        qDebug() << "Warning: cannot open frame id" << frame << "; timeline timestamp " << timestamp;
    }
    else
    {
        update_widgets_view(frame, is_no_frame);

        if (code_goto_result == 1)
        {
            QImage* ptr_img;
            ptr_img = m_video_controller->get_frame();
            update_image(ptr_img);
        }
        else if (is_no_frame)
        {
            m_video_screen->clear();
        }

        m_speed_label->setText(SPEED_LABELS[m_speed_slider->value()] + "; FPS: " + QString::number(std::min(m_video_controller->get_frame_rate_round(), fps)));

        //m_avg_fps = m_avg_fps * 0.8 + 25.0 / (skip_frames_count + 1) * 0.2;

        //qDebug() << skip_frames_count << m_video_controller->get_frame_rate()
        //         << 25. / (skip_frames_count + 1)
        //         << m_avg_fps;
        //       m_speed_label->setText(SPEED_LABELS[m_speed_slider->value()] +  "; FPS: " + QString::number(static_cast<int>(m_avg_fps + 0.5)));

        //if ((m_speed_slider->value() <= SPEED_NORMAL && m_avg_fps < 18.)
        //        || (m_avg_fps < 10.))
        //{
        //    m_count_no_skip_frames = 0;
        //    m_speed_label->setText(SPEED_LABELS[m_speed_slider->value()] + "; try to hide signals or slow down playback speed");
        //}
        //else
        //{
        //    if(++m_count_no_skip_frames > 10 || m_count_no_skip_frames == -1)
        //    {
        //        m_speed_label->setText(SPEED_LABELS[m_speed_slider->value()]);
        //        m_count_no_skip_frames = -1;
        //    }
        //}
    }

    return (code_goto_result == 1);
}

void Video_Window::send_position_changes_regarding_current(int time_diff)
{
//    uint16_t timestamp_in_video = (m_frames_slider->value() - 1) * 1000. / m_video_controller->get_frame_rate();

//    if (timestamp_in_video + time_diff < 0)
//    {
//        time_diff = -timestamp_in_video;
//    }
//    else if (timestamp_in_video + time_diff >= (m_video_controller->get_video_frames_count() - 1) * 1000. / m_video_controller->get_frame_rate())
//    {
//        time_diff = (m_video_controller->get_video_frames_count() - 1) * 1000. / m_video_controller->get_frame_rate() - timestamp_in_video;
//    }

    emit position_changes_regarding_current(time_diff);
}

void Video_Window::send_timestamp_changed_by_slider() noexcept
{
    uint64 timestamp = m_start_timestamp + (m_frames_slider->value() - 1) * 1000. / m_video_controller->get_frame_rate();
    emit position_in_ms_changed(timestamp);
}

void Video_Window::send_timestamp_changed(uint64_t timestamp) noexcept
{
    //timestamp = static_cast<uint64_t>(std::min((m_video_controller->get_video_frames_count() - 1) * 1000. / m_video_controller->get_frame_rate(), 1. * timestamp));
    if (timestamp >= m_start_timestamp)
    {
        emit position_in_ms_changed(timestamp);
    }
}

void Video_Window::send_speed_changed(int speed) noexcept
{
    emit video_speed_changed(m_speed_slider->value());
}

void Video_Window::update_video_speed(int value) noexcept
{
    QObject::disconnect(m_speed_slider, &QSlider::valueChanged, this, &Video_Window::send_speed_changed);
    m_speed_slider->setValue(value);
    m_speed_label->setText(SPEED_LABELS[value]/* +  "; FPS: " + QString::number(static_cast<int>(m_avg_fps + 0.5))*/);
    QObject::connect(m_speed_slider, &QSlider::valueChanged, this, &Video_Window::send_speed_changed);
    //m_count_no_skip_frames = -1;
}

/// help https://stackoverflow.com/a/43936590
void Video_Window::resizeEvent(QResizeEvent *event)
{
    if (m_video_screen->pixmap() != nullptr && !m_video_screen->pixmap()->isNull())
    {
        int pixmap_width = m_video_screen->pixmap()->width();
        int pixmap_height = m_video_screen->pixmap()->height();

        if (pixmap_width <= 0 || pixmap_height <= 0)
        {
            return;
        }

        int w = m_video_screen->width();
        int h = m_video_screen->height();

        if (w <= 0 || h <= 0)
        {
            return;
        }

        if (w * pixmap_height > h * pixmap_width)
        {
            int m = (w - (pixmap_width * h / pixmap_height)) / 2;
            m_video_screen->setContentsMargins(m, 0, m, 0);
        }
        else
        {
            int m = (h - (pixmap_height * w / pixmap_width)) / 2;
            m_video_screen->setContentsMargins(0, m, 0, m);
        }
    }
}

void Video_Window::closeEvent(QCloseEvent *event)
{
    Settings_Manager::get_instance()->write_window_parameters("Video_Window", "position_x", geometry().x());
    Settings_Manager::get_instance()->write_window_parameters("Video_Window", "position_y", geometry().y());

    // Если невидео не на паузе, то при попытке закрытия имитируется постановка на паузу.
    // И через таймаут уже закрывается окно на самом деле.
    if (Time_Mediator::get_instance().is_playing())
    {
        if (!m_is_closing)
        {
            if (Time_Mediator::get_instance().is_playing())
            {
                m_play_pause_button->click();
                event->ignore();

                QTimer::singleShot(300, [this]() {this->close();});

                m_is_closing = true;

                return;
            }
        }
    }

    delete m_video_controller;

    Time_Mediator::get_instance().remove_ptr_video_window(this);

    emit close_video_signal(m_video_file_path);

    qDebug() << "Video window " << m_video_file_path << "is closing...";
}


Video_Window::~Video_Window()
{
    qDebug() << "Video window " << m_video_file_path << "closed";
}
