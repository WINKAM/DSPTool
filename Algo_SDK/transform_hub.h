/***************************************************************************

             WINKAM TM strictly confidential 27.07.2017

 ***************************************************************************/
#ifndef TRANSFORM_HUB_H
#define TRANSFORM_HUB_H

#include <QString>
#include <set>
#include <map>
#include <QDebug>

class Data_Manager;
class W_Object_Data;
class Transformation_Controller;

namespace WKTR
{

class Wrap_Transformation;

// Transform_Spec struct contains data about transformation applying
struct Transform_Spec
{
    QString m_log_id; // string identificator of log
    QString m_object_id; // string identificator of log
    QString m_sensor_id; // string identificator of log
    QString m_filter_id; // string identificator of transformation (Transformation_Controller::m_name)
    QString m_parameters; // parameter string (format as Transformation_Controller::m_example_parameters)

    Transform_Spec ()
    {
    }

    Transform_Spec (const QString& log, const QString& obj, const QString& sen, const QString& fil, const QString& par)
        : m_log_id(log), m_object_id(obj), m_sensor_id(sen), m_filter_id(fil), m_parameters(par)
    {
    }

    Transform_Spec (const Transform_Spec& object)
    {
        m_log_id = object.m_log_id;
        m_object_id = object.m_object_id;
        m_sensor_id = object.m_sensor_id;
        m_filter_id = object.m_filter_id;
        m_parameters = object.m_parameters;
    }

    // operators defined to store and compare specification string using std::set
    friend bool operator > (const Transform_Spec& a, const Transform_Spec& b)
    {
        return QString::compare(a.m_parameters, b.m_parameters) > 0
                && QString::compare(a.m_filter_id, b.m_filter_id) > 0
                && QString::compare(a.m_sensor_id, b.m_sensor_id) > 0
                && QString::compare(a.m_object_id, b.m_object_id) > 0
                && QString::compare(a.m_log_id, b.m_log_id) > 0;
    }

    friend bool operator < (const Transform_Spec& a, const Transform_Spec& b)
    {
        return !(a == b) && !(a > b);
    }

    friend bool operator == (const Transform_Spec& a, const Transform_Spec& b)
    {
        return (QString::compare(a.m_parameters, b.m_parameters) == 0
                && QString::compare(a.m_filter_id, b.m_filter_id) == 0
                && QString::compare(a.m_sensor_id, b.m_sensor_id) == 0
                && QString::compare(a.m_object_id, b.m_object_id) == 0
                && QString::compare(a.m_log_id, b.m_log_id) == 0);
    }
};

// Transformation_Controller is class to store, manage and apply filters, transformations, algorithms

class Transform_Hub
{
private:
    std::map<QString, Wrap_Transformation*> m_all_transforms_map; // map of all available transformation
    Data_Manager* m_ptr_data_manager; // pointer to Data_Manager object
    std::map<QString, QString> m_transforms_info_map; // map of information (name, example_parameters) to send to GUI
    std::vector<Transform_Spec*> m_used_transforms_vector; // // specification vector of current applied transformations

protected:
    void create_transforms() noexcept;
    void insert_transform_to_map(Wrap_Transformation *ptr_transform) noexcept;
    int find_apply_transform(const Transform_Spec& specification);
    void remove_transform(const Transform_Spec& specification) noexcept;
    int update_transform(const Transform_Spec& specification_old, const Transform_Spec &specification_new) noexcept;
    bool is_exist_spec(const Transform_Spec& specification) noexcept;
    W_Object_Data* find_parent_object(const Transform_Spec& specification) noexcept;

public:
    /**
     * Creates object, fills maps.
     * @param ptr_data_manager is pointer to Data_Manager object
     */
    Transform_Hub(Data_Manager* ptr_data_manager);

    /**
     * Gets information about available transformations
     * @return map <name, exaple_parameters> of available transformations
     */
    const std::map<QString, QString>* get_transforms_info() const noexcept;

    /**
     * Gets information about currently applied transformations
     * @return Transform_Spec vector of transformations
     */
    const std::vector<Transform_Spec*>* get_transforms_spec() const noexcept;

    /**
     * Adds and applies new transformation
     * @param spec is full specification about transformation
     */
    void add_transform_spec(const Transform_Spec& spec) noexcept;

    /**
     * Update and applies transformation
     * @param id is transformation identifer
     * @param spec is full specification about transformation
     */
    void update_transform_spec(unsigned int id, const Transform_Spec& spec)noexcept;

    /**
     * Delete transformation and remove result of this transformation
     * @param id is transformation identifer
     */
    void delete_transform_spec(unsigned int id) noexcept;

    QString apply_save_transform_all_data(const Transform_Spec& spec) noexcept;

    ~Transform_Hub();
};

}

#endif // TRANSFORM_HUB_H
