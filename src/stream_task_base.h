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

#ifndef STREAM_TASK_BASE_H
#define STREAM_TASK_BASE_H

#include "ace/Global_Macros.h"
#include "ace/Synch_Traits.h"

#include "common_iget.h"
#include "common_ilock.h"

#include "common_task_base.h"

#include "stream_imodule.h"
#include "stream_isessionnotify.h"
#include "stream_istreamcontrol.h"
#include "stream_itask.h"
#include "stream_messagequeue.h"

// forward declarations
class ACE_Message_Block;
template <ACE_SYNCH_DECL,
          class TIME_POLICY>
class ACE_Module;
class ACE_Time_Value;
class Stream_IAllocator;

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename LockType, // implements Common_ILock_T/Common_IRecursiveLock_T
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
class Stream_TaskBase_T
 : public Common_TaskBase_T<ACE_SYNCH_USE,
                            TimePolicyType,
                            LockType>
 , public Stream_ITask_T<ControlMessageType,
                         DataMessageType,
                         SessionMessageType>
 , public Stream_IModuleHandler_T<ACE_SYNCH_USE,
                                  TimePolicyType,
                                  ConfigurationType>
 , public Common_IGetP_T<Stream_IStream_T<ACE_SYNCH_USE,
                                          TimePolicyType> >
 , public Common_IGetR_2_T<ConfigurationType>
{
  typedef Common_TaskBase_T<ACE_SYNCH_USE,
                            TimePolicyType,
                            LockType> inherited;

 public:
  // convenient types
  typedef Common_TaskBase_T<ACE_SYNCH_USE,
                            TimePolicyType,
                            LockType> TASK_BASE_T;
  typedef Stream_IStream_T<ACE_SYNCH_USE,
                           TimePolicyType> ISTREAM_T;

  virtual ~Stream_TaskBase_T ();

//  using inherited::finished;

  // implement (part of) Stream_ITaskBase_T
  // *NOTE*: these are just default (essentially NOP) implementations
  inline virtual void handleControlMessage (ControlMessageType&) {}
  inline virtual void handleDataMessage (DataMessageType*&, bool&) {}
  virtual void handleSessionMessage (SessionMessageType*&, // session message handle
                                     bool&);               // return value: pass this message downstream ?
  virtual void handleProcessingError (const ACE_Message_Block* const); // message handle

  // implement Stream_IModuleHandler_T
  virtual bool initialize (const ConfigurationType&,
                           Stream_IAllocator* = NULL);
  inline virtual bool postClone (ACE_Module<ACE_SYNCH_USE, TimePolicyType>*, bool = false) { return true; }

  // implement Common_IGet_T
  virtual const ISTREAM_T* const getP () const;
  inline virtual const ConfigurationType& getR_2 () const { ACE_ASSERT (configuration_);  return *configuration_; }

//  // implement Common_IDumpState
//  inline virtual void dump_state () const {};

 protected:
  // convenient types
  typedef ACE_Stream<ACE_SYNCH_USE,
                     TimePolicyType> STREAM_T;
  typedef Common_TaskBase_T<ACE_SYNCH_USE,
                            TimePolicyType,
                            LockType> COMMON_TASK_BASE_T;
  typedef Common_IGetR_T<STREAM_T> IGET_T;
  typedef Stream_MessageQueue_T<ACE_SYNCH_USE,
                                TimePolicyType,
                                SessionMessageType> MESSAGE_QUEUE_T;

  Stream_TaskBase_T (ISTREAM_T* = NULL,        // stream handle
                     MESSAGE_QUEUE_T* = NULL); // queue handle

  // helper methods
  DataMessageType* allocateMessage (unsigned int); // (requested) size
  // standard message handling (to be used by both asynch/synch derivates)
  virtual void handleMessage (ACE_Message_Block*, // message handle
                              bool&);             // return value: stop processing ?

  // convenience methods to send (session-specific) notifications downstream
  // *NOTE*: these invoke put(), so the messages are processed by 'this' module
  //         as well
  bool putControlMessage (SessionControlType, // control type
                          bool = false);      // send upstream ? : downstream
  // *NOTE*: "fire-and-forget" the second argument
  bool putSessionMessage (SessionEventType,                      // session message type
                          typename SessionMessageType::DATA_T*&, // session data container
                          UserDataType* = NULL);                 // user data handle

  // default implementation to handle user messages
  virtual void handleUserMessage (ACE_Message_Block*, // control message
                                  bool&,              // return value: stop processing ?
                                  bool&);             // return value: pass message downstream ?

  virtual void notify (SessionEventType); // session event

  // *TODO*: make this 'private' and use 'friend' access
  bool                                 aggregate_; // support multiple initializations ?
  Stream_IAllocator*                   allocator_;
  // *TODO*: remove ASAP
  ConfigurationType*                   configuration_;
  bool                                 isInitialized_;
  unsigned int                         linked_;

  typename SessionMessageType::DATA_T* sessionData_;

 private:
  // convenient types
  typedef Stream_TaskBase_T<ACE_SYNCH_USE,
                            TimePolicyType,
                            LockType,
                            ConfigurationType,
                            ControlMessageType,
                            DataMessageType,
                            SessionMessageType,
                            SessionIdType,
                            SessionControlType,
                            SessionEventType,
                            UserDataType> OWN_TYPE_T;
  typedef Stream_ISessionNotify_T<SessionIdType,
                                  typename SessionMessageType::DATA_T::DATA_T,
                                  SessionEventType> INOTIFY_T;

  ACE_UNIMPLEMENTED_FUNC (Stream_TaskBase_T ())
  ACE_UNIMPLEMENTED_FUNC (Stream_TaskBase_T (const Stream_TaskBase_T&))
  ACE_UNIMPLEMENTED_FUNC (Stream_TaskBase_T& operator= (const Stream_TaskBase_T&))

  // override/hide ACE_Task_Base members
  //virtual void next (typename inherited::TASK_T*); // downstream task handle
  inline virtual int put (ACE_Message_Block* messageBlock_in, ACE_Time_Value* timeValue_in) { return inherited::put_next (messageBlock_in, timeValue_in); }

  bool                                 freeSessionData_;
  // *NOTE*: these apply to 'downstream', iff linked, only
  // *TODO*: move all of this to Stream_HeadTask_T ASAP
  typename SessionMessageType::DATA_T* sessionData_2; // backup 'downstream' session data
  ACE_SYNCH_MUTEX*                     sessionDataLock_; // backup 'upstream' lock
};

// include template definition
#include "stream_task_base.inl"

#endif
