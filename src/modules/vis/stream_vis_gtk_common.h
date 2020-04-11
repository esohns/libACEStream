/***************************************************************************
 *   Copyright (C) 2009 by Erik Sohns   *
 *   erik.sohns@web.de   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef STREAM_VISUALIZATION_GTK_COMMON_H
#define STREAM_VISUALIZATION_GTK_COMMON_H

#if defined (GTKGL_SUPPORT)
#include <deque>
#endif // GTKGL_SUPPORT

#include "gtk/gtk.h"

#if defined (GTKGL_SUPPORT)
enum Stream_Visualization_OpenGL_InstructionType
{
  STREAM_VISUALIZATION_OPENGL_INSTRUCTION_SET_COLOR_BG,
  STREAM_VISUALIZATION_OPENGL_INSTRUCTION_SET_COLOR_FG,
  ////////////////////////////////////////
  STREAM_VISUALIZATION_OPENGL_INSTRUCTION_MAX,
  STREAM_VISUALIZATION_OPENGL_INSTRUCTION_INVALID
};
struct Stream_Visualization_OpenGL_Instruction
{
  Stream_Visualization_OpenGL_Instruction ()
   : type (STREAM_VISUALIZATION_OPENGL_INSTRUCTION_INVALID)
  {}

  enum Stream_Visualization_OpenGL_InstructionType type;
  union {
#if GTK_CHECK_VERSION(3,0,0)
    struct _GdkRGBA                                color;
#else
    GdkColor                                       color;
#endif // GTK_CHECK_VERSION(3,0,0)
  };
};
typedef std::deque<struct Stream_Visualization_OpenGL_Instruction> Stream_Visualization_OpenGL_Instructions_t;
typedef Stream_Visualization_OpenGL_Instructions_t::const_iterator Stream_Visualization_OpenGL_InstructionsIterator_t;
#endif // GTKGL_SUPPORT

#endif
