#include "windows_controller.h"

#include <QDebug>
#include "data/wlogdata.h"
#include "transform_hub.h"
#include "app_controller.h"
#include "settings_manager.h"
#include "operation_mode.h"
#include <QMessageBox>
#include "plot/time_plot_window.h"
#include <QCloseEvent>
#include <QAction>
#include <QMenu>
#include <QMenuBar>
#include <QFileDialog>
#include "video/video_window.h"
#include "data/label/alg_temp_labels.h"
#include "data/episode_meta_data.h"
#include "time_mediator.h"
#include "main_menu_mediator.h"
#include "video_windows_mediator.h"
#include "label_mediator.h"

#include <thread>
#include <chrono>

Windows_Controller::Windows_Controller(App_Controller *app_controller, W_Log_Data *log_data)
{
    m_app_controller = app_controller;

    m_log_data = log_data;
    m_main_window = new Main_Window();
    m_main_window->show();

    m_plots_controller = new Plots_Controller();

    Label_Mediator::get_instance().set_plots_controller_ptr(m_plots_controller);

    QObject::connect(this, &Windows_Controller::open_file_signal, m_app_controller, &App_Controller::open_file);
    QObject::connect(this, &Windows_Controller::open_dir_signal, m_app_controller, &App_Controller::open_dir);
    QObject::connect(this, &Windows_Controller::save_file_signal, m_app_controller, &App_Controller::save_file);
    QObject::connect(this, &Windows_Controller::open_labels_file_signal, m_app_controller, &App_Controller::open_labels_file);
    QObject::connect(this, &Windows_Controller::open_errors_file_signal, m_app_controller, &App_Controller::open_errors_file);

    QObject::connect(m_main_window, &Main_Window::open_log_file_signal, this, &Windows_Controller::open_file);
    QObject::connect(m_main_window, &Main_Window::open_label_file_signal, this, &Windows_Controller::open_labels_file);
    QObject::connect(m_main_window, &Main_Window::open_meta_file_signal, this, &Windows_Controller::open_meta_file);
    QObject::connect(m_main_window, &Main_Window::open_dir_signal, this, &Windows_Controller::open_dir);
    QObject::connect(m_main_window, &Main_Window::save_label_file_signal, this, &Windows_Controller::save_labels);
    QObject::connect(m_main_window, &Main_Window::open_video_signal, this, &Windows_Controller::open_video);
    QObject::connect(m_main_window, &Main_Window::open_error_file_signal, this, &Windows_Controller::open_errors_file);

    QObject::connect(m_main_window, &Main_Window::process_filter_signal, m_app_controller, &App_Controller::process_filter);
    QObject::connect(m_main_window, &Main_Window::delete_filter_signal, m_app_controller, &App_Controller::delete_filter);
    QObject::connect(m_main_window, &Main_Window::update_filter_signal, m_app_controller, &App_Controller::update_filter);
    QObject::connect(m_main_window, &Main_Window::process_filter_all_data, m_app_controller, &App_Controller::process_all_data);

    QObject::connect(m_main_window, &Main_Window::update_signal_settings_signal, this, &Windows_Controller::update_signal_settings);
    QObject::connect(m_main_window, &Main_Window::update_object_settings_signal, this, &Windows_Controller::update_object_settings);
    QObject::connect(m_main_window, &Main_Window::update_view_settings_signal, this, &Windows_Controller::update_view_settings);
    QObject::connect(m_main_window, &Main_Window::update_plots_signal, this, &Windows_Controller::update_plots);
    QObject::connect(m_main_window, &Main_Window::close_window_signal, this, &Windows_Controller::close_application);
    QObject::connect(m_main_window, &Main_Window::fill_objects_list_signal, this, &Windows_Controller::fill_objects_list);
    QObject::connect(m_main_window, &Main_Window::fill_sensors_list_signal, this, &Windows_Controller::fill_sensors_list);
    QObject::connect(m_main_window, &Main_Window::fill_filter_parameters_signal, this, &Windows_Controller::fill_filter_parameters);

    QObject::connect(m_main_window, &Main_Window::show_plot_signal, m_plots_controller, &Plots_Controller::show_plot);

    QObject::connect(m_plots_controller, &Plots_Controller::close_plot_by_id_signal, m_main_window, &Main_Window::close_plot_by_id);

    QObject::connect(m_app_controller, &App_Controller::update_alg_tmp_labels, m_plots_controller, &Plots_Controller::update_alg_tmp_labels);
    QObject::connect(m_main_window, &Main_Window::update_alg_tmp_labels, m_plots_controller, &Plots_Controller::update_alg_tmp_labels);

    m_transforms_info_map = nullptr;

    //    m_plots_controller->show_plot(0, 0, false);
    //    m_plots_controller->show_plot(1, 0, false);
    //    m_plots_controller->show_plot(2, 0, false);
    //    m_plots_controller->show_plot(3, 0, false);

    auto ids = Settings_Manager::get_instance()->get_ids_of_opened_plots();
    for (int i : ids)
    {
        m_main_window->open_plot_by_id(i);
    }
    m_main_window->activateWindow();
}

