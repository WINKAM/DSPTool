#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <map>

class QAction;
class Simple_Checkable_QTreeWidget;
class QWidget;
class QLineEdit;
class QPushButton;
class W_Log_Data;
class W_Object_Data;
class QString;
class QTableWidget;
class QComboBox;
class QVBoxLayout;
class QListWidget;
class QMenuBar;
class QTreeWidget;

namespace WKTR
{
class Transform_Spec;
}


class Main_Window : public QMainWindow
{
    Q_OBJECT
public:
    Main_Window(QWidget *parent = 0);
    ~Main_Window();

    void fill_mainwindow_settings(const W_Log_Data &log_data);
    void fill_objects_list(W_Log_Data *log_data);
    void fill_sensors_list(W_Object_Data *object_data);
    void fill_sensors_list(const QStringList &sensor) noexcept;
    void fill_filters_table(const W_Log_Data& log_data, const std::vector<WKTR::Transform_Spec*>& transf_spec_vector) noexcept;
    void fill_filters_list(const std::map<QString, QString>* transforms_info_map) noexcept;
    void fill_filter_parameters(const QString &parameters) noexcept;

private:
    void create_gui();
    void create_plots_settings();
    void create_video_settings();
    void create_labels_settings();
    void create_view_settings();
    void create_menu() noexcept;

    void clear_all_data();
    QVBoxLayout* create_filters_settings();

    QWidget* create_view_settings_item(const QString &color_str, int sensor_weight, const QString& object_name = "");
    int create_table_filter_row(int id, const W_Log_Data &log_data, const WKTR::Transform_Spec& spec) noexcept;

private:

    Simple_Checkable_QTreeWidget *m_plots_tree;
    Simple_Checkable_QTreeWidget *m_time_plot_signal_settings_tree;
    Simple_Checkable_QTreeWidget *m_court_plots_signal_settings_tree;
    Simple_Checkable_QTreeWidget *m_video_list;
    QTreeWidget* m_labels_list;
    QTableWidget *m_filters_table;
    QComboBox *m_filters_combo_box;
    QComboBox *m_logs_combo_box;
    QComboBox *m_objects_combo_box;
    QComboBox *m_sensors_combo_box;
    QLineEdit *m_parameters_edit;
    QPushButton *m_update_btn;

public slots:    
    void update_view_settings();
    void change_button_color() noexcept;
    void change_signal_weight(int w) noexcept;

    void open_plot_by_id(int plot_id) noexcept;
    void close_plot_by_id(int plot_id) noexcept;

    void set_default_signals_view() noexcept;
    void save_signals_view() noexcept;
    void load_signals_view() noexcept;

signals:
    void open_log_file_signal(const QString& file_name, bool with_label, bool with_meta_file);
    void open_label_file_signal();    
    void open_meta_file_signal();
    void open_dir_signal();
    void open_video_signal();
    void save_label_file_signal();
    void open_error_file_signal();
    void show_plot_signal(int plot_id, int sub_id, bool key = true);
    void update_object_settings_signal(const QString &object_id, int is_show, const QString& plot_id);
    void update_signal_settings_signal(const QString &object_id, const QString &sensor_id, const QString &signal_id, int is_show, const QString &color);
    void update_view_settings_signal(const QString &object_id, const QString &sensor_id, int is_show, const QString &color, int weight, const QString &plot_id);
    void update_plots_signal();
    void close_window_signal(QCloseEvent *event);

    void fill_objects_list_signal(const QString &log_name);
    void fill_sensors_list_signal(const QString &object_id);
    void fill_filter_parameters_signal(const QString &filter_id);

    void process_filter_signal(const QString &filter_id, const QString &log_id, const QString &object_id, const QString &sensor_id, const QString &parameters);
    void update_filter_signal(int id, const QString &filter_id, const QString &log_id, const QString &object_id, const QString &sensor_id, const QString &parameters);
    void delete_filter_signal(int filter_id);

    void process_filter_all_data(const QString &filter_id, const QString &object_id, const QString &parameters);
    void update_alg_tmp_labels();

protected:
    void closeEvent(QCloseEvent *event);
    void showEvent(QShowEvent* event);

public slots:
    void save_transformation() noexcept;
};

#endif // MAINWINDOW_H
