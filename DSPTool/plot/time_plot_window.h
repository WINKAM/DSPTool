#ifndef TIME_PLOT_WINDOW_H
#define TIME_PLOT_WINDOW_H

#include "plot/base_plot_window.h"

class Time_Plot_Window : public Base_Plot_Window
{

    Q_OBJECT
protected:
    void setup_window() noexcept;

public:
    explicit Time_Plot_Window(Plot_Type plot_type = Plot_Type::TIME);
    void add_mark_transform(double x, double width, int rColor, int gColor, int bColor, int type, const QString& message) noexcept;
    void update_plot();
    void set_default_zoom() noexcept;
    ~Time_Plot_Window();
    void set_log_time(uint64_t time_min, uint64_t time_max) noexcept;
    uint64_t get_log_time() const noexcept;

    void add_data_series(const std::vector<std::pair<float, float>>& data_series_vector, const QString &name, const QString &color, int width = 2) noexcept;
    void add_data_series(QPolygonF *data_series_polygone, const QString &name, const QString &color, int width = 2) noexcept;

    void clear_all_data_series() noexcept;
    void replot() noexcept;

    int get_delta_time() const noexcept;
    std::pair<int, int> get_min_max() const noexcept;

public Q_SLOTS:

    void update_error_item_filter();
    void update_vertical_min_max(double min_y, double max_y);
    void update_max_time(unsigned int time);
    // void update_delta_time(unsigned int time);

public Q_SLOTS:
    void update_label_item_filter();
    void update_scale();
};

#endif // TIME_PLOT_WINDOW_H
