#ifndef BASE_PARSER_H
#define BASE_PARSER_H

#include <QString>
#include <QColor>

class W_Log_Data;

#define OFFSET_X -1920
#define OFFSET_Y -40

class Base_Parser
{
protected:
    bool m_is_first_value;
    QString m_file_name;
public:
    Base_Parser();
    virtual bool parse(const QString &line, W_Log_Data *result) = 0;
    virtual ~Base_Parser() = 0;
    virtual void set_addition_log_key(bool key) noexcept = 0;
};

#endif // BASE_PARSER_H
