/*
   Copyright (C) 2011-2016,
        Mikhail Alexandrov  <alexandroff.m@gmail.com>
        Andrey Kurochkin    <andy-717@yandex.ru>
        Peter Zhigalov      <peter.zhigalov@gmail.com>

   This file is part of the `pendulum' program.

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

#include "ModelSpring.h"

#include <cmath>

namespace {

/// @brief параметры циллиндра
const int numStacks = 257;
const int numSlices = 6;
/// @brief начальное смещение
const float offsetStart = -0.0014f;
/// @brief конечное смещение
const float offsetEnd = -0.0014f;
/// @brief количество витков
const int numTurns = 10;
/// @brief диаметр витка
const float radiusTurn = 0.0989f;
/// @brief диаметр проволоки
const float radiusSpring = 0.0065f;
/// @brief размер по умолчанию
const float defSize = 1.0f;
/// @brief эпсилон, при котором ресайз не производится
const float epsilon = 0.0001f;

/// @brief Pi
const float pi = 3.1415926535897932384626433832795f;
/// @brief Pi * 2
const float pi_x2 = 6.283185307179586476925286766559f;
/// @brief Pi / 2
const float pi_d2 = 1.5707963267948966192313216916398f;

} // namespace

ModelSpring::ModelSpring()
    : m_lengthPrevious(0.0f)
{
    m_aIndex = new int [numStacks * numSlices];
    for(int i = 0; i < numStacks * numSlices; i++)
        m_aIndex[i] = -1;
}

ModelSpring::~ModelSpring()
{
    delete [] m_aIndex;
    m_aIndex = NULL;
}

/// @brief loadModelData - загрузка модели из файла
/// @param[in] filename - имя файла
/// @return true, если загрузка прошла успешно, false иначе
bool ModelSpring::loadModelData(const QString & filename)
{
    bool status = MilkshapeModel::loadModelData(filename);
    if(status)
    {
        indexate();
        resize(defSize);
        generateNormales();
    }
    return status;
}

/// @brief resize - изменение размера модели
/// @param[in] length - новая длина модели
void ModelSpring::resize(float length)
{
    length -= (offsetStart + offsetEnd);
    // длина с корректировками
    if(std::fabs(length - m_lengthPrevious) > epsilon)
    {
        int index = 0;
        // координаты скелета
        float z_skel = offsetStart;
        float x_skel, y_skel;
        // количество точек скелета в витке
        int num_oneturn = numStacks / numTurns;
        // шаг скелета
        float z_skel_step = length / numTurns / num_oneturn;
        float fi_skel_step = pi_x2 / num_oneturn;
        // обход по виткам
        for(int i = 0; i < numTurns; i++)
        {
            // обход по витку
            for(float fi_skel = 0.0f; fi_skel < pi_x2 - fi_skel_step / 2.0f; fi_skel += fi_skel_step)
            {
                float cos_fi_skel(std::cos(fi_skel));
                float sin_fi_skel(std::sin(fi_skel));
                x_skel = radiusTurn * cos_fi_skel;
                y_skel = radiusTurn * sin_fi_skel;
                // матрица перехода
                float a[3][3];
                float cos_fi_2 = std::cos(fi_skel + pi_d2);
                float sin_fi_2 = std::sin(fi_skel + pi_d2);
                float det_a = cos_fi_skel * sin_fi_2 - cos_fi_2 * sin_fi_skel;
                a[0][0] = sin_fi_2 / det_a;
                a[0][1] = cos_fi_2 / det_a;
                a[0][2] = a[1][2] = a[2][0] = a[2][1] = 0.0f;
                a[1][0] = sin_fi_skel / det_a;
                a[1][1] = cos_fi_skel / det_a;
                a[2][2] = 1.0f;
                // построение элементарных окружностей
                float fi = 0.0f;
                float fi_step = pi_x2 / numSlices;
                for(int j = 0; j < numSlices; j++)
                {
                    // координаты элементарного круга
                    float x_ = radiusSpring * std::cos(fi);
                    float y_ = 0.0f;
                    float z_ = radiusSpring * std::sin(fi);
                    // сдвинутые и повернутые координаты
                    m_pVertices[m_aIndex[index]].m_location[0] = x_ * a[0][0] + y_ * a[0][1] + z_ * a[0][2] - x_skel;
                    m_pVertices[m_aIndex[index]].m_location[1] = x_ * a[2][0] + y_ * a[2][1] + z_ * a[2][2] - z_skel;
                    m_pVertices[m_aIndex[index]].m_location[2] = x_ * a[1][0] + y_ * a[1][1] + z_ * a[1][2] - y_skel;
                    index++;
                    fi += fi_step;
                }
                z_skel += z_skel_step;
            }
        }
        // последний сектор
        while(index < m_numVertices)
        {
            x_skel = radiusTurn;
            y_skel = 0.0f;
            // построение элементарных окружностей
            float fi = 0.0f;
            float fi_step = pi_x2 / numSlices;
            for(int j = 0; j < numSlices && index < m_numVertices; j++)
            {
                // координаты элементарного круга
                float x_ = radiusSpring * std::cos(fi);
                float y_ = 0.0f;
                float z_ = radiusSpring * std::sin(fi);
                // сдвинутые и повернутые координаты
                m_pVertices[m_aIndex[index]].m_location[0] = x_ - x_skel;
                m_pVertices[m_aIndex[index]].m_location[1] = z_ - z_skel;
                m_pVertices[m_aIndex[index]].m_location[2] = y_ - y_skel;
                index++;
                fi += fi_step;
            }
        }
        m_lengthPrevious = length;
    }
}

/// @brief indexate - индексация координат модели
void ModelSpring::indexate()
{
    float * a_y_tmp = new float [numStacks];
    for(int i = 0; i < numStacks; i++)
        a_y_tmp[i] = 0.0;
    // разбиение на группы
    for(int i = 0, k = 0; i < m_numVertices; i++)
    {
        int flag = 0;
        for(int j = 0; j < k && flag == 0; j++)
        {
            if(a_y_tmp[j] == m_pVertices[i].m_location[1])
            {
                flag = 1;
            }
        }
        if(flag == 0)
        {
            a_y_tmp[k] = m_pVertices[i].m_location[1];
            k++;
        }
    }
    // разбиение по группам
    for(int i = 0; i < m_numVertices; i++)
    {
        int index = -1;
        for(int j = 0; j < numStacks && index == -1; j++)
        {
            if(a_y_tmp[j] == m_pVertices[i].m_location[1])
            {
                index = j;
            }
        }
        for(int j = index * numSlices; j < index * numSlices + numSlices; j++)
        {
            if(m_aIndex[j] == -1)
            {
                m_aIndex[j] = i;
                j = index * numSlices + numSlices;
            }
        }
    }
    delete [] a_y_tmp;
}
