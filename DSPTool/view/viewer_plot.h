#ifndef PLOTPICKER_H
#define PLOTPICKER_H

#define NUMBER_OF_CURVES 80 //число графиков, которые могут одновременно отображаться - не должно быть в новой версии

#include <qwt_plot.h>
#include <qwt_plot_picker.h>
#include <qwt_text.h>
#include <qwt_plot_marker.h>
#include <vector>

class QwtPlotCurve;
class QwtPlotZoomer;
class QwtPlotPanner;
class QMouseEvent;
class QwtPlotZoneItem;
class QwtPlotGrid;
class QwtPlotMagnifier;
class QwtPlotCanvas;

struct Plot_Marker
{
    QwtPlotMarker* m_marker;
    QString m_marker_description;
};

struct XYTimestamp
{
    QPointF m_xy_point;
    uint64_t m_timestamp;
    XYTimestamp(QPointF xy, uint64_t time) : m_xy_point(xy), m_timestamp(time) {}
    XYTimestamp() {}
};

struct XYTimestamp_Curve
{
    std::vector<XYTimestamp> m_samples;
    QString m_name;
};

enum class Plot_Type {TIME = 0, APL_XY = 1, APL_XZ = 2, APL_YZ = 3, LITE_TIME = 4};
enum Plot_Marker_Type {LABEL, ERROR, ALG};

//класс для отображения нужного текста под курсором
class Plot_Picker : public QwtPlotPicker
{
private:
    const std::map<Plot_Marker_Type, std::vector<Plot_Marker* >* >* m_markers_map;
    const std::vector<XYTimestamp_Curve>* m_ptr_curves;
    Plot_Type m_plot_type;
    QPointF* m_current_point;

public:    
    Plot_Picker(QWidget* canvas, const std::map<Plot_Marker_Type, std::vector<Plot_Marker* >* >* markers_map
                ,const std::vector<XYTimestamp_Curve>* ptr_curves, Plot_Type plot_type);
    QPointF* get_current_point() noexcept;

    virtual QwtText trackerTextF( const QPointF &pos ) const;
};

//класс для упрравления отображением графиков, масштабированием, нанесением меток
class Viewer_Plot : public QwtPlot
{
    Q_OBJECT

private:
    Plot_Picker *m_plot_picker; //объект для получения текущего положения мыши
    QwtPlotZoomer* m_plot_zoomer;//объект для зума
    QwtPlotPanner* m_plot_panner;//объект для перетаскивания плоскости графиков мышкой
    QwtPlotMarker* m_central_line;//вертикальная линия по центру плоскости (нужна при прокрутке графиков синхронно свидео)
    QwtPlotMagnifier* m_plot_magnifier;

    QwtPlotCanvas* m_canvas;

    QwtPlotGrid *m_grid;

    QVector<QwtPlotCurve *> m_curves_list;
    std::vector<XYTimestamp_Curve> m_timestamp_curves;

    std::map<Plot_Marker_Type, std::vector<Plot_Marker* >* > m_markers_map;
    std::vector<QwtPlotMarker*> m_id_object_markers;

    QPolygonF* m_ruler_points;
    QwtPlotCurve* m_ruler_curve;
    QwtPlotMarker* m_ruler_result;

    QList<QwtPlotZoneItem* > mZonerList;//список областей - вертикальных прямоугольников на графике - показывать состояния разметки
    QList<QColor> mColorList; //список цветов графиков
    QColor mColorMark; //цвет разметки
    QList<int> mPointSizeList; //список размеров точек графиков
    int mPointSizeMark; //задает толщину линий из разметки

    int mMarkersNumber;//число меток
    bool m_is_central_line_visible;//показывать ли центральную красную линию при задании окна

    bool m_is_close;

    Plot_Type m_plot_type;

    bool m_is_first_curve;

    bool m_is_ruler;

    QString get_length(QPolygonF* polygon) noexcept;

protected:

    //создает объект mCentralLine
    void create_plot_elements() noexcept;

    void resizeEvent(QResizeEvent *event);

public:
    Viewer_Plot(Plot_Type m_plot_type = Plot_Type::TIME, QWidget *parent = NULL);
    ~Viewer_Plot();
    void mousePressEvent(QMouseEvent * me);
    void mouseReleaseEvent(QMouseEvent * me);
    void wheelEvent(QWheelEvent *event);

    //задает данные для отображения на указанно графике
    void create_curve(const QString &name, QPolygonF *points, const QString &color_str = "red", int weight = 2,
                      const std::vector<XYTimestamp>& xytimestamp_samples = {});

    void set_grid_visible(bool value);

    //добавляет метку (type 0 alg.s, 1 transforms, 2 markers)
    void add_markers(double x, double width = 2, const QString &color = "black", Plot_Marker_Type type = Plot_Marker_Type::LABEL, const QString &message = "");

    //удаляет все метки
    void clear_markers_by_type(Plot_Marker_Type type) noexcept;
    void clear_all_markers() noexcept;

    void clear_id_objects() noexcept;

    //устанавливает указанный масштаб
    void set_scale(double xMin, double xMax, double yMin, double yMax);

    std::vector<double> get_scale() const noexcept;

    void clear_all_curves();

    void hide_axis_labels();

public Q_SLOTS:

    //переключение между режимами зуам и перетаскивания
    void set_interation_type(bool isZoomer);

    void set_ruler(bool is_ruler);

    //устанавливает видиомсть центральной красной линии, которая появляется при задании окна
    void set_central_line_visible(bool visible) noexcept;

    //включает автомасштабирование
    void set_auto_scale() noexcept;

Q_SIGNALS:
    void update_scale_signal(double minX, double maxX, double minY, double maxY);
    void update_resize_plot_signal(const QSize &size);
    void update_vertical_min_max_signal(double minY, double maxY);
    //void scaleChanged();

};

#endif // PLOTPICKER_H
