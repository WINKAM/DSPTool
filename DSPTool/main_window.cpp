#include "main_window.h"
#include <QAction>
#include <QMenu>
#include <QMenuBar>
#include <QDebug>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QComboBox>
#include <QSpinBox>
#include <QPushButton>
#include <QLineEdit>
#include <QFileDialog>
#include <QTreeWidgetItemIterator>
#include <QTabWidget>
#include <QTime>
#include <QColorDialog>
#include <QTableWidget>
#include <QHeaderView>
#include <QCheckBox>
#include <QListWidget>
#include <QMessageBox>
#include <QApplication>
#include <QScreen>
#include <QShortcut>

#include "view/simple_checkable_qtreewidget.h"
#include "settings_manager.h"
#include "palette_manager.h"
#include "operation_mode.h"
#include "data/wlogdata.h"
#include "transform_hub.h"
#include "data/label/alg_temp_labels.h"
#include "main_menu_mediator.h"
#include "video_windows_mediator.h"
#include "label_mediator.h"
#include "data/episode_meta_data.h"

Main_Window::Main_Window(QWidget *parent) : QMainWindow(parent)
{
    create_gui();

    setWindowTitle(tr("DSP Tool v.") + APP_VERSION + " Controls");
    setFixedSize(sizeHint());

    long position_x = Settings_Manager::get_instance()->get_window_parameters("Main_Window", "position_x");
    long position_y = Settings_Manager::get_instance()->get_window_parameters("Main_Window", "position_y");

    int full_width = 0;
    for (QScreen *item : QGuiApplication::screens())
    {
        full_width += item->geometry().width();
    }
    if (full_width - position_x < geometry().width())
    {
        position_x = full_width - geometry().width();
    }
    if (QGuiApplication::primaryScreen()->geometry().height() - position_y < geometry().height())
    {
        position_y = QGuiApplication::primaryScreen()->geometry().height() - geometry().height();
    }
    move(position_x, position_y);

    setWindowFlag(Qt::Dialog);
}

void Main_Window::showEvent(QShowEvent* event)
{
    create_menu();
}

void Main_Window::create_gui()
{
    QWidget *central_widget = new QWidget();
    QVBoxLayout *central_layout = new QVBoxLayout(central_widget);
    setCentralWidget(central_widget);

    create_plots_settings();
    create_view_settings();
    create_labels_settings();
    create_video_settings();

    m_update_btn = new QPushButton("Apply signals settings and refresh plots");
    m_update_btn->setToolTip("apply signal settings and refresh plots (F5)");
    QObject::connect(m_update_btn, SIGNAL(clicked(bool)), this, SLOT(update_view_settings()));

    QHBoxLayout *settings_layout = new QHBoxLayout();

    QTabWidget* plots_settings_tab_bar = new QTabWidget();
    plots_settings_tab_bar->addTab(m_court_plots_signal_settings_tree, "Court view plots settings");
    plots_settings_tab_bar->addTab(m_time_plot_signal_settings_tree, "Time plot settings");

    QVBoxLayout *view_settings_layout = new QVBoxLayout();
    view_settings_layout->addWidget(plots_settings_tab_bar);

    QPushButton* default_signals_view_btn = new QPushButton("Default");
    default_signals_view_btn->setToolTip("apply default color and width scheme");
    connect(default_signals_view_btn, &QPushButton::clicked, this, &Main_Window::set_default_signals_view);

    QPushButton* save_signals_view_btn = new QPushButton("Save");
    save_signals_view_btn->setToolTip("save the current color and width scheme");
    connect(save_signals_view_btn, &QPushButton::clicked, this, &Main_Window::save_signals_view);

    QPushButton* load_signals_view_btn = new QPushButton("Load");
    load_signals_view_btn->setToolTip("load the saved color and width scheme");
    connect(load_signals_view_btn, &QPushButton::clicked, this, &Main_Window::load_signals_view);

    QHBoxLayout* signals_view_buttons_layout = new QHBoxLayout();
    signals_view_buttons_layout->addWidget(default_signals_view_btn);
    signals_view_buttons_layout->addWidget(save_signals_view_btn);
    signals_view_buttons_layout->addWidget(load_signals_view_btn);

    view_settings_layout->addLayout(signals_view_buttons_layout);
    view_settings_layout->addWidget(m_update_btn);

    settings_layout->addLayout(view_settings_layout);

    QVBoxLayout *plots_view_layout = new QVBoxLayout();

    plots_view_layout->addWidget(m_plots_tree);
    plots_view_layout->addWidget(m_labels_list);
    plots_view_layout->addWidget(m_video_list);
    settings_layout->addLayout(plots_view_layout);

    central_layout->addLayout(settings_layout);

    QHBoxLayout *filter_layout = new QHBoxLayout;
    filter_layout->addLayout(create_filters_settings());

    QPushButton *save_signal_button = new QPushButton;
    QObject::connect(save_signal_button, &QPushButton::clicked, this, &Main_Window::save_transformation);

    save_signal_button->setIcon(QIcon(":/image/save.png"));
    save_signal_button->setMinimumHeight(40);
    save_signal_button->setMinimumWidth(40);

    QPushButton *clear_tmp_labels_button = new QPushButton;
    QObject::connect(clear_tmp_labels_button, &QPushButton::clicked, [=] ()
    {
        if (Operation_Mode::get_instance().is_reading_data())
        {
            return;
        }
        qDebug() << "All temporary labels was cleared";
        Alg_Temp_Labels::get_instance().clear_all_labels();
        emit update_alg_tmp_labels();
    });
    clear_tmp_labels_button->setIcon(QIcon(":/image/delete.png"));
    clear_tmp_labels_button->setMinimumHeight(40);
    clear_tmp_labels_button->setMinimumWidth(40);

    QVBoxLayout* addition_button_layout = new QVBoxLayout();
    addition_button_layout->addWidget(save_signal_button);
    addition_button_layout->addWidget(clear_tmp_labels_button);

    filter_layout->addLayout(addition_button_layout);
    filter_layout->setAlignment(addition_button_layout, Qt::AlignTop);

    central_layout->addWidget(new QLabel("Filter settings:"));
    central_layout->addLayout(filter_layout);
}

