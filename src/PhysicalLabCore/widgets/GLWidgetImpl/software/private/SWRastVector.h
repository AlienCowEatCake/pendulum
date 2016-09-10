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

#if !defined (PHYSICALLABCORE_SWRASTVECTOR_H_INCLUDED)
#define PHYSICALLABCORE_SWRASTVECTOR_H_INCLUDED

#include <cmath>
#include <cstring>
#include "SWRastConfig.h"

namespace SWRastPrivate {

/// @brief Базовый класс вектора, от него потом отнаследуемся
/// (это поможет обойти некоторые ограничения шаблонов)
template <Memsize D, typename T> class VectorBase
{
public:
    VectorBase()
    {
//        for(Memsize i = 0; i < D; i++)
//            m_values[i] = T();
        memset(m_values, 0, sizeof(T) * D);
    }
    T & operator [] (const Memsize i)
    {
        SWRAST_RANGE_ASSERT(i < D);
        return m_values[i];
    }
    const T & operator [] (const Memsize i) const
    {
        SWRAST_RANGE_ASSERT(i < D);
        return m_values[i];
    }
    Real norm() const
    {
        Real result = static_cast<Real>(0);
        for(Memsize i = 0; i < D; i++)
            result += m_values[i] * m_values[i];
        return static_cast<Real>(std::sqrt(result));
    }
    void normalize()
    {
        Real nr = static_cast<Real>(1) / norm();
        for(Memsize i = 0; i < D; i++)
            m_values[i] = static_cast<T>(m_values[i] * nr);
    }
    const T * data() const
    {
        return m_values;
    }
    T * data()
    {
        return m_values;
    }
protected:
    T m_values[D];
};

/// @brief Фиктивный класс-наследник, в частично специализированных
/// классах тоже, тогда будет доступно все то, что и в базовом классе
template <Memsize D, typename T> class Vector : public VectorBase<D, T>
{
public:
};

/// @brief Скалярное произведение векторов
template<Memsize D, typename T>
T operator * (const Vector<D, T> & left, const Vector<D, T> & right)
{
    T result = static_cast<T>(0);
    for(Memsize i = 0; i < D; i++)
        result += left[i] * right[i];
    return result;
}

/// @brief Сложение векторов
template<Memsize D, typename T>
Vector<D, T> operator + (const Vector<D, T> & left, const Vector<D, T> & right)
{
    Vector<D, T> result;
    for(Memsize i = 0; i < D; i++)
        result[i] = left[i] + right[i];
    return result;
}

/// @brief Вычитание векторов
template<Memsize D, typename T>
Vector<D, T> operator - (const Vector<D, T> & left, const Vector<D, T> & right)
{
    Vector<D, T> result;
    for(Memsize i = 0; i < D; i++)
        result[i] = left[i] - right[i];
    return result;
}

/// @brief Умножение вектора на число
template<Memsize D, typename T, typename U>
Vector<D, T> operator * (const Vector<D, T> & left, const U & right)
{
    Vector<D, T> result;
    for(Memsize i = 0; i < D; i++)
        result[i] += static_cast<T>(left[i] * right);
    return result;
}

/// @brief Деление вектора на число
template<Memsize D, typename T, typename U>
Vector<D, T> operator / (const Vector<D, T> & left, const U & right)
{
    Vector<D, T> result;
    for(Memsize i = 0; i < D; i++)
        result[i] += static_cast<T>(left[i] / right);
    return result;
}

/// @brief Векторное произведение векторов
template <typename T>
Vector<3, T> cross(const Vector<3, T> & v1, const Vector<3, T> & v2)
{
    return Vector<3, T>(v1[1] * v2[2] - v1[2] * v2[1], v1[2] * v2[0] - v1[0] * v2[2], v1[0] * v2[1] - v1[1] * v2[0]);
}

/// @brief Копирование векторов различной длины с заполнением оставшихся компонент числом fill
template<Memsize U, Memsize D, typename T>
Vector<U, T> embed(const Vector<D, T> & v, T fill = static_cast<T>(1))
{
    Vector<U, T> result;
//    for(Memsize i = 0; i < D; i++)
//        result[i] = v[i];
    memcpy(result.data(), v.data(), sizeof(T) * D);
    for(Memsize i = D; i < U; i++)
        result[i] = fill;
    return result;
}

/// @brief Копирование векторов разной длины с отбрасыванием оставшихся компонент
template<Memsize U, Memsize D, typename T>
Vector<U, T> proj(const Vector<D, T> & v)
{
    Vector<U, T> result;
//    for(Memsize i = 0; i < U; i++)
//        result[i] = v[i];
    memcpy(result.data(), v.data(), sizeof(T) * U);
    return result;
}

/// @brief Двумерный вектор (вещественный)
template<> class Vector<2, Real> : public VectorBase<2, Real>
{
public:
    Vector(Real x = static_cast<Real>(0), Real y = static_cast<Real>(0))
    {
        m_values[0] = x;
        m_values[1] = y;
    }
    Vector(const Vector<2, Real> & other) : VectorBase<2, Real>(other)
    {
//        for(Memsize i = 0; i < 2; i++)
//            m_values[i] = other.m_values[i];
        memcpy(m_values, other.m_values, sizeof(Real) * 2);
    }
    template<typename U>
    Vector(const Vector<2, U> & other)
    {
        for(Memsize i = 0; i < 2; i++)
            m_values[i] = static_cast<Real>(other[i]);
    }
};

/// @brief Двумерный вектор (целочисленный)
template<> class Vector<2, Integer> : public VectorBase<2, Integer>
{
public:
    Vector(Integer x = static_cast<Integer>(0), Integer y = static_cast<Integer>(0))
    {
        m_values[0] = x;
        m_values[1] = y;
    }
    Vector(const Vector<2, Integer> & other) : VectorBase<2, Integer>(other)
    {
//        for(Memsize i = 0; i < 2; i++)
//            m_values[i] = other.m_values[i];
        memcpy(m_values, other.m_values, sizeof(Integer) * 2);
    }
    Vector(const Vector<2, Real> & other)
    {
        for(Memsize i = 0; i < 2; i++)
            m_values[i] = static_cast<Integer>(other[i] + static_cast<Real>(0.5f));
    }
};

/// @brief Трехмерный вектор (вещественный)
template<> class Vector<3, Real> : public VectorBase<3, Real>
{
public:
    Vector(Real x = static_cast<Real>(0), Real y = static_cast<Real>(0), Real z = static_cast<Real>(0))
    {
        m_values[0] = x;
        m_values[1] = y;
        m_values[2] = z;
    }
    Vector(const Vector<3, Real> & other) : VectorBase<3, Real>(other)
    {
//        for(Memsize i = 0; i < 3; i++)
//            m_values[i] = other.m_values[i];
        memcpy(m_values, other.m_values, sizeof(Real) * 3);
    }
    template<typename U>
    Vector(const Vector<3, U> & other)
    {
        for(Memsize i = 0; i < 3; i++)
            m_values[i] = static_cast<Real>(other[i]);
    }
    Vector<3, Real> & normalize()
    {
        VectorBase<3, Real>::normalize();
        return * this;
    }
};

/// @brief Трехмерный вектор (целочисленный)
template<> class Vector<3, Integer> : public VectorBase<3, Integer>
{
public:
    Vector(Integer x = static_cast<Integer>(0), Integer y = static_cast<Integer>(0), Integer z = static_cast<Integer>(0))
    {
        m_values[0] = x;
        m_values[1] = y;
        m_values[2] = z;
    }
    Vector(const Vector<3, Integer> & other) : VectorBase<3, Integer>(other)
    {
//        for(Memsize i = 0; i < 3; i++)
//            m_values[i] = other.m_values[i];
        memcpy(m_values, other.m_values, sizeof(Integer) * 3);
    }
    Vector(const Vector<3, Real> & other)
    {
        for(Memsize i = 0; i < 3; i++)
            m_values[i] = static_cast<Integer>(other[i] + static_cast<Real>(0.5f));
    }
    Vector<3, Integer> & normalize()
    {
        VectorBase<3, Integer>::normalize();
        return * this;
    }
};

typedef Vector<2, Real>     vec2f;
typedef Vector<2, Integer>  vec2i;
typedef Vector<3, Real>     vec3f;
typedef Vector<3, Integer>  vec3i;
typedef Vector<4, Real>     vec4f;

} // SWRastPrivate

#endif // PHYSICALLABCORE_SWRASTVECTOR_H_INCLUDED

