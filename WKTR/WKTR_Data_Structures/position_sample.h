/***************************************************************************

             WINKAM TM strictly confidential 2019

 ***************************************************************************/
#ifndef POSITION_SAMPLE_H
#define POSITION_SAMPLE_H

#include <stdint.h>
#include <cmath>

namespace WKTR // WINKAM library. Digital Signal Processing
{

struct Position_Sample
{
    double m_x; // X-coordinate in millimeters
    double m_y; // Y-coordinate in millimeters
    double m_z; // Z-coordinate in millimeters
    uint64_t m_time; // timestamp in milliseconds

    Position_Sample(double x, double y, double z, uint64_t time) : m_x(x), m_y(y), m_z(z), m_time(time) {}
    Position_Sample(int32_t x, int32_t y, int32_t z, uint64_t time) : m_x(x), m_y(y), m_z(z), m_time(time) {}
    Position_Sample() : m_x(0.), m_y(0.), m_z(0.), m_time(0) {}

};

inline double calc_cartesian_distance_xy(const Position_Sample& a, const Position_Sample& b) noexcept
{
    return std::sqrt((a.m_x - b.m_x) * (a.m_x - b.m_x) + (a.m_y - b.m_y) * (a.m_y - b.m_y));
}

inline double calc_cartesian_distance_xy(double ax, double ay, double bx, double by) noexcept
{
    return std::sqrt((ax - bx) * (ax - bx) + (ay - by) * (ay - by));
}

// constants used in different algorithms

#define UNDEFINED_TIME   0

#define M_S_TO_MPH 2.23694  // m/s to mph
#define MM_TO_INC 0.0393701 // mm to inches
#define RAD_TO_DEG 57.2958  // radiuan to degrees
#define GRAV_ACCEL 9.8      // m/s^2, gravitational acceleration

}

#endif // POSITION_SAMPLE_H;
