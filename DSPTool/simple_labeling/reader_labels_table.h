#ifndef READER_LABELS_TABLE_H
#define READER_LABELS_TABLE_H

#include <QStringList>
#include <cstdint>

struct Label_Item
{
    uint64_t m_timestamp;
    QString m_label;
    QStringList m_attributes;
};

class Reader_Labels_Table
{
public:
    static bool read_labels_file(const QString& filename, std::vector<Label_Item>* output_label_items, QStringList* output_label_names
                                 , QStringList* output_object_names);
};

#endif // READER_LABELS_TABLE_H
