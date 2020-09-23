/***************************************************************************

             WINKAM TM strictly confidential 01.09.2020

 ***************************************************************************/
#include "button_grid_widget.h"

#include <QPushButton>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>

#include <QDebug>

Button_Grid_Widget::Button_Grid_Widget(unsigned int column_number_upper_limit, QWidget *parent)
    : QWidget(parent)
{
    m_grid_layout = new QGridLayout();
    set_column_number_upper_limit(column_number_upper_limit);

    setLayout(m_grid_layout);
    setMinimumSize(240, 120);
    //setWindowFlags(Qt::Tool);L
}

void Button_Grid_Widget::set_column_number_upper_limit(unsigned int column_number_upper_limit) noexcept
{
    m_column_number_upper_limit = column_number_upper_limit;

    if (m_buttons.empty())
    {
        return;
    }

    clear();

    int btn_counter = 1;
    for (QPushButton* btn : m_buttons)
    {
        auto row_col_ids = calc_row_col_ids(btn_counter);
        m_grid_layout->addWidget(btn, row_col_ids.first, row_col_ids.second);

        ++btn_counter;
    }
}

unsigned int Button_Grid_Widget::get_column_number_upper_limit() const noexcept
{
    return m_column_number_upper_limit;
}

QString Button_Grid_Widget::get_pressed_button_text() const noexcept
{
    for (QPushButton* btn : m_buttons)
    {
        if (btn->isChecked())
        {
            return btn->objectName();
        }
    }

    return "";
}

void Button_Grid_Widget::add_button(const QString& btn_text) noexcept
{
    QString text_spaced = btn_text;
    text_spaced = text_spaced.replace("_", " ");
    QPushButton* new_btn = new QPushButton();
    new_btn->setMaximumSize(300, 100);
    new_btn->setObjectName(text_spaced);
    new_btn->setCheckable(true);
    new_btn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    connect(new_btn, &QPushButton::clicked, this, &Button_Grid_Widget::button_click);

    QLabel* label = new QLabel(text_spaced, new_btn);
    label->setWordWrap(true);
    QHBoxLayout* layout = new QHBoxLayout(new_btn);
    layout->addWidget(label, 0, Qt::AlignCenter);

    auto row_col_ids = calc_row_col_ids();

    m_grid_layout->addWidget(new_btn, row_col_ids.first, row_col_ids.second);
    m_buttons.emplace_back(new_btn);
}

void Button_Grid_Widget::delete_buttons()
{
    clear();

    for (QPushButton* btn : m_buttons)
    {
        btn->deleteLater();
    }

    m_buttons.clear();
}

void Button_Grid_Widget::button_click() noexcept
{
     QPushButton* btn_sender = static_cast<QPushButton*>(sender());
     clear_checks(btn_sender);

     emit button_clicked(btn_sender->objectName());
}

void Button_Grid_Widget::clear_checks(const QPushButton* btn_sender) noexcept
{
    for (QPushButton* btn : m_buttons)
    {
        if (btn_sender != btn)
        {
            btn->setChecked(false);
        }
    }
}

void Button_Grid_Widget::clear() noexcept
{
    for (QPushButton* btn : m_buttons)
    {
        m_grid_layout->removeWidget(btn);
    }
}

std::pair<unsigned int, unsigned int> Button_Grid_Widget::calc_row_col_ids(int button_id) const noexcept
{
    unsigned int button_count = button_id - 1;
    unsigned int row_id = button_count / m_column_number_upper_limit;
    unsigned int col_id = button_count - m_column_number_upper_limit * row_id;

    return {row_id, col_id};
}

std::pair<unsigned int, unsigned int> Button_Grid_Widget::calc_row_col_ids() const noexcept
{
    return calc_row_col_ids(m_buttons.size() + 1);
}

Button_Grid_Widget::~Button_Grid_Widget()
{
    delete_buttons();
}
