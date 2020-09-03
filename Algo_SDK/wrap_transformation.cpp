/***************************************************************************

             WINKAM TM strictly confidential 26.07.2017

 ***************************************************************************/
#include "wrap_transformation.h"
#include <QDebug>
#include "data/data_manager.h"
#include "data/wlogdata.h"
#include <unordered_set>
#include "average_sliding_window.h"
//#include "signal_acceleration.h"

#ifdef ALGO_APL_PA_DEFINE
#include "average_xy_velocity_calculator.h"
#include "max_sliding_window.h"
#endif

#ifdef ALGO_WKTR_DEFINE
#include "affine_transformation.h"
#include "frequency_normalizer.h"
#include "frequency_normalizer_3d.h"
#endif

namespace WKTR
{

QString add_0_to_time(uint64_t time, bool is_ms) noexcept
{
    if (!is_ms)
    {
        if (time >= 10)
        {
            return QString::number(time);
        }
        else
        {
            return "0" + QString::number(time);
        }
    }
    else
    {
        QString res = QString::number(time);

        while(res.length() != 3)
        {
            res.push_front("0");
        }

        return res;
    }
}

QString time_to_qstring(uint64_t time) noexcept
{
    uint64_t time_ms_res = time % 1000; //ms without sec
    uint64_t time_sec = time / 1000;
    uint64_t time_sec_res = time_sec % 60;
    uint64_t time_min = time_sec / 60;
    uint64_t time_min_res = time_min % 60;
    uint64_t time_hour_res = time_min / 60;

    QString time_res = add_0_to_time(time_hour_res) + ":" + add_0_to_time(time_min_res)
            + ":" + add_0_to_time(time_sec_res) + "." + add_0_to_time(time_ms_res, true);
    return time_res;
}

void Wrap_Transformation::init(const QString& log_id
                               , const QString& object_id
                               , const QString& sensor_id
                               , const QString& parameter_string
                               , Data_Manager* ptr_data_manager) noexcept
{
    if(ptr_data_manager == nullptr)
    {
        qDebug() << "Error! Wrap_Transformation::init() ptr_data_manager == nullptr";
        return;
    }

    m_ptr_log_data = ptr_data_manager->get_log_data();
    if (m_ptr_log_data == nullptr)
    {
        qDebug() << "Error! Wrap_Transformation::init() ptr_log == nullptr";
        return;
    }

    m_parameters_string = parameter_string;

    if (object_id.indexOf("All") < 0)
    {
        m_type = Tranform_Case::SINGLE;
        m_ptr_object_data = m_ptr_log_data->get_object(object_id);
        if (m_ptr_object_data == nullptr)
        {
            qDebug() << "Error! Wrap_Transformation::init() ptr_object == nullptr";
            return;
        }
        m_ptr_sensor_data = m_ptr_object_data->get_sensor(sensor_id);
        if (m_ptr_sensor_data == nullptr)
        {
            qDebug() << "Error! Wrap_Transformation::init() m_ptr_sensor_data == nullptr";
            return;
        }
        // define new "sensor" name as input sensor name + transformation name + parameters
        m_sensor_fusion_name = m_ptr_sensor_data->get_name() + " " + m_name
                + " " + m_parameters_string;
    }
    else
    {
        if (object_id.indexOf("Player") >= 0)
        {
            m_type = Tranform_Case::ALL_PLAYERS;
        }
        else if (object_id.indexOf("Ball") >= 0)
        {
            m_type = Tranform_Case::ALL_BALLS;
        }
        else
        {
            m_type = Tranform_Case::ALL_ALL;
        }

        // define new "sensor" name as input sensor name + transformation name + parameters
        m_sensor_fusion_name = object_id + " " + m_name
                + " " + m_parameters_string;
    }
}

QString Wrap_Transformation::get_name() const noexcept
{
    return m_name;
}

QString Wrap_Transformation::get_example() const noexcept
{
    return m_example_parameters;
}

QString Wrap_Transformation::get_message_parameters_error() const noexcept
{
    return ("Error! " + m_name + " needs specification as <"
            + m_example_parameters + ">, not " + m_parameters_string);
}

Wrap_Transformation::~Wrap_Transformation()
{
}

Example_Wrap::Example_Wrap()
{
    m_name = "lpf";
    m_example_parameters = "x 0.9";
}

void Example_Wrap::init(const QString& log_id
                        , const QString& object_id
                        , const QString& sensor_id
                        , const QString& parameter_string, Data_Manager* ptr_data_manager) noexcept
{
    Wrap_Transformation::init(log_id, object_id, sensor_id, parameter_string, ptr_data_manager);
    //qDebug() << parameter_string << "!";
    QStringList str_list = parameter_string.split(" ");

    if (str_list.length() >= 2)
    {
        m_signal_id = str_list.at(0);
        m_a = std::abs(str_list.at(1).toDouble());
        m_filter.set_coefficient(m_a);

        if (str_list.length() == 3)
        {
            m_b = std::abs(str_list.at(2).toDouble());
        }
        else
        {
            m_b = 0.;
        }
    }
    else
    {
        qDebug() << get_message_parameters_error();
    }
}

int Example_Wrap::process()
{
    // get signal(s)
    if (m_ptr_sensor_data == nullptr)
    {
        return 0;
    }

    W_Signal_Data* ptr_sig = m_ptr_sensor_data->get_signal(m_signal_id);
    if (ptr_sig == nullptr)
    {
        qDebug() << "Error! Example_TRC::process() can't find signal" << m_signal_id;
        return 0;
    }

    std::vector<Sample>* ptr_sample_vector = ptr_sig->get_samples();
    if(ptr_sample_vector->empty())
    {
        return 0;
    }

    // init filter
    m_filter.set_initial_value(ptr_sample_vector->front().second);

    // check uniqueness of sensor_fusion_name
    if(m_ptr_object_data->get_sensor(m_sensor_fusion_name) != nullptr)
    {
        return 0; //this filter already was applied to this sensor with the same parameters
    }

    // create new signals
    W_Sensor_Data* new_sensor_fusion = new W_Sensor_Data(m_sensor_fusion_name);
    W_Signal_Data* new_signal = new W_Signal_Data(m_signal_id);
    W_Signal_Data* new_signal_w = new W_Signal_Data(m_signal_id + "_");

    // process data
    for (Sample s : *ptr_sample_vector)
    {
        new_signal->add_sample(s.first, m_filter.filter(s.second));
    }

    double s = ptr_sample_vector->at(1).second;
    double b = s - ptr_sample_vector->front().second;

    std::vector<Sample>::const_iterator it = ptr_sample_vector->cbegin();
    while(it < ptr_sample_vector->cend())
    {
        new_signal_w->add_sample(it->first, s);

        double s_upd = m_a * it->second + (1. - m_a) * (s + b);
        b = m_b * (s_upd - s) + (1. - m_b) * b;

        s = s_upd;

        ++it;
    }

    // add result to Data_Manager
    new_sensor_fusion->add_signal(new_signal);
    new_sensor_fusion->add_signal(new_signal_w);

    m_ptr_object_data->add_sensor(new_sensor_fusion);

    new_signal->set_color(QColor(*ptr_sig->get_color()).darker(150).name());

    return 1;
}

Example_Wrap::~Example_Wrap()
{

}


HPF::HPF()
{
    m_name = "hpf";
    m_example_parameters = "x 0.2";
}

void HPF::init(const QString& log_id
               , const QString& object_id
               , const QString& sensor_id
               , const QString& parameter_string, Data_Manager* ptr_data_manager) noexcept
{
    Wrap_Transformation::init(log_id, object_id, sensor_id, parameter_string, ptr_data_manager);
    //qDebug() << parameter_string << "!";
    QStringList str_list = parameter_string.split(" ");

    if (str_list.length() == 2)
    {
        m_signal_id = str_list.at(0);
        m_filter.set_coefficient(std::abs(str_list.at(1).toDouble()));
    }
    else
    {
        qDebug() << get_message_parameters_error();
    }
}

int HPF::process()
{
    // get signal(s)
    if (m_ptr_sensor_data == nullptr)
    {
        return 0;
    }

    W_Signal_Data* ptr_sig = m_ptr_sensor_data->get_signal(m_signal_id);
    if (ptr_sig == nullptr)
    {
        qDebug() << "Error! Example_TRC::process() can't find signal" << m_signal_id;
        return 0;
    }

    std::vector<Sample>* ptr_sample_vector = ptr_sig->get_samples();
    if(ptr_sample_vector->empty())
    {
        return 0;
    }

    // init filter
    m_filter.set_initial_value(ptr_sample_vector->front().second);

    // check uniqueness of sensor_fusion_name
    if(m_ptr_object_data->get_sensor(m_sensor_fusion_name) != nullptr)
    {
        return 0; //this filter already was applied to this sensor with the same parameters
    }

    // create new signals
    W_Sensor_Data* new_sensor_fusion = new W_Sensor_Data(m_sensor_fusion_name);
    W_Signal_Data* new_signal = new W_Signal_Data(m_signal_id);

    // process data
    for (Sample s : *ptr_sample_vector)
    {
        new_signal->add_sample(s.first,/* 0.001 * s.second * */(s.second - m_filter.filter(s.second)));
    }

    // add result to Data_Manager
    new_sensor_fusion->add_signal(new_signal);
    m_ptr_object_data->add_sensor(new_sensor_fusion);

    new_signal->set_color(QColor(*ptr_sig->get_color()).darker(150).name());

    return 1;
}

HPF::~HPF()
{

}

Stratch_Compress::Stratch_Compress()
{
    m_name = "stratch_compress";
    m_example_parameters = "z 1.01";
}

void Stratch_Compress::init(const QString& log_id
               , const QString& object_id
               , const QString& sensor_id
               , const QString& parameter_string, Data_Manager* ptr_data_manager) noexcept
{
    Wrap_Transformation::init(log_id, object_id, sensor_id, parameter_string, ptr_data_manager);
    //qDebug() << parameter_string << "!";
    QStringList str_list = parameter_string.split(" ");

    if (str_list.length() == 2)
    {
        m_signal_id = str_list.at(0);
        m_compress_coef = str_list.at(1).toDouble();
    }
    else
    {
        qDebug() << get_message_parameters_error();
    }
}

int Stratch_Compress::process()
{
    // get signal(s)
    if (m_ptr_sensor_data == nullptr)
    {
        return 0;
    }

    W_Signal_Data* ptr_sig = m_ptr_sensor_data->get_signal(m_signal_id);
    if (ptr_sig == nullptr)
    {
        qDebug() << "Error! Example_TRC::process() can't find signal" << m_signal_id;
        return 0;
    }

    std::vector<Sample>* ptr_sample_vector = ptr_sig->get_samples();
    if(ptr_sample_vector->empty())
    {
        return 0;
    }

    // check uniqueness of sensor_fusion_name
    if(m_ptr_object_data->get_sensor(m_sensor_fusion_name) != nullptr)
    {
        return 0; //this filter already was applied to this sensor with the same parameters
    }

    // create new signals
    W_Sensor_Data* new_sensor_fusion = new W_Sensor_Data(m_sensor_fusion_name);
    W_Signal_Data* new_signal = new W_Signal_Data(m_signal_id);

    // process data
    for (Sample s : *ptr_sample_vector)
    {
        new_signal->add_sample(s.first * m_compress_coef, s.second);
    }

    // add result to Data_Manager
    new_sensor_fusion->add_signal(new_signal);
    m_ptr_object_data->add_sensor(new_sensor_fusion);

    new_signal->set_color(QColor(*ptr_sig->get_color()).darker(150).name());

    return 1;
}

Stratch_Compress::~Stratch_Compress()
{

}


//***************************
Sum_XYZ::Sum_XYZ()
{
    m_name = "sum_xyz";
    m_example_parameters = "";
}

void Sum_XYZ::init(const QString& log_id
                  , const QString& object_id
                  , const QString& sensor_id
                  , const QString& parameter_string, Data_Manager* ptr_data_manager) noexcept
{
    Wrap_Transformation::init(log_id, object_id, sensor_id, parameter_string, ptr_data_manager);
}

int Sum_XYZ::process()
{
    if (m_ptr_sensor_data == nullptr)
    {
        return 0 ;
    }

    W_Signal_Data* ptr_sig_x = m_ptr_sensor_data->get_signal("x");
    W_Signal_Data* ptr_sig_y = m_ptr_sensor_data->get_signal("y");
    W_Signal_Data* ptr_sig_z = m_ptr_sensor_data->get_signal("z");

    std::vector<Sample>* ptr_sample_vector_x = ptr_sig_x->get_samples();
    std::vector<Sample>* ptr_sample_vector_y = ptr_sig_y->get_samples();
    std::vector<Sample>* ptr_sample_vector_z = ptr_sig_z->get_samples();

    if(m_ptr_object_data->get_sensor(m_sensor_fusion_name) != nullptr)
    {
        return 0; // this filter already was applied to this sensor with the same parameters
    }
    W_Sensor_Data* new_sensor_fusion = new W_Sensor_Data(m_sensor_fusion_name);
    W_Signal_Data* new_signal_s = new W_Signal_Data("s");


    auto it_x = ptr_sample_vector_x->begin();
    auto it_y = ptr_sample_vector_y->begin();
    auto it_z = ptr_sample_vector_z->begin();

    while (it_x != ptr_sample_vector_x->end() && it_y != ptr_sample_vector_y->end()
        && it_z != ptr_sample_vector_z->end())
    {
        new_signal_s->add_sample(it_x->first, std::sqrt(it_x->second * it_x->second + it_y->second * it_y->second + it_z->second * it_z->second));

        ++it_x;
        ++it_y;
        ++it_z;
    }

    new_sensor_fusion->add_signal(new_signal_s);

    m_ptr_object_data->add_sensor(new_sensor_fusion);


    new_signal_s->set_color(QColor(*ptr_sig_x->get_color()).darker(150).name());

    new_signal_s->set_show_state(2);

    new_sensor_fusion->set_show_state(2);
    m_ptr_object_data->set_show_state_time_plot(true);

    new_sensor_fusion->set_color(QColor(*m_ptr_sensor_data->get_color()).darker().name());

    return 1;
}

Sum_XYZ::~Sum_XYZ()
{

}

//***************************

Example_TRC_1::Example_TRC_1()
{
    m_name = "example_1";
    m_example_parameters = "x kx";
}

void Example_TRC_1::init(const QString& log_id
                         , const QString& object_id
                         , const QString& sensor_id
                         , const QString& parameter_string, Data_Manager* ptr_data_manager) noexcept
{
    //    Wrap_Transformation::init(log_id, object_id, sensor_id, parameter_string, ptr_data_manager);

    std::vector<Sample> all_sample_vector;
    const W_Log_Data* ptr_log_data = ptr_data_manager->get_log_data();
    for (const auto& o : *(ptr_log_data->get_objects()))
    {
        for (const auto& sens : *(o.second->get_sensors()))
        {
            for (const auto& sig : *(sens.second->get_signals()))
            {
                for (const auto& s : *(sig.second->get_samples()))
                {
                    all_sample_vector.push_back(s);
                }
            }
        }
    }

    std::sort(all_sample_vector.begin(), all_sample_vector.end()
              , [](const Sample & a, const Sample & b) -> bool
    {
        return a.first < b.first;
    });

    qDebug() << all_sample_vector.front().first << all_sample_vector.back().first;

}

int Example_TRC_1::process()
{
    return 0;
    if (m_ptr_sensor_data == nullptr)
    {
        return 0;
    }

    W_Signal_Data* ptr_sig = m_ptr_sensor_data->get_signal(m_signal_id);
    if (ptr_sig == nullptr)
    {
        qDebug() << "Error! Example_TRC_1::process() can't find signal" << m_signal_id;
        return 0;
    }

    std::vector<Sample>* ptr_sample_vector = ptr_sig->get_samples();
    if(ptr_sample_vector->empty())
    {
        return 0;
    }

    if(m_ptr_object_data->get_sensor(m_sensor_fusion_name) != nullptr)
    {
        return 0; //this filter already was applied to this sensor with the same parameters
    }

    W_Sensor_Data* new_sensor_fusion = new W_Sensor_Data(m_sensor_fusion_name);
    W_Signal_Data* new_signal = new W_Signal_Data(m_signal_id);
    for (Sample s : *ptr_sample_vector)
    {
        new_signal->add_sample(s.first, s.second * m_TRC1_gain);
    }

    new_sensor_fusion->add_signal(new_signal);
    m_ptr_object_data->add_sensor(new_sensor_fusion);

    new_signal->set_color(QColor(*ptr_sig->get_color()).darker(120).name());

    return 1;
}

Example_TRC_1::~Example_TRC_1()
{

}

Shift_Wrap::Shift_Wrap()
{
    m_name = "shift";
    m_example_parameters = "1000";
    m_shift = 0;
}

void Shift_Wrap::init(const QString& log_id
                      , const QString& object_id
                      , const QString& sensor_id
                      , const QString& parameter_string, Data_Manager* ptr_data_manager) noexcept
{
    Wrap_Transformation::init(log_id, object_id, sensor_id, parameter_string, ptr_data_manager);
    m_shift = parameter_string.toLong();
}

int Shift_Wrap::process()
{
    if (m_ptr_sensor_data == nullptr)
    {
        return 0 ;
    }

    //W_Signal_Data* ptr_sig_x = m_ptr_sensor_data->get_signal("x");
    //W_Signal_Data* ptr_sig_y = m_ptr_sensor_data->get_signal("y");
    //W_Signal_Data* ptr_sig_z = m_ptr_sensor_data->get_signal("z");

    W_Signal_Data* ptr_sig_l = m_ptr_sensor_data->get_signal("l");

    //std::vector<Sample>* ptr_sample_vector_x = ptr_sig_x->get_samples();
    //std::vector<Sample>* ptr_sample_vector_y = ptr_sig_y->get_samples();
    //std::vector<Sample>* ptr_sample_vector_z = ptr_sig_z->get_samples();

    std::vector<Sample>* ptr_sample_vector_l = ptr_sig_l->get_samples();

    if(m_ptr_object_data->get_sensor(m_sensor_fusion_name) != nullptr)
    {
        return 0; // this filter already was applied to this sensor with the same parameters
    }

    W_Sensor_Data* new_sensor_fusion = new W_Sensor_Data(m_sensor_fusion_name);
    //W_Signal_Data* new_signal_x = new W_Signal_Data("x");
    //W_Signal_Data* new_signal_y = new W_Signal_Data("y");
    //W_Signal_Data* new_signal_z = new W_Signal_Data("z");

    W_Signal_Data* new_signal_l = new W_Signal_Data("l");

    //auto it_x = ptr_sample_vector_x->begin();
    //auto it_y = ptr_sample_vector_y->begin();
    //auto it_z = ptr_sample_vector_z->begin();

//    while (it_x != ptr_sample_vector_x->end() && it_y != ptr_sample_vector_y->end()
//           && it_z != ptr_sample_vector_z->end())
//    {
//        new_signal_x->add_sample(it_x->first + m_shift, it_x->second);
//        new_signal_y->add_sample(it_y->first + m_shift, it_y->second);
//        new_signal_z->add_sample(it_z->first + m_shift, it_z->second);

//        ++it_x;
//        ++it_y;
//        ++it_z;
//    }

//    new_sensor_fusion->add_signal(new_signal_x);
//    new_sensor_fusion->add_signal(new_signal_y);
//    new_sensor_fusion->add_signal(new_signal_z);

    auto it_l = ptr_sample_vector_l->begin();
    while(it_l != ptr_sample_vector_l->end())
    {
        new_signal_l->add_sample(it_l->first + m_shift, it_l->second);
        ++it_l;
    }

    new_sensor_fusion->add_signal(new_signal_l);
    m_ptr_object_data->add_sensor(new_sensor_fusion);


    //new_signal_x->set_color(QColor(*ptr_sig_x->get_color()).darker(150).name());
    //new_signal_y->set_color(QColor(*ptr_sig_y->get_color()).darker(150).name());
    //new_signal_z->set_color(QColor(*ptr_sig_z->get_color()).darker(150).name());
    new_signal_l->set_color(QColor(*ptr_sig_l->get_color()).darker(150).name());

    //new_signal_x->set_show_state(2);
    //new_signal_y->set_show_state(2);
    //new_signal_z->set_show_state(2);
    new_signal_l->set_show_state(2);

    new_sensor_fusion->set_show_state(2);
    m_ptr_object_data->set_show_state_time_plot(true);

    new_sensor_fusion->set_color(QColor(*m_ptr_sensor_data->get_color()).darker().name());


    return 1;
}

Shift_Wrap::~Shift_Wrap()
{

}

//--------------------------------------------------------------------------------------------------
Diff_Wrap::Diff_Wrap()
{
    m_name = "diff";
    m_example_parameters = "z";
}

void Diff_Wrap::init(const QString& log_id
                     , const QString& object_id
                     , const QString& sensor_id
                     , const QString& parameter_string, Data_Manager* ptr_data_manager) noexcept
{
    Wrap_Transformation::init(log_id, object_id, sensor_id, parameter_string, ptr_data_manager);
    m_signal_id = parameter_string;
}

int Diff_Wrap::process()
{
    if (m_ptr_sensor_data == nullptr)
    {
        return 0;
    }

    W_Signal_Data* ptr_sig = m_ptr_sensor_data->get_signal(m_signal_id);

    if (ptr_sig == nullptr)
    {
        return 0;
    }

    std::vector<Sample>* ptr_sample_vector = ptr_sig->get_samples();

    if(m_ptr_object_data->get_sensor(m_sensor_fusion_name) != nullptr)
    {
        return 0; // this filter already was applied to this sensor with the same parameters
    }

    W_Sensor_Data* new_sensor_fusion = new W_Sensor_Data(m_sensor_fusion_name);
    W_Signal_Data* new_signal = new W_Signal_Data(m_signal_id);

    auto it = ptr_sample_vector->cbegin();
    double prev = it->second;
    ++it;

    while (it != ptr_sample_vector->cend())
    {
        new_signal->add_sample(it->first, it->second - prev);
        prev = it->second;
        ++it;
    }

    new_sensor_fusion->add_signal(new_signal);

    m_ptr_object_data->add_sensor(new_sensor_fusion);

    new_signal->set_color(QColor(*ptr_sig->get_color()).darker(150).name());

    new_signal->set_show_state(2);

    new_sensor_fusion->set_show_state(2);
    m_ptr_object_data->set_show_state_time_plot(true);

    new_sensor_fusion->set_color(QColor(*m_ptr_sensor_data->get_color()).darker().name());

    return 1;
}

Diff_Wrap::~Diff_Wrap()
{

}
//--------------------------------------------------------------------------------------------------

Btrw_LPF::Btrw_LPF()
{
    m_name = "btrw_lpf";
    m_example_parameters = "x";
}

void Btrw_LPF::init(const QString& log_id
                    , const QString& object_id
                    , const QString& sensor_id
                    , const QString& parameter_string, Data_Manager* ptr_data_manager) noexcept
{
    Wrap_Transformation::init(log_id, object_id, sensor_id, parameter_string, ptr_data_manager);
    QStringList str_list = parameter_string.split(" ");

    if (str_list.length() == 1)
    {
        m_signal_id = str_list.at(0);
    }
    else
    {
        qDebug() << get_message_parameters_error();
    }
}

int Btrw_LPF::process()
{
    if (m_ptr_sensor_data == nullptr)
    {
        return 0;
    }

    W_Signal_Data* ptr_sig_x = m_ptr_sensor_data->get_signal("x");
    W_Signal_Data* ptr_sig_y = m_ptr_sensor_data->get_signal("y");
    W_Signal_Data* ptr_sig_z = m_ptr_sensor_data->get_signal("z");

    /*if (ptr_sig == nullptr)
    {
        qDebug() << "Error! Btrw_LPF::process() can't find signal" << m_signal_id;
        return 0;
    }*/

    std::vector<Sample>* ptr_sample_vector_x = ptr_sig_x->get_samples();
    std::vector<Sample>* ptr_sample_vector_y = ptr_sig_y->get_samples();
    std::vector<Sample>* ptr_sample_vector_z = ptr_sig_z->get_samples();

    /*if(ptr_sample_vector->empty())
    {
        return 0;
    }*/

    if(m_ptr_object_data->get_sensor(m_sensor_fusion_name) != nullptr)
    {
        return 0; //this filter already was applied to this sensor with the same parameters
    }

    W_Sensor_Data* new_sensor_fusion = new W_Sensor_Data(m_sensor_fusion_name);
    W_Signal_Data* new_signal_x = new W_Signal_Data("x");
    W_Signal_Data* new_signal_y = new W_Signal_Data("y");
    W_Signal_Data* new_signal_z = new W_Signal_Data("z");
    Butterworth_LPF lpf_x, lpf_y, lpf_z;
    std::vector<double> coeffs_A = {1.0, 4.0, 6.0, 4.0, 1.0};
    std::vector<double> coeffs_B = {3.7264144987, -5.2160481952, 3.2500182574, -0.7604898175};
    double gain = 1.520093384e+05;

    lpf_x.set_coefficients(coeffs_A, coeffs_B, gain);
    lpf_y.set_coefficients(coeffs_A, coeffs_B, gain);
    lpf_z.set_coefficients(coeffs_A, coeffs_B, gain);

    for (Sample s : *ptr_sample_vector_x)
    {
        new_signal_x->add_sample(s.first, lpf_x.filter(s.second));
    }
    for (Sample s : *ptr_sample_vector_y)
    {
        new_signal_y->add_sample(s.first, lpf_y.filter(s.second));
    }
    for (Sample s : *ptr_sample_vector_z)
    {
        new_signal_z->add_sample(s.first, lpf_z.filter(s.second));
    }

    new_sensor_fusion->add_signal(new_signal_x);
    new_sensor_fusion->add_signal(new_signal_y);
    new_sensor_fusion->add_signal(new_signal_z);
    m_ptr_object_data->add_sensor(new_sensor_fusion);


    new_signal_x->set_color(QColor(*ptr_sig_x->get_color()).darker(200).name());
    new_signal_y->set_color(QColor(*ptr_sig_y->get_color()).darker(200).name());
    new_signal_z->set_color(QColor(*ptr_sig_z->get_color()).darker(200).name());

    new_sensor_fusion->set_color(QColor(*m_ptr_sensor_data->get_color()).darker().name());

    return 1;
}

Btrw_LPF::~Btrw_LPF()
{

}

Full_Vel_Wrap::Full_Vel_Wrap()
{
    m_name = "full";
    m_example_parameters = "10";
}

void Full_Vel_Wrap::init(const QString& log_id
                              , const QString& object_id
                              , const QString& sensor_id
                              , const QString& parameter_string, Data_Manager* ptr_data_manager) noexcept
{
    Wrap_Transformation::init(log_id, object_id, sensor_id, parameter_string, ptr_data_manager);
    QStringList str_list = parameter_string.split(" ");

    if (str_list.length() == 1)
    {
        m_shoulder_lenght = str_list.front().toInt();
    }
    else
    {
        qDebug() << get_message_parameters_error();
    }
}

int Full_Vel_Wrap::process()
{
    if (m_ptr_sensor_data == nullptr)
    {
        return 0 ;
    }

    W_Signal_Data* ptr_sig_x = m_ptr_sensor_data->get_signal("x");
    W_Signal_Data* ptr_sig_y = m_ptr_sensor_data->get_signal("y");
    W_Signal_Data* ptr_sig_z = m_ptr_sensor_data->get_signal("z");

    std::vector<Sample>* ptr_sample_vector_x = ptr_sig_x->get_samples();
    std::vector<Sample>* ptr_sample_vector_y = ptr_sig_y->get_samples();
    std::vector<Sample>* ptr_sample_vector_z = ptr_sig_z->get_samples();

    if(m_ptr_object_data->get_sensor(m_sensor_fusion_name) != nullptr)
    {
        return 0; // this filter already was applied to this sensor with the same parameters
    }

    W_Sensor_Data* new_sensor_fusion = new W_Sensor_Data(m_sensor_fusion_name);
    W_Signal_Data* new_signal_f_s = new W_Signal_Data("fs");
    W_Signal_Data* new_signal_f_v = new W_Signal_Data("fv");
    W_Signal_Data* new_signal_f_a = new W_Signal_Data("fa");

    auto it_x = ptr_sample_vector_x->begin() + 2 * m_shoulder_lenght;
    auto it_y = ptr_sample_vector_y->begin() + 2 * m_shoulder_lenght;
    auto it_z = ptr_sample_vector_z->begin() + 2 * m_shoulder_lenght;

    while (it_x != ptr_sample_vector_x->end() && it_y != ptr_sample_vector_y->end()
           && it_z != ptr_sample_vector_z->end())
    {
        double full_curr = std::sqrt(it_x->second * it_x->second
                                     + it_y->second * it_y->second
                                     + it_z->second * it_z->second);

        double vel_x = it_x->second - (it_x - m_shoulder_lenght)->second;
        double vel_y = it_y->second - (it_y - m_shoulder_lenght)->second;
        double vel_z = it_z->second - (it_z - m_shoulder_lenght)->second;

        double vel_x_1 = (it_x - m_shoulder_lenght)->second - (it_x - 2 * m_shoulder_lenght)->second;
        double vel_y_1 = (it_y - m_shoulder_lenght)->second - (it_y - 2 * m_shoulder_lenght)->second;
        double vel_z_1 = (it_z - m_shoulder_lenght)->second - (it_z - 2 * m_shoulder_lenght)->second;

        new_signal_f_s->add_sample(it_x->first, full_curr);
        new_signal_f_v->add_sample(it_x->first, sqrt(vel_x * vel_x + vel_y * vel_y + vel_z * vel_z));
        new_signal_f_a->add_sample((it_x - m_shoulder_lenght)->first, sqrt((vel_x_1 - vel_x) * (vel_x_1 - vel_x)
                                                     + (vel_y_1 - vel_y) * (vel_y_1 - vel_y)
                                                     + (vel_z_1 - vel_z) * (vel_z_1 - vel_z)));

        ++it_x;
        ++it_y;
        ++it_z;
    }

    new_sensor_fusion->add_signal(new_signal_f_s);
    new_sensor_fusion->add_signal(new_signal_f_v);
    new_sensor_fusion->add_signal(new_signal_f_a);

    m_ptr_object_data->add_sensor(new_sensor_fusion);

    new_signal_f_s->set_color(QColor(*ptr_sig_x->get_color()).darker(150).name());
    new_signal_f_v->set_color(QColor(*ptr_sig_y->get_color()).darker(150).name());
    new_signal_f_a->set_color(QColor(*ptr_sig_z->get_color()).darker(150).name());

    new_signal_f_s->set_show_state(2);
    new_signal_f_v->set_show_state(2);
    new_signal_f_a->set_show_state(2);

    new_sensor_fusion->set_show_state(2);
    m_ptr_object_data->set_show_state_time_plot(true);

    new_sensor_fusion->set_color(QColor(*m_ptr_sensor_data->get_color()).darker().name());

    return 1;
}

Full_Vel_Wrap::~Full_Vel_Wrap()
{

}

Frequency_Reductor::Frequency_Reductor()
{
    m_name = "freq_red";
    m_example_parameters = "120 60";
}

void Frequency_Reductor::init(const QString& log_id
                              , const QString& object_id
                              , const QString& sensor_id
                              , const QString& parameter_string, Data_Manager* ptr_data_manager) noexcept
{
    Wrap_Transformation::init(log_id, object_id, sensor_id, parameter_string, ptr_data_manager);
    QStringList str_list = parameter_string.split(" ");

    if (str_list.length() == 2)
    {
        m_input_freq  = str_list.at(0).toInt();
        m_output_freq = str_list.at(1).toInt();
    }
    else
    {
        qDebug() << get_message_parameters_error();
    }
}

int Frequency_Reductor::process()
{
    if (m_ptr_sensor_data == nullptr)
    {
        return 0 ;
    }

    Frequency_Normalizer freq_norm_x(m_input_freq, m_output_freq);
    Frequency_Normalizer freq_norm_y(m_input_freq, m_output_freq);
    Frequency_Normalizer freq_norm_z(m_input_freq, m_output_freq);
    Frequency_Normalizer freq_norm_f(m_input_freq, m_output_freq);

    W_Signal_Data* ptr_sig_x = m_ptr_sensor_data->get_signal("x");
    W_Signal_Data* ptr_sig_y = m_ptr_sensor_data->get_signal("y");
    W_Signal_Data* ptr_sig_z = m_ptr_sensor_data->get_signal("z");

    std::vector<Sample>* ptr_sample_vector_x = ptr_sig_x->get_samples();
    std::vector<Sample>* ptr_sample_vector_y = ptr_sig_y->get_samples();
    std::vector<Sample>* ptr_sample_vector_z = ptr_sig_z->get_samples();

    if(m_ptr_object_data->get_sensor(m_sensor_fusion_name) != nullptr)
    {
        return 0; // this filter already was applied to this sensor with the same parameters
    }
    W_Sensor_Data* new_sensor_fusion = new W_Sensor_Data(m_sensor_fusion_name);
    W_Signal_Data* new_signal_x = new W_Signal_Data("x");
    W_Signal_Data* new_signal_y = new W_Signal_Data("y");
    W_Signal_Data* new_signal_z = new W_Signal_Data("z");
    W_Signal_Data* new_signal_f = new W_Signal_Data("F");

    auto it_x = ptr_sample_vector_x->begin();
    auto it_y = ptr_sample_vector_y->begin();
    auto it_z = ptr_sample_vector_z->begin();

    double x_norm;
    double y_norm;
    double z_norm;
    double f_norm;

    bool is_accepted;

    while (it_x != ptr_sample_vector_x->end() && it_y != ptr_sample_vector_y->end()
           && it_z != ptr_sample_vector_z->end())
    {
        x_norm = freq_norm_x.process_sample(it_x->second, &is_accepted);
        if (is_accepted)
        {
            new_signal_x->add_sample(it_x->first, x_norm);
        }

        y_norm = freq_norm_y.process_sample(it_y->second, &is_accepted);
        if (is_accepted)
        {
            new_signal_y->add_sample(it_y->first, y_norm);
        }

        z_norm = freq_norm_z.process_sample(it_z->second, &is_accepted);
        if (is_accepted)
        {
            new_signal_z->add_sample(it_z->first, z_norm);
        }

        f_norm = freq_norm_f.process_sample(std::sqrt(it_x->second * it_x->second
                                                      +it_y->second * it_y->second
                                                      +it_z->second * it_z->second), &is_accepted);
        if (is_accepted)
        {
            new_signal_f->add_sample(it_x->first, f_norm);
        }


        ++it_x;
        ++it_y;
        ++it_z;
    }

    new_sensor_fusion->add_signal(new_signal_x);
    new_sensor_fusion->add_signal(new_signal_y);
    new_sensor_fusion->add_signal(new_signal_z);
    new_sensor_fusion->add_signal(new_signal_f);

    m_ptr_object_data->add_sensor(new_sensor_fusion);

    new_signal_x->set_color(QColor(*ptr_sig_x->get_color()).darker(150).name());
    new_signal_y->set_color(QColor(*ptr_sig_y->get_color()).darker(150).name());
    new_signal_z->set_color(QColor(*ptr_sig_z->get_color()).darker(150).name());
    new_signal_f->set_color("#333355");

    new_signal_x->set_show_state(2);
    new_signal_y->set_show_state(2);
    new_signal_z->set_show_state(2);
    new_signal_f->set_show_state(2);

    new_sensor_fusion->set_show_state(2);
    m_ptr_object_data->set_show_state_time_plot(true);

    new_sensor_fusion->set_color(QColor(*m_ptr_sensor_data->get_color()).darker().name());

    return 1;
}

Frequency_Reductor::~Frequency_Reductor()
{

}

Bend_Wrap::Bend_Wrap()
{
    m_name = "derivative";
    m_example_parameters = "z 10";
}

void Bend_Wrap::init(const QString& log_id
                     , const QString& object_id
                     , const QString& sensor_id
                     , const QString& parameter_string, Data_Manager* ptr_data_manager) noexcept
{
    Wrap_Transformation::init(log_id, object_id, sensor_id, parameter_string, ptr_data_manager);
    QStringList str_list = parameter_string.split(" ");

    if (str_list.length() == 2)
    {
        m_signal_id = str_list.at(0);
        m_shoulder_lenght  = str_list.at(1).toInt();
    }
    else
    {
        qDebug() << get_message_parameters_error();
    }
}

int Bend_Wrap::process()
{
    if (m_ptr_sensor_data == nullptr)
    {
        return 0 ;
    }

    W_Signal_Data* ptr_sig = m_ptr_sensor_data->get_signal(m_signal_id);

    std::vector<Sample>* ptr_sample_vector = ptr_sig->get_samples();

    if (ptr_sample_vector->size() < m_shoulder_lenght * 2)
    {
        return 0; // to short signal
    }

    if(m_ptr_object_data->get_sensor(m_sensor_fusion_name) != nullptr)
    {
        return 0; // this filter already was applied to this sensor with the same parameters
    }

    W_Sensor_Data* new_sensor_fusion = new W_Sensor_Data(m_sensor_fusion_name);
    W_Signal_Data* new_signal = new W_Signal_Data(m_signal_id + "_der_1");
    W_Signal_Data* new_signal_2 = new W_Signal_Data(m_signal_id + "_der_2");

    W_Signal_Data* new_signal_experimental = new W_Signal_Data(m_signal_id + "_mult");


    auto it = ptr_sample_vector->begin() + m_shoulder_lenght;

    while (it < ptr_sample_vector->end() - m_shoulder_lenght)
    {
        //new_signal->add_sample(it->first, 10. * (std::abs((it - m_shoulder_lenght)->second - it->second)) / (0.00001 + std::abs((it + m_shoulder_lenght)->second - it->second)));
        new_signal->add_sample(it->first, (-it->second + (it + m_shoulder_lenght)->second));
        new_signal_2->add_sample(it->first, ((it + m_shoulder_lenght)->second - 2. * it->second + (it - m_shoulder_lenght)->second));

        double mult = 0.;
        if (it->second - (it - m_shoulder_lenght)->second < 0. && (it + m_shoulder_lenght)->second - it->second > 0.)
        {
            mult = (it->second - (it - m_shoulder_lenght)->second) * ((it + m_shoulder_lenght)->second - it->second);
        }
        new_signal_experimental->add_sample(it->first, mult > 0. ? 0. : 10. * std::sqrt(-1. * mult));
        ++it;
    }

    new_sensor_fusion->add_signal(new_signal);
    new_sensor_fusion->add_signal(new_signal_experimental);
    new_sensor_fusion->add_signal(new_signal_2);

    m_ptr_object_data->add_sensor(new_sensor_fusion);

    new_signal->set_color("#333399");
    new_signal_experimental->set_color("#993333");
    new_signal_2->set_color("#339933");

    new_signal->set_show_state(2);
    //new_signal_experimental->set_show_state(2);
    //new_signal_2->set_show_state(2);

    new_sensor_fusion->set_show_state(2);
    m_ptr_object_data->set_show_state_time_plot(true);

    return 1;
}

Bend_Wrap::~Bend_Wrap()
{

}

Coordinate_System_Modificator::Coordinate_System_Modificator()
{
    m_name = "coordinate";
    m_example_parameters = "0 0 1000 1000";
}

void Coordinate_System_Modificator::init(const QString& log_id
                                         , const QString& object_id
                                         , const QString& sensor_id
                                         , const QString& parameter_string, Data_Manager* ptr_data_manager) noexcept
{
    Wrap_Transformation::init(log_id, object_id, sensor_id, parameter_string, ptr_data_manager);
    QStringList str_list = parameter_string.split(" ");
    if (str_list.size() == 4)
    {
        double x_1 = str_list.at(0).toDouble();
        double y_1 = str_list.at(1).toDouble();
        double x_2 = str_list.at(2).toDouble();
        double y_2 = str_list.at(3).toDouble();

        //        double ox = x_2 - x_1;
        //        double oy = y_2 - y_1;
        //        double hypo = std::sqrt(ox * ox + oy * oy) + 1.E-6;
        //        m_sin_a = ox / hypo;// * (y_2 > 0 ? 1. : -1.);

        //        m_cos_a = oy / hypo;


        //        m_x_shift = 0 - x_2_modif;
        //        m_y_shift = 0 - y_2_modif;

        m_rotator.set_basis(x_1, y_1, x_2, y_2);
        std::pair<double, double> xy_modif = m_rotator.rotate(x_2, y_2);
        m_transmitor.set_basis(xy_modif.first, xy_modif.second);

    }
    else
    {
        qDebug() << "Error! Coordinate_System_Modificator needs parameter string as x1 y1 x2 y2";

    }
}

int Coordinate_System_Modificator::process()
{
    if (m_ptr_sensor_data == nullptr)
    {
        return 0 ;
    }

    W_Signal_Data* ptr_sig_x = m_ptr_sensor_data->get_signal("x");
    W_Signal_Data* ptr_sig_y = m_ptr_sensor_data->get_signal("y");
    W_Signal_Data* ptr_sig_z = m_ptr_sensor_data->get_signal("z");

    std::vector<Sample>* ptr_sample_vector_x = ptr_sig_x->get_samples();
    std::vector<Sample>* ptr_sample_vector_y = ptr_sig_y->get_samples();
    std::vector<Sample>* ptr_sample_vector_z = ptr_sig_z->get_samples();

    if(m_ptr_object_data->get_sensor(m_sensor_fusion_name) != nullptr)
    {
        return 0; // this filter already was applied to this sensor with the same parameters
    }
    W_Sensor_Data* new_sensor_fusion = new W_Sensor_Data(m_sensor_fusion_name);
    W_Signal_Data* new_signal_x = new W_Signal_Data("x");
    W_Signal_Data* new_signal_y = new W_Signal_Data("y");
    W_Signal_Data* new_signal_z = new W_Signal_Data("z");

    auto it_x = ptr_sample_vector_x->begin();
    auto it_y = ptr_sample_vector_y->begin();
    auto it_z = ptr_sample_vector_z->begin();



    while (it_x != ptr_sample_vector_x->end() && it_y != ptr_sample_vector_y->end()
           && it_z != ptr_sample_vector_z->end())
    {
        int x = it_x->second;
        int y = it_y->second;

        std::pair<double, double> xy_modif = m_rotator.rotate(x, y);
        xy_modif = m_transmitor.transmite(xy_modif.first, xy_modif.second);
        new_signal_x->add_sample(it_x->first, xy_modif.first);
        new_signal_y->add_sample(it_y->first, xy_modif.second);
        new_signal_z->add_sample(it_z->first, it_z->second);

        ++it_x;
        ++it_y;
        ++it_z;
    }

    new_sensor_fusion->add_signal(new_signal_x);
    new_sensor_fusion->add_signal(new_signal_y);
    new_sensor_fusion->add_signal(new_signal_z);

    m_ptr_object_data->add_sensor(new_sensor_fusion);

    new_signal_x->set_color(QColor(*ptr_sig_x->get_color()).darker(150).name());
    new_signal_y->set_color(QColor(*ptr_sig_y->get_color()).darker(150).name());
    new_signal_z->set_color(QColor(*ptr_sig_z->get_color()).darker(150).name());

    new_signal_x->set_show_state(2);
    new_signal_y->set_show_state(2);
    new_signal_z->set_show_state(2);

    new_sensor_fusion->set_show_state(2);
    m_ptr_object_data->set_show_state_time_plot(true);

    new_sensor_fusion->set_color(QColor(*m_ptr_sensor_data->get_color()).darker().name());

    return 1;
}

Coordinate_System_Modificator::~Coordinate_System_Modificator()
{

}

#ifdef ALGO_APL_PA_DEFINE

Avg_Speed_XY::Avg_Speed_XY()
{
    m_name = "avg_speed_xy";
    m_example_parameters = "120 2000";
}

void Avg_Speed_XY::init(const QString& log_id
                        , const QString& object_id
                        , const QString& sensor_id
                        , const QString& parameter_string, Data_Manager* ptr_data_manager) noexcept
{
    Wrap_Transformation::init(log_id, object_id, sensor_id, parameter_string, ptr_data_manager);
    QStringList tokens = parameter_string.split(" ");
    if (tokens.size() >= 2)
    {
        m_avg_number = tokens[0].toInt();
        m_accel_window = tokens[1].toInt();
    }
}

int Avg_Speed_XY::process()
{
    if (m_ptr_sensor_data == nullptr)
    {
        return 0;
    }

    W_Signal_Data* ptr_sig_x = m_ptr_sensor_data->get_signal("x");
    W_Signal_Data* ptr_sig_y = m_ptr_sensor_data->get_signal("y");
    W_Signal_Data* ptr_sig_z = m_ptr_sensor_data->get_signal("z");

    std::vector<Sample>* ptr_sample_vector_x = ptr_sig_x->get_samples();
    std::vector<Sample>* ptr_sample_vector_y = ptr_sig_y->get_samples();
    std::vector<Sample>* ptr_sample_vector_z = ptr_sig_z->get_samples();

    if(m_ptr_object_data->get_sensor(m_sensor_fusion_name) != nullptr)
    {
        return 0; // this filter already was applied to this sensor with the same parameters
    }
    W_Sensor_Data* new_sensor_fusion = new W_Sensor_Data(m_sensor_fusion_name);
    W_Signal_Data* new_signal_avg = new W_Signal_Data("avg");
    W_Signal_Data* new_signal_vel = new W_Signal_Data("vel");
    W_Signal_Data* new_signal_tmp = new W_Signal_Data("tmp");

    auto it_x = ptr_sample_vector_x->begin();
    auto it_y = ptr_sample_vector_y->begin();
    auto it_z = ptr_sample_vector_z->begin();

    WKTR::Average_Sliding_Window avg_tmp(m_avg_number);
    WK_WRS::Average_XY_Velocity_Calculator avg_speed(m_avg_number);
    WK_WRS::Average_XY_Velocity_Calculator avg_speed_2(m_avg_number);

    WKTR::Max_Sliding_Window msw(m_accel_window);
    WKTR::Max_Sliding_Window msw2(m_accel_window);

    double last_avg = 0;

    double last_x = ptr_sample_vector_x->front().second;
    double last_y = ptr_sample_vector_y->front().second;

    int counter = 0;

    while (it_x != ptr_sample_vector_x->end() && it_y != ptr_sample_vector_y->end()
           && it_z != ptr_sample_vector_z->end())
    {
        double avg = avg_speed.process_sample({it_x->second, it_y->second, it_z->second, it_x->first});
        new_signal_avg->add_sample(it_x->first, avg * 2.23694 / 1000.);
        //        new_signal_vel->add_sample(it_x->first,
        //                                   120. * avg_tmp.process_sample(sqrt((it_x->second - last_x) * (it_x->second - last_x) + (it_y->second - last_y) * (it_y->second - last_y))));
        new_signal_vel->add_sample(it_x->first, 120 * sqrt((it_x->second - last_x) * (it_x->second - last_x) + (it_y->second - last_y) * (it_y->second - last_y)));

        /*        double acc = avg > last_avg
                ? msw.process_sample(it_x->first, avg).second - msw2.process_sample(it_x->first, -avg).second
                : 0.;
        if(acc > 0.1)

            new_signal_tmp->add_sample(it_x->first, acc);
        }
        */

        double accel = msw.process_sample(it_x->first, avg).second + msw2.process_sample(it_x->first, -avg).second;
        //        if (avg < avg_speed.get_oldest())
        //        {
        //            accel = -accel;
        //        }

        new_signal_tmp->add_sample(it_x->first, accel / m_accel_window * 1000.);

        last_avg = avg;

        last_x = it_x->second;
        last_y = it_y->second;

        ++it_x;
        ++it_y;
        ++it_z;
    }

    new_sensor_fusion->add_signal(new_signal_avg);
    new_sensor_fusion->add_signal(new_signal_vel);
    new_sensor_fusion->add_signal(new_signal_tmp);

    m_ptr_object_data->add_sensor(new_sensor_fusion);

    new_signal_avg->set_color(QColor(*ptr_sig_x->get_color()).darker(150).name());
    new_signal_avg->set_show_state(2);

    new_signal_vel->set_color(QColor(*ptr_sig_y->get_color()).darker(150).name());
    new_signal_vel->set_show_state(0);

    new_signal_tmp->set_color(QColor(*ptr_sig_y->get_color()).darker(150).name());
    new_signal_tmp->set_show_state(2);

    new_sensor_fusion->set_show_state(2);
    m_ptr_object_data->set_show_state(1);

    new_sensor_fusion->set_color(QColor(*m_ptr_sensor_data->get_color()).darker().name());

    return 1;
}

Avg_Speed_XY::~Avg_Speed_XY()
{

}

#endif

Benchmark::Benchmark(const QString& name, const QString& parameter_example)
{
    m_name = "\u03b2 " + name;
    m_example_parameters = parameter_example;
}

void Benchmark::init(const QString& log_id
                     , const QString& object_id
                     , const QString& sensor_id
                     , const QString& parameter_string, Data_Manager* ptr_data_manager) noexcept
{

}

int Benchmark::process()
{
    return 0;
}

Benchmark::~Benchmark()
{

}


Rqi_Algo::Rqi_Algo(const QString& name, const QString& parameter_example)
{
    m_name = "RQI " + name;
    m_example_parameters = parameter_example;
}

void Rqi_Algo::init(const QString& log_id
                    , const QString& object_id
                    , const QString& sensor_id
                    , const QString& parameter_string, Data_Manager* ptr_data_manager) noexcept
{

}

int Rqi_Algo::process()
{
    return 0;
}

Rqi_Algo::~Rqi_Algo()
{

}

}

