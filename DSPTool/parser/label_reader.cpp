/***************************************************************************

             WINKAM TM strictly confidential 16.04.2020

 ***************************************************************************/
#include "label_reader.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>

#include "data/wlogdata.h"
#include "event_parser.h"

Label_Reader::Label_Reader()
{

}

void Label_Reader::read_file(const QString& file_name, W_Log_Data* ptr_log_data)
{
    Event_Parser event_parser;
    event_parser.set_label_file_name(file_name);

    QFile file(file_name);
    file.open(QIODevice::ReadOnly);
    QTextStream text_stream(&file);
    QString line;

    while (!text_stream.atEnd())
    {
        line = text_stream.readLine();
        if (line.indexOf("event_type") == 0)
        {
            event_parser.parse_format_line(line);
            break;
        }
    }

    while (!text_stream.atEnd())
    {
        line = text_stream.readLine();
        event_parser.parse_event_line(line, ptr_log_data);
    }

    file.close();
}

Label_Reader::~Label_Reader()
{

}
