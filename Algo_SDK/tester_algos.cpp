/***************************************************************************

             WINKAM TM strictly confidential 11.10.2017

 ***************************************************************************/
#include "tester_algos.h"
#include "data/label/alg_temp_labels.h"
#include "butterworth_lpf.h"
#include "parser/base_parser.h"
#include <QString>
#include <QDebug>
#include <memory>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <QString>
#include <QTextStream>
#include <QFile>
#include <chrono>
#include <ctime>

#include <iostream>
#include <iomanip>
#include <sstream>
#include <bitset>


uint64_t get_ball_id_by_log_name(const QString& log_name) noexcept
{
    uint64_t ball_id = 0;
    if (log_name.contains("Location120_11_1"))
    {
        ball_id = 2147553334;
    }
    if (log_name.contains("Location120_11_2"))
    {
        ball_id = 2147553334;
    }
    else if (log_name.contains("Location120_12"))
    {
        ball_id = 2147567355;
    }
    else if (log_name.contains("Location120_15_2"))
    {
        ball_id = 2147575297;
    }
    else if (log_name.contains("Location120_16_1"))
    {
        ball_id = 2147562796;
    }
    else if (log_name.contains("Location120_17_"))
    {
        ball_id = 2147562796;
    }
    else if (log_name.contains("Location120_21_"))
    {
        ball_id = 2147574918;
    }
    else if (log_name.contains("Location120_22_"))
    {
        ball_id = 2147566872;
    }
    else if (log_name.contains("Location120_23_"))
    {
        ball_id = 2147565857;
    }
    else if (log_name.contains("Location120_24_"))
    {
        ball_id = 2147565721;
    }
    else if (log_name.contains("Location120_25_"))
    {
        ball_id = 2147562796;
    }
    else if (log_name.contains("Location120_26_"))
    {
        ball_id = 2147565714;
    }
    else if (log_name.contains("Location120_30_"))
    {
        ball_id = 2147566234;
    }
    else if (log_name.contains("Location120_31_"))
    {
        ball_id = 2147566234;
    }
    else if (log_name.contains("Location120_32_"))
    {
        ball_id = 2147566234;
    }
    else if (log_name.contains("Location120_33_"))
    {
        ball_id = 2147566234;
    }
    else if (log_name.contains("Location120_36_2"))
    {
        ball_id = 2147567123;
    }
    else if (log_name.contains("Location120_36_3"))
    {
        ball_id = 2147578495;
    }
    else if (log_name.contains("Location120_37_"))
    {
        ball_id = 2147565823;
    }
    else if (log_name.contains("Location120_38_"))
    {
        ball_id = 2147567180;
    }
    else if (log_name.contains("Location120_39_"))
    {
        ball_id = 2147577742;
    }
    else if (log_name.contains("Location120_40_"))
    {
        ball_id = 2147577922;
    }

    return  ball_id;
}

void write_stretch_compressed(const std::vector<XYZ_Sample> &input_raw_data_vector, const QString &parameters) noexcept
{
    if (parameters.split(' ').size() != 2)
    {
        qDebug() << "Error! write_stretch_compressed() there aren't parameters!";
        return;
    }

    double coef = parameters.split(' ').at(1).toDouble();

    qDebug() << coef;

    QFile file("stretched_compressed.csv");
    file.open(QIODevice::WriteOnly);
    if (!file.isOpen())
    {
        qDebug() << "Error! write_stretch_compressed() Can't create file to write XYZ samples!";
        return;
    }

    QTextStream file_stream;
    file_stream.setDevice(&file);
    int row_id = 0;

    for (const auto& sample : input_raw_data_vector)
    {
        file_stream << int64_t(sample.m_time * coef) << ","
                    << (row_id++) << ","
                    << sample.m_object_id << ","
                    << sample.m_x + OFFSET_X << ","
                    << sample.m_y + OFFSET_Y << ","
                    << sample.m_z << "\n";
    }

    file_stream.flush();
    file.close();
}

