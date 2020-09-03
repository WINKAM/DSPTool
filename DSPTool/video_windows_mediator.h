/***************************************************************************

             WINKAM TM strictly confidential 19.05.2020

 ***************************************************************************/
#ifndef VIDEO_WINDOWS_MEDIATOR_H
#define VIDEO_WINDOWS_MEDIATOR_H

#include <QObject>

class Video_Window;
class Simple_Checkable_QTreeWidget;

class Video_Windows_Mediator : public QObject
{
    Q_OBJECT

public:
    static Video_Windows_Mediator& get_instance()
    {
        static Video_Windows_Mediator instance;
        return instance;
    }

    void set_list_widget(Simple_Checkable_QTreeWidget* ptr_list_widget) noexcept;

    void preopen_episode_videos(const QString& episode_name) noexcept;

    void add_video_to_list_and_open(const QString& file_name, bool is_full_screen) noexcept;

    void open_video(const QString& file_name, bool is_full_screen) noexcept;

    void close_video(const QString& file_name) noexcept;

    void delete_all_video_windows() noexcept;

protected:
    bool add_video_to_list(const QString& file_name, bool is_checked = false) noexcept;

    int find_video_in_list(const QString& file_name) noexcept;

    void update_active_windows(bool state, int list_index) noexcept;

private:
    explicit Video_Windows_Mediator(QObject *parent = nullptr);

    Simple_Checkable_QTreeWidget* m_ptr_list_widget;

    std::map<QString, Video_Window *> m_video_windows;

    QString m_episode_name;
};

#endif // VIDEO_WINDOWS_MEDIATOR_H