// Создаем контролы для работы с плотами
void Main_Window::create_plots_settings()
{
    m_plots_tree = new Simple_Checkable_QTreeWidget(this);
    m_plots_tree->setHeaderLabel("Plots");
    m_plots_tree->setMaximumWidth(550);
    m_plots_tree->setMinimumWidth(550);
    m_plots_tree->setMaximumHeight(140);
    //    m_plots_tree->setMinimumHeight(350);
    QObject::connect(m_plots_tree, &Simple_Checkable_QTreeWidget::change_state, [=] (bool state, int parent_id, int child_id)
    {
        emit show_plot_signal(parent_id, child_id, state);
    });

    // test
    QTreeWidgetItem *plot1_item = new QTreeWidgetItem(m_plots_tree);
    plot1_item->setToolTip(0, "show/hide Time plot");
    plot1_item->setText(0, "Time plot");
    plot1_item->setCheckState(0, Qt::Unchecked);

    QTreeWidgetItem *plot2_xy_item = new QTreeWidgetItem(m_plots_tree);
    plot2_xy_item->setToolTip(0, "show/hide Top view plot");
    plot2_xy_item->setText(0, "Top view");
    plot2_xy_item->setCheckState(0, Qt::Unchecked);

    QTreeWidgetItem *plot2_yz_item = new QTreeWidgetItem(m_plots_tree);
    plot2_yz_item->setToolTip(0, "show/hide Front view plot");
    plot2_yz_item->setText(0, "Front view");
    plot2_yz_item->setCheckState(0, Qt::Unchecked);

    QTreeWidgetItem *plot2_xz_item = new QTreeWidgetItem(m_plots_tree);
    plot2_xz_item->setToolTip(0, "show/hide Side view plot");
    plot2_xz_item->setText(0, "Side view");
    plot2_xz_item->setCheckState(0, Qt::Unchecked);
}

void Main_Window::create_video_settings()
{
    m_video_list = new Simple_Checkable_QTreeWidget(this);
    m_video_list->setHeaderLabel("Videos");
    m_video_list->setMaximumWidth(550);
    m_video_list->setMinimumWidth(550);
    Video_Windows_Mediator::get_instance().set_list_widget(m_video_list);
}

void Main_Window::create_labels_settings()
{
    m_labels_list = new QTreeWidget(this);
    m_labels_list->setHeaderLabel("Labels");
    m_labels_list->setMaximumWidth(550);
    m_labels_list->setMinimumWidth(550);
    Label_Mediator::get_instance().set_list_widget_ptr(m_labels_list);
}

void Main_Window::create_view_settings()
{
    m_time_plot_signal_settings_tree = new Simple_Checkable_QTreeWidget(this);
    m_time_plot_signal_settings_tree->setToolTip("sets color and width of signals from Time plot");
    m_time_plot_signal_settings_tree->setColumnCount(1);
    m_time_plot_signal_settings_tree->setMinimumWidth(600);
    m_time_plot_signal_settings_tree->setMinimumHeight(550);
    m_time_plot_signal_settings_tree->setHeaderLabels(QStringList() << "Signals" << "Color and width");
    m_time_plot_signal_settings_tree->header()->setStretchLastSection(false);
    m_time_plot_signal_settings_tree->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    m_time_plot_signal_settings_tree->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);

    m_court_plots_signal_settings_tree = new Simple_Checkable_QTreeWidget(this);
    m_court_plots_signal_settings_tree->setToolTip("sets color and width of objects from court view plots");
    m_court_plots_signal_settings_tree->setColumnCount(1);
    m_court_plots_signal_settings_tree->setMinimumWidth(600);
    m_court_plots_signal_settings_tree->setMinimumHeight(550);
    m_court_plots_signal_settings_tree->setHeaderLabels(QStringList() << "Objects" << "Color and width");
    m_court_plots_signal_settings_tree->header()->setStretchLastSection(false);
    m_court_plots_signal_settings_tree->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    m_court_plots_signal_settings_tree->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);

}

// Создаем контролы для работы с преобразованиями, создаем таблицу и контролы ууправления
QVBoxLayout* Main_Window::create_filters_settings()
{
    QVBoxLayout *result = new QVBoxLayout();

    m_filters_table = new QTableWidget(this);
    m_filters_table->setColumnCount(8);
    m_filters_table->setMaximumHeight(100);
    m_filters_table->setSortingEnabled(false);
    m_filters_table->setHorizontalHeaderLabels(QStringList() << "W" <<"Filter" << "Log"
                                               << "Object" << "Sensor" << "Parameters" << "U" << "D");
    QHeaderView* header = m_filters_table->horizontalHeader();
    header->setSectionResizeMode(QHeaderView::Stretch);
    header->setSectionResizeMode(0, QHeaderView::Fixed);
    header->setSectionResizeMode(6, QHeaderView::Fixed);
    header->setSectionResizeMode(7, QHeaderView::Fixed);
    m_filters_table->setColumnWidth(0, 30);
    m_filters_table->setColumnWidth(6, 50);
    m_filters_table->setColumnWidth(7, 50);
    result->addWidget(m_filters_table);

    QHBoxLayout *add_filter_layout = new QHBoxLayout();
    m_filters_combo_box = new QComboBox();
    QObject::connect(m_filters_combo_box, static_cast<void (QComboBox::*)(const QString &)>(&QComboBox::currentIndexChanged)
                     , [=] (const QString & value)
    {
        if (value.length() > 0)
        {
            emit fill_filter_parameters_signal(value);
        }
    });
    m_logs_combo_box = new QComboBox();
    QObject::connect(m_logs_combo_box, static_cast<void (QComboBox::*)(const QString &)>(&QComboBox::currentIndexChanged)
                     , [=] (const QString & value)
    {
        if (value.length() > 0)
        {
            emit fill_objects_list_signal(value);
        }
    });
    m_objects_combo_box = new QComboBox();
    QObject::connect(m_objects_combo_box, static_cast<void (QComboBox::*)(const QString &)>(&QComboBox::currentIndexChanged)
                     , [=] (const QString & value)
    {
        if (value.length() > 0)
        {
            emit fill_sensors_list_signal(value);
        }
    });
    m_sensors_combo_box = new QComboBox();
    m_parameters_edit = new QLineEdit();
    m_parameters_edit->setMaximumWidth(150);

    add_filter_layout->addWidget(m_filters_combo_box);
    add_filter_layout->addWidget(m_logs_combo_box);
    add_filter_layout->addWidget(m_objects_combo_box);
    add_filter_layout->addWidget(m_sensors_combo_box);
    add_filter_layout->addWidget(m_parameters_edit);

    QPushButton *execute_filter_button = new QPushButton("Add");
    execute_filter_button->setMaximumWidth(70);
    QObject::connect(execute_filter_button, &QPushButton::clicked, [=] ()
    {
        if (Operation_Mode::get_instance().is_reading_data())
        {
            return;
        }

        QString filter_id = m_filters_combo_box->currentText();
        QString log_id = m_logs_combo_box->currentText();
        QString object_id = m_objects_combo_box->currentText();
        QString sensor_id = m_sensors_combo_box->currentText();
        QString parameters = m_parameters_edit->text();

        emit process_filter_signal(filter_id, log_id, object_id, sensor_id, parameters);
    });
    add_filter_layout->addWidget(execute_filter_button);

    result->addLayout(add_filter_layout);
    return result;
}

