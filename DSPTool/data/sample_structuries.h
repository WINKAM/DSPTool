/***************************************************************************

             WINKAM TM strictly confidential 25.08.2017

 ***************************************************************************/
#ifndef SAMPLE_STRUCTURIES_H
#define SAMPLE_STRUCTURIES_H

#include <stdint.h>
#include <algorithm>

typedef std::pair<uint64_t, double> Sample;
typedef std::vector<Sample> Sample_Vector;

struct XYZ_Sample
{
    double m_x;
    double m_y;
    double m_z;
    uint64_t m_object_id;
    uint64_t m_time;
};

#endif // SAMPLE_STRUCTURIES_H
