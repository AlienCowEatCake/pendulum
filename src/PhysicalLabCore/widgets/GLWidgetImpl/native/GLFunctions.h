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

#if !defined (PHYSICALLABCORE_GLFUNCTIONS_H_INCLUDED)
#define PHYSICALLABCORE_GLFUNCTIONS_H_INCLUDED

#include <QtOpenGL>

namespace GLImpl {

inline void glLightfv(GLenum light, GLenum pname, const GLfloat * params)   { ::glLightfv(light, pname, params); }
inline void glMaterialfv(GLenum face, GLenum pname, const GLfloat * params) { ::glMaterialfv(face, pname, params);}
inline void glBindTexture(GLenum target, GLuint texture)                    { ::glBindTexture(target, texture); }
inline void glGenTextures(GLsizei n, GLuint * textures)                     { ::glGenTextures(n, textures); }
inline void glTexImage2D(GLenum target, GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid * data)
                                                                            { ::glTexImage2D(target, level, internalFormat, width, height, border, format, type, data); }
inline void glNormal3fv(const GLfloat * v)                                  { ::glNormal3fv(v); }
inline void glTexCoord2f(GLfloat s, GLfloat t)                              { ::glTexCoord2f(s, t); }
inline void glVertex3fv(const GLfloat * v)                                  { ::glVertex3fv(v); }
inline void glBegin(GLenum mode)                                            { ::glBegin(mode); }
inline void glEnd()                                                         { ::glEnd(); }
inline void glMatrixMode(GLenum mode)                                       { ::glMatrixMode(mode); }
inline void glLoadIdentity()                                                { ::glLoadIdentity(); }
inline void glViewport(GLint x, GLint y, GLsizei width, GLsizei height)     { ::glViewport(x, y, width, height); }
inline void glEnable(GLenum cap)                                            { ::glEnable(cap); }
inline void glDisable(GLenum cap)                                           { ::glDisable(cap); }
inline void glScalef(GLfloat x, GLfloat y, GLfloat z)                       { ::glScalef(x, y, z); }
inline void glTranslatef(GLfloat x, GLfloat y, GLfloat z)                   { ::glTranslatef(x, y, z); }
inline void glRotatef(GLfloat angle, GLfloat x, GLfloat y, GLfloat z)       { ::glRotatef(angle, x, y, z); }
inline void glOrtho(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble near_val, GLdouble far_val)
                                                                            { ::glOrtho(left, right, bottom, top, near_val, far_val); }
inline void glShadeModel(GLenum mode)                                       { ::glShadeModel(mode); }
inline void glEnableClientState(GLenum cap)                                 { ::glEnableClientState(cap); }
inline GLboolean glIsEnabled(GLenum cap)                                    { return ::glIsEnabled(cap); }
inline void glClear(GLbitfield mask)                                        { ::glClear(mask); }
inline void glTexParameteri(GLenum target, GLenum pname, GLint param)       { ::glTexParameteri(target, pname, param); }
inline void glMaterialf(GLenum face, GLenum pname, GLfloat param)           { ::glMaterialf(face, pname, param); }

} // GLImpl

#endif // PHYSICALLABCORE_GLFUNCTIONS_H_INCLUDED

