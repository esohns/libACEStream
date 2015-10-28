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

#include <string>

#include "ace/Stream.h"
#include "ace/Synch_Traits.h"

#include "common_icontrol.h"
//#include "common_iget.h"
#include "common_time_common.h"

// forward declarations
typedef ACE_Stream<ACE_MT_SYNCH,
                   Common_TimePolicy_t> Stream_Base_t;

template <typename StatusType,
          typename StateType>
class Stream_IStreamControl_T
 : public Common_IControl
// , public Common_IGet_T<StateType>
{
 public:
  inline virtual ~Stream_IStreamControl_T () {};

  // *NOTE*: this flushes the pipeline, dropping any (session-)data
  // *TODO*: it may be better to remove this and use stop/waitForCompletion only
  virtual void flush (bool = false) = 0; // flush upstream (if any) ?
  virtual void pause () = 0;
  virtual void rewind () = 0;
  virtual StatusType status () const = 0;
  virtual void waitForCompletion (bool = true,       // wait for any worker thread(s) ?
                                  bool = false) = 0; // wait for upstream (if any) ?
  virtual void waitForIdleState () const = 0;

  virtual std::string name () const = 0;
  virtual const StateType& state () const = 0;

  // *NOTE*: cannot currently reach ACE_Stream::linked_us_ from child classes
  //         --> use this API to set/retrieve upstream (if any)
  virtual void upStream (Stream_Base_t*) = 0;
  virtual Stream_Base_t* upStream () const = 0;
};

#endif