void Main_Window::create_menu() noexcept
{
    Main_Menu_Mediator::get_instance().set_main_window(this);
    setMenuBar(Main_Menu_Mediator::get_instance().create_menu_bar(this));
}

// Заполняем список доступных преобразований, так же выставляем значение и параметры последнего испольхуемого фильтра
void Main_Window::fill_filters_list(const std::map<QString, QString>* transforms_info_map) noexcept
{
    QString last_filter_id;
    QString last_parametres;
    Settings_Manager::get_instance()->read_last_filter(last_filter_id, last_parametres);

    int count = 0;
    int item = 0;
    for (const auto &tr : *transforms_info_map)
    {
        m_filters_combo_box->addItem(tr.first);
        if (last_filter_id.length() == tr.first.length() && last_filter_id.compare(tr.first) == 0)
        {
            item = count;
        }
        else
        {
            ++count;
        }
    }
    m_filters_combo_box->setCurrentIndex(item);
    m_parameters_edit->setText(last_parametres);
}

void Main_Window::fill_filter_parameters(const QString &parameters) noexcept
{
    m_parameters_edit->setText(parameters);
}

void Main_Window::clear_all_data()
{
    m_sensors_combo_box->clear();
    m_objects_combo_box->clear();
    m_logs_combo_box->clear();
    m_time_plot_signal_settings_tree->clear();
    m_court_plots_signal_settings_tree->clear();
}

