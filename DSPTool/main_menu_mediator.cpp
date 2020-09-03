/***************************************************************************

             WINKAM TM strictly confidential 13.05.2020

 ***************************************************************************/
#include "main_menu_mediator.h"
#include <QDebug>
#include <QMenu>
#include <QMenuBar>
#include <QAction>
#include <QMessageBox>

#include "main_window.h"
#include "palette_manager.h"
#include "operation_mode.h"
#include "help_widget.h"
#include "settings_manager.h"
#include "data/episode_meta_data.h"
#include "label_mediator.h"

Main_Menu_Mediator::Main_Menu_Mediator(QObject *parent) : QObject(parent)
{
    m_ptr_main_window = nullptr;
    Operation_Mode::get_instance().set_only_low_freq_opening(Settings_Manager::get_instance()->get_is_only_low_freq_flag());
    if (Operation_Mode::get_instance().is_only_low_freq_opening())
    {
        qDebug() << "working with 25 Hz is enabled";
    }
    else
    {
        qDebug() << "working with 25 Hz is disabled";
    }

    if (Palette_Manager::get_palette_id() == 0)
    {
        qDebug() << "white theme";
    }
    else
    {
        qDebug() << "dark theme";
    }

    m_help_widget = new Help_Widget();
    m_help_widget->setWindowModality(Qt::ApplicationModal);
}

void Main_Menu_Mediator::set_main_window(Main_Window* ptr_main_window) noexcept
{
    m_ptr_main_window = ptr_main_window;
}

