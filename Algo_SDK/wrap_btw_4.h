/***************************************************************************

             WINKAM TM strictly confidential 02.10.2017

 ***************************************************************************/
#ifndef WRAP_BTW_4_H
#define WRAP_BTW_4_H

#include "wrap_transformation.h"

namespace WKTR
{


class Wrap_Btw_4 : public Wrap_Transformation
{
private:
    QString m_cut_off_x;
    QString m_cut_off_y;
    QString m_cut_off_z;

public:
    Wrap_Btw_4();
    void init(const QString& log_id
                      , const QString& object_id
                      , const QString& sensor_id
                      , const QString& parameter_string, Data_Manager* ptr_data_manager) noexcept;

    int process();
    ~Wrap_Btw_4();
};

}

#endif // WRAP_BTW_4_H
