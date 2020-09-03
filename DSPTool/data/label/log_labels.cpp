#include "log_labels.h"
#include <QDebug>

Log_Labels::Log_Labels()
{
    m_time_start = 0;
    m_is_modified = false;
}

void Log_Labels::add_label_item(Label_Item* label_item) noexcept
{
    m_all_labels_list.push_back(label_item);

    std::vector<Label_Item *>* labels_by_type = get_labels_by_type(label_item->m_type_v);
    if (labels_by_type == nullptr)
    {
        labels_by_type = new std::vector<Label_Item*>();
        labels_by_type->push_back(label_item);
        m_labels_by_type_map[label_item->m_type_v] = *labels_by_type;
    }
    else
    {
        labels_by_type->push_back(label_item);
    }
}

void Log_Labels::add_label_by_object(const QString &object_id, Label_Item *label_item) noexcept
{
    std::vector<Label_Item *>* labels_by_object = get_labels_by_object(object_id);
    if (labels_by_object == nullptr)
    {
        labels_by_object = new std::vector<Label_Item *>();
        labels_by_object->push_back(label_item);
        m_labels_by_object_map[object_id] = *labels_by_object;
    }
    else
    {
        labels_by_object->push_back(label_item);
    }
}

void Log_Labels::delete_label(Label_Item* label_item) noexcept
{

    label_item->m_tag_value_map.clear();

    auto it = std::find(m_all_labels_list.begin(), m_all_labels_list.end(), label_item);
    if (it != m_all_labels_list.end())
    {
        m_all_labels_list.erase(it);
    }

    for (auto &it_vec : m_labels_by_object_map)
    {
        std::vector<Label_Item *>* tmp = &it_vec.second;
        auto ittt = std::find(tmp->begin(), tmp->end(), label_item);
        if (ittt != tmp->end())
        {
            tmp->erase(ittt);
        }
    }

    auto itt = m_labels_by_type_map.find(label_item->m_type_v);
    if (itt != m_labels_by_type_map.end())
    {
        std::vector<Label_Item *>* tmp = &itt->second;
        auto ittt = std::find(tmp->begin(), tmp->end(), label_item);
        if (ittt != tmp->end())
        {
            delete *ittt;
            tmp->erase(ittt);
        }
    }
}

void Log_Labels::add_error_item(Label_Item* label_item) noexcept
{
    m_all_errors_list.push_back(label_item);

    std::vector<Label_Item *>* errors_by_type = get_errors_by_type(label_item->m_type_v);
    if (errors_by_type == nullptr)
    {
        errors_by_type = new std::vector<Label_Item*>();
        errors_by_type->push_back(label_item);
        m_errors_by_type_map[label_item->m_type_v] = *errors_by_type;
    }
    else
    {
        errors_by_type->push_back(label_item);
    }
}

void Log_Labels::add_error_by_object(const QString &object_id, Label_Item *label_item) noexcept
{
    std::vector<Label_Item *>* errors_by_object = get_errors_by_object(object_id);
    if (errors_by_object == nullptr)
    {
        errors_by_object = new std::vector<Label_Item *>();
        errors_by_object->push_back(label_item);
        m_errors_by_object_map[object_id] = *errors_by_object;
    }
    else
    {
        errors_by_object->push_back(label_item);
    }
}

std::map<QString, std::vector<Label_Item *>>* Log_Labels::get_map_labels_objects() noexcept
{
    return &m_labels_by_object_map;
}

std::map<QString, std::vector<Label_Item *>>* Log_Labels::get_map_labels_types() noexcept
{
    return &m_labels_by_type_map;
}

std::vector<Label_Item *>* Log_Labels::get_all_labels() noexcept
{
    return &m_all_labels_list;
}

std::vector<Label_Item *>* Log_Labels::get_labels_by_object(const QString &object_id) noexcept
{
    auto it = m_labels_by_object_map.find(object_id);
    if (it == m_labels_by_object_map.end())
    {
        return nullptr;
    }
    return &it->second;
}

std::vector<Label_Item *>* Log_Labels::get_labels_by_object(const QString &object_id, const QString &error_type) noexcept
{
    std::vector<Label_Item *>* labels_vector = new std::vector<Label_Item *>();
    auto it = m_labels_by_object_map.find(object_id);
    if (it == m_labels_by_object_map.end())
    {
        return labels_vector;
    }

    for (Label_Item *label_item : *&it->second)
    {
        for (const auto &item : label_item->m_tag_value_map)
        {
            if (item.second.toString().contains(error_type, Qt::CaseInsensitive))
            {
                labels_vector->push_back(label_item);
            }
        }

    }

    return labels_vector;
}

std::vector<Label_Item *>* Log_Labels::get_labels_by_type(const QString &type) noexcept
{
    auto it = m_labels_by_type_map.find(type);
    if (it == m_labels_by_type_map.end())
    {
        return nullptr;
    }
    return &it->second;
}

std::vector<Label_Item *>* Log_Labels::get_labels_by_object_and_type(const QString &object_id, const QString &type) noexcept
{
    std::vector<Label_Item *>* labels_vector = new std::vector<Label_Item *>();
    auto it = m_labels_by_object_map.find(object_id);
    if (it == m_labels_by_object_map.end())
    {
        return labels_vector;
    }

    for (Label_Item *label_item : *&it->second)
    {
        if (label_item->m_type_v.compare(type) == 0)
        {
            labels_vector->push_back(label_item);
        }
    }

    return labels_vector;
}