// Заполняем дерево с настройками отображения сигналов
void Main_Window::fill_mainwindow_settings(const W_Log_Data& log_data)
{
    clear_all_data();

    //    QTreeWidgetItem *log_item = new QTreeWidgetItem(m_view_settings_tree);
    //    log_item->setText(0, log_data->get_name());
    //    log_item->setExpanded(true);

    m_logs_combo_box->addItem(log_data.get_name());

    {
        QTreeWidgetItem *select_all_item = new QTreeWidgetItem(m_time_plot_signal_settings_tree);
        select_all_item->setText(0, "ALL");
        select_all_item->setCheckState(0, Qt::CheckState::Unchecked);
        select_all_item->setExpanded(true);

        for (const auto& objects_map_item : *(log_data.get_objects()))
        {
            W_Object_Data *object = objects_map_item.second;
            QTreeWidgetItem *object_item = new QTreeWidgetItem();

            QString group = "?";
            QString view_name = "?";

            if (Episode_Meta_Data::get_instance().is_meta_file_opened())
            {
                QVariant view_name_var = Episode_Meta_Data::get_instance().get_value(object->get_name().toULong(), Meta_Parameter_Type::OBJECT_VIEW_ID);
                if (view_name_var.isValid())
                {
                    view_name = view_name_var.toString();
                }

                QVariant type = Episode_Meta_Data::get_instance().get_value(object->get_name().toULong(), Meta_Parameter_Type::OBJECT_TYPE);
                if (type.isValid())
                {
                    if (static_cast<Object_Type>(type.toInt()) == Object_Type::BALL)
                    {
                        group = "ball";
                    }
                    else
                    {
                        QVariant team_id = Episode_Meta_Data::get_instance().get_value(object->get_name().toULong(), Meta_Parameter_Type::PLAYER_TEAM_ID);
                        QVariant team_name = Episode_Meta_Data::get_instance().get_value(team_id.toInt(), Meta_Parameter_Type::TEAM_NAME);
                        if (team_name.isValid())
                        {
                            group = team_name.toString();
                        }
                    }
                }
            }

            object_item->setText(0, object->get_name() + ";   " + view_name  +  ";   " + group);


            //object_item->setCheckState(0, Qt::CheckState(object->is_show()));
            object_item->setCheckState(0, Qt::CheckState::Unchecked);
            object_item->setToolTip(0, "show/hide object's signals in Time plot");

            select_all_item->addChild(object_item);

            for (const auto & sensors_map_item : *object->get_sensors())
            {
                W_Sensor_Data *sensor = sensors_map_item.second;
                QTreeWidgetItem *sensor_item = new QTreeWidgetItem();
                sensor_item->setText(0, sensor->get_name());
                //sensor_item->setCheckState(0, Qt::CheckState(sensor->is_show()));
                sensor_item->setCheckState(0, Qt::CheckState::Unchecked);
                object_item->addChild(sensor_item);
                sensor_item->setExpanded(true);
                sensor_item->setToolTip(0, "show/hide sensor's signals in Time plot");

                m_time_plot_signal_settings_tree->setItemWidget(sensor_item, 1
                                                                , create_view_settings_item("", sensor->get_signals_weight()));

                for (const auto & signals_map_item : *sensor->get_signals())
                {
                    QTreeWidgetItem *signal_item = new QTreeWidgetItem();
                    signal_item->setText(0, signals_map_item.first);
                    //signal_item->setCheckState(0, Qt::CheckState(signals_map_item.second->is_show()));
                    signal_item->setCheckState(0, Qt::CheckState::Unchecked);
                    signal_item->setToolTip(0, "show/hide signal in Time plot");
                    sensor_item->addChild(signal_item);
                    m_time_plot_signal_settings_tree->setItemWidget(signal_item, 1
                                                                    , create_view_settings_item(*signals_map_item.second->get_color(), -1));
                }
            }
        }
    }

    {
        QTreeWidgetItem *select_all_item = new QTreeWidgetItem(m_court_plots_signal_settings_tree);
        select_all_item->setText(0, "ALL");
        select_all_item->setCheckState(0, Qt::CheckState::Unchecked);
        select_all_item->setExpanded(true);

        std::vector<QVariant> team_ids = Episode_Meta_Data::get_instance().get_values(Meta_Parameter_Type::TEAM_IDS);

        std::unordered_map<int, int> team_ids_tree_item_ids;

        int i = 0;
        for (const auto& e : team_ids)
        {
            if (e.isValid())
            {
                team_ids_tree_item_ids[e.toInt()] = i;

                QTreeWidgetItem *team = new QTreeWidgetItem();

                QVariant name = Episode_Meta_Data::get_instance().get_value(e.toInt(), Meta_Parameter_Type::TEAM_NAME);
                team->setText(0, (name.isValid() ? name.toString() : "team1"));
                team->setCheckState(0, Qt::CheckState::Unchecked);
                team->setExpanded(true);
                team->setToolTip(0, "show/hide team's players in Court view plots");
                select_all_item->addChild(team);
                m_court_plots_signal_settings_tree->setItemWidget(team, 1, create_view_settings_item("white", 2, team->text(0))); // color will be set after

                ++i;
            }
        }

        QTreeWidgetItem *balls = new QTreeWidgetItem();
        balls->setText(0, "balls");
        balls->setCheckState(0, Qt::CheckState::Unchecked);
        balls->setExpanded(true);
        balls->setToolTip(0, "show/hide balls in Court view plots");
        select_all_item->addChild(balls);
        m_court_plots_signal_settings_tree->setItemWidget(balls, 1, create_view_settings_item("white", 2, balls->text(0))); // color will be set after

        QTreeWidgetItem *others = new QTreeWidgetItem();
        others->setText(0, "others");
        others->setCheckState(0, Qt::CheckState::Unchecked);
        others->setExpanded(true);
        others->setToolTip(0, "show/hide others in Court view plots");
        select_all_item->addChild(others);
        m_court_plots_signal_settings_tree->setItemWidget(others, 1, create_view_settings_item("white", 2, others->text(0))); // color will be set after

        for (const auto& objects_map_item : *(log_data.get_objects()))
        {
            W_Object_Data *object = objects_map_item.second;

            for (const auto & sensors_map_item : *object->get_sensors())
            {
                QTreeWidgetItem *object_sensor_item = new QTreeWidgetItem();

                W_Sensor_Data *sensor = sensors_map_item.second;

                QVariant view_name_var = Episode_Meta_Data::get_instance().get_value(object->get_name().toULong(), Meta_Parameter_Type::OBJECT_VIEW_ID);
                if (view_name_var.isValid())
                {
                    object_sensor_item->setText(0, view_name_var.toString() + " " + sensor->get_name());
                }
                else
                {
                    object_sensor_item->setText(0, object->get_name() + " " + sensor->get_name());
                }
                object_sensor_item->setData(2, 0, object->get_name() + " " + sensor->get_name());

                object_sensor_item->setCheckState(0, Qt::CheckState::Unchecked);
                object_sensor_item->setToolTip(0, "show/hide object in Court view plots");

                QVariant type = Episode_Meta_Data::get_instance().get_value(object->get_name().toULong(), Meta_Parameter_Type::OBJECT_TYPE);

                bool is_groudp_found = false;

                if (type.isValid())
                {
                    if (type.toInt() == static_cast<int>(Object_Type::BALL))
                    {
                        balls->addChild(object_sensor_item);
                        is_groudp_found = true;
                    }
                    else if (type.toInt() == static_cast<int>(Object_Type::PLAYER))
                    {
                        QVariant team_id = Episode_Meta_Data::get_instance().get_value(object->get_name().toULong(), Meta_Parameter_Type::PLAYER_TEAM_ID);
                        if (team_id.isValid())
                        {
                            if (team_ids_tree_item_ids.find(team_id.toInt()) != team_ids_tree_item_ids.end())
                            {
                                select_all_item->child(team_ids_tree_item_ids[team_id.toInt()])->addChild(object_sensor_item);
                                is_groudp_found = true;
                            }
                        }
                    }
                }

                if (!is_groudp_found)
                {
                    others->addChild(object_sensor_item);
                }

                m_court_plots_signal_settings_tree->setItemWidget(object_sensor_item, 1
                                                                  , create_view_settings_item(*sensor->get_color()
                                                                                              , sensor->get_signals_weight()));
            }
        }
    }

    set_default_signals_view();
}

QWidget* Main_Window::create_view_settings_item(const QString &color_str, int sensor_weight, const QString& object_name)
{
    QWidget* item = new QWidget();
    QHBoxLayout *item_layout = new QHBoxLayout(item);

    if (sensor_weight >= 0)
    {
        QSpinBox *weight = new QSpinBox();
        weight->setObjectName(object_name);
        weight->setMinimum(1);
        weight->setMaximum(10);
        weight->setValue(sensor_weight);
        weight->setMinimumWidth(50);
        weight->setMaximumWidth(50);
        weight->setToolTip("set signal width" + (object_name.isEmpty() ? "" : (" for " + object_name)));

        connect(weight, SIGNAL(valueChanged(int)), this, SLOT(change_signal_weight(int)));

        item_layout->addWidget(weight);
    }

    if (!color_str.isEmpty())
    {
        QPushButton *colors = new QPushButton;
        colors->setObjectName(object_name);
        colors->setMinimumWidth(50);
        colors->setMaximumWidth(50);
        colors->setMinimumHeight(20);
        colors->setMaximumHeight(20);
        colors->setStyleSheet("QPushButton {background-color:" + color_str + "}");
        colors->setToolTip("set signal colors" + (object_name.isEmpty() ? "" : (" for " + object_name)));

        connect(colors, SIGNAL(clicked()), this, SLOT(change_button_color()));

        item_layout->addWidget(colors);
    }

    return item;
}

