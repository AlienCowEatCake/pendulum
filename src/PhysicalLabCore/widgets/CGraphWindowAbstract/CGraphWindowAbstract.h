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

#if !defined (PHYSICALLABCORE_CGRAPHWINDOWABSTRACT_H_INCLUDED)
#define PHYSICALLABCORE_CGRAPHWINDOWABSTRACT_H_INCLUDED

#include <QMainWindow>
#include <QString>

namespace Ui {
class CGraphWindowAbstract;
}

class CScene2D;

/// @brief Абстрактный класс окно с виджетом для отрисовки графика
/// @note Потомок должен определить метод update(), в котором нужно заполнить arrX и arrY
class CGraphWindowAbstract : public QMainWindow
{
    Q_OBJECT

public:
    explicit CGraphWindowAbstract(bool haveNegativeY, QWidget *parent = 0);
    ~CGraphWindowAbstract();
    /// @brief update - Обновление данных графика
    virtual void update() = 0;
    /// @brief clear - Очистка данных графика
    void clear();
    /// @brief setLabels - Установка заголовка окна и подписей осей
    void setLabels(const QString & title, const QString & labelX, const QString & labelY);

private slots:
    /// @brief on_actionGraphColor_triggered - Слот на событие изменения цвета графика
    void on_actionGraphColor_triggered();
    /// @brief on_actionGraphWidth_triggered - Слот на событие изменения толщины графика
    void on_actionGraphWidth_triggered();
    /// @brief void on_actionSaveGraphFile_triggered - Слот на событие сохранения графика в файл
    void on_actionSaveGraphFile_triggered();

signals:
    /// @brief settingsChanged - Сигнал, испускаемый при изменении настроек
    void settingsChanged();
private slots:
    /// @brief onSettingsChanged - Слот на изменение настроек
    void onSettingsChanged();

protected:
    /// @brief arrX - Получить ссылку на вектор из значений по оси абсцисс
    QVector<float> & arrX();
    /// @brief arrX - Получить константную ссылку на вектор из значений по оси абсцисс
    const QVector<float> & arrX() const;
    /// @brief arrY - Получить ссылку на вектор из значений по оси ординат
    QVector<float> & arrY();
    /// @brief arrY - Получить константную ссылку на вектор из значений по оси ординат
    const QVector<float> & arrY() const;

    /// @brief resizeGraph - Перерисовка виджета с графиком под новый размер
    void resizeGraph(float x0, float x1, float y0, float y1);

    /// @brief scaleX - Получить коэффициент масштабирования по оси X
    float scaleX() const;
    /// @brief scaleY - Получить коэффициент масштабирования по оси Y
    float scaleY() const;

private:
    Ui::CGraphWindowAbstract *m_ui;

    CScene2D * m_scene2D;
    
    QString m_lastSaved;
};

#endif // PHYSICALLABCORE_CGRAPHWINDOWABSTRACT_H_INCLUDED

