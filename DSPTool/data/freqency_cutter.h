/***************************************************************************

             WINKAM TM strictly confidential 13.04.2020

 ***************************************************************************/
#ifndef FREQENCY_CUTTER_H
#define FREQENCY_CUTTER_H

class QFile;
class Data_Manager;

class Freqency_Cutter
{
public:
    static Freqency_Cutter& get_instance()
    {
        static Freqency_Cutter singleton;
        return singleton;
    }

    void write_lower_freq(QFile *output_file, int frequency, Data_Manager *ptr_data_manager) noexcept;

private:
    Freqency_Cutter() {}
    Freqency_Cutter(const Freqency_Cutter&) = delete;
    Freqency_Cutter& operator=(const Freqency_Cutter&) = delete;
    ~Freqency_Cutter() {}
};



#endif // FREQENCY_CUTTER_H
