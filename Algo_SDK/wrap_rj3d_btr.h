/***************************************************************************

             WINKAM TM strictly confidential 26.09.2017

 ***************************************************************************/
#ifndef WRAP_RJ3D_BTR_H
#define WRAP_RJ3D_BTR_H

#include "wrap_transformation.h"

namespace WKRJ
{
    class Random_Jump_Filter;
}

namespace WKTR
{


class Butterworth_LPF;

class Wrap_rj_3d_btr : public Wrap_Transformation
{
private:
    WKRJ::Random_Jump_Filter* m_rj_filter = nullptr;
    Butterworth_LPF* m_btr_x_filter = nullptr;
    Butterworth_LPF* m_btr_y_filter = nullptr;
    Butterworth_LPF* m_btr_z_filter = nullptr;

//    QString m_signal_id;

public:
    Wrap_rj_3d_btr();
    void init(const QString& log_id
              , const QString& object_id
              , const QString& sensor_id
              , const QString& parameter_string, Data_Manager* ptr_data_manager) noexcept;
    int process();
    ~Wrap_rj_3d_btr();
};

}

#endif // WRAP_RJ3D_BTR_H
