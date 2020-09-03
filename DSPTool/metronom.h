/***************************************************************************

             WINKAM TM strictly confidential 07.05.2020

 ***************************************************************************/
#ifndef METRONOM_H
#define METRONOM_H

#include <QObject>

class Metronom: public QObject
{
    Q_OBJECT
public:
    explicit Metronom(QObject *parent = nullptr);
    void run() noexcept;

signals:
    void next_step_got(qulonglong timestamp, int skip_count);
    void too_many_frames_skipped();

public slots:
    void timestamp_change(uint64_t timestamp) noexcept;
    void play() noexcept;
    void pause() noexcept;
    void release() noexcept;
    void change_speed(int speed) noexcept;

private:
    uint64_t m_timestamp_last_change; // для удаления накопленной ошибки
    uint64_t m_timestamp_last_emit;
    uint64_t m_timestamp;

    double m_avg_real_interval;
    int m_interval_init;
    int m_speed;
    int m_interval;
    int m_skip_counter;
    bool m_is_running;
    bool m_is_proc_loop_started;
};

#endif // METRONOM_H
