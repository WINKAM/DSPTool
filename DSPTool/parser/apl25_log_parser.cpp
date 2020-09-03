#include "apl25_log_parser.h"
#include "data/wlogdata.h"
#include <QStringList>
#include <QColor>
#include <QDebug>

Apl25_Log_Parser::Apl25_Log_Parser(const QString &file_name)// : m_file_name(file_name)
{
    m_time_start = 0;
    m_is_addition_log = false;
    m_file_name = file_name;
}

void Apl25_Log_Parser::set_addition_log_key(bool key) noexcept
{
    m_is_addition_log = key;
}

bool Apl25_Log_Parser::parse(const QString &line, W_Log_Data *result)
{
    if (result == nullptr)
    {
        return false;
    }

    QStringList separated_line = line.split(",");

    uint64_t current_time = separated_line.at(0).toULongLong();

    if (m_is_first_value)
    {
        m_is_first_value = false;
        m_time_start = current_time;
    }

    QString object_type = separated_line.at(1);
    if (object_type.compare("player") == 0 || object_type.compare("ball") == 0)
    {
        QString object_name = /*object_type + */separated_line.at(2);
        W_Object_Data *object = result->get_object(object_name);
        if (object == nullptr)
        {
            object = new W_Object_Data(object_name);
            result->add_object(object);
        }

        QString sensor_flag = "position";
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

        W_Signal_Data *x_signal = sensor->get_signal("x");
        if (x_signal == nullptr)
        {
            x_signal = new W_Signal_Data("x");
            sensor->add_signal(x_signal);
        }
        x_signal->add_sample(current_time, separated_line.at(3).toInt() - OFFSET_X);

        W_Signal_Data *y_signal = sensor->get_signal("y");
        if (y_signal == nullptr)
        {
            y_signal = new W_Signal_Data("y");
            sensor->add_signal(y_signal);
        }
        y_signal->add_sample(current_time, separated_line.at(4).toInt() - OFFSET_Y);

        W_Signal_Data *z_signal = sensor->get_signal("z");
        if (z_signal == nullptr)
        {
            z_signal = new W_Signal_Data("z");
            sensor->add_signal(z_signal);
        }
        z_signal->add_sample(current_time, separated_line.at(5).toInt());

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

        result->set_log_max_time(current_time);
    }

    return true;
}
