/***************************************************************************

             WINKAM TM strictly confidential 19.05.2020

 ***************************************************************************/
#include "video_windows_mediator.h"
#include "view/simple_checkable_qtreewidget.h"
#include <QDir>
#include <QDebug>

#include "settings_manager.h"
#include "time_mediator.h"
#include "video/video_window.h"
#include "data/episode_meta_data.h"

Video_Windows_Mediator::Video_Windows_Mediator(QObject* parent)
{
    m_ptr_list_widget = nullptr;
}

void Video_Windows_Mediator::set_list_widget(Simple_Checkable_QTreeWidget* ptr_list_widget) noexcept
{
    m_ptr_list_widget = ptr_list_widget;
    connect(m_ptr_list_widget, &Simple_Checkable_QTreeWidget::change_state, this, &Video_Windows_Mediator::update_active_windows);
}

void Video_Windows_Mediator::preopen_episode_videos(const QString& episode_name) noexcept
{
    if (m_ptr_list_widget == nullptr)
    {
        return;
    }

    delete_all_video_windows();

    m_episode_name = episode_name;

    std::vector<int> is_full_screes;
    QStringList videos_need_to_open =  Settings_Manager::get_instance()->get_opened_videos(m_episode_name, &is_full_screes);

    m_ptr_list_widget->clear();

    QStringList tmp = episode_name.split("/");
    if (tmp.size() < 2)
    {
        return;
    }

    QString episode_id = tmp.back().split("_").front();


    tmp.removeLast(); // file name
    tmp.removeLast(); // Logs syn
    tmp.push_back("videos");

    QString path = tmp.join("/");

    QDir directory(path);
    QStringList files_in_video_folder = directory.entryList(QDir::Files);

    int i = 0;

    for (const QString& f : files_in_video_folder)
    {
//        if (f.indexOf("tmp_") == 0 && f.indexOf("_30fps") > 0)
//        {
//            continue;
//        }

        QString full_name = path + "/" + f;
        QFileInfo fi(full_name);
        if (fi.fileName().indexOf(episode_id) == 0 &&
                (fi.suffix().toLower() == "mp4" || fi.suffix().toLower() == "mov" || fi.suffix().toLower() == "mpg" || fi.suffix().toLower() == "mpeg"))
        {
            if (videos_need_to_open.indexOf(full_name.right(full_name.size() - 1)) >= 0) // we pass first \ in path
            {
                add_video_to_list_and_open(full_name, is_full_screes[i]);
                ++i;
            }
            else
            {
                add_video_to_list(full_name);
            }
        }
    }
}

void Video_Windows_Mediator::add_video_to_list_and_open(const QString& file_name, bool is_full_screen) noexcept
{
    if (add_video_to_list(file_name, true))
    {
        open_video(file_name, is_full_screen);
    }
}

bool Video_Windows_Mediator::add_video_to_list(const QString& file_name, bool is_checked) noexcept
{
    if (m_ptr_list_widget == nullptr)
    {
        return false;
    }

    disconnect(m_ptr_list_widget, &Simple_Checkable_QTreeWidget::change_state, this, &Video_Windows_Mediator::update_active_windows);

    bool is_need_to_open = false;
    int index = find_video_in_list(file_name);
    if (index >= 0)
    {
        if (m_ptr_list_widget->topLevelItem(index)->checkState(0) == Qt::Unchecked)
        {
            is_need_to_open = true;
            m_ptr_list_widget->topLevelItem(index)->setCheckState(0, Qt::Checked);
        }
    }
    else
    {
        QTreeWidgetItem *item = new QTreeWidgetItem(m_ptr_list_widget);
        item->setText(0, file_name.split("/").last());
        item->setData(0, 1, file_name);

        //QVariant video_spec = Episode_Meta_Data::get_instance().get_value(item->text(0), Meta_Parameter_Type::VIDEO_SPEC);

        //if (video_spec.isValid())
        {
            item->setToolTip(0, item->text(0));
        }

        item->setCheckState(0, is_checked ? Qt::Checked : Qt::Unchecked);
        is_need_to_open = true;
    }

    connect(m_ptr_list_widget, &Simple_Checkable_QTreeWidget::change_state, this, &Video_Windows_Mediator::update_active_windows);

    return is_need_to_open;
}

