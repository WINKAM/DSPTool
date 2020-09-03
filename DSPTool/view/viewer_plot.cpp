#include <qwt_plot_curve.h>
#include <qwt_legend.h>
#include <qwt_point_data.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_magnifier.h>
#include <qwt_plot_zoomer.h>
#include <qwt_symbol.h>
#include <qwt_plot_zoneitem.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_textlabel.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_marker.h>
#include <qwt_plot_shapeitem.h>
#include <qwt_scale_engine.h>
#include <qwt_scale_draw.h>
#include <qwaitcondition.h>
#include <QStaticText>
#include <QMouseEvent>
#include "viewer_plot.h"
#include "utils.h"
#include "qwt.h"
#include <QApplication>
#include <QtMath>
#include "data/episode_meta_data.h"

Plot_Picker::Plot_Picker(QWidget* canvas, const std::map<Plot_Marker_Type, std::vector<Plot_Marker* >* >* markers_map
                         , const std::vector<XYTimestamp_Curve> *ptr_curves, Plot_Type plot_type) : QwtPlotPicker(canvas)
{
    setTrackerMode(AlwaysOn);
    m_markers_map = markers_map;
    m_plot_type = plot_type;
    m_current_point = new QPointF;
    m_ptr_curves = ptr_curves;
}

class MyScaleDraw : public QwtScaleDraw
{
public:
    MyScaleDraw() : QwtScaleDraw() { }
    virtual ~MyScaleDraw() { }
    virtual QwtText label(double) const
    {
        return QwtText();
    }
};

class QScaleDraw : public QwtScaleDraw
{
public:

    explicit QScaleDraw(bool enableScientificNotation = false)
    : m_scientificNotationEnabled(enableScientificNotation)
    {

    }

    virtual QwtText label(double value) const override
    {
        if (m_scientificNotationEnabled)
        {
            return QwtScaleDraw::label(value);
        }
        else
        {
            return QwtText(QString::number(value, 'f', 0));
        }
    }

private:

    bool    m_scientificNotationEnabled;

};

QPointF* Plot_Picker::get_current_point() noexcept
{
    return m_current_point;
}

QwtText Plot_Picker::trackerTextF(const QPointF &pos) const
{
    m_current_point->setX(pos.x());
    m_current_point->setY(pos.y());

    QwtText qwt_text = QwtPlotPicker::trackerTextF(pos);
    QColor bg(Qt::white);
    bg.setAlpha(220);
    qwt_text.setBackgroundBrush(QBrush(bg));

    QString text;
    if (m_plot_type == Plot_Type::TIME)
    {
        text = qwt_text.text();
        QStringList tmp = qwt_text.text().split(",");
        text = tmp.at(1).split(".").front() + ";" + tmp.at(0).split(".").front();
        text += "\n" + UTILS::time_to_string(pos.x());
    }
    else if (m_plot_type == Plot_Type::APL_XY)
    {
        QStringList tmp = qwt_text.text().split(",");
        text = QString::number((tmp.at(1).toDouble() * -1)).split(".").front() + "; " + tmp.at(0).split(".").front();
    }
    else
    {
        QStringList tmp = qwt_text.text().split(",");
        text = tmp.at(1).split(".").front() + ";" + tmp.at(0).split(".").front();
    }

    if (m_markers_map != nullptr)
    {
        for (const auto &it : *m_markers_map)
        {
            std::vector<Plot_Marker* >* plot_markers_vector= it.second;
            for (const Plot_Marker* itt : *plot_markers_vector)
            {
                if (fabs(itt->m_marker->xValue() - pos.x()) < 150)
                {
                    text += "\n---\n" + itt->m_marker_description;
                }
            }
        }
    }

    if (m_plot_type != Plot_Type::TIME)
    {
        uint64_t closest_timestamp = 0;
        QString name_closest;

        for (auto curve : *m_ptr_curves)
        {
            double min_delta = std::abs(curve.m_samples.begin()->m_xy_point.x() - pos.x())
                    + std::abs(curve.m_samples.begin()->m_xy_point.y() - pos.y());
            for (auto p : curve.m_samples)
            {
                double delta = std::abs(p.m_xy_point.x() - pos.x())
                        + std::abs(p.m_xy_point.y() - pos.y());

                if (delta < min_delta)
                {
                    min_delta = delta;
                    if (min_delta < 150)
                    {
                        closest_timestamp = p.m_timestamp;
                        name_closest = curve.m_name;
                    }
                }
            }
            //qDebug() << "CURVE" << curve->data()->size();
        }
        if (closest_timestamp > 0)
        {
            text += "\nid " + name_closest + "; " + QString::number(closest_timestamp) + " ms";
        }
    }

    qwt_text.setText(text);

    return qwt_text;
}

