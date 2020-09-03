#include "apl_error_parser.h"
#include "data/wlogdata.h"
#include <QStringList>
#include <QColor>
#include <QDebug>

Apl_Error_Parser::Apl_Error_Parser()
{
    m_time_start = 0;
}

void Apl_Error_Parser::set_addition_log_key(bool key) noexcept
{
    // do not using :( bad code
}

bool Apl_Error_Parser::parse(const QString &line, W_Log_Data *result)
{
    if (result == nullptr)
    {
        return false;
    }

    QStringList separated_line = line.split(",");

    QString label_type = separated_line.at(0);
    if (label_type.compare("Err.Type") == 0)
    {
        return true;
    }

    uint64_t current_time = separated_line.at(1).toULongLong();

    if (m_is_first_value)
    {
        m_is_first_value = false;
        m_time_start = current_time;
    }    

    Label_Item *label_item = new Label_Item;
    label_item->m_is_error = true;
    label_item->m_type_v = label_type;
    label_item->m_timestamp = current_time;

    label_item->m_tag_value_map["timestamp"] = separated_line.at(1).toULongLong();
    label_item->m_tag_value_map["type"] = label_type;

    label_item->m_tag_value_map["Lbl"] = separated_line.at(3);
    label_item->m_tag_value_map["Event"] = separated_line.at(4);

    QString object_player = separated_line.at(12);
    label_item->m_tag_value_map["player"] = object_player;

    QString object_ball = separated_line.at(5);
    label_item->m_tag_value_map["ball"] = object_ball;

    result->get_log_labels()->add_error_item(label_item);
    result->get_log_labels()->add_error_by_object(object_player, label_item);
    result->get_log_labels()->add_error_by_object(object_ball, label_item);

    return true;
}
