#ifndef LOG_LABELS_H
#define LOG_LABELS_H

#include <map>
#include <vector>
#include <QString>
#include <QVariant>
#include <QStringList>

struct Label_Item
{
    QString m_type_v; // type with version
//    QString m_version;
    uint64_t m_timestamp;
    std::map<QString, QVariant> m_tag_value_map;
    bool m_is_error;
};

class Log_Labels
{
private:
    uint64_t m_time_start;
    bool m_is_modified;
    std::vector<Label_Item *> m_all_labels_list;
    std::map<QString, std::vector<Label_Item *>> m_labels_by_object_map;
    std::map<QString, std::vector<Label_Item *>> m_labels_by_type_map;

    std::vector<Label_Item *> m_all_errors_list;
    std::map<QString, std::vector<Label_Item *>> m_errors_by_object_map;
    std::map<QString, std::vector<Label_Item *>> m_errors_by_type_map;
public:
    Log_Labels();
    ~Log_Labels();

    void add_label_item(Label_Item* label_item) noexcept;    
    void add_label_by_object(const QString &object_id, Label_Item *label_item) noexcept;
    void delete_label(Label_Item* label_item) noexcept;

    void add_error_item(Label_Item* label_item) noexcept;
    void add_error_by_object(const QString &object_id, Label_Item *label_item) noexcept;

    bool is_modified();
    void set_is_modified(bool value);

    std::map<QString, std::vector<Label_Item *>>* get_map_labels_objects() noexcept;
    std::map<QString, std::vector<Label_Item *>>* get_map_labels_types() noexcept;

    std::vector<Label_Item *>* get_all_labels() noexcept;
    std::vector<Label_Item *>* get_labels_by_object(const QString &object_id) noexcept;
    std::vector<Label_Item *>* get_labels_by_object(const QString &object_id, const QString &error_type) noexcept;
    std::vector<Label_Item *>* get_labels_by_type(const QString &type) noexcept;
    std::vector<Label_Item *>* get_labels_by_object_and_type(const QString &object_id, const QString &type) noexcept;
    std::vector<Label_Item *>* get_labels_by_object_and_type(const QString &object_id, const QString &type, const QString &error_type) noexcept;
    const QStringList* get_labels_name_list() const noexcept;

    std::vector<Label_Item *>* get_all_errors() noexcept;
    std::vector<Label_Item *>* get_errors_by_object(const QString &object_id) noexcept;
    std::vector<Label_Item *>* get_errors_by_object(const QString &object_id, const QString &error_type) noexcept;
    std::vector<Label_Item *>* get_errors_by_type(const QString &type) noexcept;
    std::vector<Label_Item *>* get_errors_by_object_and_type(const QString &object_id, const QString &type) noexcept;
    std::vector<Label_Item *>* get_errors_by_object_and_type(const QString &object_id, const QString &type, const QString &error_type) noexcept;
    const QStringList* get_errors_name_list() const noexcept;

    void set_time_start(uint64_t time) noexcept;
    uint64_t get_time_start() const noexcept;
    uint64_t get_usefull_time() const noexcept;
};

#endif // LOG_LABELS_H
