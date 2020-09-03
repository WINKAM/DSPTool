/***************************************************************************

             WINKAM TM strictly confidential 13.04.2020

 ***************************************************************************/
#include "freqency_cutter.h"
#include "data_manager.h"
#include <QFile>
#include <QTextStream>
#include <unordered_map>
#include "parser/base_parser.h"

void Freqency_Cutter::write_lower_freq(QFile* output_file, int frequency, Data_Manager* ptr_data_manager) noexcept
{
    double offset_x, offset_y;
    if (output_file->fileName().contains("Location25_23_"))
    {
        offset_x = 0;
        offset_y = -1200; // left+; right-
    }
    else if (output_file->fileName().contains("Location25_22_"))
    {
        offset_x = 10200;
        offset_y = -1110;
    }
    else
    {
        offset_x = 1920;
        offset_y = 40;
    }

    output_file->open(QIODevice::WriteOnly);
    if (!output_file->isOpen())
    {
        return;
    }

    uint64_t sampling_period = static_cast<unsigned long>(1000 / frequency); // ms

    QTextStream file_stream;
    file_stream.setDevice(output_file);

    std::unordered_map <uint64_t, uint64_t> last_timestamps;

    uint64_t row_id = 0;

    const std::vector<XYZ_Sample>* ptr_data_vector = ptr_data_manager->get_log_data()->get_data_vector();
    for (const XYZ_Sample& s : *ptr_data_vector)
    {
        bool is_need_to_write = false;
        auto iter = last_timestamps.find(s.m_object_id);
        {
            if (iter == last_timestamps.cend())
            {
                last_timestamps[s.m_object_id] = s.m_time;
                is_need_to_write = true;
            }
            else
            {
                if (iter->second + sampling_period < s.m_time)
                {
                    is_need_to_write = true;
                    iter->second = s.m_time;
                }
            }
        }

        if (is_need_to_write)
        {
            file_stream << s.m_time << "," << (row_id++) << "," << s.m_object_id << ","
                        << round(s.m_x - offset_x) << ","
                        << round(s.m_y - offset_y) << ","
                        << round(s.m_z) << "\n";
        }
    }

    file_stream.flush();
    output_file->close();
}

