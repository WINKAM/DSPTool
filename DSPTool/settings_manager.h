#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include <QSettings>
#include <QString>
#include <QDir>
#include <vector>
#include <map>

class Settings_Manager
{
public:
    static Settings_Manager* get_instance() noexcept;

    void write_last_log_file_path(const QString &path) noexcept;
    const QString get_last_log_file_path() const noexcept;
    void write_last_label_file_path(const QString &path) noexcept;
    const QString get_last_label_file_path() const noexcept;
    void write_last_video_file_path(const QString &path) noexcept;
    const QString get_last_video_file_path() const noexcept;
    void write_last_errors_file_path(const QString &path) noexcept;
    const QString get_last_errors_file_path() const noexcept;

    void write_last_filter(const QString &filter_id, const QString &parameters) noexcept;
    void read_last_filter(QString &filter_id, QString &parameters) noexcept;

    void write_signal_view_setting(const QString& episode, const QString& name, const QString& color, int width, int show_state) noexcept;
    void read_signal_view_setting(const QString& episode, const QString& name, QString* color, int* width, int *show_state) noexcept;

    void write_window_parameters(const QString& window_tag, const QString& property, const long value) noexcept;
    int get_window_parameters(const QString& window_tag, const QString& property) noexcept;

    void write_plot_zoom(int plot_id, int a, int b, int c, int d) noexcept;
    std::vector<int> get_plot_zoom(int plot_id) noexcept;

    void write_palette(int palette) noexcept;
    int get_palette_id() noexcept;

    void write_is_only_low_freq_flag(int is_only_low_freq) noexcept;
    int get_is_only_low_freq_flag() noexcept;

    void write_ids_of_opened_plots(std::vector<int> need_open_ids) noexcept;
    std::vector<int> get_ids_of_opened_plots() noexcept;

    void write_opened_videos(const QStringList& video_file_names, const QString& log_name, const std::vector<int>& is_full_screens) noexcept;
    QStringList get_opened_videos(const QString& log_name, std::vector<int> *is_full_screens) noexcept;

protected:
    const QString CONFIG_NAME = QString("config");
    const QString KEY_LAST_FILE_PATH = QString("log_path");
    const QString KEY_LAST_VIDEO_FILE_PATH = QString("video_path");
    const QString KEY_LAST_LABEL_FILE_PATH = QString("label_path");
    const QString KEY_LAST_ERRORS_FILE_PATH = QString("errors_path");
    const QString GROUP_FILTER = QString("group_filter");
    const QString KEY_LAST_FILTER = QString("last_filter");
    const QString KEY_LAST_FILTER_PARAMETERS = QString("last_filter_parameters");
    const QString GROUP_VIEW = QString("group_view");
    const QString KEY_SENSORS_VIEW = QString("sensors_view");
    const QString KEY_SIGNALS_VIEW = QString("signals_view");
    const QString KEY_PALETTE = QString("palette");
    const QString KEY_IDS_OPENED_PLOTS = QString("opened_plots");
    const QString KEY_ONLY_LOW_FREQ = QString("only_low_freq");

    Settings_Manager();
private:
    static Settings_Manager *s_instance;
    QSettings *m_settings;
};

#endif // SETTINGSMANAGER_H
