#include "wsensordata.h"
#include <QDebug>

W_Sensor_Data::W_Sensor_Data(const QString& name) : m_name(name)
{
    m_shift_time = 0;
    m_split_time = 1.0;
    m_color = "red";
    m_weight = 2;
    m_is_show = 0;

    int r = 30 + (qrand() % 220);
    int g = 30 + (qrand() % 220);
    int b = 30 + (qrand() % 220);
    m_color_generator = QColor(r, g, b).dark(150);
    m_color = m_color_generator.name();
}

int W_Sensor_Data::add_signal(W_Signal_Data* signal)
{
    if (m_signals_map.find(signal->get_name()) != m_signals_map.end())
    {
        return -1;
    }
    m_color_generator = m_color_generator.light(120);
    signal->set_color(m_color_generator.name());
    m_signals_map.insert({signal->get_name(), signal});
    return 1;
}

W_Signal_Data* W_Sensor_Data::get_signal(QString name)
{
    auto it = m_signals_map.find(name);
    if(it == m_signals_map.end())
    {
        return nullptr;
    }
    return it->second;
}

std::map<QString, W_Signal_Data*>* W_Sensor_Data::get_signals() noexcept
{
    return &m_signals_map;
}

void W_Sensor_Data::set_shift_time(int64_t shift) noexcept
{
    m_shift_time = shift;
}

void W_Sensor_Data::set_split_time(double split) noexcept
{
    m_split_time = split;
}

int64_t W_Sensor_Data::get_shift_time() const noexcept
{
    return m_shift_time;
}

double W_Sensor_Data::get_split_time() const  noexcept
{
    return m_split_time;
}

void W_Sensor_Data::set_color(const QString &color) noexcept
{
    m_color = color;
}

QString* W_Sensor_Data::get_color() noexcept
{
    return &m_color;
}

void W_Sensor_Data::set_weight(int weight) noexcept
{
    m_weight = weight;
}

void W_Sensor_Data::set_signals_weight(int weight) noexcept
{
    for (auto& s : m_signals_map)
    {
        s.second->set_width(weight);
    }
}

int W_Sensor_Data::get_weight() const noexcept
{
    return m_weight;
}

int W_Sensor_Data::get_signals_weight() const noexcept
{
    if (!m_signals_map.empty())
    {
        return m_signals_map.begin()->second->get_width();
    }
    return 0;
}

QString W_Sensor_Data::get_name() const noexcept
{
    return m_name;
}

int W_Sensor_Data::is_show() const noexcept
{
    return m_is_show;
}

void W_Sensor_Data::set_show_state(int state) noexcept
{
    m_is_show = state;
}

W_Sensor_Data::~W_Sensor_Data()
{
    for (auto &e : m_signals_map)
    {
        W_Signal_Data* ptr = e.second;
        delete ptr;
    }
    m_signals_map.clear();
}
