#include "base_plot_window.h"

#include <QVBoxLayout>
#include <QLabel>
#include <QSlider>
#include <QSpinBox>
#include <QIntValidator>
#include <QDebug>
#include <vector>
#include <QPoint>
#include <tuple>
#include <cmath>
#include <QPolygonF>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QCheckBox>
#include <QAction>
#include <QGuiApplication>
#include <QClipboard>
#include <QScreen>
#include <QKeyEvent>
#include <QMenuBar>

#include "view/viewer_plot.h"
#include "data/object/wobjectdata.h"
#include "data/label/log_labels.h"
#include "../utils.h"
#include "data/label/alg_temp_labels.h"
#include "data/episode_meta_data.h"
#include "settings_manager.h"
#include "operation_mode.h"
#include "plots_mediator.h"
#include "time_mediator.h"
#include "main_menu_mediator.h"
#include "video/speed_convertor.h"

Base_Plot_Window::Base_Plot_Window(Plot_Type plot_type, QWidget *parent) : QWidget(parent)
{
    m_plot_type = plot_type;
    create_gui();

    m_objects_to_show = new std::map<QString, W_Object_Data *>();

    int position_x = Settings_Manager::get_instance()->get_window_parameters("Plot" + QString::number((int)m_plot_type), "position_x");
    int position_y = Settings_Manager::get_instance()->get_window_parameters("Plot" + QString::number((int)m_plot_type), "position_y");
    //    int full_width = 0;
    //    for (QScreen *item : QGuiApplication::screens())
    //    {
    //        full_width += item->geometry().width();
    //    }
    //    if (full_width - position_x < geometry().width())
    //    {
    //        position_x = full_width - geometry().width();
    //    }
    //    if (QGuiApplication::primaryScreen()->geometry().height() - position_y < geometry().height())
    //    {
    //        position_y = QGuiApplication::primaryScreen()->geometry().height() - geometry().height();
    //    }
    move(position_x, position_y);

    Plots_Mediator::get_instance().add_ptr_plot(this);

    m_count_repeat_q_key = 0;

    setWindowFlag(Qt::Dialog);
}

void Base_Plot_Window::create_gui() noexcept
{
    QVBoxLayout *central_layout = new QVBoxLayout(this);
    central_layout->setAlignment(Qt::AlignCenter);

    setMinimumHeight(720);
    setMinimumWidth(1280);

    central_layout->addLayout(create_plot());
    central_layout->addLayout(create_controls());

    central_layout->setMenuBar(Main_Menu_Mediator::get_instance().create_menu_bar(this));
}

QVBoxLayout* Base_Plot_Window::create_plot() noexcept
{
    QVBoxLayout *plot_layout = new QVBoxLayout();

    m_signal_plot = new Viewer_Plot(m_plot_type, this);

    m_current_time_label = new QLabel("00:00:00.000");
    m_current_time_label->setStyleSheet("QLabel {font: 14pt; color: orange;}");
    m_current_time_label->setAlignment(Qt::AlignTop);
    m_current_time_label->setMaximumHeight(50);
    m_current_time_label->setContentsMargins(0, 0, 0, 0);
    m_current_time_label->setMinimumWidth(400);

    m_speed_label = new QLabel(SPEED_LABELS[SPEED_NORMAL]);
    m_speed_label->setStyleSheet("QLabel {font: 14pt; color: orange;}");
    m_speed_label->setMinimumWidth(100);

    QHBoxLayout* hbox = new QHBoxLayout();
    hbox->addSpacing(85);
    hbox->addWidget(m_speed_label);
    hbox->addStretch();
    hbox->setAlignment(Qt::AlignTop);
    hbox->addWidget(m_current_time_label);

    QLabel* copyright_label = new QLabel("WINKAM ©");
    copyright_label->setStyleSheet("font: 14pt; color:gray");
    copyright_label->setAlignment(Qt::AlignTop);
    copyright_label->setMaximumHeight(50);
    copyright_label->setContentsMargins(0, 0, 0, 0);

    QHBoxLayout* hbox2 = new QHBoxLayout();
    m_ruler_checkbox = new QCheckBox("Ruler");
    m_ruler_checkbox->setStyleSheet("color: black; background-color: rgb(245, 245, 245);");
    m_ruler_checkbox->setToolTip("enable / disable ruler (Ctrl+R)");
    m_ruler_checkbox->setShortcut(QKeySequence("Ctrl+R"));

    m_zoom_checkbox = new QCheckBox("Zoom");
    m_zoom_checkbox->setToolTip("zoom mode");
    m_zoom_checkbox->setMinimumWidth(70);
    m_zoom_checkbox->setStyleSheet("color: black; background-color: rgb(245, 245, 245);");
    m_zoom_checkbox->setToolTip("enable / disable rectangle zoom (Ctrl+T)");
    m_zoom_checkbox->setShortcut(QKeySequence("Ctrl+T"));

    QObject::connect(m_zoom_checkbox, &QCheckBox::stateChanged, [=] (int value)
    {
        m_signal_plot->set_interation_type(value == Qt::Checked);
        if (value == Qt::Checked)
        {
            m_ruler_checkbox->setCheckState(Qt::Unchecked);
        }
    });

    QObject::connect(m_ruler_checkbox, &QCheckBox::stateChanged, [=] (int value)
    {
        m_signal_plot->set_ruler(value == Qt::Checked);
        if (value == Qt::Checked)
        {
            m_zoom_checkbox->setCheckState(Qt::Unchecked);
        }
    });

    hbox2->addSpacing(85);
    hbox2->addWidget(m_ruler_checkbox);
    hbox2->addWidget(m_zoom_checkbox);

    hbox2->addStretch();
    hbox2->setAlignment(Qt::AlignBottom);
    hbox2->addWidget(copyright_label);

    QVBoxLayout* vbox = new QVBoxLayout(m_signal_plot);
    vbox->addLayout(hbox);
    vbox->addLayout(hbox2);
    vbox->addSpacing(45);

    m_signal_plot->set_interation_type(false);

    plot_layout->addWidget(m_signal_plot);

    return plot_layout;
}

