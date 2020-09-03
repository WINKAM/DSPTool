/***************************************************************************

             WINKAM TM strictly confidential 03.06.2020

 ***************************************************************************/
#include "episode_meta_data.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QDir>

Episode_Meta_Data::Episode_Meta_Data()
{
    clear();
}

void Episode_Meta_Data::find_read_meta_file(const QString& episode_name) noexcept
{
    QStringList tmp = episode_name.split("/");
    if (tmp.size() > 1)
    {
        tmp[tmp.size() - 2] = "labels";
    }

    QString episode_id = tmp.back().split("_").front();
    if (episode_id == "temp" && tmp.back().split("_").size() > 1)
    {
        episode_id = tmp.back().split("_")[1];
    }

    tmp.removeLast();
    QDir directory(tmp.join("/"));

    QStringList meta_files = directory.entryList(QStringList() << (episode_id + "_*_meta.csv"),QDir::Files);

    if (!meta_files.empty())
    {
        read_meta_file(directory.path() + "/" + meta_files.front());
    }
    else
    {
        clear();
        qDebug() << "Warning: meta-file was not found in " << directory.path();
    }
}


void Episode_Meta_Data::read_meta_file(const QString& file_name) noexcept
{
    clear();

    QFile input_file(file_name);

    if (input_file.open(QIODevice::ReadOnly))
    {
        qDebug() << "read meta-file" << file_name;

        QTextStream input_stream(&input_file);
        QString line;

        m_is_meta_file_opened = true;

        while(!input_stream.atEnd())
        {
            line = input_stream.readLine();

            if (line.indexOf("Session_timeline") == 0)
            {
                QStringList tokens = line.split(",");
                if (tokens.size() > 2)
                {
                    m_start_timestamp = tokens[2].toULongLong();

                    if (tokens.size() > 6)
                    {
                        QStringList tmp = tokens[6].split(':');
                        if (tmp.size() > 1)
                        {
                            m_start_hh_mm_in_ms = 1000 * ((tmp[0].toInt() * 60 + tmp[1].toInt()) * 60);
                            if (tmp.size() > 2)
                            {
                                m_start_hh_mm_in_ms += static_cast<uint64_t>(1000 * tmp[2].toDouble());
                            }
                        }
                    }
                }
            }

            if (line.indexOf("XYZ_timeline") == 0)
            {
                QStringList tokens = line.split(",");
                if (tokens.size() > 4)
                {
                    m_xyz_start_timestamp = tokens[4].toULongLong();
                }
            }

            if (line.indexOf("Team,Team_name,") == 0)
            {
                QStringList tokens = line.split(",");
                if (tokens.size() > 2)
                {
                    unsigned long team_id = m_team_names.size(); // new team
                    set_value(team_id, Meta_Parameter_Type::TEAM_NAME, tokens[2].simplified());
                    m_team_ids[tokens[2].simplified().toStdString()] = team_id;

                    if (tokens.size() > 6)
                    {
                        set_value(team_id, Meta_Parameter_Type::TEAM_COLOR, tokens[6].simplified());
                    }
                }
            }

            if (line.indexOf("Video_timeline") == 0)
            {
                QStringList tokens = line.split(",");
                if (tokens.size() > 4)
                {
                    m_video_start_timestamp[tokens[2].simplified().toStdString()] = tokens[4].toULongLong();
                }
            }

            if(line.indexOf("Video_specs,File_name,") == 0)
            {
                QStringList tokens = line.split(",");
                if (tokens.size() > 18)
                {
                    QString video_spec = tokens[6] + " FPS; " + tokens[7] + "; " + tokens[14] + "; " + tokens[15] + ": " + tokens[16]
                            + "; " + tokens[17] + ": " + tokens[18];

                    m_video_specs[tokens[2].toStdString()] = video_spec;
                }

            }

            if (line.indexOf("Player,Sensor_id,") == 0)
            {
                QStringList tokens = line.split(",");
                if (tokens.size() > 3)
                {
                    unsigned long player_id = tokens[2].simplified().toULong();
                    bool is_enable = (tokens[4].simplified().toLower().compare("show") == 0);
                    set_value(player_id, Meta_Parameter_Type::SENSOR_ENABLE, is_enable);
                    set_value(player_id, Meta_Parameter_Type::OBJECT_TYPE, static_cast<int>(Object_Type::PLAYER));

                    if (tokens.size() > 6)
                    {
                        if (m_team_ids.find(tokens[6].simplified().toLower().toStdString()) != m_team_ids.end())
                        {
                            unsigned long team_id = m_team_ids.at(tokens[6].simplified().toLower().toStdString());
                            set_value(player_id, Meta_Parameter_Type::PLAYER_TEAM_ID, static_cast<int>(team_id));
                        }

                        if (tokens.size() > 8)
                        {
                            set_value(player_id, Meta_Parameter_Type::OBJECT_VIEW_ID, tokens[8].simplified().toLower());
                        }
                    }
                }
            }

            if (line.indexOf("Ball,Sensor_id,") == 0)
            {
                QStringList tokens = line.split(",");
                if (tokens.size() > 3)
                {
                    unsigned long ball_id = tokens[2].simplified().toULong();
                    bool is_enable = (tokens[4].simplified().toLower().compare("show") == 0);
                    set_value(ball_id, Meta_Parameter_Type::SENSOR_ENABLE, is_enable);
                    set_value(ball_id, Meta_Parameter_Type::OBJECT_TYPE, static_cast<int>(Object_Type::BALL));

                    if (tokens.size() > 6)
                    {
                        set_value(ball_id, Meta_Parameter_Type::OBJECT_VIEW_ID, tokens[6].simplified().toLower());
                    }
                }
            }
        }
    }
    else
    {
        qDebug() << "cannot read meta-file";
    }

//    for (const auto& e : m_team_ids)
//    {
//        qDebug() << QString::fromStdString(e.first) << e.second;
//    }
//    qDebug() << "";
//    for (const auto& e : m_team_names)
//    {
//        qDebug() << e.first << e.second;
//    }
//    qDebug() << "";
//    for (const auto& e : m_team_colors)
//    {
//        qDebug() << e.first << e.second;
//    }
//    qDebug() << "";
//    for (const auto& e : m_object_view_ids)
//    {
//        qDebug() << e.first << e.second;
//    }
//    qDebug() << "";
//    for (const auto& e : m_object_types)
//    {
//        qDebug() << e.first << static_cast<int>(e.second);
//    }
//    qDebug() << "";
//    for (const auto& e : m_player_team_ids)
//    {
//        qDebug() << e.first << (e.second);
//    }
//    qDebug() << "";
//    for (const auto& e : m_sensor_enables)
//    {
//        qDebug() << e.first << (e.second);
//    }
}