void Main_Window::fill_objects_list(W_Log_Data *log_data)
{
    m_objects_combo_box->clear();
    m_objects_combo_box->addItem("All");
    m_objects_combo_box->addItem("All players");
    m_objects_combo_box->addItem("All balls");
    m_objects_combo_box->insertSeparator(3);
    for (const auto &objects_map_item : *log_data->get_objects())
    {
        W_Object_Data *object = objects_map_item.second;
        m_objects_combo_box->addItem(object->get_name());
    }

    for (int id_row = 0; id_row < m_filters_table->rowCount(); ++id_row)
    {
        QComboBox* ptr_cb = (dynamic_cast<QComboBox*>(m_filters_table->cellWidget(id_row, 3)));
        QString current_object = ptr_cb->currentText();
        bool is_current_object_in_set = false;
        ptr_cb->clear();

        for (int i = 0; i < m_objects_combo_box->count(); ++i)
        {
            if (m_objects_combo_box->itemText(i).isEmpty())
            {
                continue;
            }

            if (!is_current_object_in_set && current_object.compare(m_objects_combo_box->itemText(i)) == 0)
            {
                is_current_object_in_set = true;
            }
            ptr_cb->addItem(m_objects_combo_box->itemText(i));
        }
        if (is_current_object_in_set)
        {
            ptr_cb->setCurrentText(current_object);
        }
    }
}

void Main_Window::fill_sensors_list(W_Object_Data *object_data)
{
    m_sensors_combo_box->clear();
    for (const auto &sensors_map_item : *object_data->get_sensors())
    {
        W_Sensor_Data *sensor = sensors_map_item.second;
        m_sensors_combo_box->addItem(sensor->get_name());
    }
}

void Main_Window::fill_sensors_list(const QStringList &sensor) noexcept
{
    m_sensors_combo_box->clear();
    m_sensors_combo_box->addItems(sensor);
}

// заполняем таблицу с фильтрами, а затем обновляем дерево с логом, так как у нас появился новый сенсор
void Main_Window::fill_filters_table(const W_Log_Data& log_data, const std::vector<WKTR::Transform_Spec *> &transf_spec_vector) noexcept
{
    m_filters_table->clear();
    m_filters_table->setHorizontalHeaderLabels(QStringList() << "W" << "Filter" << "Log"
                                               << "Object" << "Sensor" << "Parameters" << "U" << "D");

    int id = 0;
    m_filters_table->setRowCount(transf_spec_vector.size());
    for (const WKTR::Transform_Spec* spec : transf_spec_vector)
    {
        if (create_table_filter_row(id, log_data, *spec))
        {
            ++id;
        }
    }
    fill_mainwindow_settings(log_data);
    emit update_plots_signal();
}

// создается строка с примененым фильтром
int Main_Window::create_table_filter_row(int id, const W_Log_Data& log_data, const WKTR::Transform_Spec& spec) noexcept
{
    QWidget *selected_parent_widget = new QWidget;
    selected_parent_widget->setStyleSheet("QWidget {background-color: #e8e8e8}");
    QHBoxLayout *selected_layout = new QHBoxLayout(selected_parent_widget);
    QCheckBox *selected_filter = new QCheckBox("");
    selected_filter->setStyleSheet("QWidget {background-color: white}");
    selected_filter->setObjectName("selected_filter");
    selected_layout->addWidget(selected_filter);
    selected_layout->setAlignment(selected_filter, Qt::AlignHCenter);

    QComboBox* cb_filter = new QComboBox(m_filters_table);
    for (int i = 0; i < m_filters_combo_box->count(); ++i)
    {
        cb_filter->addItem(m_filters_combo_box->itemText(i));
    }
    cb_filter->setCurrentText(spec.m_filter_id);

    QComboBox* cb_log = new QComboBox(m_filters_table);
    cb_log->addItem(log_data.get_name());

    QComboBox* cb_object = new QComboBox(m_filters_table);
    for (const auto & object_item : *log_data.get_objects())
    {
        cb_object->addItem(object_item.first);
    }
    cb_object->setCurrentText(spec.m_object_id);

    QComboBox* cb_sensor = new QComboBox(m_filters_table);

    W_Object_Data* ptr_object = log_data.get_object(spec.m_object_id);

    if (ptr_object == nullptr)
    {
        emit delete_filter_signal(id);
        return 0;
    }

    for (const auto & sensors_item : *ptr_object->get_sensors())
    {
        cb_sensor->addItem(sensors_item.first);
    }
    cb_sensor->setCurrentText(spec.m_sensor_id);

    QPushButton* u_button = new QPushButton(m_filters_table);
    u_button->setObjectName(QString::number(id));
    u_button->setIcon(QIcon(":/image/update.png"));
    QObject::connect(u_button, &QPushButton::clicked, [=] ()
    {
        int id = u_button->objectName().toInt();

        // todo use is_selected parameter to write new signal in file
        bool is_selected = ((QCheckBox*) m_filters_table->cellWidget(id, 0)->findChild<QCheckBox*>("selected_filter"))->isChecked();
        QString filter_id = (dynamic_cast<QComboBox*>(m_filters_table->cellWidget(id, 1)))->currentText();
        QString log_id = (dynamic_cast<QComboBox*>(m_filters_table->cellWidget(id, 2)))->currentText();
        QString object_id = (dynamic_cast<QComboBox*>(m_filters_table->cellWidget(id, 3)))->currentText();
        QString sensor_id = (dynamic_cast<QComboBox*>(m_filters_table->cellWidget(id, 4)))->currentText();
        QString parameters = (dynamic_cast<QLineEdit*>(m_filters_table->cellWidget(id, 5)))->text();

        emit update_filter_signal(id, filter_id, log_id, object_id, sensor_id, parameters);
    });

    QPushButton* d_button = new QPushButton(m_filters_table);
    d_button->setObjectName(QString::number(id));
    d_button->setIcon(QIcon(":/image/delete.png"));
    QObject::connect(d_button, &QPushButton::clicked, [=] ()
    {
        emit delete_filter_signal(d_button->objectName().toInt());
    });

    QLineEdit* le = new QLineEdit(spec.m_parameters, m_filters_table);

    m_filters_table->setCellWidget(id, 0, selected_parent_widget);
    m_filters_table->setCellWidget(id, 1, cb_filter);
    m_filters_table->setCellWidget(id, 2, cb_log);
    m_filters_table->setCellWidget(id, 3, cb_object);
    m_filters_table->setCellWidget(id, 4, cb_sensor);
    m_filters_table->setCellWidget(id, 5, le);
    m_filters_table->setCellWidget(id, 6, u_button);
    m_filters_table->setCellWidget(id, 7, d_button);

    return 1;
}

