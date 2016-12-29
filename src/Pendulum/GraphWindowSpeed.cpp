/*
   Copyright (C) 2011-2016,
        Andrei V. Kurochkin     <kurochkin.andrei.v@yandex.ru>
        Mikhail E. Aleksandrov  <alexandroff.m@gmail.com>
        Peter S. Zhigalov       <peter.zhigalov@gmail.com>

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

#include "GraphWindowSpeed.h"

#include <cmath>
#include <QEvent>
#include <QAction>
#include <QMenu>
#include <QInputDialog>
#include "utils/SettingsWrapper.h"
#include "PhysicalController.h"

GraphWindowSpeed::GraphWindowSpeed(QWidget * parent)
    : GraphWindowAbstract(true, parent)
    , m_physicalController(NULL)
{
    setNumPerionds(3);
    m_actionSetNumPeriods = new QAction(this);
    connect(m_actionSetNumPeriods, SIGNAL(triggered()), this, SLOT(onSetNumPeriodsTriggered()));
    QAction * oldFirstAction = settingsMenu()->actions().first();
    settingsMenu()->insertAction(oldFirstAction, m_actionSetNumPeriods);
    settingsMenu()->insertSeparator(oldFirstAction);
    updateActions();
    updateTitle();
}

void GraphWindowSpeed::setPhysicalController(const PhysicalController * physicalController)
{
    m_physicalController = physicalController;
}

void GraphWindowSpeed::update()
{
    if(!m_physicalController)
        return;
    clear();

    QVector<float> & arrX = this->arrX();
    QVector<float> & arrY = this->arrY();
    Action action(m_physicalController->action());

    if(action.get_w0() <= action.get_sigma() && std::fabs(action.get_A0()) < 0.0001)
    {
        arrX.push_back(0.0f);
        arrX.push_back(1.0f);
        arrY.push_back(0.0f);
        arrY.push_back(0.0f);

        resizeGraph(0.0f, 1.0f, -1.0f, 1.0f);
    }
    else
    {
        const int numT = numPeriods();

        double mactiont;
        if(action.get_w0() <= action.get_sigma())
            mactiont = -std::log(std::fabs(0.0000001 / action.get_A0())) / (numT * action.get_sigma());
        else
            mactiont = action.get_T();
        action.InitBall();
        double maxspeed = 0.0;

        const double di = mactiont / 50.0 * 1000.0;
        for(double i = 0.0; i <= mactiont * numT * 1000.0; i += di)
        {
            action.Refresh(i);
            arrX.push_back(static_cast<float>(i / 1000.0));
            arrY.push_back(static_cast<float>(action.get_v()));
            if(std::fabs(action.get_v()) > maxspeed)
                maxspeed = std::fabs(action.get_v());
        }

        const float maxspeedf = static_cast<float>(maxspeed);
        resizeGraph(0.0f, static_cast<float>(mactiont) * static_cast<float>(numT), -maxspeedf, maxspeedf);

        for(int i = 0; i < arrY.size(); i++)
        {
            arrX[i] *= scaleX();
            arrY[i] *= scaleY();
        }
    }

    repaint();
}

void GraphWindowSpeed::changeEvent(QEvent * event)
{
    GraphWindowAbstract::changeEvent(event);
    if(event->type() == QEvent::LanguageChange)
    {
        updateTitle();
        updateActions();
    }
}

void GraphWindowSpeed::updateTitle()
{
    setLabels(tr("Speed"), tr("t, s"), tr("v, m/s"));
}

void GraphWindowSpeed::updateActions()
{
    m_actionSetNumPeriods->setText(tr("Number of &Periods..."));
}

void GraphWindowSpeed::onSetNumPeriodsTriggered()
{
    bool ok = false;
    const int value =
#if (QT_VERSION >= QT_VERSION_CHECK(4, 5, 0))
            QInputDialog::getInt
#else
            QInputDialog::getInteger
#endif
            (this, tr("Number of Periods"), tr("Enter Number of Periods:"), numPeriods(), 1, 1999, 1, &ok);
    if(ok)
    {
        setNumPerionds(value);
        emit settingsChanged();
    }
}

void GraphWindowSpeed::setNumPerionds(int value)
{
    settings().setValue(QString::fromLatin1("NumT"), value);
}

int GraphWindowSpeed::numPeriods() const
{
    static const int defaultValue = 3;
    const QVariant rawValue = settings().value(QString::fromLatin1("NumT"), defaultValue);
    const int value = rawValue.isValid() ? rawValue.toInt() : defaultValue;
    return value > 0 ? value : defaultValue;
}

