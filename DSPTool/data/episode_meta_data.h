/***************************************************************************

             WINKAM TM strictly confidential 03.06.2020

 ***************************************************************************/
#ifndef EPISODE_META_DATA_H
#define EPISODE_META_DATA_H

#include <unordered_map>
#include <map>
#include <vector>
#include <QString>
#include <QVariant>
#include <string>

enum class Meta_Parameter_Type {OBJECT_VIEW_ID = 0, // id for signal plots (jersey id or ball name)
                               OBJECT_TYPE = 1,
                               SENSOR_ENABLE = 2,
                               PLAYER_TEAM_ID = 3,
                               TEAM_NAME = 4,
                               TEAM_COLOR = 5,
                               TEAM_IDS = 6,
                               VIDEO_SPEC = 7,
                               PLAYER_TEAM_NAME = 8,
                               };

enum class Object_Type {PLAYER = 0, BALL = 1};

class Episode_Meta_Data
{
public:
    static Episode_Meta_Data& get_instance()
    {
        static Episode_Meta_Data instance;
        return instance;
    }

    void read_meta_file(const QString& file_name) noexcept;

    void find_read_meta_file(const QString& episode_name) noexcept;

    QVariant get_value(unsigned long key, Meta_Parameter_Type type) const noexcept;

    QVariant get_value(const QString& key, Meta_Parameter_Type type) const noexcept;

    std::vector<QVariant> get_values(Meta_Parameter_Type type) const noexcept;

    std::vector<QVariant> get_values(const QString& key, Meta_Parameter_Type type) const noexcept;

    // setters, getters start times are excluded because they are called frequently
    void set_start_timestamp(uint64_t timestamp) noexcept;

    uint64_t get_start_timestamp() const noexcept;

    int get_start_time_hh_mm_in_ms() const noexcept;

    uint64_t get_start_shift() const noexcept; // return m_start_timestamp - m_start_hh_mm_in_ms

    uint64_t get_video_start_time(const QString& video_name) const noexcept;

    uint64_t get_xyz_start_timestamp() const noexcept;

    void clear() noexcept;

    bool is_meta_file_opened() const noexcept;

protected:    

    void set_value(unsigned long key, Meta_Parameter_Type type, const QVariant& value);

private:
    Episode_Meta_Data();

    std::unordered_map<unsigned long, QString> m_object_view_ids;       // key - sensor id, value - id for signal plots (jersey id or ball name)
    std::unordered_map<unsigned long, Object_Type> m_object_types;      // key - sensor id
    std::unordered_map<unsigned long, bool> m_sensor_enables;           // key - sensor id
    std::unordered_map<unsigned long, unsigned long> m_player_team_ids; // key - sensor id
    std::unordered_map<unsigned long, QString> m_team_names;            // key - team id
    std::unordered_map<unsigned long, QString> m_team_colors;           // key - team id

    std::map<std::string, unsigned long> m_team_ids;          // support, key - team name, value - team id

    std::unordered_map<std::string, QString> m_video_specs;  // key - video file name

    std::unordered_map<std::string, uint64_t> m_video_start_timestamp; // key - video file name

    uint64_t m_start_timestamp = 0; // ms (in metafile it is 'Start_timestamp_ms')
    uint64_t m_xyz_start_timestamp = 0; // ms (in metafile it is 'Start_timestamp_ms')

    int m_start_hh_mm_in_ms = 0; // ms (in metafile it is 'Start_time_HH:MM')

    bool m_is_meta_file_opened;
};

#endif // EPISODE_META_DATA_H
