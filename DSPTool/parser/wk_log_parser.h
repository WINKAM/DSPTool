#ifndef WK_LOG_PARSER_H
#define WK_LOG_PARSER_H

#include "base_parser.h"
#include <QObject>

class W_Log_Data;
class QString;

class WK_Log_Parser : public Base_Parser
{
private:
    qlonglong m_time_start;
    bool m_is_addition_log;

public:
    WK_Log_Parser(const QString &file_name);
    void set_addition_log_key(bool key) noexcept;
    bool parse(const QString &line, W_Log_Data *result);
};

#endif // WK_LOG_PARSER_H
