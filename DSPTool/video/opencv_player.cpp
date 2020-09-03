/***************************************************************************

             WINKAM TM strictly confidential 04.05.2020

 ***************************************************************************/
#include "opencv_player.h"
#include <QDebug>

OpenCV_Player::OpenCV_Player()
{
    m_video_capture = nullptr;
    m_video_capture_30_fps = nullptr;
}

QImage* OpenCV_Player::get_frame()
{
    return &m_qt_image;
}

bool OpenCV_Player::open(const QString &file_name) noexcept
{
    release();

    m_current_frame_id = 1;

    m_video_capture = new cv::VideoCapture(file_name.toStdString());

    if (m_video_capture->isOpened())
    {
        m_frames_count = static_cast<long>(m_video_capture->get(cv::CAP_PROP_FRAME_COUNT));
        m_video_lenght = static_cast<long>((1. * m_frames_count / m_video_capture->get(cv::CAP_PROP_FPS) * 1000));
    }
    else
    {
        return false;
    }

    m_frame_rate = m_video_capture->get(cv::CAP_PROP_FPS);
    m_frame_rate_round = static_cast<int>(0.5 + m_frame_rate);

    m_video_width = static_cast<int>(m_video_capture->get(cv::CAP_PROP_FRAME_WIDTH));
    m_video_height = static_cast<int>(m_video_capture->get(cv::CAP_PROP_FRAME_HEIGHT));

    qDebug() << "frame rate: " << m_frame_rate
             << "; m_frames_count: " << m_frames_count
             << "; m_video_length" << m_video_lenght;

    m_video_capture->grab(); // чтобы сразу при открытии окна показать начальный кадр
    if (m_video_capture->retrieve(m_current_frame_mat))
    {
        //create_image();
        cv::cvtColor(m_current_frame_mat, m_current_frame_mat, cv::COLOR_BGR2RGB);
        m_qt_image = QImage(static_cast<const unsigned char*>(m_current_frame_mat.data),
                            m_current_frame_mat.cols, m_current_frame_mat.rows, QImage::Format_RGB888);

//        if (m_frame_rate_round >= 60)
//        {
//            QStringList file_name_splited = file_name.split("/");
//            QString name = "tmp_30fps_" + file_name_splited.back();
//            file_name_splited.pop_back();
//            file_name_splited.push_back(name);
//            QString file_name_30_fps = file_name_splited.join("/");
//            m_video_capture_30_fps = new cv::VideoCapture(file_name_30_fps.toStdString());
//            if (m_video_capture_30_fps->isOpened())
//            {

//            }
//        }

        return true;
    }

    return false;

}

long OpenCV_Player::get_current_frame() const noexcept
{
    return m_current_frame_id;
}

double OpenCV_Player::get_frame_rate() const noexcept
{
    return m_frame_rate;
}

int OpenCV_Player::get_frame_rate_round() const noexcept
{
    return m_frame_rate_round;
}


int OpenCV_Player::get_video_width() const noexcept
{
    return m_video_width;
}

int OpenCV_Player::get_video_height() const noexcept
{
    return m_video_height;
}

long OpenCV_Player::get_video_frames_count() const noexcept
{
    return m_frames_count;
}

int OpenCV_Player::goto_frame_id(long id_frame) noexcept
{
    if (m_current_frame_id == id_frame)
    {
        return 0;
    }

    int64_t diff = static_cast<int64_t>(id_frame) - m_current_frame_id;

    if (diff > 0 && diff <= 40) // если нужно недалеко вперед, то лучше считать кадр за кадром, а не через m_video_capture->set(cv::CAP_PROP_POS_FRAMES, ...)
    {
        diff = std::min(diff, m_frames_count - m_current_frame_id - 1);

        long target_frame = m_current_frame_id + diff;

        while (m_current_frame_id < target_frame)
        {
            ++m_current_frame_id;
            if (!m_video_capture->grab())
            {
                break;
            }
        }
    }
    else
    {
        if (id_frame <= 0)
        {
            id_frame = 1;
        }
        m_current_frame_id = id_frame;
        m_video_capture->set(cv::CAP_PROP_POS_FRAMES, m_current_frame_id);
    }

    if (m_video_capture->retrieve(m_current_frame_mat))
    {
        //create_image();
        cv::cvtColor(m_current_frame_mat, m_current_frame_mat, cv::COLOR_BGR2RGB);

        //cv::imshow("!", m_current_frame_mat);

        return 1;
    }

    return -1;
}

void OpenCV_Player::create_image() noexcept
{
    cv::cvtColor(m_current_frame_mat, m_current_frame_mat, cv::COLOR_BGR2RGB);
    m_qt_image = QImage(static_cast<const unsigned char*>(m_current_frame_mat.data),
                        m_current_frame_mat.cols, m_current_frame_mat.rows, QImage::Format_RGB888);

}

void OpenCV_Player::release()
{
    if (m_video_capture != nullptr)
    {
        m_video_capture->release();
        delete m_video_capture;
        m_video_capture = nullptr;
        qDebug() << "delete opencv controller";
    }
}

OpenCV_Player::~OpenCV_Player()
{
    release();
}
