#ifndef FILE_READER_H
#define FILE_READER_H

#include <QThread>

class QString;
class Data_Manager;

class File_Reader : public QThread
{
    Q_OBJECT
private:
    QString m_file_name;
    bool m_is_run;
    bool m_is_labels_file;
    Data_Manager *m_data_manager;

    bool identify_parser_type(const QString& line) noexcept;
public:
    File_Reader(QObject *parent = 0);
    ~File_Reader();
    void set_files_name(const QString &log_file_name, bool is_labels_file = false);
    void set_data_manager(Data_Manager *data_manager);
    bool execute();
    bool is_runnning();
    void cancel();
signals:
    void file_reading_done(bool is_success);
    void file_reading_done(bool is_success, const QString &file_name);
protected:
    void run();
};

#endif // FILE_READER_H
