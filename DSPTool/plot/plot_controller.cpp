#include "plot_controller.h"

#include <QDebug>
#include "court_plot_window.h"
#include "time_plot_window.h"
#include "time_mediator.h"
#include <QTime>

Plots_Controller::Plots_Controller()
{
    m_plot_windows[0] = new Time_Plot_Window();
    m_plot_windows[1] = new Court_Plot_Window(Plot_Type::APL_XY);
    m_plot_windows[2] = new Court_Plot_Window(Plot_Type::APL_XZ);
    m_plot_windows[3] = new Court_Plot_Window(Plot_Type::APL_YZ);

    m_plot_windows[0]->setObjectName("PWT");
    m_plot_windows[1]->setObjectName("PWXY");
    m_plot_windows[2]->setObjectName("PWXZ");
    m_plot_windows[3]->setObjectName("PWYZ");

    m_plot_windows[0]->setWindowTitle(QString("DSP Tool v.") + APP_VERSION + " Time plot");
    m_plot_windows[1]->setWindowTitle(QString("DSP Tool v.") + APP_VERSION + " Top view plot");
    m_plot_windows[2]->setWindowTitle(QString("DSP Tool v.") + APP_VERSION + " Front view plot");
    m_plot_windows[3]->setWindowTitle(QString("DSP Tool v.") + APP_VERSION + " Side view plot");

    m_plot_windows[0]->hide();
    m_plot_windows[1]->hide();
    m_plot_windows[2]->hide();
    m_plot_windows[3]->hide();

//    QObject::connect(m_plot_windows[0], &Base_Plot_Window::change_index_labels_list_signal, m_plot_windows[0], &Base_Plot_Window::update_index_labels_list);

    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            if (i != j)
            {

                QObject::connect(m_plot_windows[i], &Base_Plot_Window::change_index_objects2_list_signal, [=] (int value)
                {
                    m_plot_windows[j]->update_index_objects2_list(value);
                });
                QObject::connect(m_plot_windows[i], &Base_Plot_Window::change_index_labels2_list_signal, [=] (int value)
                {
                    m_plot_windows[j]->update_index_labels2_list(value);
                });
            }
            //QObject::connect(m_plot_windows[i], &Base_Plot_Window::set_position_signal, m_plot_windows[j], &Base_Plot_Window::update_time1_value);
            //QObject::connect(m_plot_windows[i], &Base_Plot_Window::set_deltaT_signal, m_plot_windows[j], &Base_Plot_Window::update_deltaT_value);

            QObject::connect(m_plot_windows[i], &Base_Plot_Window::change_index_objects_list_signal, m_plot_windows[j], &Base_Plot_Window::update_index_objects_list);
            QObject::connect(m_plot_windows[i], &Base_Plot_Window::change_index_labels_list_signal, m_plot_windows[j], &Base_Plot_Window::update_index_labels_list);
            QObject::connect(m_plot_windows[i], &Base_Plot_Window::update_labels_list_signal, m_plot_windows[j], &Base_Plot_Window::update_labels_list);

            QObject::connect(m_plot_windows[i], &Base_Plot_Window::change_indext_objects_error_list_signal, m_plot_windows[j], &Base_Plot_Window::update_index_objects_errors_list);
            QObject::connect(m_plot_windows[i], &Base_Plot_Window::change_index_errors_type_list_signal, m_plot_windows[j], &Base_Plot_Window::update_index_errors_type_list);
        }

        QObject::connect(m_plot_windows[i], &Base_Plot_Window::close_plot_signal, [=] ()
        {
            emit close_plot_by_id_signal(i);
        });
    }

    for (Base_Plot_Window* p : m_plot_windows)
    {
        Time_Mediator::get_instance().add_ptr_plot_window(p);
        connect(p, &Base_Plot_Window::set_position_signal, &Time_Mediator::get_instance(), &Time_Mediator::change_timestamp);
        connect(p, &Base_Plot_Window::set_deltaT_signal, &Time_Mediator::get_instance(), &Time_Mediator::change_plot_delta_time_slot);
        connect(p, &Base_Plot_Window::press_play_pause_signal, &Time_Mediator::get_instance(), &Time_Mediator::play_pause);
        connect(p, &Base_Plot_Window::speed_down_requested, &Time_Mediator::get_instance(), &Time_Mediator::video_speed_down_request);
        connect(p, &Base_Plot_Window::speed_up_requested, &Time_Mediator::get_instance(), &Time_Mediator::video_speed_up_request);
    }
}

Base_Plot_Window* Plots_Controller::get_plot_by_id(int id)
{
    return m_plot_windows[id];
}

void Plots_Controller::update_data(W_Object_Data *object)
{
    for (int i = 0; i < 4; ++i)
    {
        m_plot_windows[i]->update_data(object);
    }
}

void Plots_Controller::set_hoops(std::map<int, Hoop> * hoops) noexcept
{
    for (int i = 1; i < 4; ++i)
    {
        ((Court_Plot_Window*) m_plot_windows[i])->set_hoops(hoops);
    }
}

uint64_t Plots_Controller::get_current_time() noexcept
{
    for (int i = 0; i < 4; ++i)
    {
        if (!m_plot_windows[i]->isHidden())
        {
            return m_plot_windows[i]->get_current_time();
        }
    }
    return 0;
}

void Plots_Controller::set_labels(Log_Labels *labels)
{
    for (int i = 0; i < 4; ++i)
    {
        m_plot_windows[i]->set_labels(labels);
    }
}

void Plots_Controller::update_plots()
{
    for (int i = 0; i < 4; ++i)
    {
//        if (m_plot_windows[i]->isVisible())
//        {
            m_plot_windows[i]->update_plot();
//        }
    }
}

void Plots_Controller::set_log_time(uint64_t time_min, uint64_t time_max)
{
    for (int i = 0; i < 4; ++i)
    {
        m_plot_windows[i]->clear_window();
        m_plot_windows[i]->set_log_time(time_min, time_max);
    }
}

void Plots_Controller::update_alg_tmp_labels()
{
    m_plot_windows[0]->update_alg_tmp_labels();
}

void Plots_Controller::show_plot(int plot_id, int type_id, bool key) noexcept
{
    if (key)
    {
        qDebug() << "show" << m_plot_windows[plot_id]->windowTitle().split(". ").front();
        m_plot_windows[plot_id]->show();
        m_plot_windows[plot_id]->update_time1_value(get_current_time());
    }
    else
    {
        qDebug() << "hide" << m_plot_windows[plot_id]->windowTitle().split(". ").front();
        m_plot_windows[plot_id]->hide();
    }
}

void Plots_Controller::close_all() noexcept
{
    for (int i = 0; i < 4; ++i)
    {
        m_plot_windows[i]->close();
    }
}

Plots_Controller::~Plots_Controller()
{
    for (int i = 0; i < 4; ++i)
    {
        delete m_plot_windows[i];
    }
}