QVBoxLayout* Base_Plot_Window::create_controls() noexcept
{
    QVBoxLayout* result = new QVBoxLayout();

    std::vector<int> zoom_setting = Settings_Manager::get_instance()->get_plot_zoom(static_cast<int>(m_plot_type));

    QHBoxLayout* time_layout = new QHBoxLayout();
    m_time1_slider = new QSlider();
    m_time1_slider->setFocusPolicy(Qt::FocusPolicy::NoFocus);
    m_time1_slider->setOrientation(Qt::Horizontal);
    m_time1_slider->setPageStep(5000);
    m_time1_slider->setSingleStep(500);
    m_time1_slider->setProperty("min_time", 0);

    m_time1_spin = new QDoubleSpinBox();
    m_time1_spin->setDecimals(0); // QDoubleSpinBox for large timestamps
    m_time1_spin->setToolTip("current timestamp (ms)");
    m_time1_spin->setMinimumWidth(100);
    m_time1_spin->setSingleStep(100);
    m_time1_spin->setStyleSheet("QDoubleSpinBox { color: red; background-color: #AAAAAA;}");
    QObject::connect(m_time1_slider, &QSlider::valueChanged, this, &Base_Plot_Window::get_slider_time);
    QObject::connect(m_time1_spin, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &Base_Plot_Window::get_spinbox_time);
    QLabel *time_label = new QLabel("                Time (ms)");
    time_label->setMinimumWidth(40);
    time_layout->addWidget(time_label);
    time_layout->addWidget(m_time1_slider);
    time_layout->addSpacing(10);
    time_layout->addWidget(m_time1_spin);

    m_vertical_max_spin = new QSpinBox();
    m_vertical_max_spin->setFixedWidth(70);
    m_vertical_max_spin->setSingleStep(100);
    m_vertical_max_spin->setMinimum(INT32_MIN);
    m_vertical_max_spin->setMaximum(INT32_MAX);

    if (m_plot_type ==  Plot_Type::TIME && zoom_setting.size() > 3)
    {
        m_vertical_max_spin->setValue(zoom_setting[3]);
    }
    else
    {
        m_vertical_max_spin->setValue(5000);
    }

    QObject::connect(m_vertical_max_spin, SIGNAL(valueChanged(int)), this, SLOT(update_scale()));

    //time_layout->addSpacing(10);
    m_vert_max_label = new QLabel("Vert. Max");
    time_layout->addWidget(m_vert_max_label);
    time_layout->addWidget(m_vertical_max_spin);

    m_x_center_spin = new QSpinBox();
    m_x_center_spin->setFixedWidth(100);
    m_x_center_spin->setSingleStep(100);
    m_x_center_spin->setMinimum(INT32_MIN);
    m_x_center_spin->setMaximum(INT32_MAX);
    m_x_center_spin->setValue(0);
    m_x_center_spin->setSuffix(" mm");

    m_x_width_spin = new QSpinBox();
    m_x_width_spin->setFixedWidth(100);
    m_x_width_spin->setSingleStep(100);
    m_x_width_spin->setMinimum(100);
    m_x_width_spin->setMaximum(INT32_MAX);
    m_x_width_spin->setValue(0);
    m_x_width_spin->setSuffix(" mm");

    time_layout->addSpacing(10);
    time_layout->addWidget(m_x_center_spin);
    time_layout->addWidget(m_x_width_spin);
    //time_layout->addSpacing(100);

    QPushButton* copytimestamp_btn = new QPushButton("Copy timestamp");
    copytimestamp_btn->setToolTip("copy timestamp (P)");
    copytimestamp_btn->setFixedWidth(120);
    copytimestamp_btn->setShortcut(QKeySequence("P"));
    QObject::connect(copytimestamp_btn, &QPushButton::clicked, [this]()
    {
        qDebug().noquote() << "timestamp" << QString::number(static_cast<uint64_t>(m_time1_spin->value())) << "ms is copied to clipboard";

        QGuiApplication::clipboard()->setText(QString::number(static_cast<uint64_t>(m_time1_spin->value())));
    });

    time_layout->addWidget(copytimestamp_btn);

    result->addLayout(time_layout);

    QHBoxLayout* delta_time_layout = new QHBoxLayout();
    m_delta_time_slider = new QSlider();
    m_delta_time_slider->setOrientation(Qt::Horizontal);
    m_delta_time_slider->setMaximum(10000);
    m_delta_time_slider->setMinimum(100);
    m_delta_time_slider->setValue(Time_Mediator::get_instance().get_plot_delta_time());

    m_delta_time_spin = new QSpinBox();
    m_delta_time_spin->setToolTip("time window (ms)");
    m_delta_time_spin->setMinimumWidth(100);
    m_delta_time_spin->setMaximum(10000);
    m_delta_time_spin->setMinimum(100);
    m_delta_time_spin->setValue(Time_Mediator::get_instance().get_plot_delta_time());
    m_delta_time_spin->setSingleStep(500);
    QObject::connect(m_delta_time_slider, &QSlider::valueChanged, this, &Base_Plot_Window::get_slider_deltaT);
    QObject::connect(m_delta_time_spin, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &Base_Plot_Window::get_spinbox_deltaT);
    QLabel *delta_time_label = new QLabel("Time window (ms)");
    delta_time_label->setMinimumWidth(40);
    delta_time_layout->addWidget(delta_time_label);
    delta_time_layout->addWidget(m_delta_time_slider);
    delta_time_layout->addSpacing(10);
    delta_time_layout->addWidget(m_delta_time_spin);


    m_vertical_min_spin = new QSpinBox();
    m_vertical_min_spin->setFixedWidth(70);
    m_vertical_min_spin->setSingleStep(100);
    m_vertical_min_spin->setMinimum(INT32_MIN);
    m_vertical_min_spin->setMaximum(INT32_MAX);

    if (m_plot_type ==  Plot_Type::TIME && zoom_setting.size() > 3)
    {
        m_vertical_min_spin->setValue(zoom_setting[1]);
    }
    else
    {
        m_vertical_min_spin->setValue(0);
    }


    QObject::connect(m_vertical_min_spin, SIGNAL(valueChanged(int)), this, SLOT(update_scale()));

    //delta_time_layout->addSpacing(10);
    m_vert_min_label = new QLabel("Vert. Min");
    delta_time_layout->addWidget(m_vert_min_label);
    delta_time_layout->addWidget(m_vertical_min_spin);

    m_y_center_spin = new QSpinBox();
    m_y_center_spin->setFixedWidth(100);
    m_y_center_spin->setSingleStep(100);
    m_y_center_spin->setMinimum(INT32_MIN);
    m_y_center_spin->setMaximum(INT32_MAX);
    m_y_center_spin->setValue(0);
    m_y_center_spin->setSuffix(" mm");

    m_y_height_spin = new QSpinBox();
    m_y_height_spin->setFixedWidth(100);
    m_y_height_spin->setSingleStep(100);
    m_y_height_spin->setMinimum(100);
    m_y_height_spin->setMaximum(INT32_MAX);
    m_y_height_spin->setValue(0);
    m_y_height_spin->setSuffix(" mm");


    if (zoom_setting.size() < 4)
    {
        switch (m_plot_type)
        {
        case Plot_Type::APL_XY:
            m_x_width_spin->setValue(34000);
            m_y_height_spin->setValue(16000);
            break;

        case Plot_Type::APL_XZ:
            m_x_width_spin->setValue(16000);
            m_y_height_spin->setValue(6000);
            m_y_center_spin->setValue(3000);
            break;

        case Plot_Type::APL_YZ:
            m_x_width_spin->setValue(26469);
            m_y_height_spin->setValue(13300);
            m_y_center_spin->setValue(4000);
            break;
        }
    }
    else
    {
        m_x_width_spin->setValue(zoom_setting[0]);
        m_x_center_spin->setValue(zoom_setting[1]);

        m_y_height_spin->setValue(zoom_setting[2]);
        m_y_center_spin->setValue(zoom_setting[3]);
    }


    delta_time_layout->addSpacing(10);
    delta_time_layout->addWidget(m_y_center_spin);
    delta_time_layout->addWidget(m_y_height_spin);

    QPushButton* default_zoom_btn = new QPushButton("Default scale");
    default_zoom_btn->setFixedWidth(120);
    default_zoom_btn->setToolTip("set plot to default scale");
    default_zoom_btn->setShortcut(QKeySequence("D"));
    QObject::connect(default_zoom_btn, &QPushButton::clicked, [this]()
    {
        set_default_zoom();
    });

    delta_time_layout->addWidget(default_zoom_btn);

    result->addLayout(delta_time_layout);

    if (m_plot_type != Plot_Type::LITE_TIME)
    {
        QLabel* label_filter_1 = new QLabel("object");
        label_filter_1->setFixedWidth(152);
        QLabel* label_filter_2 = new QLabel("event_type");
        label_filter_2->setFixedWidth(152);
        QLabel* label_filter_3 = new QLabel("event_subtype");

        QHBoxLayout* label_filter_layout = new QHBoxLayout();
        label_filter_layout->addWidget(label_filter_1);
        label_filter_layout->addWidget(label_filter_2);
        label_filter_layout->addWidget(label_filter_3);
        label_filter_layout->addStretch();

        result->addWidget(new QLabel("Menu of label filter"));
        result->addLayout(label_filter_layout);

        result->addLayout(create_label_controls());

        //result->addWidget(new QLabel("Errors:"));
        result->addLayout(create_errors_controls());
    }

    return result;
}

