#ifndef SWRAST_GEOMETRY_H
#define SWRAST_GEOMETRY_H

#include <cmath>
#include <cstring>
#include <cassert>

// =================================================================================================

// Тип числа с плавающей точкой
typedef float  fp_type;
// Тип индексов (memsize-тип)
typedef size_t size_type;

// =================================================================================================

// Базовый класс вектора, от него потом отнаследуемся
// (это поможет обойти некоторые ограничения шаблонов)
template <size_type D, typename T> class vec_base
{
public:
    vec_base()
    {
        for(size_type i = 0; i < D; i++)
            values[i] = T();
    }
    T & operator [] (const size_type i)
    {
        assert(i < D);
        return values[i];
    }
    const T & operator [] (const size_type i) const
    {
        assert(i < D);
        return values[i];
    }
    inline T & x()
    {
        assert(D > 0);
        return values[0];
    }
    inline const T & x() const
    {
        assert(D > 0);
        return values[0];
    }
    inline const T & x(T val) const
    {
        assert(D > 0);
        values[0] = val;
        return values[0];
    }
    inline T & y()
    {
        assert(D > 1);
        return values[1];
    }
    inline const T & y() const
    {
        assert(D > 1);
        return values[1];
    }
    inline const T & y(T val) const
    {
        assert(D > 1);
        values[1] = val;
        return values[1];
    }
    inline T & z()
    {
        assert(D > 2);
        return values[2];
    }
    inline const T & z() const
    {
        assert(D > 2);
        return values[2];
    }
    inline const T & z(T val) const
    {
        assert(D > 2);
        values[2] = val;
        return values[2];
    }
    fp_type norm() const
    {
        fp_type result = static_cast<fp_type>(0);
        for(size_type i = 0; i < D; i++)
            result += values[i] * values[i];
        return static_cast<fp_type>(sqrt(result));
    }
    void normalize()
    {
        fp_type nr = static_cast<fp_type>(1) / norm();
        for(size_type i = 0; i < D; i++)
            values[i] = static_cast<T>(values[i] * nr);
    }
protected:
    T values[D];
};

// Вот тут мы и отнаследуемся, причем в частично специализированных
// классах тоже, тогда будет доступно все то, что и в базовом классе
template <size_type D, typename T> class vec_t : public vec_base<D, T>
{
public:
};

// Определим обобщенные операторы
template<size_type D, typename T>
T operator * (const vec_t<D, T> & left, const vec_t<D, T> & right)
{
    T result = static_cast<T>(0);
    for(size_type i = 0; i < D; i++)
        result += left[i] * right[i];
    return result;
}

template<size_type D, typename T>
vec_t<D, T> operator + (const vec_t<D, T> & left, const vec_t<D, T> & right)
{
    vec_t<D, T> result;
    for(size_type i = 0; i < D; i++)
        result[i] = left[i] + right[i];
    return result;
}

template<size_type D, typename T>
vec_t<D, T> operator - (const vec_t<D, T> & left, const vec_t<D, T> & right)
{
    vec_t<D, T> result;
    for(size_type i = 0; i < D; i++)
        result[i] = left[i] - right[i];
    return result;
}

template<size_type D, typename T, typename U>
vec_t<D, T> operator * (const vec_t<D, T> & left, const U & right)
{
    vec_t<D, T> result;
    for(size_type i = 0; i < D; i++)
        result[i] += static_cast<T>(left[i] * right);
    return result;
}

template<size_type D, typename T, typename U>
vec_t<D, T> operator / (const vec_t<D, T> & left, const U & right)
{
    vec_t<D, T> result;
    for(size_type i = 0; i < D; i++)
        result[i] += static_cast<T>(left[i] / right);
    return result;
}

template <typename T>
vec_t<3, T> cross(const vec_t<3, T> & v1, const vec_t<3, T> & v2)
{
    return vec_t<3, T>(v1[1] * v2[2] - v1[2] * v2[1], v1[2] * v2[0] - v1[0] * v2[2], v1[0] * v2[1] - v1[1] * v2[0]);
}

template<size_type U, size_type D, typename T>
vec_t<U, T> embed(const vec_t<D, T> & v, T fill = static_cast<T>(1))
{
    vec_t<U, T> result;
    for(size_type i = 0; i < D; i++)
        result[i] = v[i];
    for(size_type i = D; i < U; i++)
        result[i] = fill;
    return result;
}

template<size_type U, size_type D, typename T>
vec_t<U, T> proj(const vec_t<D, T> & v)
{
    vec_t<U, T> result;
    for(size_type i = 0; i < U; i++)
        result[i] = v[i];
    return result;
}

