/***************************************************************************

             WINKAM TM strictly confidential 02.06.2020

 ***************************************************************************/
#include "help_widget.h"
#include <QTabWidget>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>

Help_Widget::Help_Widget()
{

    QLabel* label_about = new QLabel("<p>DSPTool Basketball - Signal processing and labeling tool. It allows analysis and labeling of synced signals, multiple videos and labels with accuracy upto 1ms.</p> "
                                     "<p>We intended it for labeling, development and testing in the pre-production and production phases when high accuracy and consistency is essential.</p>"
                                     "<p>This version is specifically optimized for Basketball XYZ and IMU signals.</p>"
                                     "<p>WINKAM™© all rights reserved. <a href='http://winkam.com'>www.winkam.com</a></p>"
                                     "<p>If you have a question please contact us at <a href='algos@winkam.com'>algos@winkam.com</a> with DSPTool in the subject line.</p>");
    label_about->setOpenExternalLinks(true);
    label_about->setWordWrap(true);
    label_about->setMargin(10);
    label_about->setAlignment(Qt::AlignTop);

    QLabel* label_guide = new QLabel("<p>To avoid errors please use the ST dataset file naming and labeling format [<a href='https://docs.google.com/document/d/1AFGh6Zhreg8EZIS7DuyIg0z9pLmXREbISo2fUKQMd6w/edit?usp=sharing'>link</a>].</p>"
                                     "<p>Here is the folder to video guides [<a href='https://drive.google.com/drive/folders/1lSTc8m51PIqb7DAy1o_f8SMPKZT0Tzo3'>link</a>].</p>"
                                     "<p>Here is the folder with dataset example [<a href='https://drive.google.com/drive/folders/19afO44k9AGuo18ezK_cQv3MP0UyS3lp-'>link</a>].</p>");

    label_guide->setOpenExternalLinks(true);
    label_guide->setWordWrap(true);
    label_guide->setMargin(10);
    label_guide->setAlignment(Qt::AlignTop);


    QLabel* label_shortcuts = new QLabel("<p><b>General Shortcuts:</b></p>"
                                         "<p>F1 - Help and shortcuts</p>"
                                         "<p>Ctrl + X - Exit</p>"
                                         "<p><b>Opening data:</b></p>"
                                         "<p>Ctrl + O - Open file with the XYZ data (from the ST dataset).</p>"
                                         "<p>Videos and labels will be automatically added to quick access panels.</p>"
                                         "<p>Ctrl +  L - Adding extra labeling file to quick access </p>"
                                         "<p>Ctrl +  M - Opening meta-file</p>"
                                         "<p>Ctrl + V - Adding extra video to quick access</p>"
                                         "<p><b>Actions with plots:</b></p>"
                                         "<p>F5 - Apply signals settings and refresh plots</p>"
                                         "<p>Ctrl + scroll up (down) - Zoom in (out)</p>"
                                         "<p>Ctrl + R  OR Shift + Left click - Ruler</p>"
                                         "<p>CTRL + T - Zoom of the selected area</p>"
                                         "<p><b>Time navigation:</b></p>"
                                         "<p>W - Skip to the next frame/sample forward. </p>"
                                         "<p>Q - Skip to the previous frame/sample backward. </p>"
                                         "<p>S - Seek forward one second.</p>"
                                         "<p>A - Seek backward one second.</p>"
                                         "<p>Shift + W - Seek forward ten frames/samples. </p>"
                                         "<p>Shift + Q - Seek backward ten frames/samples. </p>"
                                         "<p>Shift + S - Seek forward ten seconds.</p>"
                                         "<p>Shift + A - Seek backward ten seconds.</p>"
                                         "<p><b>Labeling:</b></p>"
                                         "<p>P - copy current timestamp, ms</p>");

    label_shortcuts->setMargin(10);
    label_shortcuts->setAlignment(Qt::AlignTop);

    QLabel* label_settings = new QLabel("<p>If you need to work with the original signal turn off in the settings \"Work with 25 Hz signals\". It may affect the performance while working with a lot of sensors simultaneously.</p>"
                                        "<p>You can switch light / dark themes.</p>");


    label_settings->setMargin(10);
    label_settings->setAlignment(Qt::AlignTop);
    label_settings->setWordWrap(true);

    QLabel* label_license = new QLabel("<p>Non-commercial license:</p>"
                                       "<p>Winkam grants ShotTracker Inc (ShotTracker.com) for 2 years starting August 1, 2020 a non-exclusive, worldwide, royalty-free license to use"
                                       "DSPTool software of WINKAM by following means: internal R&D of signal processing and machine learning algorithm/filters/models, labeling (annotation), testing and debugging of the models/algorithms; without right of sale or right of sublicensing.</p>"
                                       "<p>WINKAM (C) all rights reserved.</p>"
                                       "<p><a href='http://winkam.com'>www.winkam.com</a></p>"
                                       "<p>Contact: <a href='algos@winkam.com'>algos@winkam.com</a></p>");


    label_license->setMargin(10);
    label_license->setAlignment(Qt::AlignTop);
    label_license->setWordWrap(true);


    QTabWidget* tab_bar = new QTabWidget();

    tab_bar->addTab(label_about, "About");
    tab_bar->addTab(label_guide, "Guide");
    tab_bar->addTab(label_shortcuts, "Shortcuts");
    tab_bar->addTab(label_settings, "Settings");
    tab_bar->addTab(label_license, "License");


    QPushButton* ok_close_btn = new QPushButton("OK");
    connect(ok_close_btn, &QPushButton::clicked, this, &Help_Widget::close);

    QVBoxLayout* main_layout = new QVBoxLayout();

    main_layout->addWidget(tab_bar);

    QHBoxLayout* h_layout = new QHBoxLayout();
    h_layout->addStretch();
    h_layout->addWidget(ok_close_btn);

    main_layout->addLayout(h_layout);

    setLayout(main_layout);
    setWindowTitle(QString("DSP Tool v.") + APP_VERSION + " Help");
    setFixedSize(650, 850);
    setWindowFlags(windowFlags() | Qt::WindowMinimizeButtonHint);
}
