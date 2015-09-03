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

#ifndef STREAM_ISTREAMCONTROL_H
#define STREAM_ISTREAMCONTROL_H

#include "common_icontrol.h"
//#include "common_iget.h"

template <typename StatusType,
          typename StateType>
class Stream_IStreamControl_T
 : public Common_IControl
// , public Common_IGet_T<StateType>
{
 public:
  inline virtual ~Stream_IStreamControl_T () {};

  virtual void flush () = 0;
  virtual void pause () = 0;
  virtual void rewind () = 0;
  virtual const StatusType& status () const = 0;
  virtual void waitForCompletion () = 0;

  virtual const StateType& state () const = 0;
};

#endif