// Обновляем настройки отображения сигналов
void Windows_Controller::update_signal_settings(const QString &object_id, const QString &sensor_id, const QString &signal_id, int is_show, const QString &color)
{
    if (m_log_data != nullptr)
    {
        W_Object_Data *object = m_log_data->get_object(object_id);
        if (object != nullptr)
        {
            W_Sensor_Data *sensor = object->get_sensor(sensor_id);
            if (sensor != nullptr)
            {
                W_Signal_Data *signal = sensor->get_signal(signal_id);
                if (signal != nullptr)
                {
                    signal->set_show_state(is_show);
                    signal->set_color(color);
                }
            }
        }
    }
}

// Обновляем настройки отображения объектов и добавляем на плоты те объекты, которые выбраны в дереве
void Windows_Controller::update_object_settings(const QString &object_id, int is_show, const QString &plot_id)
{
    if (m_log_data != nullptr)
    {
        W_Object_Data *object = m_log_data->get_object(object_id);
        if (object != nullptr)
        {
            if (plot_id.contains("Time"))
            {
                object->set_show_state_time_plot(is_show);
            }
            else
            {
                object->set_show_state_court_plots(is_show);
            }

            m_plots_controller->update_data(object);
        }
    }
}

// Обновляем настройки отображения сенсоров
void Windows_Controller::update_view_settings(const QString &object_id, const QString &sensor_id, int is_show, const QString &color, int weight, const QString &plot_id)
{
    if (m_log_data != nullptr)
    {
        W_Object_Data *object = m_log_data->get_object(object_id);
        if (object != nullptr)
        {
            W_Sensor_Data *sensor = object->get_sensor(sensor_id);
            if (sensor != nullptr)
            {
                if (plot_id.contains("Time"))
                {
                    sensor->set_signals_weight(weight);
                }
                else
                {
                    sensor->set_color(color);
                    sensor->set_weight(weight);
                    sensor->set_show_state(is_show);
                }
            }
        }
    }
}

// После того как все настройки для сигналов и сенсоров применены и указано какие объекты показывать -
// вызывается эта функция, которая говрит плотам обновить отображение
void Windows_Controller::update_plots()
{
    m_plots_controller->update_plots();
}

Main_Window* Windows_Controller::get_main_window() const noexcept
{
    return m_main_window;
}

Plots_Controller* Windows_Controller::get_plot2_controller() const noexcept
{
    return m_plots_controller;
}

