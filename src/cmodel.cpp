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

#include "cmodel.h"

#if !defined USE_SWRAST
GLuint LoadGLTextures(const char* name)
{
    QImage texti;
    GLuint texPntr[1];
    texti.load(name);
    texti = QGLWidget::convertToGLFormat(texti);
    glGenTextures(1, &texPntr[0]);
    glBindTexture(GL_TEXTURE_2D, texPntr[0]);
    glTexImage2D(GL_TEXTURE_2D, 0, 3, (GLsizei)texti.width(), (GLsizei)texti.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, texti.bits());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    return texPntr[0];
}
#else
GLuint LoadGLTextures(const char* name)
{
    Q_UNUSED(name);
    // TODO
    return 0;
}
#endif

//конструктор модели
Cmodel::Cmodel()
{
    m_numMeshes = 0;
    m_pMeshes = NULL;
    m_numMaterials = 0;
    m_pMaterials = NULL;
    m_numTriangles = 0;
    m_pTriangles = NULL;
    m_numVertices = 0;
    m_pVertices = NULL;
}

//деструктор модели
Cmodel::~Cmodel()
{
    int i;
    for (i = 0; i < m_numMeshes; i++)
        delete [] m_pMeshes[i].m_pTriangleIndices;
    for (i = 0; i < m_numMaterials; i++)
        delete [] m_pMaterials[i].m_pTextureFilename;

    m_numMeshes = 0;
    if(m_pMeshes)
    {
        delete [] m_pMeshes;
        m_pMeshes = NULL;
    }

    m_numMaterials = 0;
    if(m_pMaterials)
    {
        delete [] m_pMaterials;
        m_pMaterials = NULL;
    }

    m_numTriangles = 0;
    if(m_pTriangles)
    {
        delete [] m_pTriangles;
        m_pTriangles = NULL;
    }

    m_numVertices = 0;
    if(m_pVertices)
    {
        delete [] m_pVertices;
        m_pVertices = NULL;
    }
}

// вывод модели на экран
void Cmodel::draw()
{
    GLboolean texEnabled = glIsEnabled(GL_TEXTURE_2D); //если поддерживается загрузка текстур
    for(int i = 0; i < m_numMeshes; i++) // отрисовка по группам с одинаковыми материалами
    {
        int materialIndex = m_pMeshes[i].m_materialIndex;
        if(materialIndex >= 0) // если материал задан
        {
            // установить параметры материала
            glMaterialfv(GL_FRONT, GL_AMBIENT, m_pMaterials[materialIndex].m_ambient);
            glMaterialfv(GL_FRONT, GL_DIFFUSE, m_pMaterials[materialIndex].m_diffuse);
            glMaterialfv(GL_FRONT, GL_SPECULAR, m_pMaterials[materialIndex].m_specular);
            glMaterialfv(GL_FRONT, GL_EMISSION, m_pMaterials[materialIndex].m_emissive);
            glMaterialf(GL_FRONT, GL_SHININESS, m_pMaterials[materialIndex].m_shininess);
            // если задана текстура
            if (m_pMaterials[materialIndex].m_texture > 0)
            {// установить параметры текстуры
                glBindTexture(GL_TEXTURE_2D, m_pMaterials[materialIndex].m_texture);
                glEnable(GL_TEXTURE_2D);
            }
            else // иначе отключить текстуры
                glDisable(GL_TEXTURE_2D);
        }
        else
        {
            // иначе отключить материалы
            glDisable(GL_TEXTURE_2D);
        }
        // начать вывод по треугольникам
        glBegin(GL_TRIANGLES);
        {
            for(int j = 0; j < m_pMeshes[i].m_numTriangles; j++)
            {// добавление в OpenGL конвеер координаты треугольников и нормалей
                int triangleIndex = m_pMeshes[i].m_pTriangleIndices[j];
                const Triangle* pTri = &m_pTriangles[triangleIndex];
                for(int k = 0; k < 3; k++)
                {
                    int index = pTri->m_vertexIndices[k];
                    glNormal3fv(pTri->m_vertexNormals[k]);
                    glTexCoord2f(pTri->m_s[k], pTri->m_t[k]);
                    glVertex3fv(m_pVertices[index].m_location);
                }
            }
        }
        glEnd();
    }
    // если задана текстура
    if(texEnabled)
        glEnable(GL_TEXTURE_2D);// включить текстуры
    else
        glDisable(GL_TEXTURE_2D);// иначе отключить текстуры
}

void Cmodel::reloadTextures()
{
    for(int i = 0; i < m_numMaterials; i++)
        if(strlen(m_pMaterials[i].m_pTextureFilename) > 0)
            m_pMaterials[i].m_texture = LoadGLTextures(m_pMaterials[i].m_pTextureFilename);
        else
            m_pMaterials[i].m_texture = 0;
}
