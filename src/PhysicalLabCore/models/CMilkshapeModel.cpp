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

#include "CMilkshapeModel.h"

#include <QFile>
#include <QByteArray>
#include <cmath>

namespace {

/// @note MilkShape 3D хранит данные в упакованном бинарном формате, поэтому все структуры должны быть без выравнивания
#if defined(_MSC_VER)
#	pragma pack(push, packing)
#	pragma pack(1)
#	define PACK_STRUCT
#elif defined(__GNUC__) && __GNUC__ >= 4
#	pragma pack(push, 1)
#	define PACK_STRUCT
#elif defined(__GNUC__)
#	define PACK_STRUCT	__attribute__((packed))
#else
#	error you must byte-align these structures with the appropriate compiler directives
#endif

typedef unsigned char MS3DByte;
typedef unsigned short MS3DWord;

/// @brief Заголовок файла ms3d
struct MS3DHeader
{
    char m_ID[10];
    int m_version;
} PACK_STRUCT;

/// @brief Данные о вершинах
struct MS3DVertex
{
    MS3DByte m_flags;
    float m_vertex[3];
    char m_boneID;
    MS3DByte m_refCount;
} PACK_STRUCT;

/// @brief Данные о треугольниках
struct MS3DTriangle
{
    MS3DWord m_flags;
    MS3DWord m_vertexIndices[3];
    float m_vertexNormals[3][3];
    float m_s[3], m_t[3];
    MS3DByte m_smoothingGroup;
    MS3DByte m_groupIndex;
} PACK_STRUCT;

/// @brief Данные о материалах
struct MS3DMaterial
{
    char m_name[32];
    float m_ambient[4];
    float m_diffuse[4];
    float m_specular[4];
    float m_emissive[4];
    float m_shininess;
    float m_transparency;
    MS3DByte m_mode;
    char m_texture[128];
    char m_alphamap[128];
} PACK_STRUCT;

/// @brief Данные об анимации (Joint)
struct MS3DJoint
{
    MS3DByte m_flags;
    char m_name[32];
    char m_parentName[32];
    float m_rotation[3];
    float m_translation[3];
    MS3DWord m_numRotationKeyframes;
    MS3DWord m_numTranslationKeyframes;
} PACK_STRUCT;

/// @brief Данные об анимации (Keyframe)
struct MS3DKeyframe
{
    float m_time;
    float m_parameter[3];
} PACK_STRUCT;

#if defined(_MSC_VER)
#	pragma pack(pop, packing)
#elif defined(__GNUC__) && __GNUC__ >= 4
#	pragma pack(pop)
#endif
#undef PACK_STRUCT

} // namespace

CMilkshapeModel::CMilkshapeModel()
{}

CMilkshapeModel::~CMilkshapeModel()
{}

/// @brief loadModelData - загрузка модели из файла
/// @param[in] filename - имя файла
/// @return true, если загрузка прошла успешно, false иначе
bool CMilkshapeModel::loadModelData(const QString & filename)
{
    QFile inputFile(filename);
    inputFile.open(QFile::ReadOnly);
    inputFile.seek(0);
    QByteArray pBuffer = inputFile.readAll();
    inputFile.close();

    const char *pPtr = pBuffer;
    const MS3DHeader *pHeader = reinterpret_cast<const MS3DHeader*>(pPtr);
    pPtr += sizeof(MS3DHeader);

    if(strncmp(pHeader->m_ID, "MS3D000000", 10) != 0)
        return false; // неправильный заголовок файла
    // загрузка вершин
    int nVertices = *reinterpret_cast<const MS3DWord*>(pPtr);
    m_numVertices = nVertices;
    m_pVertices = new Vertex[nVertices];
    pPtr += sizeof(MS3DWord);

    for(int i = 0; i < nVertices; i++)
    {
        const MS3DVertex *pVertex = reinterpret_cast<const MS3DVertex*>(pPtr);
        m_pVertices[i].m_boneID = pVertex->m_boneID;
        memcpy(m_pVertices[i].m_location, pVertex->m_vertex, sizeof(float)*3);
        pPtr += sizeof(MS3DVertex);
    }
    // загрузка треугольников
    int nTriangles = *reinterpret_cast<const MS3DWord*>(pPtr);
    m_numTriangles = nTriangles;
    m_pTriangles = new Triangle[nTriangles];
    pPtr += sizeof(MS3DWord);

    for(int i = 0; i < nTriangles; i++)
    {
        const MS3DTriangle *pTriangle = reinterpret_cast<const MS3DTriangle*>(pPtr);
        int vertexIndices[3] = {pTriangle->m_vertexIndices[0], pTriangle->m_vertexIndices[1], pTriangle->m_vertexIndices[2]};
        float t[3] = {1.0f-pTriangle->m_t[0], 1.0f-pTriangle->m_t[1], 1.0f-pTriangle->m_t[2]};
        memcpy(m_pTriangles[i].m_vertexNormals, pTriangle->m_vertexNormals, sizeof(float)*3*3);
        memcpy(m_pTriangles[i].m_s, pTriangle->m_s, sizeof(float)*3);
        memcpy(m_pTriangles[i].m_t, t, sizeof(float)*3);
        memcpy(m_pTriangles[i].m_vertexIndices, vertexIndices, sizeof(int)*3);
        pPtr += sizeof(MS3DTriangle);
    }
    // загрузка сетки
    int nGroups = *reinterpret_cast<const MS3DWord*>(pPtr);
    m_numMeshes = nGroups;
    m_pMeshes = new Mesh[nGroups];
    pPtr += sizeof(MS3DWord);
    for(int i = 0; i < nGroups; i++)
    {
        pPtr += sizeof(MS3DByte);
        pPtr += 32;

        MS3DWord nTriangles = *reinterpret_cast<const MS3DWord*>(pPtr);
        pPtr += sizeof(MS3DWord);
        int *pTriangleIndices = new int[nTriangles];
        for(int j = 0; j < nTriangles; j++)
        {
            pTriangleIndices[j] = *reinterpret_cast<const MS3DWord*>(pPtr);
            pPtr += sizeof(MS3DWord);
        }

        char materialIndex = *reinterpret_cast<const char*>(pPtr);
        pPtr += sizeof(char);

        m_pMeshes[i].m_materialIndex = materialIndex;
        m_pMeshes[i].m_numTriangles = nTriangles;
        m_pMeshes[i].m_pTriangleIndices = pTriangleIndices;
    }
    // загрузка материалов
    int nMaterials = *reinterpret_cast<const MS3DWord*>(pPtr);
    m_numMaterials = nMaterials;
    m_pMaterials = new Material[nMaterials];
    pPtr += sizeof(MS3DWord);
    for(int i = 0; i < nMaterials; i++)
    {
        const MS3DMaterial *pMaterial = reinterpret_cast<const MS3DMaterial*>(pPtr);
        memcpy(m_pMaterials[i].m_ambient, pMaterial->m_ambient, sizeof(float)*4);
        memcpy(m_pMaterials[i].m_diffuse, pMaterial->m_diffuse, sizeof(float)*4);
        memcpy(m_pMaterials[i].m_specular, pMaterial->m_specular, sizeof(float)*4);
        memcpy(m_pMaterials[i].m_emissive, pMaterial->m_emissive, sizeof(float)*4);
        m_pMaterials[i].m_shininess = pMaterial->m_shininess;
        m_pMaterials[i].m_pTextureFilename = new char[7+strlen(pMaterial->m_texture)+1];
        strcpy(m_pMaterials[i].m_pTextureFilename, ":/data/");
        strcpy(m_pMaterials[i].m_pTextureFilename+7, pMaterial->m_texture);
        // При компиляции используются текстуры в PNG-8 для экономии места
        strcpy(m_pMaterials[i].m_pTextureFilename+7+strlen(pMaterial->m_texture)-3, "png");
        pPtr += sizeof(MS3DMaterial);
    }

    reloadTextures();
    pBuffer.clear();
    return true;
}

