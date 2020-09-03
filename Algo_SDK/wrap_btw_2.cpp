/***************************************************************************

             WINKAM TM strictly confidential 02.10.2017

 ***************************************************************************/
#include "wrap_btw_2.h"
#include "data/object/wsensordata.h"
#include "data/object/wobjectdata.h"
#include "butterworth_lpf.h"

namespace WKTR
{

Wrap_Btw_2::Wrap_Btw_2()
{
    m_name = "btw lpf 2";
    m_example_parameters = "x 4.0 y 4.0 z 5.0";
}

void Wrap_Btw_2::init(const QString& log_id
                      , const QString& object_id
                      , const QString& sensor_id
                      , const QString& parameter_string, Data_Manager* ptr_data_manager) noexcept
{
    Wrap_Transformation::init(log_id, object_id, sensor_id
                              , parameter_string, ptr_data_manager);

    if (parameter_string.split(" ").size() == 6)
    {
        m_cut_off_x = parameter_string.split(" ").at(1);
        m_cut_off_y = parameter_string.split(" ").at(3);
        m_cut_off_z = parameter_string.split(" ").at(5);

    }
    else
    {
        qDebug() << get_message_parameters_error();
    }
}

Butterworth_LPF* create_btw_2(const QString& cut_off)
{
    std::vector<double> a_vector = {1., 2., 1.};

    Butterworth_LPF* btw = new Butterworth_LPF();

    int int_cuff_off = cut_off.toDouble() * 10;

    switch (int_cuff_off)
    {
    case 40:
        btw->set_coefficients(a_vector, {1.7055521455, -0.7436551951}, 1.049784742e+02);
        break;
    case 50:
        btw->set_coefficients(a_vector, {1.6329931619, -0.6905989232}, 6.943749902e+01);
        break;


    default:
        qDebug() << "Wrap_Btw_2 :: invalid cut-off. List of available cuf-off: 4.0 5.0";
        break;
    }

    return btw;
}

int Wrap_Btw_2::process()
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
        return 0; //this filter already was applied to this sensor with the same parameters
    }

    Butterworth_LPF* btw_x = create_btw_2(m_cut_off_x);
    Butterworth_LPF* btw_y = create_btw_2(m_cut_off_y);
    Butterworth_LPF* btw_z = create_btw_2(m_cut_off_z);

    W_Sensor_Data* new_sensor_fusion = new W_Sensor_Data(m_sensor_fusion_name);
    W_Signal_Data* new_signal_x = new W_Signal_Data("x");
    W_Signal_Data* new_signal_y = new W_Signal_Data("y");
    W_Signal_Data* new_signal_z = new W_Signal_Data("z");

    for (Sample s : *ptr_sample_vector_x)
    {
        double x_f = btw_x->filter(s.second);
        new_signal_x->add_sample(s.first, x_f);

    }
    for (Sample s : *ptr_sample_vector_y)
    {
        double y_f = btw_y->filter(s.second);
        new_signal_y->add_sample(s.first, y_f);
    }
    for (Sample s : *ptr_sample_vector_z)
    {
        new_signal_z->add_sample(s.first, btw_z->filter(s.second));
    }

    new_sensor_fusion->add_signal(new_signal_x);
    new_sensor_fusion->add_signal(new_signal_y);
    new_sensor_fusion->add_signal(new_signal_z);

    m_ptr_object_data->add_sensor(new_sensor_fusion);

    new_signal_x->set_color(QColor(*ptr_sig_x->get_color()).darker(150).name());
    new_signal_y->set_color(QColor(*ptr_sig_y->get_color()).darker(150).name());
    new_signal_z->set_color(QColor(*ptr_sig_z->get_color()).darker(150).name());

    new_sensor_fusion->set_color(QColor(*m_ptr_sensor_data->get_color()).darker().name());

    new_signal_x->set_show_state(2);
    new_signal_y->set_show_state(2);
    new_signal_z->set_show_state(2);

    new_sensor_fusion->set_show_state(2);
    m_ptr_object_data->set_show_state_time_plot(true);

    delete btw_x;
    delete btw_y;
    delete btw_z;

    return 1;
}

Wrap_Btw_2::~Wrap_Btw_2()
{

}

}
