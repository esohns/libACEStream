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

#ifndef STREAM_MODULE_MISC_COMMON_H
#define STREAM_MODULE_MISC_COMMON_H

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include "guiddef.h"

// *TODO*: move this somewhere else
// {F9F62434-535B-4934-A695-BE8D10A4C699}
DEFINE_GUID (CLSID_ACEStream_Source_Filter,
             0xf9f62434,
             0x535b,
             0x4934,
             0xa6, 0x95,
             0xbe, 0x8d, 0x10, 0xa4, 0xc6, 0x99);
#endif

#endif
