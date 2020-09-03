/***************************************************************************

             WINKAM TM strictly confidential 26.09.2017

 ***************************************************************************/
#include "wrap_rj3d_btr.h"
#include "random_jump_filter.h"
#include "butterworth_lpf.h"
#include "data/wlogdata.h"

namespace WKTR
{

Wrap_rj_3d_btr::Wrap_rj_3d_btr()
{
    m_name = "RJ_3D_BTR";
    m_example_parameters = "";
}

void Wrap_rj_3d_btr::init(const QString& log_id
          , const QString& object_id
          , const QString& sensor_id
          , const QString& parameter_string, Data_Manager* ptr_data_manager) noexcept
{
    Wrap_Transformation::init(log_id, object_id, sensor_id, parameter_string, ptr_data_manager);

    if (m_rj_filter == nullptr)
    {
        m_rj_filter = new WKRJ::Random_Jump_Filter();
    }
    else
    {
        m_rj_filter->reset();
    }

    if (m_btr_x_filter == nullptr)
    {
        m_btr_x_filter = new Butterworth_LPF();
        m_btr_z_filter = new Butterworth_LPF();
        m_btr_y_filter = new Butterworth_LPF();
    }
    else
    {
        m_btr_x_filter->reset();
        m_btr_y_filter->reset();
        m_btr_z_filter->reset();
    }

    m_rj_filter->set_enabled_butterworth_filter(false); // BTW manually added

    auto coeffs_A_1_9 = {1.0, 4.0, 6.0, 4.0, 1.0};
    auto coeffs_B_1_9 = {3.7400882011, -5.2535101224,  3.2843183397, -0.7709827093};
    double filter_gain_1_9 =  1.854194132e+05; //1.9


    auto coeffs_A_1_8 = {1.0, 4.0, 6.0, 4.0, 1.0};
    auto coeffs_B_1_8 = { 3.7537627567, -5.2911525842, 3.3189386048 , -0.7816187403};
    double filter_gain_1_8 =  2.286922409e+05; //1.8

    auto coeffs_A = {1.0, 4.0, 6.0, 4.0, 1.0};
    auto coeffs_B = { 3.6580603024, -5.0314335334, 3.0832283018, -0.7101038983};
    double filter_gain = 6.430156186e+04; //2.5

    m_btr_x_filter->set_coefficients(coeffs_A_1_8, coeffs_B_1_8, filter_gain_1_8);
    m_btr_y_filter->set_coefficients(coeffs_A_1_8, coeffs_B_1_8, filter_gain_1_8);
    m_btr_z_filter->set_coefficients(coeffs_A_1_9, coeffs_B_1_9, filter_gain_1_9);

    m_btr_x_filter->set_initial_value(m_ptr_sensor_data->get_signal("x")->get_samples()->front().second);
    m_btr_y_filter->set_initial_value(m_ptr_sensor_data->get_signal("y")->get_samples()->front().second);
    m_btr_z_filter->set_initial_value(m_ptr_sensor_data->get_signal("z")->get_samples()->front().second);
}

int Wrap_rj_3d_btr::process()
{
    if (m_ptr_sensor_data == nullptr || m_rj_filter == nullptr)
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

    W_Sensor_Data* new_sensor_fusion = new W_Sensor_Data(m_sensor_fusion_name);
    W_Signal_Data* new_signal_x = new W_Signal_Data("x");
    W_Signal_Data* new_signal_y = new W_Signal_Data("y");
    W_Signal_Data* new_signal_z = new W_Signal_Data("z");

    int i = 0;
    for (Sample s : *ptr_sample_vector_x)
    {
        Position_Sample sample = {ptr_sample_vector_x->at(i).second, ptr_sample_vector_y->at(i).second
                                        , ptr_sample_vector_z->at(i).second, s.first};
        Position_Sample sample_out = m_rj_filter->process_position_sample(sample, true);

        double sx = m_btr_x_filter->filter(sample_out.m_x);
        double sy = m_btr_y_filter->filter(sample_out.m_y);
        double sz = m_btr_z_filter->filter(sample_out.m_z);

        new_signal_x->add_sample(sample_out.m_time, sx);
        new_signal_y->add_sample(sample_out.m_time, sy);
        new_signal_z->add_sample(sample_out.m_time, sz);

        ++i;
    }

    new_sensor_fusion->add_signal(new_signal_x);
    new_sensor_fusion->add_signal(new_signal_y);
    new_sensor_fusion->add_signal(new_signal_z);

    m_ptr_object_data->add_sensor(new_sensor_fusion);

    new_signal_x->set_color(QColor(*ptr_sig_x->get_color()).darker(150).name());
    new_signal_y->set_color(QColor(*ptr_sig_y->get_color()).darker(150).name());
    new_signal_z->set_color(QColor(*ptr_sig_z->get_color()).darker(150).name());

    new_sensor_fusion->set_color(QColor(*m_ptr_sensor_data->get_color()).darker().name());


    return 1;
}

Wrap_rj_3d_btr::~Wrap_rj_3d_btr()
{
    delete m_rj_filter;
    delete m_btr_x_filter;
    delete m_btr_y_filter;
    delete m_btr_z_filter;
}

}
