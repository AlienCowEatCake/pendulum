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

#if !defined (PHYSICALLABCORE_GRAPHWINDOWABSTRACT_H_INCLUDED)
#define PHYSICALLABCORE_GRAPHWINDOWABSTRACT_H_INCLUDED

#include <QMainWindow>
#include <QString>

namespace Ui {
class GraphWindowAbstract;
}

class Scene2D;
class SettingsWrapper;
class ImageSaver;

/// @brief Абстрактный класс окно с виджетом для отрисовки графика
/// @note Потомок должен определить метод update(), в котором нужно заполнить arrX и arrY
class GraphWindowAbstract : public QMainWindow
{
    Q_OBJECT

public:
    explicit GraphWindowAbstract(bool haveNegativeY, QWidget *parent = 0);
    ~GraphWindowAbstract();
    /// @brief update - Обновление данных графика
    virtual void update() = 0;
    /// @brief clear - Очистка данных графика
    void clear();
    /// @brief setLabels - Установка заголовка окна и подписей осей
    void setLabels(const QString & title, const QString & labelX, const QString & labelY);
    /// @brief settings - Получить SettingsWrapper
    SettingsWrapper & settings();
    const SettingsWrapper & settings() const;

private slots:
    /// @brief on_actionGraphColor_triggered - Слот на событие изменения цвета графика
    void on_actionGraphColor_triggered();
    /// @brief on_actionGraphWidth_triggered - Слот на событие изменения толщины графика
    void on_actionGraphWidth_triggered();
    /// @brief on_actionSaveGraphFile_triggered - Слот на событие сохранения графика в файл
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

    /// @brief settingsMenu - Получить меню с настройками окна для возможности добавления новых настроек
    QMenu * settingsMenu() const;

    /// @brief changeEvent - событие при изменении чего-либо (например, языка)
    void changeEvent(QEvent *event);

private:
    Ui::GraphWindowAbstract *m_ui;

    Scene2D * m_scene2D;
    
    ImageSaver * m_imageSaver;
};

#endif // PHYSICALLABCORE_GRAPHWINDOWABSTRACT_H_INCLUDED

