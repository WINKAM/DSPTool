/***************************************************************************

             WINKAM TM strictly confidential 09.08.2017

 ***************************************************************************/
#include "wrap_ddsr_3d.h"
//#include "ddsr_filter.h"
#include "data/wlogdata.h"
#include "random_jump_filter.h"

namespace WKTR
{

Wrap_Ddsr_3D::Wrap_Ddsr_3D()
{
    m_name = "ddsr_3D";
    m_example_parameters = "z";
}

void Wrap_Ddsr_3D::init(const QString& log_id
          , const QString& object_id
          , const QString& sensor_id
          , const QString& parameter_string, Data_Manager* ptr_data_manager) noexcept
{
    Wrap_Transformation::init(log_id, object_id, sensor_id, parameter_string, ptr_data_manager);

    WKRJ::DDSR_Filter_Parameters filter_parameters;
    filter_parameters.m_lpf_basic_coef = 0.8;
    filter_parameters.m_lpf_addditional_coef = 0.3;
    filter_parameters.m_peak_filter_threshold = 20.;
    filter_parameters.m_stair_filter_start_threshold = 25.;
    filter_parameters.m_stair_filter_stop_threshold = 5.;
    filter_parameters.m_stair_filter_lenght = 10;

    if (m_filter == nullptr)
    {
        m_filter = new WKRJ::Random_Jump_Filter();
    }
    else
    {
        m_filter->reset();
    }

    m_filter->set_parameters(filter_parameters);

    m_filter->set_enabled_butterworth_filter(false);

//    QStringList str_list = parameter_string.split(" ");

//    if (str_list.length() == 1)
//    {
//        m_signal_id = str_list.at(0);
//    }
//    else
//    {
//        qDebug() << get_message_parameters_error();
//    }
}

int Wrap_Ddsr_3D::process()
{
    if (m_ptr_sensor_data == nullptr || m_filter == nullptr)
    {
        return 0;
    }

    W_Signal_Data* ptr_sigx = m_ptr_sensor_data->get_signal("x");
    W_Signal_Data* ptr_sigy = m_ptr_sensor_data->get_signal("y");
    W_Signal_Data* ptr_sigz = m_ptr_sensor_data->get_signal("z");

//    if (ptr_sig == nullptr)
//    {
//        qDebug() << "Error! Wrap_Ddsr::process() can't find signal" << m_signal_id;
//        return 0;
//    }

    std::vector<Sample>* ptr_sample_vectorx = ptr_sigx->get_samples();
    std::vector<Sample>* ptr_sample_vectory = ptr_sigy->get_samples();
    std::vector<Sample>* ptr_sample_vectorz = ptr_sigz->get_samples();

//    if(ptr_sample_vector->empty())
//    {
//        return 0;
//    }

    if(m_ptr_object_data->get_sensor(m_sensor_fusion_name) != nullptr)
    {
        return 0; //this filter already was applied to this sensor with the same parameters
    }

    W_Sensor_Data* new_sensor_fusion = new W_Sensor_Data(m_sensor_fusion_name);
    W_Signal_Data* new_signal_x = new W_Signal_Data("x");
    W_Signal_Data* new_signal_y = new W_Signal_Data("y");
    W_Signal_Data* new_signal_z = new W_Signal_Data("z");

    int i = 0;
    for (Sample s : *ptr_sample_vectorx)
    {
        Position_Sample sample = {ptr_sample_vectorx->at(i).second, ptr_sample_vectory->at(i).second
                                        , ptr_sample_vectorz->at(i).second, s.first};
        Position_Sample sample_out = m_filter->process_position_sample(sample, true);
        new_signal_x->add_sample(sample.m_time, sample_out.m_x);
        new_signal_y->add_sample(sample.m_time, sample_out.m_y);
        new_signal_z->add_sample(sample.m_time, sample_out.m_z);
        ++i;
    }

    new_sensor_fusion->add_signal(new_signal_x);
    new_sensor_fusion->add_signal(new_signal_y);
    new_sensor_fusion->add_signal(new_signal_z);
    m_ptr_object_data->add_sensor(new_sensor_fusion);


    new_signal_x->set_color(QColor(*ptr_sigx->get_color()).lighter(150).name());
    new_signal_y->set_color(QColor(*ptr_sigy->get_color()).lighter(150).name());
    new_signal_z->set_color(QColor(*ptr_sigz->get_color()).lighter(150).name());

    new_sensor_fusion->set_color(QColor(*m_ptr_sensor_data->get_color()).darker().name());

    return 1;
}

Wrap_Ddsr_3D::~Wrap_Ddsr_3D()
{
    delete m_filter;
}

}