QVariant Episode_Meta_Data::get_value(unsigned long key, Meta_Parameter_Type type) const noexcept
{
    switch (type)
    {
    case Meta_Parameter_Type::OBJECT_VIEW_ID:
        if (m_object_view_ids.find(key) != m_object_view_ids.end())
        {
            return QVariant(m_object_view_ids.at(key));
        }
        break;

    case Meta_Parameter_Type::OBJECT_TYPE:
        if (m_object_types.find(key) != m_object_types.end())
        {
            return QVariant(static_cast<int>(m_object_types.at(key)));
        }
        break;

    case Meta_Parameter_Type::SENSOR_ENABLE:
        if (m_sensor_enables.find(key) != m_sensor_enables.end())
        {
            return QVariant(m_sensor_enables.at(key));
        }
        break;

    case Meta_Parameter_Type::PLAYER_TEAM_ID:
        if (m_player_team_ids.find(key) != m_player_team_ids.end())
        {
            return QVariant(static_cast<int>(m_player_team_ids.at(key)));
        }
        break;

    case Meta_Parameter_Type::PLAYER_TEAM_NAME:
        if (m_player_team_ids.find(key) != m_player_team_ids.end())
        {
            unsigned long team_id = m_player_team_ids.at(key);
            if (m_team_names.find(team_id) != m_team_names.end())
            {
                return QVariant(m_team_names.at(team_id));
            }
        }
        break;

    case Meta_Parameter_Type::TEAM_NAME:
        if (m_team_names.find(key) != m_team_names.end())
        {
            return QVariant(m_team_names.at(key));
        }
        break;

    case Meta_Parameter_Type::TEAM_COLOR:
        if (m_team_colors.find(key) != m_team_colors.end())
        {
            return QVariant(m_team_colors.at(key));
        }
        break;
    }

    return QVariant();
}