Viewer_Plot::Viewer_Plot(Plot_Type plot_type, QWidget *parent): QwtPlot(parent)
{    
    m_plot_type = plot_type;

    switch (m_plot_type)
    {
    case Plot_Type::TIME:
        setAxisTitle(0, "Amplitude");
        setAxisTitle(2, "Time [ms]");
        break;

    case Plot_Type::APL_XY:
        setAxisTitle(0, "X [mm]");
        setAxisTitle(2, "Y [mm]");
        break;

    case Plot_Type::APL_XZ:
        setAxisTitle(0, "Z [mm]");
        setAxisTitle(2, "X [mm]");
        break;

    case Plot_Type::APL_YZ:
        setAxisTitle(0, "Z [mm]");
        setAxisTitle(2, "Y [mm]");
        break;
    }

    QwtPlotCanvas *canvas = new QwtPlotCanvas();
    canvas->setLineWidth(1);
    canvas->setFrameStyle(QFrame::Box | QFrame::Plain);

    QPalette canvasPalette(QColor(245, 245, 245)); // серый фон
    canvasPalette.setColor(QPalette::Foreground, Qt::gray); //серая рамка
    canvas->setPalette(canvasPalette);
    setCanvas(canvas);

    m_canvas = canvas;
    create_plot_elements();

    m_plot_picker = new Plot_Picker(canvas, &m_markers_map, &m_timestamp_curves, m_plot_type);

    m_plot_panner = new QwtPlotPanner(canvas);
    m_plot_panner->setEnabled(true);

    m_plot_zoomer = new QwtPlotZoomer(canvas);
    m_plot_zoomer->setMousePattern(QwtEventPattern::MouseSelect2, Qt::RightButton, Qt::ControlModifier);
    m_plot_zoomer->setMousePattern(QwtEventPattern::MouseSelect3, Qt::RightButton);
    m_plot_zoomer->setEnabled(false);

    m_plot_magnifier = new QwtPlotMagnifier(canvas); // zoom in/out with the wheel
    m_plot_magnifier->setWheelModifiers(Qt::KeyboardModifier::ControlModifier);

    m_is_central_line_visible = true;

    m_is_close = false;
    m_is_first_curve = true;

    m_is_ruler = false;

    setAxisScaleDraw(xBottom, new QScaleDraw);

    //    setAutoReplot(true);
}

void Viewer_Plot::create_plot_elements() noexcept
{
    m_grid = new QwtPlotGrid();
    m_grid->attach(this);
    m_grid->setPen(QColor(200, 200, 200), 2); //сетка!

    m_central_line = new QwtPlotMarker();
    m_central_line->setLineStyle(QwtPlotMarker::VLine);
    m_central_line->setLinePen("#FF7a7a", 2, Qt::DashLine);
    m_central_line->attach(this);
    m_central_line->setVisible(false);

    m_ruler_curve = nullptr;
    m_ruler_result = nullptr;
    m_ruler_points = new QPolygonF;
}


void Viewer_Plot::set_central_line_visible(bool is_visible) noexcept
{
    m_is_central_line_visible = is_visible;
}

void Viewer_Plot::set_grid_visible(bool value)
{
    m_grid->setVisible(value);
}

