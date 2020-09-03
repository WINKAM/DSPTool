/***************************************************************************

             WINKAM TM strictly confidential 28.04.2020

 ***************************************************************************/
#include "player_widget.h"
#include <QSlider>
#include <QPushButton>
#include <QLineEdit>
#include <QSpinBox>
#include <QLabel>
#include <QLayout>
#include <QComboBox>
#include <QCloseEvent>
#include <QScreen>
#include <QGuiApplication>
#include <QDebug>
#include <QShortcut>
#include <QIcon>
#include <QString>

#include "settings_manager.h"

Player_Widget::Player_Widget(QWidget *parent) : QWidget(parent)
{
    m_is_close_blocked = false;

    create_widgets();
    create_shortcuts();
    set_window_state();
}

void Player_Widget::set_window_state() noexcept
{
    //setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    setWindowFlags(Qt::WindowStaysOnTopHint | Qt::WindowMinimizeButtonHint);
    setFixedSize(1400, 100);
    setWindowTitle(QString("DSP Tool v.") + APP_VERSION + " Player");

    long position_x = Settings_Manager::get_instance()->get_window_parameters("Player", "position_x");
    long position_y = Settings_Manager::get_instance()->get_window_parameters("Player", "position_y");

    int full_width = 0;
    for (QScreen *item : QGuiApplication::screens())
    {
        full_width += item->geometry().width();
    }
    if (full_width - position_x < geometry().width())
    {
        position_x = full_width - geometry().width();
    }
    if (QGuiApplication::primaryScreen()->geometry().height() - position_y < geometry().height())
    {
        position_y = QGuiApplication::primaryScreen()->geometry().height() - geometry().height();
    }

    move(position_x, position_y);
}

void Player_Widget::create_widgets() noexcept
{
    QVBoxLayout* main_layout   = new QVBoxLayout();
    QHBoxLayout* layout_labels = new QHBoxLayout();
    QHBoxLayout* layout_player = new QHBoxLayout();
    QHBoxLayout* layout_label_controller = new QHBoxLayout();


    m_current_time_label =  new QLabel("0 ms\t\t00:00:00\t\t1 frame");
    m_current_time_label->setTextInteractionFlags(Qt::TextSelectableByMouse);
    QLabel* label_time =  new QLabel("time, ms");
    QLabel* label_frame = new QLabel("frame");
    QLabel* label_speed = new QLabel("speed x 1");


    layout_labels->addSpacing(90);
    layout_labels->addWidget(m_current_time_label);
    layout_labels->addStretch();
    layout_labels->addWidget(label_speed);
    layout_labels->addSpacing(15);
    layout_labels->addWidget(label_time);
    layout_labels->addSpacing(20);
    layout_labels->addWidget(label_frame);
    layout_labels->addSpacing(20);

    m_play_pause_btn = new QPushButton();
    m_play_pause_btn->setIcon(QIcon(":/image/play.png"));

    m_stop_btn = new QPushButton();
    m_stop_btn->setIcon(QIcon(":/image/stop.png"));

    m_speed_slider = new QSlider();
    m_speed_slider->setOrientation(Qt::Horizontal);
    m_speed_slider->setMinimum(0);
    m_speed_slider->setMaximum(3);
    m_speed_slider->setValue(1);
    m_speed_slider->setMaximumWidth(70);

    m_time_slider = new QSlider();
    m_time_slider->setOrientation(Qt::Horizontal);
    m_time_slider->setValue(0);
    m_time_slider->setMinimum(0);
    m_time_slider->setMaximum(10000);

    m_timestamp_spinbox = new QSpinBox();
    m_timestamp_spinbox->setRange(0, 10000);
    m_timestamp_spinbox->setValue(0);

    m_frame_spinbox = new QSpinBox();
    m_frame_spinbox->setRange(1, 10000);
    m_frame_spinbox->setValue(1);

    layout_player->addWidget(m_play_pause_btn);
    layout_player->addWidget(m_stop_btn);
    layout_player->addWidget(m_time_slider);
    layout_player->addWidget(m_speed_slider);
    layout_player->addWidget(m_timestamp_spinbox);
    layout_player->addWidget(m_frame_spinbox);

    m_label_object_combobox = new QComboBox();
    m_label_object_combobox->setFixedWidth(120);

    m_label_event_combobox = new QComboBox();
    m_label_event_combobox->setFixedWidth(120);

    m_label_subtype_combobox = new QComboBox();
    m_label_subtype_combobox->setFixedWidth(120);

    m_goto_first_label_btn = new QPushButton("<<");
    m_goto_first_label_btn->setFixedWidth(50);

    m_goto_prev_label_btn = new QPushButton("<");
    m_goto_prev_label_btn->setFixedWidth(50);

    m_goto_next_label_btn = new QPushButton(">");
    m_goto_next_label_btn->setFixedWidth(50);

    m_goto_last_label_btn = new QPushButton(">>");
    m_goto_last_label_btn->setFixedWidth(50);

    m_label_tags_lineedit = new QLineEdit();

    layout_label_controller->addWidget(new QLabel("Labels:"));
    layout_label_controller->addWidget(m_label_object_combobox);
    layout_label_controller->addWidget(m_label_event_combobox);
    layout_label_controller->addWidget(m_label_subtype_combobox);
    layout_label_controller->addWidget(m_goto_first_label_btn);
    layout_label_controller->addWidget(m_goto_prev_label_btn);
    layout_label_controller->addWidget(m_goto_next_label_btn);
    layout_label_controller->addWidget(m_goto_last_label_btn);
    layout_label_controller->addWidget(m_label_tags_lineedit);

    main_layout->addLayout(layout_labels);
    main_layout->addLayout(layout_player);
    main_layout->addLayout(layout_label_controller);

    setLayout(main_layout);
}

void Player_Widget::create_shortcuts() noexcept
{
//    QShortcut* key_speed_down = new QShortcut(this);
//    key_speed_down->setKey(Qt::Key_Z);
//    connect(key_speed_down, &QShortcut::activated, []() {qDebug() << "Z";});

//    QShortcut* key = new QShortcut(this);
//    key->setKey(Qt::Key_Q);
//    connect(key, &QShortcut::activated, []() {qDebug() << "ZZ";});
}

void Player_Widget::save_config() noexcept
{
    Settings_Manager::get_instance()->write_window_parameters("Player", "position_x", geometry().x());
    Settings_Manager::get_instance()->write_window_parameters("Player", "position_y", geometry().y());
}

void Player_Widget::closeEvent(QCloseEvent *event)
{
    qDebug() << "!!!!!";
    if (m_is_close_blocked)
    {
        event->ignore();
        showMinimized();
    }
    else
    {
        save_config();
    }
}

Player_Widget::~Player_Widget()
{
}
