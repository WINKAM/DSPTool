/***************************************************************************

             WINKAM TM strictly confidential 09.08.2017

 ***************************************************************************/
#ifndef WRAP_DDSR_3D_H
#define WRAP_DDSR_3D_H

#include "wrap_transformation.h"
#include <QDebug>

namespace WKRJ
{
    class Random_Jump_Filter;
}

namespace WKTR
{

//class DDSR_Filter;

class Wrap_Ddsr_3D : public Wrap_Transformation
{
private:
    WKRJ::Random_Jump_Filter* m_filter = nullptr;
    QString m_signal_id;

public:
    explicit Wrap_Ddsr_3D();
    void init(const QString& log_id
              , const QString& object_id
              , const QString& sensor_id
              , const QString& parameter_string, Data_Manager* ptr_data_manager) noexcept;
    int process();
    ~Wrap_Ddsr_3D();
};

}
#endif // WRAP_DDSR_3D_H
