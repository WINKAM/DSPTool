#ifndef APL_LOG_PARSER_H
#define APL_LOG_PARSER_H

#include <QObject>
#include "base_parser.h"

class W_Log_Data;

class Apl25_Log_Parser : public Base_Parser
{
private:
    uint64_t m_time_start;
    bool m_is_addition_log;
public:
    Apl25_Log_Parser(const QString &file_name);
    void set_addition_log_key(bool key) noexcept;
    bool parse(const QString &line, W_Log_Data *result);
};

#endif // APL_LOG_PARSER_H
