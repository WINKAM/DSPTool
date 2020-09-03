#ifndef WLOGDATA_H
#define WLOGDATA_H

#include <map>
#include <QString>
#include "object/wobjectdata.h"
#include "label/log_labels.h"

//класс для хранения данных со всех объектов лога

class W_Log_Data
{
private:
    std::vector<XYZ_Sample> m_data_vector;
    std::map<QString, W_Object_Data*> m_objects_map;
    Log_Labels m_log_labels;
    QString m_name;
    long m_shitf_time;
    uint64_t m_log_max_time;
    uint64_t m_log_min_time;

    int m_data_set_id;

public:

    W_Log_Data(const QString& name);

    QString get_name() const noexcept;

    void set_name(QString name) noexcept;

    W_Object_Data* get_object(const QString& name) const noexcept;

    const std::map<QString, W_Object_Data*>* get_objects() const noexcept;

    const std::vector<XYZ_Sample>* get_data_vector() const noexcept;

    Log_Labels *get_log_labels() noexcept;

    int add_object(W_Object_Data* object) noexcept;

    W_Object_Data* get_object_to_change(const QString &name) noexcept;

    void add_xyz_sample(const XYZ_Sample& s) noexcept;

    void set_shitf_time(int64_t shift) noexcept;

    int64_t get_shitf_time() const noexcept;

    void set_log_max_time(uint64_t time) noexcept;

    void set_log_min_time(uint64_t time) noexcept;

    uint64_t get_log_max_time() const noexcept;

    uint64_t get_log_min_time() const noexcept;

    uint64_t get_usefull_time() const noexcept;

    void set_data_set_id(int data_set_id) noexcept;

    int get_data_set_id() const noexcept;
    ~W_Log_Data();
};

#endif // WLOGDATA_H
