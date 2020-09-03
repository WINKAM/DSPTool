#include "apl_basketball_court.h"
#include "view/viewer_plot.h"
#include <qwt_plot_curve.h>
#include <qwt_plot_shapeitem.h>
#include <QDebug>

Apl_Basketball_Court::Apl_Basketball_Court(QObject *parent) : QObject(parent)
{
    m_plot = nullptr;

    m_size.m_length = 28651.2 - 2211.2; // ncaa - apalach
    m_size.m_width = 15240 - 480; // ncaa - apalach
    m_size.m_height = 6000;
    m_size.m_half_length = m_size.m_length / 2;
    m_size.m_half_width = m_size.m_width / 2;

    m_size.m_center_circle_radius = 1828.8;

    m_size.m_backboard_from_edge = 1219.2;
    m_size.m_backboard_length = 1828.8;
    m_size.m_backboard_width = 1050;
    m_size.m_backboard_from_floor = 2900;

    m_size.m_ring_center_from_edge = 1600.2;
    m_size.m_ring_radius = 228.6;
    m_size.m_ring_from_floor = 3050;

    m_size.m_free_throw_line_length = 3657.6;
    m_size.m_free_throw_line_from_edge = 5791.2;

    m_size.m_3_point_line_radius = 6019.8;

}

void Apl_Basketball_Court::update_plot_scale(const QSize &size)
{
    if (m_plot != nullptr)
    {
        double width, height;
        if (m_type == Plot_Type::APL_XY)
        {
            width = m_size.m_length;
            height = m_size.m_width;
        }
        else if (m_type == Plot_Type::APL_XZ)
        {
            width = m_size.m_width;
            height = m_size.m_height;
        }
        else if (m_type == Plot_Type::APL_YZ)
        {
            width = m_size.m_length;
            height = m_size.m_height;
        }

        double ratio = (double)size.width() / (double)size.height();
        double x_min, x_max, y_min, y_max;

        if (ratio < (double) (width / height))
        {
            double tmp_height = (double)size.height() * width / (double)size.width();
            if (m_type == Plot_Type::APL_XY)
            {
                y_max = tmp_height / 2.;
                y_min = -1. * y_max;
            }
            else
            {
                y_max = tmp_height;
                y_min = 0;
            }
            x_max = width / 2.;
            x_min = -1. * x_max;
        }
        else
        {
            double tmp_width = (double)size.width() * height / (double)size.height();
            x_max = tmp_width / 2.;
            x_min = -1. * x_max;
            if (m_type == Plot_Type::APL_XY)
            {
                y_max = height / 2.;
                y_min = -1. * y_max;
            }
            else
            {
                y_max = height;
                y_min = 0;
            }
        }

        m_plot->set_scale(x_min, x_max, y_min, y_max);
    }
}

void Apl_Basketball_Court::build(Plot_Type type, Viewer_Plot* plot) noexcept
{
    m_plot = plot;
    m_type = type;    

    QObject::connect(m_plot, &Viewer_Plot::update_resize_plot_signal, this, &Apl_Basketball_Court::update_plot_scale);

    //QObject::connect(m_plot, &Viewer_Plot::update_scale_signal, [=](double minX, double maxX, double minY, double maxY){
    //    qDebug() << (maxX - minX) / (maxY - minY);});


    draw_court(type);
}

void Apl_Basketball_Court::draw_court(Plot_Type type) noexcept
{
    switch (type)
    {
    case Plot_Type::APL_XY:
        create_xy_court();
        break;
    case Plot_Type::APL_XZ:
        create_xz_court();
        break;
    case Plot_Type::APL_YZ:
        create_yz_court();
        break;
    default:
        break;
    }
}

