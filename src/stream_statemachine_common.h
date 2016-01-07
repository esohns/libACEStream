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

#ifndef STREAM_STATEMACHINE_COMMON_H
#define STREAM_STATEMACHINE_COMMON_H

#include "common_istatemachine.h"

enum Stream_StateMachine_ControlState
{
  STREAM_STATE_INVALID = -1,
  STREAM_STATE_INITIALIZED = 0,
  STREAM_STATE_RUNNING,
  STREAM_STATE_PAUSED,
  STREAM_STATE_STOPPED,
  STREAM_STATE_FINISHED,
  /////////////////////////////////////
  STREAM_STATE_MAX
};

template <typename StateType>
class Stream_StateMachine_IControl_T
 : virtual public Common_IStateMachine_T<Stream_StateMachine_ControlState>
{
 public:
  virtual ~Stream_StateMachine_IControl_T () {}

  virtual void finished () = 0;
};

// convenience types
typedef Stream_StateMachine_ControlState Stream_StateType_t;

#endif