QVBoxLayout* Base_Plot_Window::create_label_controls() noexcept
{
    QVBoxLayout *root_labels_layout = new QVBoxLayout;
    QHBoxLayout *labels_layout = new QHBoxLayout;
    m_objects_box = new QComboBox();
    m_objects_box->setToolTip("select object");
    m_objects_box->setFixedWidth(150);
    QObject::connect(m_objects_box, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=] (int value)
    {
        emit change_index_objects_list_signal(value);
    });
    m_labels_box = new QComboBox();
    m_labels_box->setToolTip("select event type");
    m_labels_box->setFixedWidth(150);
    QObject::connect(m_labels_box, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &Base_Plot_Window::get_label_combobox_value);

    QObject::connect(m_labels_box, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &Base_Plot_Window::update_subtype_combobox);


    m_subtype_box = new QComboBox();
    m_subtype_box->setToolTip("select event subtype");
    m_subtype_box->setMinimumWidth(150);

    //connect(m_subtype_box, &QComboBox::currentTextChanged, &(Plots_Mediator::get_instance()), &Plots_Mediator::change_label_subtype_combobox_slot);


    m_label_info_edit = new QLineEdit();
    m_label_info_edit->setToolTip("labeling details");
    m_label_info_edit->setReadOnly(true);

    labels_layout->addWidget(m_objects_box);
    labels_layout->addWidget(m_labels_box);
    labels_layout->addWidget(m_subtype_box);

    QPushButton *first_label_button = new QPushButton("<<");
    first_label_button->setToolTip("go-to the first label");
    first_label_button->setMaximumWidth(30);
    QObject::connect(first_label_button, &QPushButton::clicked, this, &Base_Plot_Window::open_first_label);
    labels_layout->addWidget(first_label_button);
    QPushButton *prev_label_button = new QPushButton("<");
    prev_label_button->setToolTip("go-to the previous label");
    QObject::connect(prev_label_button, &QPushButton::clicked, this, &Base_Plot_Window::open_prev_label);
    prev_label_button->setMaximumWidth(30);
    labels_layout->addWidget(prev_label_button);
    QPushButton *next_label_button = new QPushButton(">");
    next_label_button->setToolTip("go-to the next label");
    QObject::connect(next_label_button, &QPushButton::clicked, this, &Base_Plot_Window::open_next_label);
    next_label_button->setMaximumWidth(30);
    labels_layout->addWidget(next_label_button);
    QPushButton *last_label_button = new QPushButton(">>");
    last_label_button->setToolTip("go-to the last label");
    QObject::connect(last_label_button, &QPushButton::clicked, this, &Base_Plot_Window::open_last_label);
    last_label_button->setMaximumWidth(30);
    labels_layout->addWidget(last_label_button);

    //QPushButton *delete_label_button = new QPushButton("Delete");
    //delete_label_button->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    //labels_layout->addWidget(delete_label_button);
    //QObject::connect(delete_label_button, &QPushButton::clicked, this, &Base_Plot_Window::delete_selected_label);

    labels_layout->addSpacing(30);
    labels_layout->addWidget(m_label_info_edit);

    root_labels_layout->addLayout(labels_layout);

    QHBoxLayout *custom_labels_layout = new QHBoxLayout;
    m_objects_box2 = new QComboBox();
    m_objects_box2->setMinimumWidth(200);
    QObject::connect(m_objects_box2, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=] (int value)
    {
        emit change_index_objects2_list_signal(value);
    });
    m_comment_edit = new QLineEdit();
    m_comment_edit->setPlaceholderText("Comment...");
    m_labels_box2 = new QComboBox();
    m_labels_box2->setMinimumWidth(200);
    m_labels_box2->addItems(QStringList() << "RJ" << "right" << "left" << "Other");
    QObject::connect(m_labels_box2, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=] (int value)
    {
        emit change_index_labels2_list_signal(value);
        if (m_labels_box2->itemText(value).contains("left") || m_labels_box2->itemText(value).contains("right"))
        {
            m_comment_edit->setPlaceholderText("1t,good");
        }
        else
        {
            m_comment_edit->setPlaceholderText("Comment...");
        }
    });

    QPushButton *add_label_btn = new QPushButton("Add");
    QObject::connect(add_label_btn, &QPushButton::clicked, this, &Base_Plot_Window::add_new_label);

    custom_labels_layout->addWidget(m_objects_box2);
    custom_labels_layout->addWidget(m_labels_box2);
    custom_labels_layout->addWidget(m_comment_edit);
    custom_labels_layout->addWidget(add_label_btn);

    //root_labels_layout->addLayout(custom_labels_layout);

    m_selected_object = 0;
    m_selected_object2 = 0;
    m_selected_label = "";

    return root_labels_layout;
}

