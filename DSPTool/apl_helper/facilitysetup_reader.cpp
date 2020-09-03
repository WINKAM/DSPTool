#include "facilitysetup_reader.h"
#include <QTime>
#include <QDebug>
#include <QFile>
#include "data/data_manager.h"
#include <iostream>
#include "utils.h"
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>

Facilitysetup_Reader::Facilitysetup_Reader(QObject *parent) : QThread(parent)
{
    m_is_run = false;
    m_data_manager = nullptr;
}

void Facilitysetup_Reader::set_files_name(const QString &log_file_name)
{
    m_file_name = log_file_name;
}

void Facilitysetup_Reader::set_data_manager(Data_Manager *data_manager)
{
    m_data_manager = data_manager;
}

bool Facilitysetup_Reader::execute()
{
    if (!m_is_run)
    {
        m_is_run = true;
        start(NormalPriority);
        return true;
    }
    return false;
}

void Facilitysetup_Reader::run()
{
    if (m_data_manager == nullptr)
    {
        std::cout << "Data Manager is null" << std::endl;
        emit file_reading_done(false, m_file_name);
        m_is_run = false;
    }

    QFile file(m_file_name);
    if (file.exists())
    {        

        if (file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            m_data_manager->clear_hoops();
            QByteArray save_data = file.readAll();
            QJsonDocument load_doc(QJsonDocument::fromJson(save_data));
            QJsonObject top_json_object = load_doc.object();
            QJsonArray courts_json_array = top_json_object["courts"].toArray();

            if (courts_json_array.size() > 0)
            {
                QJsonObject court_json_object = courts_json_array[0].toObject();
                QJsonArray hoops_json_array = court_json_object["hoops"].toArray();

                for (int i = 0; i < hoops_json_array.size(); ++i)
                {
                    QJsonObject hoop_json_object = hoops_json_array[i].toObject();
                    QStringList hoop_name_list = hoop_json_object["name"].toString().split(" ");
                    if (hoop_name_list.size() == 2)
                    {
                        int hoop_id = hoop_name_list.at(1).toInt();
                        Hoop* hoop = new Hoop;
                        hoop->m_x = hoop_json_object["x"].toInt() + 1920;
                        hoop->m_y = hoop_json_object["y"].toInt() + 40;
                        hoop->m_z = hoop_json_object["z"].toInt();

                        m_data_manager->add_hoop(hoop_id, hoop);
                    }
                }
            }

            file.close();

            emit file_reading_done(true, m_file_name);
        }
        else
        {
            std::cout << "Can't open file" << std::endl;
            emit file_reading_done(false, m_file_name);
        }
    }
    else
    {
        emit file_reading_done(false, m_file_name);
    }

    m_is_run = false;
}

bool Facilitysetup_Reader::is_runnning()
{
    return m_is_run;
}

void Facilitysetup_Reader::cancel()
{
    m_is_run = false;
}

Facilitysetup_Reader::~Facilitysetup_Reader()
{

}
