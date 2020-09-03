#ifndef WSIGNALDATA_H
#define WSIGNALDATA_H

#include <QColor>
#include <vector>
#include <tuple>
#include "../sample_structuries.h"
//класс для хранения данных о сигнале и его настройках отображения

class W_Signal_Data
{
private:
    Sample_Vector *m_samples;

    const QString m_name;
    QString m_color;
    int m_width;
    int m_is_show; // 0 - hide, 1 - show partical, 2 - show full

public:        
    W_Signal_Data(const QString& name) noexcept;

    QString get_name()const  noexcept;
    void add_sample(uint64_t time, double value) noexcept;

    Sample_Vector* get_samples() const noexcept;

    int is_show() const noexcept;
    void set_show_state(int state) noexcept;

    void set_width(int width) noexcept;
    int get_width() noexcept;

    void set_color(const QString &color) noexcept;
    QString* get_color() noexcept;

    ~W_Signal_Data();
};

#endif // WSIGNALDATA_H