void Viewer_Plot::create_curve(const QString &name, QPolygonF *points, const QString &color_str, int weight
                               , const std::vector<XYTimestamp>& xytimestamp_samples)
{
    QwtPlotCurve* curve = new QwtPlotCurve(name);

    QColor color;
    color.setNamedColor(color_str);

    curve->setRenderHint(QwtPlotItem::RenderAntialiased, true);
    curve->attach(this);
    curve->setPen(color, weight / 2.0, Qt::SolidLine);
    curve->setSymbol(new QwtSymbol(QwtSymbol::Ellipse, QBrush(color), QPen(color, weight), QSize(weight, weight)));

    curve->setSamples(*points);

    m_curves_list.push_back(curve);

    QString view_name = name;

    if (m_plot_type != Plot_Type::TIME && Episode_Meta_Data::get_instance().is_meta_file_opened())
    {
        QVariant view_name_var = Episode_Meta_Data::get_instance().get_value(name.split(" ").front().toULong(), Meta_Parameter_Type::OBJECT_VIEW_ID);

        if (view_name_var.isValid())
        {
            view_name = view_name_var.toString();

            QwtPlotMarker *mark = new QwtPlotMarker();

            QwtText marker_text(view_name);
            marker_text.setColor("white");
            marker_text.setFont(QFont("Arial", 13, QFont::Bold));
            mark->setLabel(marker_text);
            mark->setValue(points->last().rx(), points->last().ry());
            mark->attach(this);
            m_id_object_markers.emplace_back(mark);
        }
    }

    if (!xytimestamp_samples.empty())
    {
        XYTimestamp_Curve xyt_curve;
        xyt_curve.m_name = "sensor: " + name.split(" ")[0];
        xyt_curve.m_samples = xytimestamp_samples;
        m_timestamp_curves.push_back(xyt_curve);

        QwtPlotCurve *position_marker = new QwtPlotCurve(name);
        position_marker->attach(this);
        position_marker->setSymbol(new QwtSymbol(QwtSymbol::Ellipse, QBrush(color), QPen(color, std::min(12, weight * 6)), QSize(std::min(12, weight * 6), std::min(12, weight * 6))));
        QPolygonF polygon;
        polygon << points->at(points->size() - 1);
        position_marker->setSamples(polygon);
        m_curves_list.push_back(position_marker);
    }

    curve->setVisible(weight > 0);


    //    if (m_curves_list.size() == 2 && m_plot_type == Plot_Type::TIME && m_is_first_curve)
    //    {
    //        m_is_first_curve = false;
    //        setAxisAutoScale(QwtPlot::yLeft, true);
    //        emit update_vertical_min_max_signal(axisScaleDiv(QwtPlot::yLeft).lowerBound(), axisScaleDiv(QwtPlot::yLeft).upperBound());
    //    }
}

void Viewer_Plot::add_markers(double x, double width, const QString &color, Plot_Marker_Type type, const QString &message)
{
    QwtPlotMarker* marker = new QwtPlotMarker();
    marker->setLineStyle(QwtPlotMarker::VLine);
    marker->attach(this);

    marker->setLinePen(QColor(color), width, Qt::DashLine);
    marker->setXValue(x);

    Plot_Marker* plot_marker = new Plot_Marker;
    plot_marker->m_marker = marker;
    plot_marker->m_marker_description = message;

    //if (!message.isEmpty())
    {
        auto it = m_markers_map.find(type);
        if (it == m_markers_map.end())
        {
            m_markers_map[type] = new std::vector<Plot_Marker* >();
        }
        m_markers_map[type]->push_back(plot_marker);
    }
    replot();
}

void Viewer_Plot::set_scale(double xMin, double xMax, double yMin, double yMax)
{
    if (m_plot_type == Plot_Type::TIME)
    {
        xMin = std::max(0., xMin);
        xMax = std::max(0., xMax);
    }

    if (m_plot_type == Plot_Type::TIME && m_is_first_curve)
    {
        m_is_first_curve = false;
        setAxisAutoScale(QwtPlot::yLeft, true);
        //emit update_vertical_min_max_signal(axisScaleDiv(QwtPlot::yLeft).lowerBound(), axisScaleDiv(QwtPlot::yLeft).upperBound());
    }
    else
    {
        setAxisScale(QwtPlot::yLeft, yMin, yMax);
    }

    setAxisScale(QwtPlot::xBottom, xMin, xMax);

    m_central_line->setXValue((xMin+xMax)/2);
    m_central_line->setVisible(m_is_central_line_visible);
    /*m_canvas->invalidateBackingStore();
    m_canvas->update()*/;
    replot();
}

