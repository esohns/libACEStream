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

#ifndef STREAM_TASK_BASE_ASYNCH_H
#define STREAM_TASK_BASE_ASYNCH_H

#include "ace/Global_Macros.h"
#include "ace/Synch_Traits.h"

#include "stream_task_base.h"

// forward declarations
class ACE_Message_Block;
class ACE_Time_Value;

// *IMPORTANT NOTE*: the message queue needs to be synchronized so that shutdown
//                   can be asynchronous
template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          ////////////////////////////////
          typename ConfigurationType,
          ////////////////////////////////
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType,
          ////////////////////////////////
          typename SessionIdType,
          typename SessionControlType,
          typename SessionEventType,
          ////////////////////////////////
          typename UserDataType>
class Stream_TaskBaseAsynch_T
// *TODO*: figure out whether it is possible to use ACE_NULL_SYNCH in this case
 : public Stream_TaskBase_T<ACE_SYNCH_USE,
                            TimePolicyType,
                            ConfigurationType,
                            ControlMessageType,
                            DataMessageType,
                            SessionMessageType,
                            SessionIdType,
                            SessionControlType,
                            SessionEventType,
                            UserDataType>
{
  typedef Stream_TaskBase_T<ACE_SYNCH_USE,
                            TimePolicyType,
                            ConfigurationType,
                            ControlMessageType,
                            DataMessageType,
                            SessionMessageType,
                            SessionIdType,
                            SessionControlType,
                            SessionEventType,
                            UserDataType> inherited;

 public:
  inline virtual ~Stream_TaskBaseAsynch_T () {};

  // override some ACE_Task_Base members
  virtual int open (void* = NULL);
  virtual int close (u_long = 0);
  virtual int module_closed (void);

  virtual int put (ACE_Message_Block*,
                   ACE_Time_Value* = NULL);

  virtual int svc (void);

  // implement (part of) Common_ITask_T
  virtual void waitForIdleState () const;

  // implement (part of) Stream_ITaskBase
  virtual void handleSessionMessage (SessionMessageType*&, // session message handle
                                     bool&);               // return value: pass message downstream ?

 protected:
  // convenient types
  typedef Stream_TaskBase_T<ACE_SYNCH_USE,
                            TimePolicyType,
                            ConfigurationType,
                            ControlMessageType,
                            DataMessageType,
                            SessionMessageType,
                            SessionIdType,
                            SessionControlType,
                            SessionEventType,
                            UserDataType> TASK_BASE_T;

  Stream_TaskBaseAsynch_T (typename TASK_BASE_T::ISTREAM_T* = NULL); // stream handle

 private:
  ACE_UNIMPLEMENTED_FUNC (Stream_TaskBaseAsynch_T ())
  ACE_UNIMPLEMENTED_FUNC (Stream_TaskBaseAsynch_T (const Stream_TaskBaseAsynch_T&))
  ACE_UNIMPLEMENTED_FUNC (Stream_TaskBaseAsynch_T& operator= (const Stream_TaskBaseAsynch_T&))
};

// include template definition
#include "stream_task_base_asynch.inl"

#endif