/// @brief generateNormales - просчет нормалей
void CMilkshapeModel::generateNormales()
{
    for(int i = 0; i < m_numTriangles; i++)
    {
        float ax = m_pVertices[m_pTriangles[i].m_vertexIndices[1]].m_location[0] - m_pVertices[m_pTriangles[i].m_vertexIndices[0]].m_location[0];
        float ay = m_pVertices[m_pTriangles[i].m_vertexIndices[1]].m_location[1] - m_pVertices[m_pTriangles[i].m_vertexIndices[0]].m_location[1];
        float az = m_pVertices[m_pTriangles[i].m_vertexIndices[1]].m_location[2] - m_pVertices[m_pTriangles[i].m_vertexIndices[0]].m_location[2];
        float bx = m_pVertices[m_pTriangles[i].m_vertexIndices[2]].m_location[0] - m_pVertices[m_pTriangles[i].m_vertexIndices[1]].m_location[0];
        float by = m_pVertices[m_pTriangles[i].m_vertexIndices[2]].m_location[1] - m_pVertices[m_pTriangles[i].m_vertexIndices[1]].m_location[1];
        float bz = m_pVertices[m_pTriangles[i].m_vertexIndices[2]].m_location[2] - m_pVertices[m_pTriangles[i].m_vertexIndices[1]].m_location[2];
        float nx = ay * bz - by * az;
        float ny = az * bx - bz * ax;
        float nz = ax * by - bx * ay;
        float nn = std::sqrt(nx * nx + ny * ny + nz * nz);
        for(int j = 0; j < 3; j++)
        {
            m_pTriangles[i].m_vertexNormals[j][0] = nx / nn;
            m_pTriangles[i].m_vertexNormals[j][1] = ny / nn;
            m_pTriangles[i].m_vertexNormals[j][2] = nz / nn;
        }
    }
    // сглаживание нормалей
    for(int i = 0; i < m_numVertices; i++)
    {
        int counter = 0;
        float count_x = 0.0;
        float count_y = 0.0;
        float count_z = 0.0;
        for(int j = 0; j < m_numTriangles; j++)
        {
            for(int k = 0; k < 3; k++)
            {
                if(m_pTriangles[j].m_vertexIndices[k] == i)
                {
                    count_x += m_pTriangles[j].m_vertexNormals[k][0];
                    count_y += m_pTriangles[j].m_vertexNormals[k][1];
                    count_z += m_pTriangles[j].m_vertexNormals[k][2];
                    counter++;
                }
            }
        }
        count_x = count_x / static_cast<float>(counter);
        count_y = count_y / static_cast<float>(counter);
        count_z = count_z / static_cast<float>(counter);
        for(int j = 0; j < m_numTriangles; j++)
        {
            for(int k = 0; k < 3; k++)
            {
                if(m_pTriangles[j].m_vertexIndices[k] == i)
                {
                    m_pTriangles[j].m_vertexNormals[k][0] = count_x;
                    m_pTriangles[j].m_vertexNormals[k][1] = count_y;
                    m_pTriangles[j].m_vertexNormals[k][2] = count_z;
                }
            }
        }
    }
}