QVBoxLayout* Base_Plot_Window::create_errors_controls() noexcept
{
    QVBoxLayout *root_labels_layout = new QVBoxLayout;
    QHBoxLayout *labels_layout = new QHBoxLayout;
    m_objects_error_box = new QComboBox();
    m_objects_error_box->setMinimumWidth(200);
    //    QObject::connect(m_objects_error_box, &QComboBox::currentTextChanged, this, &Base_Plot_Window::update_error_item_filter);
    QObject::connect(m_objects_error_box, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=] (int value)
    {
        emit change_indext_objects_error_list_signal(value);
    });

    m_errors_type_box = new QComboBox();
    m_errors_type_box->setMinimumWidth(200);
    //    QObject::connect(m_errors_type_box, &QComboBox::currentTextChanged, this, &Base_Plot_Window::update_error_item_filter);
    QObject::connect(m_errors_type_box, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=] (int value)
    {
        emit change_index_errors_type_list_signal(value);
    });

    labels_layout->addWidget(m_objects_error_box);
    labels_layout->addWidget(m_errors_type_box);


    QPushButton *first_label_button = new QPushButton("<<");
    first_label_button->setMaximumWidth(50);
    QObject::connect(first_label_button, &QPushButton::clicked, this, &Base_Plot_Window::open_first_error);
    labels_layout->addWidget(first_label_button);
    QPushButton *prev_label_button = new QPushButton("<");
    QObject::connect(prev_label_button, &QPushButton::clicked, this, &Base_Plot_Window::open_prev_error);
    prev_label_button->setMaximumWidth(50);
    labels_layout->addWidget(prev_label_button);
    QPushButton *next_label_button = new QPushButton(">");
    QObject::connect(next_label_button, &QPushButton::clicked, this, &Base_Plot_Window::open_next_error);
    next_label_button->setMaximumWidth(50);
    labels_layout->addWidget(next_label_button);
    QPushButton *last_label_button = new QPushButton(">>");
    QObject::connect(last_label_button, &QPushButton::clicked, this, &Base_Plot_Window::open_last_error);
    last_label_button->setMaximumWidth(50);
    labels_layout->addWidget(last_label_button);

    labels_layout->addStretch();

    //root_labels_layout->addLayout(labels_layout);

    m_selected_error_object = 0;
    m_selected_error_type = 0;

    return root_labels_layout;
}

void Base_Plot_Window::get_label_combobox_value(int value) noexcept
{
    emit change_index_labels_list_signal(value);
}

void Base_Plot_Window::get_slider_time(int value) noexcept
{
    QObject::disconnect(m_time1_spin, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &Base_Plot_Window::get_spinbox_time);
    emit set_position_signal(value + m_time1_slider->property("min_time").toULongLong());
    QObject::connect(m_time1_spin, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &Base_Plot_Window::get_spinbox_time);
}

void Base_Plot_Window::get_spinbox_time(uint64_t value) noexcept
{
    QObject::disconnect(m_time1_slider, &QSlider::valueChanged, this, &Base_Plot_Window::get_slider_time);
    emit set_position_signal(value);
    QObject::connect(m_time1_slider, &QSlider::valueChanged, this, &Base_Plot_Window::get_slider_time);
}

void Base_Plot_Window::get_slider_deltaT(int value) noexcept
{
    QObject::disconnect(m_delta_time_spin, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &Base_Plot_Window::get_spinbox_deltaT);
    emit set_deltaT_signal(value);
    QObject::connect(m_delta_time_spin, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &Base_Plot_Window::get_spinbox_deltaT);
}

void Base_Plot_Window::get_spinbox_deltaT(int value) noexcept
{
    QObject::disconnect(m_delta_time_slider, &QSlider::valueChanged, this, &Base_Plot_Window::get_slider_deltaT);
    emit set_deltaT_signal(value);
    QObject::connect(m_delta_time_slider, &QSlider::valueChanged, this, &Base_Plot_Window::get_slider_deltaT);
}

