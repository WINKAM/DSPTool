#ifndef APP_CONTROLLER_H
#define APP_CONTROLLER_H

#include <QObject>
#include <QString>

class Windows_Controller;
class File_Reader;
class File_Writer;
class Facilitysetup_Reader;
class Data_Manager;
class QStringList;
class Wrap_Apl_Banchmark;
class W_Log_Data;
class QApplication;

namespace WKTR
{
    class Transform_Hub;
    class Transform_Spec;
}

enum class App_State {NOT_WAIT = 0, WAIT_RESULT_LOG = 1, WAIT_LOG = 2, WAIT_LABELS = 3, WAIT_LABELS_AFTER_LOG = 4,
                      OPEN_LOGS_DIR = 5, WAIT_LABEL_DIR_FORMAT = 6, WAIT_ERRORS = 7};

class App_Controller : public QObject
{
    Q_OBJECT

private:
    Windows_Controller* m_windows_controller;
    File_Reader* m_file_reader;
    File_Writer* m_file_writer;
    Facilitysetup_Reader* m_facility_reader;
    Wrap_Apl_Banchmark* m_wrap_apl_benchmark;
    Data_Manager* m_data_manager;
    Data_Manager* m_data_manger_test_res; // data manager to store result of processing log

    WKTR::Transform_Hub* m_transform_manager;

    App_State m_state;    
    QStringList m_files_list;

    bool m_is_cmd;
    bool m_is_dir;
    QString* m_filter;
    QString* m_filter_parameters;
    QString* m_hoop1_config;
    QString* m_hoop2_config;
    int m_benchmark_id;
    QString *m_application_path;
    QApplication* m_q_application;

    QString prepare_benchmark() noexcept;
    void prepare_facility_file(const QString &benchmark_path) noexcept;
    void change_hoop_config(const QString &path, int hoop_id, const QString &config) noexcept;
    void run_benchmark(const QString &filter_id) noexcept;
    QString get_new_hoop_parameter(QString old_parameter, int shift) noexcept;

public:
    explicit App_Controller(QObject *parent = 0);
    ~App_Controller();
    void set_parameters(int benchmark_id, char *filter, char *parameters) noexcept;
    void set_hoops_config(char* hoop1_config, char* hoop2_config) noexcept;
    void start_cmd(const char *path, bool is_file) noexcept;
    void start_gui() noexcept;
    void set_q_application(QApplication* q_application) noexcept;
    QApplication* get_q_application() noexcept;

signals:
    void save_transformation_gui();
    void show_dialog_gui(const QString &msg);
    void set_log_data_gui(W_Log_Data *log_data);
    void update_log_data_gui(W_Log_Data *log_data);
    void update_filtes_table_gui(const W_Log_Data &log_data, const std::vector<WKTR::Transform_Spec*>& transf_spec_vector);
    void update_alg_tmp_labels();
    void close_application();
public slots:
    void save_file(const QString &file_name) noexcept;
    void open_file(const QString &file_name, bool is_with_label, bool is_with_meta_file) noexcept;
    void open_dir(const QString &dir_name) noexcept;
    void open_labels_file(const QString &file_name) noexcept;
    void open_errors_file(const QString &file_name) noexcept;
    void update_data_from_log(bool is_success, const QString &file_name);
    void show_writing_status(bool is_success, const QString &file_name) noexcept;
    void process_filter(const QString &filter_id, const QString &log_id, const QString &object_id, const QString &sensor_id, const QString &parameters);
    void process_all_data(const QString &filter_id, const QString &object_id, const QString &parameters);
    void update_filter(int id, const QString &filter_id, const QString &log_id, const QString &object_id, const QString &sensor_id, const QString &parameters);
    void delete_filter(int filter_id);
};

#endif // APP_CONTROLLER_H

