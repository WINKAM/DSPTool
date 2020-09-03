#include "wrap_apl_banchmark.h"

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QString>
#include <QTime>
#include <QProcess>
#include "utils.h"
#include <iostream>

Wrap_Apl_Banchmark::Wrap_Apl_Banchmark()
{
    m_benchmark_id = 1;
    m_dataset_id = 0;
    m_hoop1_config = nullptr;
    m_hoop2_config = nullptr;
}

void Wrap_Apl_Banchmark::initialize(int benchmark_id, int dataset_id) noexcept
{
    m_benchmark_id = benchmark_id;
    m_dataset_id = dataset_id;
}

void Wrap_Apl_Banchmark::set_hoop1_config(QString* config) noexcept
{
    m_hoop1_config = config;
}

void Wrap_Apl_Banchmark::set_hoop2_config(QString* config) noexcept
{
    m_hoop2_config = config;
}

bool Wrap_Apl_Banchmark::prepare() noexcept
{
    QString benchmark_path = prepare_benchmark();
    if (benchmark_path.length() == 0)
    {
        return false;
    }

    if (!QDir::setCurrent(benchmark_path))
    {
        qDebug() << "Could not change the current working directory!";
        return false;
    }
    return true;
}

QString Wrap_Apl_Banchmark::prepare_benchmark() noexcept
{
    QString benchmark_path = "./benchmarks/" + QString::number(m_benchmark_id);

    QDir benchmark_dir(benchmark_path);
    if (!benchmark_dir.exists())
    {
        qDebug() << "There are no benchmarks!" << benchmark_path;
        return "";
    }

    QFile file (benchmark_path + "/eventbuffer.csv");
    file.remove();

    prepare_facility_file(benchmark_path);

    return benchmark_path;
}

void Wrap_Apl_Banchmark::prepare_facility_file(const QString &benchmark_path) noexcept
{
    QFile config_file(benchmark_path + "/facilitysetup.json");
    if (config_file.exists())
    {
        config_file.remove();
    }

    config_file.setFileName("./backup_facility/facilitysetup_ds" + QString::number(m_dataset_id) + ".json");
    if (!config_file.exists())
    {
        config_file.setFileName("./backup_facility/facilitysetup_ds0.json");
    }
    config_file.copy(benchmark_path + "/facilitysetup.json");

    if (m_hoop1_config != nullptr)
    {
        qDebug() << "hoop 1 editing" << *m_hoop1_config;
        change_hoop_config(benchmark_path + "/facilitysetup.json", 1, *m_hoop1_config);
    }

    if (m_hoop2_config != nullptr)
    {
        qDebug() << "hoop 2 editing" << *m_hoop2_config;
        change_hoop_config(benchmark_path + "/facilitysetup.json", 2, *m_hoop2_config);
    }
}

void Wrap_Apl_Banchmark::change_hoop_config(const QString &path, int hoop_id, const QString &config) noexcept
{
    QStringList hoop_config_list = config.split(" ");
    if (hoop_config_list.size() != 3)
    {
        return;
    }

    int x_shift = hoop_config_list.at(0).toInt();
    int y_shift = hoop_config_list.at(1).toInt();
    int z_shift = hoop_config_list.at(2).toInt();

    QFile edit_file(path);

    if (edit_file.open(QIODevice::ReadWrite))// open for read and write
    {
        QString result_text;
        QTextStream input_stream(&edit_file);
        QString line;
        while (!input_stream.atEnd())
        {
            line = input_stream.readLine();
            if (hoop_id == 1 && line.contains("Hoop 1"))
            {
                result_text.append(line);
                result_text.append("\n");
                result_text.append(get_new_hoop_parameter(input_stream.readLine(), x_shift));
                result_text.append("\n");
                result_text.append(get_new_hoop_parameter(input_stream.readLine(), y_shift));
                result_text.append("\n");
                result_text.append(get_new_hoop_parameter(input_stream.readLine(), z_shift));
                result_text.append("\n");
            }
            else if (hoop_id == 2 && line.contains("Hoop 2"))
            {
                result_text.append(line);
                result_text.append("\n");
                result_text.append(get_new_hoop_parameter(input_stream.readLine(), x_shift));
                result_text.append("\n");
                result_text.append(get_new_hoop_parameter(input_stream.readLine(), y_shift));
                result_text.append("\n");
                result_text.append(get_new_hoop_parameter(input_stream.readLine(), z_shift));
                result_text.append("\n");
            }
            else
            {
                result_text.append(line);
                result_text.append("\n");
            }
        }
        qDebug() << "facility file end editing";
        edit_file.seek(0); // go to the beginning of the file
        edit_file.write(result_text.toLatin1()); // write the new text back to the file

        edit_file.close(); // close the file handle.
    }
}

QString Wrap_Apl_Banchmark::get_new_hoop_parameter(QString old_parameter, int shift) noexcept
{
    QString result = old_parameter;
    QStringList parameters_list = old_parameter.split(" : ");
    if (parameters_list.size() == 2 &&
            (parameters_list.at(0).contains("x") || parameters_list.at(0).contains("y") || parameters_list.at(0).contains("z")))
    {
        QString old_value_string = parameters_list.at(1);
        int old_value = old_value_string.replace(",", "").toInt();
        int new_value = old_value + shift;
        result.replace(QString::number(old_value), QString::number(new_value));
    }

    return result;
}

QString Wrap_Apl_Banchmark::execute(QString &filter_id) noexcept
{
    if (filter_id.indexOf("RQI") == 0)
    {
        qDebug() << "RQI without APL benchmark!";
        return "./eventbuffer.csv";
    }
    else
    {
        QTime time;
        time.start();

        if (filter_id.compare("\u03b2 aplf") != 0)
        {
            qDebug() << "WK";
            QProcess::execute("./stlocserver", {"--WINKAMFILE=stlocation.csv"});
        }
        else
        {
            qDebug() << "APL";
            QProcess::execute("./stlocserver", {});
        }

        std::cout << "RQI Stlocserver time = " << UTILS::time_to_string(time.elapsed()).toLatin1().data() << std::endl;

        delete_tmp_files();

        QDir eventbuffers("./eventbuffers");
        if (!eventbuffers.exists())
        {
            eventbuffers.mkpath(".");
        }

        QFile eventbuffer_file ("./eventbuffer.csv");
        if (eventbuffer_file.exists())
        {
            QString backup_name = "./eventbuffers/eventbuffer_";

            if (filter_id.length() > 0)
            {
                backup_name.append(filter_id.replace(" ", "_"));
                backup_name.append("_");
            }
            backup_name.append(QDateTime::currentDateTime().toString("yyyy-MM-dd_hh-mm-ss"));
            backup_name.append(".csv");

            if (!eventbuffer_file.copy(backup_name))
            {
                return "./eventbuffers.csv";
            }
            eventbuffer_file.remove();
            return backup_name;
        }
    }
    return "";
}

void Wrap_Apl_Banchmark::delete_tmp_files() noexcept
{
    QFile config_file("./facilitysetup.json");
    if (config_file.exists())
    {
        config_file.remove();
    }

    QFile log_file("./stlocation.csv");
    if (log_file.exists())
    {
        log_file.remove();
    }
}
