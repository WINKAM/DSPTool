#ifndef APL_ERROR_PARSER_H
#define APL_ERROR_PARSER_H

#include <QObject>
#include "base_parser.h"
#include <map>

class W_Log_Data;
class QString;

namespace APL_ERRORS
{
const std::map<QString, std::map<QString, int>> APL_ERROR_TYPES =
{
    {"miss", {{"timestamp", 0},
              {"type", 1},
              {"player", 2},
              {"timestamp2", 3},
              {"pos_x", 4},
              {"pos_y", 5},
              {"pos_z", 6},
              {"ball", 7},
              {"hoop_id", 8},
              {"hoop_x", 9},
              {"hoop_y", 10},
              {"hoop_z", 11},
              {"hand", 12},
              {"error", 13},
              {"comment", 14}}},
    {"make", {{"timestamp", 0},
              {"type", 1},
              {"player", 2},
              {"timestamp2", 3},
              {"pos_x", 4},
              {"pos_y", 5},
              {"pos_z", 6},
              {"ball", 7},
              {"hoop_id", 8},
              {"hoop_x", 9},
              {"hoop_y", 10},
              {"hoop_z", 11},
              {"hand", 12},
              {"error", 13},
              {"comment", 14}}},
    {"gotpossession", {{"timestamp", 0},
                       {"type", 1},
                       {"player", 2},
                       {"pos_x", 3},
                       {"pos_y", 4},
                       {"pos_z", 5},
                       {"ball", 6}}},
    {"shortpossession", {{"timestamp", 0},
                         {"type", 1},
                         {"player", 2},
                         {"pos_x", 3},
                         {"pos_y", 4},
                         {"pos_z", 5},
                         {"ball", 6},
                         {"dribbles", 7}}},
    {"lostpossession", {{"timestamp", 0},
                        {"type", 1},
                        {"player", 2},
                        {"pos_x", 3},
                        {"pos_y", 4},
                        {"pos_z", 5},
                        {"ball", 6},
                        {"dribbles", 7}}},
    {"RJ", {{"timestamp", 0},
            {"type", 1},
            {"object", 2},
            {"comment", 3}}}
};

}

class Apl_Error_Parser : public Base_Parser
{

private:
    uint64_t m_time_start;
public:
    Apl_Error_Parser();
    void set_addition_log_key(bool key) noexcept;
    bool parse(const QString &line, W_Log_Data *result);
};

#endif // APL_ERROR_PARSER_H
