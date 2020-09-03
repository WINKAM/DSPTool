#ifndef SIMPLE_CHECKABLE_QTREEWIDGET_H
#define SIMPLE_CHECKABLE_QTREEWIDGET_H

#include <QTreeWidget>

class QTreeWidgetItem;

class Simple_Checkable_QTreeWidget : public QTreeWidget
{
    Q_OBJECT
public:
    Simple_Checkable_QTreeWidget(QWidget *parent);

public slots:
    void update_checks(QTreeWidgetItem *item, int column);

signals:
    void change_state(bool state, int parent_id, int child_id);
};

#endif // SIMPLE_CHECKABLE_QTREEWIDGET_H
