#include "app_controller.h"

#include "parser/label_reader.h"
#include <QDebug>
#include "transform_hub.h"
#include "file_reader.h"
#include "apl_helper/facilitysetup_reader.h"
#include "apl_helper/wrap_apl_banchmark.h"
#include "file_writer.h"
#include "data/data_manager.h"
#include "operation_mode.h"
#include <QTime>
#include <QProcess>
#include "parser/apl_label_parser.h"
#include "utils.h"
#include <QFile>
#include "settings_manager.h"
#include <iostream>
#include <QDateTime>
#include <QDir>

#include "data/episode_meta_data.h"
#include "label_mediator.h"

#ifdef GUI
#include "windows_controller.h"
#endif

#ifdef RQI_UWB
#include "rqi_uwb.h"
#endif

App_Controller::App_Controller(QObject *parent) : QObject(parent)
{
    m_q_application = nullptr;
    m_filter = nullptr;
    m_filter_parameters = nullptr;
    m_hoop1_config = nullptr;
    m_hoop2_config = nullptr;
    m_benchmark_id = 1;

    m_data_manager = new Data_Manager();

    Label_Mediator::get_instance().set_data_manager_ptr(m_data_manager);

    m_data_manger_test_res = nullptr;

    m_transform_manager = new WKTR::Transform_Hub(m_data_manager);

    m_wrap_apl_benchmark = new Wrap_Apl_Banchmark();


    m_state = App_State::NOT_WAIT;
    m_is_dir = false;

    m_file_reader = new File_Reader();
    m_file_reader->set_data_manager(m_data_manager);
    QObject::connect(m_file_reader, static_cast<void (File_Reader::*)(bool, const QString &)>(&File_Reader::file_reading_done),
                     this, &App_Controller::update_data_from_log);


    m_file_writer = new File_Writer();
    m_file_writer->set_data_manager(m_data_manager);
    QObject::connect(m_file_writer, static_cast<void (File_Writer::*)(bool, const QString &)>(&File_Writer::file_writing_done),
                     this, &App_Controller::show_writing_status);

    m_application_path = new QString(QDir::currentPath());

    m_facility_reader = nullptr;
}

void App_Controller::set_q_application(QApplication* q_application) noexcept
{
    m_q_application = q_application;
}

QApplication* App_Controller::get_q_application() noexcept
{
    return m_q_application;
}

void App_Controller::set_parameters(int benchmark_id, char *filter, char *parameters) noexcept
{
    m_benchmark_id = benchmark_id;
    if (filter != nullptr)
    {
        m_filter = new QString(filter);
    }
    else
    {
        m_filter = new QString("");
    }

    if (parameters != nullptr)
    {
        m_filter_parameters = new QString(parameters);
    }
    else
    {
        m_filter_parameters = new QString("");
    }
}

void App_Controller::set_hoops_config(char* hoop1_config,  char* hoop2_config) noexcept
{
    if (hoop1_config != nullptr)
    {
        m_hoop1_config = new QString(hoop1_config);
    }

    if (hoop2_config != nullptr)
    {
        m_hoop2_config = new QString(hoop2_config);
    }
}

void App_Controller::start_cmd(const char* path, bool is_file) noexcept
{
    m_is_cmd = true;
    if (path != nullptr)
    {
        if (is_file)
        {
            open_file(QString(path), true, true);
        }
        else
        {
            open_dir(QString(path));
        }
    }
}

void App_Controller::start_gui() noexcept
{
    m_is_cmd = false;
#ifdef GUI
    m_windows_controller = new Windows_Controller(this, m_data_manager->get_log_data());
    m_windows_controller->fill_filters_list(m_transform_manager->get_transforms_info());
    QObject::connect(this, &App_Controller::save_transformation_gui, m_windows_controller, &Windows_Controller::save_transformation);
    QObject::connect(this, &App_Controller::show_dialog_gui, m_windows_controller, &Windows_Controller::show_dialog);
    QObject::connect(this, &App_Controller::set_log_data_gui, m_windows_controller, &Windows_Controller::set_log_data);
    QObject::connect(this, &App_Controller::update_log_data_gui, m_windows_controller, &Windows_Controller::update_log_data);
    QObject::connect(this, SIGNAL(update_filtes_table_gui(W_Log_Data, std::vector<WKTR::Transform_Spec*>)),
                     m_windows_controller, SLOT(update_filtes_table(W_Log_Data, std::vector<WKTR::Transform_Spec*>)));

    m_facility_reader = new Facilitysetup_Reader();
    m_facility_reader->set_data_manager(m_data_manager);
    QObject::connect(m_facility_reader, static_cast<void (Facilitysetup_Reader::*)(bool, const QString &)>(&Facilitysetup_Reader::file_reading_done),
                     [=] (bool is_success, const QString &file_name)
    {
        if (is_success)
        {
            m_windows_controller->update_hoops_setup(m_data_manager->get_hoops());
        }
    });
#endif
}