void Apl_Basketball_Court::create_xy_court() noexcept
{
//// XY -----------------------
    // Отрисовка площадки. 94х50 футов (28651.2х15240 мм)
    QwtPlotShapeItem* court = new QwtPlotShapeItem("court");
    court->setRenderHint(QwtPlotItem::RenderAntialiased, true);
    court->attach(m_plot);
    court->setPen(QColor("black"), 2, Qt::SolidLine);
    QPainterPath court_path;
    court_path.addRect(QRectF(QPointF(-1 * (m_size.m_half_length), -1 * (m_size.m_half_width)), QSize(m_size.m_length, m_size.m_width)));
    court->setShape(court_path);

    // Отрисовка центральной линии
    QPolygonF center_line_polygon;
    center_line_polygon << QPointF(0, m_size.m_half_width) << QPointF(0, -1 * (m_size.m_half_width));
    QwtPlotShapeItem* center_line = new QwtPlotShapeItem("center_line");
    center_line->setRenderHint(QwtPlotItem::RenderAntialiased, true);
    center_line->attach(m_plot);
    center_line->setPen(QColor("black"), 2, Qt::SolidLine);
    QPainterPath center_line_path;
    center_line_path.addPolygon(center_line_polygon);
    center_line->setShape(center_line_path);

    // Отрисовка центрального круга, радиус 6 футов (1828.8 мм)
    QwtPlotShapeItem* center = new QwtPlotShapeItem("center");
    center->setRenderHint(QwtPlotItem::RenderAntialiased, true);
    center->attach(m_plot);
    center->setPen(QColor("black"), 2, Qt::SolidLine);
    QPainterPath center_path;
    center_path.addEllipse(QPointF(0, 0), m_size.m_center_circle_radius, m_size.m_center_circle_radius);
    center->setShape(center_path);

// LEFT -----------------------

    // Отрисовка щита. Щит находится на расстоянии 43 фута (13106.4 мм) Ширина 6 (1828.8 mm)
//    QPolygonF left_backboard_polygon;
//    left_backboard_polygon << QPointF(-1 * (m_size.m_half_length - m_size.m_backboard_from_edge), -1 * (m_size.m_backboard_length / 2))
//                           << QPointF(-1 * (m_size.m_half_length - m_size.m_backboard_from_edge), (m_size.m_backboard_length / 2));
//    QwtPlotShapeItem* left_backboard_line = new QwtPlotShapeItem("left_backboard_line");
//    left_backboard_line->setRenderHint(QwtPlotItem::RenderAntialiased, true);
//    left_backboard_line->attach(m_plot);
//    left_backboard_line->setPen(QColor("black"), 2, Qt::SolidLine);
//    QPainterPath left_backboard_line_path;
//    left_backboard_line_path.addPolygon(left_backboard_polygon);
//    left_backboard_line->setShape(left_backboard_line_path);

//    // Отрисовка левого кольца. Радиус кольца 9 дюймов, что 228.6 мм
//    // Так как щит находится на 43 фута от центра, center of rings 15" after backboard
//    QwtPlotShapeItem* left_ring = new QwtPlotShapeItem("left_ring");
//    left_ring->setRenderHint(QwtPlotItem::RenderAntialiased, true);
//    left_ring->attach(m_plot);
//    left_ring->setPen(QColor("black"), 2, Qt::SolidLine);
//    QPainterPath left_ring_path;
//    left_ring_path.addEllipse(QPointF(-1 * (m_size.m_half_length - m_size.m_ring_center_from_edge), 0), m_size.m_ring_radius, m_size.m_ring_radius);
//    left_ring->setShape(left_ring_path);

    // Левая 3х очковая линии. С отрисовкой тут сложности, рисуется в прямоугольнике. NCAA
//    {
        QwtPlotShapeItem* left_3_points = new QwtPlotShapeItem("left_3_points");
        left_3_points->setRenderHint(QwtPlotItem::RenderAntialiased, true);
        left_3_points->attach(m_plot);
        left_3_points->setPen(QColor("black"), 2, Qt::SolidLine);
        QPainterPath left_3_points_path;
        left_3_points_path.moveTo(QPointF(-1 * (m_size.m_half_length - m_size.m_ring_center_from_edge), m_size.m_3_point_line_radius));
        left_3_points_path.arcTo(QRectF(QPointF(-1 * (m_size.m_half_length - m_size.m_ring_center_from_edge + m_size.m_3_point_line_radius),
                                                -1 * m_size.m_3_point_line_radius),
                                        QSize(m_size.m_3_point_line_radius * 2, m_size.m_3_point_line_radius * 2)), -90, 180);
        left_3_points->setShape(left_3_points_path);

        // Прямая линия которая дорисовывает левую 3х очковую зону.
        QPolygonF left_3_points_bottom_line_polygon;
        left_3_points_bottom_line_polygon << QPointF(-1 * m_size.m_half_length, -1 * m_size.m_3_point_line_radius)
                                          << QPointF(-1 * (m_size.m_half_length - m_size.m_ring_center_from_edge), -1 * m_size.m_3_point_line_radius);
        QwtPlotShapeItem* left_3_points_bottom_line = new QwtPlotShapeItem("left_3_points_bottom_line");
        left_3_points_bottom_line->setRenderHint(QwtPlotItem::RenderAntialiased, true);
        left_3_points_bottom_line->attach(m_plot);
        left_3_points_bottom_line->setPen(QColor("black"), 2, Qt::SolidLine);
        QPainterPath left_3_points_bottom_line_path;
        left_3_points_bottom_line_path.addPolygon(left_3_points_bottom_line_polygon);
        left_3_points_bottom_line->setShape(left_3_points_bottom_line_path);

        // Прямая линия которая дорисовывает левую 3х очковую зону.
        QPolygonF left_3_points_top_line_polygon;
        left_3_points_top_line_polygon << QPointF(-1 * m_size.m_half_length, m_size.m_3_point_line_radius)
                                          << QPointF(-1 * (m_size.m_half_length - m_size.m_ring_center_from_edge), m_size.m_3_point_line_radius);
        QwtPlotShapeItem* left_3_points_top_line = new QwtPlotShapeItem("left_3_points_top_line");
        left_3_points_top_line->setRenderHint(QwtPlotItem::RenderAntialiased, true);
        left_3_points_top_line->attach(m_plot);
        left_3_points_top_line->setPen(QColor("black"), 2, Qt::SolidLine);
        QPainterPath left_3_points_top_line_path;
        left_3_points_top_line_path.addPolygon(left_3_points_top_line_polygon);
        left_3_points_top_line->setShape(left_3_points_top_line_path);
//    }

    // Левый прямоугольник под кольцом. Ширина прямоугольника 12 футов (3657.6 мм), длина 19 футов (5791.2 мм)
    QwtPlotShapeItem* left_rect = new QwtPlotShapeItem("left_rect");
    left_rect->setRenderHint(QwtPlotItem::RenderAntialiased, true);
    left_rect->attach(m_plot);
    left_rect->setPen(QColor("black"), 2, Qt::SolidLine);
    QPainterPath left_rect_path;
    left_rect_path.addRect(QRectF(QPointF(-1 * m_size.m_half_length, -1 * (m_size.m_free_throw_line_length / 2)),
                                  QSize(m_size.m_free_throw_line_from_edge, m_size.m_free_throw_line_length)));
    left_rect->setShape(left_rect_path);

    // Левая арка. все дуги рисуются не по координатам. Радиус арки 6 футов (1828.8 мм)
    QwtPlotShapeItem* left_arc = new QwtPlotShapeItem("left_arc");
    left_arc->setRenderHint(QwtPlotItem::RenderAntialiased, true);
    left_arc->attach(m_plot);
    left_arc->setPen(QColor("black"), 2, Qt::SolidLine);
    QPainterPath left_arc_path;
    left_arc_path.moveTo(QPointF(-1 * (m_size.m_half_length - m_size.m_free_throw_line_from_edge), m_size.m_center_circle_radius));
    left_arc_path.arcTo(QRectF(QPointF(-1 * (m_size.m_half_length - m_size.m_free_throw_line_from_edge + m_size.m_center_circle_radius), -1 * m_size.m_center_circle_radius),
                               QSize((m_size.m_center_circle_radius * 2), (m_size.m_center_circle_radius * 2))), -90, 180);
    left_arc->setShape(left_arc_path);

// RIGTH -----------------------

    // Отрисовка щита. Щит находится на расстоянии 43 фута (13106.4 мм) Ширина ?????
//    QPolygonF right_backboard_polygon;
//    right_backboard_polygon << QPointF((m_size.m_half_length - m_size.m_backboard_from_edge), -1 * (m_size.m_backboard_length / 2))
//                            << QPointF((m_size.m_half_length - m_size.m_backboard_from_edge), (m_size.m_backboard_length / 2));
//    QwtPlotShapeItem* right_backboard_line = new QwtPlotShapeItem("right_backboard_line");
//    right_backboard_line->setRenderHint(QwtPlotItem::RenderAntialiased, true);
//    right_backboard_line->attach(m_plot);
//    right_backboard_line->setPen(QColor("black"), 2, Qt::SolidLine);
//    QPainterPath right_backboard_line_path;
//    right_backboard_line_path.addPolygon(right_backboard_polygon);
//    right_backboard_line->setShape(right_backboard_line_path);

//    // Отрисовка правого кольца. Радиус кольца 9 дюймов, что 228.6 мм
//    // Так как щит находится на 43 фута от центра, center of rings 15" after backboard
//    QwtPlotShapeItem* right_ring = new QwtPlotShapeItem("right_ring");
//    right_ring->setRenderHint(QwtPlotItem::RenderAntialiased, true);
//    right_ring->attach(m_plot);
//    right_ring->setPen(QColor("black"), 2, Qt::SolidLine);
//    QPainterPath right_ring_path;
//    right_ring_path.addEllipse(QPointF((m_size.m_half_length - m_size.m_ring_center_from_edge), 0), m_size.m_ring_radius, m_size.m_ring_radius);
//    right_ring->setShape(right_ring_path);

    // Правая 3х очковая линии. С отрисовкой тут сложности, рисуется в прямоугольнике.
//    {
        QwtPlotShapeItem* right_3_points = new QwtPlotShapeItem("right_3_points");
        right_3_points->setRenderHint(QwtPlotItem::RenderAntialiased, true);
        right_3_points->attach(m_plot);
        right_3_points->setPen(QColor("black"), 2, Qt::SolidLine);
        QPainterPath right_3_points_path;
        right_3_points_path.moveTo(QPointF((m_size.m_half_length - m_size.m_ring_center_from_edge), -1 * m_size.m_3_point_line_radius));
        right_3_points_path.arcTo(QRectF(QPointF((m_size.m_half_length - m_size.m_ring_center_from_edge - m_size.m_3_point_line_radius),
                                                -1 * m_size.m_3_point_line_radius),
                                        QSize(m_size.m_3_point_line_radius * 2, m_size.m_3_point_line_radius * 2)), 90, 180);
        right_3_points->setShape(right_3_points_path);

        // Прямая линия которая дорисовывает правую 3х очковую зону.
        QPolygonF right_3_points_top_line_polygon;
        right_3_points_top_line_polygon << QPointF(m_size.m_half_length, m_size.m_3_point_line_radius)
                                        << QPointF((m_size.m_half_length - m_size.m_ring_center_from_edge), m_size.m_3_point_line_radius);
        QwtPlotShapeItem* right_3_points_top_line = new QwtPlotShapeItem("right_3_points_top_line");
        right_3_points_top_line->setRenderHint(QwtPlotItem::RenderAntialiased, true);
        right_3_points_top_line->attach(m_plot);
        right_3_points_top_line->setPen(QColor("black"), 2, Qt::SolidLine);
        QPainterPath right_3_points_top_line_path;
        right_3_points_top_line_path.addPolygon(right_3_points_top_line_polygon);
        right_3_points_top_line->setShape(right_3_points_top_line_path);

        // Прямая линия которая дорисовывает правую 3х очковую зону.
        QPolygonF right_3_points_bottom_line_polygon;
        right_3_points_bottom_line_polygon << QPointF(m_size.m_half_length, -1 * m_size.m_3_point_line_radius)
                                           << QPointF((m_size.m_half_length - m_size.m_ring_center_from_edge), -1 * m_size.m_3_point_line_radius);
        QwtPlotShapeItem* right_3_points_bottom_line = new QwtPlotShapeItem("right_3_points_bottom_line");
        right_3_points_bottom_line->setRenderHint(QwtPlotItem::RenderAntialiased, true);
        right_3_points_bottom_line->attach(m_plot);
        right_3_points_bottom_line->setPen(QColor("black"), 2, Qt::SolidLine);
        QPainterPath right_3_points_bottom_line_path;
        right_3_points_bottom_line_path.addPolygon(right_3_points_bottom_line_polygon);
        right_3_points_bottom_line->setShape(right_3_points_bottom_line_path);
//    }

    // Правый прямоугольник под кольцом. Ширина прямоугольника 12 футов (3657.6 мм), длина 19 футов (5791.2 мм)
    QwtPlotShapeItem* right_rect = new QwtPlotShapeItem("right_rect");
    right_rect->setRenderHint(QwtPlotItem::RenderAntialiased, true);
    right_rect->attach(m_plot);
    right_rect->setPen(QColor("black"), 2, Qt::SolidLine);
    QPainterPath right_rect_path;
    right_rect_path.addRect(QRectF(QPointF((m_size.m_half_length - m_size.m_free_throw_line_from_edge), -1 * (m_size.m_free_throw_line_length / 2)),
                                  QSize(m_size.m_free_throw_line_from_edge, m_size.m_free_throw_line_length)));
    right_rect->setShape(right_rect_path);

    // Правая арка. все дуги рисуются не по координатам. Радиус арки 6 футов (1828.8 мм)
    QwtPlotShapeItem* right_arc = new QwtPlotShapeItem("right_arc");
    right_arc->setRenderHint(QwtPlotItem::RenderAntialiased, true);
    right_arc->attach(m_plot);
    right_arc->setPen(QColor("black"), 2, Qt::SolidLine);
    QPainterPath right_arc_path;
    right_arc_path.moveTo(QPointF((m_size.m_half_length - m_size.m_free_throw_line_from_edge), -1 * m_size.m_center_circle_radius));
    right_arc_path.arcTo(QRectF(QPointF((m_size.m_half_length - m_size.m_free_throw_line_from_edge - m_size.m_center_circle_radius), -1 * m_size.m_center_circle_radius),
                               QSize((m_size.m_center_circle_radius * 2), (m_size.m_center_circle_radius * 2))), 90, 180);
    right_arc->setShape(right_arc_path);
}

