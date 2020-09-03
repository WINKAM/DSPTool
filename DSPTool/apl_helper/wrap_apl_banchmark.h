#ifndef WRAP_APL_BANCHMARK_H
#define WRAP_APL_BANCHMARK_H

class QString;

class Wrap_Apl_Banchmark
{
private:
    int m_benchmark_id;
    int m_dataset_id;
    QString* m_hoop1_config;
    QString* m_hoop2_config;
public:
    Wrap_Apl_Banchmark();
    void initialize(int benchmark_id, int dataset_id) noexcept;
    void set_hoop1_config(QString* config) noexcept;
    void set_hoop2_config(QString* config) noexcept;
    bool prepare() noexcept;
    QString execute(QString &filter_id) noexcept;

private:
    QString prepare_benchmark() noexcept;
    void prepare_facility_file(const QString &benchmark_path) noexcept;
    void change_hoop_config(const QString &path, int hoop_id, const QString &config) noexcept;
    QString get_new_hoop_parameter(QString old_parameter, int shift) noexcept;
    void delete_tmp_files() noexcept;
};

#endif // WRAP_APL_BANCHMARK_H
