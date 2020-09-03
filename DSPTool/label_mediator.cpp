/***************************************************************************

             WINKAM TM strictly confidential 08.07.2020

 ***************************************************************************/
#include "label_mediator.h"
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QDebug>
#include "plot/plot_controller.h"
#include "data/data_manager.h"
#include "parser/label_reader.h"

Label_Mediator::Label_Mediator()
{

}

void Label_Mediator::open_label(const QString& file_name) noexcept
{
    if (m_ptr_list_widget != nullptr && m_ptr_data_manager != nullptr && m_ptr_plots_controller != nullptr)
    {
        if (!m_opened_labels.contains(file_name) && !file_name.endsWith("meta.csv"))
        {
            qDebug() << "read label" << file_name;
            Label_Reader::read_file(file_name, m_ptr_data_manager->get_log_data());

            m_ptr_plots_controller->set_labels(m_ptr_data_manager->get_log_data()->get_log_labels());

            QTreeWidgetItem* item = new QTreeWidgetItem(m_ptr_list_widget);
            item->setText(0, file_name.split("/").back());
            m_opened_labels.insert(file_name);
        }
        else if (m_opened_labels.contains(file_name))
        {
            qDebug() << file_name << "has already opened";
        }
        else if (file_name.endsWith("meta.csv"))
        {
            qDebug() << file_name << "is not label file";
        }

    }
}

void Label_Mediator::set_list_widget_ptr(QTreeWidget* ptr_list_widget) noexcept
{
    m_ptr_list_widget = ptr_list_widget;
}

void Label_Mediator::set_data_manager_ptr(Data_Manager* ptr_data_manager) noexcept
{
    m_ptr_data_manager = ptr_data_manager;
}

void Label_Mediator::set_plots_controller_ptr(Plots_Controller *ptr_plots_controller) noexcept
{
    m_ptr_plots_controller = ptr_plots_controller;
}

void Label_Mediator::clear() noexcept
{
    if (m_ptr_list_widget != nullptr)
    {
        m_ptr_list_widget->clear();
    }

    m_opened_labels.clear();
}

Label_Mediator::~Label_Mediator()
{

}
