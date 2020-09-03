#include "file_writer.h"
#include <QTime>
#include <QDebug>
#include <QFile>
#include "data/data_manager.h"

File_Writer::File_Writer(QObject *parent) : QThread(parent)
{
    m_is_run = false;
    m_data_manager = nullptr;
}

void File_Writer::set_files_name(const QString &log_file_name)
{
    m_file_name = log_file_name;
}

void File_Writer::set_data_manager(Data_Manager *data_manager)
{
    m_data_manager = data_manager;
}

bool File_Writer::execute()
{
    if (!m_is_run)
    {
        m_is_run = true;
        start(NormalPriority);
        return true;
    }
    return false;
}

void File_Writer::run()
{
    QTime time;
    time.start();

    QFile file(m_file_name);
    if (file.exists())
    {

        if (file.open(QIODevice::WriteOnly))
        {
            QTextStream output_stream(&file);

            qDebug() << "start writing: " << file.fileName();

            m_data_manager->write_labels(output_stream);

            file.close();

            qDebug() << "write time = " << time.elapsed();
        }


        emit file_writing_done(true, m_file_name);
    }
    else
    {
        emit file_writing_done(false, m_file_name);
    }

    m_is_run = false;
}

bool File_Writer::is_runnning()
{
    return m_is_run;
}

void File_Writer::cancel()
{
    m_is_run = false;
}

File_Writer::~File_Writer()
{

}
