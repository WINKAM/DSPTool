/***************************************************************************

             WINKAM TM strictly confidential 04.05.2020

 ***************************************************************************/
#ifndef SPEED_CONVERTOR_H
#define SPEED_CONVERTOR_H

#include <vector>
#include <QString>

#define SPEED_MIN       0
#define SPEED_NORMAL    4
#define SPEED_MAX       8

const std::vector<QString> SPEED_LABELS = {"speed x 0.1", "speed x 0.25", "speed x 0.5", "speed x 0.75"
                                          , "speed normal"
                                           , "speed x 1.25", "speed x 1.5", "speed x 2", "speed x 4"};

const std::vector<double> SPEED_INTERVAL_COEFS = {10., 4., 2., 1.33333, 1, 0.8, 0.66666, 0.5, 0.25};

#endif // SPEED_CONVERTOR_H
