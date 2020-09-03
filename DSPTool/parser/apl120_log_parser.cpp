#include "apl120_log_parser.h"

#include "data/wlogdata.h"
//#include "data/wobjectdata.h"
//#include "data/wsensordata.h"
//#include "data/wsignaldata.h"
#include <QStringList>
#include <QColor>
#include <QDebug>
#include "data/episode_meta_data.h"

Apl120_Log_Parser::Apl120_Log_Parser(const QString &file_name)
{
    m_time_start = 0;
    m_timeline_shift = 0;
    m_is_addition_log = false;
    m_file_name = file_name;


    //qDebug() << m_file_name;
    if (m_file_name.contains("Location120_23_"))
    {
        m_offset_x = 0;
        m_offset_y = -1200; // left+; right-
    }
    else if (m_file_name.contains("Location120_22_"))
    {
        m_offset_x = 10200;
        m_offset_y = -1110;
    }
    else
    {
        m_offset_x = 1920;
        m_offset_y = 40;
    }
}

void Apl120_Log_Parser::set_addition_log_key(bool key) noexcept
{
    m_is_addition_log = key;
}

bool Apl120_Log_Parser::parse(const QString &line, W_Log_Data *result)
{
    if (result == nullptr)
    {
        return false;
    }

    QStringList separated_line = line.split(",");

    if (separated_line.size() != 6)
    {
        return false;
    }

    uint64_t current_time = separated_line[0].toULongLong();


    if (m_is_first_value)
    {
        m_is_first_value = false;
        m_time_start = current_time;

        uint64_t xyz_start_timstamp = Episode_Meta_Data::get_instance().get_xyz_start_timestamp();

        if (xyz_start_timstamp > 0)
        {
            m_time_start = xyz_start_timstamp;
            m_timeline_shift = static_cast<int>(1.0 * current_time - m_time_start);
        }
        else
        {

        }
        result->set_log_min_time(m_time_start);
    }

    QString object_name = separated_line[2];

    if (Episode_Meta_Data::get_instance().is_meta_file_opened())
    {
        QVariant is_enable = Episode_Meta_Data::get_instance().get_value(object_name.toULong(), Meta_Parameter_Type::SENSOR_ENABLE);
        if (is_enable.isValid() && !is_enable.toBool())
        {
            // pass this object
            return false;
        }
    }

    W_Object_Data *object = result->get_object(object_name);
    if (object == nullptr)
    {
        object = new W_Object_Data(object_name);
        result->add_object(object);
    }

    QString sensor_flag = "position(mm)";
    if (m_is_addition_log)
    {
        sensor_flag.append(m_file_name.split("/").back());
    }

    W_Sensor_Data *sensor = object->get_sensor(sensor_flag);
    if (sensor == nullptr)
    {
        sensor = new W_Sensor_Data(sensor_flag);
        object->add_sensor(sensor);
    }

    XYZ_Sample xyz_sample;
    xyz_sample.m_object_id = object_name.toULong();
    xyz_sample.m_time = static_cast<uint64_t>(1. * current_time - m_timeline_shift);
    xyz_sample.m_x = separated_line[3].toDouble() + m_offset_x;
    xyz_sample.m_y = separated_line[4].toDouble() + m_offset_y;
    xyz_sample.m_z = separated_line[5].toDouble();

    //    if (current_time <= m_time_start)
    //    {
    //        qDebug() << xyz_sample.m_object_id << xyz_sample.m_time << current_time;
    //    }

    W_Signal_Data *x_signal = sensor->get_signal("x");
    if (x_signal == nullptr)
    {
        x_signal = new W_Signal_Data("x");
        sensor->add_signal(x_signal);
    }
    x_signal->add_sample(xyz_sample.m_time, xyz_sample.m_x);

    W_Signal_Data *y_signal = sensor->get_signal("y");
    if (y_signal == nullptr)
    {
        y_signal = new W_Signal_Data("y");
        sensor->add_signal(y_signal);
    }
    y_signal->add_sample(xyz_sample.m_time, xyz_sample.m_y);

    W_Signal_Data *z_signal = sensor->get_signal("z");
    if (z_signal == nullptr)
    {
        z_signal = new W_Signal_Data("z");
        sensor->add_signal(z_signal);
    }
    z_signal->add_sample(xyz_sample.m_time, xyz_sample.m_z);

    // set color
    if (m_is_addition_log)
    {
        if (object_name.length() > 5) // ball
        {
            sensor->set_color(QColor("#FF6B00").dark(150).name());
        }
        x_signal->set_color(QColor("red").dark(150).name());
        y_signal->set_color(QColor("green").dark(150).name());
        z_signal->set_color(QColor("blue").dark(150).name());
    }
    else
    {
        sensor->set_weight(3);
        if (object_name.length() > 5) // ball
        {
            sensor->set_color("#FF6B00");
        }
        x_signal->set_color("red");
        y_signal->set_color("green");
        z_signal->set_color("blue");
    }


    result->add_xyz_sample(xyz_sample);

    result->set_log_max_time(xyz_sample.m_time);

    return true;
}
