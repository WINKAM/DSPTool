#-------------------------------------------------
#-------------------------------------------------

QT += core gui
QT += widgets

CONFIG += c++11

TARGET = DSPTool_4_39_87
TEMPLATE = app

QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE += -O3

QMAKE_LFLAGS_RELEASE -= -O1

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

VERSION = 4.39.87
DEFINES += APP_VERSION=\\\"$$VERSION\\\"

CONFIG += GUI_CONFIG

CONFIG += WKTR_CONFIG

SOURCES += main.cpp \
    video/opencv_player.cpp \
    data/episode_meta_data.cpp \
    data/wlogdata.cpp \
    data/object/wobjectdata.cpp \
    data/object/wsensordata.cpp \
    data/object/wsignaldata.cpp \
    data/data_manager.cpp \
    app_controller.cpp \
    data/freqency_cutter.cpp \
    help_widget.cpp \
    label_mediator.cpp \
    main_menu_mediator.cpp \
    metronom.cpp \
    operation_mode.cpp \
    palette_manager.cpp \
    parser/apl120_log_parser.cpp \
    parser/base_parser.cpp \
    parser/apl_label_parser.cpp \
    data/label/log_labels.cpp \
    file_reader.cpp \
    parser/event_parser.cpp \
    parser/label_reader.cpp \
    parser/mb_log_parser.cpp \
    player_widget.cpp \
    plots_mediator.cpp \
    settings_manager.cpp \
    time_mediator.cpp \
    utils.cpp \
    file_writer.cpp \
    parser/apl_error_parser.cpp \
    apl_helper/facilitysetup_reader.cpp \
    apl_helper/wrap_apl_banchmark.cpp \
    parser/wk_log_parser.cpp \
    parser/alps_log_parser.cpp \
    data/label/alg_temp_labels.cpp \
    parser/apl25_log_parser.cpp \
    video_windows_mediator.cpp

HEADERS  += data/wlogdata.h \
   video//opencv_player.h \
    data/episode_meta_data.h \
    data/object/wobjectdata.h \
    data/object/wsensordata.h \
    data/object/wsignaldata.h \
    data/data_manager.h \
    app_controller.h \    \
    data/freqency_cutter.h \
    help_widget.h \
    label_mediator.h \
    main_menu_mediator.h \
    metronom.h \
    operation_mode.h \
    palette_manager.h \
    parser/apl120_log_parser.h \
    parser/base_parser.h \
    parser/apl_label_parser.h \
    data/label/log_labels.h \
    file_reader.h \
    parser/event_parser.h \
    parser/label_reader.h \
    parser/mb_log_parser.h \
    player_widget.h \
    plots_mediator.h \
    settings_manager.h \
    data/sample_structuries.h \
    time_mediator.h \
    utils.h \
    file_writer.h \
    parser/apl_error_parser.h \
    apl_helper/facilitysetup_reader.h \
    apl_helper/wrap_apl_banchmark.h \
    parser/wk_log_parser.h \
    parser/alps_log_parser.h \
    data/label/alg_temp_labels.h \
    parser/apl25_log_parser.h \
    video/speed_convertor.h \
    video_windows_mediator.h


INCLUDEPATH += /usr/local/include

INCLUDEPATH += $$(HOME)/Tools/opencv-4.2.0/include


DEFINES += GUI

RESOURCES += \
    res/res.qrc

LIBS += -L/$$(HOME)/Tools/opencv-4.2.0/build/lib -lopencv_core
LIBS += -L/$$(HOME)/Tools/opencv-4.2.0/build/lib -lopencv_highgui
LIBS += -L/$$(HOME)/Tools/opencv-4.2.0/build/lib -lopencv_imgcodecs
LIBS += -L/$$(HOME)/Tools/opencv-4.2.0/build/lib -lopencv_imgproc
LIBS += -L/$$(HOME)/Tools/opencv-4.2.0/build/lib -lopencv_features2d
LIBS += -L/$$(HOME)/Tools/opencv-4.2.0/build/lib -lopencv_video
LIBS += -L/$$(HOME)/Tools/opencv-4.2.0/build/lib -lopencv_videoio


