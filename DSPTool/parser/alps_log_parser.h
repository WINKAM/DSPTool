/***************************************************************************

             WINKAM TM strictly confidential 17.06.2019

 ***************************************************************************/
#ifndef ALPS_LOG_PARSER_H
#define ALPS_LOG_PARSER_H

#include "base_parser.h"
#include <QObject>

class W_Log_Data;
class QString;


class Alps_Log_Parser : public Base_Parser
{
public:
    Alps_Log_Parser(const QString &file_name);
    void set_addition_log_key(bool key) noexcept;
    bool parse(const QString &line, W_Log_Data *result);

private:
    qlonglong m_time_start;
    qlonglong m_qrs_prev_timestamp;
    bool m_is_addition_log;

};

#endif // ALPS_LOG_PARSER_H
