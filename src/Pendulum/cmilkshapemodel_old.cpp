#include "cmodel.h"

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
    generateNormales();
}
