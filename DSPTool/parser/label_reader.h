/***************************************************************************

             WINKAM TM strictly confidential 16.04.2020

 ***************************************************************************/
#ifndef LABEL_READER_H
#define LABEL_READER_H

class W_Log_Data;
class QString;

class Label_Reader
{
public:
    Label_Reader();
    static void read_file(const QString& file_name, W_Log_Data* ptr_log_data);
    ~Label_Reader();

private:

};

#endif // LABEL_READER_H
