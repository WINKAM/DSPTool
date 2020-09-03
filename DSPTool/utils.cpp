#include "utils.h"
#include <QString>
#include <QStringList>
#include <QFile>
#include <QDir>
#include <QFileInfoList>
#include <QFileInfo>
#include <QDebug>

#include "data/episode_meta_data.h"

namespace UTILS
{


QString get_label_name_file(const QString &log_name_file) noexcept
{

    QString result;

    QStringList path_list = log_name_file.split("/");
    if (path_list.size() > 0)
    {
        QString log_name = path_list.back();
        path_list.pop_back();
        path_list.pop_back();

        result = path_list.join("/");
        result.append("/Labels/");

/*        if (log_name_file.contains("WK"))
        {
            QDir current_dir(result);
            current_dir.setNameFilters(QStringList() << "*.csv");
            QStringList file_list = current_dir.entryList();
            if (file_list.size() > 0)
            {
                for (const QString& item_file : file_list)
                {
                    if (item_file.contains("VJ"))
                    {
                        result.append(item_file);
                    }
                }
            }
        }
        else if (log_name_file.toLower().contains("wgame") || log_name_file.toLower().contains("gamepad"))
        {
            QDir current_dir(result);
            current_dir.setNameFilters(QStringList() << "*.csv");
            QStringList file_list = current_dir.entryList();
            if (file_list.size() > 0)
            {
                for (const QString& item_file : file_list)
                {
                    if (item_file.contains(".csv"))
                    {
                        result.append(item_file);
                        return result;
                    }
                }
            }
        }
        else*/
        {
            QStringList name_list = log_name.split("_");
            if (name_list.size() > 2)
            {
                result.append("Location_");
                result.append(name_list.at(1));
                QStringList tmp = name_list.at(2).split(".");
                if (tmp.size() > 0)
                {
                    result.append("_");
                    result.append(tmp.at(0));
                }
                result.append("_labels");
            }
        }
    }

    return result;
}

QString get_data_set_dir_name(const QString &log_name_file) noexcept
{
    QString result;

    QStringList path_list = log_name_file.split("/");
    if (path_list.size() > 0)
    {
        for (const QString &it : path_list)
        {
            if (it.contains("Data set") || it.contains("Episode"))
            {
                result = it;
                break;
            }
        }
    }
    return result;
}

QString get_facility_file_path(const QString &log_name_file) noexcept
{
    int data_set_id = get_data_set_file_id(log_name_file);
    QString result = QDir::currentPath() + "/backup_facility/facilitysetup_ds" + QString::number(data_set_id) + ".json";
    if (QFile(result).exists())
    {
        return result;
    }
    return QDir::currentPath() + "/backup_facility/facilitysetup_ds0.json";
}

int get_data_set_file_id(const QString &log_name_file) noexcept
{
    int result = 0;
    QStringList path_list = log_name_file.split("/");
    if (path_list.size() > 0)
    {
        QString log_name = path_list.back();

        QStringList name_list = log_name.split("_");
        if (name_list.size() > 1)
        {
            QString data_set_id = name_list.at(1);
            result = data_set_id.toInt();
        }
    }
    return result;
}

QStringList get_log_files_in_dir(const QString &dir_name) noexcept
{
    qDebug() << "In Dir" << dir_name << ":";
    QStringList result;
    QDir dir(dir_name);
    if (dir.exists())
    {
        dir.setFilter(QDir::Dirs | QDir::NoSymLinks);
        dir.setSorting(QDir::Name | QDir::Reversed);

        QStringList list_set = dir.entryList();
        for (const QString &it : list_set)
        {
            QDir sub_dir(dir.path() + "/" + it);
            sub_dir.setFilter(QDir::Dirs | QDir::NoSymLinks);
            sub_dir.setSorting(QDir::Name);
            QStringList list_log_types = sub_dir.entryList();

            for (const QString &itt : list_log_types)
            {
                if (itt.contains("Logs sync"))
                {
                    QDir log_dir(sub_dir.path() + "/" + itt);
                    log_dir.setFilter(QDir::Files | QDir::NoSymLinks);
                    log_dir.setSorting(QDir::Name);
                    QFileInfoList list_files = log_dir.entryInfoList();
                    for (const QFileInfo &file_info : list_files)
                    {
                        if (file_info.fileName().contains(".csv"))// && file_info.fileName().contains("Location120"))
                        {
                            result << file_info.filePath();
                            qDebug() << file_info.filePath();
                        }
                    }
                }
            }
        }
    }
    return result;
}

QString add_zeros_to_time(uint64_t time, bool is_ms = false) noexcept
{
    if (!is_ms)
    {
        if (time >= 10)
        {
            return QString::number(time);
        }
        else
        {
            return "0" + QString::number(time);
        }
    }
    else
    {
        QString res = QString::number(time);

        while(res.length() != 3)
        {
            res.push_front("0");
        }

        return res;
    }
}

QString time_to_string(int64_t time) noexcept
{
    time -= Episode_Meta_Data::get_instance().get_start_shift();

    uint64_t tmp_time = (time >= 0 ? time : -time);

    uint64_t time_ms_res = tmp_time % 1000; //ms without sec
    uint64_t time_sec = tmp_time / 1000;
    uint64_t time_sec_res = time_sec % 60;
    uint64_t time_min = time_sec / 60;
    uint64_t time_min_res = time_min % 60;
    uint64_t time_hour_res = time_min / 60;

    QString time_res = (time >= 0 ? "" : "-")
            + add_zeros_to_time(time_hour_res) + ":" + add_zeros_to_time(time_min_res)
            + ":" + add_zeros_to_time(time_sec_res) + "." + add_zeros_to_time(time_ms_res, true);

    return time_res;
}

}
