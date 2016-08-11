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

#include "Model.h"

#include <algorithm>
#include <QImage>
#include <QMap>

namespace {

/// @brief LoadGLTextures - загрузка текстуры из файла
/// @param[in] name - имя файла с текстурой
/// @return идентификатор текстуры
GLuint LoadGLTextures(const char * name)
{
    static QMap<QString, GLuint> texturesCache;
    QString qName = QString::fromUtf8(name);
    QMap<QString, GLuint>::ConstIterator it = texturesCache.find(qName);
    if(it == texturesCache.end())
    {
        QImage texImage;
        GLuint texPtr;
        texImage.load(qName);
#if !defined (USE_SWRAST)
        // Некоторые железные реализации OpenGL не поддерживают не квадратные текстуры. Другие же
        // не поддерживают текстуры, размерами не равные степеням двойки. У третьих сильно
        // ограничен максимальный размер текстуры. Поэтому для них всех отресайзим текстуру до
        // квадрата, размером в степень двойки, но в пределах максимального размера. Для софтверной
        // реализации это только навредит, поэтому для нее этого делать не будем.
        int imgSize = std::max(texImage.height(), texImage.width()), texSize = 1;
        while(texSize < imgSize)
            texSize *= 2;
        GLint maxTexSize = texSize;
        glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTexSize);
        maxTexSize = std::max(maxTexSize, 64);
        if(texSize > maxTexSize)
            texSize = maxTexSize;
        if(texImage.height() != texSize || texImage.width() != texSize)
            texImage = texImage.scaled(texSize, texSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
#endif
        texImage = GLWidgetImpl::convertToGLFormat(texImage);
        GLImpl::glGenTextures(1, &texPtr);
        GLImpl::glBindTexture(GL_TEXTURE_2D, texPtr);
        GLsizei texWidth = static_cast<GLsizei>(texImage.width());
        GLsizei texHeight = static_cast<GLsizei>(texImage.height());
        GLImpl::glTexImage2D(GL_TEXTURE_2D, 0, 3, texWidth, texHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, texImage.bits());
        GLImpl::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        GLImpl::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        texturesCache[qName] = texPtr;
        return texPtr;
    }
    return it.value();
}

} // namespace

Model::Model()
    : m_numMeshes(0), m_pMeshes(NULL),
      m_numMaterials(0), m_pMaterials(NULL),
      m_numTriangles(0), m_pTriangles(NULL),
      m_numVertices(0), m_pVertices(NULL)
{}

Model::~Model()
{
    for(int i = 0; i < m_numMeshes; i++)
        delete [] m_pMeshes[i].m_pTriangleIndices;
    for(int i = 0; i < m_numMaterials; i++)
        delete [] m_pMaterials[i].m_pTextureFilename;

    m_numMeshes = 0;
    delete [] m_pMeshes;
    m_pMeshes = NULL;

    m_numMaterials = 0;
    delete [] m_pMaterials;
    m_pMaterials = NULL;

    m_numTriangles = 0;
    delete [] m_pTriangles;
    m_pTriangles = NULL;

    m_numVertices = 0;
    delete [] m_pVertices;
    m_pVertices = NULL;
}

/// @brief draw - отрисовка модели
void Model::draw() const
{
    GLboolean texEnabled = GLImpl::glIsEnabled(GL_TEXTURE_2D);  //если поддерживается загрузка текстур
    for(int i = 0; i < m_numMeshes; i++)    // отрисовка по группам с одинаковыми материалами
    {
        int materialIndex = m_pMeshes[i].m_materialIndex;
        if(materialIndex >= 0)  // если материал задан
        {
            // установить параметры материала
            GLImpl::glMaterialfv(GL_FRONT, GL_AMBIENT, m_pMaterials[materialIndex].m_ambient);
            GLImpl::glMaterialfv(GL_FRONT, GL_DIFFUSE, m_pMaterials[materialIndex].m_diffuse);
            GLImpl::glMaterialfv(GL_FRONT, GL_SPECULAR, m_pMaterials[materialIndex].m_specular);
            GLImpl::glMaterialfv(GL_FRONT, GL_EMISSION, m_pMaterials[materialIndex].m_emissive);
            GLImpl::glMaterialf(GL_FRONT, GL_SHININESS, m_pMaterials[materialIndex].m_shininess);
            if(m_pMaterials[materialIndex].m_texture > 0)   // если задана текстура
            {
                // установить параметры текстуры
                GLImpl::glBindTexture(GL_TEXTURE_2D, m_pMaterials[materialIndex].m_texture);
                GLImpl::glEnable(GL_TEXTURE_2D);
            }
            else
            {
                // иначе отключить текстуры
                GLImpl::glDisable(GL_TEXTURE_2D);
            }
        }
        else
        {
            // иначе отключить материалы
            GLImpl::glDisable(GL_TEXTURE_2D);
        }
        // начать вывод по треугольникам
        GLImpl::glBegin(GL_TRIANGLES);
        {
            for(int j = 0; j < m_pMeshes[i].m_numTriangles; j++)
            {
                // добавление в OpenGL конвеер координаты треугольников и нормалей
                int triangleIndex = m_pMeshes[i].m_pTriangleIndices[j];
                const Triangle * pTri = &m_pTriangles[triangleIndex];
                for(int k = 0; k < 3; k++)
                {
                    int index = pTri->m_vertexIndices[k];
                    GLImpl::glNormal3fv(pTri->m_vertexNormals[k]);
                    GLImpl::glTexCoord2f(pTri->m_s[k], pTri->m_t[k]);
                    GLImpl::glVertex3fv(m_pVertices[index].m_location);
                }
            }
        }
        GLImpl::glEnd();
    }
    // если задана текстура
    if(texEnabled)
        GLImpl::glEnable(GL_TEXTURE_2D);    // включить текстуры
    else
        GLImpl::glDisable(GL_TEXTURE_2D);   // иначе отключить текстуры
}

/// @brief reloadTextures - перезагрузка всех текстур модели
void Model::reloadTextures()
{
    for(int i = 0; i < m_numMaterials; i++)
    {
        if(m_pMaterials[i].m_pTextureFilename[0] != '\0')
            m_pMaterials[i].m_texture = LoadGLTextures(m_pMaterials[i].m_pTextureFilename);
        else
            m_pMaterials[i].m_texture = 0;
    }
}

