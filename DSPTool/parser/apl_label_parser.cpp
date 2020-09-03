#include "apl_label_parser.h"
#include "data/wlogdata.h"
#include <QStringList>
#include <QColor>
#include <QDebug>

Apl_Label_Parser::Apl_Label_Parser()
{
    m_time_start = 0;
}

void Apl_Label_Parser::set_addition_log_key(bool key) noexcept
{
    // do not using :( bad code
}

bool Apl_Label_Parser::parse(const QString &line, W_Log_Data *result)
{
    if (result == nullptr)
    {
        return false;
    }

    QStringList separated_line = line.split(",");

    if (separated_line.length() < 2)
    {
        return false;
    }

    if (m_is_first_value)
    {
        m_is_first_value = false;
        m_time_start = 0;//current_time;
        result->get_log_labels()->set_time_start(m_time_start);
        m_ball_possesion_map.clear();
        m_event_parser.reset();
    }

    // If new format, then we use new Event_Parser.
    // Otherwise, we continue using old parser.
    if (m_event_parser.parse_line(line, result))
    {
        return true;
    }

//    uint64_t current_time = separated_line.at(2).toULongLong();

//    QString label_type = separated_line.at(3);

//    if (label_type.compare("miss") == 0 || label_type.compare("make") == 0 || label_type.compare("make/miss") == 0)
//    {
//        Label_Item *label_item = new Label_Item;
//        label_item->m_is_error = false;
//        //        label_item->m_type = label_type;
//        label_item->m_type = "all shots";
//        label_item->m_timestamp = current_time;

//        label_item->m_tag_value_map["timestamp"] = separated_line.at(2).toULongLong();
//        label_item->m_tag_value_map["type"] = label_type;

//        label_item->m_tag_value_map["id"] = separated_line.at(0);
//        label_item->m_tag_value_map["signal"] = separated_line.at(1);

//        QString object_player = separated_line.at(4);
//        label_item->m_tag_value_map["player"] = object_player;

//        label_item->m_tag_value_map["timestamp2"] = separated_line.at(5).toLongLong();

//        label_item->m_tag_value_map["pos_x"] = separated_line.at(6).toInt();
//        label_item->m_tag_value_map["pos_y"] = separated_line.at(7).toInt();
//        label_item->m_tag_value_map["pos_z"] = separated_line.at(8).toInt();

//        QString object_ball = separated_line.at(9);
//        label_item->m_tag_value_map["ball"] = object_ball;

//        label_item->m_tag_value_map["hoop_id"] = separated_line.at(10);
//        label_item->m_tag_value_map["hoop_x"] = separated_line.at(11).toInt();
//        label_item->m_tag_value_map["hoop_y"] = separated_line.at(12).toInt();
//        label_item->m_tag_value_map["hoop_z"] = separated_line.at(13).toInt();

//        if (separated_line.size() > 15)
//        {
//            QString sub_shot = separated_line.at(15);
//            label_item->m_tag_value_map["sub"] = sub_shot;
//            if (sub_shot.compare("JS") == 0 || sub_shot.compare("JSN") == 0
//                    || sub_shot.compare("FT") == 0)
//            {
///*                if (separated_line.size() >= 27)
//                {
//                    label_item->m_tag_value_map["catch"] = separated_line.at(15).toInt();
//                    label_item->m_tag_value_map["dribble"] = separated_line.at(16).toInt();
//                    label_item->m_tag_value_map["start"] = separated_line.at(17).toInt();
//                    label_item->m_tag_value_map["start_xy_1"] = separated_line.at(18).toInt();
//                    label_item->m_tag_value_map["start_xy_2"] = separated_line.at(19).toInt();
//                    label_item->m_tag_value_map["dip"] = separated_line.at(20).toInt();
//                    label_item->m_tag_value_map["release"] = separated_line.at(21).toInt();
//                    label_item->m_tag_value_map["view"] = separated_line.at(22);

//                    label_item->m_tag_value_map["shift"] = separated_line.at(24);
//                    label_item->m_tag_value_map["shift_range"] = separated_line.at(25);
//                    if (!separated_line.at(26).isEmpty())
//                    {
//                        label_item->m_tag_value_map["start_dip_?"] = separated_line.at(26);
//                    }
//                }
//                else*/ if (separated_line.size() > 20)
//                {
//                    label_item->m_tag_value_map["catch"] = separated_line.at(17);
//                    label_item->m_tag_value_map["dribble"] = separated_line.at(18);
//                    label_item->m_tag_value_map["start"] = separated_line.at(19);
//                    label_item->m_tag_value_map["dip"] = separated_line.at(20);
//                    label_item->m_tag_value_map["release"] = separated_line.at(21);
//                    label_item->m_tag_value_map["view"] = separated_line.at(22);

//                    if (separated_line.size() > 24)
//                    {
//                        label_item->m_tag_value_map["shift"] = separated_line.at(24);
//                    }
//                    if (separated_line.size() > 25)
//                    {
//                        label_item->m_tag_value_map["shift_range"] = separated_line.at(25);
//                    }
//                }
//            }
//        }

//        result->get_log_labels()->add_label_item(label_item);
//        result->get_log_labels()->add_label_by_object(object_player, label_item);
//        result->get_log_labels()->add_label_by_object(object_ball, label_item);
//    }
//    else if (label_type.compare("ST") == 0)
//    {
//        Label_Item *label_item = new Label_Item;
//        label_item->m_is_error = false;
//        label_item->m_type = label_type;
//        label_item->m_timestamp = static_cast<unsigned long>(current_time - m_time_start);

//        label_item->m_tag_value_map["timestamp"] = current_time;
//        label_item->m_tag_value_map["type"] = label_type;
//        label_item->m_tag_value_map["object"] = separated_line.at(4);
//        label_item->m_tag_value_map["player_id"] = separated_line.at(5);
//        label_item->m_tag_value_map["shot_type"] = separated_line.at(6);
//        label_item->m_tag_value_map["bank"] = separated_line.at(7);
//        label_item->m_tag_value_map["view"] = separated_line.at(8);
//        label_item->m_tag_value_map["shift"] = separated_line.at(9);
//        label_item->m_tag_value_map["shift_range"] = separated_line.at(10);

//        result->get_log_labels()->add_label_item(label_item);
//        result->get_log_labels()->add_label_by_object(separated_line.at(4), label_item);
//    }
//    else if (label_type.compare("FB") == 0)
//    {
//        Label_Item *label_item = new Label_Item;
//        label_item->m_is_error = false;
//        label_item->m_type = label_type;
//        label_item->m_timestamp = static_cast<unsigned long>(current_time - m_time_start);

//        label_item->m_tag_value_map["timestamp"] = current_time;
//        label_item->m_tag_value_map["type"] = label_type;
//        label_item->m_tag_value_map["category"] = separated_line.at(4);
//        label_item->m_tag_value_map["object"] = separated_line.at(5);
//        label_item->m_tag_value_map["player_id"] = separated_line.at(6);
//        label_item->m_tag_value_map["view"] = separated_line.at(7);
//        label_item->m_tag_value_map["shift"] = separated_line.at(8);
//        label_item->m_tag_value_map["shift_range"] = separated_line.at(9);

//        result->get_log_labels()->add_label_item(label_item);
//        result->get_log_labels()->add_label_by_object(separated_line.at(5), label_item);
//    }
//    else if (label_type.compare("gotpossession") == 0 || label_type.compare("lostpossession") == 0
//             || label_type.compare("shortpossession") == 0)
//    {
//        Label_Item *label_item = new Label_Item;
//        label_item->m_is_error = false;

//        QString object_player = separated_line.at(2);

//        bool is_got = (label_type.compare("gotpossession") == 0);
//        bool is_lost = (label_type.compare("lostpossession") == 0);

//        if (m_ball_possesion_map.find(object_player) == m_ball_possesion_map.end())
//        {
//            m_ball_possesion_map[object_player] = is_got;
//        }

//        if (is_lost)
//        {
//            if (!m_ball_possesion_map[object_player])
//            {
//                label_type = "shortpossession";
//            }
//            else
//            {
//                m_ball_possesion_map[object_player] = false;
//            }
//        }
//        else
//        {
//            m_ball_possesion_map[object_player] = is_got;
//        }

//        label_item->m_type = label_type;
//        label_item->m_timestamp = static_cast<unsigned long>(current_time - m_time_start);

//        label_item->m_tag_value_map["timestamp"] = current_time;
//        label_item->m_tag_value_map["type"] = label_type;

//        label_item->m_tag_value_map["player"] = object_player;

//        label_item->m_tag_value_map["pos_x"] = separated_line.at(3).toInt();
//        label_item->m_tag_value_map["pos_y"] = separated_line.at(4).toInt();
//        label_item->m_tag_value_map["pos_z"] = separated_line.at(5).toInt();

//        QString object_ball = separated_line.at(6);
//        label_item->m_tag_value_map["ball"] = object_ball;

//        if (!is_got)
//        {
//            label_item->m_tag_value_map["dribbles"] = separated_line.at(7).toInt();
//        }

//        if (label_item->m_timestamp != 0)
//        {
//            result->get_log_labels()->add_label_item(label_item);
//            result->get_log_labels()->add_label_by_object(object_player, label_item);
//            result->get_log_labels()->add_label_by_object(object_ball, label_item);
//        }
//    }
//    else if (label_type.compare("RJ") == 0)
//    {
//        Label_Item *label_item = new Label_Item;
//        label_item->m_is_error = false;
//        label_item->m_type = label_type;
//        label_item->m_timestamp = static_cast<unsigned long>(current_time - m_time_start);

//        label_item->m_tag_value_map["timestamp"] = current_time;
//        label_item->m_tag_value_map["type"] = label_type;
//        label_item->m_tag_value_map["object"] = separated_line.at(2);

//        label_item->m_tag_value_map["comment"] = separated_line.at(3);

//        result->get_log_labels()->add_label_item(label_item);
//        result->get_log_labels()->add_label_by_object(separated_line.at(2), label_item);
//    }
//    else if (label_type.compare("accel") == 0)
//    {
//        Label_Item *label_item = new Label_Item;
//        label_item->m_is_error = false;
//        label_item->m_type = label_type;
//        label_item->m_timestamp = static_cast<unsigned long>(current_time - m_time_start);

//        label_item->m_tag_value_map["timestamp"] = current_time;
//        label_item->m_tag_value_map["type"] = label_type;
//        QString object_player = separated_line.at(2);
//        label_item->m_tag_value_map["player"] = object_player;

//        label_item->m_tag_value_map["a-d"] = separated_line.at(3) ;

//        result->get_log_labels()->add_label_item(label_item);
//        result->get_log_labels()->add_label_by_object(object_player, label_item);
//    }
//    else if (label_type.compare("VJ") == 0 || label_type.compare("VJ*") == 0)
//    {
//        Label_Item *label_item = new Label_Item;
//        label_item->m_is_error = false;
//        label_item->m_type = "VJ";
//        label_item->m_timestamp = static_cast<unsigned long>(current_time - m_time_start);

//        label_item->m_tag_value_map["timestamp"] = current_time;
//        label_item->m_tag_value_map["type"] = label_type;
//        QString object_player = separated_line.at(2);
//        label_item->m_tag_value_map["player"] = object_player;

//        label_item->m_tag_value_map["type"] = separated_line.at(3);

//        if (label_type.compare("VJ*") == 0)
//        {
//            label_item->m_tag_value_map["*"] = "*";
//        }

//        if (separated_line.size() > 3)
//        {
//            label_item->m_tag_value_map["height"] = separated_line.at(4);
//            label_item->m_tag_value_map["begin"] = separated_line.at(5);
//            label_item->m_tag_value_map["end"] = separated_line.at(6);
//            label_item->m_tag_value_map["hang_time"] = separated_line.at(7);

//            if (label_item->m_tag_value_map["hang_time"].toInt() < 250 /*|| label_item->m_tag_value_map["hang_time"].toInt() >= 350*/)
//            {
//                label_item->m_tag_value_map["type"] = "low";
//            }
//            else if (label_item->m_tag_value_map["type"] == "low")
//            {
//                label_item->m_tag_value_map["type"] = "medium";
//            }

//            label_item->m_tag_value_map["shift"] = separated_line.at(8);
//            label_item->m_tag_value_map["range"] = separated_line.at(9);
//        }

//        result->get_log_labels()->add_label_item(label_item);
//        result->get_log_labels()->add_label_by_object(object_player, label_item);
//    }
//    else if (label_type.compare("activity") == 0)
//    {
//        Label_Item *label_item = new Label_Item;
//        label_item->m_is_error = false;
//        label_item->m_type = label_type;
//        label_item->m_timestamp = static_cast<unsigned long>(current_time - m_time_start);

//        label_item->m_tag_value_map["timestamp"] = current_time;
//        label_item->m_tag_value_map["type"] = label_type;
//        QString object_player = separated_line.at(2);
//        label_item->m_tag_value_map["player"] = object_player;

//        label_item->m_tag_value_map["sub"] = separated_line.at(3);

//        qlonglong time_begin_activity = separated_line.at(4).toLongLong();
//        qlonglong time_end_activity = separated_line.at(5).toLongLong();

//        label_item->m_tag_value_map["begin"] = (time_begin_activity - m_time_start);
//        label_item->m_tag_value_map["end"] = (time_end_activity - m_time_start);
//        label_item->m_tag_value_map["duration"] = ((time_end_activity - m_time_start) - (time_begin_activity - m_time_start));

//        result->get_log_labels()->add_label_item(label_item);
//        result->get_log_labels()->add_label_by_object(object_player, label_item);
//    }
//    else if (label_type.contains("dribble"))
//    {
//        Label_Item *label_item = new Label_Item;
//        label_item->m_is_error = false;
//        label_item->m_type = "dribble";
//        label_item->m_timestamp = static_cast<unsigned long>(current_time - m_time_start);

//        QString object_ball = separated_line.at(2);
//        label_item->m_tag_value_map["ball"] = object_ball;

//        label_item->m_tag_value_map["timestamp"] = current_time;

//        if (separated_line.size() > 6)
//        {
//            label_item->m_tag_value_map["player_id"] = separated_line.at(3);
//            label_item->m_tag_value_map["view"] = separated_line.at(4);
//            label_item->m_tag_value_map["shift"] = separated_line.at(5);
//            label_item->m_tag_value_map["range"] = separated_line.at(6);
//        }

//        result->get_log_labels()->add_label_item(label_item);
//        result->get_log_labels()->add_label_by_object(object_ball, label_item);

//    }
//    else if (label_type.contains("floor_bounce") || label_type.contains("uncer"))
//    {
//        Label_Item *label_item = new Label_Item;
//        label_item->m_is_error = false;
//        label_item->m_type = label_type;
//        label_item->m_timestamp = static_cast<unsigned long>(current_time - m_time_start);

//        QString object_ball = separated_line.at(2);
//        label_item->m_tag_value_map["ball"] = object_ball;

//        label_item->m_tag_value_map["timestamp"] = current_time;

//        if (separated_line.size() > 3)
//        {
//            label_item->m_tag_value_map["view"] = separated_line.at(4);
//            label_item->m_tag_value_map["shift"] = separated_line.at(5);
//            label_item->m_tag_value_map["range"] = separated_line.at(6);
//        }

//        result->get_log_labels()->add_label_item(label_item);
//        result->get_log_labels()->add_label_by_object(object_ball, label_item);

//    }
//    else if ((label_type.compare("noballvisible") == 0 || label_type.compare("noplayervisible") == 0)
//             && separated_line.size() > 4)
//    {
//        Label_Item *label_item = new Label_Item;
//        label_item->m_is_error = false;
//        label_item->m_type = label_type;
//        label_item->m_timestamp = static_cast<unsigned long>(current_time - m_time_start);

//        label_item->m_tag_value_map["timestamp"] = current_time;

//        QString object = separated_line.at(2);
//        if (label_type.contains("ball"))
//        {
//            label_item->m_tag_value_map["ball"] = object;
//        }
//        else
//        {
//            label_item->m_tag_value_map["player"] = object;
//        }

//        label_item->m_tag_value_map["end"] = QString::number(separated_line.at(3).toLong() - m_time_start);

//        result->get_log_labels()->add_label_item(label_item);
//        result->get_log_labels()->add_label_by_object(object, label_item);
//    }
//    else if (line.compare(",l,") == 0 || line.compare(",r,") == 0 || line.compare(",b,") == 0)
//    {
//        Label_Item *label_item = new Label_Item;
//        label_item->m_is_error = false;
//        label_item->m_type = label_type;
//        label_item->m_timestamp = static_cast<unsigned long>(current_time - m_time_start);

//        label_item->m_tag_value_map["timestamp"] = current_time;
//        label_item->m_tag_value_map["type"] = label_type;
//        label_item->m_tag_value_map["leg"] = label_type;
//        label_item->m_tag_value_map["take_off"] = separated_line.at(2).toLongLong();
//        label_item->m_tag_value_map["height"] = separated_line.at(3).toInt();
//        label_item->m_tag_value_map["landing_left"] = separated_line.at(4).toLongLong();
//        label_item->m_tag_value_map["landing_right"] = separated_line.at(5).toLongLong();
//        label_item->m_tag_value_map["flying_time"] = separated_line.at(6).toInt();
//        label_item->m_tag_value_map["object"] = separated_line.at(7);

//        result->get_log_labels()->add_label_item(label_item);
//        result->get_log_labels()->add_label_by_object(separated_line.at(2), label_item);

//    }
//    else if (line.contains(",left,") || line.contains(",right,"))
//    {
//        QString label_type = separated_line.at(1);

//        Label_Item *label_item = new Label_Item;
//        label_item->m_is_error = false;
//        label_item->m_type = label_type;
//        label_item->m_timestamp = static_cast<unsigned long>(current_time - m_time_start);

//        label_item->m_tag_value_map["timestamp"] = current_time;
//        label_item->m_tag_value_map["type"] = label_type;
//        label_item->m_tag_value_map["tap"] = separated_line.at(2);
//        label_item->m_tag_value_map["quality"] = separated_line.at(3);

//        result->get_log_labels()->add_label_item(label_item);
//        result->get_log_labels()->add_label_by_object(label_type, label_item);
//    }
//    return true;
    //    }
    //    else if (line.contains(";"))
    //    {
    //        QStringList separated_line = line.split(";");

    //        qlonglong current_time = separated_line.at(0).toLongLong();

    //        if (m_is_first_value)
    //        {
    //            m_is_first_value = false;
    //            m_time_start = 0;
    //            result->get_log_labels()->set_time_start(0);
    //            m_ball_possesion_map.clear();
    //        }

    //        if (separated_line.size() == 4)
    //        {
    //            QString label_type = separated_line.at(1);

    //            Label_Item *label_item = new Label_Item;
    //            label_item->m_is_error = false;
    //            label_item->m_type = label_type;
    //            label_item->m_timestamp = (unsigned long) (current_time - m_time_start);

    //            label_item->m_tag_value_map["timestamp"] = current_time;
    //            label_item->m_tag_value_map["type"] = label_type;
    //            label_item->m_tag_value_map["tap"] = separated_line.at(2);
    //            label_item->m_tag_value_map["quality"] = separated_line.at(3);

    //            result->get_log_labels()->add_label_item(label_item);
    //            result->get_log_labels()->add_label_by_object(label_type, label_item);
    //        }

    //        return true;
    //    }
    //    return false;
    return false;
}
