#include "settings_manager.h"
#include <QDebug>
#include "utils.h"

Settings_Manager* Settings_Manager::s_instance = nullptr;

Settings_Manager* Settings_Manager::get_instance() noexcept
{
    if (s_instance == nullptr)
    {
        s_instance = new Settings_Manager();
    }
    return s_instance;
}

Settings_Manager::Settings_Manager()
{
    m_settings = new QSettings("config", QSettings::IniFormat);
    qDebug() << "read config file";
}

void Settings_Manager::write_last_log_file_path(const QString &path) noexcept
{
    QStringList tmp = path.split("/");
    tmp[tmp.size() - 1] = tmp[tmp.size() - 1].remove("temp_");

    QStringList tmp2 = tmp[tmp.size() - 1].split("_");
    tmp2.removeLast(); // remove 25

    tmp[tmp.size() - 1] = tmp2.join("_") + "_120" + ".csv";

    m_settings->setValue(KEY_LAST_FILE_PATH, tmp.join("/"));

    QString result = path.left(path.lastIndexOf("/"));
    write_last_video_file_path(result.replace("sensors", "videos"));
}

const QString Settings_Manager::get_last_log_file_path() const noexcept
{
    return m_settings->value(KEY_LAST_FILE_PATH, QDir::home().absolutePath()).toString();
}

void Settings_Manager::write_last_label_file_path(const QString &path) noexcept
{
    m_settings->setValue(KEY_LAST_LABEL_FILE_PATH, path);
}

const QString Settings_Manager::get_last_label_file_path() const noexcept
{
    return m_settings->value(KEY_LAST_LABEL_FILE_PATH, QDir::home().absolutePath()).toString();
}

void Settings_Manager::write_last_video_file_path(const QString &path) noexcept
{
    m_settings->setValue(KEY_LAST_VIDEO_FILE_PATH, path);
}

const QString Settings_Manager::get_last_video_file_path() const noexcept
{
    return m_settings->value(KEY_LAST_VIDEO_FILE_PATH, QDir::home().absolutePath()).toString();
}

void Settings_Manager::write_last_errors_file_path(const QString &path) noexcept
{
    m_settings->setValue(KEY_LAST_ERRORS_FILE_PATH, path);
}

const QString Settings_Manager::get_last_errors_file_path() const noexcept
{
    return m_settings->value(KEY_LAST_ERRORS_FILE_PATH, QDir::home().absolutePath()).toString();
}

void Settings_Manager::write_last_filter(const QString &filter_id, const QString &parameters) noexcept
{
    m_settings->beginGroup(GROUP_FILTER);
    m_settings->setValue(KEY_LAST_FILTER, filter_id);
    m_settings->setValue(KEY_LAST_FILTER_PARAMETERS, parameters);
    m_settings->endGroup();
}

void Settings_Manager::read_last_filter(QString &filter_id, QString &parameters) noexcept
{
    m_settings->beginGroup(GROUP_FILTER);
    filter_id = m_settings->value(KEY_LAST_FILTER, "").toString();
    parameters = m_settings->value(KEY_LAST_FILTER_PARAMETERS, "").toString();
    m_settings->endGroup();
}

void Settings_Manager::write_signal_view_setting(const QString& episode, const QString& name, const QString& color, int width, int show_state) noexcept
{
    QString name_cleared = name;
    name_cleared = name_cleared.replace(" ", "_").replace(";", "_").replace("(", "_").replace(")", "_").replace("?", "_");

    m_settings->beginGroup("Signal_view_settings");

    m_settings->setValue("signal_color" + episode + name_cleared, color);
    m_settings->setValue("signal_width" + episode + name_cleared, width);
    m_settings->setValue("signal_state" + episode + name_cleared, show_state);

    m_settings->endGroup();
}