void Apl_Basketball_Court::create_xz_court() noexcept
{
//// XZ -----------------------
    // Отрисовка пола площадки
    QPolygonF floor_polygon;
    floor_polygon << QPointF(-1 * m_size.m_half_width, 0) << QPointF(m_size.m_half_width, 0);
    QwtPlotShapeItem *floor_line = new QwtPlotShapeItem("floor_line");
    floor_line->setRenderHint(QwtPlotItem::RenderAntialiased, true);
    floor_line->attach(m_plot);
    floor_line->setPen(QColor("black"), 2, Qt::SolidLine);
    QPainterPath floor_line_path;
    floor_line_path.addPolygon(floor_polygon);
    floor_line->setShape(floor_line_path);

    // Отрисовка левой границы площадки. Ширина всей площадки 50 футов, это 15240 мм. Высота площадки 6 метров
    QPolygonF left_line_polygon;
    left_line_polygon << QPointF(-1 * m_size.m_half_width, 0) << QPointF(-1 * m_size.m_half_width, m_size.m_height);
    QwtPlotShapeItem *left_line = new QwtPlotShapeItem("left_line");
    left_line->setRenderHint(QwtPlotItem::RenderAntialiased, true);
    left_line->attach(m_plot);
    left_line->setPen(QColor("gray"), 1, Qt::SolidLine);
    QPainterPath left_line_path;
    left_line_path.addPolygon(left_line_polygon);
    left_line->setShape(left_line_path);

    // Отрисовка левой границы 3х очковой линии. Растояние между краем площадки и 3х очковой линией 3 фута - это 914.4 мм. Высота площадки 6 метров
    QPolygonF left_3_points_polygon;
    left_3_points_polygon << QPointF(-1 * m_size.m_3_point_line_radius, 0) << QPointF(-1 * m_size.m_3_point_line_radius, m_size.m_height);
    QwtPlotShapeItem *left_3_points = new QwtPlotShapeItem("left_3_points");
    left_3_points->setRenderHint(QwtPlotItem::RenderAntialiased, true);
    left_3_points->attach(m_plot);
    left_3_points->setPen(QColor("gray"), 1, Qt::SolidLine);
    QPainterPath left_3_points_path;
    left_3_points_path.addPolygon(left_3_points_polygon);
    left_3_points->setShape(left_3_points_path);

    // Отрисовка левой границы центрального кольца. Радиус центрального кольца 6 футов, это 1828.6 мм. Высота площадки 6 метров
    QPolygonF left_center_polygon;
    left_center_polygon << QPointF(-1 * m_size.m_center_circle_radius, 0) << QPointF(-1 * m_size.m_center_circle_radius, m_size.m_height);
    QwtPlotShapeItem *left_center = new QwtPlotShapeItem("left_center");
    left_center->setRenderHint(QwtPlotItem::RenderAntialiased, true);
    left_center->attach(m_plot);
    left_center->setPen(QColor("gray"), 1, Qt::SolidLine);
    QPainterPath left_center_path;
    left_center_path.addPolygon(left_center_polygon);
    left_center->setShape(left_center_path);

    // Отрисовка правой границы центрального кольца. Радиус центрального кольца 6 футов, это 1828.6 мм. Высота площадки 6 метров
    QPolygonF right_center_polygon;
    right_center_polygon << QPointF(m_size.m_center_circle_radius, 0) << QPointF(m_size.m_center_circle_radius, m_size.m_height);
    QwtPlotShapeItem *right_center = new QwtPlotShapeItem("right_center");
    right_center->setRenderHint(QwtPlotItem::RenderAntialiased, true);
    right_center->attach(m_plot);
    right_center->setPen(QColor("gray"), 1, Qt::SolidLine);
    QPainterPath right_center_path;
    right_center_path.addPolygon(right_center_polygon);
    right_center->setShape(right_center_path);

    // Отрисовка правой границы 3х очковой линии. Растояние между краем площадки и 3х очковой линией 3 фута - это 914.4 мм. Высота площадки 6 метров
    QPolygonF right_3_points_polygon;
    right_3_points_polygon << QPointF(m_size.m_3_point_line_radius, 0) << QPointF(m_size.m_3_point_line_radius, m_size.m_height);
    QwtPlotShapeItem *right_3_points = new QwtPlotShapeItem("right_3_points");
    right_3_points->setRenderHint(QwtPlotItem::RenderAntialiased, true);
    right_3_points->attach(m_plot);
    right_3_points->setPen(QColor("gray"), 1, Qt::SolidLine);
    QPainterPath right_3_points_path;
    right_3_points_path.addPolygon(right_3_points_polygon);
    right_3_points->setShape(right_3_points_path);

    // Отрисовка правой границы площадки. Ширина всей площадки 50 футов, это 15240 мм. Высота площадки 6 метров
    QPolygonF right_line_polygon;
    right_line_polygon << QPointF(m_size.m_half_width, 0) << QPointF(m_size.m_half_width, m_size.m_height);
    QwtPlotShapeItem *right_line = new QwtPlotShapeItem("right_line");
    right_line->setRenderHint(QwtPlotItem::RenderAntialiased, true);
    right_line->attach(m_plot);
    right_line->setPen(QColor("gray"), 1, Qt::SolidLine);
    QPainterPath right_line_path;
    right_line_path.addPolygon(right_line_polygon);
    right_line->setShape(right_line_path);

    // Отрисовка кольца. Радиус кольца 9 дюймов, что 228.6 мм, находится на высоте 3050 мм
//    QPolygonF ring_polygon;
//    ring_polygon << QPointF(-1 * m_size.m_ring_radius, m_size.m_ring_from_floor) << QPointF(m_size.m_ring_radius, m_size.m_ring_from_floor);
//    QwtPlotShapeItem *ring_line = new QwtPlotShapeItem("ring_line");
//    ring_line->setRenderHint(QwtPlotItem::RenderAntialiased, true);
//    ring_line->attach(m_plot);
//    ring_line->setPen(QColor("black"), 2, Qt::SolidLine);
//    QPainterPath ring_line_path;
//    ring_line_path.addPolygon(ring_polygon);
//    ring_line->setShape(ring_line_path);

//    // Отрисовка щита. Ширина щита 1828.8х1050 мм. Нижняя граница щита на высоте 2900 мм
//    QwtPlotShapeItem* ring_board = new QwtPlotShapeItem("ring_board");
//    ring_board->setRenderHint(QwtPlotItem::RenderAntialiased, true);
//    ring_board->attach(m_plot);
//    ring_board->setPen(QColor("black"), 2, Qt::SolidLine);
//    QPainterPath ring_board_path;
//    ring_board_path.addRect(QRectF(QPointF(-1 * (m_size.m_backboard_length / 2), m_size.m_backboard_from_floor),
//                                   QSize(m_size.m_backboard_length, m_size.m_backboard_width)));
//    ring_board->setShape(ring_board_path);
}

