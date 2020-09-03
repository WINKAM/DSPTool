/***************************************************************************

             WINKAM TM strictly confidential 13.04.2020

 ***************************************************************************/
#ifndef OPERATION_MODE_H
#define OPERATION_MODE_H

#include <unordered_map>
#include <vector>
#include <QString>

class Operation_Mode
{
public:
    static Operation_Mode& get_instance()
    {
        static Operation_Mode singleton;
        return singleton;
    }

    QString get_log_name() const noexcept;

    void set_begin_reading_data(const QString& file_name = "") noexcept;
    void set_episode_name(const QString& file_name) noexcept;
    void set_end_reading_data() noexcept;
    bool is_reading_data() const noexcept;

    //void set_plot_zoom(int plot_id, int a, int b, int c, int d) noexcept;
    //std::vector<int> get_plot_zoom(int plot_id) const noexcept;

    void set_only_low_freq_opening(bool is_only_low_freq_opening = true) noexcept;
    bool is_only_low_freq_opening() const noexcept;

private:
    QString m_log_name;
    //std::unordered_map<int, std::vector<int>> m_plot_zooms;
    bool m_is_only_low_freq_opening = false;
    bool m_is_reading_data = false;

    Operation_Mode() {}
    Operation_Mode(const Operation_Mode&) = delete;
    Operation_Mode& operator=(const Operation_Mode&) = delete;
    ~Operation_Mode() {}
};

#endif // OPERATION_MODE_H