// вызывается когда данные считаны из файла и надо обновить информацию во всех окнах
void Windows_Controller::set_log_data(W_Log_Data *log_data)
{
    Alg_Temp_Labels::get_instance().clear_all_labels();
    m_plots_controller->update_alg_tmp_labels();
    m_log_data = log_data;
    //m_main_window->open_plot_by_id(1);
    //m_plots_controller->show_plot(1, 0, true);

    uint64_t episode_start_time = Episode_Meta_Data::get_instance().get_start_timestamp();

    if (episode_start_time == 0)
    {
        episode_start_time = log_data->get_log_min_time();
        Episode_Meta_Data::get_instance().set_start_timestamp(episode_start_time);
    }

    m_plots_controller->set_log_time(episode_start_time, log_data->get_log_max_time());

    m_plots_controller->set_labels(log_data->get_log_labels());

    m_main_window->fill_mainwindow_settings(*log_data);
    //m_main_window->update_view_settings();
    Video_Windows_Mediator::get_instance().preopen_episode_videos(log_data->get_name());
}

void Windows_Controller::update_log_data(W_Log_Data *log_data)
{
    m_log_data = log_data;
    m_plots_controller->set_labels(log_data->get_log_labels());
}

void Windows_Controller::update_hoops_setup(std::map<int, Hoop>* hoops) noexcept
{
    m_plots_controller->set_hoops(hoops);
}

void Windows_Controller::fill_objects_list(const QString &log_name) noexcept
{
    m_main_window->fill_objects_list(m_log_data); // know we have only one log file
}

void Windows_Controller::fill_sensors_list(const QString &object_id) noexcept
{
    if (!object_id.contains("All"))
    {
        m_main_window->fill_sensors_list(m_log_data->get_object(object_id));
    }
    else
    {
        m_main_window->fill_sensors_list(QStringList() << "All");
    }

}

void Windows_Controller::fill_filter_parameters(const QString &filter_id) noexcept
{
    if (m_transforms_info_map != nullptr)
    {
        m_main_window->fill_filter_parameters(m_transforms_info_map->at(filter_id));
    }
}

void Windows_Controller::fill_filters_list(const std::map<QString, QString>* transforms_info_map) noexcept
{
    m_transforms_info_map = transforms_info_map;
    m_main_window->fill_filters_list(transforms_info_map);
}

void Windows_Controller::close_application(QCloseEvent *event)
{
    if (show_save_dialog() && (m_log_data == nullptr || m_log_data->get_log_labels() == nullptr
            || !m_log_data->get_log_labels()->is_modified())
            && !Operation_Mode::get_instance().is_reading_data())
    {
        m_plots_controller->close_all();
        Video_Windows_Mediator::get_instance().delete_all_video_windows();
        event->accept();
    }
    else
    {
        show_dialog("The episode is loading. Please wait.");
        event->ignore();
    }
}

void Windows_Controller::update_filtes_table(const W_Log_Data &log_data, const std::vector<WKTR::Transform_Spec*>& transf_spec_vector)
{
    m_main_window->fill_filters_table(log_data, transf_spec_vector);
}

void Windows_Controller::open_file(const QString& file_name, bool is_with_labels, bool is_with_meta_file) noexcept
{
    QString file_name_tmp = file_name;
    if (file_name_tmp.isEmpty())
    {
        file_name_tmp = QFileDialog::getOpenFileName(nullptr, tr("Open File"), Settings_Manager::get_instance()->get_last_log_file_path(),
                                                     tr("Log Files (*.csv)"), nullptr, QFileDialog::DontUseNativeDialog);
    }

    if (!file_name_tmp.isEmpty())
    {
        Settings_Manager::get_instance()->write_last_log_file_path(file_name_tmp);

        Label_Mediator::get_instance().clear();

        emit open_file_signal(file_name_tmp, is_with_labels, is_with_meta_file);
    }
}

void Windows_Controller::open_dir() noexcept
{
    QString dir_name = QFileDialog::getExistingDirectory(nullptr, tr("Open Dir"), Settings_Manager::get_instance()->get_last_log_file_path(),
                                                         QFileDialog::ShowDirsOnly | QFileDialog::DontUseNativeDialog);
    if (!dir_name.isEmpty())
    {
        Settings_Manager::get_instance()->write_last_log_file_path(dir_name);
        emit open_dir_signal(dir_name);
    }
}