void Main_Window::change_button_color() noexcept
{
    QPushButton *ptrPB = (QPushButton*)sender();
    QPalette palette = ptrPB->palette();

    QColor color = QColorDialog::getColor(ptrPB->palette().color(QPalette::Background), this);
    if (color.isValid())
    {
        palette.setColor(QPalette::Background, color);
        ptrPB->setStyleSheet("QPushButton {background-color:" + color.name() + "}");
    }

    if (!ptrPB->objectName().isEmpty())
    {
        QTreeWidgetItem * select_all_item = m_court_plots_signal_settings_tree->topLevelItem(0);
        for (int i = 0; i < select_all_item->childCount(); ++i)
        {
            QTreeWidgetItem *group_item = select_all_item->child(i);
            if (group_item->text(0).compare(ptrPB->objectName()) == 0)
            {
                for (int j = 0; j < group_item->childCount(); ++j)
                {
                    QTreeWidgetItem *sensor_item = group_item->child(j);

                    QWidget *sensor_widget = m_court_plots_signal_settings_tree->itemWidget(sensor_item, 1);
                    ((QPushButton *) sensor_widget->findChild<QPushButton*>())->setStyleSheet("QPushButton {background-color:"
                                                                                              + color.name() + "}");
                }
            }
        }
    }
}

void Main_Window::change_signal_weight(int w) noexcept
{
    QSpinBox *ptrSB = (QSpinBox*)sender();

    if (!ptrSB->objectName().isEmpty())
    {
        QTreeWidgetItem * select_all_item = m_court_plots_signal_settings_tree->topLevelItem(0);
        for (int i = 0; i < select_all_item->childCount(); ++i)
        {
            QTreeWidgetItem *group_item = select_all_item->child(i);
            if (group_item->text(0).compare(ptrSB->objectName()) == 0)
            {
                for (int j = 0; j < group_item->childCount(); ++j)
                {
                    QTreeWidgetItem *sensor_item = group_item->child(j);

                    QWidget *sensor_widget = m_court_plots_signal_settings_tree->itemWidget(sensor_item, 1);
                    ((QSpinBox *) sensor_widget->findChild<QSpinBox*>())->setValue(w);
                }
            }
        }
    }
}

void Main_Window::open_plot_by_id(int plot_id) noexcept
{
    m_plots_tree->topLevelItem(plot_id)->setCheckState(0, Qt::Checked);
}

void Main_Window::close_plot_by_id(int plot_id) noexcept
{
    m_plots_tree->topLevelItem(plot_id)->setCheckState(0, Qt::Unchecked);
}

void Main_Window::set_default_signals_view() noexcept
{    
    QTreeWidgetItem *select_all_item = m_time_plot_signal_settings_tree->topLevelItem(0);

    if (select_all_item == nullptr)
    {
        return;
    }

    qDebug() << "apply default color and width scheme";

    for (int i = 0; i < select_all_item->childCount(); ++i)
    {
        QTreeWidgetItem *object_item = select_all_item->child(i);
        for (int j = 0; j < object_item->childCount(); ++j)
        {
            QTreeWidgetItem *sensor_item = object_item->child(j);
            QWidget *sensor_widget = m_time_plot_signal_settings_tree->itemWidget(sensor_item, 1);

            ((QSpinBox *) sensor_widget->findChild<QSpinBox*>())->setValue(2);

            for (int k = 0; k < sensor_item->childCount(); ++k)
            {
                QTreeWidgetItem *signal_item = sensor_item->child(k);
                QWidget *signal_widget = m_time_plot_signal_settings_tree->itemWidget(signal_item, 1);
                QString signal_id = signal_item->text(0);
                if (signal_id.compare("x") == 0)
                {
                    ((QPushButton*)signal_widget->findChild<QPushButton*>())->setStyleSheet("QPushButton {background-color: red}");
                }
                else if (signal_id.compare("y") == 0)
                {
                    ((QPushButton*)signal_widget->findChild<QPushButton*>())->setStyleSheet("QPushButton {background-color: green}");
                }
                else if (signal_id.compare("z") == 0)
                {
                    ((QPushButton*)signal_widget->findChild<QPushButton*>())->setStyleSheet("QPushButton {background-color: blue}");
                }
                else
                {
                    ((QPushButton*)signal_widget->findChild<QPushButton*>())->setStyleSheet("QPushButton {background-color: black}");
                }
            }
        }
    }

    select_all_item = m_court_plots_signal_settings_tree->topLevelItem(0);

    QStringList default_team_colors = {"blue", "green", "red", "megenta", "cyan"};
    int default_color_id = 0;

    for (int i = 0; i < select_all_item->childCount(); ++i)
    {
        QTreeWidgetItem *group_item = select_all_item->child(i);

        QWidget *group_widget = m_court_plots_signal_settings_tree->itemWidget(group_item, 1);

        ((QSpinBox *) group_widget->findChild<QSpinBox*>())->setValue(2);

        QString color_name;

        if (group_item->text(0).contains("balls"))
        {
            color_name = "#FF6B00";
        }
        else if (group_item->text(0).contains("others"))
        {
            color_name = "darkGray";
        }
        else
        {
            QVariant team_color = Episode_Meta_Data::get_instance().get_value(group_item->text(0), Meta_Parameter_Type::TEAM_COLOR);
            if (team_color.isValid() && QColor::isValidColor(team_color.toString()))
            {
                color_name = team_color.toString();
            }
            else
            {
                color_name = default_team_colors[default_color_id];
                default_color_id = (default_color_id < default_team_colors.size() - 1) ? default_color_id + 1 : 0;
            }
        }

        ((QPushButton *) group_widget->findChild<QPushButton*>())->setStyleSheet("QPushButton {background-color: " + color_name + " }");

        for (int j = 0; j < group_item->childCount(); ++j)
        {
            QTreeWidgetItem *sensor_item = group_item->child(j);

            QWidget *sensor_widget = m_court_plots_signal_settings_tree->itemWidget(sensor_item, 1);

            ((QSpinBox *) sensor_widget->findChild<QSpinBox*>())->setValue(2);
            ((QPushButton *) sensor_widget->findChild<QPushButton*>())->setStyleSheet("QPushButton {background-color: " +  color_name + "}");
        }
    }

    update_view_settings();
}

