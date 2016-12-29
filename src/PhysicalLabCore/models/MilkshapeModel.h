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

#if !defined(PHYSICALLABCORE_MILKSHAPEMODEL_H_INCLUDED)
#define PHYSICALLABCORE_MILKSHAPEMODEL_H_INCLUDED

#include "Model.h"

/// @brief Класс для описания моделей в формате MilkShape 3D
class MilkshapeModel: public Model
{
public:
    MilkshapeModel();
    ~MilkshapeModel();

    /// @brief loadModelData - загрузка модели из файла
    /// @param[in] filename - имя файла
    /// @return true, если загрузка прошла успешно, false иначе
    bool loadModelData(const QString & filename);

protected:
    /// @brief generateNormales - просчет нормалей
    /// @param[in] smoothNormalesWeight - веса сглаживания нормалей [0..1], чем больше, тем сильнее сглаживание
    void generateNormales(float smoothNormalesWeight = 1.0f);
};

#endif // PHYSICALLABCORE_MILKSHAPEMODEL_H_INCLUDED

