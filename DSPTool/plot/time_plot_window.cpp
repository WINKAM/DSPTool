#include "time_plot_window.h"

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
#include "settings_manager.h"
#include "view/viewer_plot.h"
#include "data/object/wobjectdata.h"
#include "data/label/log_labels.h"

Time_Plot_Window::Time_Plot_Window(Plot_Type plot_type) : Base_Plot_Window(plot_type, 0)
{
    setup_window();
    int width = Settings_Manager::get_instance()->get_window_parameters("Plot" + QString::number((int)m_plot_type), "width");
    int height = Settings_Manager::get_instance()->get_window_parameters("Plot" + QString::number((int)m_plot_type), "height");
    if (width == -1)
    {
        showMaximized();
    }
    else
    {
        resize(width, height);
    }
}

void Time_Plot_Window::setup_window() noexcept
{
    m_x_center_spin->setVisible(false);
    m_x_width_spin->setVisible(false);
    m_y_center_spin->setVisible(false);
    m_y_height_spin->setVisible(false);

    m_vertical_min_spin->setToolTip("vertical axis minimum");
    m_vertical_max_spin->setToolTip("vertical axis maximum");

    QObject::connect(m_signal_plot, SIGNAL(update_scale_signal(double, double, double, double)), this, SLOT(update_scale_controlls(double, double, double, double)));
    QObject::connect(m_signal_plot, SIGNAL(update_vertical_min_max_signal(double, double)), this, SLOT(update_vertical_min_max(double, double)));
}

void Time_Plot_Window::set_default_zoom() noexcept
{
    m_delta_time_spin->setValue(3000);
    m_vertical_min_spin->setValue(0);
    m_vertical_max_spin->setValue(5000);
}

void Time_Plot_Window::update_plot()
{
    m_signal_plot->clear_all_curves();

    uint64_t time1 = static_cast<uint64_t>(m_time1_slider->value()) + m_time1_slider->property("min_time").toULongLong();
    int delta_time = m_delta_time_slider->value();

    for (const auto &it : *m_objects_to_show)
    {
        W_Object_Data *item = it.second;

        for (const auto &sensor_item : *item->get_sensors())
        {
            W_Sensor_Data *sensor = sensor_item.second;
            for (const auto &signal_item : *sensor->get_signals())
            {
                W_Signal_Data *signal = signal_item.second;
                if (signal != nullptr && signal->is_show())
                {
                    QPolygonF polygon;
                    auto iter = signal->get_samples()->cbegin();
                    bool is_prev_sample_appended = false;
                    while (iter < signal->get_samples()->cend())
                    {
                        if (iter->first >= time1 - delta_time)
                        {
                            if (!is_prev_sample_appended && iter != signal->get_samples()->cbegin()
                                    && (iter - 1)->first < time1 - delta_time)
                            {
                                is_prev_sample_appended = true;
                                polygon.append(QPointF((iter - 1)->first, (iter - 1)->second));
                            }

                            polygon.append(QPointF(iter->first, iter->second));

                            if (iter->first > time1 + delta_time)
                            {
                                break;
                            }
                        }
                        ++iter;
                    }
                    if (polygon.size() > 0)
                    {
                        m_signal_plot->create_curve(signal->get_name(), &polygon, *signal->get_color(), sensor->get_signals_weight());
                    }
                }
            }
        }
    }

    m_signal_plot->replot();
    m_signal_plot->set_interation_type(m_zoom_checkbox->isChecked());
    //update_scale();
}

void Time_Plot_Window::update_scale()
{
    int64_t time1 = static_cast<uint64_t>(m_time1_slider->value()) + m_time1_slider->property("min_time").toULongLong();
    int delta_time = m_delta_time_slider->value();
    int v_min = m_vertical_min_spin->value();
    int v_max = m_vertical_max_spin->value();

    QObject::disconnect(m_signal_plot, SIGNAL(update_scale_signal(double, double, double, double)), this, SLOT(update_scale_controlls(double, double, double, double)));
    m_signal_plot->set_scale(time1 - delta_time, time1 + delta_time, v_min, v_max);
    QObject::connect(m_signal_plot, SIGNAL(update_scale_signal(double, double, double, double)), this, SLOT(update_scale_controlls(double, double, double, double)));

    update_plot();
}