QVariant Episode_Meta_Data::get_value(const QString& key, Meta_Parameter_Type type) const noexcept
{
    switch (type)
    {
    case Meta_Parameter_Type::TEAM_COLOR:
        if (m_team_ids.find(key.toStdString()) != m_team_ids.end())
        {
            unsigned long team_id = m_team_ids.at(key.toStdString());
            if (m_team_colors.find(team_id) != m_team_colors.end())
            {
                return m_team_colors.at(team_id);
            }
        }
        break;
    case Meta_Parameter_Type::VIDEO_SPEC:
        if (m_video_specs.find(key.toStdString()) != m_video_specs.end())
        {
            return m_video_specs.at(key.toStdString());
        }
    default:
        break;
    }

    return QVariant();
}

std::vector<QVariant> Episode_Meta_Data::get_values(Meta_Parameter_Type type) const noexcept
{
    std::vector<QVariant> result;
    switch (type)
    {
    case Meta_Parameter_Type::TEAM_IDS:
        result.reserve(m_team_ids.size());
        for (const auto& e : m_team_ids)
        {
            result.emplace_back(QVariant(static_cast<int>(e.second)));
        }
        break;

    default:
        break;
    }

    return result;
}

void Episode_Meta_Data::set_value(unsigned long key, Meta_Parameter_Type type, const QVariant& value)
{
    switch (type)
    {
    case Meta_Parameter_Type::OBJECT_VIEW_ID:
        m_object_view_ids[key] = value.toString();
        break;

    case Meta_Parameter_Type::OBJECT_TYPE:
        m_object_types[key] = static_cast<Object_Type>(value.toInt());
        break;

    case Meta_Parameter_Type::SENSOR_ENABLE:
        m_sensor_enables[key] = value.toBool();
        break;

    case Meta_Parameter_Type::PLAYER_TEAM_ID:
        m_player_team_ids[key] = value.toULongLong();
        break;

    case Meta_Parameter_Type::TEAM_NAME:
        m_team_names[key] = value.toString();
        break;

    case Meta_Parameter_Type::TEAM_COLOR:
        m_team_colors[key] = value.toString();
        break;
    }
}

void Episode_Meta_Data::set_start_timestamp(uint64_t timestamp) noexcept
{
    m_start_timestamp = timestamp;
}

uint64_t Episode_Meta_Data::get_start_timestamp() const noexcept
{
    return m_start_timestamp;
}

int Episode_Meta_Data::get_start_time_hh_mm_in_ms() const noexcept
{
    return m_start_hh_mm_in_ms;
}

uint64_t Episode_Meta_Data::get_start_shift() const noexcept
{
    return m_start_timestamp - m_start_hh_mm_in_ms;
}

uint64_t Episode_Meta_Data::get_video_start_time(const QString& video_name) const noexcept
{
    auto iter = m_video_start_timestamp.find(video_name.simplified().toStdString());
    if (iter != m_video_start_timestamp.end())
    {
        return iter->second;
    }

    return get_start_timestamp();
}

uint64_t Episode_Meta_Data::get_xyz_start_timestamp() const noexcept
{
    return m_xyz_start_timestamp;
}

bool Episode_Meta_Data::is_meta_file_opened() const noexcept
{
    return m_is_meta_file_opened;
}

void Episode_Meta_Data::clear() noexcept
{
    m_is_meta_file_opened = false;
    m_object_view_ids.clear();
    m_object_types.clear();
    m_sensor_enables.clear();
    m_player_team_ids.clear();
    m_team_names.clear();
    m_team_colors.clear();
    m_team_ids.clear();
}
