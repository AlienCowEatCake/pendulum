/* 
   Copyright (C) 2011-2016,
        Andrei V. Kurochkin     <kurochkin.andrei.v@yandex.ru>
        Mikhail E. Aleksandrov  <alexandroff.m@gmail.com>
        Peter S. Zhigalov       <peter.zhigalov@gmail.com>

   This file is part of the `PhysicalLabCore' library.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#if !defined (PHYSICALLABCORE_SWRASTMATRIX_H_INCLUDED)
#define PHYSICALLABCORE_SWRASTMATRIX_H_INCLUDED

#include "SWRastVector.h"

namespace SWRastPrivate {

/// @brief Класс матрицы
template<Memsize DR, Memsize DC, typename T> class Matrix
{
public:
    Vector<DC, T> & operator [] (Memsize i)
    {
        SWRAST_RANGE_ASSERT(i < DR);
        return m_values[i];
    }
    const Vector<DC, T> & operator [] (Memsize i) const
    {
        SWRAST_RANGE_ASSERT(i < DR);
        return m_values[i];
    }
    Vector<DR, T> col(Memsize i) const
    {
        SWRAST_RANGE_ASSERT(i < DC);
        Vector<DR, T> result;
        for(Memsize j = 0; j < DR; j++)
            result[j] = m_values[j][i];
        return result;
    }
    void set_col(Memsize i, const Vector<DR, T> & v)
    {
        SWRAST_RANGE_ASSERT(i < DC);
        for(Memsize j = 0; j < DR; j++)
            m_values[j][i] = v[j];
    }
    static Matrix<DR, DC, T> identity()
    {
        Matrix<DR, DC, T> result;
        for(Memsize i = 0; i < DR; i++)
            for(Memsize j = 0; j < DC; j++)
                result[i][j] = static_cast<T>(i == j);
        return result;
    }
    Matrix<DC, DR, T> transpose() const
    {
        Matrix<DC, DR, T> result;
        for(Memsize i = 0; i < DR; i++)
            for(Memsize j = 0; j < DC; j++)
                result[j][i] = m_values[i][j];
        return result;
    }
    T determenant() const;
    Matrix<DR, DC, T> inverse() const;
    Vector<DR, T> operator * (const Vector<DC, T> & other) const
    {
        Vector<DR, T> result;
        for(Memsize i = 0; i < DR; i++)
            for(Memsize j = 0; j < DC; j++)
                result[i] += m_values[i][j] * other[j];
        return result;
    }
    template<Memsize U>
    Matrix<DR, U, T> operator * (const Matrix<DC, U, T> & other) const
    {
        Matrix<DR, U, T> result;
        for(Memsize i = 0; i < DR; i++)
            for(Memsize j = 0; j < DC; j++)
                for(Memsize k = 0; k < U; k++)
                    result[i][k] += m_values[i][j] * other[j][k];
        return result;
    }
    Matrix<DR, DC, T> operator * (const T other) const
    {
        Matrix<DR, DC, T> result;
        for(Memsize i = 0; i < DR; i++)
            for(Memsize j = 0; j < DC; j++)
                result[i][j] *= other;
        return result;
    }
    Matrix<DR, DC, T> operator / (const T other) const
    {
        Matrix<DR, DC, T> result;
        for(Memsize i = 0; i < DR; i++)
            for(Memsize j = 0; j < DC; j++)
                result[i][j] /= other;
        return result;
    }

protected:
    Vector<DC, T> m_values[DR];
};

/// @brief Определитель матрицы 3x3
template<typename T>
T determenant(const Matrix<3, 3, T> & A)
{
    return A[0][0] * A[1][1] * A[2][2] +
           A[1][0] * A[2][1] * A[0][2] +
           A[0][1] * A[1][2] * A[2][0] -
           A[2][0] * A[1][1] * A[0][2] -
           A[2][1] * A[1][2] * A[0][0] -
           A[1][0] * A[0][1] * A[2][2];
}

/// @brief Определитель матрицы 4x4
template<typename T>
T determenant(const Matrix<4, 4, T> & A)
{
    return A[2][3] * (A[3][1] * (A[0][0] * A[1][2] - A[0][2] * A[1][0]) +
                      A[0][1] * (A[1][0] * A[3][2] - A[1][2] * A[3][0])) +
           A[2][1] * (A[3][2] * (A[0][0] * A[1][3] - A[0][3] * A[1][0]) +
                      A[3][0] * (A[0][3] * A[1][2] - A[0][2] * A[1][3]) +
                      A[3][3] * (A[0][2] * A[1][0] - A[0][0] * A[1][2])) +
           A[1][1] * (A[2][3] * (A[0][2] * A[3][0] - A[0][0] * A[3][2]) +
                      A[2][2] * (A[0][0] * A[3][3] - A[0][3] * A[3][0])) +
           A[2][2] * A[3][1] * (A[0][3] * A[1][0] - A[0][0] * A[1][3]) +
           A[2][0] * (A[0][1] * (A[1][2] * A[3][3] - A[1][3] * A[3][2]) +
                      A[1][1] * (A[0][3] * A[3][2] - A[0][2] * A[3][3]) +
                      A[3][1] * (A[0][2] * A[1][3] - A[0][3] * A[1][2])) +
           A[0][1] * A[2][2] * (A[1][3] * A[3][0] - A[1][0] * A[3][3]);
}

/// @brief Обращение матрицы 3x3
template<typename T>
Matrix<3, 3, T> inverse(const Matrix<3, 3, T> & A, T & detA)
{
    Matrix<3, 3, T> Inv_A;
    detA = determenant(A);
    T inv_detA = static_cast<T>(1) / detA;

    Inv_A[0][0] = (A[1][1] * A[2][2] - A[2][1] * A[1][2]) * inv_detA;
    Inv_A[0][1] = - (A[0][1] * A[2][2] - A[2][1] * A[0][2]) * inv_detA;
    Inv_A[0][2] = (A[0][1] * A[1][2] - A[1][1] * A[0][2]) * inv_detA;

    Inv_A[1][0] = - (A[1][0] * A[2][2] - A[2][0] * A[1][2]) * inv_detA;
    Inv_A[1][1] = (A[0][0] * A[2][2] - A[2][0] * A[0][2]) * inv_detA;
    Inv_A[1][2] = - (A[0][0] * A[1][2] - A[1][0] * A[0][2]) * inv_detA;

    Inv_A[2][0] = (A[1][0] * A[2][1] - A[2][0] * A[1][1]) * inv_detA;
    Inv_A[2][1] = - (A[0][0] * A[2][1] - A[2][0] * A[0][1]) * inv_detA;
    Inv_A[2][2] = (A[0][0] * A[1][1] - A[1][0] * A[0][1]) * inv_detA;

    return Inv_A;
}

/// @brief Обращение матрицы 4x4
template<typename T>
Matrix<4, 4, T> inverse(const Matrix<4, 4, T> & A, T & detA)
{
    Matrix<4, 4, T> Inv_A;
    detA = determenant(A);
    T inv_detA = static_cast<T>(1) / detA;

    Inv_A[0][0] = (A[3][1] * (A[1][2] * A[2][3] - A[1][3] * A[2][2]) +
                   A[3][2] * (A[1][3] * A[2][1] - A[1][1] * A[2][3]) +
                   A[3][3] * (A[1][1] * A[2][2] - A[1][2] * A[2][1])) * inv_detA;
    Inv_A[0][1] = (A[3][1] * (A[0][3] * A[2][2] - A[0][2] * A[2][3]) +
                   A[3][2] * (A[0][1] * A[2][3] - A[0][3] * A[2][1]) +
                   A[3][3] * (A[0][2] * A[2][1] - A[0][1] * A[2][2])) * inv_detA;
    Inv_A[0][2] = (A[3][1] * (A[0][2] * A[1][3] - A[0][3] * A[1][2]) +
                   A[3][2] * (A[0][3] * A[1][1] - A[0][1] * A[1][3]) +
                   A[3][3] * (A[0][1] * A[1][2] - A[0][2] * A[1][1])) * inv_detA;
    Inv_A[0][3] = (A[2][1] * (A[0][3] * A[1][2] - A[0][2] * A[1][3]) +
                   A[2][2] * (A[0][1] * A[1][3] - A[0][3] * A[1][1]) +
                   A[2][3] * (A[0][2] * A[1][1] - A[0][1] * A[1][2])) * inv_detA;

    Inv_A[1][0] = (A[3][0] * (A[1][3] * A[2][2] - A[1][2] * A[2][3]) +
                   A[3][2] * (A[1][0] * A[2][3] - A[1][3] * A[2][0]) +
                   A[3][3] * (A[1][2] * A[2][0] - A[1][0] * A[2][2])) * inv_detA;
    Inv_A[1][1] = (A[3][0] * (A[0][2] * A[2][3] - A[0][3] * A[2][2]) +
                   A[3][2] * (A[0][3] * A[2][0] - A[0][0] * A[2][3]) +
                   A[3][3] * (A[0][0] * A[2][2] - A[0][2] * A[2][0])) * inv_detA;
    Inv_A[1][2] = (A[3][0] * (A[0][3] * A[1][2] - A[0][2] * A[1][3]) +
                   A[3][2] * (A[0][0] * A[1][3] - A[0][3] * A[1][0]) +
                   A[3][3] * (A[0][2] * A[1][0] - A[0][0] * A[1][2])) * inv_detA;
    Inv_A[1][3] = (A[2][0] * (A[0][2] * A[1][3] - A[0][3] * A[1][2]) +
                   A[2][2] * (A[0][3] * A[1][0] - A[0][0] * A[1][3]) +
                   A[2][3] * (A[0][0] * A[1][2] - A[0][2] * A[1][0])) * inv_detA;

    Inv_A[2][0] = (A[3][0] * (A[1][1] * A[2][3] - A[1][3] * A[2][1]) +
                   A[3][1] * (A[1][3] * A[2][0] - A[1][0] * A[2][3]) +
                   A[3][3] * (A[1][0] * A[2][1] - A[1][1] * A[2][0])) * inv_detA;
    Inv_A[2][1] = (A[3][0] * (A[0][3] * A[2][1] - A[0][1] * A[2][3]) +
                   A[3][1] * (A[0][0] * A[2][3] - A[0][3] * A[2][0]) +
                   A[3][3] * (A[0][1] * A[2][0] - A[0][0] * A[2][1])) * inv_detA;
    Inv_A[2][2] = (A[3][0] * (A[0][1] * A[1][3] - A[0][3] * A[1][1]) +
                   A[3][1] * (A[0][3] * A[1][0] - A[0][0] * A[1][3]) +
                   A[3][3] * (A[0][0] * A[1][1] - A[0][1] * A[1][0])) * inv_detA;
    Inv_A[2][3] = (A[2][0] * (A[0][3] * A[1][1] - A[0][1] * A[1][3]) +
                   A[2][1] * (A[0][0] * A[1][3] - A[0][3] * A[1][0]) +
                   A[2][3] * (A[0][1] * A[1][0] - A[0][0] * A[1][1])) * inv_detA;

    Inv_A[3][0] = (A[3][0] * (A[1][2] * A[2][1] - A[1][1] * A[2][2]) +
                   A[3][1] * (A[1][0] * A[2][2] - A[1][2] * A[2][0]) +
                   A[3][2] * (A[1][1] * A[2][0] - A[1][0] * A[2][1])) * inv_detA;
    Inv_A[3][1] = (A[3][0] * (A[0][1] * A[2][2] - A[0][2] * A[2][1]) +
                   A[3][1] * (A[0][2] * A[2][0] - A[0][0] * A[2][2]) +
                   A[3][2] * (A[0][0] * A[2][1] - A[0][1] * A[2][0])) * inv_detA;
    Inv_A[3][2] = (A[3][0] * (A[0][2] * A[1][1] - A[0][1] * A[1][2]) +
                   A[3][1] * (A[0][0] * A[1][2] - A[0][2] * A[1][0]) +
                   A[3][2] * (A[0][1] * A[1][0] - A[0][0] * A[1][1])) * inv_detA;
    Inv_A[3][3] = (A[2][0] * (A[0][1] * A[1][2] - A[0][2] * A[1][1]) +
                   A[2][1] * (A[0][2] * A[1][0] - A[0][0] * A[1][2]) +
                   A[2][2] * (A[0][0] * A[1][1] - A[0][1] * A[1][0])) * inv_detA;

    return Inv_A;
}

/// @brief Вычисление определителя как метод матрицы
template<Memsize DR, Memsize DC, typename T>
T Matrix<DR, DC, T>::determenant() const
{
    return SWRastPrivate::determenant(* this);
}

/// @brief Обращение матрицы как метод матрицы
template<Memsize DR, Memsize DC, typename T>
Matrix<DR, DC, T> Matrix<DR, DC, T>::inverse() const
{
    T d;
    return SWRastPrivate::inverse(* this, d);
}

typedef Matrix<4, 4, Real> matrix4f;

} // SWRastPrivate

#endif // PHYSICALLABCORE_SWRASTMATRIX_H_INCLUDED

