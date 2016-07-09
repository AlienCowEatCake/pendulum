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

#if !defined (PHYSICALLABCORE_PHYSICALCONTROLLERABSTRACT_H_INCLUDED)
#define PHYSICALLABCORE_PHYSICALCONTROLLERABSTRACT_H_INCLUDED

#include <QTimer>

/// @brief Класс абстрактный контроллер физического процесса
class PhysicalControllerAbstract : public QObject
{
    Q_OBJECT

public:
    PhysicalControllerAbstract(QObject * parent = NULL);
    ~PhysicalControllerAbstract();

    /// @brief Основные состояния симуляции
    enum SimulationState
    {
        SimulationNotRunning,   ///< Симуляция не запущена
        SimulationRunning,      ///< Симуляция запущена
        SimulationPaused        ///< Симуляция на паузе
    };

    /// @brief connectToTimer - подсоединяет внешний обработчик событий таймера симуляции
    /// @param[in] receiver - объект, который надо подключить к таймеру
    /// @param[in] method - слот, на который вешается событие
    /// @return true, если connect успешен, false - иначе
    bool connectToTimer(const QObject * receiver, const char * method);
    /// @brief disconnectFromTimer - отсоединяет внешний обработчик событий таймера симуляции
    /// @param[in] receiver - объект, который надо отключить от таймера
    /// @param[in] method - слот, на который вешалось событие
    /// @return true, если disconnect успешен, false - иначе
    bool disconnectFromTimer(const QObject * receiver, const char * method);

    /// @brief timerStep - получить шаг таймера
    /// @return шаг таймера в миллисекундах
    int timerStep() const;
    /// @brief setTimerStep - установить шаг таймера
    /// @param[in] timerStep - шаг таймера в миллисекундах
    void setTimerStep(int timerStep);

    /// @brief simulationSpeed - получить скорость симуляции
    /// @return скорость симуляции (1.0 == 100%)
    double simulationSpeed() const;
    /// @brief setSimulationSpeed - установить скорость симуляции
    /// @param[in] simulationSpeed - скорость симуляции (1.0 == 100%)
    void setSimulationSpeed(double simulationSpeed);

    /// @brief currentState - получить текущее состояние эмуляции
    SimulationState currentState() const;

    /// @brief currentTime - получить текущее время симуляции
    /// @return время в миллисекундах
    unsigned long long currentTime() const;

    /// @brief resetPhysicalEngine - сброс состояния физического процесса
    virtual void resetPhysicalEngine() = 0;

public slots:

    /// @brief startSimulation - слот на запуск симуляции
    void startSimulation();
    /// @brief stopSimulation - слот на остановку симуляции
    void stopSimulation();
    /// @brief pauseSimulation - слот на установку на паузу симуляции
    void pauseSimulation();
    /// @brief resumeSimulation - слот на снятие с паузы симуляции
    void resumeSimulation();

protected:

    /// @brief Таймер симуляции
    QTimer * m_timer;

    /// @brief Шаг таймера в миллисекундах
    int m_timerStep;
    /// @brief Скорость симуляции (1.0 == 100%)
    double m_simulationSpeed;

    /// @brief Время с начала симуляции в миллисекундах
    unsigned long long int m_currentTime;

    /// @brief Текущее состояние симуляции
    SimulationState m_currentState;
};

#endif // PHYSICALLABCORE_PHYSICALCONTROLLERABSTRACT_H_INCLUDED

