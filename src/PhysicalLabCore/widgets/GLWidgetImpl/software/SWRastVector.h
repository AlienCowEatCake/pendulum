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
#include <cassert>

namespace SWRastInternal {

/// @brief Тип числа с плавающей точкой
typedef float       real_type;
/// @brief Тип числа целого со знаком
typedef int         int_type;
/// @brief Тип индексов (memsize-тип)
typedef std::size_t size_type;


/// @brief Базовый класс вектора, от него потом отнаследуемся
/// (это поможет обойти некоторые ограничения шаблонов)
template <size_type D, typename T> class VectorBase
{
public:
    VectorBase()
    {
        for(size_type i = 0; i < D; i++)
            m_values[i] = T();
    }
    T & operator [] (const size_type i)
    {
        assert(i < D);
        return m_values[i];
    }
    const T & operator [] (const size_type i) const
    {
        assert(i < D);
        return m_values[i];
    }
    real_type norm() const
    {
        real_type result = static_cast<real_type>(0);
        for(size_type i = 0; i < D; i++)
            result += m_values[i] * m_values[i];
        return static_cast<real_type>(std::sqrt(result));
    }
    void normalize()
    {
        real_type nr = static_cast<real_type>(1) / norm();
        for(size_type i = 0; i < D; i++)
            m_values[i] = static_cast<T>(m_values[i] * nr);
    }
protected:
    T m_values[D];
};

/// @brief Фиктивный класс-наследник, в частично специализированных
/// классах тоже, тогда будет доступно все то, что и в базовом классе
template <size_type D, typename T> class Vector : public VectorBase<D, T>
{
public:
};

/// @brief Скалярное произведение векторов
template<size_type D, typename T>
T operator * (const Vector<D, T> & left, const Vector<D, T> & right)
{
    T result = static_cast<T>(0);
    for(size_type i = 0; i < D; i++)
        result += left[i] * right[i];
    return result;
}

/// @brief Сложение векторов
template<size_type D, typename T>
Vector<D, T> operator + (const Vector<D, T> & left, const Vector<D, T> & right)
{
    Vector<D, T> result;
    for(size_type i = 0; i < D; i++)
        result[i] = left[i] + right[i];
    return result;
}

/// @brief Вычитание векторов
template<size_type D, typename T>
Vector<D, T> operator - (const Vector<D, T> & left, const Vector<D, T> & right)
{
    Vector<D, T> result;
    for(size_type i = 0; i < D; i++)
        result[i] = left[i] - right[i];
    return result;
}

/// @brief Умножение вектора на число
template<size_type D, typename T, typename U>
Vector<D, T> operator * (const Vector<D, T> & left, const U & right)
{
    Vector<D, T> result;
    for(size_type i = 0; i < D; i++)
        result[i] += static_cast<T>(left[i] * right);
    return result;
}

/// @brief Деление вектора на число
template<size_type D, typename T, typename U>
Vector<D, T> operator / (const Vector<D, T> & left, const U & right)
{
    Vector<D, T> result;
    for(size_type i = 0; i < D; i++)
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
template<size_type U, size_type D, typename T>
Vector<U, T> embed(const Vector<D, T> & v, T fill = static_cast<T>(1))
{
    Vector<U, T> result;
    for(size_type i = 0; i < D; i++)
        result[i] = v[i];
    for(size_type i = D; i < U; i++)
        result[i] = fill;
    return result;
}

/// @brief Копирование векторов разной длины с отбрасыванием оставшихся компонент
template<size_type U, size_type D, typename T>
Vector<U, T> proj(const Vector<D, T> & v)
{
    Vector<U, T> result;
    for(size_type i = 0; i < U; i++)
        result[i] = v[i];
    return result;
}

/// @brief Двумерный вектор (вещественный)
template<> class Vector<2, real_type> : public VectorBase<2, real_type>
{
public:
    Vector(real_type x = 0.0f, real_type y = 0.0f)
    {
        m_values[0] = x;
        m_values[1] = y;
    }
    Vector(const Vector<2, real_type> & other) : VectorBase<2, real_type>(other)
    {
        for(size_type i = 0; i < 2; i++)
            m_values[i] = other.m_values[i];
    }
    template<typename U>
    Vector(const Vector<2, U> & other)
    {
        for(size_type i = 0; i < 2; i++)
            m_values[i] = static_cast<real_type>(other[i]);
    }
};

/// @brief Двумерный вектор (целочисленный)
template<> class Vector<2, int_type> : public VectorBase<2, int_type>
{
public:
    Vector(int_type x = 0, int_type y = 0)
    {
        m_values[0] = x;
        m_values[1] = y;
    }
    Vector(const Vector<2, int_type> & other) : VectorBase<2, int_type>(other)
    {
        for(size_type i = 0; i < 2; i++)
            m_values[i] = other.m_values[i];
    }
    Vector(const Vector<2, real_type> & other)
    {
        for(size_type i = 0; i < 2; i++)
            m_values[i] = static_cast<int_type>(other[i] + 0.5f);
    }
};

/// @brief Трехмерный вектор (вещественный)
template<> class Vector<3, real_type> : public VectorBase<3, real_type>
{
public:
    Vector(real_type x = 0.0f, real_type y = 0.0f, real_type z = 0.0f)
    {
        m_values[0] = x;
        m_values[1] = y;
        m_values[2] = z;
    }
    Vector(const Vector<3, real_type> & other) : VectorBase<3, real_type>(other)
    {
        for(size_type i = 0; i < 3; i++)
            m_values[i] = other.m_values[i];
    }
    template<typename U>
    Vector(const Vector<3, U> & other)
    {
        for(size_type i = 0; i < 3; i++)
            m_values[i] = static_cast<real_type>(other[i]);
    }
    Vector<3, real_type> & normalize()
    {
        real_type nr = static_cast<real_type>(1) / norm();
        for(size_type i = 0; i < 3; i++)
            m_values[i] *= static_cast<real_type>(nr);
        return * this;
    }
};

/// @brief Трехмерный вектор (целочисленный)
template<> class Vector<3, int_type> : public VectorBase<3, int_type>
{
public:
    Vector(int_type x = 0, int_type y = 0, int_type z = 0)
    {
        m_values[0] = x;
        m_values[1] = y;
        m_values[2] = z;
    }
    Vector(const Vector<3, int_type> & other) : VectorBase<3, int_type>(other)
    {
        for(size_type i = 0; i < 3; i++)
            m_values[i] = other.m_values[i];
    }
    Vector(const Vector<3, real_type> & other)
    {
        for(size_type i = 0; i < 3; i++)
            m_values[i] = static_cast<int_type>(other[i] + 0.5f);
    }
    Vector<3, int_type> & normalize()
    {
        real_type nr = static_cast<real_type>(1) / norm();
        for(size_type i = 0; i < 3; i++)
            m_values[i] = static_cast<int_type>(m_values[i] * nr);
        return * this;
    }
};

typedef Vector<2, real_type> vec2f;
typedef Vector<2, int_type>  vec2i;
typedef Vector<3, real_type> vec3f;
typedef Vector<3, int_type>  vec3i;
typedef Vector<4, real_type> vec4f;

} // SWRastInternal

#endif // PHYSICALLABCORE_SWRASTVECTOR_H_INCLUDED

