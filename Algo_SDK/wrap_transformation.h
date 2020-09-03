/***************************************************************************

             WINKAM TM strictly confidential 26.07.2017

 ***************************************************************************/
#ifndef TRANSFORMATION_CONTROLLER_H
#define TRANSFORMATION_CONTROLLER_H

#include <QString>
#include <QDebug>
#include "low_pass_filter.h"
#include "butterworth_lpf.h"

#ifdef ALGO_WKTR_DEFINE

#include "affine_transformation.h"

#endif

class Data_Manager;
class W_Object_Data;
class W_Sensor_Data;
class W_Log_Data;

namespace WKTR
{

QString add_0_to_time(uint64_t time, bool is_ms = false) noexcept;
QString time_to_qstring(uint64_t time) noexcept;

/* Wrap_Transformation is base class wrapper
 * to interaction between transformations (arbitrary API) and Data Manager
 */

enum class Tranform_Case  {SINGLE = 0, ALL_BALLS = 1, ALL_PLAYERS = 2, ALL_ALL = 3};

class Wrap_Transformation
{
protected:
    W_Object_Data* m_ptr_object_data = nullptr; // pointer to object which has filtered signals
    W_Sensor_Data* m_ptr_sensor_data = nullptr; // pointer to sensor which has filtered signals
    W_Log_Data* m_ptr_log_data = nullptr;
    QString m_parameters_string; // details of applying transformation (id log, object, sensor and parameters)
    QString m_example_parameters; // valid parameter string example
    QString m_name; // uniq transformations name
    QString m_sensor_fusion_name; // // uniq transformations name + specification
    Tranform_Case m_type; // all-balls, all-player, all-all, single object

    double m_TRC1_gain;
public:
    /**
     * Initializes class fileds, check validity of input data
     * @param log_id is string identificator of log
     * @param object_id is string identificator of object
     * @param sensor_id is string identificator of sensor
     * @param specification_string is string of parametes
     * @param ptr_data_manager is pointer to data_manager object
     */
    virtual void init(const QString& log_id
                      , const QString& object_id
                      , const QString& sensor_id
                      , const QString& parameter_string, Data_Manager* ptr_data_manager) noexcept;
    /**
     * Runs transformation
     * @return 1 if all right, 0 if new sensor fusion wasn't created
     */
    virtual int process() = 0;

    /**
     * Gets name of transformation
     * @return name of transformation
     */
    QString get_name() const noexcept;

    /**
     * Gets valid parameter string example
     * @return parameter string example
     */
    QString get_example() const noexcept;

    /**
     * Gets message about error in parameter string
     * @return error message
     */
    QString get_message_parameters_error() const noexcept;

    virtual ~Wrap_Transformation() = 0;
};

/* Below are some examples.
 * You should create your wrappers of transformations to separate files in the appropriate project folders.
 */

class Example_Wrap : public Wrap_Transformation
{
private:
    Low_Pass_Filter m_filter;
    QString m_signal_id;

