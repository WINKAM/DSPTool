#include "file_reader.h"
#include <QTime>
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include "data/data_manager.h"
#include <iostream>
#include "utils.h"
#include <time.h>
#include "operation_mode.h"
#include "data/freqency_cutter.h"

File_Reader::File_Reader(QObject *parent) : QThread(parent)
{
    m_is_run = false;
    m_data_manager = nullptr;
    m_is_labels_file = false;
}

void File_Reader::set_files_name(const QString &log_file_name, bool is_labels_file)
{
    m_file_name = log_file_name;
    m_is_labels_file = is_labels_file;
}

void File_Reader::set_data_manager(Data_Manager *data_manager)
{
    m_data_manager = data_manager;
}

bool File_Reader::execute()
{
    if (!m_is_run)
    {
        m_is_run = true;
        start(NormalPriority);
        return true;
    }
    return false;
}

void File_Reader::run()
{
    if (m_data_manager == nullptr)
    {
        std::cerr << "Data Manager is null" << std::endl;
        emit file_reading_done(false, m_file_name);
        m_is_run = false;
    }

    bool result = false;

    bool is_need_to_create_25_Hz = false;

    QFile file(m_file_name);
    if (file.exists())
    {
        //std::cerr << "RQI: File: " << file.fileName().toLatin1().data() << std::endl;

        if (!m_is_labels_file && Operation_Mode::get_instance().is_only_low_freq_opening())
        {
            QStringList tmp = file.fileName().split("/");
            tmp[tmp.size() - 1] = ("temp_" + tmp[tmp.size() - 1]);
            QStringList f_name_list = tmp[tmp.size() - 1].split("_");
            f_name_list[f_name_list.size() - 1] = "25";
            tmp[tmp.size() - 1] = f_name_list.join("_");// tmp[tmp.size() - 1].remove(".csv");
            QFile file_25Hz(tmp.join("/"));//.replace("Location120_", "Location25_").replace("Logs sync", "Temporary Logs sync 25Hz"));

            if (file_25Hz.exists())
            {
                file.setFileName(file_25Hz.fileName());
            }
            else
            {
                is_need_to_create_25_Hz = true;
            }
        }

        if (file.open(QIODevice::ReadOnly))
        {
            QTextStream input_stream(&file);
            QString line;

            bool is_parser_indetify = false;
            if (m_is_labels_file)
            {
                is_parser_indetify = true;
                m_data_manager->prepare_label_parser(m_file_name);
            }
            else
            {
                qDebug() << "read" << file.fileName();

                while (!input_stream.atEnd() && m_is_run)
                {
                    line = input_stream.readLine();
                    is_parser_indetify = identify_parser_type(line);
                    if (is_parser_indetify)
                    {
                        break;
                    }
                }
            }

            if (is_parser_indetify)
            {
                m_data_manager->get_log_data()->set_data_set_id(UTILS::get_data_set_file_id(file.fileName()));
                Operation_Mode::get_instance().set_episode_name(file.fileName());
                input_stream.seek(0);
                //                clock_t start = clock();
                while (!input_stream.atEnd() && m_is_run)
                {
                    line = input_stream.readLine();
                    {
                        m_data_manager->parse_line(line);
                    }
                }

                //                clock_t end = clock();
                //                double seconds = (double)(end - start) / CLOCKS_PER_SEC;
                //                qDebug() << seconds << "!!!!!";

                if (m_is_run)
                {
                    result = true;
                }
            }
            else
            {
                is_need_to_create_25_Hz = false;
            }
            file.close();
        }
        else
        {
            std::cout << "Can't open file" << std::endl;
        }
    }

    if (is_need_to_create_25_Hz)
    {
        QStringList tmp = file.fileName().split("/");
        tmp[tmp.size() - 1] = ("temp_" + tmp[tmp.size() - 1]);
        QStringList f_name_list = tmp[tmp.size() - 1].split("_");
        f_name_list[f_name_list.size() - 1] = "25";
        tmp[tmp.size() - 1] = f_name_list.join("_");// tmp[tmp.size() - 1].remove(".csv");
        QFile file_25Hz(tmp.join("/"));//.replace("Location120_", "Location25_").replace("Logs sync", "Temporary Logs sync 25Hz"));
        qDebug() << "create 25 Hz file" << file_25Hz.fileName();
        Freqency_Cutter::get_instance().write_lower_freq(&file_25Hz, 25, m_data_manager);
    }

    emit file_reading_done(result, m_file_name);

    m_is_run = false;
}

bool File_Reader::identify_parser_type(const QString& line) noexcept
{
    if (line.contains("#LP_"))
    {
        m_data_manager->prepare_log_parser(m_file_name, Data_Manager::LOG_PARSER_TYPE::LOG_ALPS);
        return true;
    }

    if (line.contains("#MONTBLANC"))
    {
        m_data_manager->prepare_log_parser(m_file_name, Data_Manager::LOG_PARSER_TYPE::LOG_MB);
        return true;
    }

    if (line.contains("#") || line.contains("name"))
    {
        return false;
    }

    if (line.contains(";"))
    {
        QStringList separated_line = line.split(";");
        if (separated_line.size() == 5)// && (line.contains("A;") || line.contains("G;") || line.contains("M;")))
        {
            // wk sensors and wk log collector
            //qDebug() << "WK";
            m_data_manager->prepare_log_parser(m_file_name, Data_Manager::LOG_PARSER_TYPE::LOG_WK);
            return true;
        }
    }
    else
    {
        QStringList separated_line = line.split(",");
        if (separated_line.size() == 9 && (separated_line.at(0).toLongLong() > 1000000000000))
        {
            // "Session_N_n_n.csv"  file 25 Hz
            //m_data_manager->prepare_log_parser(m_file_name, Data_Manager::LOG_PARSER_TYPE::LOG_APL_25HZ);
            return false;//true;
        }
        else if (separated_line.size() == 6)
        {
            //qDebug() << "120";

            // "Location120_N_n.csv" or "Apl120_N_n.csv" file 120 Hz
            m_data_manager->prepare_log_parser(m_file_name, Data_Manager::LOG_PARSER_TYPE::LOG_APL_120HZ);
            return true;
        }
    }

    return false;
}

bool File_Reader::is_runnning()
{
    return m_is_run;
}

void File_Reader::cancel()
{
    m_is_run = false;
}

File_Reader::~File_Reader()
{

}