void Video_Windows_Mediator::open_video(const QString& file_name, bool is_full_screen) noexcept
{
    //    if (m_video_windows.size() > 0)
    //    {
    //        m_video_windows.begin()->second->close();
    //    }

    Video_Window* video_window = new Video_Window();
    video_window->setObjectName("VW" + file_name);

    if (!video_window->open_video_file(file_name))
    {
        video_window->close();
        delete video_window;

        return;
    }

    video_window->video_loaded();

    video_window->run_video_player();

    video_window->set_start_timestamp(Episode_Meta_Data::get_instance().get_video_start_time(file_name.split("/").back()));

    video_window->recieve_position_in_ms(Time_Mediator::get_instance().get_current_timestamp());

    Time_Mediator::get_instance().add_ptr_video_window(video_window);
    connect(video_window, &Video_Window::position_in_ms_changed, &Time_Mediator::get_instance(), &Time_Mediator::change_timestamp);
    connect(video_window, &Video_Window::position_changes_regarding_current, &Time_Mediator::get_instance(), &Time_Mediator::goto_regarding_current_time);
    connect(video_window, &Video_Window::video_speed_changed, &Time_Mediator::get_instance(), &Time_Mediator::change_video_speed_request);
    connect(video_window, &Video_Window::press_play_button_signal, &Time_Mediator::get_instance(), &Time_Mediator::play);
    connect(video_window, &Video_Window::press_pause_button_signal, &Time_Mediator::get_instance(), &Time_Mediator::pause);

    connect(video_window, &Video_Window::close_video_signal, this, &Video_Windows_Mediator::close_video);

    if (is_full_screen)
    {
        video_window->showMaximized();
    }

    m_video_windows[file_name] = video_window;
}

void Video_Windows_Mediator::close_video(const QString& file_name) noexcept
{
    if (m_ptr_list_widget == nullptr)
    {
        return;
    }

    disconnect(m_ptr_list_widget, &Simple_Checkable_QTreeWidget::change_state, this, &Video_Windows_Mediator::update_active_windows);

    int index = find_video_in_list(file_name);

    m_ptr_list_widget->topLevelItem(index)->setCheckState(0, Qt::Unchecked);

    if (m_video_windows.size() > 0)
    {
        auto it = m_video_windows.find(file_name);
        if (it != m_video_windows.end())
        {
            it->second->deleteLater();
            m_video_windows.erase(it);
        }
    }

    connect(m_ptr_list_widget, &Simple_Checkable_QTreeWidget::change_state, this, &Video_Windows_Mediator::update_active_windows);
}

void Video_Windows_Mediator::update_active_windows(bool state, int list_index) noexcept
{
    if (m_ptr_list_widget == nullptr)
    {
        return;
    }

    auto li = m_ptr_list_widget->topLevelItem(list_index);

    if (state)
    {
        if (m_video_windows.find(li->data(0, 1).toString()) == m_video_windows.end())
        {
            open_video(li->data(0, 1).toString(), false);
        }
    }
    else
    {
        qDebug() << "close video by checkbox";
        if (m_video_windows.find(li->data(0, 1).toString()) != m_video_windows.end())
        {
            m_video_windows[li->data(0, 1).toString()]->close();
        }
    }
}

void Video_Windows_Mediator::delete_all_video_windows() noexcept
{
    if (m_ptr_list_widget == nullptr)
    {
        return;
    }

    disconnect(m_ptr_list_widget, &Simple_Checkable_QTreeWidget::change_state, this, &Video_Windows_Mediator::update_active_windows);

    if (!m_episode_name.isEmpty())
    {
        QStringList opened_videos;
        std::vector<int> is_full_screens;
        for (int i = 0; i < m_ptr_list_widget->topLevelItemCount(); ++i)
        {
            auto li = m_ptr_list_widget->topLevelItem(i);

            if (li->checkState(0) == Qt::Checked)
            {
                opened_videos.push_back(li->data(0, 1).toString());
                if (m_video_windows.find(li->data(0, 1).toString()) != m_video_windows.end())
                {
                    is_full_screens.emplace_back((m_video_windows[li->data(0, 1).toString()]->isMaximized() ? 1 : 0));
                }
                else
                {
                    is_full_screens.emplace_back(0);
                }
            }
        }

        Settings_Manager::get_instance()->write_opened_videos(opened_videos, m_episode_name, is_full_screens);
    }

    for (auto w : m_video_windows)
    {
        disconnect(w.second, &Video_Window::close_video_signal, this, &Video_Windows_Mediator::close_video);
        w.second->close();
    }

    m_video_windows.clear();

    connect(m_ptr_list_widget, &Simple_Checkable_QTreeWidget::change_state, this, &Video_Windows_Mediator::update_active_windows);
}

int Video_Windows_Mediator::find_video_in_list(const QString& file_name) noexcept
{
    if (m_ptr_list_widget == nullptr)
    {
        return -1;
    }

    for (int i = 0; i < m_ptr_list_widget->topLevelItemCount(); ++i)
    {
        if (m_ptr_list_widget->topLevelItem(i)->data(0, 1).toString().compare(file_name) == 0)
        {
            return i;
        }
    }

    return -1;
}
