#ifndef LABEL_WRITER_H
#define LABEL_WRITER_H

#include <QThread>

class QString;
class Data_Manager;

class File_Writer : public QThread
{
    Q_OBJECT
private:
    QString m_file_name;
    bool m_is_run;
    Data_Manager *m_data_manager;
public:
    File_Writer(QObject *parent = 0);
    ~File_Writer();
    void set_files_name(const QString &log_file_name);
    void set_data_manager(Data_Manager *data_manager);
    bool execute();
    bool is_runnning();
    void cancel();
signals:
    void file_writing_done(bool is_success);
    void file_writing_done(bool is_success, const QString &file_name);
protected:
    void run();
};

#endif // LABEL_WRITER_H
