/***************************************************************************

             WINKAM TM strictly confidential 02.09.2020

 ***************************************************************************/
#ifndef LABEL_ADDING_WIDGET_H
#define LABEL_ADDING_WIDGET_H

#include <QWidget>
#include <QStringList>
#include <vector>

struct Combo_Box_Data
{
    QString m_name;
    QStringList m_items;
};

class Button_Grid_Widget;
class QVBoxLayout;
class QGridLayout;
//class QComboBox;
//class QPushButton;

class Label_Adding_Widget : public QWidget
{
    Q_OBJECT

public:
    Label_Adding_Widget(QWidget *parent = nullptr);

    void set_buttons(const QStringList& button_labels) noexcept;

    void set_comboboxes(const std::vector<Combo_Box_Data>& comboboxes) noexcept;

    ~Label_Adding_Widget();

protected:
    QVBoxLayout *create_open_save_buttons() noexcept;
    QGridLayout *create_right_panel() noexcept;

private:
    Button_Grid_Widget* m_button_grid;
    QGridLayout* m_right_panel_layout;
};
#endif // LABEL_ADDING_WIDGET_H