void Main_Window::save_signals_view() noexcept
{
    QTreeWidgetItem *select_all_item = m_time_plot_signal_settings_tree->topLevelItem(0);
    if (select_all_item == nullptr)
    {
        return;
    }

    QStringList episode_name_splited = Operation_Mode::get_instance().get_log_name().split("/");
    QString episode_name;
    if (episode_name_splited.size() > 2)
    {
        episode_name = *(episode_name_splited.end() - 3);
    }
    else
    {
        return;
    }

    episode_name = episode_name.replace(" ", "_");

    for (int i = 0; i < select_all_item->childCount(); ++i)
    {
        QTreeWidgetItem *object_item = select_all_item->child(i);
        for (int j = 0; j < object_item->childCount(); ++j)
        {
            QTreeWidgetItem *sensor_item = object_item->child(j);
            QWidget *sensor_widget = m_time_plot_signal_settings_tree->itemWidget(sensor_item, 1);

            Settings_Manager::get_instance()->write_signal_view_setting(episode_name, object_item->text(0) + "_" + sensor_item->text(0), "-"
                                                                        , ((QSpinBox *) sensor_widget->findChild<QSpinBox*>())->value()
                                                                        , sensor_item->checkState(0));

            for (int k = 0; k < sensor_item->childCount(); ++k)
            {
                QTreeWidgetItem *signal_item = sensor_item->child(k);
                QWidget *signal_widget = m_time_plot_signal_settings_tree->itemWidget(signal_item, 1);

                Settings_Manager::get_instance()->write_signal_view_setting(episode_name, object_item->text(0) + "_" + sensor_item->text(0) + "_" + signal_item->text(0)
                                                                            , ((QPushButton*)signal_widget->findChild<QPushButton*>())
                                                                            ->palette().background().color().name(), 0, signal_item->checkState(0));

            }
        }
    }

    select_all_item = m_court_plots_signal_settings_tree->topLevelItem(0);

    for (int i = 0; i < select_all_item->childCount(); ++i)
    {
        QTreeWidgetItem *group_item = select_all_item->child(i);

        QWidget *group_widget = m_court_plots_signal_settings_tree->itemWidget(group_item, 1);

        Settings_Manager::get_instance()->write_signal_view_setting(episode_name, group_item->text(0)
                                                                    , ((QPushButton*)group_widget->findChild<QPushButton*>())
                                                                    ->palette().background().color().name()
                                                                    , ((QSpinBox *) group_widget->findChild<QSpinBox*>())->value()
                                                                    , group_item->checkState(0));

        for (int j = 0; j < group_item->childCount(); ++j)
        {
            QTreeWidgetItem *sensor_item = group_item->child(j);

            QWidget *sensor_widget = m_court_plots_signal_settings_tree->itemWidget(sensor_item, 1);

            Settings_Manager::get_instance()->write_signal_view_setting(episode_name, group_item->text(0) + "_" + sensor_item->text(0)
                                                                        , ((QPushButton*)sensor_widget->findChild<QPushButton*>())
                                                                        ->palette().background().color().name()
                                                                        , ((QSpinBox *) sensor_widget->findChild<QSpinBox*>())->value()
                                                                        , sensor_item->checkState(0));
        }
    }
}

void Main_Window::load_signals_view() noexcept
{
    QTreeWidgetItem *select_all_item = m_time_plot_signal_settings_tree->topLevelItem(0);
    if (select_all_item == nullptr)
    {
        return;
    }

    QStringList episode_name_splited = Operation_Mode::get_instance().get_log_name().split("/");
    QString episode_name;
    if (episode_name_splited.size() > 2)
    {
        episode_name = *(episode_name_splited.end() - 3);
    }
    else
    {
        return;
    }

    episode_name = episode_name.replace(" ", "_");

    qDebug() << "load the saved color and width scheme";

    QString color;
    int widht;
    int state;

    for (int i = 0; i < select_all_item->childCount(); ++i)
    {
        QTreeWidgetItem *object_item = select_all_item->child(i);
        for (int j = 0; j < object_item->childCount(); ++j)
        {
            QTreeWidgetItem *sensor_item = object_item->child(j);
            QWidget *sensor_widget = m_time_plot_signal_settings_tree->itemWidget(sensor_item, 1);

            Settings_Manager::get_instance()->read_signal_view_setting(episode_name, object_item->text(0) + "_" + sensor_item->text(0)
                                                                       , &color, &widht, &state);

            ((QSpinBox *) sensor_widget->findChild<QSpinBox*>())->setValue(widht);
            sensor_item->setCheckState(0, Qt::CheckState(state));

            for (int k = 0; k < sensor_item->childCount(); ++k)
            {
                QTreeWidgetItem *signal_item = sensor_item->child(k);
                QWidget *signal_widget = m_time_plot_signal_settings_tree->itemWidget(signal_item, 1);

                Settings_Manager::get_instance()->read_signal_view_setting(episode_name, object_item->text(0) + "_" + sensor_item->text(0) + "_" + signal_item->text(0)
                                                                           , &color, &widht, &state);

                ((QPushButton *) signal_widget->findChild<QPushButton*>())->setStyleSheet("QPushButton {background-color:" + color + "}");

                signal_item->setCheckState(0, Qt::CheckState(state));
            }
        }
    }

    select_all_item = m_court_plots_signal_settings_tree->topLevelItem(0);

    for (int i = 0; i < select_all_item->childCount(); ++i)
    {
        QTreeWidgetItem *group_item = select_all_item->child(i);

        QWidget *group_widget = m_court_plots_signal_settings_tree->itemWidget(group_item, 1);

        Settings_Manager::get_instance()->read_signal_view_setting(episode_name, group_item->text(0)
                                                                   , &color, &widht, &state);

        ((QPushButton *) group_widget->findChild<QPushButton*>())->setStyleSheet("QPushButton {background-color:" + color + "}");
        ((QSpinBox *) group_widget->findChild<QSpinBox*>())->setValue(widht);
        group_item->setCheckState(0, Qt::CheckState(state));

        for (int j = 0; j < group_item->childCount(); ++j)
        {
            QTreeWidgetItem *sensor_item = group_item->child(j);

            QWidget *sensor_widget = m_court_plots_signal_settings_tree->itemWidget(sensor_item, 1);

            Settings_Manager::get_instance()->read_signal_view_setting(episode_name, group_item->text(0) + "_" + sensor_item->text(0)
                                                                       , &color, &widht, &state);

            ((QPushButton *) sensor_widget->findChild<QPushButton*>())->setStyleSheet("QPushButton {background-color:" + color + "}");
            ((QSpinBox *) sensor_widget->findChild<QSpinBox*>())->setValue(widht);
            sensor_item->setCheckState(0, Qt::CheckState(state));

        }
    }

    update_view_settings();
}