std::vector<double> Viewer_Plot::get_scale() const noexcept
{
    return {axisScaleDiv(QwtPlot::xBottom).lowerBound(), axisScaleDiv(QwtPlot::xBottom).upperBound()
                , axisScaleDiv(QwtPlot::yLeft).lowerBound(), axisScaleDiv(QwtPlot::yLeft).upperBound()};
}


void Viewer_Plot::clear_markers_by_type(Plot_Marker_Type type) noexcept
{
    auto it = m_markers_map.find(type);
    if (it != m_markers_map.end())
    {
        std::vector<Plot_Marker* >* plot_marker_vector = it->second;

        if (plot_marker_vector != nullptr && plot_marker_vector->size() > 0)
        {
            for (Plot_Marker* itt : *plot_marker_vector)
            {
                delete itt->m_marker;
            }
            plot_marker_vector->clear();
        }
        delete plot_marker_vector;
        m_markers_map.erase(it);
    }

    if (!m_is_close)
    {
        replot();
    }
}

void Viewer_Plot::clear_id_objects() noexcept
{
    for (QwtPlotMarker* m : m_id_object_markers)
    {
        m->detach();
        delete m;
    }
    m_id_object_markers.clear();
}

void Viewer_Plot::clear_all_markers() noexcept
{
    clear_markers_by_type(Plot_Marker_Type::LABEL);
    clear_markers_by_type(Plot_Marker_Type::ERROR);
    clear_markers_by_type(Plot_Marker_Type::ALG);
    m_markers_map.clear();
}

void Viewer_Plot::clear_all_curves()
{
    for (int i = 0; i < m_curves_list.size(); ++i)
    {
        delete m_curves_list.at(i);
    }
    m_curves_list.clear();

    m_timestamp_curves.clear();
}

void Viewer_Plot::hide_axis_labels()
{
    setAxisScaleDraw(0, new MyScaleDraw);
    setAxisScaleDraw(2, new MyScaleDraw);
}

void Viewer_Plot::set_interation_type(bool isZoomer)
{
    if (m_plot_zoomer != 0)
    {
        m_plot_zoomer->setEnabled(isZoomer);
        m_plot_panner->setEnabled(!isZoomer);
    }
}

void Viewer_Plot::set_ruler(bool is_ruler)
{
    if (m_is_ruler == true && is_ruler == false)
    {
        if (m_ruler_curve != nullptr)
        {
            m_ruler_curve->detach();
            delete m_ruler_curve;
            m_ruler_curve = nullptr;
        }
        if (m_ruler_result != nullptr)
        {
            m_ruler_result->detach();
            delete m_ruler_result;
            m_ruler_result = nullptr;
        }
        m_ruler_points->clear();
        replot();
    }
    m_is_ruler = is_ruler;
}

void Viewer_Plot::set_auto_scale() noexcept
{
    setAxisAutoScale(QwtPlot::yLeft, true);
    setAxisAutoScale(QwtPlot::xBottom, true);
}

void Viewer_Plot::resizeEvent(QResizeEvent *event)
{
    QwtPlot::resizeEvent(event);
    emit update_resize_plot_signal(event->size());
}

