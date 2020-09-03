/***************************************************************************

             WINKAM TM strictly confidential 2019

 ***************************************************************************/
#include "affine_transformation.h"
#include <cmath>

namespace WKTR // WINKAM library. Digital Signal Processing
{

template <class T>
Rotation<T>::Rotation(T x1, T y1, T x2, T y2)
{
    set_basis(x1, y1, x2, y2);
}

template <class T>
void Rotation<T>::set_basis(T x1, T y1, T x2, T y2) noexcept
{
    double ox = x2 - x1;
    double oy = y2 - y1;
    double hypo = std::sqrt(ox * ox + oy * oy) + 1.E-6;
    m_sin_a = ox / hypo;

    m_cos_a = oy / hypo;
}

template <class T>
std::pair<T, T> Rotation<T>::rotate(T x, T y) const noexcept
{
    return {x * m_cos_a - y * m_sin_a, x * m_sin_a + y * m_cos_a};
}

template <class T>
std::pair<T, T> Rotation<T>::rotate(std::pair<T, T> xy) const noexcept
{
    return {xy.first * m_cos_a - xy.second * m_sin_a, xy.first * m_sin_a + xy.second * m_cos_a};
}

template <class T>
Rotation<T>::~Rotation()
{

}

template <class T>
Transmition<T>::Transmition(T x, T y)
{
    set_basis(x, y);
}

template <class T>
void Transmition<T>::set_basis(T x, T y) noexcept
{
    m_shift_x = -x;
    m_shift_y = -y;
}

template <class T>
void Transmition<T>::set_basis(std::pair<T, T> xy) noexcept
{
    set_basis(xy.first, xy.second);
}

template <class T>
std::pair<T, T> Transmition<T>::transmite(double x, double y) const noexcept
{
    return {x + m_shift_x, y + m_shift_y};
}

template <class T>
std::pair<T, T> Transmition<T>::transmite(std::pair<T, T> xy) const noexcept
{
    return {xy.first + m_shift_x, xy.second + m_shift_y};
}


template <class T>
Transmition<T>::~Transmition()
{

}

}

template class WKTR::Rotation<double>;
template class WKTR::Rotation<int>;
template class WKTR::Rotation<long>;

template class WKTR::Transmition<double>;
template class WKTR::Transmition<int>;
template class WKTR::Transmition<long>;