void Time_Plot_Window::set_log_time(uint64_t time_min, uint64_t time_max) noexcept
{
    m_time1_slider->setMaximum(static_cast<int>(time_max - time_min));
    m_time1_slider->setProperty("min_time", qulonglong(time_min));

    m_time1_spin->setMinimum(time_min);
    m_time1_spin->setMaximum(time_max);

    int half_time = (time_max - time_min) / 2 + 1;
    m_delta_time_slider->setMaximum(half_time);
    m_delta_time_spin->setMaximum(half_time);
}

uint64_t Time_Plot_Window::get_log_time() const noexcept
{
    return static_cast<uint64_t>(m_time1_slider->maximum()) + m_time1_slider->property("min_time").toULongLong();
}

void Time_Plot_Window::update_label_item_filter()
{    
    m_selected_label_vector.clear();

    if (m_objects_box->currentIndex() == 0) // none
    {
        m_current_label_iterator = m_selected_label_vector.begin();
        m_signal_plot->clear_markers_by_type(Plot_Marker_Type::LABEL);
        return;
    }

    if (m_objects_box->count() > 0 && m_labels_box->count() > 0)
    {        
        QString object_id = m_objects_box->currentText().split(";").back().simplified();
        QString label_type = m_labels_box->currentText();
        QString label_subtype = m_subtype_box->currentText();

        std::vector<Label_Item *>* tmp_ptr_label_vector = nullptr;

        if (m_objects_box->currentIndex() == 1) // all
        {
            if (m_labels_box->currentIndex() == 1)
            {
                tmp_ptr_label_vector = m_labels->get_all_labels();
            }
            else if (m_labels_box->currentIndex() > 1)
            {
                tmp_ptr_label_vector = m_labels->get_labels_by_type(label_type);
            }
        }
        else
        {
            if (m_labels_box->currentIndex() == 1)
            {
                tmp_ptr_label_vector = m_labels->get_labels_by_object(object_id);
            }
            else if (m_labels_box->currentIndex() > 1)
            {
                tmp_ptr_label_vector = m_labels->get_labels_by_object_and_type(object_id, label_type);
            }
        }

        std::vector<Label_Item *> tmp;

        if (tmp_ptr_label_vector != nullptr
                && label_subtype.compare("all") != 0
                && label_subtype.compare("none") != 0)
        {
            for (Label_Item* li : *tmp_ptr_label_vector)
            {
                auto iter = li->m_tag_value_map.find("event_subtype");
                if ((iter == li->m_tag_value_map.cend() && label_subtype.compare("-") == 0)
                        || (iter != li->m_tag_value_map.cend() && iter->second.toString().compare(label_subtype) == 0))
                {
                    tmp.emplace_back(li);
                }
            }

            tmp_ptr_label_vector = &tmp;
        }

        if (tmp_ptr_label_vector != nullptr)
        {
            m_selected_label_vector = *tmp_ptr_label_vector;
        }

        if (!m_selected_label_vector.empty())
        {
            std::sort(m_selected_label_vector.begin(), m_selected_label_vector.end(), [](const Label_Item *a, const Label_Item *b) -> bool
            {
                return a->m_timestamp < b->m_timestamp;
            });

            m_current_label_iterator = m_selected_label_vector.begin();

            m_signal_plot->clear_markers_by_type(Plot_Marker_Type::LABEL);
            for (const Label_Item *label_item : m_selected_label_vector)
            {
                QString message;
                for (const auto &it : label_item->m_tag_value_map)
                {
                    message.append(it.first);
                    message.append(": ");
                    message.append(it.second.toString());
                    message.append("\n");
                }
                m_signal_plot->add_markers(label_item->m_timestamp, 2, "black", Plot_Marker_Type::LABEL, message);
            }
        }
        else
        {
            m_signal_plot->clear_markers_by_type(Plot_Marker_Type::LABEL);
        }
    }
}


