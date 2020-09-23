/***************************************************************************

             WINKAM TM strictly confidential 01.09.2020

 ***************************************************************************/
#include "label_table_list.h"
#include <QTableWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QHeaderView>
#include <QDebug>

#include "simple_label_mediator.h"

#define DELETE_BUTTONS_COLUMN_ID 0
#define TIMESTAMP_BUTTONS_COLUMN_ID 1
#define OPEN_ATTRIBUTES_BUTTONS_COLUMN_ID 3

#define TABLE_BUTTON_WIDTH 25

Label_Table_List::Label_Table_List(const QStringList &header, QWidget *parent)
    : QWidget(parent)
{
    m_table_widget = new QTableWidget();

    set_header(header);

    m_table_widget->horizontalHeader()->setMinimumSectionSize(1);
    m_table_widget->verticalHeader()->hide();

    m_table_widget->setColumnWidth(DELETE_BUTTONS_COLUMN_ID, TABLE_BUTTON_WIDTH);
    m_table_widget->setColumnWidth(OPEN_ATTRIBUTES_BUTTONS_COLUMN_ID, TABLE_BUTTON_WIDTH);

    connect(m_table_widget, &QTableWidget::itemClicked, [](QTableWidgetItem* item)
    {
        if (item->column() == TIMESTAMP_BUTTONS_COLUMN_ID)
        {
            Simple_Label_Mediator::get_instance().goto_by_label_table(item->text().toULongLong());
        }
    });

    auto* main_layout = new QVBoxLayout();

    main_layout->addWidget(m_table_widget);

    setLayout(main_layout);

    setMinimumSize(240, 120);
    setWindowFlags(Qt::Tool);
}

void Label_Table_List::set_header(const QStringList& header) noexcept
{
    m_table_widget->clear();
    m_table_widget->setRowCount(0);
    m_table_widget->setColumnCount(header.size() + 2);

    QStringList tmp = header;
    tmp.insert(0, "X");
    tmp.insert(3, "A");

    m_table_widget->setHorizontalHeaderLabels(tmp);
}

void Label_Table_List::add_item(const QStringList& string_list)
{
    int r;
    for (r = m_table_widget->rowCount() - 1; r >= 0; --r)
    {
        if (m_table_widget->item(r, TIMESTAMP_BUTTONS_COLUMN_ID)->text().toULongLong() <= string_list[0].toULongLong())
        {
            break;
        }
    }

    ++r;

    m_table_widget->insertRow(r);

    int string_list_id = 0;

    for(int j = 0; j < m_table_widget->columnCount() && string_list_id < string_list.size(); j++)
    {
        if (j == OPEN_ATTRIBUTES_BUTTONS_COLUMN_ID || j == DELETE_BUTTONS_COLUMN_ID)
        {
            continue;
        }

        QTableWidgetItem *item = m_table_widget->item(r, j);
        if(!item)
        {
            item = new QTableWidgetItem;
            item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
            m_table_widget->setItem(r, j, item);
        }
        item->setText(string_list.at(string_list_id));
        ++string_list_id;

        QPushButton *delete_btn = new QPushButton("X", m_table_widget);
        delete_btn->setFixedWidth(TABLE_BUTTON_WIDTH);
        m_table_widget->setCellWidget(r, DELETE_BUTTONS_COLUMN_ID, delete_btn);

        connect(delete_btn, &QPushButton::clicked, this, &Label_Table_List::delete_row);

        QPushButton *attribute_btn = new QPushButton("A", m_table_widget);
        attribute_btn->setFixedWidth(TABLE_BUTTON_WIDTH);
        m_table_widget->setCellWidget(r, OPEN_ATTRIBUTES_BUTTONS_COLUMN_ID, attribute_btn);
    }
}

std::vector<QStringList> Label_Table_List::get_items() noexcept
{
    std::vector<QStringList> items;

    for (int row = 0; row < m_table_widget->rowCount(); ++row)
    {
        QStringList item;

        for (int col = 0; col < m_table_widget->columnCount(); ++col)
        {
            if (!(col == DELETE_BUTTONS_COLUMN_ID || col == TIMESTAMP_BUTTONS_COLUMN_ID + 1 || col == OPEN_ATTRIBUTES_BUTTONS_COLUMN_ID))
            {
                item.push_back(m_table_widget->item(row, col)->text());
            }
        }

        items.emplace_back(item);
    }

    return items;
}

void Label_Table_List::delete_row() noexcept
{
    QWidget *w = qobject_cast<QWidget *>(sender());
    if(w)
    {
        int row = m_table_widget->indexAt(w->pos()).row();
        m_table_widget->removeRow(row);
        m_table_widget->setCurrentCell(0, 0);

        emit deleted_row();
    }
}

Label_Table_List::~Label_Table_List()
{
}