LIBS += -L/usr/local/lib -lpng -ldl

INCLUDEPATH +=  $$(HOME)/Tools/qwt-6.1.3/src/
LIBS += -L/$$(HOME)/Tools/qwt-6.1.3/lib/ -lqwt

INCLUDEPATH += ../Algo_SDK/

SOURCES += \
    ../Algo_SDK/transform_hub.cpp \
    ../Algo_SDK/wrap_transformation.cpp \
    ../Algo_SDK/tester_filters.cpp \
    ../Algo_SDK/tester_algos.cpp \
    ../Algo_SDK/wrap_btw_4.cpp \
    ../Algo_SDK/wrap_btw_2.cpp

HEADERS  += \
    ../Algo_SDK/tester_filters.h \
    ../Algo_SDK/transform_hub.h \
    ../Algo_SDK/wrap_transformation.h \
    ../Algo_SDK/tester_algos.h \
    ../Algo_SDK/wrap_btw_4.h \
    ../Algo_SDK/wrap_btw_2.h

GUI_CONFIG {

SOURCES += main_window.cpp \
    view/apl_basketball_court.cpp \
    windows_controller.cpp \
    view/simple_checkable_qtreewidget.cpp \
    plot/base_plot_window.cpp \
    plot/court_plot_window.cpp \
    plot/time_plot_window.cpp \
    plot/plot_controller.cpp \
    video/video_window.cpp \
    view/viewer_plot.cpp \

HEADERS += \
    windows_controller.h \
    view/simple_checkable_qtreewidget.h \
    main_window.h \
    view/apl_basketball_court.h \
    plot/base_plot_window.h \
    plot/court_plot_window.h \
    plot/time_plot_window.h \
    plot/plot_controller.h \
    video/video_window.h \
    view/viewer_plot.h \
}


WKTR_CONFIG {
INCLUDEPATH += ../WKTR/Filters_Simple
INCLUDEPATH += ../WKTR/WKTR_Data_Structures
INCLUDEPATH += ../WKTR/Affine_Transformation
INCLUDEPATH += ../WKTR/Frequency_Normalizer

SOURCES += ../WKTR/Filters_Simple/low_pass_filter.cpp
SOURCES += ../WKTR/Filters_Simple/butterworth_lpf.cpp
SOURCES += ../WKTR/WKTR_Data_Structures/position_velocity_buffer.cpp
SOURCES += ../WKTR/WKTR_Data_Structures/average_sliding_window.cpp
SOURCES += ../WKTR/WKTR_Data_Structures/max_sliding_window.cpp
SOURCES += ../WKTR/Affine_Transformation/affine_transformation.cpp
SOURCES += ../WKTR/WKTR_Data_Structures/simple_features_detector.cpp
SOURCES += ../WKTR/Frequency_Normalizer/frequency_normalizer.cpp
SOURCES += ../WKTR/Frequency_Normalizer/frequency_normalizer_3d.cpp
SOURCES += ../WKTR/WKTR_Data_Structures/signal_quality_detector.cpp

HEADERS += ../WKTR/Filters_Simple/low_pass_filter.h
HEADERS += ../WKTR/Filters_Simple/butterworth_lpf.h
HEADERS += ../WKTR/WKTR_Data_Structures/position_velocity_buffer.h
HEADERS += ../WKTR/WKTR_Data_Structures/position_sample.h
HEADERS += ../WKTR/WKTR_Data_Structures/average_sliding_window.h
HEADERS += ../WKTR/WKTR_Data_Structures/max_sliding_window.h
HEADERS += ../WKTR/Affine_Transformation/affine_transformation.h
HEADERS += ../WKTR/WKTR_Data_Structures/simple_features_detector.h
HEADERS += ../WKTR/Frequency_Normalizer/frequency_normalizer.h
HEADERS += ../WKTR/Frequency_Normalizer/frequency_normalizer_3d.h
HEADERS += ../WKTR/WKTR_Data_Structures/signal_quality_detector.h


DEFINES += ALGO_WKTR_DEFINE
}
