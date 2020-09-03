#ifndef UTILS_H
#define UTILS_H

#include <cstdint>

class QString;
class QStringList;

namespace UTILS
{
    QString get_label_name_file(const QString &log_name_file) noexcept;
    QString get_data_set_dir_name(const QString &log_name_file) noexcept;

    QString get_facility_file_path(const QString &log_name_file) noexcept;

    int get_data_set_file_id(const QString &log_name_file) noexcept;
    QStringList get_log_files_in_dir(const QString &dir_name) noexcept;
    QString time_to_string(int64_t time) noexcept;
}


#endif // UTILS_H
