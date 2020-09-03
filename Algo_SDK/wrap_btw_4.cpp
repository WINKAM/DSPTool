/***************************************************************************

             WINKAM TM strictly confidential 02.10.2017

 ***************************************************************************/
#include "wrap_btw_4.h"
#include "data/object/wsensordata.h"
#include "data/object/wobjectdata.h"
#include "butterworth_lpf.h"

namespace WKTR
{

Wrap_Btw_4::Wrap_Btw_4()
{
    m_name = "btw lpf 4";
    m_example_parameters = "x 1.8 y 1.8 z 1.9";
}

void Wrap_Btw_4::init(const QString& log_id
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

Butterworth_LPF* create_btw_4(const QString& cut_off)
{
    std::vector<double> a_vector = {1., 4., 6., 4., 1};

    Butterworth_LPF* btw = new Butterworth_LPF();

    int int_cuff_off = cut_off.toDouble() * 10;

    switch (int_cuff_off)
    {
    case 17:
        btw->set_coefficients(a_vector, {3.7674381215,  -5.3289759499, 3.3538817279 , -0.7923999281}, 2.855683536e+05);
        break;
    case 18:
        btw->set_coefficients(a_vector, { 3.7537627567, -5.2911525842, 3.3189386048 , -0.7816187403}, 2.286922409e+05);
        break;
    case 19:
        btw->set_coefficients(a_vector, {3.7400882011, -5.2535101224,  3.2843183397, -0.7709827093}, 1.854194132e+05);
        break;
    case 20:
        btw->set_coefficients(a_vector, {3.7264144987, -5.2160481952,  3.2500182574, -0.7604898175}, 1.520093384e+05);
        break;
    case 22:
        btw->set_coefficients(a_vector, {3.6990698273,  -5.1416644591, 3.1823680365 , -0.7399255301}, 1.051764170e+05);
        break;
    case 23:
        btw->set_coefficients(a_vector, {3.6853989451, -5.1047419027, 3.1490126437, -0.7298502470}, 8.861270442e+04);
        break;
    case 25:
        btw->set_coefficients(a_vector, {3.6580603024, -5.0314335334, 3.0832283018, -0.7101038983},  6.430156186e+04);
        break;
    case 30:
        btw->set_coefficients(a_vector, {3.5897338871, -4.8512758825, 2.9240526562, -0.6630104844},  3.201129162e+04);
        break;
    case 40:
        btw->set_coefficients(a_vector, { 3.4531851376  , -4.5041390916,  2.6273036182, -0.5778338981}, 1.077997190e+04);
        break;
    case 50:
        btw->set_coefficients(a_vector, {3.3168079106 , -4.1742455501, 2.3574027806, -0.5033753607}, 4.691779923e+03);
        break;


    default:
        qDebug() << "Wrap_Btw_4 :: invalid cut-off. List of available cuf-off: 1.7 1.8 1.9 2.0 2.2 2.3 2.5 3.0 4.0 5.0";
        break;
    }

    return btw;
}

int Wrap_Btw_4::process()
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

    Butterworth_LPF* btw_x = create_btw_4(m_cut_off_x);
    Butterworth_LPF* btw_y = create_btw_4(m_cut_off_y);
    Butterworth_LPF* btw_z = create_btw_4(m_cut_off_z);

    W_Sensor_Data* new_sensor_fusion = new W_Sensor_Data(m_sensor_fusion_name);
    W_Signal_Data* new_signal_x = new W_Signal_Data("x");
    W_Signal_Data* new_signal_y = new W_Signal_Data("y");
    W_Signal_Data* new_signal_z = new W_Signal_Data("z");

    for (Sample s : *ptr_sample_vector_x)
    {
        new_signal_x->add_sample(s.first, btw_x->filter(s.second));
    }
    for (Sample s : *ptr_sample_vector_y)
    {
        new_signal_y->add_sample(s.first, btw_y->filter(s.second));
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

Wrap_Btw_4::~Wrap_Btw_4()
{

}

}