// Тут происходит считывание данныех с дерева лога, и все выставленные настройки (цвет, ширина линий)
// отправляются чтобы примениться. Те данные, что отмеченные в чекбоксе будут нарисованы на плотах
void Main_Window::update_view_settings()
{
    if (Operation_Mode::get_instance().is_reading_data() || m_time_plot_signal_settings_tree->topLevelItemCount() == 0)
    {
        return;
    }

    qDebug() << "apply signal settings and refresh plots";

    QTreeWidgetItem* select_all_item = m_court_plots_signal_settings_tree->topLevelItem(0);
    QString plot_id = "Court plot";

    for (int i = 0; i < select_all_item->childCount(); ++i)
    {
        QTreeWidgetItem *group_item = select_all_item->child(i);
        for (int j = 0; j < group_item->childCount(); ++j)
        {
            QTreeWidgetItem *sensor_item = group_item->child(j);
            QString object_id = sensor_item->data(2, 0).toString().split(" ").front();
            int is_show = sensor_item->checkState(0);

            QString sensor_id = sensor_item->data(2, 0).toString().split(" ").back();
            //sensor_id.remove(object_id + " ");
            QWidget *sensor_widget = m_court_plots_signal_settings_tree->itemWidget(sensor_item, 1);

            int weight = ((QSpinBox *) sensor_widget->findChild<QSpinBox*>())->value();
            QString color = ((QPushButton *) sensor_widget->findChild<QPushButton*>())->palette().background().color().name();

            emit update_object_settings_signal(object_id, is_show, plot_id);
            emit update_view_settings_signal(object_id, sensor_id, is_show, color, weight, plot_id);
        }
    }

     select_all_item = m_time_plot_signal_settings_tree->topLevelItem(0);
     plot_id = "Time plot";

    for (int i = 0; i < select_all_item->childCount(); ++i)
    {
        QTreeWidgetItem *object_item = select_all_item->child(i);
        QString object_id = object_item->text(0).split("; ").front();
        int is_show = object_item->checkState(0);
        for (int j = 0; j < object_item->childCount(); ++j)
        {
            QTreeWidgetItem *sensor_item = object_item->child(j);
            QString sensor_id = sensor_item->text(0);
            QWidget *sensor_widget = m_time_plot_signal_settings_tree->itemWidget(sensor_item, 1);

            int weight = ((QSpinBox *) sensor_widget->findChild<QSpinBox*>())->value();
            int is_show = sensor_item->checkState(0);

            for (int k = 0; k < sensor_item->childCount(); ++k)
            {
                QTreeWidgetItem *signal_item = sensor_item->child(k);
                QString signal_id = signal_item->text(0);
                int is_show = signal_item->checkState(0);
                QWidget *signal_widget = m_time_plot_signal_settings_tree->itemWidget(signal_item, 1);
                QString color = ((QPushButton *) signal_widget->findChild<QPushButton*>())->palette().background().color().name();
                emit update_signal_settings_signal(object_id, sensor_id, signal_id, is_show, color);

            }
            emit update_view_settings_signal(object_id, sensor_id, is_show, "blue", weight, plot_id);
        }
        emit update_object_settings_signal(object_id, is_show, plot_id);
    }



    emit update_plots_signal();
}

// Проходим по всей таблице фильтров и собираем данные
void Main_Window::save_transformation() noexcept
{
    if (Operation_Mode::get_instance().is_reading_data())
    {
        return;
    }

    emit process_filter_all_data(m_filters_combo_box->currentText(),
                                 m_objects_combo_box->currentText(),
                                 m_parameters_edit->text());
}

void Main_Window::closeEvent(QCloseEvent *event)
{
    Settings_Manager::get_instance()->write_window_parameters("Main_Window", "position_x", geometry().x());
    Settings_Manager::get_instance()->write_window_parameters("Main_Window", "position_y", geometry().y());
    Settings_Manager::get_instance()->write_last_filter(m_filters_combo_box->currentText(), m_parameters_edit->text());
    Settings_Manager::get_instance()->write_palette(Palette_Manager::get_palette_id());
    Settings_Manager::get_instance()->write_is_only_low_freq_flag(Operation_Mode::get_instance().is_only_low_freq_opening());

    std::vector<int> opened_plots;
    for (int i = 0; i < m_plots_tree->topLevelItemCount(); ++i)
    {
        if (m_plots_tree->topLevelItem(i)->checkState(0) == Qt::Checked)
        {
            opened_plots.emplace_back(i);
        }
    }
    Settings_Manager::get_instance()->write_ids_of_opened_plots(opened_plots);

    emit close_window_signal(event);
}

Main_Window::~Main_Window()
{
}