// Прототипы будущих векторов
template<> class vec_t<2, int>;
template<> class vec_t<2, float>;
template<> class vec_t<3, int>;
template<> class vec_t<3, float>;

// Двумерный вектор (вещественный)
template<> class vec_t<2, float> : public vec_base<2, float>
{
public:
    vec_t(float x = 0.0f, float y = 0.0f)
    {
        values[0] = x;
        values[1] = y;
    }
    vec_t(const vec_t<2, float> & other)
    {
        for(size_type i = 0; i < 2; i++)
            values[i] = other.values[i];
    }
    template<typename U>
    vec_t(const vec_t<2, U> & other)
    {
        for(size_type i = 0; i < 2; i++)
            values[i] = static_cast<float>(other[i]);
    }
};

// Двумерный вектор (целочисленный)
template<> class vec_t<2, int> : public vec_base<2, int>
{
public:
    vec_t(int x = 0, int y = 0)
    {
        values[0] = x;
        values[1] = y;
    }
    vec_t(const vec_t<2, int> & other)
    {
        for(size_type i = 0; i < 2; i++)
            values[i] = other.values[i];
    }
    vec_t(const vec_t<2, float> & other)
    {
        for(size_type i = 0; i < 2; i++)
            values[i] = static_cast<int>(other[i] + 0.5f);
    }
};

// Трехмерный вектор (вещественный)
template<> class vec_t<3, float> : public vec_base<3, float>
{
public:
    vec_t(float x = 0.0f, float y = 0.0f, float z = 0.0f)
    {
        values[0] = x;
        values[1] = y;
        values[2] = z;
    }
    vec_t(const vec_t<3, float> & other)
    {
        for(size_type i = 0; i < 3; i++)
            values[i] = other.values[i];
    }
    template<typename U>
    vec_t(const vec_t<3, U> & other)
    {
        for(size_type i = 0; i < 3; i++)
            values[i] = static_cast<float>(other[i]);
    }
    vec_t<3, float> & normalize()
    {
        fp_type nr = static_cast<fp_type>(1) / norm();
        for(size_type i = 0; i < 3; i++)
            values[i] *= static_cast<float>(nr);
        return * this;
    }
};

// Трехмерный вектор (целочисленный)
template<> class vec_t<3, int> : public vec_base<3, int>
{
public:
    vec_t(int x = 0, int y = 0, int z = 0)
    {
        values[0] = x;
        values[1] = y;
        values[2] = z;
    }
    vec_t(const vec_t<3, int> & other)
    {
        for(size_type i = 0; i < 3; i++)
            values[i] = other.values[i];
    }
    vec_t(const vec_t<3, float> & other)
    {
        for(size_type i = 0; i < 3; i++)
            values[i] = static_cast<int>(other[i] + 0.5f);
    }
    vec_t<3, int> & normalize()
    {
        fp_type nr = static_cast<fp_type>(1) / norm();
        for(size_type i = 0; i < 3; i++)
            values[i] = static_cast<int>(values[i] * nr);
        return * this;
    }
};

typedef vec_t<2, float> vec2f;
typedef vec_t<2, int>   vec2i;
typedef vec_t<3, float> vec3f;
typedef vec_t<3, int>   vec3i;
typedef vec_t<4, float> vec4f;

// =================================================================================================

// Класс матрицы
template<size_type DR, size_type DC, typename T> class mat_t
{
public:
    vec_t<DC, T> & operator [] (size_type i)
    {
        assert(i < DR);
        return values[i];
    }
    const vec_t<DC, T> & operator [] (size_type i) const
    {
        assert(i < DR);
        return values[i];
    }
    vec_t<DR, T> col(size_type i) const
    {
        assert(i < DC);
        vec_t<DR, T> result;
        for(size_type j = 0; j < DR; j++)
            result[j] = values[j][i];
        return result;
    }
    void set_col(size_type i, const vec_t<DR, T> & v)
    {
        assert(i < DC);
        for(size_type j = 0; j < DR; j++)
            values[j][i] = v[j];
    }
    static mat_t<DR, DC, T> identity()
    {
        mat_t<DR, DC, T> result;
        for(size_type i = 0; i < DR; i++)
            for(size_type j = 0; j < DC; j++)
                result[i][j] = (i == j);
        return result;
    }
    mat_t<DC, DR, T> transpose() const
    {
        mat_t<DC, DR, T> result;
        for(size_type i = 0; i < DR; i++)
            for(size_type j = 0; j < DC; j++)
                result[j][i] = values[i][j];
        return result;
    }
    T determenant() const;
    mat_t<DR, DC, T> inverse() const;
    vec_t<DR, T> operator * (const vec_t<DC, T> & other) const
    {
        vec_t<DR, T> result;
        for(size_type i = 0; i < DR; i++)
            for(size_type j = 0; j < DC; j++)
                result[i] += values[i][j] * other[j];
        return result;
    }
    template<size_type U>
    mat_t<DR, U, T> operator * (const mat_t<DC, U, T> & other) const
    {
        mat_t<DR, U, T> result;
        for(size_type i = 0; i < DR; i++)
            for(size_type j = 0; j < DC; j++)
                for(size_type k = 0; k < U; k++)
                    result[i][k] += values[i][j] * other[j][k];
        return result;
    }
    mat_t<DR, DC, T> operator * (const T other) const
    {
        mat_t<DR, DC, T> result;
        for(size_type i = 0; i < DR; i++)
            for(size_type j = 0; j < DC; j++)
                result[i][j] *= other;
        return result;
    }
    mat_t<DR, DC, T> operator / (const T other) const
    {
        mat_t<DR, DC, T> result;
        for(size_type i = 0; i < DR; i++)
            for(size_type j = 0; j < DC; j++)
                result[i][j] /= other;
        return result;
    }

protected:
    vec_t<DC, T> values[DR];
};

