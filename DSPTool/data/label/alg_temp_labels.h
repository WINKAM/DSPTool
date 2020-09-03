/***************************************************************************

WINKAM TM strictly confidential 13.11.2017

***************************************************************************/
#ifndef ALG_TEMP_LABELS_H
#define ALG_TEMP_LABELS_H

#include <QString>
#include <vector>
#include <QDebug>


class Alg_Temp_Labels
{
public:
    struct Alg_Label
    {
        double m_time;
        QString m_color;
        double m_width;
        QString m_description;
    };

private:

    std::vector<Alg_Label> m_alg_labels;

public:
    static Alg_Temp_Labels& get_instance()
    {
        static Alg_Temp_Labels singleton;
        return singleton;
    }

    void clear_all_labels() noexcept;
    void add_label(double time, const QString &color = "#ababab", double width = 2, const QString &description = "") noexcept;
    std::vector<Alg_Label>* get_alg_temp_labels() noexcept;

private:
    Alg_Temp_Labels() {}
    ~Alg_Temp_Labels() {}
    Alg_Temp_Labels(const Alg_Temp_Labels &) = delete;
    Alg_Temp_Labels& operator= (const Alg_Temp_Labels &) = delete;
};

#endif // ALG_TEMP_LABELS_H
