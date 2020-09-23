/***************************************************************************

             WINKAM TM strictly confidential 07.09.2020

 ***************************************************************************/
#include "simple_label_mediator.h"

#include "label_adding_widget.h"
#include "label_table_list.h"
#include "settings_manager.h"
#include "reader_labels_table.h"
#include "utils.h"
#include "time_mediator.h"
#include "label_mediator.h"
#include "data/episode_meta_data.h"
#include "operation_mode.h"

#include <QDebug>
#include <QGuiApplication>
#include <QScreen>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>

Simple_Label_Mediator::Simple_Label_Mediator()
{
    setObjectName("Simple_Label_Mediator");
    m_is_unsaved_changes = false;
}

void Simple_Label_Mediator::read_labels() noexcept
{    
    open_save_dialog();
    m_is_unsaved_changes = false;

    if (Operation_Mode::get_instance().get_log_name().isEmpty())
    {
        QMessageBox msgBox;
        msgBox.setText("Please open the file with sensor data (File->Open...)");
        msgBox.exec();
        return;
    }

    QString path = Settings_Manager::get_instance()->get_last_label_file_path();

    QString file_name = QFileDialog::getOpenFileName(nullptr, tr("Open File"), path,
                                                     tr("Label files (*.csv)"), nullptr, QFileDialog::DontUseNativeDialog);
    if (!file_name.isEmpty())
    {
        qDebug() << "read labels" << file_name;

        m_label_file_name = file_name;

        Settings_Manager::get_instance()->write_last_label_file_path(m_label_file_name);

        std::vector<Label_Item> label_items;

        Reader_Labels_Table::read_labels_file(file_name, &label_items, &m_label_names, &m_object_labels);

        QStringList table_header = m_object_labels;
        table_header.insert(0, "timestamp");
        table_header.insert(1, "hh:mm:ss");
        table_header.insert(2, "label");

        m_label_table->set_header(table_header);

        m_label_adding->set_buttons(m_label_names);

        std::vector<Combo_Box_Data> cb_data;

        QStringList object_names;

        if (Episode_Meta_Data::get_instance().is_meta_file_opened())
        {
            std::vector<QVariant> tmp = Episode_Meta_Data::get_instance().get_values(Meta_Parameter_Type::OBJECT_VIEW_ID);
            for (const QVariant& id : tmp)
            {
                object_names.push_back(id.toString());
            }
        }
        else
        {
            object_names = Label_Mediator::get_instance().get_sensor_ids();
        }

        //object_names.sort();
        //object_names.
        std::sort(object_names.begin(), object_names.end(), std::greater<QString>());

        for (const QString& object_name : m_object_labels)
        {
            Combo_Box_Data cb;
            cb.m_name = object_name;

            cb.m_items = object_names;

            cb_data.emplace_back(cb);
        }

        m_label_adding->set_comboboxes(cb_data);

        for (const Label_Item& li : label_items)
        {
            add_label_item(li);
        }
    }

    m_is_unsaved_changes = false;
}

void Simple_Label_Mediator::write_labels() noexcept
{
    if (m_label_file_name.isEmpty())
    {
        QMessageBox msgBox;
        msgBox.setText("Please open the file with labels (Open button)");
        msgBox.exec();
        return;
    }

    qDebug() << "write " << m_label_file_name;

    std::vector<QStringList> label_items = m_label_table->get_items();

    QFile file_output(m_label_file_name);

    if (file_output.open(QIODevice::WriteOnly))
    {
        QTextStream stream(&file_output);

        stream << "#labels:";
        for (const QString& s : m_label_names)
        {
            stream << s << ",";
        }
        stream << endl;

        stream << "#objects:";
        for (const QString& s : m_object_labels)
        {
            stream << s << ",";
        }
        stream << endl;

        stream << "#attributes:" << endl;

        for (const QStringList& li : label_items)
        {
            if (li.isEmpty())
            {
                continue;
            }

            for (int i = 0; i < li.size() - 1; ++i)
            {
                stream << li[i] << ",";
            }

            stream << li.back();

            stream << endl;
        }
    }

    file_output.close();

    m_is_unsaved_changes = false;
}

void Simple_Label_Mediator::add_label_item(const Label_Item& label_item) noexcept
{
    // add timestamp
    QStringList tmp = label_item.m_attributes;
    tmp.insert(0, QString::number(label_item.m_timestamp));
    tmp.insert(1, UTILS::time_to_string(label_item.m_timestamp));
    tmp.insert(2, label_item.m_label);
    m_label_table->add_item(tmp);
    m_is_unsaved_changes = true;
}