void App_Controller::open_file(const QString &file_name, bool is_with_label, bool is_with_meta_file) noexcept
{
    if (m_file_writer->is_runnning())
    {
        return;
    }

    if (m_file_reader->is_runnning())
    {
        m_file_reader->cancel();
        m_state = App_State::NOT_WAIT;
    }

    if (!file_name.isEmpty())
    {
        Operation_Mode::get_instance().set_begin_reading_data(file_name);

        m_file_reader->set_data_manager(m_data_manager);
        m_file_reader->set_files_name(file_name);

        if (is_with_meta_file)
        {
            Episode_Meta_Data::get_instance().find_read_meta_file(file_name);
        }

        m_file_reader->execute();
        if (is_with_label)
        {
            m_state = App_State::WAIT_LABELS_AFTER_LOG;
        }
        else
        {
            m_state = App_State::WAIT_LOG;
        }

        if (m_facility_reader != nullptr)
        {
            if (m_facility_reader->is_runnning())
            {
                m_facility_reader->cancel();
            }
            m_facility_reader->set_files_name(UTILS::get_facility_file_path(file_name));
            m_facility_reader->execute();
        }
    }
}

void App_Controller::open_dir(const QString &dir_name) noexcept
{
    if (m_file_writer->is_runnning())
    {
        return;
    }

    m_is_dir = true;

    if (m_file_reader->is_runnning())
    {
        m_file_reader->cancel();
        m_state = App_State::NOT_WAIT;
    }

    m_files_list.clear();
    m_files_list = UTILS::get_log_files_in_dir(dir_name);

    if (m_files_list.size() > 0)
    {
        open_file(m_files_list.at(0), true, true);
        m_files_list.pop_front();
        m_state = App_State::OPEN_LOGS_DIR;
    }

}

void App_Controller::open_labels_file(const QString &file_name) noexcept
{
    if (m_file_writer->is_runnning())
    {
        return;
    }

    if (m_data_manager->get_log_data() != nullptr)
    {
        if (m_file_reader->is_runnning())
        {
            m_file_reader->cancel();
            m_state = App_State::NOT_WAIT;
        }
        if (!file_name.isEmpty())
        {
            Operation_Mode::get_instance().set_begin_reading_data();

            QStringList tmp = file_name.split("/");
            if (tmp.size() > 1)
            {
                tmp[tmp.size() - 2] = "labels";
            }
            QString log_id = tmp.last();
            log_id = log_id.split("_").front();
            tmp.removeLast();
            QDir directory(tmp.join("/"));

            //qDebug() << directory;

            QStringList label_files = directory.entryList(QStringList() << (log_id + "_*_lbl_*.csv"),QDir::Files);

            for (const QString& lf : label_files)
            {
                Label_Mediator::get_instance().open_label(directory.path() + "/" + lf);
                //qDebug() << "read" << lf;
                //Label_Reader::read_file(directory.path() + "/" + lf, m_data_manager->get_log_data());
            }

            m_state = App_State::WAIT_LABELS;

            update_data_from_log(true, file_name);
        }
    }
    else
    {
        qDebug() << "Open file with sensor data";

        // m_windows_controller
        emit show_dialog_gui("Open file with sensor data");
    }
}

void App_Controller::open_errors_file(const QString &file_name) noexcept
{
    if (m_file_writer->is_runnning())
    {
        return;
    }

    if (m_data_manager->get_log_data() != nullptr)
    {
        if (m_file_reader->is_runnning())
        {
            m_file_reader->cancel();
            m_state = App_State::NOT_WAIT;
        }
        if (!file_name.isEmpty())
        {
            m_file_reader->set_files_name(file_name, true);
            Operation_Mode::get_instance().set_begin_reading_data();
            m_file_reader->execute();
            m_state = App_State::WAIT_ERRORS;
        }
    }
    else
    {
        qDebug() << "Open log file first";

        // m_windows_controller
        emit show_dialog_gui("Open log file firstly");
    }
}