void Apl_Basketball_Court::create_yz_court() noexcept
{
//// YZ -----------------------
    // Отрисовка пола площадки
    QPolygonF floor_polygon;
    floor_polygon << QPointF(-1 * m_size.m_half_length, 0) << QPointF(m_size.m_half_length, 0);
    QwtPlotShapeItem *floor_line = new QwtPlotShapeItem("floor_line");
    floor_line->setRenderHint(QwtPlotItem::RenderAntialiased, true);
    floor_line->attach(m_plot);
    floor_line->setPen(QColor("black"), 2, Qt::SolidLine);
    QPainterPath floor_line_path;
    floor_line_path.addPolygon(floor_polygon);
    floor_line->setShape(floor_line_path);

    // Отрисовка левого кольца. Радиус кольца 9 дюймов, что 228.6 мм, находится на высоте 3050 мм
    // щит находится на 43 фута от центра,
//    QPolygonF left_ring_polygon;
//    left_ring_polygon << QPointF(-1 * (m_size.m_half_length - m_size.m_ring_center_from_edge + m_size.m_ring_radius), m_size.m_ring_from_floor)
//                      << QPointF(-1 * (m_size.m_half_length - m_size.m_ring_center_from_edge - m_size.m_ring_radius), m_size.m_ring_from_floor);
//    QwtPlotShapeItem *left_ring_line = new QwtPlotShapeItem("left_ring_line");
//    left_ring_line->setRenderHint(QwtPlotItem::RenderAntialiased, true);
//    left_ring_line->attach(m_plot);
//    left_ring_line->setPen(QColor("black"), 2, Qt::SolidLine);
//    QPainterPath left_ring_line_path;
//    left_ring_line_path.addPolygon(left_ring_polygon);
//    left_ring_line->setShape(left_ring_line_path);

//    // Отрисовка левого щита. Высота щита 1050 мм, нижняя граница находится на высоте 2900 мм
//    // Щит находится на 43 футов от центра
//    QPolygonF left_board_polygon;
//    left_board_polygon << QPointF(-1 * (m_size.m_half_length - m_size.m_backboard_from_edge), m_size.m_backboard_from_floor)
//                       << QPointF(-1 * (m_size.m_half_length - m_size.m_backboard_from_edge), (m_size.m_backboard_from_floor + m_size.m_backboard_width));
//    QwtPlotShapeItem *left_board_line = new QwtPlotShapeItem("left_board_line");
//    left_board_line->setRenderHint(QwtPlotItem::RenderAntialiased, true);
//    left_board_line->attach(m_plot);
//    left_board_line->setPen(QColor("black"), 2, Qt::SolidLine);
//    QPainterPath left_board_line_path;
//    left_board_line_path.addPolygon(left_board_polygon);
//    left_board_line->setShape(left_board_line_path);

    // Отрисовка правого кольца. Радиус кольца 9 дюймов, что 228.6 мм, находится на высоте 3050 мм
    // Так как щит находится на 43 фута от центра, то взял еще 50 мм  от щита еще
//    QPolygonF right_ring_polygon;
//    right_ring_polygon << QPointF((m_size.m_half_length - m_size.m_ring_center_from_edge + m_size.m_ring_radius), m_size.m_ring_from_floor)
//                       << QPointF((m_size.m_half_length - m_size.m_ring_center_from_edge - m_size.m_ring_radius), m_size.m_ring_from_floor);
//    QwtPlotShapeItem *right_ring_line = new QwtPlotShapeItem("right_ring_line");
//    right_ring_line->setRenderHint(QwtPlotItem::RenderAntialiased, true);
//    right_ring_line->attach(m_plot);
//    right_ring_line->setPen(QColor("black"), 2, Qt::SolidLine);
//    QPainterPath right_ring_line_path;
//    right_ring_line_path.addPolygon(right_ring_polygon);
//    right_ring_line->setShape(right_ring_line_path);

//    // Отрисовка правого щита. Высота щита 1050 мм, нижняя граница находится на высоте 2900 мм
//    // Щит находится на 43 футов от центра
//    QPolygonF right_board_polygon;
//    right_board_polygon << QPointF((m_size.m_half_length - m_size.m_backboard_from_edge), m_size.m_backboard_from_floor)
//                       << QPointF((m_size.m_half_length - m_size.m_backboard_from_edge), (m_size.m_backboard_from_floor + m_size.m_backboard_width));
//    QwtPlotShapeItem *right_board_line = new QwtPlotShapeItem("right_board_line");
//    right_board_line->setRenderHint(QwtPlotItem::RenderAntialiased, true);
//    right_board_line->attach(m_plot);
//    right_board_line->setPen(QColor("black"), 2, Qt::SolidLine);
//    QPainterPath right_board_line_path;
//    right_board_line_path.addPolygon(right_board_polygon);
//    right_board_line->setShape(right_board_line_path);

    // Левая граница площадки. Ширина площадки 94 фута, половина ширины в мм 14325.6. Высота площадки 6 метров
    QPolygonF left_line_polygon;
    left_line_polygon << QPointF(-1 * m_size.m_half_length, 0) << QPointF(-1 * m_size.m_half_length, m_size.m_height);
    QwtPlotShapeItem *left_line = new QwtPlotShapeItem("left_line");
    left_line->setRenderHint(QwtPlotItem::RenderAntialiased, true);
    left_line->attach(m_plot);
    left_line->setPen(QColor("gray"), 1, Qt::SolidLine);
    QPainterPath left_line_path;
    left_line_path.addPolygon(left_line_polygon);
    left_line->setShape(left_line_path);

    // Левая штрафная линии. Находится на расстоянии 28 футов (8534.4 мм) от центра площадки Высота площадки 6 метров
    QPolygonF left_free_throw_polygon;
    left_free_throw_polygon << QPointF(-1 * (m_size.m_half_length - m_size.m_free_throw_line_from_edge), 0)
                            << QPointF(-1 * (m_size.m_half_length - m_size.m_free_throw_line_from_edge), m_size.m_height);
    QwtPlotShapeItem *left_free_throw = new QwtPlotShapeItem("left_free_throw");
    left_free_throw->setRenderHint(QwtPlotItem::RenderAntialiased, true);
    left_free_throw->attach(m_plot);
    left_free_throw->setPen(QColor("gray"), 1, Qt::SolidLine);
    QPainterPath left_free_throw_path;
    left_free_throw_path.addPolygon(left_free_throw_polygon);
    left_free_throw->setShape(left_free_throw_path);

    // Левая граница арки. Находится на расстоянии 22 футов (6705.6 мм) от центра площадки Высота площадки 6 метров
    QPolygonF left_arc_polygon;
    left_arc_polygon << QPointF(-1 * (m_size.m_half_length - m_size.m_free_throw_line_from_edge - m_size.m_center_circle_radius), 0)
                     << QPointF(-1 * (m_size.m_half_length - m_size.m_free_throw_line_from_edge - m_size.m_center_circle_radius), m_size.m_height);
    QwtPlotShapeItem *left_arc = new QwtPlotShapeItem("left_arc");
    left_arc->setRenderHint(QwtPlotItem::RenderAntialiased, true);
    left_arc->attach(m_plot);
    left_arc->setPen(QColor("gray"), 1, Qt::SolidLine);
    QPainterPath left_arc_path;
    left_arc_path.addPolygon(left_arc_polygon);
    left_arc->setShape(left_arc_path);

    // Левая 3х очковая линии. Находится на расстоянии ? футов (6503.2 мм) от центра площадки Высота площадки 6 метров
    QPolygonF left_3_points_polygon;
    left_3_points_polygon << QPointF(-1 * (m_size.m_half_length - m_size.m_ring_center_from_edge - m_size.m_3_point_line_radius), 0)
                          << QPointF(-1 * (m_size.m_half_length - m_size.m_ring_center_from_edge - m_size.m_3_point_line_radius), m_size.m_height);
    QwtPlotShapeItem *left_3_points = new QwtPlotShapeItem("left_3_points");
    left_3_points->setRenderHint(QwtPlotItem::RenderAntialiased, true);
    left_3_points->attach(m_plot);
    left_3_points->setPen(QColor("gray"), 1, Qt::SolidLine);
    QPainterPath left_3_points_path;
    left_3_points_path.addPolygon(left_3_points_polygon);
    left_3_points->setShape(left_3_points_path);

    // Левая линия центрального круга. Находится на расстоянии 6 футов (1828.8 мм) от центра площадки Высота площадки 6 метров
    QPolygonF left_center_polygon;
    left_center_polygon << QPointF(-1 * m_size.m_center_circle_radius, 0) << QPointF(-1 * m_size.m_center_circle_radius, m_size.m_height);
    QwtPlotShapeItem *left_center = new QwtPlotShapeItem("left_center");
    left_center->setRenderHint(QwtPlotItem::RenderAntialiased, true);
    left_center->attach(m_plot);
    left_center->setPen(QColor("gray"), 1, Qt::SolidLine);
    QPainterPath left_center_path;
    left_center_path.addPolygon(left_center_polygon);
    left_center->setShape(left_center_path);

    // Центральная линия площадки. Высота площадки 6 метров
    QPolygonF center_polygon;
    center_polygon << QPointF(0, 0) << QPointF(0, m_size.m_height);
    QwtPlotShapeItem *center = new QwtPlotShapeItem("center");
    center->setRenderHint(QwtPlotItem::RenderAntialiased, true);
    center->attach(m_plot);
    center->setPen(QColor("gray"), 1, Qt::SolidLine);
    QPainterPath center_path;
    center_path.addPolygon(center_polygon);
    center->setShape(center_path);

    // Правая линия центрального круга. Находится на расстоянии 6 футов (1828.8 мм) от центра площадки Высота площадки 6 метров
    QPolygonF right_center_polygon;
    right_center_polygon << QPointF(m_size.m_center_circle_radius, 0) << QPointF(m_size.m_center_circle_radius, m_size.m_height);
    QwtPlotShapeItem *right_center = new QwtPlotShapeItem("right_center");
    right_center->setRenderHint(QwtPlotItem::RenderAntialiased, true);
    right_center->attach(m_plot);
    right_center->setPen(QColor("gray"), 1, Qt::SolidLine);
    QPainterPath right_center_path;
    right_center_path.addPolygon(right_center_polygon);
    right_center->setShape(right_center_path);

    // Правая 3х очковая линии. Находится на расстоянии ? футов (6503.2 мм) от центра площадки Высота площадки 6 метров
    QPolygonF right_3_points_polygon;
    left_3_points_polygon << QPointF((m_size.m_half_length - m_size.m_ring_center_from_edge - m_size.m_3_point_line_radius), 0)
                          << QPointF((m_size.m_half_length - m_size.m_ring_center_from_edge - m_size.m_3_point_line_radius), m_size.m_height);
    QwtPlotShapeItem *right_3_points = new QwtPlotShapeItem("right_3_points");
    right_3_points->setRenderHint(QwtPlotItem::RenderAntialiased, true);
    right_3_points->attach(m_plot);
    right_3_points->setPen(QColor("gray"), 1, Qt::SolidLine);
    QPainterPath right_3_points_path;
    right_3_points_path.addPolygon(right_3_points_polygon);
    right_3_points->setShape(right_3_points_path);

    // Правая граница арки. Находится на расстоянии 22 футов (6705.6 мм) от центра площадки Высота площадки 6 метров
    QPolygonF right_arc_polygon;
    right_arc_polygon << QPointF((m_size.m_half_length - m_size.m_free_throw_line_from_edge - m_size.m_center_circle_radius), 0)
                      << QPointF((m_size.m_half_length - m_size.m_free_throw_line_from_edge - m_size.m_center_circle_radius), m_size.m_height);
    QwtPlotShapeItem *right_arc = new QwtPlotShapeItem("right_arc");
    right_arc->setRenderHint(QwtPlotItem::RenderAntialiased, true);
    right_arc->attach(m_plot);
    right_arc->setPen(QColor("gray"), 1, Qt::SolidLine);
    QPainterPath right_arc_path;
    right_arc_path.addPolygon(right_arc_polygon);
    right_arc->setShape(right_arc_path);

    // Правая штрафная линии. Находится на расстоянии 28 футов (8534.4 мм) от центра площадки Высота площадки 6 метров
    QPolygonF right_free_throw_polygon;
    right_free_throw_polygon << QPointF((m_size.m_half_length - m_size.m_free_throw_line_from_edge), 0)
                             << QPointF((m_size.m_half_length - m_size.m_free_throw_line_from_edge), m_size.m_height);
    QwtPlotShapeItem *right_free_throw = new QwtPlotShapeItem("right_free_throw");
    right_free_throw->setRenderHint(QwtPlotItem::RenderAntialiased, true);
    right_free_throw->attach(m_plot);
    right_free_throw->setPen(QColor("gray"), 1, Qt::SolidLine);
    QPainterPath right_free_throw_path;
    right_free_throw_path.addPolygon(right_free_throw_polygon);
    right_free_throw->setShape(right_free_throw_path);

    // Правая граница площадки. Ширина площадки 94 фута, половина ширины в мм 14325.6. Высота площадки 6 метров
    QPolygonF right_line_polygon;
    right_line_polygon << QPointF(m_size.m_half_length, 0) << QPointF(m_size.m_half_length, m_size.m_height);
    QwtPlotShapeItem *right_line = new QwtPlotShapeItem("right_line");
    right_line->setRenderHint(QwtPlotItem::RenderAntialiased, true);
    right_line->attach(m_plot);
    right_line->setPen(QColor("gray"), 1, Qt::SolidLine);
    QPainterPath right_line_path;
    right_line_path.addPolygon(right_line_polygon);
    right_line->setShape(right_line_path);
}