void Base_Plot_Window::update_data(W_Object_Data *object)
{
    QString log_name = Operation_Mode::get_instance().get_log_name().split("/").back();

    switch (m_plot_type)
    {
    case Plot_Type::TIME:
        setWindowTitle(QString("DSP Tool v.") + APP_VERSION + " Time plot. " + log_name);
        break;
    case Plot_Type::APL_XY:
        setWindowTitle(QString("DSP Tool v.") + APP_VERSION + " Top view  plot. " + log_name);
        break;
    case Plot_Type::APL_XZ:
        setWindowTitle(QString("DSP Tool v.") + APP_VERSION + " Front view  plot. " + log_name);
        break;
    case Plot_Type::APL_YZ:
        setWindowTitle(QString("DSP Tool v.") + APP_VERSION + " Side view  plot. " + log_name);
        break;

    }

    if (object->is_show())
    {
        if (m_objects_to_show->find(object->get_name()) == m_objects_to_show->end())
        {
            m_objects_to_show->insert({object->get_name(), object});
        }
    }
    else
    {
        if (m_objects_to_show->find(object->get_name()) != m_objects_to_show->end())
        {
            m_objects_to_show->erase(object->get_name());
        }
    }

    if (!m_objects_to_label_combobox.contains(object->get_name()))
    {
        m_objects_to_label_combobox.append(object->get_name());
    }

    m_objects_box->clear();
    m_objects_box->addItem("none");
    m_objects_box->addItem("all");

    QSet<QString> tmp_set;

    for (const auto &object_item : m_objects_to_label_combobox)
    {
        QString new_item;

        QVariant team_name = Episode_Meta_Data::get_instance().get_value(object_item.toULong(), Meta_Parameter_Type::PLAYER_TEAM_NAME);
        if (team_name.isValid())
        {
            new_item = team_name.toString() + "; ";
        }

        QVariant object_name = Episode_Meta_Data::get_instance().get_value(object_item.toULong(), Meta_Parameter_Type::OBJECT_VIEW_ID);

        if (object_name.isValid())
        {
            new_item += object_name.toString() + "; ";
        }

        new_item += object_item;

        if (tmp_set.find(new_item) == tmp_set.end())
        {
            m_objects_box->addItem(new_item);
        }
    }

    //    m_objects_box2->clear();
    //    m_objects_box2->addItem("none");
    //    for (const auto &object_item : *m_objects_to_show)
    //    {
    //        m_objects_box2->addItem(object_item.first);
    //    }

    //    m_objects_error_box->clear();
    //    for (const auto &object_item : *m_objects_to_show)
    //    {
    //        m_objects_error_box->addItem(object_item.first);
    //    }
}

void Base_Plot_Window::update_scale_controlls(double min_x, double max_x, double min_y, double max_y)
{    
    //qDebug() << min_x << max_x << min_y << max_y;

    min_x = std::max(0., min_x);
    max_x = std::max(0., max_x);

    uint64_t time1 = static_cast<uint64_t>(min_x);
    uint64_t time2 = static_cast<uint64_t>(max_x);

    uint64_t center_time = (time1 + time2) / 2;

    //m_time1_slider->setProperty("min_time", qulonglong(time1));
    m_time1_slider->setValue(static_cast<int>(static_cast<int64_t>(center_time) - m_time1_slider->property("min_time").toULongLong()));
    //qDebug() << m_time1_slider->value() << "VVVVV";
    m_time1_slider->setPageStep(static_cast<int>(time2 - time1) / 3);
    m_time1_slider->setSingleStep(static_cast<int>(time2 - time1) / 100);

    m_delta_time_slider->setValue(static_cast<int>(center_time - time1));
    m_delta_time_slider->setPageStep(static_cast<int>(time2 - time1) / 3);
    m_delta_time_slider->setSingleStep(static_cast<int>(time2 - time1) / 100);

    m_vertical_min_spin->setValue(static_cast<int>(min_y));
    m_vertical_max_spin->setValue(static_cast<int>(max_y));
}

void Base_Plot_Window::update_time1_value(uint64_t value)
{
    if (!isHidden())
    {
        QObject::disconnect(m_time1_slider, &QSlider::valueChanged, this, &Base_Plot_Window::get_slider_time);
        QObject::disconnect(m_time1_spin, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &Base_Plot_Window::get_spinbox_time);
        uint64_t tmp = value - m_time1_slider->property("min_time").toULongLong();
        m_time1_slider->setValue(tmp);

        m_time1_spin->setValue(value);

        m_current_time_label->setText(UTILS::time_to_string(value) + "     " + QString::number(value) + " ms");
        QObject::connect(m_time1_slider, &QSlider::valueChanged, this, &Base_Plot_Window::get_slider_time);
        QObject::connect(m_time1_spin, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &Base_Plot_Window::get_spinbox_time);
        update_scale();
    }
}

void Base_Plot_Window::update_deltaT_value(unsigned int value)
{
    //    if (!isHidden())
    {
        QObject::disconnect(m_delta_time_slider, &QSlider::valueChanged, this, &Base_Plot_Window::get_slider_deltaT);
        QObject::disconnect(m_delta_time_spin, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &Base_Plot_Window::get_spinbox_deltaT);
        m_delta_time_slider->setValue(value);
        m_delta_time_spin->setValue(value);
        QObject::connect(m_delta_time_slider, &QSlider::valueChanged, this, &Base_Plot_Window::get_slider_deltaT);
        QObject::connect(m_delta_time_spin, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &Base_Plot_Window::get_spinbox_deltaT);
        update_scale();
    }
}

void Base_Plot_Window::set_speed(int speed) noexcept
{
    m_speed_label->setText(SPEED_LABELS[speed]);
}

void Base_Plot_Window::set_labels(Log_Labels *labels)
{
    m_labels = labels;
    update_labels_list();
}

void Base_Plot_Window::update_labels_list()
{
    QObject::disconnect(m_labels_box, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &Base_Plot_Window::get_label_combobox_value);
    m_labels_box->clear();
    m_labels_box->addItem("none");

    const QStringList *name_list = m_labels->get_labels_name_list();
    if (name_list->size() > 0)
    {
        m_labels_box->addItem("all");
        m_labels_box->addItems(*name_list);
    }
    QObject::connect(m_labels_box, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &Base_Plot_Window::get_label_combobox_value);

    m_errors_type_box->clear();
    const QStringList *name_errors_list = m_labels->get_errors_name_list();
    if (name_errors_list->size() > 0)
    {
        m_errors_type_box->addItem("none");
        m_errors_type_box->addItem("all");
        m_errors_type_box->addItems(*name_errors_list);
    }

    if (m_selected_object < m_objects_box->count())
    {
        m_objects_box->setCurrentIndex(m_selected_object);
    }

    if (m_selected_object2 < m_objects_box2->count())
    {
        m_objects_box2->setCurrentIndex(m_selected_object2);
    }


    if (m_selected_error_object < m_objects_error_box->count())
    {
        m_objects_error_box->setCurrentIndex(m_selected_error_object);
    }

    if (m_labels_box->findText(m_selected_label) > -1)
    {
        //        qDebug() << "m_selected_label" << m_selected_label;
        //        m_labels_box->setCurrentIndex(m_selected_label);
        m_labels_box->setCurrentText(m_selected_label);
    }

    if (m_selected_error_type < m_errors_type_box->count())
    {
        m_errors_type_box->setCurrentIndex(m_selected_error_type);
    }
}

