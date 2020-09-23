/***************************************************************************

             WINKAM TM strictly confidential 01.09.2020

 ***************************************************************************/
#ifndef LABEL_TABLE_LIST_H
#define LABEL_TABLE_LIST_H

#include <QWidget>
#include <QStringList>

class QTableWidget;


class Label_Table_List : public QWidget
{
    Q_OBJECT

public:
    Label_Table_List(const QStringList& header, QWidget *parent = nullptr);

    void set_header(const QStringList& header) noexcept;

    void add_item(const QStringList& string_list);

    std::vector<QStringList> get_items() noexcept;

    ~Label_Table_List();

signals:
    void deleted_row();

protected:
    void delete_row() noexcept;

private:
    QTableWidget* m_table_widget;

};
#endif // LABEL_TABLE_LIST_H