void Simple_Label_Mediator::add_label_item(const QStringList& label_item) noexcept
{
    if (m_label_file_name.isEmpty())
    {
        QMessageBox msgBox;
        msgBox.setText("Please open the file with labels (Open button)");
        msgBox.exec();
        return;
    }

    QStringList tmp = label_item;
    uint64_t timestamp = Time_Mediator::get_instance().get_current_timestamp();
    tmp.insert(0, QString::number(timestamp));
    tmp.insert(1, UTILS::time_to_string(timestamp));
    m_label_table->add_item(tmp);
    m_is_unsaved_changes = true;
}

void Simple_Label_Mediator::goto_by_label_table(uint64_t timestamp) noexcept
{
    qDebug() << "goto " << timestamp << "from the table of labels";
    emit goto_timestamp(timestamp);
}

void Simple_Label_Mediator::create_widgets(QWidget* parent) noexcept
{
    m_label_adding = std::make_shared<Label_Adding_Widget>(parent);
    m_label_table = std::make_shared<Label_Table_List>(QStringList({"timestamp", "hh:mm:ss", "label"}), parent);

    m_label_adding->setWindowTitle("add labels");
    m_label_table->setWindowTitle("labels");

    m_label_adding->setWindowFlag(Qt::Tool);
    m_label_table->setWindowFlag(Qt::Tool);

    int width = Settings_Manager::get_instance()->get_window_parameters("Label_Adding", "width");
    int height = Settings_Manager::get_instance()->get_window_parameters("Label_Adding", "height");

    if (width > 0 && height > 0)
    {
        int position_x = Settings_Manager::get_instance()->get_window_parameters("Label_Adding", "position_x");
        int position_y = Settings_Manager::get_instance()->get_window_parameters("Label_Adding", "position_y");

        m_label_adding->resize(width, height);
        m_label_adding->move(position_x, position_y);
    }
    else
    {
        m_label_adding->move(0, QGuiApplication::primaryScreen()->geometry().height() - m_label_adding->height());
    }

    width = Settings_Manager::get_instance()->get_window_parameters("Label_Table", "width");
    height = Settings_Manager::get_instance()->get_window_parameters("Label_Table", "height");

    if (width > 0 && height > 0)
    {
        int position_x = Settings_Manager::get_instance()->get_window_parameters("Label_Table", "position_x");
        int position_y = Settings_Manager::get_instance()->get_window_parameters("Label_Table", "position_y");

        m_label_table->resize(width, height);
        m_label_table->move(position_x, position_y);
    }
    else
    {
        m_label_table->move(QGuiApplication::primaryScreen()->geometry().width() - m_label_table->width(), 0);
    }

    connect(m_label_table.get(), &Label_Table_List::deleted_row, [this](){m_is_unsaved_changes = true;});
}

void Simple_Label_Mediator::delete_widgets() noexcept
{
    open_save_dialog();

    qDebug() << "destroy labeling widgets";

    Settings_Manager::get_instance()->write_window_parameters("Label_Adding", "width", m_label_adding->width());
    Settings_Manager::get_instance()->write_window_parameters("Label_Adding", "height", m_label_adding->height());

    Settings_Manager::get_instance()->write_window_parameters("Label_Adding", "position_x", m_label_adding->geometry().x());
    Settings_Manager::get_instance()->write_window_parameters("Label_Adding", "position_y", m_label_adding->geometry().y());

    Settings_Manager::get_instance()->write_window_parameters("Label_Table", "width", m_label_table->width());
    Settings_Manager::get_instance()->write_window_parameters("Label_Table", "height", m_label_table->height());

    Settings_Manager::get_instance()->write_window_parameters("Label_Table", "position_x", m_label_table->geometry().x());
    Settings_Manager::get_instance()->write_window_parameters("Label_Table", "position_y", m_label_table->geometry().y());

    m_label_adding.reset();
    m_label_table.reset();
}

void Simple_Label_Mediator::open_save_dialog() noexcept
{
    if (m_is_unsaved_changes)
    {
        QMessageBox msgBox;
        msgBox.setText("Do you wnat to save labels?");
        msgBox.setStandardButtons(QMessageBox::No | QMessageBox::Yes);
        msgBox.setDefaultButton(QMessageBox::Yes);
        if (msgBox.exec() == QMessageBox::Yes)
        {
            write_labels();
        }
    }
}

void Simple_Label_Mediator::show_widgets() noexcept
{
    qDebug() << "show labeling widgets";
    m_label_adding->show();
    m_label_table->show();
}

void Simple_Label_Mediator::hide_widgets() noexcept
{
    m_label_adding->hide();
    m_label_table->hide();
}

Simple_Label_Mediator::~Simple_Label_Mediator()
{
}
