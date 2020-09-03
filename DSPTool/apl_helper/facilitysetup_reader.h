#ifndef FACILITY_READER_H
#define FACILITY_READER_H

#include <QThread>

class QString;
class Data_Manager;

class Facilitysetup_Reader : public QThread
{
    Q_OBJECT
private:
    QString m_file_name;
    bool m_is_run;
    Data_Manager *m_data_manager;
public:
    Facilitysetup_Reader(QObject *parent = 0);
    ~Facilitysetup_Reader();
    void set_files_name(const QString &log_file_name);
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

#endif // FACILITY_READER_H
