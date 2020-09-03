#include "data_manager.h"

#include "parser/apl25_log_parser.h"
#include "parser/apl120_log_parser.h"
#include "parser/apl_label_parser.h"
#include "parser/wk_log_parser.h"
#include "parser/alps_log_parser.h"
#include "parser/mb_log_parser.h"
#include "parser/apl_error_parser.h"
#include "../utils.h"

#include <QDebug>
#include <QList>

Data_Manager::Data_Manager()
{
    m_log_data = nullptr;
    m_parser = nullptr;
}

void Data_Manager::prepare_label_parser(const QString &file_name) noexcept
{
    delete m_parser;
    if (m_log_data == nullptr)
    {
        m_log_data = new W_Log_Data(file_name);
    }

    if (file_name.contains("errors.csv"))
    {
        m_log_data->get_log_labels()->get_all_errors()->clear();
        m_parser = new Apl_Error_Parser();
    }
    else
    {
        m_log_data->get_log_labels()->get_all_labels()->clear();
        m_parser = new Apl_Label_Parser();
    }
}

void Data_Manager::prepare_log_parser(const QString &file_name, LOG_PARSER_TYPE type) noexcept
{
    delete m_parser;
    if (type == LOG_APL_120HZ)
    {
        m_parser = new Apl120_Log_Parser(file_name);
    }
    else if (type == LOG_WK)
    {
        m_parser = new WK_Log_Parser(file_name);
    }
    else if (type == LOG_MB)
    {
        m_parser = new MB_Log_Parser(file_name);
    }
    else if (type == LOG_APL_25HZ)
    {
        m_parser = new Apl25_Log_Parser(file_name);
    }
    else if (type == LOG_ALPS)
    {
        m_parser = new Alps_Log_Parser(file_name);
    }
    check_log_file_name(file_name);
}

void Data_Manager::check_log_file_name(const QString &file_name) noexcept
{
    if (m_log_data != nullptr)
    {
        QString opened_log = UTILS::get_data_set_dir_name(m_log_data->get_name());
        QString new_log = UTILS::get_data_set_dir_name(file_name);

//        if (opened_log.length() > 0 && new_log.length() > 0 && opened_log.compare(new_log) == 0)
//        {
//            m_parser->set_addition_log_key(true);
//            m_log_data->set_name(file_name);
//        }
//        else
        {
            m_parser->set_addition_log_key(false);
            delete m_log_data;
            m_log_data = new W_Log_Data(file_name);
        }
    }
    else
    {
        m_log_data = new W_Log_Data(file_name);
    }
}

void Data_Manager::parse_line(const QString &line)
{    
    m_parser->parse(line, m_log_data);
}

void Data_Manager::write_labels(QTextStream &stream)
{
    uint64_t time_start = m_log_data->get_log_labels()->get_time_start();
    stream << QString::number(time_start) << "," << "time_start" << "\n";

    std::vector<QString> tmp;
    for (Label_Item *item : *m_log_data->get_log_labels()->get_all_labels())
    {
        if (!item->m_is_error)
        {
            tmp.clear();
            auto label_type = APL_LABELS::APL_LABEL_TYPES.find(item->m_type_v);
            if (label_type != APL_LABELS::APL_LABEL_TYPES.end())
            {
                tmp.resize(label_type->second.size());

                for (const auto & it : item->m_tag_value_map)
                {
                    auto tag = label_type->second.find(it.first);
                    if (tag != label_type->second.end())
                    {
                        if (tag->first.compare("timestamp") == 0)
                        {
                            tmp[tag->second] = QString::number(it.second.toLongLong());// + time_start);
                        }
                        else
                        {
                            tmp[tag->second] = it.second.toString();
                        }
                    }
                }
            }

            if (!tmp.empty())
            {
                QStringList result;
                result.sort();
                for (const QString &value : tmp)
                {
                    if (value.size() > 0)
                    {
                        result.push_back(value);
                    }
                }
                stream << result.join(",") << "\n";
            }
        }
    }
}

W_Log_Data* Data_Manager::get_log_data() noexcept
{
    return m_log_data;
}

void Data_Manager::clear_hoops() noexcept
{
    m_hoops.clear();
}

void Data_Manager::add_hoop(int hoop_id, Hoop* hoop) noexcept
{
    m_hoops[hoop_id] = *hoop;
    qDebug() << "hoop id:" << hoop_id << ":" << hoop->m_x << hoop->m_y << hoop->m_z;
}

std::map<int, Hoop>* Data_Manager::get_hoops() noexcept
{
    return &m_hoops;
}

Data_Manager::~Data_Manager()
{
    delete m_log_data;
    delete m_parser;
}
