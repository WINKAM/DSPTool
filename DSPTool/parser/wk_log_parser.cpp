#include "wk_log_parser.h"

#include "data/wlogdata.h"
#include <QStringList>
#include <QColor>
#include <QDebug>

WK_Log_Parser::WK_Log_Parser(const QString &file_name)
{
    m_time_start = -1;
    m_is_addition_log = false;
    m_file_name = file_name;
}

void WK_Log_Parser::set_addition_log_key(bool key) noexcept
{
    m_is_addition_log = key;
}

bool WK_Log_Parser::parse(const QString &line, W_Log_Data *result)
{
    if (result == nullptr || line.contains("#") || line.contains("name"))
    {
        return false;
    }

    QStringList separated_line = line.split(";");

    if (separated_line.size() != 5)
    {
        return false;
    }

    qlonglong current_time = separated_line.at(4).toLongLong();

    if (m_is_first_value)
    {
        m_is_first_value = false;
        m_time_start = current_time;
    }

    QString object_name = m_file_name.right(m_file_name.length() - m_file_name.lastIndexOf("/") - 1).replace(".csv", "");
    W_Object_Data *object = result->get_object(object_name);
    if (object == nullptr)
    {
        object = new W_Object_Data(object_name);
        result->add_object(object);
    }

    QString sensor_flag = separated_line.at(0);

    if (sensor_flag.compare("0") == 0)
    {
        sensor_flag = "A";
    }
    else if (sensor_flag.compare("1") == 0)
    {
        sensor_flag = "G";
    }

    W_Sensor_Data *sensor = object->get_sensor(sensor_flag);
    if (sensor == nullptr)
    {
        sensor = new W_Sensor_Data(sensor_flag);
        object->add_sensor(sensor);
    }    

    XYZ_Sample xyz_sample;
    xyz_sample.m_object_id = sensor_flag.compare("A") == 0 ? 0 : sensor_flag.compare("G") == 0 ? 1 : 2;
//    xyz_sample.m_object_id = sensor_flag.compare("A") == 0 ? 0 : sensor_flag.compare("G") == 0 ? 1 : sensor_flag.compare("A2") == 0 ? 2 : 3;
    xyz_sample.m_time = (long) (current_time - m_time_start);
    xyz_sample.m_x = separated_line.at(1).toDouble() * 1000;
    xyz_sample.m_y = separated_line.at(2).toDouble() * 1000;
    xyz_sample.m_z = separated_line.at(3).toDouble() * 1000;

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
    if (sensor_flag.compare("G"))
    {
        x_signal->set_color(QColor("red").dark(150).name());
        y_signal->set_color(QColor("green").dark(150).name());
        z_signal->set_color(QColor("blue").dark(150).name());
    }
    else
    {
        sensor->set_weight(3);
        x_signal->set_color("red");
        y_signal->set_color("green");
        z_signal->set_color("blue");
    }


    result->add_xyz_sample(xyz_sample);

    result->set_log_max_time(xyz_sample.m_time);   

    return true;
}