QMenuBar* Main_Menu_Mediator::create_menu_bar(QWidget* parent) const noexcept
{
    // FILE

    QMenu *file_menu = new QMenu(tr("&File"), parent);

    QMenuBar* menubar = new QMenuBar(parent);
    QAction *open_act = new QAction(tr("&Open..."), parent);
    open_act->setShortcut(tr("Ctrl+O"));
    QObject::connect(open_act, &QAction::triggered, [=] ()
    {
        if (!Operation_Mode::get_instance().is_reading_data())
        {
            m_ptr_main_window->open_log_file_signal("", true, true);
        }
    });

//    QAction *reopen_meta_act = new QAction(tr("Reopen meta-file"), parent);
//    reopen_meta_act->setShortcut(tr("Ctrl+M"));
//    QObject::connect(reopen_meta_act, &QAction::triggered, [=] ()
//    {
//        Episode_Meta_Data::get_instance().find_read_meta_file(Operation_Mode::get_instance().get_log_name());
//        m_ptr_main_window->set_default_signals_view();
//    });

//    QAction *open_dir_act = new QAction(tr("Open &Dir"), parent);
//    open_dir_act->setShortcut(tr("Ctrl+D"));
//    QObject::connect(open_dir_act, &QAction::triggered, [=] ()
//    {
//        if (!Operation_Mode::get_instance().is_reading_data())
//        {
//            m_ptr_main_window->open_dir_signal();
//        }
//    });

    QAction *open_labels_act = new QAction(tr("Open &label"), parent);
    open_labels_act->setShortcut(tr("Ctrl+L"));
    QObject::connect(open_labels_act, &QAction::triggered, [=] ()
    {
        m_ptr_main_window->open_label_file_signal();
    });

    QAction *open_meta_file_act = new QAction(tr("Open &meta-file"), parent);
    open_meta_file_act->setShortcut(tr("Ctrl+M"));
    QObject::connect(open_meta_file_act, &QAction::triggered, [=] ()
    {
        m_ptr_main_window->open_meta_file_signal();
    });


//    QAction *save_act = new QAction(tr("&Save Labels"), parent);
//    save_act->setShortcut(tr("Ctrl+S"));
//    QObject::connect(save_act, &QAction::triggered, [=] ()
//    {
//        m_ptr_main_window->save_label_file_signal();
//    });

//    QAction *open_errors_act = new QAction(tr("Open E&rrors"), parent);
//    open_errors_act->setShortcut(tr("Ctrl+R"));
//    QObject::connect(open_errors_act, &QAction::triggered, [=] ()
//    {
//        m_ptr_main_window->open_error_file_signal();
//    });

    QAction *exit_act = new QAction(tr("E&xit"), parent);
    exit_act->setShortcut(tr("Ctrl+X"));
    QObject::connect(exit_act, &QAction::triggered, m_ptr_main_window, &Main_Window::close);

    QAction *open_video_act = new QAction(tr("Open &Video"), parent);
    open_video_act->setShortcut(tr("Ctrl+V"));
    QObject::connect(open_video_act, &QAction::triggered, [=] ()
    {
        m_ptr_main_window->open_video_signal();
    });


    file_menu->addAction(open_act);
    //file_menu->addAction(reopen_meta_act);

    file_menu->addAction(open_labels_act);
    file_menu->addAction(open_meta_file_act);
    //file_menu->addAction(open_errors_act);
    file_menu->addAction(open_video_act);
    //file_menu->addAction(open_dir_act);
    file_menu->addSeparator();
    //file_menu->addAction(save_act);
    file_menu->addSeparator();
    file_menu->addAction(exit_act);

    // PLOTS
    QMenu *view_menu = new QMenu(tr("&Plots"), parent);

    QAction *update_signals_view_act = new QAction(tr("Apply signal settings and refresh plots"), parent);
    update_signals_view_act->setShortcut(tr("F5"));
    QObject::connect(update_signals_view_act, &QAction::triggered, [this] ()
    {
        m_ptr_main_window->update_view_settings();
    });

//    QAction *hide_controls_window = new QAction(tr("Hide controls window"), parent);
//    hide_controls_window->setShortcut(tr("Ctrl+Alt"));
//    QObject::connect(hide_controls_window, &QAction::triggered, [this] ()
//    {
//        m_ptr_main_window->clearFocus();
//    });


    view_menu->addAction(update_signals_view_act);
    //view_menu->addAction(hide_controls_window);

    // SETTING
    QMenu *settings_menu = new QMenu(tr("&Settings"), parent);

    QAction *is_dark_pallete_act = new QAction(tr("Dark theme"), parent);
    is_dark_pallete_act->setCheckable(true);
    is_dark_pallete_act->setChecked(Palette_Manager::get_palette_id() == 1);

    QObject::connect(is_dark_pallete_act, &QAction::triggered, [] (bool is_checked)
    {
        if (is_checked)
        {
            qDebug() << "set dark theme";
            Palette_Manager::set_dark_palette();
        }
        else
        {
            qDebug() << "set white theme";
            Palette_Manager::set_light_palette();
        }
    });

    QAction *is_only_low_freq_act = new QAction(tr("Work with 25 Hz signals"), parent);
    is_only_low_freq_act->setCheckable(true);
    is_only_low_freq_act->setChecked(Operation_Mode::get_instance().is_only_low_freq_opening());

    QObject::connect(is_only_low_freq_act, &QAction::triggered, [this] (bool is_checked)
    {
        if(Operation_Mode::get_instance().is_reading_data())
        {
            return;
        }

        if (!Operation_Mode::get_instance().get_log_name().isEmpty())
        {
            QMessageBox msgBox;
            //msgBox.setText("The document has been modified.");
            msgBox.setText("Apply new settings and reopen the episode?");
            msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
            msgBox.setDefaultButton(QMessageBox::Yes);
            if (msgBox.exec() == QMessageBox::Yes)
            {
                if (is_checked)
                {
                    qDebug() << "enable working with 25 Hz signals";
                }
                else
                {
                    qDebug() << "disable working with 25 Hz signals";
                }

                Operation_Mode::get_instance().set_only_low_freq_opening(is_checked);
                QString log_name = Operation_Mode::get_instance().get_log_name();
                QStringList tmp = log_name.split("/");
                if (tmp.back().indexOf("temp_") == 0)
                {
                    // need to get original 120 Hz episode name
                    QStringList tmp2 = tmp.back().split("_");
                    tmp2.removeFirst();
                    tmp2.removeLast();
                    tmp2.append("120.csv");
                    tmp[tmp.size() - 1] = tmp2.join("_");
                    log_name = tmp.join("/");
                }

                m_ptr_main_window->open_log_file_signal(log_name, true, true);
            }
        }
        else
        {
            Operation_Mode::get_instance().set_only_low_freq_opening(is_checked);
            if (is_checked)
            {
                qDebug() << "enable working with 25 Hz signals";
            }
            else
            {
                qDebug() << "disable working with 25 Hz signals";
            }
        }
    });



    settings_menu->addAction(is_only_low_freq_act);
    settings_menu->addAction(is_dark_pallete_act);

    connect(settings_menu, &QMenu::aboutToShow, [is_only_low_freq_act, is_dark_pallete_act]()
    {
        is_only_low_freq_act->setChecked(Operation_Mode::get_instance().is_only_low_freq_opening());
        is_dark_pallete_act->setChecked(Palette_Manager::get_palette_id());
    });

    // HELP
    QMenu *help_menu = new QMenu(tr("&Help"));

    QAction* help_act = new QAction(tr("&Help"), parent);
    help_act->setShortcut(Qt::Key_F1);
    QObject::connect(help_act, &QAction::triggered, [=] ()
    {
        m_help_widget->show();
//        QString app_name = "DSP Tool ";
//        QString message = app_name.append(APP_VERSION).append(
//                          "<p>F1 - Подсказка по горячим клавишам и версии программы</p>"
//                          "<p>Ctrl + O - Открыть файл с семплом. Если есть файл с разметкой - он открыватеся автоматически</p>"
//                          "<p>Ctrl + L - Открыть файл с разметкой. Открывается при условии, что файл с семплом уже открыт</p>"
//                          "<p>Ctrl + V - Открыть видеофайл. В текущей версии поддержка только одного видео</p>"
//                          "<p>Ctrl + E - Горячая клавиша для кнопки Update</p>"
//                    "<a href='http://winkam.com'>WINKAM TM</a>");
//        QMessageBox::about(parent, m_ptr_main_window->windowTitle(), message);
    });

    QAction* about_qt_act = new QAction(tr("About Qt"), parent);
    QObject::connect(about_qt_act, &QAction::triggered, [=] ()
    {
       QMessageBox::aboutQt(parent);
    });

    QAction* about_qwt_act = new QAction(tr("About Qwt"), parent);
    QObject::connect(about_qwt_act, &QAction::triggered, [=] ()
    {
        QMessageBox::about(parent, "Qwt 6.1.4", "<p>Qwt is an extension to the libraries of the Qt Project.  The Qwt library contains widgets and components which are primarily useful for technical and scientific purposes. It includes a 2-D plotting widget, different kinds of sliders, and much more.</p>"
                                              "<a href='https://qwt.sourceforge.io/'>qwt.sourceforge.io</a>");
    });

    QAction* about_opencv_act = new QAction(tr("About OpenCV"), parent);
    QObject::connect(about_opencv_act, &QAction::triggered, [=] ()
    {
        QMessageBox::about(parent, "OpenCV 4.2", "<p>OpenCV is a highly optimized library with focus on real-time applications.</p>"
                                              "<a href='https://opencv.org/'>opencv.org</a>");
    });

    QAction* about_ffmpeg_act = new QAction(tr("About FFmpeg"), parent);
    QObject::connect(about_ffmpeg_act, &QAction::triggered, [=] ()
    {
        QMessageBox::about(parent, "FFmpeg", "<p>FFmpeg is a complete, cross-platform solution to record, convert and stream audio and video.</p>"
                                              "<a href='https://www.ffmpeg.org/'>ffmpeg.org</a>");
    });


    help_menu->addAction(help_act);
    help_menu->addAction(about_qt_act);
    help_menu->addAction(about_qwt_act);
    help_menu->addAction(about_opencv_act);
    help_menu->addAction(about_ffmpeg_act);

    menubar->addMenu(file_menu);
    menubar->addMenu(view_menu);
    menubar->addMenu(settings_menu);
    menubar->addMenu(help_menu);

    menubar->setVisible(false);
    menubar->setVisible(true);


    return menubar;
}