std::vector<Label_Item *>* Log_Labels::get_labels_by_object_and_type(const QString &object_id, const QString &type, const QString &error_type) noexcept
{
    std::vector<Label_Item *>* labels_vector = new std::vector<Label_Item *>();
    auto it = m_labels_by_object_map.find(object_id);
    if (it == m_labels_by_object_map.end())
    {
        return labels_vector;
    }

    for (Label_Item *label_item : *&it->second)
    {
        if (label_item->m_type_v.compare(type) == 0)
        {
            for (const auto &item : label_item->m_tag_value_map)
            {
                if (item.second.toString().contains(error_type, Qt::CaseInsensitive))
                {
                    labels_vector->push_back(label_item);
                }
            }
        }
    }

    return labels_vector;
}

const QStringList* Log_Labels::get_labels_name_list() const noexcept
{
    QStringList *list = new QStringList();
    for (const auto &item : m_labels_by_type_map)
    {
        list->append(item.first);
    }
    return list;
}

////
std::vector<Label_Item *>* Log_Labels::get_all_errors() noexcept
{
    return &m_all_errors_list;
}

std::vector<Label_Item *>* Log_Labels::get_errors_by_object(const QString &object_id) noexcept
{
    auto it = m_errors_by_object_map.find(object_id);
    if (it == m_errors_by_object_map.end())
    {
        return nullptr;
    }
    return &it->second;
}

std::vector<Label_Item *>* Log_Labels::get_errors_by_object(const QString &object_id, const QString &error_type) noexcept
{
    std::vector<Label_Item *>* errors_vector = new std::vector<Label_Item *>();
    auto it = m_errors_by_object_map.find(object_id);
    if (it == m_errors_by_object_map.end())
    {
        return errors_vector;
    }

    for (Label_Item *label_item : *&it->second)
    {
        for (const auto &item : label_item->m_tag_value_map)
        {
            if (item.second.toString().contains(error_type, Qt::CaseInsensitive))
            {
                errors_vector->push_back(label_item);
            }
        }

    }

    return errors_vector;
}

std::vector<Label_Item *>* Log_Labels::get_errors_by_type(const QString &type) noexcept
{
    auto it = m_errors_by_type_map.find(type);
    if (it == m_errors_by_type_map.end())
    {
        return nullptr;
    }
    return &it->second;
}

std::vector<Label_Item *>* Log_Labels::get_errors_by_object_and_type(const QString &object_id, const QString &type) noexcept
{
    std::vector<Label_Item *>* errors_vector = new std::vector<Label_Item *>();
    auto it = m_errors_by_object_map.find(object_id);
    if (it == m_errors_by_object_map.end())
    {
        return errors_vector;
    }

    for (Label_Item *label_item : *&it->second)
    {
        if (label_item->m_type_v.compare(type) == 0)
        {
            errors_vector->push_back(label_item);
        }
    }

    return errors_vector;
}

std::vector<Label_Item *>* Log_Labels::get_errors_by_object_and_type(const QString &object_id, const QString &type, const QString &error_type) noexcept
{
    std::vector<Label_Item *>* errors_vector = new std::vector<Label_Item *>();
    auto it = m_errors_by_object_map.find(object_id);
    if (it == m_errors_by_object_map.end())
    {
        return errors_vector;
    }

    for (Label_Item *label_item : *&it->second)
    {
        if (label_item->m_type_v.compare(type) == 0)
        {
            for (const auto &item : label_item->m_tag_value_map)
            {
                if (item.second.toString().contains(error_type, Qt::CaseInsensitive))
                {
                    errors_vector->push_back(label_item);
                }
            }
        }
    }

    return errors_vector;
}

const QStringList* Log_Labels::get_errors_name_list() const noexcept
{
    QStringList *list = new QStringList();
    for (const auto &item : m_errors_by_type_map)
    {
        list->append(item.first);
    }
    return list;
}
////

bool Log_Labels::is_modified()
{
    return m_is_modified;
}

void Log_Labels::set_is_modified(bool value)
{
    m_is_modified = value;
}

void Log_Labels::set_time_start(uint64_t time) noexcept
{
    m_time_start = time;
}

uint64_t Log_Labels::get_time_start() const noexcept
{
    return m_time_start;
}

uint64_t Log_Labels::get_usefull_time() const noexcept
{
    if (m_all_labels_list.size() > 1)
    {

        auto minmax = std::minmax_element(m_all_labels_list.begin(), m_all_labels_list.end()
                                          , [] (Label_Item const* lhs, Label_Item const* rhs)
        { return lhs->m_timestamp < rhs->m_timestamp; });

        return (*(minmax.second))->m_timestamp - (*(minmax.first))->m_timestamp;

    }
    else
    {
        return 1;
    }
}

Log_Labels::~Log_Labels()
{
    for (Label_Item *item : m_all_labels_list)
    {
        delete item;
    }

    m_all_labels_list.clear();
    m_labels_by_object_map.clear();
    m_labels_by_type_map.clear();

    for (Label_Item *item : m_all_errors_list)
    {
        delete item;
    }

    m_all_errors_list.clear();
    m_errors_by_object_map.clear();
    m_errors_by_type_map.clear();
}
