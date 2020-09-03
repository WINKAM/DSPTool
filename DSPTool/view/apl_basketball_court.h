#ifndef APL_BASKETBALL_COURT_H
#define APL_BASKETBALL_COURT_H

#include <QObject>

class Viewer_Plot;
class QSize;
enum class Plot_Type;

namespace APL_BASKET
{
    struct Size
    {
        double m_width;
        double m_length;
        double m_half_width;
        double m_half_length;
        double m_height;

        double m_center_circle_radius;

        double m_backboard_from_edge;
        double m_backboard_length;
        double m_backboard_width;
        double m_backboard_from_floor;

        double m_ring_center_from_edge;
        double m_ring_radius;
        double m_ring_from_floor;

        double m_free_throw_line_length;
        double m_free_throw_line_from_edge;

        double m_3_point_line_radius;
    };
}

class Apl_Basketball_Court : public QObject
{
    Q_OBJECT
private:
    APL_BASKET::Size m_size;
    Viewer_Plot *m_plot;
    Plot_Type m_type;

public:
    explicit Apl_Basketball_Court(QObject *parent = 0);
    void build(Plot_Type type, Viewer_Plot* plot) noexcept;

    void draw_court(Plot_Type type) noexcept;
    void draw_hoop(Plot_Type type, double x, double y, double z, const QString &color) noexcept;

    ~Apl_Basketball_Court();
private:
    void create_xy_court() noexcept;
    void create_yz_court() noexcept;
    void create_xz_court() noexcept;
    void draw_hoop_xy(double x, double y, const QString &color) noexcept;
    void draw_hoop_xz(double x, double z, const QString &color) noexcept;
    void draw_hoop_yz(double y, double z, const QString &color) noexcept;

public slots:
    void update_plot_scale(const QSize &size);
signals:
};

#endif // APL_BASKETBALL_COURT_H
