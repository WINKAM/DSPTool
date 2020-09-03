/***************************************************************************

             WINKAM TM strictly confidential 2019

 ***************************************************************************/
#ifndef SAMPLE_H
#define SAMPLE_H

#include <stdint.h>

namespace WKTR // WINKAM library. Digital Signal Processing
{

// Struct Sample_3d stores 3-axis sample and timestamp
struct Sample_3d
{
    double m_x;
    double m_y;
    double m_z;
    uint32_t m_time; // timestamp in milliseconds

    Sample_3d(){}
    Sample_3d(double x, double y, double z, uint32_t time) : m_x(x), m_y(y), m_z(z), m_time(time) {}
};


const uint32_t UNDEFINED_TIME_U32 = UINT32_MAX;

}

#endif // POSITION_SAMPLE_H;
