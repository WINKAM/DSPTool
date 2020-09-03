/***************************************************************************

             WINKAM TM strictly confidential 09.04.2020

 ***************************************************************************/
#ifndef EVENT_PARSER_H
#define EVENT_PARSER_H

#include <QMap>
#include <QString>
#include <QStringList>

class W_Log_Data;

class Event_Parser
{
public:
    Event_Parser();

    bool parse_line(const QString &line, W_Log_Data *result) noexcept;

    void set_label_file_name(const QString &line) noexcept;

    void parse_format_line(const QString &line) noexcept;

    void parse_event_line(const QString &line, W_Log_Data *result) const noexcept;

    void reset();

    ~Event_Parser();

protected:

private:
    QStringList m_event_tags;
    QString m_label_name_version;
};

#endif // EVENT_PARSER_H
