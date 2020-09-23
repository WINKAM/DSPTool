/***************************************************************************

             WINKAM TM strictly confidential 01.09.2020

 ***************************************************************************/
#ifndef BUTTON_GRID_WIDGET_H
#define BUTTON_GRID_WIDGET_H

#include <QWidget>
#include <vector>

class QPushButton;
class QGridLayout;

class Button_Grid_Widget : public QWidget
{
    Q_OBJECT

public:
    Button_Grid_Widget(unsigned int column_number_upper_limit = 5, QWidget *parent = nullptr);

    void set_column_number_upper_limit(unsigned int column_number_upper_limit) noexcept;

    unsigned int get_column_number_upper_limit() const noexcept;

    QString get_pressed_button_text() const noexcept;

    void add_button(const QString& btn_text = "") noexcept;

    void delete_buttons();

    ~Button_Grid_Widget();

signals:
    void button_clicked(const QString& message);

protected:
    std::pair<unsigned int, unsigned int> calc_row_col_ids() const noexcept; // for added button, return row id, column id

    std::pair<unsigned int, unsigned int> calc_row_col_ids(int button_id) const noexcept; // for exist button, return row id, column id

    void clear() noexcept;

    void button_click() noexcept;

    void clear_checks(const QPushButton *btn_sender) noexcept;

private:
    std::vector<QPushButton*> m_buttons;

    QGridLayout* m_grid_layout;

    unsigned int m_column_number_upper_limit;

};

#endif // BUTTON_GRID_WIDGET_H