void Apl_Basketball_Court::draw_hoop(Plot_Type type, double x, double y, double z, const QString &color) noexcept
{
    switch (type)
    {
    case Plot_Type::APL_XY:
        draw_hoop_xy(x, y, color);
        break;
    case Plot_Type::APL_XZ:
        draw_hoop_xz(x, z, color);
        break;
    case Plot_Type::APL_YZ:
        draw_hoop_yz(y, z, color);
        break;
    default:
        break;
    }
}

void Apl_Basketball_Court::draw_hoop_xy(double x, double y, const QString &color) noexcept
{
    if (m_plot != nullptr)
    {
        double board_y = (y > 0 ? (y + 381) : (y - 381)); // 381мм - расстояние от щита до центра кольца
        QPolygonF left_backboard_polygon;
        left_backboard_polygon << QPointF(board_y, -1 * (m_size.m_backboard_length / 2) + x)
                               << QPointF(board_y, (m_size.m_backboard_length / 2) + x);
        QwtPlotShapeItem* left_backboard_line = new QwtPlotShapeItem("left_backboard_line");
        left_backboard_line->setRenderHint(QwtPlotItem::RenderAntialiased, true);
        left_backboard_line->attach(m_plot);
        left_backboard_line->setPen(QColor(color), 2, Qt::SolidLine);
        QPainterPath left_backboard_line_path;
        left_backboard_line_path.addPolygon(left_backboard_polygon);
        left_backboard_line->setShape(left_backboard_line_path);

        QwtPlotShapeItem* left_ring = new QwtPlotShapeItem("left_ring");
        left_ring->setRenderHint(QwtPlotItem::RenderAntialiased, true);
        left_ring->attach(m_plot);
        left_ring->setPen(QColor(color), 2, Qt::SolidLine);
        QPainterPath left_ring_path;
        left_ring_path.addEllipse(QPointF(y, x), m_size.m_ring_radius, m_size.m_ring_radius);
        left_ring->setShape(left_ring_path);
    }
}

