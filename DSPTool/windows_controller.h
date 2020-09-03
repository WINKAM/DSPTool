#ifndef WINDOWS_CONTROLLER_H
#define WINDOWS_CONTROLLER_H

#include <QObject>
#include "main_window.h"
#include "plot/plot_controller.h"
#include "plot/court_plot_window.h"
#include "plot/base_plot_window.h"
#include <map>
#include <vector>

class W_Log_Data;
class QString;
class App_Controller;
class Video_Window;
class Video_Windows_Sync;
class QApplication;

struct Hoop;

class Windows_Controller : public QObject
{
    Q_OBJECT
private:
    App_Controller *m_app_controller;

    Main_Window *m_main_window;
    Plots_Controller *m_plots_controller;
    Base_Plot_Window *m_time_plot_window;
    Base_Plot_Window *m_xy_plot_window;
    Base_Plot_Window *m_xz_plot_window;
    Base_Plot_Window *m_yz_plot_window;

    W_Log_Data *m_log_data = nullptr;
    const std::map<QString, QString>* m_transforms_info_map;

    void create_menu(QMainWindow *window) noexcept;

    void close_all_video_windows() noexcept;

private slots:
    void update_signal_settings(const QString &object_id, const QString &sensor_id, const QString &signal_id, int is_show, const QString &color);
    void update_object_settings(const QString &object_id, int is_show, const QString &plot_id);
    void update_view_settings(const QString &object_id, const QString &sensor_id, int is_show, const QString &color, int weight, const QString &plot_id);
    void update_plots();
    void close_application(QCloseEvent *event);

    void fill_objects_list(const QString &log_name) noexcept;
    void fill_sensors_list(const QString &object_id) noexcept;
    void fill_filter_parameters(const QString &filter_id) noexcept;

    void open_file(const QString &file_name, bool is_with_labels, bool is_with_meta_file) noexcept;
    void open_dir() noexcept;
    void open_labels_file() noexcept;
    void open_meta_file() noexcept;
    void save_labels() noexcept;
    void open_video() noexcept;
    void open_errors_file() noexcept;

    void update_filtes_table(const W_Log_Data &log_data, const std::vector<WKTR::Transform_Spec*>& transf_spec_vector);

public:
    Windows_Controller(App_Controller *app_controller, W_Log_Data *log_data);
    ~Windows_Controller();

    Main_Window* get_main_window() const noexcept;
    Plots_Controller* get_plot2_controller() const noexcept;

    void fill_filters_list(const std::map<QString, QString>* transforms_info_map) noexcept;
    void show_dialog(const QString &message) noexcept;
    void save_transformation() noexcept;
    void update_hoops_setup(std::map<int, Hoop>* hoops) noexcept;
    void set_log_data(W_Log_Data *log_data);
    void update_log_data(W_Log_Data *log_data);

    bool show_save_dialog() noexcept;        

signals:
    void save_file_signal(const QString &file_name);
    void open_file_signal(const QString &file_name, bool is_with_label, bool is_with_meta_file);
    void open_dir_signal(const QString &file_name);
    void open_labels_file_signal(const QString &file_name);
    void open_errors_file_signal(const QString &file_name);
};

#endif // WINDOWS_CONTROLLER_H
