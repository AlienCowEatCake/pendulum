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

#if !defined (PHYSICALCONTROLLER_H_INCLUDED)
#define PHYSICALCONTROLLER_H_INCLUDED

#include "utils/PhysicalControllerAbstract.h"
#include "Action.h"

/// @brief Класс контроллер физического процесса
class PhysicalController : public PhysicalControllerAbstract
{
    Q_OBJECT

public:
    PhysicalController(QObject * parent = NULL);

    /// @brief action - получить константный модуль физических расчетов
    const Action & action() const;
    /// @brief action - получить модуль физических расчетов
    Action & action();

    /// @brief resetPhysicalEngine - сброс состояния физического процесса
    void resetPhysicalEngine();

private slots:

    /// @brief actionTime - слот на событие таймера, обновляет m_action
    void actionTime();

private:

    /// @brief Модуль физических расчетов
    Action m_action;

};

#endif // PHYSICALCONTROLLER_H_INCLUDED

