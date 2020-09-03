#include "wobjectdata.h"
#include <QDebug>

W_Object_Data:: W_Object_Data(const QString& name) : m_name(name)
{
    m_is_shown_time_plot = false;
    m_is_shown_court_plots = false;
}

QString W_Object_Data::get_name() const noexcept
{
    return m_name;
}

W_Sensor_Data* W_Object_Data::get_sensor(const QString& name) const noexcept
{
    auto it = m_sensors_map.find(name);
    if (it == m_sensors_map.end())
    {
        return nullptr;
    }
    return it->second;
}

std::map<QString, W_Sensor_Data*>* W_Object_Data::get_sensors() noexcept
{
    return &m_sensors_map;
}

int W_Object_Data::add_sensor(W_Sensor_Data* sensor) noexcept
{
    if (m_sensors_map.find(sensor->get_name()) != m_sensors_map.end())
    {
        return -1;
    }

    m_sensors_map.insert({sensor->get_name(), sensor});
    return 1;
}

void W_Object_Data::remove_sensor(const QString &name) noexcept
{
    auto deleted_it = m_sensors_map.find(name);
    if (deleted_it == m_sensors_map.end())
    {
        return;
    }

    W_Sensor_Data* ptr_deleted_sensor = deleted_it->second;
    delete ptr_deleted_sensor;

    m_sensors_map.erase(deleted_it);
}

void W_Object_Data::set_show_state_time_plot(bool state) noexcept
{
    m_is_shown_time_plot = state;
}

void W_Object_Data::set_show_state_court_plots(bool state) noexcept
{
    m_is_shown_court_plots = state;
}


bool W_Object_Data::is_show() const noexcept
{
    return (m_is_shown_time_plot || m_is_shown_court_plots);
}

W_Object_Data::~W_Object_Data()
{
    for (auto& e : m_sensors_map)
    {
        W_Sensor_Data* ptr = e.second;
        delete ptr;
    }
    m_sensors_map.clear();
}
