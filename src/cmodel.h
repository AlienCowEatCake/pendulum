/* 
   Copyright (C) 2011-2014,
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

#ifndef CMODEL_H
#define CMODEL_H

#if !defined USE_SWRAST
#include <QGLWidget>
#include <QtOpenGL>
typedef QGLWidget BaseWidget;
#else
#include "swrast/swrast_common.h"
#include "swrast/swrast_widget.h"
typedef SWRastWidget BaseWidget;
#endif
#include <QImage>
#include <cmath>
#include <cstring>

// класс для описания 3D моделей
class Cmodel
{
protected:
    // Сетка - группа треугольников с одним материалом
    struct Mesh
    {
        int m_materialIndex;
        int m_numTriangles;
        int *m_pTriangleIndices;
    };
    int m_numMeshes;
    Mesh *m_pMeshes;
    // данные о материалах
    struct Material
    {
        float m_ambient[4], m_diffuse[4], m_specular[4], m_emissive[4];
        float m_shininess;
        GLuint m_texture;
        char *m_pTextureFilename;
    };
    int m_numMaterials;
    Material *m_pMaterials;
    // данные о треугольниках
    struct Triangle
    {
        float m_vertexNormals[3][3];
        float m_s[3], m_t[3];
        int m_vertexIndices[3];
    };
    int m_numTriangles;
    Triangle *m_pTriangles;
    // данные о вершинах
    struct Vertex
    {
        char m_boneID;
        float m_location[3];
    };
    int m_numVertices;
    Vertex *m_pVertices;

public:
    // конструктор модели
    Cmodel();
    // деструктор модели
    virtual ~Cmodel();
    // загрузка модели
    virtual bool loadModelData(const char *filename) = 0;
    // вывод модели на экран
    void draw();
    // перезагрузка текстур при потере контекста
    void reloadTextures();
};

// класс, создержащий 3D модели в формате MilkShape 3D
class CMilkshapeModel: public Cmodel
{
public:
    // конструктор модели
    CMilkshapeModel();
    // деструктор модели
    virtual ~CMilkshapeModel();
    // загрузка модели
    virtual bool loadModelData(const char *filename);
protected:
    // просчет нормалей
    void normal_gen();
};

// класс пружинка
class CModelSpring: public CMilkshapeModel
{
protected:
    // индексный массив
    int * a_index;
    // параметры циллиндра
    int num_stacks;
    int num_slices;
    // начальное смещение
    float offset_start;
    // конечное смещение
    float offset_end;
    // количество витков
    int num_turns;
    //диаметр витка
    float radius_turn;
    // диаметр проволоки
    float radius_spring;
    // индексация координат модели
    void indexate();
    // размер по умолчанию
    float def_size;
    // пропуск незначительных изменений
    float length_prev;
    float epsilon;
    // константы
    float pi_x2;
    float pi;
    float pi_d2;
public:
    // конструктор модели
    CModelSpring();
    // деструктор модели
    ~CModelSpring();
    // изменение размера модели
    void resize(float length);
    // инициализация
    void init();
};

#endif //CMODEL_H
