#include "reader_labels_table.h"

#include <QFile>
#include <QTextStream>
#include <QDebug>

QStringList read_header_line(QTextStream* in)
{
    const QRegExp separator("(:|;|,)");

    QString line;

    while (!in->atEnd())
    {
        line = in->readLine();
        if (!line.isEmpty())
        {
            break;
        }
    }

    QStringList tokens = line.split(separator);

    QStringList output;

    for (int i = 1; i < tokens.size(); ++i) // the first is keywors
    {
        QString tmp = tokens[i].simplified();
        if (!tmp.isEmpty())
        {
            output.append(tmp);
        }
    }

    return output;
}

bool Reader_Labels_Table::read_labels_file(const QString& filename, std::vector<Label_Item>* output_label_items, QStringList* output_label_names
                                           , QStringList* output_object_names)
{
    QFile inputFile(filename);

    const QRegExp separator("(,|;|\t)");

    if (inputFile.open(QIODevice::ReadOnly))
    {
        QTextStream in(&inputFile);

        // read header
        *output_label_names = read_header_line(&in);
        *output_object_names = read_header_line(&in);

        QString line;

        while (!in.atEnd())
        {
            line = in.readLine();

            if (line.isEmpty() || line[0] == '#')
            {
                continue;
            }

            QStringList tokens = line.split(separator);

            if (tokens.size() < 2)
            {
                continue;
            }

            Label_Item label_item;
            label_item.m_timestamp = tokens[0].simplified().toULongLong();
            label_item.m_label = tokens[1].simplified();

            for (int i = 2; i < tokens.size(); ++i)
            {
                label_item.m_attributes.append(tokens[i].simplified());
            }

            output_label_items->push_back(label_item);

            //qDebug() << label_item.m_timestamp << label_item.m_label << label_item.m_attributes;
        }
    }

    inputFile.close();
}
