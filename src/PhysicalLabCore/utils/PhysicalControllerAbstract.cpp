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

#include "PhysicalControllerAbstract.h"

#include <cassert>

PhysicalControllerAbstract::PhysicalControllerAbstract(QObject * parent)
    : QObject(parent), m_timer(new QTimer(this)), m_simulationSpeed(1.0), m_currentTime(0), m_currentState(SimulationNotRunning)
{
    m_timerStep = 25;
}

PhysicalControllerAbstract::~PhysicalControllerAbstract()
{
}

/// @brief connectToTimer - подсоединяет внешний обработчик событий таймера симуляции
/// @param[in] receiver - объект, который надо подключить к таймеру
/// @param[in] method - слот, на который вешается событие
/// @return true, если connect успешен, false - иначе
bool PhysicalControllerAbstract::connectToTimer(const QObject * receiver, const char * method)
{
    return connect(m_timer, SIGNAL(timeout()), receiver, method);
}

/// @brief disconnectFromTimer - отсоединяет внешний обработчик событий таймера симуляции
/// @param[in] receiver - объект, который надо отключить от таймера
/// @param[in] method - слот, на который вешалось событие
/// @return true, если disconnect успешен, false - иначе
bool PhysicalControllerAbstract::disconnectFromTimer(const QObject * receiver, const char * method)
{
    return disconnect(m_timer, SIGNAL(timeout()), receiver, method);
}

/// @brief startSimulation - слот на запуск симуляции
void PhysicalControllerAbstract::startSimulation()
{
    assert(m_currentState == SimulationNotRunning);
    m_currentTime = 0;
    resetPhysicalEngine();
    m_timer->start(m_timerStep);
    m_currentState = SimulationRunning;
}

/// @brief stopSimulation - слот на остановку симуляции
void PhysicalControllerAbstract::stopSimulation()
{
    assert(m_currentState == SimulationRunning || m_currentState == SimulationPaused);
    m_timer->stop();
    m_currentTime = 0;
    resetPhysicalEngine();
    m_currentState = SimulationNotRunning;
}

/// @brief pauseSimulation - слот на установку на паузу симуляции
void PhysicalControllerAbstract::pauseSimulation()
{
    assert(m_currentState == SimulationRunning);
    m_timer->stop();
    m_currentState = SimulationPaused;
}

/// @brief resumeSimulation - слот на снятие с паузы симуляции
void PhysicalControllerAbstract::resumeSimulation()
{
    assert(m_currentState == SimulationPaused);
    m_timer->start(m_timerStep);
    m_currentState = SimulationRunning;
}

/// @brief currentTime - получить текущее время симуляции
/// @return время в миллисекундах
unsigned long long PhysicalControllerAbstract::currentTime() const
{
    return m_currentTime;
}

/// @brief currentState - получить текущее состояние эмуляции
PhysicalControllerAbstract::SimulationState PhysicalControllerAbstract::currentState() const
{
    return m_currentState;
}

/// @brief timerStep - получить шаг таймера
/// @return шаг таймера в миллисекундах
int PhysicalControllerAbstract::timerStep() const
{
    return m_timerStep;
}

/// @brief setTimerStep - установить шаг таймера
/// @param[in] timerStep - шаг таймера в миллисекундах
void PhysicalControllerAbstract::setTimerStep(int timerStep)
{
    m_timerStep = timerStep;
    if(m_timer->isActive())
    {
        m_timer->stop();
        m_timer->start(m_timerStep);
    }
}

/// @brief simulationSpeed - получить скорость симуляции
/// @return скорость симуляции (1.0 == 100%)
double PhysicalControllerAbstract::simulationSpeed() const
{
    return m_simulationSpeed;
}

/// @brief setSimulationSpeed - установить скорость симуляции
/// @param[in] simulationSpeed - скорость симуляции (1.0 == 100%)
void PhysicalControllerAbstract::setSimulationSpeed(double actionSpeed)
{
    m_simulationSpeed = actionSpeed;
}

