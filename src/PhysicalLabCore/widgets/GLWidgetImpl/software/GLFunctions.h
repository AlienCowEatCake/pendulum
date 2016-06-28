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

#if !defined(PHYSICALLABCORE_GLFUNCTIONS_H_INCLUDED)
#define PHYSICALLABCORE_GLFUNCTIONS_H_INCLUDED

#include "SWRastWidget.h"

namespace GLImpl {

extern SWRastWidget * g_SWContext;

inline void glLightfv(GLenum light, GLenum pname, const GLfloat * params)   { g_SWContext->glLightfv(light, pname, params); }
inline void glMaterialfv(GLenum face, GLenum pname, const GLfloat * params) { g_SWContext->glMaterialfv(face, pname, params);}
inline void glBindTexture(GLenum target, GLuint texture)                    { g_SWContext->glBindTexture(target, texture); }
inline void glGenTextures(GLsizei n, GLuint * textures)                     { g_SWContext->glGenTextures(n, textures); }
inline void glTexImage2D(GLenum target, GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid * data)
                                                                            { g_SWContext->glTexImage2D(target, level, internalFormat, width, height, border, format, type, data); }
inline void glNormal3fv(const GLfloat * v)                                  { g_SWContext->glNormal3fv(v); }
inline void glTexCoord2f(GLfloat s, GLfloat t)                              { g_SWContext->glTexCoord2f(s, t); }
inline void glVertex3fv(const GLfloat * v)                                  { g_SWContext->glVertex3fv(v); }
inline void glBegin(GLenum mode)                                            { g_SWContext->glBegin(mode); }
inline void glEnd()                                                         { g_SWContext->glEnd(); }
inline void glMatrixMode(GLenum mode)                                       { g_SWContext->glMatrixMode(mode); }
inline void glLoadIdentity()                                                { g_SWContext->glLoadIdentity(); }
inline void glViewport(GLint x, GLint y, GLsizei width, GLsizei height)     { g_SWContext->glViewport(x, y, width, height); }
inline void glEnable(GLenum cap)                                            { g_SWContext->glEnable(cap); }
inline void glDisable(GLenum cap)                                           { g_SWContext->glDisable(cap); }
inline void glScalef(GLfloat x, GLfloat y, GLfloat z)                       { g_SWContext->glScalef(x, y, z); }
inline void glTranslatef(GLfloat x, GLfloat y, GLfloat z)                   { g_SWContext->glTranslatef(x, y, z); }
inline void glRotatef(GLfloat angle, GLfloat x, GLfloat y, GLfloat z)       { g_SWContext->glRotatef(angle, x, y, z); }
inline void glOrtho(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble near_val, GLdouble far_val)
                                                                            { g_SWContext->glOrtho(left, right, bottom, top, near_val, far_val); }
/// @note Заглушки
inline void glShadeModel(GLenum mode)                                       { Q_UNUSED(mode); }
inline void glEnableClientState(GLenum cap)                                 { Q_UNUSED(cap); }
inline GLboolean glIsEnabled(GLenum cap)                                    { Q_UNUSED(cap); return GL_TRUE; }
inline void glClear(GLbitfield mask)                                        { Q_UNUSED(mask); }
inline void glTexParameteri(GLenum target, GLenum pname, GLint param)       { Q_UNUSED(target); Q_UNUSED(pname); Q_UNUSED(param); }
inline void glMaterialf(GLenum face, GLenum pname, GLfloat param)           { Q_UNUSED(face); Q_UNUSED(pname); Q_UNUSED(param); }

} // GLImpl

#endif // PHYSICALLABCORE_GLFUNCTIONS_H_INCLUDED


