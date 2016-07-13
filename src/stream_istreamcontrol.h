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

#include "ace/Module.h"
#include "ace/Stream.h"
#include "ace/Synch_Traits.h"

#include "common_icontrol.h"
//#include "common_iget.h"
#include "common_time_common.h"

#include "stream_common.h"

// forward declarations
typedef ACE_Module<ACE_MT_SYNCH,
                   Common_TimePolicy_t> Stream_Module_t;
typedef ACE_Stream<ACE_MT_SYNCH,
                   Common_TimePolicy_t> Stream_Base_t;

class Stream_IStreamControlBase
 : public Common_IControl
{
 public:
  inline virtual ~Stream_IStreamControlBase () {};

  virtual bool load (Stream_ModuleList_t&) = 0; // return value: module list

  // *NOTE*: flush the pipeline, releasing any data
  // *NOTE*: session messages are not flushed, if all modules implement
  //         Stream_IMessageQueue
  // *TODO*: this last bit shouldn't be necessary
  virtual void flush (bool = true,       // flush inbound data ?
                      bool = false) = 0; // flush upstream (if any) ?
  virtual void pause () = 0;
  virtual void rewind () = 0;
  // *NOTE*: wait for all data queues to drain
  virtual void waitForCompletion (bool = true,       // wait for any worker thread(s) ?
                                  bool = false) = 0; // wait for upstream (if any) ?
  //// *NOTE*: wait for all worker threads to join
  //virtual void waitForIdleState (bool = false) const = 0; // wait for upstream (if any) ?

  virtual const Stream_Module_t* find (const std::string&) const = 0; // module name
  virtual std::string name () const = 0;

  // *NOTE*: cannot currently reach ACE_Stream::linked_us_ from child classes
  //         --> use this API to set/retrieve upstream (if any)
  virtual void upStream (Stream_Base_t*) = 0;
  virtual Stream_Base_t* upStream () const = 0;
};

template <typename ControlType,
          typename NotificationType,
          typename StatusType,
          typename StateType>
class Stream_IStreamControl_T
 : public Stream_IStreamControlBase
// , public Common_IGet_T<StateType>
{
 public:
  inline virtual ~Stream_IStreamControl_T () {};

  // *NOTE*: enqeues a control message
  virtual void control (ControlType,       // control type
                        bool = false) = 0; // forward upstream ?
  // *NOTE*: enqeues a session message
  virtual void notify (NotificationType,  // session message type
                       bool = false) = 0; // forward upstream ?

  virtual const StateType& state () const = 0;

  virtual StatusType status () const = 0;
};

#endif