void App_Controller::update_data_from_log(bool is_success, const QString &file_name)
{
    Operation_Mode::get_instance().set_end_reading_data();

    if (m_state == App_State::OPEN_LOGS_DIR)
    {
        QString label_file_name = UTILS::get_label_name_file(file_name);

        if (!label_file_name.isEmpty())
        {
            open_labels_file(label_file_name);
            m_state = App_State::WAIT_LABEL_DIR_FORMAT;
        }
        else
        {
            if (m_files_list.size() > 0)
            {
                open_file(m_files_list.at(0), true, true);
                m_files_list.pop_front();
                m_state = App_State::OPEN_LOGS_DIR;
            }
        }
    }
    else if (m_state == App_State::WAIT_LABEL_DIR_FORMAT)
    {
        // m_windows_controller
        emit save_transformation_gui();

        if (m_is_cmd)
        {
            process_all_data(*m_filter, "", *m_filter_parameters);
        }
    }
    else if (m_state == App_State::WAIT_RESULT_LOG)
    {
        bool is_error = false;
        if (m_data_manager->get_log_data() == nullptr)
        {
            // m_windows_controller
            emit show_dialog_gui("Error! App_Controller::update_data_from_log() :: you don't open labels");

            std::cerr << "Error! App_Controller::update_data_from_log() :: you didn't open labels" << std::endl;
            is_error = true;
        }
        if (m_data_manger_test_res->get_log_data() == nullptr)
        {
            // m_windows_controller
            emit show_dialog_gui("Error! App_Controller::update_data_from_log() :: error in result log");

            std::cerr << "Error! App_Controller::update_data_from_log() :: error in result log" << std::endl;
            is_error = true;
        }
        if (!is_error)
        {
#ifdef RQI_UWB
            std::vector<RQI_UWB::Error_Label> output_errors;
            RQI_UWB::calculate_rqi(m_data_manger->get_log_data()->get_name()
                                   , *(m_data_manger->get_log_data()->get_log_labels()->get_map_labels_objects())
                                   , *(m_data_manger_test_res->get_log_data()->get_log_labels()->get_map_labels_objects())
                                   , *(m_data_manger->get_log_data()->get_log_labels()->get_map_labels_types())
                                   , *(m_data_manger_test_res->get_log_data()->get_log_labels()->get_map_labels_types())
                                   , m_data_manger->get_log_data()->get_log_labels()->get_usefull_time()
                                   , m_filter + QString("_") + m_filter_parameters, m_data_manger->get_log_data()->get_data_set_id()
                                   , true, &output_errors);
#endif
        }

        if (m_files_list.size() > 0)
        {
            open_file(m_files_list.at(0), true, true);
            m_files_list.pop_front();
            m_state = App_State::OPEN_LOGS_DIR;
        }
        else
        {
            m_state = App_State::NOT_WAIT;
            qDebug() << "Test is done";

            if (m_is_dir)
            {
                m_is_dir = false;
#ifdef RQI_UWB
                RQI_UWB::show_total_rqi();
#endif
            }

            if (m_is_cmd)
            {
                exit(0);
            }
        }

        if (!QDir::setCurrent(*m_application_path))
        {
            qDebug() << "Could not change the current working directory!";
            exit(0);
        }
    }
    else
    {
        if (m_state == App_State::WAIT_LABELS_AFTER_LOG)
        {
            QString label_file_name = file_name;
            if (!label_file_name.isEmpty())
            {
                if (!m_is_cmd)
                {
                    Settings_Manager::get_instance()->write_last_label_file_path(label_file_name);
                }
                open_labels_file(label_file_name);
            }
            else
            {
                // m_windows_controller]
                emit set_log_data_gui(m_data_manager->get_log_data());
//                emit show_dialog_gui("Label file name is null");

                qDebug() << "Label file name is null";
                if (m_is_cmd)
                {
                    exit(0);
                }
            }
        }
        else if (m_state == App_State::WAIT_ERRORS)
        {
            emit update_log_data_gui(m_data_manager->get_log_data());

            if (!is_success)
            {
                // m_windows_controller
                emit show_dialog_gui("File " + file_name + " not found");
            }
        }
        else
        {
            // m_windows_controller
            emit set_log_data_gui(m_data_manager->get_log_data());

            if (is_success)
            {
                if (m_is_cmd)
                {
                    process_all_data(*m_filter, "", *m_filter_parameters);
                }
            }
            else
            {
                // m_windows_controller
                if (!file_name.contains("Alps"))
                {
                    emit show_dialog_gui("File " + file_name + " not found");
                    qDebug() << "File" << file_name << "not found";
                }
            }
        }
    }

}

