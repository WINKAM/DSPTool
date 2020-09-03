/***************************************************************************

             WINKAM TM strictly confidential 13.05.2020

 ***************************************************************************/
#ifndef MAIN_MENU_MEDIATOR_H
#define MAIN_MENU_MEDIATOR_H

#include <QObject>

class Main_Window;
class QMenuBar;
class Help_Widget;

class Main_Menu_Mediator : public QObject
{
    Q_OBJECT
public:
    static Main_Menu_Mediator& get_instance()
    {
        static Main_Menu_Mediator instance;
        return instance;
    }

    void set_main_window(Main_Window* ptr_main_window) noexcept;

    QMenuBar* create_menu_bar(QWidget *parent) const noexcept;

private:
    explicit Main_Menu_Mediator(QObject *parent = nullptr);
    Main_Window* m_ptr_main_window;
    Help_Widget* m_help_widget;
};

#endif // MAIN_MENU_MEDIATOR_H
