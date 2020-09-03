/***************************************************************************

             WINKAM TM strictly confidential 17.06.2019

 ***************************************************************************/
#include "alps_log_parser.h"

#include "data/wlogdata.h"
#include <QStringList>
#include <QColor>
#include <QDebug>

Alps_Log_Parser::Alps_Log_Parser(const QString &file_name)
{
    m_time_start = -1;
    m_is_addition_log = false;
    m_file_name = file_name;
}

void Alps_Log_Parser::set_addition_log_key(bool key) noexcept
{
    m_is_addition_log = key;
}

bool Alps_Log_Parser::parse(const QString &line, W_Log_Data *result)
{
    if (result == nullptr || line.contains("#") || line.contains("name"))
    {
        if (line.contains("#LP_REF") || line.contains("#LP_"))
        {
           QStringList separated_line = line.split("_");
           m_time_start = separated_line.back().toLongLong();
        }

        return false;
    }

    QStringList separated_line = line.split(",");

    if (separated_line.size() == 3 || separated_line.size() == 4)
    {
        //reference signal

        qlonglong current_time = separated_line.at(1).toLongLong();

//        if (m_is_addition_log)
//        {
//            sensor_flag.append(m_file_name.split("/").back());
//        }
        QString object_name = m_file_name.split("/").back();

        W_Object_Data *object = result->get_object(object_name);
        if (object == nullptr)
        {
            object = new W_Object_Data(object_name);
            result->add_object(object);
        }

        W_Sensor_Data *sensor = object->get_sensor("LP");

        if (sensor == nullptr)
        {
            sensor = new W_Sensor_Data("LP");
            object->add_sensor(sensor);
            sensor->set_weight(3);
            sensor->set_color("#FF0000");
        }

         W_Signal_Data *signal_pulse = sensor->get_signal("pulse");
         if (signal_pulse == nullptr)
         {
             signal_pulse = new W_Signal_Data("pulse");
             sensor->add_signal(signal_pulse);
             signal_pulse->set_color("#444444");
         }
         signal_pulse->add_sample(current_time - m_time_start, separated_line.at(2).toDouble() * 100);
         result->set_log_max_time(current_time - m_time_start);

         //qDebug() << current_time - m_time_start << current_time << m_time_start;

         if (separated_line.size() > 3 && !separated_line.at(3).isEmpty())
         {
             if (m_is_first_value)
             {
                m_qrs_prev_timestamp = current_time - m_time_start;
                m_is_first_value = false;
             }

             QStringList rr_intervals = separated_line.at(3).split('-');

             for (const QString& rri : separated_line.at(3).split('-'))
             {
                 qlonglong qrs_timestamp = rri.toLongLong();

                 Label_Item* label_item = new Label_Item();
                 label_item->m_type_v = "qrs";
                 label_item->m_is_error = false;
                 label_item->m_timestamp = static_cast<uint64_t>(m_qrs_prev_timestamp);
                 label_item->m_tag_value_map["rrt"] = rri;

                 result->get_log_labels()->add_label_item(label_item);
                 result->get_log_labels()->add_label_by_object("LP", label_item);

                 m_qrs_prev_timestamp += qrs_timestamp;
             }

         }

         return true;
    }

    if (separated_line.size() == 7)
    {
        // algo input

        qlonglong current_time = separated_line.at(0).toLongLong();

        if(current_time - m_time_start > result->get_log_max_time())
        {
            result->set_log_max_time(current_time - m_time_start);
        }

        QString object_name = m_file_name.split("/").back();

        W_Object_Data *object = result->get_object(object_name);

        W_Sensor_Data *sensor = object->get_sensor("LP");

        XYZ_Sample xyz_sample_light;
        xyz_sample_light.m_object_id = 0;
        xyz_sample_light.m_x = separated_line.at(2).toDouble() * 1000;
        xyz_sample_light.m_time = current_time - m_time_start;

        result->add_xyz_sample(xyz_sample_light);

//        W_Signal_Data *signal_amb = sensor->get_signal("trig_amb");
//        if (signal_amb == nullptr)
//        {
//            signal_amb = new W_Signal_Data("trig_amb");
//            sensor->add_signal(signal_amb);
//            signal_amb->set_color("#55BB55");
//        }
//        signal_amb->add_sample(current_time - m_time_start, separated_line.at(1).toDouble() * 1000);

        W_Signal_Data *signal_sig = sensor->get_signal("l");
        if (signal_sig == nullptr)
        {
            signal_sig = new W_Signal_Data("l");
            sensor->add_signal(signal_sig);
            signal_sig->set_color("#5522BB");
        }
        signal_sig->add_sample(current_time - m_time_start, separated_line.at(2).toDouble() * 1000);


        XYZ_Sample xyz_sample_accel;
        xyz_sample_accel.m_object_id = 1;
        xyz_sample_accel.m_x = separated_line.at(3).toDouble();
        xyz_sample_accel.m_y = separated_line.at(4).toDouble();
        xyz_sample_accel.m_z = separated_line.at(5).toDouble();
        xyz_sample_accel.m_time = current_time - m_time_start;
        result->add_xyz_sample(xyz_sample_accel);

        W_Signal_Data *signal_ax = sensor->get_signal("ax");
        if (signal_ax == nullptr)
        {
            signal_ax = new W_Signal_Data("ax");
            sensor->add_signal(signal_ax);
            signal_ax->set_color(QColor("red").name());
        }
        signal_ax->add_sample(current_time - m_time_start, separated_line.at(3).toDouble());

        W_Signal_Data *signal_ay = sensor->get_signal("ay");
        if (signal_ay == nullptr)
        {
            signal_ay = new W_Signal_Data("ay");
            sensor->add_signal(signal_ay);
            signal_ay->set_color(QColor("green").name());
        }
        signal_ay->add_sample(current_time - m_time_start, separated_line.at(4).toDouble() );

        W_Signal_Data *signal_az = sensor->get_signal("az");
        if (signal_az == nullptr)
        {
            signal_az = new W_Signal_Data("az");
            sensor->add_signal(signal_az);
            signal_az->set_color(QColor("blue").name());
        }
        signal_az->add_sample(current_time - m_time_start, separated_line.at(5).toDouble());

        W_Signal_Data *signal_temp = sensor->get_signal("temp");
        if (signal_temp == nullptr)
        {
            signal_temp = new W_Signal_Data("temp");
            sensor->add_signal(signal_temp);
            signal_temp->set_color(QColor("red").darker(220).name());
        }
        signal_temp->add_sample(current_time - m_time_start, separated_line.at(6).toDouble());

        return true;
    }

    return  false;
}