void Base_Plot_Window::update_subtype_combobox()
{
    disconnect(m_subtype_box, &QComboBox::currentTextChanged, &(Plots_Mediator::get_instance()), &Plots_Mediator::change_label_subtype_combobox_slot);

    m_subtype_box->clear();

    std::vector<Label_Item *>* ptr_labels_by_type = m_labels->get_labels_by_type(m_labels_box->currentText());
    if (ptr_labels_by_type != nullptr
            && !ptr_labels_by_type->empty())
    {
        QSet<QString> subtypes;

        for (const Label_Item* li : *ptr_labels_by_type)
        {
            const auto iter = li->m_tag_value_map.find("event_subtype");
            if (iter != li->m_tag_value_map.cend())
            {
                subtypes.insert(iter->second.toString());
            }
            else
            {
                subtypes.insert("-");
            }
        }

        m_subtype_box->addItem("all");

        if (!(subtypes.size() == 1 && subtypes.contains("-")))
        {
            auto tmp = subtypes.toList();
            tmp.sort();
            m_subtype_box->addItems(tmp);
        }
    }
    else
    {
        m_subtype_box->addItem(m_labels_box->currentText());
    }

    connect(m_subtype_box, &QComboBox::currentTextChanged, &(Plots_Mediator::get_instance()), &Plots_Mediator::change_label_subtype_combobox_slot);
}

void Base_Plot_Window::open_first_label()
{
    if (!m_selected_label_vector.empty() && m_current_label_iterator != m_selected_label_vector.begin())
    {
        m_current_label_iterator = m_selected_label_vector.begin();
        go_to_label(*m_current_label_iterator);
    }
}

void Base_Plot_Window::open_prev_label()
{
    if (!m_selected_label_vector.empty())// && m_current_label_iterator != m_selected_label_vector.begin())
    {
        //        -- m_current_label_iterator;
        //        go_to_label(*m_current_label_iterator);

        uint64_t current_time = m_time1_spin->value();

        if (current_time > (*m_current_label_iterator)->m_timestamp)
        {
            while (m_current_label_iterator != m_selected_label_vector.end())
            {
                m_current_label_iterator ++;

                if (m_current_label_iterator == m_selected_label_vector.end())
                {
                    m_current_label_iterator --;
                    break;
                }

                if ((*m_current_label_iterator)->m_timestamp > current_time)
                {
                    m_current_label_iterator --;
                    break;
                }
            }
            go_to_label(*m_current_label_iterator);
        }
        else
        {
            if (m_current_label_iterator == m_selected_label_vector.begin())
            {
                go_to_label(*m_current_label_iterator);
            }
            else
            {
                while (m_current_label_iterator != m_selected_label_vector.begin())
                {
                    m_current_label_iterator --;
                    if ((*m_current_label_iterator)->m_timestamp < current_time)
                    {
                        break;
                    }
                }
                go_to_label(*m_current_label_iterator);
            }
        }
    }
}

void Base_Plot_Window::open_next_label()
{
    if (!m_selected_label_vector.empty())
    {
        uint64_t current_time = m_time1_spin->value();

        if (current_time >= (*m_current_label_iterator)->m_timestamp)
        {
            while (m_current_label_iterator != m_selected_label_vector.end())
            {
                m_current_label_iterator ++;
                if (m_current_label_iterator == m_selected_label_vector.end())
                {
                    m_current_label_iterator --;
                    break;
                }
                if ((*m_current_label_iterator)->m_timestamp > current_time)
                {
                    go_to_label(*m_current_label_iterator);
                    break;
                }
            }
        }
        else
        {
            if (m_current_label_iterator == m_selected_label_vector.begin())
            {
                go_to_label(*m_current_label_iterator);
            }
            else
            {
                while (m_current_label_iterator != m_selected_label_vector.begin())
                {
                    m_current_label_iterator --;
                    if ((*m_current_label_iterator)->m_timestamp < current_time)
                    {
                        break;
                    }
                }
                m_current_label_iterator ++;
                go_to_label(*m_current_label_iterator);
            }
        }
    }
}

void Base_Plot_Window::open_last_label()
{
    if (!m_selected_label_vector.empty())
    {
        m_current_label_iterator = m_selected_label_vector.end();
        -- m_current_label_iterator;
        go_to_label(*m_current_label_iterator);
    }
}

void Base_Plot_Window::go_to_label(Label_Item *label)
{
    if (label != nullptr)
    {
        QString comment = QString::number(label->m_timestamp) + " " + label->m_type_v;
        /*auto it = label->m_tag_value_map.find("comment");
        if (it != label->m_tag_value_map.end())
        {
            comment += " " + it->second.toString();
        }*/
        for (auto e : label->m_tag_value_map)
        {
            comment += "; " + e.first + "= " + e.second.toString();
        }
        m_label_info_edit->setText(comment);
        m_time1_slider->setValue(label->m_timestamp - m_time1_slider->property("min_time").toULongLong());
    }
    else
    {
        m_label_info_edit->setText("nothing");
    }
}

void Base_Plot_Window::open_first_error()
{
    if (!m_selected_errors_vector.empty() && m_current_error_iterator != m_selected_errors_vector.begin())
    {
        m_current_error_iterator = m_selected_errors_vector.begin();
        go_to_label(*m_current_error_iterator);
    }
}

void Base_Plot_Window::open_prev_error()
{
    if (!m_selected_errors_vector.empty() && m_current_error_iterator != m_selected_errors_vector.begin())
    {
        -- m_current_error_iterator;
        go_to_label(*m_current_error_iterator);
    }
}

void Base_Plot_Window::open_next_error()
{
    if (!m_selected_errors_vector.empty())
    {
        if (m_current_error_iterator != m_selected_errors_vector.end())
        {
            go_to_label(*m_current_error_iterator);
            ++ m_current_error_iterator;
        }
        else
        {
            -- m_current_error_iterator;
        }
    }
}

void Base_Plot_Window::open_last_error()
{
    if (!m_selected_errors_vector.empty())
    {
        m_current_error_iterator = m_selected_errors_vector.end();
        -- m_current_error_iterator;
        go_to_label(*m_current_error_iterator);
    }
}

