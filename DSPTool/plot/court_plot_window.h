#ifndef COURT_PLOT_WINDOW_H
#define COURT_PLOT_WINDOW_H

#include "plot/base_plot_window.h"
#include "view/apl_basketball_court.h"

struct Hoop;

class Court_Plot_Window : public Base_Plot_Window
{
    Q_OBJECT

private:
    Apl_Basketball_Court *m_basketball_court;
    Plot_Type m_court_type;

    double m_xy_scale_proportion;

protected:
    void setup_window() noexcept;

public:
    explicit Court_Plot_Window(Plot_Type type);
    void set_default_zoom() noexcept;
    void update_plot();
    ~Court_Plot_Window();
    void set_log_time(uint64_t time_min, uint64_t time_max) noexcept;
    uint64_t get_log_time() const noexcept;

    void set_hoops(std::map<int, Hoop> * hoops) noexcept;
protected Q_SLOTS:
    void update_label_item_filter();
    void update_error_item_filter();

public Q_SLOTS:
    void update_scale();
    void update_scale_controls(bool is_need_fix_proportion) noexcept;
    void update_from_scale_controls() noexcept;
};

#endif // COURT_PLOT_WINDOW_H
