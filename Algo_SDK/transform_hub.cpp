/***************************************************************************

             WINKAM TM strictly confidential 27.07.2017

 ***************************************************************************/
#include "transform_hub.h"
#include "data/data_manager.h"
#include "data/wlogdata.h"
#include "wrap_transformation.h"
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QIODevice>
#include <QDateTime>
#include <iostream>
#include "unordered_map"
#include "vector"
#include "low_pass_filter.h"
#include "parser/base_parser.h"
#include "wrap_btw_2.h"
#include "wrap_btw_4.h"
#include "tester_filters.h"
#include "tester_algos.h"

#ifdef ALGO_WKTR_DEFINE
#endif


namespace WKTR
{

inline int round(double x)
{
    return static_cast<int>(x + (x < 0. ? -0.5 : 0.5));
}

// Write your code to add your new transforms into this function only
void Transform_Hub::create_transforms() noexcept
{
    insert_transform_to_map(new Example_Wrap());
    insert_transform_to_map(new Sum_XYZ());
    insert_transform_to_map(new HPF());
    insert_transform_to_map(new Example_TRC_1());
    insert_transform_to_map(new Wrap_Btw_2());
    insert_transform_to_map(new Wrap_Btw_4());
    insert_transform_to_map(new Shift_Wrap());
    insert_transform_to_map(new Btrw_LPF());
    insert_transform_to_map(new Benchmark("aplf"));
    insert_transform_to_map(new Benchmark("raw"));
    insert_transform_to_map(new Diff_Wrap());
    insert_transform_to_map(new Bend_Wrap());
    insert_transform_to_map(new Full_Vel_Wrap());
    insert_transform_to_map(new Stratch_Compress());



#ifdef ALGO_WKTR_DEFINE
    insert_transform_to_map(new Frequency_Reductor());
    insert_transform_to_map(new Coordinate_System_Modificator());
#endif



}

Transform_Hub::Transform_Hub(Data_Manager *ptr_data_manager) : m_ptr_data_manager(ptr_data_manager)
{
    create_transforms();
}

void Transform_Hub::insert_transform_to_map(Wrap_Transformation* ptr_transform) noexcept
{
    if (m_all_transforms_map.find(ptr_transform->get_name()) != m_all_transforms_map.end())
    {
        qDebug() << "Error! Transform_Hub::insert_transform_to_map() Transformation named"
                    + ptr_transform->get_name() + " is already exist!";
    }
    else
    {
        m_all_transforms_map.insert({ptr_transform->get_name(), ptr_transform});
        m_transforms_info_map.insert({ptr_transform->get_name(), ptr_transform->get_example()});
    }
}

int Transform_Hub::find_apply_transform(const Transform_Spec& specification)
{
    auto it = m_all_transforms_map.find(specification.m_filter_id);
    if (it == m_all_transforms_map.end())
    {
        qDebug() << "Error! Transform_Hub::find_apply_transform() Can't find" << specification.m_filter_id;
        return 0;
    }

    Wrap_Transformation* ptr_transformation = it->second;

    ptr_transformation->init(specification.m_log_id, specification.m_object_id, specification.m_sensor_id
                             , specification.m_parameters, m_ptr_data_manager);


    return ptr_transformation->process();
}

int Transform_Hub::update_transform(const Transform_Spec &specification_old, const Transform_Spec &specification_new) noexcept
{
    W_Object_Data* ptr_obj_old = find_parent_object(specification_old);
    W_Sensor_Data* ptr_sensor_old = nullptr;
    W_Sensor_Data* copy_old = nullptr;

    if(ptr_obj_old != nullptr)
    {
        ptr_sensor_old = ptr_obj_old->get_sensor(specification_old.m_sensor_id
                                                                + " " + specification_old.m_filter_id + " "
                                                                + specification_old.m_parameters);
        copy_old = new W_Sensor_Data(ptr_sensor_old->get_name() + "tmp");
        copy_old->set_color(*ptr_sensor_old->get_color());
        copy_old->set_weight(ptr_sensor_old->get_weight());

        copy_old->set_show_state(ptr_sensor_old->is_show());
        copy_old->set_shift_time(ptr_sensor_old->get_shift_time());
        copy_old->set_split_time(ptr_sensor_old->get_split_time());

        for (const auto& it : *ptr_sensor_old->get_signals())
        {
            W_Signal_Data* signal = new W_Signal_Data(it.second->get_name());
            copy_old->add_signal(signal);
            signal->set_show_state(it.second->is_show());
            signal->set_color(*it.second->get_color());
        }
    }

    remove_transform(specification_old);

    if (find_apply_transform(specification_new) == 0)
    {
        // can't create new sensor fusion
        return 0;
    }

    W_Object_Data* ptr_obj_new = find_parent_object(specification_new);
    if(ptr_obj_new == nullptr)
    {
        return 0;
    }

    W_Sensor_Data* ptr_sensor_new = ptr_obj_new->get_sensor(specification_new.m_sensor_id
                                                            + " " + specification_new.m_filter_id + " "
                                                            + specification_new.m_parameters);

    if (ptr_obj_old != nullptr)
    {
        /* We have
         * 1) old sensor with old data and actual view setting;
         * 2) new sensor with actual data and default view setting.
         */
        ptr_sensor_new->set_color(*copy_old->get_color());
        ptr_sensor_new->set_weight(copy_old->get_weight());
        ptr_sensor_new->set_show_state(copy_old->is_show());
        ptr_sensor_new->set_shift_time(copy_old->get_shift_time());
        ptr_sensor_new->set_split_time(copy_old->get_split_time());

        auto it_new = ptr_sensor_new->get_signals()->begin();
        auto it_old = copy_old->get_signals()->begin();
        while(it_new != ptr_sensor_new->get_signals()->end()
              && it_old != copy_old->get_signals()->end())
        {

            it_new->second->set_show_state(it_old->second->is_show());
            it_new->second->set_color(*it_old->second->get_color());
            ++it_new;
            ++it_old;
        }

        delete copy_old;
    }

    return 1;
}

void Transform_Hub::remove_transform(const Transform_Spec& specification) noexcept
{
    W_Object_Data* ptr_obj = find_parent_object(specification);
    if(ptr_obj == nullptr)
    {
        return;
    }

    ptr_obj->remove_sensor(specification.m_sensor_id + " " + specification.m_filter_id + " "
                           + specification.m_parameters);
}

W_Object_Data* Transform_Hub::find_parent_object(const Transform_Spec& specification) noexcept
{
    auto it = m_all_transforms_map.find(specification.m_filter_id);
    if (it == m_all_transforms_map.end())
    {
        return nullptr;
    }

    W_Log_Data* ptr_log = m_ptr_data_manager->get_log_data();
    if(ptr_log == nullptr)
    {
        return nullptr;
    }

    return ptr_log->get_object(specification.m_object_id);
}

bool Transform_Hub::is_exist_spec(const Transform_Spec &specification) noexcept
{
    return (std::find_if(m_used_transforms_vector.begin(), m_used_transforms_vector.end()
                         , [&specification] (const Transform_Spec* s) {return *s == specification;})
            != m_used_transforms_vector.end());
}

const std::map<QString, QString>* Transform_Hub::get_transforms_info() const noexcept
{
    return &m_transforms_info_map;
}

const std::vector<Transform_Spec*>* Transform_Hub::get_transforms_spec() const noexcept
{
    return &m_used_transforms_vector;
}

void Transform_Hub::add_transform_spec(const Transform_Spec& spec) noexcept
{
    Transform_Spec* new_spec = new Transform_Spec(spec);
    if (std::find_if(m_used_transforms_vector.begin(), m_used_transforms_vector.end()
                     , [spec] (const Transform_Spec* s) {return *s == spec;})
            != m_used_transforms_vector.end())
    {
        return;
    }

    if (find_apply_transform(spec) == 1)
    {
        m_used_transforms_vector.emplace_back(new_spec);
    }
}

void Transform_Hub::update_transform_spec(unsigned int id, const Transform_Spec& spec)noexcept
{
    if(id >= m_used_transforms_vector.size())
    {
        qDebug() << "Error! Transform_Hub::update_transform_spec() invalid id " << id;
        return;
    }

    Transform_Spec* ptr = m_used_transforms_vector[id];

    if (update_transform(*ptr, spec) == 1)
    {
        ptr->m_log_id = spec.m_log_id;
        ptr->m_object_id = spec.m_object_id;
        ptr->m_sensor_id = spec.m_sensor_id;
        ptr->m_filter_id = spec.m_filter_id;
        ptr->m_parameters = spec.m_parameters;
    }
    else
    {
        m_used_transforms_vector.erase(m_used_transforms_vector.begin() + id);
    }
}

void Transform_Hub::delete_transform_spec(unsigned int id) noexcept
{
    if(id >= m_used_transforms_vector.size())
    {
        qDebug() << "Error! Transform_Hub::delete_transform_spec() invalid id " << id;
        return;
    }

    Transform_Spec* ptr = m_used_transforms_vector[id];
    remove_transform(*ptr);

    m_used_transforms_vector.erase(m_used_transforms_vector.begin() + id);
}

QString Transform_Hub::apply_save_transform_all_data(const Transform_Spec& spec) noexcept
{
    const std::vector<XYZ_Sample>* ptr_data_vector = m_ptr_data_manager->get_log_data()->get_data_vector();
    Log_Labels* ptr_log_labels = m_ptr_data_manager->get_log_data()->get_log_labels();

    std::vector<XYZ_Sample> all_sample_vector;

    // u03b2 is symbol betta; it shows that need to apply transfromation and then run benchmark
    if (spec.m_filter_id.compare("\u03b2 aplf") == 0)
    {
        test_aplf(*ptr_data_vector, &all_sample_vector);
    }
    else if (spec.m_filter_id.compare("stratch_compress") == 0)
    {
        write_stretch_compressed(*ptr_data_vector, spec.m_parameters);
    }
    else if (spec.m_filter_id.compare("RQI Light") == 0)
    {
        //test_winkam_light(*ptr_data_vector, spec.m_parameters, *ptr_log_labels, m_ptr_data_manager);
    }
    else if (spec.m_filter_id.compare("RQI MB") == 0)
    {
        //test_winkam_mb(*ptr_data_vector, spec.m_parameters, *ptr_log_labels, m_ptr_data_manager);
    }

    else
    {
        qDebug() << "Transform_Hub::apply_save_transform_all_data :: There is not tester for" << spec.m_filter_id;
        // std::cout << "RQI test run without any filter!" << std::endl;
        // test_raw_signal(*ptr_data_vector, &all_sample_vector);
    }

    QString output_file_name = "./stlocation.csv";
    /*QFile file(output_file_name);
    file.open(QIODevice::WriteOnly);
    if (!file.isOpen())
    {
        qDebug() << "Error! Transform_Hub::apply_save_transform_all_data() Can't create file to write transformed log!";
        return "";
    }

    QTextStream file_stream;
    file_stream.setDevice(&file);

    unsigned long long row_id = 0;
    for (const auto& res_s : all_sample_vector)
    {
        file_stream << res_s.m_time << "," << (row_id++) << "," << res_s.m_object_id << ","
                    << round(res_s.m_x + OFFSET_X) << ","
                    << round(res_s.m_y + OFFSET_Y) << ","
                    << round(res_s.m_z + 0.5) << "\n";
    }

    file_stream.flush();
    file.close();*/

    return output_file_name;
}

Transform_Hub::~Transform_Hub()
{
    for (auto& e : m_all_transforms_map)
    {
        delete e.second;
    }

    for (auto& e : m_used_transforms_vector)
    {
        delete e;
    }
}

}
