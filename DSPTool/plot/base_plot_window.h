#ifndef BASE_PLOT_WINDOW_H
#define BASE_PLOT_WINDOW_H

#include <QWidget>
#include <map>
#include "view/viewer_plot.h"

class QLabel;
class QSlider;
class QVBoxLayout;
class QSpinBox;
class QDoubleSpinBox;
class W_Object_Data;
class QString;
class Viewer_Plot;
class QComboBox;
class QLineEdit;
class QHBoxLayout;
class Log_Labels;
class Label_Item;
class QCheckBox;
class QMenuBar;

class Base_Plot_Window : public QWidget
{
    Q_OBJECT

protected:
    Plot_Type m_plot_type;

    QLabel *m_court_label;
    QLabel *m_current_time_label;
    QLabel *m_speed_label;
    QLabel *m_vert_min_label;
    QLabel *m_vert_max_label;
    Viewer_Plot *m_signal_plot;
    QSlider *m_time1_slider;
    QSlider *m_delta_time_slider;
    QDoubleSpinBox *m_time1_spin;
    QSpinBox *m_delta_time_spin;
    QSpinBox *m_vertical_min_spin;
    QSpinBox *m_vertical_max_spin;

    QSpinBox *m_x_center_spin;
    QSpinBox *m_y_center_spin;
    QSpinBox *m_x_width_spin;
    QSpinBox *m_y_height_spin;

    QCheckBox *m_zoom_checkbox;
    QCheckBox *m_ruler_checkbox;

    QComboBox *m_objects_box;
    int m_selected_object;
    QComboBox *m_labels_box;
    QString m_selected_label;
    QComboBox *m_subtype_box;
    QLineEdit *m_label_info_edit;

    QComboBox *m_objects_box2;
    int m_selected_object2;
    QComboBox *m_labels_box2;
    QLineEdit *m_comment_edit;

    QComboBox *m_objects_error_box;
    int m_selected_error_object;
    QComboBox *m_errors_type_box;
    int m_selected_error_type;

    void create_gui() noexcept;
    QVBoxLayout* create_plot() noexcept;
    QVBoxLayout* create_controls() noexcept;
    QVBoxLayout* create_label_controls() noexcept;
    QVBoxLayout* create_errors_controls() noexcept;

    std::map<QString, W_Object_Data*> *m_objects_to_show;
    QStringList m_objects_to_label_combobox;
    Log_Labels *m_labels;

    std::vector<Label_Item*> m_selected_label_vector;
    std::vector<Label_Item*>::iterator m_current_label_iterator;

    std::vector<Label_Item*> m_selected_errors_vector;
    std::vector<Label_Item*>::iterator m_current_error_iterator;

    int m_count_repeat_q_key;

    void go_to_label(Label_Item *label);

    virtual void setup_window() noexcept = 0;

public:
    explicit Base_Plot_Window(Plot_Type plot_type, QWidget *parent = 0);
    ~Base_Plot_Window();
    virtual void set_default_zoom() noexcept = 0;
    virtual void set_log_time(uint64_t time_min, uint64_t time_max) noexcept = 0;
    virtual uint64_t get_log_time() const noexcept = 0;
    void update_data(W_Object_Data *object);
    virtual void update_plot() = 0;
    void set_labels(Log_Labels *labels);
    void clear_window();

    //void set_menu_bar(QMenuBar* menu_bar) noexcept;

    uint64_t get_current_time() noexcept;

    void update_alg_tmp_labels() noexcept;

    void change_label_subtype_combobox_current_text(const QString& text) noexcept;

Q_SIGNALS:
    void close_plot_signal(const QString &type);

    void change_index_objects_list_signal(int index);
    void change_index_objects2_list_signal(int index);
    void change_index_labels_list_signal(int index);
    void change_index_labels2_list_signal(int index);

    void change_indext_objects_error_list_signal(int index);
    void change_index_errors_type_list_signal(int index);

    void update_labels_list_signal();

    void set_position_signal(uint64_t value);
    void set_deltaT_signal(int value);

    void press_play_pause_signal();
    void speed_up_requested();
    void speed_down_requested();

protected Q_SLOTS:
    void open_first_label();
    void open_prev_label();
    void open_next_label();
    void open_last_label();

    void update_subtype_combobox();

    void open_first_error();
    void open_prev_error();
    void open_next_error();
    void open_last_error();

    virtual void update_label_item_filter() = 0;
    virtual void update_error_item_filter() = 0;
    void add_new_label();
    void delete_selected_label();

    void get_label_combobox_value(int value) noexcept;
    void get_slider_time(int value) noexcept;
    void get_spinbox_time(uint64_t value) noexcept;
    void get_slider_deltaT(int value) noexcept;
    void get_spinbox_deltaT(int value) noexcept;

public Q_SLOTS:
    void update_time1_value(uint64_t value);
    void update_deltaT_value(unsigned int value);
    void set_speed(int speed) noexcept;
    virtual void update_scale() = 0;
    void update_scale_controlls(double min_x, double max_x, double min_y, double max_y);

    void update_index_objects_list(int index) noexcept;
    void update_index_objects2_list(int index) noexcept;
    void update_index_labels_list(int index) noexcept;
    void update_index_labels2_list(int index) noexcept;

    void update_index_objects_errors_list(int index) noexcept;
    void update_index_errors_type_list(int index) noexcept;

    void update_labels_list();

protected:
    void closeEvent(QCloseEvent* event);
    bool event(QEvent *event);
    void showEvent(QShowEvent* event);
};

#endif // BASE_PLOT_WINDOW_H
