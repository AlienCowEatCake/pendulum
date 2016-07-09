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

#if !defined (SCENE3D_H_INCLUDED)
#define SCENE3D_H_INCLUDED

#include "widgets/Scene3DAbstract/Scene3DAbstract.h"
#include "ModelSpring.h"

class PhysicalController;

/// @brief Класс 3D сцены в главном окне
class Scene3D : public Scene3DAbstract
{
    Q_OBJECT

public:
    Scene3D(QWidget* parent = 0);

    /// @brief setPhysicalController - установить контроллер физического процесса
    void setPhysicalController(const PhysicalController * physicalController);

protected:
    /// @brief initializeGL - инициализировать OpenGL-сцену
    void initializeGL();
    /// @brief paintGL - отрисовать OpenGL-сцену
    void paintGL();

private:
    MilkshapeModel m_tripod;        ///< Штатив
    MilkshapeModel m_sphere;        ///< Грузик
    MilkshapeModel m_scale;         ///< Линейка
    MilkshapeModel m_spring_start;  ///< Начальный сегмент пружины
    MilkshapeModel m_spring_end;    ///< Конечный сегмент пружины
    MilkshapeModel m_strng;         ///< Указатель от грузика к линейке
    ModelSpring m_cyllinder;        ///< Тело пружины

    const PhysicalController * m_physicalController;
};

#endif // SCENE3D_H_INCLUDED