void Apl_Basketball_Court::draw_hoop_xz(double x, double z, const QString &color) noexcept
{
    if (m_plot != nullptr)
    {
        QwtPlotShapeItem* ring_board = new QwtPlotShapeItem("ring_board");
        ring_board->setRenderHint(QwtPlotItem::RenderAntialiased, true);
        ring_board->attach(m_plot);
        ring_board->setPen(QColor(color), 2, Qt::SolidLine);
        QPainterPath ring_board_path;
        ring_board_path.addRect(QRectF(QPointF(-1 * (m_size.m_backboard_length / 2) + x, z - 150), // 150мм приблизительное расстояние от нижней границы щита до кольца
                                       QSize(m_size.m_backboard_length, m_size.m_backboard_width)));
        ring_board->setShape(ring_board_path);

        QPolygonF ring_polygon;
        ring_polygon << QPointF(x - m_size.m_ring_radius, z) << QPointF(x + m_size.m_ring_radius, z);
        QwtPlotShapeItem *ring_line = new QwtPlotShapeItem("ring_line");
        ring_line->setRenderHint(QwtPlotItem::RenderAntialiased, true);
        ring_line->attach(m_plot);
        ring_line->setPen(QColor(color), 2, Qt::SolidLine);
        QPainterPath ring_line_path;
        ring_line_path.addPolygon(ring_polygon);
        ring_line->setShape(ring_line_path);
    }
}

