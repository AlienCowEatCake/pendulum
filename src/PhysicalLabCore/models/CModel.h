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

#if !defined(PHYSICALLABCORE_CMODEL_H_INCLUDED)
#define PHYSICALLABCORE_CMODEL_H_INCLUDED

#include "../widgets/GLWidgetImpl/GLWidgetImpl.h"

/// @brief Класс для описания абстрактных 3D моделей
class CModel
{
protected:

    /// @brief Сетка - группа треугольников с одним материалом
    struct Mesh
    {
        int m_materialIndex;
        int m_numTriangles;
        int *m_pTriangleIndices;
    };
    int m_numMeshes;
    Mesh * m_pMeshes;

    /// @brief Материал - структура, хранящая параметры освещения и текстуру
    struct Material
    {
        float m_ambient[4], m_diffuse[4], m_specular[4], m_emissive[4];
        float m_shininess;
        GLuint m_texture;
        char *m_pTextureFilename;
    };
    int m_numMaterials;
    Material *m_pMaterials;

    /// @brief Треугольник - структура, хранящая индексы точек, координаты нормалей и текстур
    struct Triangle
    {
        float m_vertexNormals[3][3];
        float m_s[3], m_t[3];
        int m_vertexIndices[3];
    };
    int m_numTriangles;
    Triangle *m_pTriangles;

    /// @brief Вершина - структура, хранящая свои координаты и идентификатор кости, к которой она принадлежит
    struct Vertex
    {
        char m_boneID;
        float m_location[3];
    };
    int m_numVertices;
    Vertex *m_pVertices;

    /// @brief reloadTextures - перезагрузка всех текстур модели
    void reloadTextures();

public:
    CModel();
    virtual ~CModel();

    /// @brief loadModelData - загрузка модели из файла
    /// @param[in] filename - имя файла
    /// @return true, если загрузка прошла успешно, false иначе
    virtual bool loadModelData(const char * filename) = 0;

    /// @brief draw - отрисовка модели
    void draw() const;
};

#endif // PHYSICALLABCORE_CMODEL_H_INCLUDED