    double m_a;
    double m_b;

public:
    explicit Example_Wrap();
    void init(const QString& log_id
              , const QString& object_id
              , const QString& sensor_id
              , const QString& parameter_string, Data_Manager* ptr_data_manager) noexcept;
    int process();
    ~Example_Wrap();
};


class HPF : public Wrap_Transformation
{
private:
    Low_Pass_Filter m_filter;
    QString m_signal_id;

public:
    explicit HPF();
    void init(const QString& log_id
              , const QString& object_id
              , const QString& sensor_id
              , const QString& parameter_string, Data_Manager* ptr_data_manager) noexcept;
    int process();
    ~HPF();
};

class Stratch_Compress : public Wrap_Transformation
{
private:
    QString m_signal_id;
    double m_compress_coef = 1.;

public:
    explicit Stratch_Compress();
    void init(const QString& log_id
              , const QString& object_id
              , const QString& sensor_id
              , const QString& parameter_string, Data_Manager* ptr_data_manager) noexcept;
    int process();
    ~Stratch_Compress();
};

class Example_TRC_1 : public Wrap_Transformation
{
private:
    QString m_signal_id;

public:
    explicit Example_TRC_1();
    void init(const QString& log_id
              , const QString& object_id
              , const QString& sensor_id
              , const QString& parameter_string, Data_Manager* ptr_data_manager) noexcept;
    int process();
    ~Example_TRC_1();
};

class Sum_XYZ : public Wrap_Transformation
{
private:
    QString m_signal_id;

public:
    explicit Sum_XYZ();
    void init(const QString& log_id
              , const QString& object_id
              , const QString& sensor_id
              , const QString& parameter_string, Data_Manager* ptr_data_manager) noexcept;
    int process();
    ~Sum_XYZ();
};

class Shift_Wrap : public Wrap_Transformation
{
private:    
    uint64_t m_shift;

public:
    explicit Shift_Wrap();
    void init(const QString& log_id
              , const QString& object_id
              , const QString& sensor_id
              , const QString& parameter_string, Data_Manager* ptr_data_manager) noexcept;
    int process();
    ~Shift_Wrap();
};

class Diff_Wrap : public Wrap_Transformation
{
private:
    QString m_signal_id;

public:
    explicit Diff_Wrap();
    void init(const QString& log_id
              , const QString& object_id
              , const QString& sensor_id
              , const QString& parameter_string, Data_Manager* ptr_data_manager) noexcept;
    int process();
    ~Diff_Wrap();
};

class Btrw_LPF : public Wrap_Transformation
{
private:
    QString m_signal_id;
public:
    explicit Btrw_LPF();
    void init(const QString& log_id
              , const QString& object_id
              , const QString& sensor_id
              , const QString& parameter_string, Data_Manager* ptr_data_manager) noexcept;
    int process();
    ~Btrw_LPF();
};

class Benchmark : public Wrap_Transformation
{
private:

public:
    explicit Benchmark(const QString &name, const QString &parameter_example = "");
    void init(const QString& log_id
              , const QString& object_id
              , const QString& sensor_id
              , const QString& parameter_string, Data_Manager* ptr_data_manager) noexcept;
    int process();
    ~Benchmark();
};

class Rqi_Algo : public Wrap_Transformation
{
private:

public:
    explicit Rqi_Algo(const QString &name, const QString &parameter_example = "");
    void init(const QString& log_id
              , const QString& object_id
              , const QString& sensor_id
              , const QString& parameter_string, Data_Manager* ptr_data_manager) noexcept;
    int process();
    ~Rqi_Algo();
};

class Bend_Wrap : public Wrap_Transformation
{
private:
    QString m_signal_id;
    int m_shoulder_lenght;
public:
    explicit Bend_Wrap();
    void init(const QString& log_id
              , const QString& object_id
              , const QString& sensor_id
              , const QString& parameter_string, Data_Manager* ptr_data_manager) noexcept;
    int process();
    ~Bend_Wrap();
};

class Full_Vel_Wrap : public Wrap_Transformation
{
private:
    int m_shoulder_lenght;
public:
    explicit Full_Vel_Wrap();
    void init(const QString& log_id
              , const QString& object_id
              , const QString& sensor_id
              , const QString& parameter_string, Data_Manager* ptr_data_manager) noexcept;
    int process();
    ~Full_Vel_Wrap();
};

#ifdef ALGO_WKTR_DEFINE

class Frequency_Reductor : public Wrap_Transformation
{
private:
    int m_input_freq;
    int m_output_freq;
public:
    explicit Frequency_Reductor();
    void init(const QString& log_id
              , const QString& object_id
              , const QString& sensor_id
              , const QString& parameter_string, Data_Manager* ptr_data_manager) noexcept;
    int process();
    ~Frequency_Reductor();
};

class Coordinate_System_Modificator : public Wrap_Transformation
{
private:
    WKTR::Rotation<double> m_rotator;
    WKTR::Transmition<double> m_transmitor;

public:
    explicit Coordinate_System_Modificator();
    void init(const QString& log_id
              , const QString& object_id
              , const QString& sensor_id
              , const QString& parameter_string, Data_Manager* ptr_data_manager) noexcept;
    int process();
    ~Coordinate_System_Modificator();
};

#endif

#ifdef ALGO_APL_PA_DEFINE

class Avg_Speed_XY : public Wrap_Transformation
{
private:
    int m_avg_number = 120;
    int m_accel_window = 2000;
public:
    explicit Avg_Speed_XY();
    void init(const QString& log_id
              , const QString& object_id
              , const QString& sensor_id
              , const QString& parameter_string, Data_Manager* ptr_data_manager) noexcept;
    int process();
    ~Avg_Speed_XY();
};



#endif

}

#endif // TRANSFORMATION_CONTROLLER_H
