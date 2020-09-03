#ifndef WSENSORDATA_H
#define WSENSORDATA_H

#include <map>
#include <QString>
#include "wsignaldata.h"

class QColor;

//класс для хранения сигналов с одного девайса и настроек их отображения
class W_Sensor_Data
{
private:
    std::map<QString, W_Signal_Data*> m_signals_map;
    const QString m_name;

    QString m_color;
    QColor m_color_generator;
    int m_weight;
    int m_is_show; // 0 - hide, 1 - show partical, 2 - show full

    int64_t m_shift_time;
    double m_split_time;

public:
    W_Sensor_Data(const QString& name);

    int add_signal(W_Signal_Data* signal);
    W_Signal_Data* get_signal(QString name);
    std::map<QString, W_Signal_Data*>* get_signals() noexcept;

    void set_shift_time(int64_t shift) noexcept;
    int64_t get_shift_time() const noexcept;

    void set_split_time(double split) noexcept;
    double get_split_time() const noexcept;

    void set_color(const QString &color) noexcept;
    QString* get_color() noexcept;

    void set_weight(int weight) noexcept;
    void set_signals_weight(int weight) noexcept;
    int get_weight() const noexcept;
    int get_signals_weight() const noexcept;

    int is_show() const noexcept;
    void set_show_state(int state) noexcept;

    QString get_name() const noexcept;
    ~W_Sensor_Data();
};

#endif // WSENSORDATA_H
