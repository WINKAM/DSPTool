/***************************************************************************

             WINKAM TM strictly confidential 2019

 ***************************************************************************/
//v.1.0.0
#ifndef AFFINE_TRANSFORMATION_H
#define AFFINE_TRANSFORMATION_H

#include <utility>

namespace WKTR // WINKAM library. Digital Signal Processing
{

template<class T>
class Rotation
{
private:
    double m_sin_a;
    double m_cos_a;

public:
    Rotation(T x1 = 0, T y1 = 0, T x2 = 0, T y2 = 1);
    void set_basis(T x1, T y1, T x2, T y2) noexcept;
    std::pair<T, T> rotate(T x, T y) const noexcept;
    std::pair<T, T> rotate(std::pair<T, T> xy) const noexcept;

    ~Rotation();    
};

template<class T>
class Transmition
{
private:
    T m_shift_x;
    T m_shift_y;

public:
    Transmition(T x = 0, T y = 0);
    void set_basis(T x, T y) noexcept;
    void set_basis(std::pair<T, T> xy) noexcept;

    std::pair<T, T> transmite(double x, double y) const noexcept;
    std::pair<T, T> transmite(std::pair<T, T> xy) const noexcept;

    ~Transmition();
};

}

#endif // AFFINE_TRANSFORMATION_H