// Определитель матрицы 3x3
template<typename T>
T det(const mat_t<3, 3, T> & A)
{
    return A[0][0] * A[1][1] * A[2][2] +
           A[1][0] * A[2][1] * A[0][2] +
           A[0][1] * A[1][2] * A[2][0] -
           A[2][0] * A[1][1] * A[0][2] -
           A[2][1] * A[1][2] * A[0][0] -
           A[1][0] * A[0][1] * A[2][2];
}

// Определитель матрицы 4x4
template<typename T>
T det(const mat_t<4, 4, T> & A)
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

// Обращение матрицы 3x3
template<typename T>
mat_t<3, 3, T> inv(const mat_t<3, 3, T> & A, T & detA)
{
    mat_t<3, 3, T> Inv_A;
    detA = det(A);

    Inv_A[0][0] = (A[1][1] * A[2][2] - A[2][1] * A[1][2]) / detA;
    Inv_A[0][1] = - (A[0][1] * A[2][2] - A[2][1] * A[0][2]) / detA;
    Inv_A[0][2] = (A[0][1] * A[1][2] - A[1][1] * A[0][2]) / detA;

    Inv_A[1][0] = - (A[1][0] * A[2][2] - A[2][0] * A[1][2]) / detA;
    Inv_A[1][1] = (A[0][0] * A[2][2] - A[2][0] * A[0][2]) / detA;
    Inv_A[1][2] = - (A[0][0] * A[1][2] - A[1][0] * A[0][2]) / detA;

    Inv_A[2][0] = (A[1][0] * A[2][1] - A[2][0] * A[1][1]) / detA;
    Inv_A[2][1] = - (A[0][0] * A[2][1] - A[2][0] * A[0][1]) / detA;
    Inv_A[2][2] = (A[0][0] * A[1][1] - A[1][0] * A[0][1]) / detA;

    return Inv_A;
}

