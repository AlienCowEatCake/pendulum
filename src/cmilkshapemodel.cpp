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

#include <QFile>
#include <QByteArray>
#include "cmodel.h"

using namespace std;
//конструктор модели
CMilkshapeModel::CMilkshapeModel()
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
CMilkshapeModel::~CMilkshapeModel()
{
    for(int i = 0; i < m_numMeshes; i++)
        delete [] m_pMeshes[i].m_pTriangleIndices;
    for(int i = 0; i < m_numMaterials; i++)
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

// MilkShape 3D структуры

// упаковка структур (отключение выравнивания областей памяти)
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

// определение типов данных
typedef unsigned char byte;
typedef unsigned short word;

// заголовок файла ms3d
struct MS3DHeader
{
    char m_ID[10];
    int m_version;
} PACK_STRUCT;

// данные о вершинах
struct MS3DVertex
{
    byte m_flags;
    float m_vertex[3];
    char m_boneID;
    byte m_refCount;
} PACK_STRUCT;

// данные о треугольниках
struct MS3DTriangle
{
    word m_flags;
    word m_vertexIndices[3];
    float m_vertexNormals[3][3];
    float m_s[3], m_t[3];
    byte m_smoothingGroup;
    byte m_groupIndex;
} PACK_STRUCT;

// данные о материалах
struct MS3DMaterial
{
    char m_name[32];
    float m_ambient[4];
    float m_diffuse[4];
    float m_specular[4];
    float m_emissive[4];
    float m_shininess;
    float m_transparency;
    byte m_mode;
    char m_texture[128];
    char m_alphamap[128];
} PACK_STRUCT;

// данные об анимации
struct MS3DJoint
{
    byte m_flags;
    char m_name[32];
    char m_parentName[32];
    float m_rotation[3];
    float m_translation[3];
    word m_numRotationKeyframes;
    word m_numTranslationKeyframes;
} PACK_STRUCT;

struct MS3DKeyframe
{
    float m_time;
    float m_parameter[3];
} PACK_STRUCT;

// конец упаковки структур
#if defined(_MSC_VER)
#	pragma pack(pop, packing)
#elif defined(__GNUC__) && __GNUC__ >= 4
#	pragma pack(pop)
#endif
#undef PACK_STRUCT

// загрузка модели
bool CMilkshapeModel::loadModelData(const char *filename)
{
    QFile inputFile(filename);
    inputFile.open(QFile::ReadOnly);
    inputFile.seek(0);
    QByteArray pBuffer = inputFile.readAll();
    inputFile.close();

    const char *pPtr = pBuffer;
    MS3DHeader *pHeader = (MS3DHeader*)pPtr;
    pPtr += sizeof(MS3DHeader);

    if(strncmp(pHeader->m_ID, "MS3D000000", 10) != 0)
        return false; // неправильный заголовок файла
    // загрузка вершин
    int nVertices = *(word*)pPtr;
    m_numVertices = nVertices;
    m_pVertices = new Vertex[nVertices];
    pPtr += sizeof(word);

    int i;
    for(i = 0; i < nVertices; i++)
    {
        MS3DVertex *pVertex = (MS3DVertex*)pPtr;
        m_pVertices[i].m_boneID = pVertex->m_boneID;
        memcpy(m_pVertices[i].m_location, pVertex->m_vertex, sizeof(float)*3);
        pPtr += sizeof(MS3DVertex);
    }
    // загрузка треугольников
    int nTriangles = *(word*)pPtr;
    m_numTriangles = nTriangles;
    m_pTriangles = new Triangle[nTriangles];
    pPtr += sizeof(word);

    for(i = 0; i < nTriangles; i++)
    {
        MS3DTriangle *pTriangle = (MS3DTriangle*)pPtr;
        int vertexIndices[3] = {pTriangle->m_vertexIndices[0], pTriangle->m_vertexIndices[1], pTriangle->m_vertexIndices[2]};
        float t[3] = {1.0f-pTriangle->m_t[0], 1.0f-pTriangle->m_t[1], 1.0f-pTriangle->m_t[2]};
        memcpy(m_pTriangles[i].m_vertexNormals, pTriangle->m_vertexNormals, sizeof(float)*3*3);
        memcpy(m_pTriangles[i].m_s, pTriangle->m_s, sizeof(float)*3);
        memcpy(m_pTriangles[i].m_t, t, sizeof(float)*3);
        memcpy(m_pTriangles[i].m_vertexIndices, vertexIndices, sizeof(int)*3);
        pPtr += sizeof(MS3DTriangle);
    }
    // загрузка сетки
    int nGroups = *(word*)pPtr;
    m_numMeshes = nGroups;
    m_pMeshes = new Mesh[nGroups];
    pPtr += sizeof(word);
    for(i = 0; i < nGroups; i++)
    {
        pPtr += sizeof(byte);
        pPtr += 32;

        word nTriangles = *(word*)pPtr;
        pPtr += sizeof(word);
        int *pTriangleIndices = new int[nTriangles];
        for(int j = 0; j < nTriangles; j++)
        {
            pTriangleIndices[j] = *(word*)pPtr;
            pPtr += sizeof(word);
        }

        char materialIndex = *(char*)pPtr;
        pPtr += sizeof(char);

        m_pMeshes[i].m_materialIndex = materialIndex;
        m_pMeshes[i].m_numTriangles = nTriangles;
        m_pMeshes[i].m_pTriangleIndices = pTriangleIndices;
    }
    // загрузка материалов
    int nMaterials = *(word*)pPtr;
    m_numMaterials = nMaterials;
    m_pMaterials = new Material[nMaterials];
    pPtr += sizeof(word);
    for(i = 0; i < nMaterials; i++)
    {
        MS3DMaterial *pMaterial = (MS3DMaterial*)pPtr;
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

// просчет нормалей
void CMilkshapeModel::normal_gen()
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
        float nn = sqrt(nx * nx + ny * ny + nz * nz);
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
        count_x = count_x / (float)counter;
        count_y = count_y / (float)counter;
        count_z = count_z / (float)counter;
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

//=============================================================================================================================
// класс пружинка
// конструктор модели
CModelSpring::CModelSpring()
{
    m_numMeshes = 0;
    m_pMeshes = NULL;
    m_numMaterials = 0;
    m_pMaterials = NULL;
    m_numTriangles = 0;
    m_pTriangles = NULL;
    m_numVertices = 0;
    m_pVertices = NULL;
    num_stacks = 257;
    num_slices = 6;
    offset_start = -0.0014f;
    offset_end = -0.0014f;
    num_turns = 10;
    radius_turn = 0.0989f;
    radius_spring = 0.0065f;
    def_size = 1.0f;
    length_prev = 0.0f;
    epsilon = 0.0001f;
    a_index = new int [num_stacks * num_slices];
    for(int i = 0; i < num_stacks * num_slices; i++)
        a_index[i] = -1;
    pi_x2 = 6.283185307179586476925286766559f;
    pi = 3.1415926535897932384626433832795f;
    pi_d2 = 1.5707963267948966192313216916398f;
}

// деструктор модели
CModelSpring::~CModelSpring()
{
    for(int i = 0; i < m_numMeshes; i++)
        delete [] m_pMeshes[i].m_pTriangleIndices;
    for(int i = 0; i < m_numMaterials; i++)
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

    if(a_index)
    {
        delete [] a_index;
        a_index = NULL;
    }
}

// изменение размеров модели
void CModelSpring::resize(float length)
{
    length -= (offset_start + offset_end);
    // длина с корректировками
    if(fabs(length - length_prev) > epsilon)
    {
        int index = 0;
        // координаты скелета
        float z_skel = offset_start;
        float x_skel, y_skel;
        // количество точек скелета в витке
        int num_oneturn = num_stacks / num_turns;
        // шаг скелета
        float z_skel_step = length / num_turns / num_oneturn;
        float fi_skel_step = pi_x2 / num_oneturn;
        // обход по виткам
        for(int i = 0; i < num_turns; i++)
        {
            // обход по витку
            for(float fi_skel = 0.0; fi_skel < pi_x2 - fi_skel_step / 2.0; fi_skel += fi_skel_step)
            {
                float cos_fi_skel(cos(fi_skel));
                float sin_fi_skel(sin(fi_skel));
                x_skel = radius_turn * cos_fi_skel;
                y_skel = radius_turn * sin_fi_skel;
                // матрица перехода
                float a[3][3];
                float cos_fi_2 = cos(fi_skel + pi_d2);
                float sin_fi_2 = sin(fi_skel + pi_d2);
                float det_a = cos_fi_skel * sin_fi_2 - cos_fi_2 * sin_fi_skel;
                a[0][0] = sin_fi_2 / det_a;
                a[0][1] = cos_fi_2 / det_a;
                a[0][2] = a[1][2] = a[2][0] = a[2][1] = 0.0;
                a[1][0] = sin_fi_skel / det_a;
                a[1][1] = cos_fi_skel / det_a;
                a[2][2] = 1.0;
                // построение элементарных окружностей
                float fi = 0.0;
                float fi_step = pi_x2 / num_slices;
                for(int j = 0; j < num_slices; j++)
                {
                    // координаты элементарного круга
                    float x_ = radius_spring * cos(fi);
                    float y_ = 0.0;
                    float z_ = radius_spring * sin(fi);
                    // сдвинутые и повернутые координаты
                    m_pVertices[a_index[index]].m_location[0] = x_ * a[0][0] + y_ * a[0][1] + z_ * a[0][2] - x_skel;
                    m_pVertices[a_index[index]].m_location[1] = x_ * a[2][0] + y_ * a[2][1] + z_ * a[2][2] - z_skel;
                    m_pVertices[a_index[index]].m_location[2] = x_ * a[1][0] + y_ * a[1][1] + z_ * a[1][2] - y_skel;
                    index++;
                    fi += fi_step;
                }
                z_skel += z_skel_step;
            }
        }
        // последний сектор
        while(index < m_numVertices)
        {
            x_skel = radius_turn;
            y_skel = 0.0;
            // построение элементарных окружностей
            float fi = 0.0;
            float fi_step = pi_x2 / num_slices;
            for(int j = 0; j < num_slices && index < m_numVertices; j++)
            {
                // координаты элементарного круга
                float x_ = radius_spring * cos(fi);
                float y_ = 0.0;
                float z_ = radius_spring * sin(fi);
                // сдвинутые и повернутые координаты
                m_pVertices[a_index[index]].m_location[0] = x_ - x_skel;
                m_pVertices[a_index[index]].m_location[1] = z_ - z_skel;
                m_pVertices[a_index[index]].m_location[2] = y_ - y_skel;
                index++;
                fi += fi_step;
            }
        }
        length_prev = length;
    }
}

// индексация координат модели
void CModelSpring::indexate()
{
    float * a_y_tmp;
    a_y_tmp = new float [num_stacks];
    for(int i = 0; i < num_stacks; i++)
        a_y_tmp[i] = 0.0;
    // разбиение на группы
    for(int i = 0, k = 0; i < m_numVertices; i++)
    {
        int flag = 0;
        for(int j = 0; j < k && flag == 0; j++)
        {
            if(a_y_tmp[j] == m_pVertices[i].m_location[1])
            {
                flag = 1;
            }
        }
        if(flag == 0)
        {
            a_y_tmp[k] = m_pVertices[i].m_location[1];
            k++;
        }
    }
    // разбиение по группам
    for(int i = 0; i < m_numVertices; i++)
    {
        int index = -1;
        for(int j = 0; j < num_stacks && index == -1; j++)
        {
            if(a_y_tmp[j] == m_pVertices[i].m_location[1])
            {
                index = j;
            }
        }
        for(int j = index * num_slices; j < index * num_slices + num_slices; j++)
        {
            if(a_index[j] == -1)
            {
                a_index[j] = i;
                j = index * num_slices + num_slices;
            }
        }
    }
    delete [] a_y_tmp;
    a_y_tmp = NULL;
}

void CModelSpring::init()
{
    indexate();
    resize(def_size);
    normal_gen();
}
