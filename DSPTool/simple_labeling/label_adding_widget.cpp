/***************************************************************************

             WINKAM TM strictly confidential 02.09.2020

 ***************************************************************************/
#include "label_adding_widget.h"
#include "button_grid_widget.h"
#include "simple_label_mediator.h"
#include "data/episode_meta_data.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QComboBox>
#include <QLabel>
#include <QDebug>
#include <QWidget>
#include <QDebug>

Label_Adding_Widget::Label_Adding_Widget(QWidget *parent)
    : QWidget(parent)
{
    m_button_grid = new Button_Grid_Widget();

    QHBoxLayout* main_h_layout = new QHBoxLayout();
    main_h_layout->addLayout(create_open_save_buttons());
    main_h_layout->addWidget(m_button_grid);
    QVBoxLayout* vertical_right_layout = new QVBoxLayout();
    vertical_right_layout->addLayout(create_right_panel());
    vertical_right_layout->addStretch();
    main_h_layout->addLayout(vertical_right_layout);

    setLayout(main_h_layout);

    //setWindowFlags(Qt::Tool);
}

void Label_Adding_Widget::set_buttons(const QStringList& button_labels) noexcept
{
    m_button_grid->delete_buttons();
    for (const QString& str : button_labels)
    {
        m_button_grid->add_button(str);
    }
}

void Label_Adding_Widget::set_comboboxes(const std::vector<Combo_Box_Data>& comboboxes) noexcept
{
    for (int row = 2; row < m_right_panel_layout->rowCount(); ++row)
    {
        QWidget* w = m_right_panel_layout->itemAtPosition(row, 0)->widget();
        m_right_panel_layout->removeWidget(w);
        w->deleteLater();
    }

    int row = 2;
    for (const Combo_Box_Data& cbd : comboboxes)
    {
        QLabel* l = new QLabel(cbd.m_name);
        QComboBox* cb = new QComboBox();

        cb->setEditable(true);
        cb->addItems(cbd.m_items);

        for (int i = 0; i < cbd.m_items.size(); ++i)
        {
            QString team_name = cbd.m_items[i].split(":").front();
            QString team_color;
            if (team_name.compare("ball") == 0)
            {
                team_color = "orange";
            }
            else
            {
                team_color = Episode_Meta_Data::get_instance().get_value(team_name, Meta_Parameter_Type::TEAM_COLOR).toString();
            }

            if (!team_color.isEmpty())
            {
                cb->setItemData(i, QColor(team_color), Qt::BackgroundRole);
            }

            if (i == 0)
            {
                cb->setStyleSheet("QComboBox { background-color: " + team_color + "; }");
            }
        }

        connect(cb, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [cb, this](int index)
        {
            cb->setStyleSheet("QComboBox { background-color: " + cb->itemData(index, Qt::BackgroundRole).toString() + "; }");
            this->setFocus();
        });

        connect(cb, &QComboBox::editTextChanged, [cb](const QString& text)
        {
            int index = cb->findText(text);
            if(index != -1)
            {
                cb->setCurrentIndex(index);
                cb->setItemText(cb->currentIndex(), text);
            }
            else
            {
                cb->addItem(text);
            }
        });

        m_right_panel_layout->addWidget(l, row++, 0);
        m_right_panel_layout->addWidget(cb, row++, 0);
    }
}

QVBoxLayout* Label_Adding_Widget::create_open_save_buttons() noexcept
{
    QPushButton* open_btn = new QPushButton("Open");
    QPushButton* save_btn = new QPushButton("Save");

    open_btn->setFixedWidth(100);
    save_btn->setFixedWidth(100);

    connect(open_btn, &QPushButton::clicked, []()
    {
        Simple_Label_Mediator::get_instance().read_labels();
    });

    connect(save_btn, &QPushButton::clicked, []()
    {
        Simple_Label_Mediator::get_instance().write_labels();
    });


    QVBoxLayout* open_save_v_layout = new QVBoxLayout();
    open_save_v_layout->addWidget(open_btn);
    open_save_v_layout->addWidget(save_btn);
    open_save_v_layout->addStretch();

    return open_save_v_layout;
}

QGridLayout* Label_Adding_Widget::create_right_panel() noexcept
{
    QPushButton* add_btn = new QPushButton("Add");
    QPushButton* attribute_btn = new QPushButton("Attribute");

    add_btn->setFixedWidth(100);
    attribute_btn->setFixedWidth(100);

    attribute_btn->setVisible(false);

    connect(add_btn, &QPushButton::clicked, [this]()
    {
        QStringList data = {m_button_grid->get_pressed_button_text()};

        for (int row = 3; row < m_right_panel_layout->rowCount(); row += 2) // 0 - Add btn, 1 - Attribute btn, 2 - first combobox label, 3 - first combobox
        {
            data.append(static_cast<QComboBox*>(m_right_panel_layout->itemAtPosition(row, 0)->widget())->currentText());
        }

        Simple_Label_Mediator::get_instance().add_label_item(data);
    });

    m_right_panel_layout = new QGridLayout();

    m_right_panel_layout->addWidget(add_btn, 0, 0);
    m_right_panel_layout->addWidget(attribute_btn, 1, 0);

    return m_right_panel_layout;
}

Label_Adding_Widget::~Label_Adding_Widget()
{
}