void Base_Plot_Window::add_new_label()
{
    //    if (m_objects_box->count() > 0)
    //    {
    //        m_selected_object = m_objects_box->currentIndex();
    //        m_selected_label = m_labels_box->currentText();
    //        m_selected_object2 = m_objects_box2->currentIndex();
    //        //qDebug() << "!!! m_selected_label" << m_selected_label;

    //        QString object_id = m_objects_box2->currentText();
    //        QString label_type = m_labels_box2->currentText();
    //        QString comment = m_comment_edit->text();
    //        long time = m_time1_slider->value();

    //        Label_Item *label_item = new Label_Item;
    //        label_item->m_is_error = false;
    //        label_item->m_type = label_type;
    //        label_item->m_timestamp = time;
    //        label_item->m_tag_value_map["type"] = label_type;
    //        label_item->m_tag_value_map["timestamp"] = QVariant((unsigned long long)time + m_labels->get_time_start());
    //        if (!object_id.contains("none"))
    //        {
    //            label_item->m_tag_value_map["object"] = object_id;
    //        }
    //        label_item->m_tag_value_map["comment"] = comment;

    //        m_labels->add_label_item(label_item);
    //        m_labels->add_label_by_object(object_id, label_item);


    //        m_labels->set_is_modified(true);

    //        emit update_labels_list_signal();
    //    }
}

void Base_Plot_Window::showEvent(QShowEvent* event)
{
    update_time1_value(Time_Mediator::get_instance().get_current_timestamp());
}

void Base_Plot_Window::closeEvent(QCloseEvent *event)
{
    int width = size().width();
    int height = size().height();
    if (isMaximized())
    {
        width = -1;
        height = -1;
    }

    //Operation_Mode::set_plot_zoom()

    Settings_Manager::get_instance()->write_window_parameters("Plot" + QString::number((int)m_plot_type), "width", width);
    Settings_Manager::get_instance()->write_window_parameters("Plot" + QString::number((int)m_plot_type), "height", height);

    Settings_Manager::get_instance()->write_window_parameters("Plot" + QString::number((int)m_plot_type), "position_x", geometry().x());
    Settings_Manager::get_instance()->write_window_parameters("Plot" + QString::number((int)m_plot_type), "position_y", geometry().y());

    if (m_plot_type == Plot_Type::TIME)
    {
        Settings_Manager::get_instance()->write_plot_zoom(static_cast<int>(m_plot_type), 0, m_vertical_min_spin->value()
                                                          , 0, m_vertical_max_spin->value());
    }
    else
    {
        Settings_Manager::get_instance()->write_plot_zoom(static_cast<int>(m_plot_type), m_x_width_spin->value(), m_x_center_spin->value()
                                                          , m_y_height_spin->value(), m_y_center_spin->value());
    }

    emit close_plot_signal("m_plot_type"); // m_plot_type var is not use
}

void Base_Plot_Window::clear_window()
{
    m_objects_to_show->clear();
    m_objects_to_label_combobox.clear();
    m_selected_label_vector.clear();
    m_selected_errors_vector.clear();
    m_objects_box->clear();
    m_objects_box2->clear();
    m_objects_error_box->clear();

    QObject::disconnect(m_labels_box, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &Base_Plot_Window::get_label_combobox_value);
    QObject::disconnect(m_labels_box, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &Base_Plot_Window::update_subtype_combobox);
    m_labels_box->clear();
    QObject::connect(m_labels_box, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &Base_Plot_Window::get_label_combobox_value);
    QObject::connect(m_labels_box, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &Base_Plot_Window::update_subtype_combobox);

    m_errors_type_box->clear();
    m_signal_plot->clear_all_curves();
    m_signal_plot->clear_all_markers();
    m_signal_plot->replot();
    m_signal_plot->set_interation_type(m_zoom_checkbox->isChecked());
}

void Base_Plot_Window::update_index_objects_list(int index) noexcept
{
    if (index < m_objects_box->count())
    {
        m_objects_box->setCurrentIndex(index);
        update_label_item_filter();
    }
}

void Base_Plot_Window::update_index_objects2_list(int index) noexcept
{
    if (index < m_objects_box2->count())
    {
        m_objects_box2->setCurrentIndex(index);
    }
}

void Base_Plot_Window::update_index_labels_list(int index) noexcept
{
    if (index < m_labels_box->count())
    {
        //        if (m_plot_type == Plot_Type::TIME)
        //        {
        //            qDebug() << "#########" << index << "plot";
        //        }
        //        m_labels_box->setCurrentIndex(index);
        //        QObject::disconnect(m_labels_box, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &Base_Plot_Window::get_label_combobox_value);
        m_labels_box->setCurrentText(m_labels_box->itemText(index));

        update_label_item_filter();
        //        QObject::connect(m_labels_box, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &Base_Plot_Window::get_label_combobox_value);
    }
}

void Base_Plot_Window::update_index_labels2_list(int index) noexcept
{
    if (index < m_labels_box2->count())
    {
        m_labels_box2->setCurrentIndex(index);
    }
}


void Base_Plot_Window::update_index_objects_errors_list(int index) noexcept
{
    if (index < m_objects_error_box->count())
    {
        m_objects_error_box->setCurrentIndex(index);
        update_error_item_filter();
    }
}

void Base_Plot_Window::update_index_errors_type_list(int index) noexcept
{
    if (index < m_errors_type_box->count())
    {
        m_errors_type_box->setCurrentIndex(index);
        update_error_item_filter();
    }
}

void Base_Plot_Window::delete_selected_label()
{
    if (m_label_info_edit->text().length() > 0)
    {
        if (m_current_label_iterator != m_selected_label_vector.end())
        {
            m_selected_object = m_objects_box->currentIndex();
            m_selected_label = m_labels_box->currentText();
            m_selected_object2 = m_objects_box2->currentIndex();
            m_comment_edit->setText("");

            Label_Item *label = *m_current_label_iterator;
            m_labels->delete_label(label);

            m_labels->set_is_modified(true);

            emit update_labels_list_signal();
        }
    }
}

uint64_t Base_Plot_Window::get_current_time() noexcept
{
    return static_cast<uint64_t>(m_time1_slider->value()) + m_time1_slider->property("min_time").toULongLong();
}