void Viewer_Plot::mousePressEvent(QMouseEvent *me)
{    
    if (me->button() == Qt::LeftButton && (QApplication::keyboardModifiers().testFlag(Qt::ShiftModifier) || m_is_ruler))
    {
        if (m_ruler_points->size() >= 2)
        {
            m_ruler_points->clear();
        }
        *m_ruler_points << *m_plot_picker->get_current_point();

        if (m_ruler_curve != nullptr)
        {
            m_ruler_curve->detach();
            delete m_ruler_curve;
        }
        m_ruler_curve = new QwtPlotCurve();
        m_ruler_curve->attach(this);
        m_ruler_curve->setSymbol(new QwtSymbol(QwtSymbol::Hexagon, QBrush(QColor("#5f5f5f")), QPen(QColor("#5f5f5f"), 4), QSize(4, 4)));
        m_ruler_curve->setSamples(*m_ruler_points);
        replot();

        if (m_ruler_result != nullptr)
        {
            m_ruler_result->detach();
            delete m_ruler_result;
            m_ruler_result = nullptr;
        }

        if (m_ruler_points->size() == 2)
        {
            QwtText text("                " + get_length(m_ruler_points));
            text.setFont(QFont("Helvetica", 12, QFont::Bold));
            text.setColor(QColor("black"));
            //            text.setBorderPen(QPen(QColor(200,150,0), 1, Qt::DotLine));
            m_ruler_result = new QwtPlotMarker();
            m_ruler_result->setValue(m_ruler_points->at(1).x(), m_ruler_points->at(1).y());
            m_ruler_result->setLabel(text);
            m_ruler_result->attach(this);
        }
    }
    else if (me->button() == Qt::RightButton && (QApplication::keyboardModifiers().testFlag(Qt::ShiftModifier) || m_is_ruler))
    {
        if (m_ruler_curve != nullptr)
        {
            m_ruler_curve->detach();
            delete m_ruler_curve;
            m_ruler_curve = nullptr;
        }
        if (m_ruler_result != nullptr)
        {
            m_ruler_result->detach();
            delete m_ruler_result;
            m_ruler_result = nullptr;
        }
        m_ruler_points->clear();
        replot();
    }
    else
    {
        m_central_line->setVisible(false);
    }

    QwtPlot::mousePressEvent(me);
}

QString Viewer_Plot::get_length(QPolygonF* polygon) noexcept
{
    QString result_string;
    double result_t = 0.;
    double result_l = 0.;
    if (polygon->size() > 1)
    {
        for (int i = 1; i < polygon->size(); ++i)
        {
            if (m_plot_type == Plot_Type::TIME)
            {
                result_t += qFabs(polygon->at(i - 1).x() - polygon->at(i).x());
                result_string.append("T: " + QString::number(result_t) + "\n                ");
                result_l += qFabs(polygon->at(i - 1).y() - polygon->at(i).y());
                result_string.append("L: " + QString::number(result_l));
            }
            else
            {
                //                result += qSqrt((polygon->at(i - 1).x() - polygon->at(i).x()) * (polygon->at(i - 1).x() - polygon->at(i).x()) -
                //                           (polygon->at(i - 1).y() - polygon->at(i).y()) * (polygon->at(i - 1).y() - polygon->at(i).y()));
                //                qDebug() << "res D" << result;
                result_l += std::sqrt((polygon->at(i - 1).x() - polygon->at(i).x()) * (polygon->at(i - 1).x() - polygon->at(i).x())
                                      + (polygon->at(i - 1).y() - polygon->at(i).y()) * (polygon->at(i - 1).y() - polygon->at(i).y()));
                result_string.append("L: " + QString::number(result_l));
            }
        }
    }
    return result_string;
}

void Viewer_Plot::mouseReleaseEvent(QMouseEvent * me)
{
    Q_EMIT update_scale_signal(axisScaleDiv(QwtPlot::xBottom).lowerBound(), axisScaleDiv(QwtPlot::xBottom).upperBound()
                               , axisScaleDiv(QwtPlot::yLeft).lowerBound(), axisScaleDiv(QwtPlot::yLeft).upperBound());

    QwtPlot::mouseReleaseEvent(me);
}

void Viewer_Plot::wheelEvent(QWheelEvent *event)
{
//        qDebug() << (event->modifiers() & Qt::ControlModifier);

        Q_EMIT update_scale_signal(axisScaleDiv(QwtPlot::xBottom).lowerBound(), axisScaleDiv(QwtPlot::xBottom).upperBound()
                                   , axisScaleDiv(QwtPlot::yLeft).lowerBound(), axisScaleDiv(QwtPlot::yLeft).upperBound());

        QwtPlot::wheelEvent(event);
}

Viewer_Plot::~Viewer_Plot()
{
    m_is_close = true;
    clear_all_markers();

    clear_all_curves();

    delete m_plot_zoomer;
    delete m_plot_picker;
    delete m_plot_panner;
    delete m_central_line;
    delete m_plot_magnifier;
}
