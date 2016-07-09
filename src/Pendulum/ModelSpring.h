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

#if !defined (MODELSPRING_H_INCLUDED)
#define MODELSPRING_H_INCLUDED

#include "models/MilkshapeModel.h"

/// @brief Класс для описания модели пружинки (она умеет растягиваться)
class ModelSpring: public MilkshapeModel
{
public:
    ModelSpring();
    ~ModelSpring();

    /// @brief loadModelData - загрузка модели из файла
    /// @param[in] filename - имя файла
    /// @return true, если загрузка прошла успешно, false иначе
    virtual bool loadModelData(const QString & filename);

    /// @brief resize - изменение размера модели
    /// @param[in] length - новая длина модели
    void resize(float length);

protected:
    /// @brief индексный массив
    int * m_aIndex;
    /// @brief предыдущая длина для пропуска незначительных изменений
    float m_lengthPrevious;

    /// @brief indexate - индексация координат модели
    void indexate();
};

#endif // MODELSPRING_H_INCLUDED