void Windows_Controller::open_labels_file() noexcept
{
    if (m_log_data != nullptr)
    {
        QStringList tmp = m_log_data->get_name().split("/");
        if (tmp.size() > 1)
        {
            tmp.removeLast();
            tmp[tmp.size() - 1] = "labels";

            QString file_name = QFileDialog::getOpenFileName(nullptr, tr("Open File"), tmp.join("/"),
                                                             tr("Labels Files (*.csv)"), nullptr, QFileDialog::DontUseNativeDialog);
            if (!file_name.isEmpty())
            {
                Label_Mediator::get_instance().open_label(file_name);
            }
        }
    }
    else
    {
        show_dialog("Please open the file with sensor data");
    }
}

void Windows_Controller::open_meta_file() noexcept
{
    if (m_log_data != nullptr)
    {
        QStringList tmp = m_log_data->get_name().split("/");
        if (tmp.size() > 1)
        {
            tmp.removeLast();
            tmp[tmp.size() - 1] = "labels";

            QString file_name = QFileDialog::getOpenFileName(nullptr, tr("Open File"), tmp.join("/"),
                                                             tr("Metafiles (*.csv)"), nullptr, QFileDialog::DontUseNativeDialog);
            if (!file_name.isEmpty())
            {
                Episode_Meta_Data::get_instance().read_meta_file(file_name);

                open_file(m_log_data->get_name(), true, false);

                //m_main_window->fill_mainwindow_settings(*m_log_data);
                //Label_Mediator::get_instance().open_label(file_name);
            }
        }
    }
    else
    {
        show_dialog("Please open the file with sensor data");
    }
}


void Windows_Controller::save_labels() noexcept
{
    QString file_name = QFileDialog::getSaveFileName(nullptr, tr("Save File"), Settings_Manager::get_instance()->get_last_label_file_path(),
                                                     tr("Label File (*.csv)"));

    if (!file_name.isEmpty())
    {
        emit save_file_signal(file_name);
    }
}

void Windows_Controller::show_dialog(const QString &message) noexcept
{
    QMessageBox msgBox;
    msgBox.setText(message);
    msgBox.exec();
}

bool Windows_Controller::show_save_dialog() noexcept
{
    /*if (m_log_data != nullptr && m_log_data->get_log_labels() != nullptr && m_log_data->get_log_labels()->is_modified())
    {
        QMessageBox::StandardButton ret;
        ret = QMessageBox::warning(m_main_window, tr("Hey!"),
                                   tr("The labels have been modified.\n"
                                      "Do you want to save your changes?"),
                                   QMessageBox::Save | QMessageBox::Discard
                                   | QMessageBox::Cancel);
        if (ret == QMessageBox::Save)
        {
            save_labels();
            return false;
        }
        else if (ret == QMessageBox::Cancel)
        {
            return false;
        }
    }*/
    return true;
}

void Windows_Controller::open_video() noexcept
{
    QString file_name = QFileDialog::getOpenFileName(nullptr, tr("Open Video"), Settings_Manager::get_instance()->get_last_video_file_path(),
                                                     tr("Video Files (*.mp4 *.mpg *.mov *.264)"), 0, QFileDialog::DontUseNativeDialog);
    if (!file_name.isEmpty())
    {
        Settings_Manager::get_instance()->write_last_video_file_path(file_name);
        Video_Windows_Mediator::get_instance().add_video_to_list_and_open(file_name, false);
    }
}

void Windows_Controller::open_errors_file() noexcept
{
    QString file_name = QFileDialog::getOpenFileName(nullptr, tr("Open Errors"), Settings_Manager::get_instance()->get_last_errors_file_path(),
                                                     tr("Errors Files (*errors.csv)"), nullptr, QFileDialog::DontUseNativeDialog);
    if (!file_name.isEmpty())
    {
        Settings_Manager::get_instance()->write_last_errors_file_path(file_name);
        emit open_errors_file_signal(file_name);
    }
}

void Windows_Controller::save_transformation() noexcept
{
    m_main_window->save_transformation();
}

Windows_Controller::~Windows_Controller()
{
}
