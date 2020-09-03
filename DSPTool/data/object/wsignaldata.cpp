#include "wsignaldata.h"
#include <QDebug>

W_Signal_Data::W_Signal_Data(const QString& name) noexcept : m_name(name)
{
    m_color = "red";
    m_width = 2;
    m_samples = new Sample_Vector;
    m_is_show = 0;
}

QString W_Signal_Data::get_name() const noexcept
{
    return m_name;
}

void W_Signal_Data::add_sample(uint64_t time, double value) noexcept
{
    m_samples->emplace_back(Sample{time, value});
}

Sample_Vector* W_Signal_Data::get_samples() const noexcept
{
    return m_samples;
}

int W_Signal_Data::is_show() const noexcept
{
    return m_is_show;
}

void W_Signal_Data::set_show_state(int state) noexcept
{
    m_is_show = state;
}

void W_Signal_Data::set_width(int width) noexcept
{
    m_width = width;
}

int W_Signal_Data::get_width() noexcept
{
    return m_width;
}

void W_Signal_Data::set_color(const QString &color) noexcept
{
    m_color = color;
}

QString* W_Signal_Data::get_color() noexcept
{
    return &m_color;
}

W_Signal_Data::~W_Signal_Data()
{
    m_samples->clear();
    delete m_samples;
}
