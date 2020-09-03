/***************************************************************************

             WINKAM TM strictly confidential 09.04.2020

 ***************************************************************************/
#include "event_parser.h"
#include "data/wlogdata.h"
#include <QDebug>
#include <QMessageBox>

//#define TAGS_POSITION 4 // Event line include 0) event_id, 1) signal_quality, 2) timestamp, 3) event_type, 4) tags


Event_Parser::Event_Parser()
{
    reset();
}

void Event_Parser::set_label_file_name(const QString &line) noexcept
{
    QStringList tmp_list = line.split("/");
    QString tmp = tmp_list.back().split("_").back();
    tmp_list = tmp.split(".");
    tmp_list.removeLast();
    m_label_name_version = tmp_list.empty() ? "v.?.?" : tmp_list.join(".");
}

bool Event_Parser::parse_line(const QString &line, W_Log_Data *result) noexcept
{
    if (line.indexOf("event_type") == 0)
    {
        parse_format_line(line);
    }
    else if (!m_event_tags.empty())
    {
        parse_event_line(line, result);
    }
    else
    {
        return false;
    }

    return true;
}

void Event_Parser::parse_format_line(const QString &line) noexcept
{
    m_event_tags = line.split(',');
    if (m_event_tags.size() < 6) // event_type,event_subtype,event_id,HHMMSS_to_video,HHMMSS_to_timeline,ms_timestamp_to_timeline
    {
        QMessageBox msgBox;
        msgBox.setText("Error parsing header in the label file: " + line) ;
        msgBox.exec();
        qDebug() << "Error parsing header in the label file:" << line;

        m_event_tags.clear();
        return;
    }

    QSet<QString> string_set;

    QString message;

    for (auto s : m_event_tags)
    {
        if (!s.isEmpty() && QString::compare(s, "-") != 0 && string_set.contains(s))
        {
            message.append(s + ", ");
        }
        else
        {
            string_set.insert(s);
        }
    }

    if (!message.isEmpty())
    {
        message.remove(message.size() - 2, 2); // remove the last ", ".
        QMessageBox msgBox;
        msgBox.setText("There are the same column names in header of label file: " + message) ;
        msgBox.exec();
    }

    //qDebug() << m_event_tags;
}

void Event_Parser::parse_event_line(const QString &line, W_Log_Data *result) const noexcept
{
    if (m_event_tags.isEmpty())
    {
        return;
    }

    QStringList label_list = line.split(',');

    if (label_list.size() < 6)
    {
        return;
    }

    Label_Item* label_item = new Label_Item;
    label_item->m_is_error = false;

    label_item->m_type_v = label_list[0].replace("*", "").replace("^", "") + "." + m_label_name_version;
//    label_item->m_version = ;

    auto iter_tag_list = m_event_tags.cbegin();
    auto iter_label_list = label_list.cbegin();

    while(iter_tag_list < m_event_tags.cend() && iter_label_list < label_list.cend())
    {
        if (!iter_label_list->isEmpty() && QString::compare(*iter_tag_list, "-") != 0)
        {
            if (iter_tag_list->indexOf("ms_timestamp_to_timeline") >= 0)
            {
                label_item->m_timestamp = static_cast<uint64_t>(iter_label_list->toULongLong());
            }
            else
            {
                label_item->m_tag_value_map[*iter_tag_list] = *iter_label_list;
                if (iter_tag_list->contains("player") || iter_tag_list->contains("ball"))
                {
                    result->get_log_labels()->add_label_by_object(*iter_label_list, label_item);
                }
            }
        }

        ++iter_tag_list;
        ++iter_label_list;
    }

    label_item->m_tag_value_map["criterion_version"] = m_label_name_version;


    result->get_log_labels()->add_label_item(label_item);
}

void Event_Parser::reset()
{
    m_event_tags.clear();
    m_label_name_version.clear();
}

Event_Parser::~Event_Parser()
{
    reset();
}
