#include "court_plot_window.h"

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
#include <ctime>
#include "settings_manager.h"
#include "view/viewer_plot.h"
#include "data/object/wobjectdata.h"
#include "data/label/log_labels.h"
#include "data/data_manager.h"

Court_Plot_Window::Court_Plot_Window(Plot_Type type) : Base_Plot_Window(type)
{
    m_court_type = type;
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

void Court_Plot_Window::setup_window() noexcept
{
    m_vertical_max_spin->setVisible(false);
    m_vertical_min_spin->setVisible(false);
    m_vert_max_label->setVisible(false);
    m_vert_min_label->setVisible(false);

    m_signal_plot->set_grid_visible(false);
    m_signal_plot->set_central_line_visible(false);
    //m_signal_plot->hide_axis_labels();

    m_basketball_court = new Apl_Basketball_Court();
    m_basketball_court->build(m_court_type, m_signal_plot);
    m_basketball_court->draw_hoop(m_court_type, 0, 11619.8, 3050, "blue"); // default ncaa
    m_basketball_court->draw_hoop(m_court_type, 0, -11619.8, 3050, "black"); // default ncaa


    QObject::connect(m_signal_plot, &Viewer_Plot::update_scale_signal
                     , [=]() {update_scale_controls(false);});

    QObject::connect(m_signal_plot, &Viewer_Plot::update_resize_plot_signal
                     , [=]() { update_scale_controls(true);});

    QObject::connect(m_x_center_spin, SIGNAL(valueChanged(int))
                     , this, SLOT(update_from_scale_controls()));

    QObject::connect(m_y_center_spin, SIGNAL(valueChanged(int))
                     , this, SLOT(update_from_scale_controls()));


    QObject::connect(m_x_width_spin, SIGNAL(valueChanged(int))
                     , this, SLOT(update_from_scale_controls()));

    QObject::connect(m_y_height_spin, SIGNAL(valueChanged(int))
                     , this, SLOT(update_from_scale_controls()));

    switch (m_plot_type)
    {
    case Plot_Type::APL_XY:
        m_x_width_spin->setToolTip("horizontal axis (Y) range (mm)");
        m_y_height_spin->setToolTip("vertical axis (X) range (mm)");
        m_x_center_spin->setToolTip("horizontal axis (Y) shift (mm)");
        m_y_center_spin->setToolTip("vertical axis (X) shift (mm)");
        break;
    case Plot_Type::APL_XZ:
        m_x_width_spin->setToolTip("horizontal axis (X) range (mm)");
        m_y_height_spin->setToolTip("vertical axis (Z) range (mm)");
        m_x_center_spin->setToolTip("horizontal axis (X) shift (mm)");
        m_y_center_spin->setToolTip("vertical axis (Z) shift (mm)");
        break;
    case Plot_Type::APL_YZ:
        m_x_width_spin->setToolTip("horizontal axis (Y) range (mm)");
        m_y_height_spin->setToolTip("vertical axis (Z) range (mm)");
        m_x_center_spin->setToolTip("horizontal axis (Y) shift (mm)");
        m_y_center_spin->setToolTip("vertical axis (Z) shift (mm)");
        break;
    default:
        break;

    }


}

void Court_Plot_Window::set_default_zoom() noexcept
{
    switch (m_plot_type)
    {
    case Plot_Type::APL_XY:
        m_x_center_spin->setValue(0);
        m_y_center_spin->setValue(0);

        m_x_width_spin->setValue(34000);
        m_y_height_spin->setValue(16000);
        break;

    case Plot_Type::APL_XZ:
        m_x_center_spin->setValue(0);
        m_y_center_spin->setValue(0);

        m_x_width_spin->setValue(16000);
        m_y_height_spin->setValue(6000);
        m_y_center_spin->setValue(3000);
        break;

    case Plot_Type::APL_YZ:
        m_x_center_spin->setValue(0);
        m_y_center_spin->setValue(0);

        m_x_width_spin->setValue(26469);
        m_y_height_spin->setValue(13300);
        m_y_center_spin->setValue(4000);
        break;
    }
}

void Court_Plot_Window::update_plot()
{
    m_signal_plot->clear_all_curves();
    uint64_t time1 = static_cast<uint64_t>(m_time1_slider->value()) + m_time1_slider->property("min_time").toULongLong();
    int delta_time = m_delta_time_slider->value();

    m_signal_plot->clear_id_objects();

    /// draw curves
    for (const auto &it : *m_objects_to_show)
    {
        W_Object_Data *item = it.second;

        for (const auto &sensor_item : *item->get_sensors())
        {
            W_Sensor_Data *sensor = sensor_item.second;

            if (sensor != nullptr && sensor->is_show())
            {
                W_Signal_Data *x_signal = sensor->get_signal("x");
                W_Signal_Data *y_signal = sensor->get_signal("y");
                W_Signal_Data *z_signal = sensor->get_signal("z");

                if (m_court_type == Plot_Type::APL_XY)
                {
                    if (x_signal != nullptr && y_signal != nullptr)
                    {
                        QPolygonF polygon;
                        std::vector<XYTimestamp> xyt_vector;
                        auto iter_x = x_signal->get_samples()->cbegin();
                        auto iter_y = y_signal->get_samples()->cbegin();
                        while (iter_x < x_signal->get_samples()->cend())
                        {
                            if (iter_x->first >= (time1 - delta_time))
                            {
                                if (iter_x->first > time1)
                                {
                                    break;
                                }

                                QPointF p(iter_y->second, -1 * iter_x->second);
                                polygon.append(p);

                                xyt_vector.push_back(XYTimestamp(p, iter_x->first));
                            }
                            ++iter_x;
                            ++iter_y;
                        }
                        if (polygon.size() > 0)
                        {
                            m_signal_plot->create_curve(item->get_name() + " " + x_signal->get_name() + y_signal->get_name(), &polygon, *sensor->get_color(), sensor->get_weight(), xyt_vector);
                        }
                    }
                }

                if (m_court_type == Plot_Type::APL_XZ)
                {
                    if (x_signal != nullptr && z_signal != nullptr)
                    {
                        QPolygonF polygon;
                        std::vector<XYTimestamp> xyt_vector;
                        auto iter_x = x_signal->get_samples()->cbegin();
                        auto iter_z = z_signal->get_samples()->cbegin();
                        while (iter_x < x_signal->get_samples()->cend())
                        {
                            if (iter_x->first >= (time1 - delta_time))
                            {
                                if (iter_x->first > time1)
                                {
                                    break;
                                }

                                QPointF p(-1 * iter_x->second, iter_z->second);
                                polygon.append(p);
                                xyt_vector.push_back(XYTimestamp(p, iter_x->first));
                            }
                            ++iter_x;
                            ++iter_z;
                        }
                        if (polygon.size() > 0)
                        {
                            m_signal_plot->create_curve(item->get_name() + " " + x_signal->get_name() + z_signal->get_name(), &polygon, *sensor->get_color(), sensor->get_weight(), xyt_vector);
                        }
                    }
                }

                if (m_court_type == Plot_Type::APL_YZ)
                {
                    if (y_signal != nullptr && z_signal != nullptr)
                    {
                        QPolygonF polygon;
                        std::vector<XYTimestamp> xyt_vector;
                        auto iter_y = y_signal->get_samples()->cbegin();
                        auto iter_z = z_signal->get_samples()->cbegin();
                        while (iter_y < y_signal->get_samples()->cend())
                        {
                            if (iter_y->first >= (time1 - delta_time))
                            {
                                if (iter_y->first > time1)
                                {
                                    break;
                                }

                                QPointF p(iter_y->second, iter_z->second);
                                polygon.append(p);
                                xyt_vector.push_back(XYTimestamp(p, iter_y->first));
                            }
                            ++iter_y;
                            ++iter_z;
                        }
                        if (polygon.size() > 0)
                        {
                            m_signal_plot->create_curve(item->get_name() + " " + y_signal->get_name() + z_signal->get_name(), &polygon, *sensor->get_color(), sensor->get_weight(), xyt_vector);
                        }
                    }
                }
            }
        }
    }

    m_signal_plot->replot();
}

void Court_Plot_Window::update_scale_controls(bool is_need_fix_proportion) noexcept
{
    std::vector<double> scales = m_signal_plot->get_scale();

    if (is_need_fix_proportion)
    {
        m_xy_scale_proportion = (scales[1] - scales[0]) / (scales[3] - scales[2]);
    }

    if (is_need_fix_proportion
            && m_x_width_spin->value() != static_cast<int>(scales[1] - scales[0])
            && m_y_height_spin->value() != static_cast<int>(scales[3] - scales[2]))
    {
        m_signal_plot->set_scale(m_x_center_spin->value() - m_x_width_spin->value() / 2
                                 , m_x_center_spin->value() + m_x_width_spin->value() / 2
                                 , m_y_center_spin->value() - m_y_height_spin->value() / 2
                                 , m_y_center_spin->value() + m_y_height_spin->value() / 2);

        m_signal_plot->replot();

        update_from_scale_controls();
        return;
    }

    m_x_center_spin->setValue(static_cast<int>((scales[1] + scales[0]) / 2));
    m_x_width_spin->setValue(static_cast<int>(scales[1] - scales[0]));
    m_y_center_spin->setValue(static_cast<int>((scales[3] + scales[2]) / 2));
    m_y_height_spin->setValue(static_cast<int>(scales[3] - scales[2]));
}

void Court_Plot_Window::update_from_scale_controls() noexcept
{
    QObject* sender = QObject::sender();
    if (sender != m_x_center_spin || m_y_center_spin)
    {
        if (sender == m_x_width_spin)
        {
            if (m_x_width_spin->value() < 500)
            {
                // to avoid distortion of scale XY proportion.
                return;
            }

            QObject::disconnect(m_y_height_spin, SIGNAL(valueChanged(int))
                                , this, SLOT(update_from_scale_controls()));

            m_y_height_spin->setValue(m_x_width_spin->value() / m_xy_scale_proportion);

            QObject::connect(m_y_height_spin, SIGNAL(valueChanged(int))
                             , this, SLOT(update_from_scale_controls()));
        }
        else
        {
            if (m_y_height_spin->value() < 500)
            {
                // to avoid distortion of scale XY proportion.
                return;
            }

            QObject::disconnect(m_x_width_spin, SIGNAL(valueChanged(int))
                                , this, SLOT(update_from_scale_controls()));

            m_x_width_spin->setValue(m_y_height_spin->value() * m_xy_scale_proportion);

            QObject::connect(m_x_width_spin, SIGNAL(valueChanged(int))
                             , this, SLOT(update_from_scale_controls()));

        }
    }

    m_signal_plot->set_scale(m_x_center_spin->value() - m_x_width_spin->value() / 2
                             , m_x_center_spin->value() + m_x_width_spin->value() / 2
                             , m_y_center_spin->value() - m_y_height_spin->value() / 2
                             , m_y_center_spin->value() + m_y_height_spin->value() / 2);

}

void Court_Plot_Window::update_scale()
{
    update_plot();
    //    int time1 = m_time1_slider->value();
    //    int delta_time = m_delta_time_slider->value();
    //    int v_min = m_vertical_min_spin->value();
    //    int v_max = m_vertical_max_spin->value();
    //m_signal_plot->set_scale(1000, 20000, 1000, 20000);
}

void Court_Plot_Window::set_log_time(uint64_t time_min, uint64_t time_max) noexcept
{
    m_time1_slider->setMaximum(static_cast<int>(time_max - time_min));
    m_time1_slider->setProperty("min_time", qulonglong(time_min));

    m_time1_spin->setMinimum(time_min);
    m_time1_spin->setMaximum(time_max);
}

uint64_t Court_Plot_Window::get_log_time() const noexcept
{
    return static_cast<uint64_t>(m_time1_slider->maximum()) + m_time1_slider->property("min_time").toULongLong();
}

void Court_Plot_Window::set_hoops(std::map<int, Hoop> * hoops) noexcept
{
    m_signal_plot->detachItems(QwtPlotItem::Rtti_PlotItem, false);
    m_basketball_court->draw_court(m_court_type);
    m_basketball_court->draw_hoop(m_court_type, hoops->at(1).m_x, hoops->at(1).m_y, hoops->at(1).m_z, "blue");
    m_basketball_court->draw_hoop(m_court_type, hoops->at(2).m_x, hoops->at(2).m_y, hoops->at(2).m_z, "black");
}

void Court_Plot_Window::update_label_item_filter()
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
        QString object_id = m_objects_box->currentText();
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
        }
    }
}

void Court_Plot_Window::update_error_item_filter()
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
        }
    }
}

Court_Plot_Window::~Court_Plot_Window()
{
}
