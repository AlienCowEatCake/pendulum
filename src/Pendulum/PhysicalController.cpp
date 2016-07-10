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

#include "PhysicalController.h"

#include <cassert>

PhysicalController::PhysicalController(QObject * parent)
    : PhysicalControllerAbstract(parent)
{
#if !defined (USE_SWRAST)
    m_timerStep = 25;
#else
    m_timerStep = 40;
#endif
    connectToTimer(this, SLOT(actionTime()));
}

/// @brief action - получить константный модуль физических расчетов
const Action & PhysicalController::action() const
{
    return m_action;
}

/// @brief action - получить модуль физических расчетов
Action &PhysicalController::action()
{
    return m_action;
}

/// @brief resetPhysicalEngine - сброс состояния физического процесса
void PhysicalController::resetPhysicalEngine()
{
    m_action.InitBall();
}

/// @brief actionTime - слот на событие таймера, обновляет m_action
void PhysicalController::actionTime()
{
    m_currentTime += static_cast<long long unsigned int>(m_timerStep * m_simulationSpeed);
    m_action.Refresh(m_currentTime);
}
