/* 
   Copyright (C) 2011-2016,
        Mikhail Alexandrov  <alexandroff.m@gmail.com>
        Andrey Kurochkin    <andy-717@yandex.ru>
        Peter Zhigalov      <peter.zhigalov@gmail.com>

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

#if !defined(PHYSICALLABCORE_CMILKSHAPEMODEL_H_INCLUDED)
#define PHYSICALLABCORE_CMILKSHAPEMODEL_H_INCLUDED

#include "CModel.h"

/// @brief Класс для описания моделей в формате MilkShape 3D
class CMilkshapeModel: public CModel
{
public:
    CMilkshapeModel();
    ~CMilkshapeModel();

    /// @brief loadModelData - загрузка модели из файла
    /// @param[in] filename - имя файла
    /// @return true, если загрузка прошла успешно, false иначе
    bool loadModelData(const char * filename);

protected:
    /// @brief generateNormales - просчет нормалей
    void generateNormales();
};

#endif // PHYSICALLABCORE_CMILKSHAPEMODEL_H_INCLUDED