void Base_Plot_Window::change_label_subtype_combobox_current_text(const QString& text) noexcept
{
    disconnect(m_subtype_box, &QComboBox::currentTextChanged, &(Plots_Mediator::get_instance()), &Plots_Mediator::change_label_subtype_combobox_slot);

    m_subtype_box->setCurrentText(text);

    update_label_item_filter();

    connect(m_subtype_box, &QComboBox::currentTextChanged, &(Plots_Mediator::get_instance()), &Plots_Mediator::change_label_subtype_combobox_slot);
}

void Base_Plot_Window::update_alg_tmp_labels() noexcept
{
    m_signal_plot->clear_markers_by_type(Plot_Marker_Type::ALG);

    std::vector<Alg_Temp_Labels::Alg_Label>* labels = Alg_Temp_Labels::get_instance().get_alg_temp_labels();
    if (labels != nullptr && labels->size() > 0)
    {
        for (const Alg_Temp_Labels::Alg_Label &label_item : *labels)
        {
            m_signal_plot->add_markers(label_item.m_time, label_item.m_width, label_item.m_color, Plot_Marker_Type::ALG, label_item.m_description);
        }
    }
    else
    {
        m_signal_plot->clear_markers_by_type(Plot_Marker_Type::ALG);
    }
}

bool Base_Plot_Window::event(QEvent *event)
{
    if (event->type() == QEvent::KeyRelease)
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent* >(event);

        //        if (keyEvent->key() == Qt::Key_P || keyEvent->key() == 1047)
        //        {
        //            if (!(QGuiApplication::keyboardModifiers() & Qt::ShiftModifier))
        //            {
        //                QGuiApplication::clipboard()->setText(QString::number(m_time1_spin->value()));
        //            }
        //        }

        if (keyEvent->key() == Qt::Key_W || keyEvent->key() == 1062) // Ц
        {
            if (QGuiApplication::keyboardModifiers() & Qt::ShiftModifier)
            {
                emit set_position_signal(Time_Mediator::get_instance().get_current_timestamp() + 10
                                         * (Operation_Mode::get_instance().is_only_low_freq_opening() ? 40 : 8)); // 120 Hz - 8 ms; 25 Hz - 40 ms

                //m_time1_slider->setValue(m_time1_slider->value() + 10
                //                         * (Operation_Mode::get_instance().is_only_low_freq_opening() ? 40 : 8)); // 120 Hz - 8 ms; 25 Hz - 40 ms

            }
            else
            {
                emit set_position_signal(Time_Mediator::get_instance().get_current_timestamp() + (Operation_Mode::get_instance().is_only_low_freq_opening() ? 40 : 8)); // 120 Hz - 8 ms; 25 Hz - 40 ms

                //m_time1_slider->setValue(m_time1_slider->value() + 1 * (Operation_Mode::get_instance().is_only_low_freq_opening() ? 40 : 8)); // 120 Hz - 8 ms; 25 Hz - 40 ms
            }

            return true;
        }

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
                    emit set_position_signal(Time_Mediator::get_instance().get_current_timestamp() - 10
                                             * (Operation_Mode::get_instance().is_only_low_freq_opening() ? 40 :  8)); // 120 Hz - 8 ms; 25 Hz - 40 ms

                    //                    m_time1_slider->setValue(m_time1_slider->value() - 10
                    //                                             * (Operation_Mode::get_instance().is_only_low_freq_opening() ? 40 :  8)); // 120 Hz - 8 ms; 25 Hz - 40 ms
                }
                else
                {
                    emit set_position_signal(Time_Mediator::get_instance().get_current_timestamp() - 1
                                             * (Operation_Mode::get_instance().is_only_low_freq_opening() ? 40 :  8)); // 120 Hz - 8 ms; 25 Hz - 40 ms

                    //                    m_time1_slider->setValue(m_time1_slider->value() - 1
                    //                                             * (Operation_Mode::get_instance().is_only_low_freq_opening() ? 40 :  8)); // 120 Hz - 8 ms; 25 Hz - 40 ms
                }
            }
            return true;
        }

        if (!(QGuiApplication::keyboardModifiers() & Qt::ShiftModifier)
                && !(QGuiApplication::keyboardModifiers() & Qt::ControlModifier))
        {
            if (keyEvent->key() == Qt::Key_Z || keyEvent->key() == 1071) // Я
            {
                emit speed_down_requested();
                return true;
            }
            if (keyEvent->key() == Qt::Key_X || keyEvent->key() == 1063) // Ч
            {
                emit speed_up_requested();
                return true;
            }
        }

        if (!keyEvent->isAutoRepeat())
        {
            if (keyEvent->key() == Qt::Key_Space)
            {
                emit press_play_pause_signal();
            }
            else if (keyEvent->key() == Qt::Key_S || keyEvent->key() == 1067) // Ы
            {
                if (QGuiApplication::keyboardModifiers() & Qt::ShiftModifier)
                {
                    emit set_position_signal(Time_Mediator::get_instance().get_current_timestamp() + 10000); // 10 sec
                    //m_time1_slider->setValue(m_time1_slider->value() + 10000); // 10 sec
                }
                else
                {
                    emit set_position_signal(Time_Mediator::get_instance().get_current_timestamp() + 1000); // 1 sec
                    //m_time1_slider->setValue(m_time1_slider->value() + 1000); // 1 sec
                }
            }
            else if (keyEvent->key() == Qt::Key_A || keyEvent->key() == 1060) // Ф
            {
                if (QGuiApplication::keyboardModifiers() & Qt::ShiftModifier)
                {
                    emit set_position_signal(Time_Mediator::get_instance().get_current_timestamp() - 10000); // 10 sec
                    //m_time1_slider->setValue(m_time1_slider->value() - 10000); // 10 sec
                }
                else
                {
                    emit set_position_signal(Time_Mediator::get_instance().get_current_timestamp() - 1000); // 1 sec
                    //m_time1_slider->setValue(m_time1_slider->value() - 1000); // 1 sec
                }
            }
        }
    }
    return QWidget::event(event);
}

Base_Plot_Window::~Base_Plot_Window()
{
    m_objects_to_show->clear();
    m_selected_label_vector.clear();
    m_selected_errors_vector.clear();
    delete m_objects_to_show;
}
