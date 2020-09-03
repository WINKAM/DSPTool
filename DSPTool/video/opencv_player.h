/***************************************************************************

             WINKAM TM strictly confidential 04.05.2020

 ***************************************************************************/
#ifndef OPENCV_PLAYER_H
#define OPENCV_PLAYER_H


#include <QFile>
#include <QImage>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/opencv.hpp"
//#include <QWaitCondition>

class QString;
class QTimer;

class OpenCV_Player
{
private:
    QImage m_qt_image;
    cv::Mat m_current_frame_mat;

    long m_frames_count;
    long m_current_frame_id;
    long m_video_lenght;
    int m_video_width;
    int m_video_height;

    cv::VideoCapture *m_video_capture;
    cv::VideoCapture *m_video_capture_30_fps; // only for videos >= 60 fps

    double m_frame_rate;
    int m_frame_rate_round;

protected:
    void create_image() noexcept;

public:
    OpenCV_Player();

    ~OpenCV_Player();

    QImage* get_frame();

    bool open(const QString &file_name) noexcept;

    void release();

    long get_current_frame() const noexcept;

    double get_frame_rate() const noexcept;

    int get_frame_rate_round() const noexcept;

    int get_video_width() const noexcept;

    int get_video_height() const noexcept;

    long get_video_frames_count() const noexcept;

    // returns 1 - the frame was loaded; 0 - the frame have been loaded already and you can get it via get_frame(); -1 - error, frame wasn't loaded
    int goto_frame_id(long id_frame) noexcept;
};

#endif // OPENCV_PLAYER_H