void Apl_Basketball_Court::draw_hoop_yz(double y, double z, const QString &color) noexcept
{
    if (m_plot != nullptr)
    {
        double board_y = (y > 0 ? (y + 381) : (y - 381)); // 381мм - расстояние от щита до центра кольца
        QPolygonF left_board_polygon;
        left_board_polygon << QPointF(board_y, m_size.m_backboard_from_floor - 150) // 150мм приблизительное расстояние от нижней границы щита до кольца
                           << QPointF(board_y, (m_size.m_backboard_from_floor + m_size.m_backboard_width - 150));
        QwtPlotShapeItem *left_board_line = new QwtPlotShapeItem("left_board_line");
        left_board_line->setRenderHint(QwtPlotItem::RenderAntialiased, true);
        left_board_line->attach(m_plot);
        left_board_line->setPen(QColor(color), 2, Qt::SolidLine);
        QPainterPath left_board_line_path;
        left_board_line_path.addPolygon(left_board_polygon);
        left_board_line->setShape(left_board_line_path);

        QPolygonF left_ring_polygon;
        left_ring_polygon << QPointF(y + m_size.m_ring_radius, z)
                          << QPointF(y - m_size.m_ring_radius, z);
        QwtPlotShapeItem *left_ring_line = new QwtPlotShapeItem("left_ring_line");
        left_ring_line->setRenderHint(QwtPlotItem::RenderAntialiased, true);
        left_ring_line->attach(m_plot);
        left_ring_line->setPen(QColor(color), 2, Qt::SolidLine);
        QPainterPath left_ring_line_path;
        left_ring_line_path.addPolygon(left_ring_polygon);
        left_ring_line->setShape(left_ring_line_path);
    }
}

Apl_Basketball_Court::~Apl_Basketball_Court()
{

}
