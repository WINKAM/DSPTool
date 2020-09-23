/***************************************************************************

             WINKAM TM strictly confidential 07.09.2020

 ***************************************************************************/
#ifndef SIMPLE_LABEL_MEDIATOR_H
#define SIMPLE_LABEL_MEDIATOR_H

#include <QObject>
#include <memory>

class Label_Adding_Widget;
class Label_Table_List;

struct Label_Item;

class Simple_Label_Mediator : public QObject
{
    Q_OBJECT
public:
    static Simple_Label_Mediator& get_instance()
    {
        static Simple_Label_Mediator instance;
        return instance;
    }

    void read_labels() noexcept;

    void add_label_item(const Label_Item& label_item) noexcept;

    void add_label_item(const QStringList& label_item) noexcept;

    void goto_by_label_table(uint64_t timestamp) noexcept;

    void create_widgets(QWidget* parent) noexcept;

    void delete_widgets() noexcept;

    void show_widgets() noexcept;

    void hide_widgets() noexcept;

    void write_labels() noexcept;

signals:
    void goto_timestamp(uint64_t timestamp);

protected:
    void open_save_dialog() noexcept;

private:
    Simple_Label_Mediator();
    ~Simple_Label_Mediator();

    std::shared_ptr<Label_Adding_Widget> m_label_adding;
    std::shared_ptr<Label_Table_List> m_label_table;

    QString m_label_file_name;
    QStringList m_label_names;
    QStringList m_object_labels;

    bool m_is_unsaved_changes;
};

#endif // SIMPLE_LABEL_MEDIATOR_H
