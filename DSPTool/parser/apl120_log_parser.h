#ifndef APL120_LOG_PARSER_H
#define APL120_LOG_PARSER_H

#include "base_parser.h"
#include <QObject>

class W_Log_Data;
class QString;

class Apl120_Log_Parser : public Base_Parser
{
private:
    uint64_t m_time_start;
    int m_timeline_shift;

    int m_offset_x;
    int m_offset_y;

    bool m_is_addition_log;

public:
    Apl120_Log_Parser(const QString &file_name);
    void set_addition_log_key(bool key) noexcept;
    bool parse(const QString &line, W_Log_Data *result);
};

#endif // APL120_LOG_PARSER_H
