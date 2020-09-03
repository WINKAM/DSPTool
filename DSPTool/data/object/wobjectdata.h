#ifndef WOBJECTDATA_H
#define WOBJECTDATA_H

#include <map>
#include <QString>
#include "wsensordata.h"

//класс для хранения данных всех сенсоров с одного объекта (игрока, дроида, телефона)

class W_Object_Data
{
private:
    std::map<QString, W_Sensor_Data*> m_sensors_map;
    const QString m_name;
    bool m_is_shown_time_plot;
    bool m_is_shown_court_plots;

public:
    W_Object_Data(const QString& name);
    QString get_name() const noexcept;
    W_Sensor_Data* get_sensor(const QString& name) const noexcept;
    std::map<QString, W_Sensor_Data*>* get_sensors() noexcept;
    int add_sensor(W_Sensor_Data* sensor) noexcept;
    void remove_sensor(const QString& name) noexcept;
    void set_show_state_time_plot(bool state) noexcept;
    void set_show_state_court_plots(bool state) noexcept;
    bool is_show() const noexcept;
    ~W_Object_Data();
};

#endif // WOBJECTDATA_H
