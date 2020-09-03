/***************************************************************************

             WINKAM TM strictly confidential 08.07.2020

 ***************************************************************************/
#ifndef LABEL_MEDIATOR_H
#define LABEL_MEDIATOR_H

#include <QObject>
#include <QSet>

class QTreeWidget;
class Data_Manager;
class Plots_Controller;

class Label_Mediator : public QObject
{
    Q_OBJECT
public:

    static Label_Mediator& get_instance()
    {
        static Label_Mediator instance;
        return instance;
    }

    void set_list_widget_ptr(QTreeWidget *ptr_list_widget) noexcept;

    void set_data_manager_ptr(Data_Manager* ptr_data_manager) noexcept;

    void set_plots_controller_ptr(Plots_Controller* ptr_plots_controller) noexcept;

    void open_label(const QString& file_name) noexcept;

    void clear() noexcept;

private:

    QTreeWidget* m_ptr_list_widget = nullptr;

    QSet<QString> m_opened_labels;

    Data_Manager* m_ptr_data_manager = nullptr;

    Plots_Controller* m_ptr_plots_controller = nullptr;

    Label_Mediator();

    ~Label_Mediator();
};

#endif // LABEL_MEDIATOR_H
