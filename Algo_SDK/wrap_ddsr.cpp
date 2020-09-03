/***************************************************************************

             WINKAM TM strictly confidential 04.08.2017

 ***************************************************************************/
#include "wrap_ddsr.h"
//#include "ddsr_filter.h"
#include "data/wlogdata.h"
#include "random_jump_filter.h"
#include "ddsr_filter.h"

namespace WKTR
{

Wrap_Ddsr::Wrap_Ddsr()
{
    m_name = "ddsr";
    m_example_parameters = "z";
}

void Wrap_Ddsr::init(const QString& log_id
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
        m_filter = new WKRJ::DDSR_Filter(filter_parameters);
    }
    else
    {
        m_filter->reset();
    }

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

int Wrap_Ddsr::process()
{
    if (m_ptr_sensor_data == nullptr || m_filter == nullptr)
    {
        return 0;
    }

//    W_Signal_Data* ptr_sigx = m_ptr_sensor_data->get_signal("x");
//    W_Signal_Data* ptr_sigy = m_ptr_sensor_data->get_signal("y");
//    W_Signal_Data* ptr_sigz = m_ptr_sensor_data->get_signal("z");

    W_Signal_Data* ptr_sig = m_ptr_sensor_data->get_signal(m_signal_id);

    if (ptr_sig == nullptr)
    {
        qDebug() << "Error! Wrap_Ddsr::process() can't find signal" << m_signal_id;
        return 0;
    }

//    std::vector<Sample>* ptr_sample_vectorx = ptr_sigx->get_samples();
//    std::vector<Sample>* ptr_sample_vectory = ptr_sigy->get_samples();
//    std::vector<Sample>* ptr_sample_vectorz = ptr_sigz->get_samples();

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
//    W_Signal_Data* new_signal_x = new W_Signal_Data("x");
//    W_Signal_Data* new_signal_y = new W_Signal_Data("y");
//    W_Signal_Data* new_signal_z = new W_Signal_Data("z");

     W_Signal_Data* new_signal = new W_Signal_Data(m_signal_id);

    int i = 0;
    for (Sample s : *ptr_sample_vector)
    {
//        Position_Sample sample = {ptr_sample_vectorx->at(i).second, ptr_sample_vectory->at(i).second
//                                        , ptr_sample_vectorz->at(i).second, s.first};
//        Position_Sample sample_out = m_filter->process_position_sample(sample, false);
//        new_signal_x->add_sample(sample.m_time, sample_out.m_x);
//        new_signal_y->add_sample(sample.m_time, sample_out.m_y);
//        new_signal_z->add_sample(sample.m_time, sample_out.m_z);
        new_signal->add_sample(s.first, m_filter->process_point(s.second));
        ++i;
    }

    //new_sensor_fusion->add_signal(new_signal_x);
    //new_sensor_fusion->add_signal(new_signal_y);
    //new_sensor_fusion->add_signal(new_signal_z);
    new_sensor_fusion->add_signal(new_signal);
    m_ptr_object_data->add_sensor(new_sensor_fusion);

//    new_signal_x->set_color(QColor(*ptr_sigx->get_color()).darker(150).name());
//    new_signal_y->set_color(QColor(*ptr_sigy->get_color()).darker(150).name());
//    new_signal_z->set_color(QColor(*ptr_sigz->get_color()).darker(150).name());

    new_sensor_fusion->set_color(QColor(*m_ptr_sensor_data->get_color()).darker().name());
    return 1;
}

Wrap_Ddsr::~Wrap_Ddsr()
{
    delete m_filter;
}

}