void Settings_Manager::read_signal_view_setting(const QString& episode, const QString& name, QString* color, int* width, int* show_state) noexcept
{
    QString name_cleared = name;
    name_cleared = name_cleared.replace(" ", "_").replace(";", "_").replace("(", "_").replace(")", "_").replace("?", "_");

    m_settings->beginGroup("Signal_view_settings");

    *color = m_settings->value("signal_color" + episode + name_cleared, "").toString();
    *width = m_settings->value("signal_width" + episode + name_cleared, 0).toInt();
    *show_state = m_settings->value("signal_state" + episode + name_cleared, 0).toInt();

    m_settings->endGroup();
}

void Settings_Manager::write_window_parameters(const QString& window_tag, const QString& property, const long value) noexcept
{
    m_settings->beginGroup(window_tag);
    m_settings->setValue(property, QVariant::fromValue(value));
    m_settings->endGroup();
}

int Settings_Manager::get_window_parameters(const QString& window_tag, const QString& property) noexcept
{
    m_settings->beginGroup(window_tag);
    int result = m_settings->value(property, 0).toInt();
    m_settings->endGroup();
    return result;
}

void Settings_Manager::write_plot_zoom(int plot_id, int a, int b, int c, int d) noexcept
{
    m_settings->setValue("PLOT_ZOOM_" + QString::number(plot_id), QString::number(a) + "," + QString::number(b) + ","
                         + QString::number(c) + "," + QString::number(d));
}

std::vector<int> Settings_Manager::get_plot_zoom(int plot_id) noexcept
{
    QStringList str_list = m_settings->value("PLOT_ZOOM_" + QString::number(plot_id), "").toString().split(',');
    std::vector<int> result_vector;
    for (auto& str : str_list)
    {
        result_vector.emplace_back(str.toInt());
    }

    return result_vector;
}

void Settings_Manager::write_palette(int palette) noexcept
{
    m_settings->setValue(KEY_PALETTE, palette);
}

int Settings_Manager::get_palette_id() noexcept
{
    return m_settings->value(KEY_PALETTE, 1).toInt();
}

void Settings_Manager::write_is_only_low_freq_flag(int is_only_low_freq) noexcept
{
    m_settings->setValue(KEY_ONLY_LOW_FREQ, is_only_low_freq > 0 ? 1 : 0);
}

int Settings_Manager::get_is_only_low_freq_flag() noexcept
{
    return (m_settings->value(KEY_ONLY_LOW_FREQ, 1).toInt() > 0) ? 1 : 0;
}

void Settings_Manager::write_ids_of_opened_plots(std::vector<int> need_open_ids) noexcept
{
    QString str = "";
    for (int i : need_open_ids)
    {
        str += QString::number(i) + ",";
    }
    m_settings->setValue(KEY_IDS_OPENED_PLOTS, str);
}

std::vector<int> Settings_Manager::get_ids_of_opened_plots() noexcept
{
    std::vector<int> output_ids;
    QString str = m_settings->value(KEY_IDS_OPENED_PLOTS, "").toString();

    for (const QString& s : str.split(','))
    {
        bool is_ok;
        int id = s.toInt(&is_ok);
        if (is_ok)
        {
            output_ids.emplace_back(id);
        }
    }

    return output_ids;
}

inline QString path_to_key(const QString& path)
{
    QString key = path;
    return key.replace("/", "_").replace(" ", "_"); // cannot use string with '\' and ' ' as key
}

void Settings_Manager::write_opened_videos(const QStringList& video_file_names, const QString& log_name, const std::vector<int>& is_full_screens) noexcept
{
    m_settings->remove(path_to_key("video_opened_" + log_name));
    m_settings->beginGroup(path_to_key("video_opened_" + log_name));
    int i = 0;
    for (const QString& str : video_file_names)
    {
        m_settings->setValue(str, is_full_screens.at(i));
        ++i;
    }
    m_settings->endGroup();
}

QStringList Settings_Manager::get_opened_videos(const QString& log_name, std::vector<int>* is_full_screens) noexcept
{
    m_settings->beginGroup(path_to_key("video_opened_" + log_name));
    QStringList result = m_settings->allKeys();
    is_full_screens->resize(result.size());
    int i = 0;
    for (const QString& s : result)
    {
        is_full_screens->at(i) = m_settings->value(s, 0).toInt();
        ++i;
    }
    m_settings->endGroup();
    return result;
}