void Time_Plot_Window::update_error_item_filter()
{
    m_selected_errors_vector.clear();
    if (m_objects_error_box->count() > 0 && m_errors_type_box->count() > 0)
    {
        QString object_id = m_objects_error_box->currentText();
        QString label_type = m_errors_type_box->currentText();

        std::vector<Label_Item *>* tmp_ptr_errors_vector = nullptr;

        if (m_errors_type_box->currentIndex() == 1)
        {
            tmp_ptr_errors_vector = m_labels->get_errors_by_object(object_id);
        }
        else if (m_errors_type_box->currentIndex() > 1)
        {
            tmp_ptr_errors_vector = m_labels->get_errors_by_object_and_type(object_id, label_type);
        }

        if (tmp_ptr_errors_vector != nullptr)
        {
            m_selected_errors_vector = *tmp_ptr_errors_vector;
        }

        if (!m_selected_errors_vector.empty())
        {
            std::sort(m_selected_errors_vector.begin(), m_selected_errors_vector.end(), [](const Label_Item *a, const Label_Item *b) -> bool
            {
                return a->m_timestamp < b->m_timestamp;
            });

            m_current_error_iterator = m_selected_errors_vector.begin();

            m_signal_plot->clear_markers_by_type(Plot_Marker_Type::ERROR);
            for (const Label_Item *label_item : m_selected_errors_vector)
            {
                QString message;
                for (const auto &it : label_item->m_tag_value_map)
                {
                    message.append(it.first);
                    message.append(": ");
                    message.append(it.second.toString());
                    message.append("\n");
                }
                m_signal_plot->add_markers(label_item->m_timestamp, 2, "#EDD400", Plot_Marker_Type::ERROR, message); //
            }
        }
        else
        {
            m_signal_plot->clear_markers_by_type(Plot_Marker_Type::ERROR);
        }
    }
}


void Time_Plot_Window::update_vertical_min_max(double min_y, double max_y)
{
    //qDebug() << "!!" << min_y << max_y;
    m_vertical_min_spin->setValue(min_y);
    m_vertical_max_spin->setValue(max_y);
}

void Time_Plot_Window::update_max_time(unsigned int time)
{
    m_time1_slider->setMaximum(time);
    m_time1_spin->setMaximum(time);
}

int Time_Plot_Window::get_delta_time() const noexcept
{
    return m_delta_time_slider->value();
}

std::pair<int, int> Time_Plot_Window::get_min_max() const noexcept
{
    return {m_vertical_min_spin->value(), m_vertical_max_spin->value()};
}

void Time_Plot_Window::add_data_series(QPolygonF* data_series_polygone, const QString& name, const QString& color, int width) noexcept
{
    m_signal_plot->create_curve(name, data_series_polygone, color, width);
    m_signal_plot->set_interation_type(m_zoom_checkbox->isChecked());
}

void Time_Plot_Window::add_data_series(const std::vector<std::pair<float, float>>& data_series_vector, const QString& name, const QString& color, int width) noexcept
{
    QPolygonF polygon;
    polygon.reserve(data_series_vector.size());
    for (std::pair<float, float> p : data_series_vector)
    {
        polygon.append(QPointF(p.first, p.second));            
    }
    //qDebug() << polygon.size();
    m_signal_plot->create_curve(name, &polygon, color, width);
    m_signal_plot->set_interation_type(m_zoom_checkbox->isChecked());
}

void Time_Plot_Window::clear_all_data_series() noexcept
{
    m_signal_plot->clear_all_curves();
    m_signal_plot->clear_all_markers();
}

void Time_Plot_Window::replot() noexcept
{
    m_signal_plot->replot();
}

Time_Plot_Window::~Time_Plot_Window()
{

}
