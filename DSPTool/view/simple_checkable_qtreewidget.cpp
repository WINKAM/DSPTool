#include "simple_checkable_qtreewidget.h"
#include <QDebug>

Simple_Checkable_QTreeWidget::Simple_Checkable_QTreeWidget(QWidget *parent) : QTreeWidget(parent)
{
    QObject::connect(this, SIGNAL(itemChanged(QTreeWidgetItem *, int)), this, SLOT(update_checks(QTreeWidgetItem *, int)));
}

// https://stackoverflow.com/questions/9194280/parent-dependent-qtreewidgetitem-checkboxes-in-dynamically-generated-qtreewidget
void Simple_Checkable_QTreeWidget::update_checks(QTreeWidgetItem *item, int column)
{
    Qt::CheckState state = item->checkState(0);
    int parent_id = -1;
    int child_id = -1;

    bool diff = false;
    if (column != 0 && column != -1)
    {
        return;
    }

    if (item->childCount() != 0 && item->checkState(0) != Qt::PartiallyChecked && column != -1)
    {
        Qt::CheckState checkState = item->checkState(0);
        for (int i = 0; i < item->childCount(); ++i)
        {
           item->child(i)->setCheckState(0, checkState);
        }
        state = checkState;
        parent_id = indexFromItem(item, column).row();
    }
    else if (item->childCount() == 0 || column == -1)
    {
        if (item->parent() == 0)
        {
            parent_id = indexFromItem(item, column).row();
        }
        else
        {
            child_id = indexFromItem(item, column).row();
            for (int j = 0; j < item->parent()->childCount(); ++j)
            {
                if (j != item->parent()->indexOfChild(item) && item->checkState(0) != item->parent()->child(j)->checkState(0))
                {
                    diff = true;
                }
            }
            if (diff)
            {
                item->parent()->setCheckState(0, Qt::PartiallyChecked);
            }
            else
            {
                item->parent()->setCheckState(0, item->checkState(0));
            }

            if (item->parent() != 0)
            {
                parent_id = indexFromItem(item->parent(), column).row();
                update_checks(item->parent(), -1);
            }
        }
    }    

    if (state != Qt::PartiallyChecked && parent_id > -1)
    {
        emit change_state(state, parent_id, child_id);
    }
}

