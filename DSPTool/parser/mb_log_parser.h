/***************************************************************************

             WINKAM TM strictly confidential 30.07.2019

 ***************************************************************************/
#ifndef MB_LOG_PARSER_H
#define MB_LOG_PARSER_H

#include "base_parser.h"
#include <QObject>

class W_Log_Data;
class QString;


class MB_Log_Parser : public Base_Parser
{
private:
    qlonglong m_time_start;
    bool m_is_addition_log;

public:
    MB_Log_Parser(const QString &file_name);
    void set_addition_log_key(bool key) noexcept;
    bool parse(const QString &line, W_Log_Data *result);
};

#endif // MB_LOG_PARSER_H
