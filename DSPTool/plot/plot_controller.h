#ifndef PLOT2_CONTROLLER_H
#define PLOT2_CONTROLLER_H

#include <QObject>
#include "base_plot_window.h"
#include "time_plot_window.h"

class W_Object_Data;
struct Hoop;

class Plots_Controller : public QObject
{
    Q_OBJECT
private:
    Base_Plot_Window *m_plot_windows[4];

public:
    Plots_Controller();

    void update_data(W_Object_Data *object);
    void set_labels(Log_Labels *labels);
    void update_plots();
    void set_log_time(uint64_t time_min, uint64_t time_max);
    void update_alg_tmp_labels();

    ~Plots_Controller();

    void close_all() noexcept;

    Base_Plot_Window* get_plot_by_id(int id);

    void set_hoops(std::map<int, Hoop> * hoops) noexcept;

    uint64_t get_current_time() noexcept;

public slots:
    void show_plot(int plot_id, int type_id, bool key) noexcept;

signals:
    void close_plot_by_id_signal(int plot_id);
    void lock_video_controller();
    void unlock_video_controller();
};

#endif // PLOT2_CONTROLLER_H
