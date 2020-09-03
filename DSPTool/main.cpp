#include "main_window.h"
#include <QApplication>
#include <QDebug>
#include <QStyleFactory>
#include <QThread>

#include "app_controller.h"
#include "palette_manager.h"
#include "settings_manager.h"
#include "player_widget.h"
#include "time_mediator.h"
#include "metronom.h"
#include <iostream>

void print_help();

int main(int argc, char *argv[])
{        
    qDebug().nospace() << "DSP_Tool v." << APP_VERSION;

    bool is_cmd = false;
    bool is_file = false;
    char *path = nullptr;
    char *filter = nullptr;
    char *parameters = nullptr;
    char *hoop1_config = nullptr;
    char *hoop2_config = nullptr;
    int benchmark_id = 1;

    if (argc > 1)
    {
        for (int i = 1; i < argc; ++i)
        {
            std::string flag(argv[i]);

            if (flag.compare("-c") == 0 || flag.compare("--cmd") == 0)
            {
                is_cmd = true;
            }
            else if (flag.compare("-v") == 0 || flag.compare("--version") == 0)
            {
                std::cout << "Version 4.7.0" << std::endl;
                return 0;
            }
            else if (flag.compare("-h") == 0 || flag.compare("--help") == 0)
            {
                print_help();
                return 0;
            }

            if (i + 1 != argc)
            {
                if (flag.compare("--dir") == 0)
                {
                    is_file = false;
                    path = argv[i + 1];
                }
                if (flag.compare("--file") == 0)
                {
                    is_file = true;
                    path = argv[i + 1];
                }
                else if (flag == "--filter")
                {
                    filter = argv[i + 1];
                }
                else if (flag == "--parameters")
                {
                    parameters = argv[i + 1];
                }
                else if (flag == "--hoop1")
                {
                    hoop1_config = argv[i + 1];
                }
                else if (flag == "--hoop2")
                {
                    hoop2_config = argv[i + 1];
                }
                else if (flag.compare("-b") == 0 || flag.compare("--benchmark") == 0)
                {
                    benchmark_id = QString(argv[i + 1]).toInt();
                }
            }
        }
    }

    App_Controller app;
    if (is_cmd)
    {
        std::cout << "RQI Run: ";
        for (int i = 1; i < argc; ++i)
        {
            std::cout << argv[i] << " ";
        }
        std::cout << std::endl;
        QCoreApplication a(argc, argv);
        app.set_parameters(benchmark_id, filter, parameters);
        app.set_hoops_config(hoop1_config, hoop2_config);
        app.start_cmd(path, is_file);
        return a.exec();
    }
    else
    {
        qApp->setStyle(QStyleFactory::create("Fusion"));
        Palette_Manager::set_palette_by_id(Settings_Manager::get_instance()->get_palette_id());

        QApplication a(argc, argv);

        a.clipboard();

        QThread metronom_thread;
        Metronom metronom;
        metronom.setObjectName("metronom");
        metronom.moveToThread(&metronom_thread);

        QObject::connect(&metronom, &Metronom::next_step_got,  &Time_Mediator::get_instance(), &Time_Mediator::change_timestamp_by_metronom);

        QObject::connect(&metronom_thread, &QThread::started,  &metronom, &Metronom::run);
        QObject::connect(&metronom_thread, &QThread::finished, &metronom, &Metronom::release);

        Time_Mediator::get_instance().set_metronom_ptr(&metronom);

        metronom_thread.start();

        app.set_q_application(&a);
        app.start_gui();

        int ouput = a.exec();

        qDebug() << "DSPTool is closing...";

        metronom.release();
        QThread::msleep(1000);

        metronom_thread.quit();

        QThread::msleep(1000);

        qDebug() << "closed.";

        return ouput;
    }

}

void print_help()
{
    std::cout <<
                 "Commands:" << std::endl <<
                 "-h | --help                     Print this help" << std::endl <<
                 "-v | --version                  Print the tool version" << std::endl <<
                 "-c | --command                  Use only command line" << std::endl <<
                 "--file [\"path\"]                 Set path to a file (*.csv)" << std::endl <<
                 "--dir [\"path\"]                  Set path to a dir" << std::endl <<
                 "--filter [\"string\"]             Set filter name" << std::endl <<
                 "--parameters [\"string\"]         Set filter parameters" << std::endl <<
                 "--hoop1 [\"string\"]               Set hoop 1 shift: \"X Y Z\"" << std::endl <<
                 "--hoop2 [\"string\"]               Set hoop 2 shift: \"X Y Z\"" << std::endl <<
                 "-b | --benchmark [id]           Set benchbark id" << std::endl;
}