// Обращение матрицы 4x4
template<typename T>
mat_t<4, 4, T> inv(const mat_t<4, 4, T> & A, T & detA)
{
    mat_t<4, 4, T> Inv_A;
    detA = det(A);

    Inv_A[0][0] = (A[3][1] * (A[1][2] * A[2][3] - A[1][3] * A[2][2]) +
                   A[3][2] * (A[1][3] * A[2][1] - A[1][1] * A[2][3]) +
                   A[3][3] * (A[1][1] * A[2][2] - A[1][2] * A[2][1])) / detA;
    Inv_A[0][1] = (A[3][1] * (A[0][3] * A[2][2] - A[0][2] * A[2][3]) +
                   A[3][2] * (A[0][1] * A[2][3] - A[0][3] * A[2][1]) +
                   A[3][3] * (A[0][2] * A[2][1] - A[0][1] * A[2][2])) / detA;
    Inv_A[0][2] = (A[3][1] * (A[0][2] * A[1][3] - A[0][3] * A[1][2]) +
                   A[3][2] * (A[0][3] * A[1][1] - A[0][1] * A[1][3]) +
                   A[3][3] * (A[0][1] * A[1][2] - A[0][2] * A[1][1])) / detA;
    Inv_A[0][3] = (A[2][1] * (A[0][3] * A[1][2] - A[0][2] * A[1][3]) +
                   A[2][2] * (A[0][1] * A[1][3] - A[0][3] * A[1][1]) +
                   A[2][3] * (A[0][2] * A[1][1] - A[0][1] * A[1][2])) / detA;

    Inv_A[1][0] = (A[3][0] * (A[1][3] * A[2][2] - A[1][2] * A[2][3]) +
                   A[3][2] * (A[1][0] * A[2][3] - A[1][3] * A[2][0]) +
                   A[3][3] * (A[1][2] * A[2][0] - A[1][0] * A[2][2])) / detA;
    Inv_A[1][1] = (A[3][0] * (A[0][2] * A[2][3] - A[0][3] * A[2][2]) +
                   A[3][2] * (A[0][3] * A[2][0] - A[0][0] * A[2][3]) +
                   A[3][3] * (A[0][0] * A[2][2] - A[0][2] * A[2][0])) / detA;
    Inv_A[1][2] = (A[3][0] * (A[0][3] * A[1][2] - A[0][2] * A[1][3]) +
                   A[3][2] * (A[0][0] * A[1][3] - A[0][3] * A[1][0]) +
                   A[3][3] * (A[0][2] * A[1][0] - A[0][0] * A[1][2])) / detA;
    Inv_A[1][3] = (A[2][0] * (A[0][2] * A[1][3] - A[0][3] * A[1][2]) +
                   A[2][2] * (A[0][3] * A[1][0] - A[0][0] * A[1][3]) +
                   A[2][3] * (A[0][0] * A[1][2] - A[0][2] * A[1][0])) / detA;

    Inv_A[2][0] = (A[3][0] * (A[1][1] * A[2][3] - A[1][3] * A[2][1]) +
                   A[3][1] * (A[1][3] * A[2][0] - A[1][0] * A[2][3]) +
                   A[3][3] * (A[1][0] * A[2][1] - A[1][1] * A[2][0])) / detA;
    Inv_A[2][1] = (A[3][0] * (A[0][3] * A[2][1] - A[0][1] * A[2][3]) +
                   A[3][1] * (A[0][0] * A[2][3] - A[0][3] * A[2][0]) +
                   A[3][3] * (A[0][1] * A[2][0] - A[0][0] * A[2][1])) / detA;
    Inv_A[2][2] = (A[3][0] * (A[0][1] * A[1][3] - A[0][3] * A[1][1]) +
                   A[3][1] * (A[0][3] * A[1][0] - A[0][0] * A[1][3]) +
                   A[3][3] * (A[0][0] * A[1][1] - A[0][1] * A[1][0])) / detA;
    Inv_A[2][3] = (A[2][0] * (A[0][3] * A[1][1] - A[0][1] * A[1][3]) +
                   A[2][1] * (A[0][0] * A[1][3] - A[0][3] * A[1][0]) +
                   A[2][3] * (A[0][1] * A[1][0] - A[0][0] * A[1][1])) / detA;

    Inv_A[3][0] = (A[3][0] * (A[1][2] * A[2][1] - A[1][1] * A[2][2]) +
                   A[3][1] * (A[1][0] * A[2][2] - A[1][2] * A[2][0]) +
                   A[3][2] * (A[1][1] * A[2][0] - A[1][0] * A[2][1])) / detA;
    Inv_A[3][1] = (A[3][0] * (A[0][1] * A[2][2] - A[0][2] * A[2][1]) +
                   A[3][1] * (A[0][2] * A[2][0] - A[0][0] * A[2][2]) +
                   A[3][2] * (A[0][0] * A[2][1] - A[0][1] * A[2][0])) / detA;
    Inv_A[3][2] = (A[3][0] * (A[0][2] * A[1][1] - A[0][1] * A[1][2]) +
                   A[3][1] * (A[0][0] * A[1][2] - A[0][2] * A[1][0]) +
                   A[3][2] * (A[0][1] * A[1][0] - A[0][0] * A[1][1])) / detA;
    Inv_A[3][3] = (A[2][0] * (A[0][1] * A[1][2] - A[0][2] * A[1][1]) +
                   A[2][1] * (A[0][2] * A[1][0] - A[0][0] * A[1][2]) +
                   A[2][2] * (A[0][0] * A[1][1] - A[0][1] * A[1][0])) / detA;

    return Inv_A;
}

// Вычисление определителя как метод матрицы
template<size_type DR, size_type DC, typename T>
T mat_t<DR, DC, T>::determenant() const
{
    return det(* this);
}

// Обращение матрицы как метод матрицы
template<size_type DR, size_type DC, typename T>
mat_t<DR, DC, T> mat_t<DR, DC, T>::inverse() const
{
    T d;
    return inv(* this, d);
}

typedef mat_t<4, 4, float> matrix;

// =================================================================================================

#endif // SWRAST_GEOMETRY_H
