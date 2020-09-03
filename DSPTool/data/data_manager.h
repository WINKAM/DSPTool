#ifndef DATA_MANAGER_H
#define DATA_MANAGER_H

#include "wlogdata.h"

class QString;
class Base_Parser;
class QTextStream;

struct Hoop
{
    double m_x;
    double m_y;
    double m_z;
};

class Data_Manager
{    
public:
    enum LOG_PARSER_TYPE {LOG_APL_120HZ, LOG_APL_25HZ, LOG_WK, LOG_ALPS, LOG_MB};

private:
    W_Log_Data *m_log_data;
    Base_Parser *m_parser;

    std::map<int, Hoop> m_hoops;
public:
    Data_Manager();
    void prepare_label_parser(const QString &file_name) noexcept;
    void prepare_log_parser(const QString &file_name, LOG_PARSER_TYPE type) noexcept;
    void check_log_file_name(const QString &file_name) noexcept;
    void parse_line(const QString &file_name);
    void write_labels(QTextStream &stream);

    void clear_hoops() noexcept;
    void add_hoop(int hoop_id, Hoop* hoop) noexcept;
    std::map<int, Hoop>* get_hoops() noexcept;

    W_Log_Data* get_log_data() noexcept;
    ~Data_Manager();
};

#endif // DATA_MANAGER_H