void App_Controller::process_all_data(const QString &filter_id, const QString &object_id, const QString &parameters)
{
    if (m_data_manager->get_log_data() == nullptr)
    {
        return;
    }

    bool is_using_apl_benhmark = (filter_id.indexOf("\u03b2") == 0);

    if (is_using_apl_benhmark)
    {
        qDebug() << "Start benchmark prepearing";
        m_wrap_apl_benchmark->initialize(m_benchmark_id, m_data_manager->get_log_data()->get_data_set_id());

        m_wrap_apl_benchmark->set_hoop1_config(m_hoop1_config);
        m_wrap_apl_benchmark->set_hoop2_config(m_hoop2_config);

        if (!m_wrap_apl_benchmark->prepare())
        {
            return;
        }
    }

    if (!m_is_cmd)
    {
        delete m_filter;
        delete m_filter_parameters;
        m_filter = new QString(filter_id);
        m_filter_parameters = new QString(parameters);
    }

    qDebug() << "Start processing";
    QString out_file = m_transform_manager->apply_save_transform_all_data(WKTR::Transform_Spec("", object_id, "", filter_id, parameters));
    qDebug() << "End processing";

    if (!m_is_dir)
    {
        emit update_filtes_table_gui(*m_data_manager->get_log_data(), *m_transform_manager->get_transforms_spec());
        emit update_alg_tmp_labels();
    }

    if (out_file.isEmpty())
    {
        return;
    }

    if (is_using_apl_benhmark)
    {
        qDebug() << "Start benchmark";
        QString eventbuffer_file = m_wrap_apl_benchmark->execute(*m_filter);

        if (eventbuffer_file.length() > 0)
        {
            //3. start reading result log file
            qDebug() << "Start RQI calculating";
            delete m_data_manger_test_res;
            m_data_manger_test_res = new Data_Manager();
            m_file_reader->set_data_manager(m_data_manger_test_res);
            m_file_reader->set_files_name(eventbuffer_file, true);
            m_file_reader->execute();
            m_state = App_State::WAIT_RESULT_LOG; // after stop will be call void App_Controller::update_data_from_log(bool is_success)
        }
        else
        {
            qDebug() << "There is no an evenbuffer.csv file!!!";
        }
    }
    else
    {
        if (m_is_dir)
        {
            if (m_files_list.size() > 0)
            {
                open_file(m_files_list.at(0), true, true);
                m_files_list.pop_front();
                m_state = App_State::OPEN_LOGS_DIR;
            }
            else
            {
                m_state = App_State::NOT_WAIT;
                qDebug() << "Test is done";

                if (m_is_dir)
                {
                    m_is_dir = false;
    #ifdef RQI_UWB
                    RQI_UWB::show_total_rqi();
    #endif
                }
            }
        }
    }
}

void App_Controller::process_filter(const QString &filter_id, const QString &log_id, const QString &object_id, const QString &sensor_id, const QString &parameters)
{
    if (m_data_manager->get_log_data() == nullptr)
    {
        return;
    }

    m_transform_manager->add_transform_spec(WKTR::Transform_Spec(log_id, object_id, sensor_id, filter_id, parameters));

    emit update_alg_tmp_labels();

    // m_windows_controller
    emit update_filtes_table_gui(*m_data_manager->get_log_data(), *m_transform_manager->get_transforms_spec());
}

void App_Controller::update_filter(int id, const QString &filter_id, const QString &log_id, const QString &object_id, const QString &sensor_id, const QString &parameters)
{
    WKTR::Transform_Spec spec;
    spec.m_filter_id = filter_id;
    spec.m_log_id = log_id;
    spec.m_object_id = object_id;
    spec.m_sensor_id = sensor_id;
    spec.m_parameters = parameters;

    m_transform_manager->update_transform_spec(id, spec);

    emit update_alg_tmp_labels();

    // m_windows_controller
    emit update_filtes_table_gui(*m_data_manager->get_log_data(), *m_transform_manager->get_transforms_spec());
}

void App_Controller::delete_filter(int filter_id)
{
    m_transform_manager->delete_transform_spec(filter_id);

    // m_windows_controller
    emit update_filtes_table_gui(*m_data_manager->get_log_data(), *m_transform_manager->get_transforms_spec());
}

void App_Controller::save_file(const QString &file_name) noexcept
{
    if (m_file_writer->is_runnning() || m_file_reader->is_runnning())
    {
        return;
    }

    if (!file_name.isEmpty())
    {
        m_file_writer->set_files_name(file_name);
        m_file_writer->execute();
    }
}

void App_Controller::show_writing_status(bool is_success, const QString &file_name) noexcept
{
    if (is_success)
    {
        qDebug() << "Labels was writed";

        // m_windows_controller
        emit show_dialog_gui("Labels was writed");

        m_data_manager->get_log_data()->get_log_labels()->set_is_modified(false);
    }
}

App_Controller::~App_Controller()
{
    if (m_file_writer->is_runnning())
    {
        m_file_writer->cancel();
        delete m_file_writer;
    }
    if (!m_file_reader->is_runnning())
    {
        m_file_reader->cancel();
        delete m_file_reader;
    }
    delete m_wrap_apl_benchmark;
    delete m_transform_manager;
    delete m_data_manager;
    delete m_data_manger_test_res;
    delete m_filter;
    delete m_filter_parameters;
    delete m_hoop1_config;
    delete m_hoop2_config;

    delete m_windows_controller;
}
