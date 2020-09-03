#include "wlogdata.h"
#include <QDebug>

W_Log_Data:: W_Log_Data(const QString& name)
{
    m_shitf_time = 0;
    m_name = name;
    m_data_set_id = 0;
}

QString W_Log_Data::get_name() const noexcept
{
    return m_name;
}

void W_Log_Data::set_name(QString name) noexcept
{
    m_name = name;
}

W_Object_Data* W_Log_Data::get_object(const QString& name) const noexcept
{
    auto it = m_objects_map.find(name);
    if (it == m_objects_map.end())
    {
        return nullptr;
    }
    return it->second;
}

W_Object_Data* W_Log_Data::get_object_to_change(const QString& name) noexcept
{
    auto it = m_objects_map.find(name);
    if (it == m_objects_map.end())
    {
        return nullptr;
    }
    return it->second;
}

const std::map<QString, W_Object_Data*>* W_Log_Data::get_objects() const noexcept
{
    return &m_objects_map;
}

const std::vector<XYZ_Sample>* W_Log_Data::get_data_vector() const noexcept
{
    return &m_data_vector;
}

Log_Labels* W_Log_Data::get_log_labels() noexcept
{
    return &m_log_labels;
}

void W_Log_Data::set_shitf_time(int64_t shift) noexcept
{
    m_shitf_time = shift;
}

int64_t W_Log_Data::get_shitf_time() const noexcept
{
    return m_shitf_time;
}

int W_Log_Data::add_object(W_Object_Data* object) noexcept
{
    if (m_objects_map.find(object->get_name()) != m_objects_map.end())
    {
        return -1;
    }

    m_objects_map.insert({object->get_name(), object});
    return 1;
}

void W_Log_Data::add_xyz_sample(const XYZ_Sample &s) noexcept
{
    m_data_vector.emplace_back(s);
}

void W_Log_Data::set_log_max_time(uint64_t time) noexcept
{
    m_log_max_time = time;
}

void W_Log_Data::set_log_min_time(uint64_t time) noexcept
{
    m_log_min_time = time;
}

uint64_t W_Log_Data::get_log_max_time() const noexcept
{
    return m_log_max_time;
}

uint64_t W_Log_Data::get_log_min_time() const noexcept
{
    return m_log_min_time;
}

uint64_t W_Log_Data::get_usefull_time() const noexcept
{
    return m_log_labels.get_usefull_time();
}

void W_Log_Data::set_data_set_id(int data_set_id) noexcept
{
    m_data_set_id = data_set_id;
}

int W_Log_Data::get_data_set_id() const noexcept
{
    return m_data_set_id;
}

W_Log_Data::~W_Log_Data()
{
    for (auto &e : m_objects_map)
    {
        W_Object_Data* ptr = e.second;
        delete ptr;
    }
    m_objects_map.clear();
    m_data_vector.clear();
}
