/***************************************************************************

             WINKAM TM strictly confidential 04.08.2017

 ***************************************************************************/
#ifndef WRAP_DDSR_H
#define WRAP_DDSR_H

#include "wrap_transformation.h"

namespace WKRJ
{
    class Random_Jump_Filter;
    class DDSR_Filter;
}

namespace WKTR
{

//class DDSR_Filter;

class Wrap_Ddsr : public Wrap_Transformation
{
private:
    WKRJ::DDSR_Filter* m_filter = nullptr;
    QString m_signal_id;

public:
    explicit Wrap_Ddsr();
    void init(const QString& log_id
              , const QString& object_id
              , const QString& sensor_id
              , const QString& parameter_string, Data_Manager* ptr_data_manager) noexcept;
    int process();
    ~Wrap_Ddsr();
};

}
#endif // WRAP_DDSR_H
