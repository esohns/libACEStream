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

#include <ace/Log_Msg.h>
#include <ace/Message_Block.h>

#include "common_defines.h"
#include "common_timer_manager_common.h"
#include "common_tools.h"

#include "stream_defines.h"
#include "stream_iallocator.h"
#include "stream_macros.h"

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType,
          typename ConfigurationType,
          typename SessionControlType,
          typename SessionEventType,
          typename StreamStateType,
          typename SessionDataType,
          typename SessionDataContainerType,
          typename StatisticContainerType,
          typename UserDataType>
Stream_HeadModuleTaskBase_T<ACE_SYNCH_USE,
                            TimePolicyType,
                            ControlMessageType,
                            DataMessageType,
                            SessionMessageType,
                            ConfigurationType,
                            SessionControlType,
                            SessionEventType,
                            StreamStateType,
                            SessionDataType,
                            SessionDataContainerType,
                            StatisticContainerType,
                            UserDataType>::Stream_HeadModuleTaskBase_T (ACE_SYNCH_MUTEX_T* lock_in,
                                                                        bool autoStart_in,
                                                                        enum Stream_HeadModuleConcurrency concurrency_in,
                                                                        bool generateSessionMessages_in)
 : inherited (lock_in)
 , inherited2 ()
 , concurrency_ (concurrency_in)
 , concurrent_ (true)
 , isInitialized_ (false)
 , sessionEndProcessed_ (false)
 , sessionEndSent_ (false)
 , streamLock_ (NULL)
 , streamState_ (NULL)
 , statisticCollectionHandler_ (ACTION_COLLECT,
                                this,
                                false)
 , timerID_ (-1)
 /////////////////////////////////////////
 , autoStart_ (autoStart_in)
 , generateSessionMessages_ (generateSessionMessages_in)
 , sessionDataLock_ (NULL)
{
  STREAM_TRACE (ACE_TEXT ("Stream_HeadModuleTaskBase_T::Stream_HeadModuleTaskBase_T"));

  inherited2::threadCount_ = STREAM_MODULE_DEFAULT_HEAD_THREADS;

  // set group ID for worker thread(s)
  inherited2::grp_id (STREAM_MODULE_TASK_GROUP_ID);
}

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType,
          typename ConfigurationType,
          typename SessionControlType,
          typename SessionEventType,
          typename StreamStateType,
          typename SessionDataType,
          typename SessionDataContainerType,
          typename StatisticContainerType,
          typename UserDataType>
Stream_HeadModuleTaskBase_T<ACE_SYNCH_USE,
                            TimePolicyType,
                            ControlMessageType,
                            DataMessageType,
                            SessionMessageType,
                            ConfigurationType,
                            SessionControlType,
                            SessionEventType,
                            StreamStateType,
                            SessionDataType,
                            SessionDataContainerType,
                            StatisticContainerType,
                            UserDataType>::~Stream_HeadModuleTaskBase_T ()
{
  STREAM_TRACE (ACE_TEXT ("Stream_HeadModuleTaskBase_T::~Stream_HeadModuleTaskBase_T"));

  int result = -1;

  if (timerID_ != -1)
  {
    const void* act_p = NULL;
    result =
      COMMON_TIMERMANAGER_SINGLETON::instance ()->cancel_timer (timerID_,
                                                                &act_p);
    if (result == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to cancel timer (ID: %d): \"%m\", continuing\n"),
                  timerID_));
    else
      ACE_DEBUG ((LM_WARNING, // <-- should happen in STREAM_END_SESSION
                  ACE_TEXT ("cancelled timer in Stream_HeadModuleTaskBase_T dtor (id was: %d)\n"),
                  timerID_));
  } // end IF
}

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType,
          typename ConfigurationType,
          typename SessionControlType,
          typename SessionEventType,
          typename StreamStateType,
          typename SessionDataType,
          typename SessionDataContainerType,
          typename StatisticContainerType,
          typename UserDataType>
int
Stream_HeadModuleTaskBase_T<ACE_SYNCH_USE,
                            TimePolicyType,
                            ControlMessageType,
                            DataMessageType,
                            SessionMessageType,
                            ConfigurationType,
                            SessionControlType,
                            SessionEventType,
                            StreamStateType,
                            SessionDataType,
                            SessionDataContainerType,
                            StatisticContainerType,
                            UserDataType>::put (ACE_Message_Block* messageBlock_in,
                                                ACE_Time_Value* timeout_in)
{
  STREAM_TRACE (ACE_TEXT ("Stream_HeadModuleTaskBase_T::put"));

  int result = -1;

  // use the queue, if necessary
  switch (concurrency_)
  {
    case STREAM_HEADMODULECONCURRENCY_ACTIVE:
    case STREAM_HEADMODULECONCURRENCY_PASSIVE:
    {
      result = inherited2::putq (messageBlock_in, timeout_in);
      if (result == -1)
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("%s: failed to ACE_Task::putq(): \"%m\", aborting\n"),
                    inherited2::name ()));
      return result;
    }
    default:
      break;
  } // end SWITCH

  // --> process 'in-line'

  // sanity check(s)
  ACE_ASSERT (streamLock_);

  bool release_lock = false;
  if (!concurrent_)
  {
    try {
      release_lock = streamLock_->lock (true);
    } catch (...) {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("%s: caught exception in Stream_ILock_T::lock(true): \"%m\", continuing\n"),
                  inherited2::name ()));
    }
  } // end IF

  bool stop_processing = false;
  inherited2::handleMessage (messageBlock_in,
                             stop_processing);

  // clean up
  if (release_lock)
  {
    try {
      streamLock_->unlock (false);
    } catch (...) {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("%s: caught exception in Stream_ILock_T::unlock(false): \"%m\", continuing\n"),
                  inherited2::name ()));
    }
  } // end IF

  //return (stop_processing ? -1 : 0);
  return 0;
}

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType,
          typename ConfigurationType,
          typename SessionControlType,
          typename SessionEventType,
          typename StreamStateType,
          typename SessionDataType,
          typename SessionDataContainerType,
          typename StatisticContainerType,
          typename UserDataType>
int
Stream_HeadModuleTaskBase_T<ACE_SYNCH_USE,
                            TimePolicyType,
                            ControlMessageType,
                            DataMessageType,
                            SessionMessageType,
                            ConfigurationType,
                            SessionControlType,
                            SessionEventType,
                            StreamStateType,
                            SessionDataType,
                            SessionDataContainerType,
                            StatisticContainerType,
                            UserDataType>::open (void* arg_in)
{
  STREAM_TRACE (ACE_TEXT ("Stream_HeadModuleTaskBase_T::open"));

  int result = -1;

  // step0: initialize this
  ACE_ASSERT (!inherited2::sessionData_);
  inherited2::sessionData_ =
    reinterpret_cast<SessionDataContainerType*> (arg_in);
  if (inherited2::sessionData_)
    inherited2::sessionData_->increase ();

  // step1: initialize the message queue
  // *NOTE*: the first time around, the queue will have been open()ed
  //         from within the default ctor; this sets it into an ACTIVATED state.
  //         The second time around (i.e. the stream has been stopped/started,
  //         the queue will have been deactivate()d in the process, and getq()
  //         (see svc()) would fail (ESHUTDOWN)
  //         --> (re-)activate() the queue
  result = inherited2::queue_.activate ();
  if (result == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Message_Queue::activate(): \"%m\", aborting\n")));
    return -1;
  } // end IF

  // standard usecase: being implicitly invoked through ACE_Stream::push()
  // --> don't do anything, unless auto-starting
  if (autoStart_)
  {
    if (inherited2::mod_)
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("%s: auto-starting...\n"),
                  inherited2::mod_->name ()));
    else
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("auto-starting...\n")));

    try {
      start ();
    } catch (...) {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("%s: caught exception in Stream_IStreamControl_T::start(), aborting\n"),
                  inherited2::mod_->name ()));
      return -1;
    }
  } // end IF

  return 0;
}

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType,
          typename ConfigurationType,
          typename SessionControlType,
          typename SessionEventType,
          typename StreamStateType,
          typename SessionDataType,
          typename SessionDataContainerType,
          typename StatisticContainerType,
          typename UserDataType>
int
Stream_HeadModuleTaskBase_T<ACE_SYNCH_USE,
                            TimePolicyType,
                            ControlMessageType,
                            DataMessageType,
                            SessionMessageType,
                            ConfigurationType,
                            SessionControlType,
                            SessionEventType,
                            StreamStateType,
                            SessionDataType,
                            SessionDataContainerType,
                            StatisticContainerType,
                            UserDataType>::close (u_long arg_in)
{
  STREAM_TRACE (ACE_TEXT ("Stream_HeadModuleTaskBase_T::close"));

  int result = -1;

  // *NOTE*: this method may be invoked by:
  //         - an external thread closing down the active object
  //           --> should NEVER happen as a module !
  //         - worker thread(s) calling this after returning from svc()
  //           --> in this case, this should be a NOP
  // *NOTE*: inherited2::thr_count_ has already been decreased at this stage
  switch (arg_in)
  {
    // called from ACE_Task_Base on clean-up
    case 0:
    {
      if (concurrency_ == STREAM_HEADMODULECONCURRENCY_ACTIVE)
      {
        if (inherited2::mod_)
          ACE_DEBUG ((LM_DEBUG,
                      ACE_TEXT ("%s: worker thread (ID: %t) leaving...\n"),
                      inherited2::mod_->name ()));
        else
          ACE_DEBUG ((LM_DEBUG,
                      ACE_TEXT ("(%s): worker thread (ID: %t) leaving...\n"),
                      ACE_TEXT (inherited2::threadName_.c_str ())));
      } // end IF

      if (inherited2::thr_count_ == 0) // last thread ?
      {
        // *NOTE*: deactivate the queue so it does not accept new data
        result = inherited2::msg_queue_->deactivate ();
        if (result == -1)
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("failed to ACE_Message_Queue::deactivate(): \"%m\", continuing\n")));
        result = inherited2::msg_queue_->flush ();
        if (result == -1)
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("failed to ACE_Message_Queue::flush(): \"%m\", continuing\n")));
        else if (result &&
                 inherited2::mod_)
          ACE_DEBUG ((LM_DEBUG,
                      ACE_TEXT ("%s: flushed %d message(s)...\n"),
                      inherited2::mod_->name (),
                      result));
      } // end IF

      break;
    }
    case 1:
    {
      // *WARNING*: SHOULD NEVER GET HERE
      // --> refer to module_closed () hook
      ACE_ASSERT (false);
      ACE_NOTSUP_RETURN (-1);

      ACE_NOTREACHED (return -1;)
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid argument (was: %u), aborting\n"),
                  arg_in));
      return -1;
    }
  } // end SWITCH

  return 0;
}

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType,
          typename ConfigurationType,
          typename SessionControlType,
          typename SessionEventType,
          typename StreamStateType,
          typename SessionDataType,
          typename SessionDataContainerType,
          typename StatisticContainerType,
          typename UserDataType>
int
Stream_HeadModuleTaskBase_T<ACE_SYNCH_USE,
                            TimePolicyType,
                            ControlMessageType,
                            DataMessageType,
                            SessionMessageType,
                            ConfigurationType,
                            SessionControlType,
                            SessionEventType,
                            StreamStateType,
                            SessionDataType,
                            SessionDataContainerType,
                            StatisticContainerType,
                            UserDataType>::module_closed (void)
{
  STREAM_TRACE (ACE_TEXT ("Stream_HeadModuleTaskBase_T::module_closed"));

  // *NOTE*: this will be a NOP IF the stream was
  //         stop()ped BEFORE it is deleted !

  // *NOTE*: this method is invoked by an external thread either:
  //         - from the ACE_Module dtor OR
  //         - during explicit ACE_Module::close() (e.g. stream is being
  //           reinitialized --> module reset)

  // *NOTE*: when control flow gets here, the stream should already be in a
  //         final state

  // sanity check
  // *WARNING*: this test CAN NOT prevent potential race conditions
  if (isRunning ())
  {
    // *NOTE*: MAY happen after application receives a SIGINT
    //         select() returns -1, reactor invokes:
    //         remove_handler --> remove_reference --> delete this
    ACE_DEBUG ((LM_WARNING,
                ACE_TEXT ("%s: stream still active in Stream_HeadModuleTaskBase_T::module_closed(), continuing\n"),
                inherited2::mod_->name ()));
    stop (true,   // wait for completion ?
          false); // N/A
  } // end IF

  return 0;
}

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType,
          typename ConfigurationType,
          typename SessionControlType,
          typename SessionEventType,
          typename StreamStateType,
          typename SessionDataType,
          typename SessionDataContainerType,
          typename StatisticContainerType,
          typename UserDataType>
int
Stream_HeadModuleTaskBase_T<ACE_SYNCH_USE,
                            TimePolicyType,
                            ControlMessageType,
                            DataMessageType,
                            SessionMessageType,
                            ConfigurationType,
                            SessionControlType,
                            SessionEventType,
                            StreamStateType,
                            SessionDataType,
                            SessionDataContainerType,
                            StatisticContainerType,
                            UserDataType>::svc (void)
{
  STREAM_TRACE (ACE_TEXT ("Stream_HeadModuleTaskBase_T::svc"));

  if (concurrency_ == STREAM_HEADMODULECONCURRENCY_ACTIVE)
  {
    if (inherited2::mod_)
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("%s: worker thread (ID: %t) starting...\n"),
                  inherited2::mod_->name ()));
    else
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("(%s): worker thread (ID: %t) starting...\n"),
                  ACE_TEXT (inherited2::threadName_.c_str ())));
  } // end IF

  // sanity check(s)
  ACE_ASSERT (inherited2::sessionData_);

  int                            error                    = 0;
  bool                           has_finished             = false;
  ACE_Message_Block*             message_block_p          = NULL;
  bool                           release_lock             = false;
  int                            result                   = -1;
  SessionDataContainerType*      session_data_container_p =
    inherited2::sessionData_;
  const SessionDataType*         session_data_p           =
    &inherited2::sessionData_->get ();
  bool                           stop_processing          = false;
//  SessionMessageType*            session_message_p        = NULL;
//  enum Stream_SessionMessageType session_message_type     =
//    STREAM_SESSION_MESSAGE_INVALID;

  do
  {
    message_block_p = NULL;
    result = inherited2::getq (message_block_p,
                               NULL);
    if (result == -1)
    {
      error = ACE_OS::last_error ();
      if (error != EWOULDBLOCK) // Win32: 10035
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("%s: worker thread (ID: %t) failed to ACE_Task::getq(): \"%m\", aborting\n"),
                    inherited2::mod_->name ()));

      if (!has_finished)
      {
        has_finished = true;
        // enqueue(/process) STREAM_SESSION_END
        inherited::finished ();
      } // end IF

      break;
    } // end IF
    ACE_ASSERT (message_block_p);

    switch (message_block_p->msg_type ())
    {
      case ACE_Message_Block::MB_STOP:
      {
        // *NOTE*: when close()d manually (i.e. user abort), 'finished' will
        //         not have been set at this stage

        // signal the controller ?
        if (!has_finished)
        {
          has_finished = true;
          // enqueue(/process) STREAM_SESSION_END
          inherited::finished ();
        } // end IF

        if (inherited2::thr_count_ > 1)
        {
          result = inherited2::putq (message_block_p, NULL);
          if (result == -1)
            ACE_DEBUG ((LM_ERROR,
                        ACE_TEXT ("%s: failed to ACE_Task::putq(): \"%m\", aborting\n"),
                        inherited2::mod_->name ()));
        } // end IF
        else
          message_block_p->release ();

        // has STREAM_SESSION_END been processed ?
        if (!sessionEndProcessed_)
          continue; // process STREAM_SESSION_END

        result = 0;

        goto done; // STREAM_SESSION_END has been processed
      }
      default:
      {
        // sanity check(s)
        ACE_ASSERT (streamLock_);

        // grab stream lock if processing is 'non-concurrent'
        if (!concurrent_)
        {
          try {
            release_lock = streamLock_->lock (true);
          } catch (...) {
            ACE_DEBUG ((LM_ERROR,
                        ACE_TEXT ("%s: caught exception in Stream_ILock_T::lock(true), continuing\n"),
                        inherited2::mod_->name ()));
          }
        } // end IF

        inherited2::handleMessage (message_block_p,
                                   stop_processing);

        // *IMPORTANT NOTE*: as the session data may change if this stream is
        //                   (un-)link()ed (e.g. inbound network data
        //                   processing), the data handle may need to be updated
        if (inherited2::sessionData_ &&
            (session_data_container_p != inherited2::sessionData_))
          session_data_p = &inherited2::sessionData_->get ();

        if (release_lock)
        {
          try {
            streamLock_->unlock (false);
          } catch (...) {
            ACE_DEBUG ((LM_ERROR,
                        ACE_TEXT ("%s: caught exception in Stream_ILock_T::unlock(false), continuing\n"),
                        inherited2::mod_->name ()));
          }
        } // end IF

        // finished ?
        if (stop_processing)
        {
          // *IMPORTANT NOTE*: message_block_p has already been released() !

          if (!has_finished)
          {
            has_finished = true;
            // enqueue(/process) STREAM_SESSION_END
            inherited::finished ();
          } // end IF

          continue;
        } // end IF

        break;
      }
    } // end SWITCH

    // session aborted ?
    // sanity check(s)
    // *TODO*: remove type inferences
    ACE_ASSERT (session_data_p->lock);
    { ACE_GUARD_RETURN (ACE_SYNCH_MUTEX_T, aGuard, *session_data_p->lock, result);

      if (session_data_p->aborted &&
          !has_finished)
      { // *TODO*: remove type inferences
        ACE_DEBUG ((LM_DEBUG,
                    ACE_TEXT ("%s: session %u aborted...\n"),
                    inherited2::mod_->name (),
                    session_data_p->sessionID));

        has_finished = true;
        // enqueue(/process) STREAM_SESSION_END
        inherited::finished ();
      } // end IF
    } // end lock scope
  } while (true);
  result = -1;

done:
  return result;
}

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType,
          typename ConfigurationType,
          typename SessionControlType,
          typename SessionEventType,
          typename StreamStateType,
          typename SessionDataType,
          typename SessionDataContainerType,
          typename StatisticContainerType,
          typename UserDataType>
void
Stream_HeadModuleTaskBase_T<ACE_SYNCH_USE,
                            TimePolicyType,
                            ControlMessageType,
                            DataMessageType,
                            SessionMessageType,
                            ConfigurationType,
                            SessionControlType,
                            SessionEventType,
                            StreamStateType,
                            SessionDataType,
                            SessionDataContainerType,
                            StatisticContainerType,
                            UserDataType>::handleSessionMessage (SessionMessageType*& message_inout,
                                                                 bool& passMessageDownstream_out)
{
  STREAM_TRACE (ACE_TEXT ("Stream_HeadModuleTaskBase_T::handleSessionMessage"));

  int result = -1;

  // don't care (implies yes per default, if part of a stream)
  ACE_UNUSED_ARG (passMessageDownstream_out);

  // sanity check(s)
  ACE_ASSERT (inherited2::mod_);
  ACE_ASSERT (inherited2::configuration_);

  switch (message_inout->type ())
  {
    case STREAM_SESSION_MESSAGE_BEGIN:
    {
      // *TODO*: remove type inference
      // sanity check(s)
      ACE_ASSERT (inherited2::configuration_->streamConfiguration);

      // schedule regular statistic collection ?
      // *NOTE*: the runtime-statistic module is responsible for regular
      //         reporting, the head module merely collects information
      if (inherited2::configuration_->streamConfiguration->statisticReportingInterval !=
          ACE_Time_Value::zero)
      {
        ACE_Time_Value interval (STREAM_DEFAULT_STATISTIC_COLLECTION_INTERVAL,
                                 0);
        ACE_ASSERT (timerID_ == -1);
        ACE_Event_Handler* handler_p = &statisticCollectionHandler_;
        timerID_ =
          COMMON_TIMERMANAGER_SINGLETON::instance ()->schedule_timer (handler_p,                  // event handler
                                                                      NULL,                       // argument
                                                                      COMMON_TIME_NOW + interval, // first wakeup time
                                                                      interval);                  // interval
        if (timerID_ == -1)
        {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("%s: failed to Common_Timer_Manager::schedule_timer(): \"%m\", returning\n"),
                      inherited2::mod_->name ()));
          return;
        } // end IF
//        ACE_DEBUG ((LM_DEBUG,
//                    ACE_TEXT ("scheduled statistic collecting timer (ID: %d) for interval %#T...\n"),
//                    timerID_,
//                    &interval));
      } // end IF

      break;
    }
    case STREAM_SESSION_MESSAGE_END:
    {
      // *NOTE*: only process the first 'session end' message (see above: 2566)
      { ACE_GUARD (ACE_SYNCH_MUTEX, aGuard, inherited2::lock_);

        if (sessionEndProcessed_) break; // done
        sessionEndProcessed_ = true;
      } // end lock scope

      if (timerID_ != -1)
      {
        const void* act_p = NULL;
        result =
          COMMON_TIMERMANAGER_SINGLETON::instance ()->cancel_timer (timerID_,
                                                                    &act_p);
        if (result == -1)
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("%s: failed to cancel timer (ID: %d): \"%m\", continuing\n"),
                      inherited2::mod_->name (),
                      timerID_));
        timerID_ = -1;
      } // end IF

      if (concurrency_ != STREAM_HEADMODULECONCURRENCY_CONCURRENT)
        inherited2::stop (false,  // wait for completion ?
                          false); // N/A

      break;
    }
    default:
      break;
  } // end SWITCH
}

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType,
          typename ConfigurationType,
          typename SessionControlType,
          typename SessionEventType,
          typename StreamStateType,
          typename SessionDataType,
          typename SessionDataContainerType,
          typename StatisticContainerType,
          typename UserDataType>
bool
Stream_HeadModuleTaskBase_T<ACE_SYNCH_USE,
                            TimePolicyType,
                            ControlMessageType,
                            DataMessageType,
                            SessionMessageType,
                            ConfigurationType,
                            SessionControlType,
                            SessionEventType,
                            StreamStateType,
                            SessionDataType,
                            SessionDataContainerType,
                            StatisticContainerType,
                            UserDataType>::initialize (const ConfigurationType& configuration_in,
                                                       Stream_IAllocator* allocator_in)
{
  STREAM_TRACE (ACE_TEXT ("Stream_HeadModuleTaskBase_T::initialize"));

  int result = -1;

  // sanity check(s)
  // *TODO*: remove type inference
  ACE_ASSERT (configuration_in.stateMachineLock);

  if (isInitialized_)
  {
    sessionEndProcessed_ = false;
    sessionEndSent_ = false;
    streamLock_ = NULL;
    streamState_ = NULL;

    if (timerID_ != -1)
    {
      const void* act_p = NULL;
      result =
        COMMON_TIMERMANAGER_SINGLETON::instance ()->cancel_timer (timerID_,
                                                                  &act_p);
      if (result == -1)
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to cancel timer (ID: %d): \"%m\", continuing\n"),
                    timerID_));
    } // end IF
    timerID_ = -1;

    sessionDataLock_ = NULL;

    isInitialized_ = false;
  } // end IF

  // *TODO*: remove type inferences
  concurrent_ = configuration_in.concurrent;
  concurrency_ = configuration_in.concurrency;
  streamLock_ = configuration_in.streamLock;

  // *NOTE*: deactivate the queue so it does not accept new data
  result = inherited2::msg_queue_->activate ();
  if (result == -1)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Message_Queue::activate(): \"%m\", continuing\n")));

  if (!inherited2::initialize (configuration_in,
                               allocator_in))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Stream_TaskBase_T::initialize(), aborting\n")));
    return false;
  } // end IF
  ACE_ASSERT (inherited2::configuration_);

  // *TODO*: remove type inference
  //inherited2::configuration_->streamLock = this;
  isInitialized_ =
      inherited::initialize (*inherited2::configuration_->stateMachineLock);
  if (!isInitialized_)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_StateMachine_Base_T::initialize(), aborting\n")));
    return false;
  } // end IF
  inherited::change (STREAM_STATE_INITIALIZED);

  return true;
}

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType,
          typename ConfigurationType,
          typename SessionControlType,
          typename SessionEventType,
          typename StreamStateType,
          typename SessionDataType,
          typename SessionDataContainerType,
          typename StatisticContainerType,
          typename UserDataType>
void
Stream_HeadModuleTaskBase_T<ACE_SYNCH_USE,
                            TimePolicyType,
                            ControlMessageType,
                            DataMessageType,
                            SessionMessageType,
                            ConfigurationType,
                            SessionControlType,
                            SessionEventType,
                            StreamStateType,
                            SessionDataType,
                            SessionDataContainerType,
                            StatisticContainerType,
                            UserDataType>::control (SessionControlType control_in,
                                                    bool /* forwardUpStream_in */)
{
  STREAM_TRACE (ACE_TEXT ("Stream_HeadModuleTaskBase_T::control"));

  SessionEventType message_type = STREAM_SESSION_MESSAGE_INVALID;

  switch (control_in)
  {
    case STREAM_CONTROL_FLUSH:
    {
      if (!inherited2::putControlMessage (STREAM_CONTROL_FLUSH))
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("%s: failed to Stream_TaskBase_T::putControlMessage(%d), continuing\n"),
                    inherited2::name (),
                    control_in));
      break;
    }
    case STREAM_CONTROL_LINK:
      message_type = STREAM_SESSION_MESSAGE_LINK; goto send_session_message;
    case STREAM_CONTROL_STEP:
      message_type = STREAM_SESSION_MESSAGE_STEP; goto send_session_message;
    case STREAM_CONTROL_UNLINK:
      message_type = STREAM_SESSION_MESSAGE_UNLINK; goto send_session_message;
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown control (was: %d), returning\n"),
                  control_in));
      return;
    }
  } // end SWITCH

  return;

send_session_message:
  // *NOTE*: in 'concurrent' (server-side-)scenarios there is a race
  //         condition when the connection is close()d asynchronously
  //         --> see below: line 2015
  bool release_lock = false;
  if (concurrency_ == STREAM_HEADMODULECONCURRENCY_CONCURRENT)
  {
    // sanity check(s)
    ACE_ASSERT (streamLock_);

    // *NOTE*: prevent potential deadlocks here; in 'busy' scenarios (i.e. high
    //         contention for message buffers/queue slots), a thread may be
    //         holding
    try {
      release_lock = streamLock_->lock (true);
    } catch (...) {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("%s: caught exception in Stream_ILock_T::lock(true), continuing\n"),
                  inherited2::mod_->name ()));
    }
  } // end IF

  SessionDataContainerType* session_data_container_p =
    inherited2::sessionData_;
  if (session_data_container_p)
    session_data_container_p->increase ();

  if (!inherited2::putSessionMessage (message_type,
                                      session_data_container_p,
                                      (streamState_ ? streamState_->userData : NULL)))
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: failed to Stream_TaskBase_T::putSessionMessage(%d), continuing\n"),
                inherited2::name (),
                message_type));

  // clean up
  if (release_lock)
  {
    try {
      streamLock_->unlock (false);
    } catch (...) {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("%s: caught exception in Stream_ILock_T::unlock(false), continuing\n"),
                  inherited2::mod_->name ()));
    }
  } // end IF
}
template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType,
          typename ConfigurationType,
          typename SessionControlType,
          typename SessionEventType,
          typename StreamStateType,
          typename SessionDataType,
          typename SessionDataContainerType,
          typename StatisticContainerType,
          typename UserDataType>
void
Stream_HeadModuleTaskBase_T<ACE_SYNCH_USE,
                            TimePolicyType,
                            ControlMessageType,
                            DataMessageType,
                            SessionMessageType,
                            ConfigurationType,
                            SessionControlType,
                            SessionEventType,
                            StreamStateType,
                            SessionDataType,
                            SessionDataContainerType,
                            StatisticContainerType,
                            UserDataType>::notify (SessionEventType notification_in,
                                                   bool /* forwardUpStream_in */)
{
  STREAM_TRACE (ACE_TEXT ("Stream_HeadModuleTaskBase_T::notify"));

  switch (notification_in)
  {
    case STREAM_SESSION_MESSAGE_ABORT:
    {
      // *NOTE*: in 'concurrent' (server-side-)scenarios there is a race
      //         condition when the connection is close()d asynchronously
      //         --> see below: line 2015
      bool release_lock = false;
      if (concurrency_ == STREAM_HEADMODULECONCURRENCY_CONCURRENT)
      {
        // sanity check(s)
        ACE_ASSERT (streamLock_);

        try {
            release_lock = streamLock_->lock (true);
        } catch (...) {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("%s: caught exception in Stream_ILock_T::lock(true), continuing\n"),
                      inherited2::mod_->name ()));
        }
      } // end IF

      SessionDataContainerType* session_data_container_p =
        inherited2::sessionData_;
      if (session_data_container_p)
      {
        session_data_container_p->increase ();

        SessionDataType& session_data_r =
          const_cast<SessionDataType&> (session_data_container_p->get ());

        ACE_ASSERT (session_data_r.lock);
        { ACE_GUARD (ACE_SYNCH_MUTEX_T, aGuard, *session_data_r.lock);

          session_data_r.aborted = true;
        } // end lock scope

        session_data_container_p->decrease ();
      } // end IF

      if (release_lock)
      {
        try {
            streamLock_->unlock (false);
        } catch (...) {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("%s: caught exception in Stream_ILock_T::unlock(false), continuing\n"),
                      inherited2::mod_->name ()));
        }
      } // end IF
      // *WARNING*: falls through
    }
    case STREAM_SESSION_MESSAGE_DISCONNECT:
    default:
    {
      // *NOTE*: in 'concurrent' (server-side-)scenarios there is a race
      //         condition when the connection is close()d asynchronously
      //         --> see below: line 2015
      bool release_lock = false;
      if (concurrency_ == STREAM_HEADMODULECONCURRENCY_CONCURRENT)
      {
        // sanity check(s)
        ACE_ASSERT (streamLock_);

        try {
            release_lock = streamLock_->lock (true);
        } catch (...) {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("%s: caught exception in Stream_ILock_T::lock(true), continuing\n"),
                      inherited2::mod_->name ()));
        }
      } // end IF

      SessionDataContainerType* session_data_container_p =
          inherited2::sessionData_;
      if (session_data_container_p)
        session_data_container_p->increase ();

      if (!inherited2::putSessionMessage (static_cast<Stream_SessionMessageType> (notification_in),
                                          session_data_container_p,
                                          (streamState_ ? streamState_->userData : NULL)))
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("%s: failed to Stream_TaskBase_T::putSessionMessage(%d), continuing\n"),
                    inherited2::name (),
                    notification_in));

        // clean up
        if (session_data_container_p)
          session_data_container_p->decrease ();
      } // end IF

      if (release_lock)
      {
        try {
            streamLock_->unlock (false);
        } catch (...) {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("%s: caught exception in Stream_ILock_T::unlock(false), continuing\n"),
                      inherited2::mod_->name ()));
        }
      } // end IF

      break;
    }
  } // end SWITCH
}

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType,
          typename ConfigurationType,
          typename SessionControlType,
          typename SessionEventType,
          typename StreamStateType,
          typename SessionDataType,
          typename SessionDataContainerType,
          typename StatisticContainerType,
          typename UserDataType>
void
Stream_HeadModuleTaskBase_T<ACE_SYNCH_USE,
                            TimePolicyType,
                            ControlMessageType,
                            DataMessageType,
                            SessionMessageType,
                            ConfigurationType,
                            SessionControlType,
                            SessionEventType,
                            StreamStateType,
                            SessionDataType,
                            SessionDataContainerType,
                            StatisticContainerType,
                            UserDataType>::start ()
{
  STREAM_TRACE (ACE_TEXT ("Stream_HeadModuleTaskBase_T::start"));

  if (inherited2::sessionData_)
  {
    // *TODO*: remove type inference
    SessionDataType& session_data_r =
        const_cast<SessionDataType&> (inherited2::sessionData_->get ());

    { ACE_GUARD (ACE_SYNCH_MUTEX, aGuard, *session_data_r.lock);

      session_data_r.startOfSession = COMMON_TIME_NOW;
    } // end lock scope
  } // end IF

  // --> start a worker thread, if active
  inherited::change (STREAM_STATE_RUNNING);
}

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType,
          typename ConfigurationType,
          typename SessionControlType,
          typename SessionEventType,
          typename StreamStateType,
          typename SessionDataType,
          typename SessionDataContainerType,
          typename StatisticContainerType,
          typename UserDataType>
void
Stream_HeadModuleTaskBase_T<ACE_SYNCH_USE,
                            TimePolicyType,
                            ControlMessageType,
                            DataMessageType,
                            SessionMessageType,
                            ConfigurationType,
                            SessionControlType,
                            SessionEventType,
                            StreamStateType,
                            SessionDataType,
                            SessionDataContainerType,
                            StatisticContainerType,
                            UserDataType>::stop (bool wait_in,
                                                 bool lockedAccess_in)
{
  STREAM_TRACE (ACE_TEXT ("Stream_HeadModuleTaskBase_T::stop"));

  ACE_UNUSED_ARG (lockedAccess_in);

  // (try to) change state
  inherited::change (STREAM_STATE_STOPPED);

  if (wait_in)
    wait (true,
          false,
          false);
}

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType,
          typename ConfigurationType,
          typename SessionControlType,
          typename SessionEventType,
          typename StreamStateType,
          typename SessionDataType,
          typename SessionDataContainerType,
          typename StatisticContainerType,
          typename UserDataType>
bool
Stream_HeadModuleTaskBase_T<ACE_SYNCH_USE,
                            TimePolicyType,
                            ControlMessageType,
                            DataMessageType,
                            SessionMessageType,
                            ConfigurationType,
                            SessionControlType,
                            SessionEventType,
                            StreamStateType,
                            SessionDataType,
                            SessionDataContainerType,
                            StatisticContainerType,
                            UserDataType>::isRunning () const
{
  STREAM_TRACE (ACE_TEXT ("Stream_HeadModuleTaskBase_T::isRunning"));

  Stream_StateMachine_ControlState status = inherited::current ();

  return ((status == STREAM_STATE_PAUSED) || (status == STREAM_STATE_RUNNING));
}

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType,
          typename ConfigurationType,
          typename SessionControlType,
          typename SessionEventType,
          typename StreamStateType,
          typename SessionDataType,
          typename SessionDataContainerType,
          typename StatisticContainerType,
          typename UserDataType>
void
Stream_HeadModuleTaskBase_T<ACE_SYNCH_USE,
                            TimePolicyType,
                            ControlMessageType,
                            DataMessageType,
                            SessionMessageType,
                            ConfigurationType,
                            SessionControlType,
                            SessionEventType,
                            StreamStateType,
                            SessionDataType,
                            SessionDataContainerType,
                            StatisticContainerType,
                            UserDataType>::link ()
{
  STREAM_TRACE (ACE_TEXT ("Stream_HeadModuleTaskBase_T::link"));

  // sanity check(s)
  if (!inherited2::sessionData_)
    return;

  typename SessionMessageType::DATA_T::DATA_T& session_data_r =
    const_cast<typename SessionMessageType::DATA_T::DATA_T&> (inherited2::sessionData_->get ());

  // *NOTE*: after the stream is unlinked again, the upstream session data
  //         lock (if any) may go away unexpectedly
  //         --> retain a handle to the original lock
  sessionDataLock_ = session_data_r.lock;
}
template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType,
          typename ConfigurationType,
          typename SessionControlType,
          typename SessionEventType,
          typename StreamStateType,
          typename SessionDataType,
          typename SessionDataContainerType,
          typename StatisticContainerType,
          typename UserDataType>
void
Stream_HeadModuleTaskBase_T<ACE_SYNCH_USE,
                            TimePolicyType,
                            ControlMessageType,
                            DataMessageType,
                            SessionMessageType,
                            ConfigurationType,
                            SessionControlType,
                            SessionEventType,
                            StreamStateType,
                            SessionDataType,
                            SessionDataContainerType,
                            StatisticContainerType,
                            UserDataType>::unlink ()
{
  STREAM_TRACE (ACE_TEXT ("Stream_HeadModuleTaskBase_T::unlink"));

  // sanity check(s)
  if (!inherited2::sessionData_)
    return;

  typename SessionMessageType::DATA_T::DATA_T& session_data_r =
    const_cast<typename SessionMessageType::DATA_T::DATA_T&> (inherited2::sessionData_->get ());

  // *NOTE*: after this, the upstream session data lock may go away
  //         unexpectedly
  //         --> reset original lock (if any)
  session_data_r.lock = sessionDataLock_;

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("%s: stream has been unlinked, reset session data lock...\n"),
              inherited2::mod_->name ()));
}

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType,
          typename ConfigurationType,
          typename SessionControlType,
          typename SessionEventType,
          typename StreamStateType,
          typename SessionDataType,
          typename SessionDataContainerType,
          typename StatisticContainerType,
          typename UserDataType>
bool
Stream_HeadModuleTaskBase_T<ACE_SYNCH_USE,
                            TimePolicyType,
                            ControlMessageType,
                            DataMessageType,
                            SessionMessageType,
                            ConfigurationType,
                            SessionControlType,
                            SessionEventType,
                            StreamStateType,
                            SessionDataType,
                            SessionDataContainerType,
                            StatisticContainerType,
                            UserDataType>::lock (bool block_in)
{
  STREAM_TRACE (ACE_TEXT ("Stream_HeadModuleTaskBase_T::lock"));

  int result = -1;
  ACE_SYNCH_MUTEX_T& lock_r = inherited2::queue_.lock ();

  result = (block_in ? lock_r.acquire () : lock_r.tryacquire ());
  if (result == -1)
  {
    int error = ACE_OS::last_error ();
    if (error == EBUSY)
      return false;
  } // end IF

  return true;
}
template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType,
          typename ConfigurationType,
          typename SessionControlType,
          typename SessionEventType,
          typename StreamStateType,
          typename SessionDataType,
          typename SessionDataContainerType,
          typename StatisticContainerType,
          typename UserDataType>
int
Stream_HeadModuleTaskBase_T<ACE_SYNCH_USE,
                            TimePolicyType,
                            ControlMessageType,
                            DataMessageType,
                            SessionMessageType,
                            ConfigurationType,
                            SessionControlType,
                            SessionEventType,
                            StreamStateType,
                            SessionDataType,
                            SessionDataContainerType,
                            StatisticContainerType,
                            UserDataType>::unlock (bool unlock_in)
{
  STREAM_TRACE (ACE_TEXT ("Stream_HeadModuleTaskBase_T::unlock"));

  int result = -1;
  ACE_SYNCH_MUTEX_T& lock_r = inherited2::queue_.lock ();
  ACE_thread_mutex_t& mutex_r = lock_r.lock ();

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  if (!ACE_OS::thr_equal (reinterpret_cast<ACE_thread_t> (mutex_r.OwningThread),
                          ACE_OS::thr_self ()))
    return -1;
#else
  if (!ACE_OS::thr_equal (static_cast<ACE_thread_t> (mutex_r.__data.__owner),
                          ACE_OS::thr_self ()))
    return -1;
#endif

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  do
  {
    result = lock_r.release ();
    if (!unlock_in) break;
  } while (mutex_r.RecursionCount > 0);
#else
  do
  {
    result = lock_r.release ();
    if (!unlock_in) break;
  } while (mutex_r.__data.__count > 0);
#endif
  if (result == -1)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: failed to ACE_SYNCH_RECURSIVE_MUTEX::release(): \"%m\", continuing\n"),
                inherited2::mod_->name ()));

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  return mutex_r.RecursionCount;
#else
  return mutex_r.__data.__count;
#endif
}

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType,
          typename ConfigurationType,
          typename SessionControlType,
          typename SessionEventType,
          typename StreamStateType,
          typename SessionDataType,
          typename SessionDataContainerType,
          typename StatisticContainerType,
          typename UserDataType>
void
Stream_HeadModuleTaskBase_T<ACE_SYNCH_USE,
                            TimePolicyType,
                            ControlMessageType,
                            DataMessageType,
                            SessionMessageType,
                            ConfigurationType,
                            SessionControlType,
                            SessionEventType,
                            StreamStateType,
                            SessionDataType,
                            SessionDataContainerType,
                            StatisticContainerType,
                            UserDataType>::wait (bool waitForThreads_in,
                                                 bool waitForUpStream_in,
                                                 bool waitForDownStream_in)
{
  STREAM_TRACE (ACE_TEXT ("Stream_HeadModuleTaskBase_T::wait"));

  ACE_UNUSED_ARG (waitForUpStream_in);
  ACE_UNUSED_ARG (waitForDownStream_in);

  // sanity check(s)
  ACE_ASSERT (inherited2::configuration_);

  // *IMPORTANT NOTE*: when a connection is close()d, a race condition may
  //                   arise here between any of the following actors:
  // - the application (main) thread waiting in Stream_Base_T::waitForCompletion
  // - a (network) event dispatching thread (connection hanndler calling
  //   handle_close() --> wait() of the (network) data processing
  //   (sub-)stream)
  // - a stream head module thread pushing the SESSION_END message (i.e.
  //   processing in the 'Net Source/Target' module)
  // - a 'Net IO' module thread processing the SESSION_END message

  int result = -1;
  ACE_Reverse_Lock<ACE_SYNCH_MUTEX> reverse_lock (inherited2::lock_);

  // *NOTE*: be sure to release the stream lock to support 'concurrent'
  //         scenarios (e.g. scenarios where upstream delivers data)
  int nesting_level = -1;
  ACE_ASSERT (streamLock_);
  //ACE_Reverse_Lock<ACE_SYNCH_MUTEX> reverse_lock (streamLock_->getLock ());
  //ACE_GUARD (ACE_Reverse_Lock<ACE_SYNCH_MUTEX>, aGuard, reverse_lock);
  try {
    nesting_level = streamLock_->unlock (true);
  } catch (...) {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: caught exception in Stream_ILock_T::unlock(true), continuing\n"),
                inherited2::mod_->name ()));
  }

  // step1: wait for final state
  inherited::wait (STREAM_STATE_FINISHED,
                   NULL); // <-- block

  // step2: wait for worker(s) to join ?
  if (!waitForThreads_in)
  {
    if (nesting_level >= 0)
      COMMON_ILOCK_ACQUIRE_N (streamLock_,
                              nesting_level + 1);

    return;
    //goto continue_;
  } // end IF

  ACE_GUARD (ACE_SYNCH_MUTEX, aGuard, inherited2::lock_);

  // *NOTE*: pthread_join() returns EDEADLK when the calling thread IS the
  //         thread to join
  //         --> prevent this by comparing thread ids
  // *TODO*: check the whole array
  if (inherited2::threadIDs_.empty () ||
      ACE_OS::thr_equal (ACE_OS::thr_self (),
                          inherited2::threadIDs_[0].id ()))
    goto continue_;

  switch (concurrency_)
  {
    case STREAM_HEADMODULECONCURRENCY_ACTIVE:
    {
      { ACE_GUARD (ACE_Reverse_Lock<ACE_SYNCH_MUTEX>, aGuard_2, reverse_lock);

        result = inherited2::wait ();
      } // end lock scope
      if (result == -1)
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ACE_Task_Base::wait(): \"%m\", continuing\n")));
      break;
    }
    case STREAM_HEADMODULECONCURRENCY_PASSIVE:
    {
      ACE_thread_t thread_id = inherited2::threadIDs_[0].id ();
      ACE_THR_FUNC_RETURN status;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
      ACE_hthread_t handle = inherited2::threadIDs_[0].handle ();
      if (handle != ACE_INVALID_HANDLE)
      {
        { ACE_GUARD (ACE_Reverse_Lock<ACE_SYNCH_MUTEX>, aGuard_2, reverse_lock);

          result = ACE_Thread::join (handle, &status);
        } // end lock scope
        if (result == -1)
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("failed to ACE_Thread::join(): \"%m\", continuing\n")));
        else if (result == 0)
        {
          // *NOTE*: successful join()s close the thread handle
          //         (see OS_NS_Thread.inl:2971)
          inherited2::threadIDs_[0].handle (ACE_INVALID_HANDLE);
        } // end IF
        inherited2::threadIDs_[0].id (std::numeric_limits<DWORD>::max ());
      } // end IF
      else
        result = 0;
#else
      if (static_cast<int> (thread_id) != -1)
      {
        { ACE_GUARD (ACE_Reverse_Lock<ACE_SYNCH_MUTEX>, aGuard_2, reverse_lock);

          result = ACE_Thread::join (thread_id, NULL, &status);
        } // end lock scope
        inherited2::threadIDs_[0].id (-1);
      } // end IF
      else
        result = 0;
#endif
      if (result == -1)
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ACE_Thread::join(%lu): \"%m\", continuing\n"),
                    thread_id));
      break;
    }
    default:
      break;
  } // end SWITCH

continue_:
  if (nesting_level >= 0)
    COMMON_ILOCK_ACQUIRE_N (streamLock_,
                            nesting_level + 1);
}

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType,
          typename ConfigurationType,
          typename SessionControlType,
          typename SessionEventType,
          typename StreamStateType,
          typename SessionDataType,
          typename SessionDataContainerType,
          typename StatisticContainerType,
          typename UserDataType>
bool
Stream_HeadModuleTaskBase_T<ACE_SYNCH_USE,
                            TimePolicyType,
                            ControlMessageType,
                            DataMessageType,
                            SessionMessageType,
                            ConfigurationType,
                            SessionControlType,
                            SessionEventType,
                            StreamStateType,
                            SessionDataType,
                            SessionDataContainerType,
                            StatisticContainerType,
                            UserDataType>::initialize (const StreamStateType& streamState_in)
{
  STREAM_TRACE (ACE_TEXT ("Stream_HeadModuleTaskBase_T::initialize"));

  streamState_ = &const_cast<StreamStateType&> (streamState_in);
//  sessionData_ = &const_cast<SessionDataType&> (sessionData_in);

  inherited::change (STREAM_STATE_INITIALIZED);

  return true;
}

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType,
          typename ConfigurationType,
          typename SessionControlType,
          typename SessionEventType,
          typename StreamStateType,
          typename SessionDataType,
          typename SessionDataContainerType,
          typename StatisticContainerType,
          typename UserDataType>
bool
Stream_HeadModuleTaskBase_T<ACE_SYNCH_USE,
                            TimePolicyType,
                            ControlMessageType,
                            DataMessageType,
                            SessionMessageType,
                            ConfigurationType,
                            SessionControlType,
                            SessionEventType,
                            StreamStateType,
                            SessionDataType,
                            SessionDataContainerType,
                            StatisticContainerType,
                            UserDataType>::putStatisticMessage (const StatisticContainerType& statisticData_in)
{
  STREAM_TRACE (ACE_TEXT ("Stream_HeadModuleTaskBase_T::putStatisticMessage"));

  bool result = false;

  // *NOTE*: in 'concurrent' (server-side-)scenarios there is a race
  //         condition when the connection is close()d asynchronously
  //         --> see below: line 2015
  bool release_lock = false;
  if (concurrency_ == STREAM_HEADMODULECONCURRENCY_CONCURRENT)
  {
    // sanity check(s)
    ACE_ASSERT (streamLock_);

    try {
      release_lock = streamLock_->lock (true);
    } catch (...) {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("%s: caught exception in Stream_ILock_T::lock(true), continuing\n"),
                  inherited2::mod_->name ()));
    }
  } // end IF

  SessionDataContainerType* session_data_container_p =
    inherited2::sessionData_;
  if (session_data_container_p)
  {
    // step0: prepare session data object container
    session_data_container_p->increase ();

    // step1: update session state
    SessionDataType& session_data_r =
      const_cast<SessionDataType&> (inherited2::sessionData_->get ());
    // *TODO*: remove type inferences
    session_data_r.currentStatistic = statisticData_in;

    // *TODO*: attach stream state information to the session data
  } // end IF

  // step3: send the statistic data downstream
  //  // *NOTE*: fire-and-forget session_data_container_p here
  // *TODO*: remove type inference
  result =
      inherited2::putSessionMessage (STREAM_SESSION_MESSAGE_STATISTIC,
                                     session_data_container_p,
                                     (streamState_ ? streamState_->userData : NULL));
  if (!result)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: failed to Stream_TaskBase_T::putSessionMessage(%d), aborting\n"),
                inherited2::name (),
                STREAM_SESSION_MESSAGE_STATISTIC));

    // clean up
    if (session_data_container_p)
      session_data_container_p->decrease ();
  } // end IF

  if (release_lock)
  {
    try {
      streamLock_->unlock (false);
    } catch (...) {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("%s: caught exception in Stream_ILock_T::unlock(false), continuing\n"),
                  inherited2::mod_->name ()));
    }
  } // end IF

  return result;
}

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType,
          typename ConfigurationType,
          typename SessionControlType,
          typename SessionEventType,
          typename StreamStateType,
          typename SessionDataType,
          typename SessionDataContainerType,
          typename StatisticContainerType,
          typename UserDataType>
void
Stream_HeadModuleTaskBase_T<ACE_SYNCH_USE,
                            TimePolicyType,
                            ControlMessageType,
                            DataMessageType,
                            SessionMessageType,
                            ConfigurationType,
                            SessionControlType,
                            SessionEventType,
                            StreamStateType,
                            SessionDataType,
                            SessionDataContainerType,
                            StatisticContainerType,
                            UserDataType>::onChange (Stream_StateType_t newState_in)
{
  STREAM_TRACE (ACE_TEXT ("Stream_HeadModuleTaskBase_T::onChange"));

  // sanity check(s)
  ACE_ASSERT (inherited::stateLock_);

  int result = -1;
  ACE_Reverse_Lock<ACE_SYNCH_MUTEX_T> reverse_lock (*inherited::stateLock_);

  switch (newState_in)
  {
    case STREAM_STATE_INITIALIZED:
    {
      ACE_GUARD (ACE_SYNCH_MUTEX, aGuard, inherited2::lock_);

      sessionEndSent_ = false;
      sessionEndProcessed_ = false;

      // --> re-initialize ?
      if (!inherited2::threadIDs_.empty ())
      {
#if defined (ACE_WIN32) || defined (ACE_WIN64)
        ACE_hthread_t handle = inherited2::threadIDs_[0].handle ();
        if (handle != ACE_INVALID_HANDLE)
          if (!::CloseHandle (handle))
            ACE_DEBUG ((LM_ERROR,
                        ACE_TEXT ("failed to CloseHandle(0x%@): \"%s\", continuing\n"),
                        handle,
                        ACE_TEXT (Common_Tools::error2String (::GetLastError ()).c_str ())));
#endif
        inherited2::threadIDs_.clear ();
      } // end IF

      //ACE_DEBUG ((LM_DEBUG,
      //            ACE_TEXT ("%s: head module (re-)initialized...\n"),
      //            inherited2::mod_->name ()));

      break;
    }
    case STREAM_STATE_RUNNING:
    {
      // *NOTE*: implement tape-recorder logic:
      //         transition PAUSED --> PAUSED is mapped to PAUSED --> RUNNING
      //         --> check for this condition before doing anything else...
      { ACE_GUARD (ACE_SYNCH_MUTEX_T, aGuard, *inherited::stateLock_);

        if (inherited::state_ == STREAM_STATE_PAUSED)
        {
          // resume worker ?
          switch (concurrency_)
          {
            case STREAM_HEADMODULECONCURRENCY_ACTIVE:
            {
              result = inherited2::resume ();
              if (result == -1)
                ACE_DEBUG ((LM_ERROR,
                            ACE_TEXT ("failed to ACE_Task::resume(): \"%m\", continuing\n")));
              break;
            } // end IF
            case STREAM_HEADMODULECONCURRENCY_PASSIVE:
            {
              ACE_GUARD (ACE_SYNCH_MUTEX, aGuard, inherited2::lock_);

              // task object not active --> resume the borrowed thread
              ACE_hthread_t handle = inherited2::threadIDs_[0].handle ();
    #if defined (ACE_WIN32) || defined (ACE_WIN64)
              ACE_ASSERT (handle != ACE_INVALID_HANDLE);
    #else
              ACE_ASSERT (static_cast<int> (handle) != ACE_INVALID_HANDLE);
    #endif
              result = ACE_Thread::resume (handle);
              if (result == -1)
                ACE_DEBUG ((LM_ERROR,
                            ACE_TEXT ("failed to ACE_Thread::resume(): \"%m\", continuing\n")));

              break;
            }
            default:
              break;
          } // end SWITCH

          break;
        } // end IF
      } // end lock scope

      // send initial session message downstream ?
      if (generateSessionMessages_)
      {
        // *NOTE*: in 'concurrent' (server-side-)scenarios there is a race
        //         condition when the connection is close()d asynchronously
        //         --> see below: line 2015
        bool release_lock = false;
        if (!concurrent_)
        {
          // sanity check(s)
          ACE_ASSERT (streamLock_);

          try {
            release_lock = streamLock_->lock (true);
          } catch (...) {
            ACE_DEBUG ((LM_ERROR,
                        ACE_TEXT ("%s: caught exception in Stream_ILock_T::lock(true), continuing\n"),
                        inherited2::mod_->name ()));
          }
        } // end IF

        SessionDataContainerType* session_data_container_p =
          inherited2::sessionData_;
        if (session_data_container_p)
          session_data_container_p->increase ();

        if (!inherited2::putSessionMessage (STREAM_SESSION_MESSAGE_BEGIN,                    // session message type
                                            session_data_container_p,                        // session data
                                            (streamState_ ? streamState_->userData : NULL))) // user data handle
        {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("failed to Stream_TaskBase_T::putSessionMessage(STREAM_SESSION_MESSAGE_BEGIN), continuing\n")));

          // clean up
          if (session_data_container_p)
            session_data_container_p->decrease ();
        } // end IF

        if (release_lock)
        {
          try {
            streamLock_->unlock (false);
          } catch (...) {
            ACE_DEBUG ((LM_ERROR,
                        ACE_TEXT ("%s: caught exception in Stream_ILock_T::unlock(false), continuing\n"),
                        inherited2::mod_->name ()));
          }
        } // end IF
      } // end IF

      switch (concurrency_)
      {
        case STREAM_HEADMODULECONCURRENCY_ACTIVE:
        {
          // spawn a worker thread
          // *TODO*: rewrite for thread counts > 1 (see also: wait() above)
          ACE_ASSERT (inherited2::threadCount_ >= 1);

          ACE_thread_t* thread_ids_p = NULL;
          ACE_NEW_NORETURN (thread_ids_p,
                            ACE_thread_t[inherited2::threadCount_]);
          if (!thread_ids_p)
          {
            ACE_DEBUG ((LM_CRITICAL,
                        ACE_TEXT ("failed to allocate memory (%u), aborting\n"),
                        (sizeof (ACE_thread_t) * inherited2::threadCount_)));
            return;
          } // end IF
          ACE_OS::memset (thread_ids_p, 0, sizeof (thread_ids_p));
          ACE_hthread_t* thread_handles_p = NULL;
          ACE_NEW_NORETURN (thread_handles_p,
                            ACE_hthread_t[inherited2::threadCount_]);
          if (!thread_handles_p)
          {
            ACE_DEBUG ((LM_CRITICAL,
                        ACE_TEXT ("failed to allocate memory (%u), aborting\n"),
                        (sizeof (ACE_hthread_t) * inherited2::threadCount_)));

            // clean up
            delete [] thread_ids_p;

            return;
          } // end IF
          ACE_OS::memset (thread_handles_p, 0, sizeof (thread_handles_p));
          const char** thread_names_p = NULL;
          ACE_NEW_NORETURN (thread_names_p,
                            const char*[inherited2::threadCount_]);
          if (!thread_names_p)
          {
            ACE_DEBUG ((LM_CRITICAL,
                        ACE_TEXT ("failed to allocate memory (%u), aborting\n"),
                        (sizeof (const char*) * inherited2::threadCount_)));

            // clean up
            delete [] thread_ids_p;
            delete [] thread_handles_p;

            return;
          } // end IF
          ACE_OS::memset (thread_names_p, 0, sizeof (thread_names_p));
          char* thread_name_p = NULL;
          std::string buffer;
          std::ostringstream converter;
          for (unsigned int i = 0;
               i < inherited2::threadCount_;
               ++i)
          {
            thread_name_p = NULL;
            ACE_NEW_NORETURN (thread_name_p,
                              char[BUFSIZ]);
            if (!thread_name_p)
            {
              ACE_DEBUG ((LM_CRITICAL,
                          ACE_TEXT ("failed to allocate memory (%u), aborting\n"),
                          (sizeof (char) * BUFSIZ)));

              // clean up
              delete [] thread_ids_p;
              delete [] thread_handles_p;
              for (unsigned int j = 0; j < i; j++)
                delete [] thread_names_p[j];
              delete [] thread_names_p;

              return;
            } // end IF
            ACE_OS::memset (thread_name_p, 0, sizeof (thread_name_p));
            converter.clear ();
            converter.str (ACE_TEXT_ALWAYS_CHAR (""));
            converter << (i + 1);
            buffer = inherited2::threadName_;
            buffer += ACE_TEXT_ALWAYS_CHAR (" #");
            buffer += converter.str ();
            ACE_OS::strcpy (thread_name_p,
                            buffer.c_str ());
            thread_names_p[i] = thread_name_p;
          } // end FOR
          result =
            ACE_Task_Base::activate ((THR_NEW_LWP      |
                                      THR_JOINABLE     |
                                      THR_INHERIT_SCHED),                         // flags
                                      static_cast<int> (inherited2::threadCount_), // # threads
                                      0,                                           // force spawning
                                      ACE_DEFAULT_THREAD_PRIORITY,                 // priority
                                      inherited2::grp_id (),                       // group id (see above)
                                      NULL,                                        // corresp. task --> use 'this'
                                      thread_handles_p,                            // thread handle(s)
                                      NULL,                                        // thread stack(s)
                                      NULL,                                        // thread stack size(s)
                                      thread_ids_p,                                // thread id(s)
                                      thread_names_p);                             // thread name(s)
          if (result == -1)
          {
            ACE_DEBUG ((LM_ERROR,
                        ACE_TEXT ("failed to ACE_Task_Base::activate(): \"%m\", continuing\n")));

            // clean up
            delete[] thread_ids_p;
            delete[] thread_handles_p;
            for (unsigned int i = 0; i < inherited2::threadCount_; i++)
              delete[] thread_names_p[i];
            delete[] thread_names_p;

            break;
          } // end IF

          std::ostringstream string_stream;
          ACE_Thread_ID thread_id;
          { ACE_GUARD (ACE_SYNCH_MUTEX, aGuard, inherited2::lock_);

            for (unsigned int i = 0;
                 i < inherited2::threadCount_;
                 ++i)
            {
              string_stream << ACE_TEXT_ALWAYS_CHAR ("#") << (i + 1)
                            << ACE_TEXT_ALWAYS_CHAR (" ")
                            << thread_ids_p[i]
                            << ACE_TEXT_ALWAYS_CHAR ("\n");

              // clean up
              delete [] thread_names_p[i];

              thread_id.handle (thread_handles_p[i]);
              thread_id.id (thread_ids_p[i]);
              inherited2::threadIDs_.push_back (thread_id);
            } // end FOR
          } // end lock scope
          std::string thread_ids_string = string_stream.str ();
          ACE_DEBUG ((LM_DEBUG,
                      ACE_TEXT ("(%s) spawned %u worker thread(s) (group: %d):\n%s"),
                      ACE_TEXT (inherited2::threadName_.c_str ()),
                      inherited2::threadCount_,
                      inherited2::grp_id (),
                      ACE_TEXT (thread_ids_string.c_str ())));

          // clean up
          delete[] thread_ids_p;
          delete[] thread_handles_p;
          delete[] thread_names_p;

          break;
        }
        case STREAM_HEADMODULECONCURRENCY_PASSIVE:
        {
          // *NOTE*: if the object is 'passive', the whole operation pertaining
          //         to newState_in is processed 'inline' by the calling thread,
          //         i.e. would complete 'before' the state has transitioned to
          //         'running'
          //         --> set the state early
          { ACE_GUARD (ACE_SYNCH_MUTEX_T, aGuard, *inherited::stateLock_);

            inherited::state_ = STREAM_STATE_RUNNING;
          } // end lock scope

          { ACE_GUARD (ACE_SYNCH_MUTEX, aGuard_2, inherited2::lock_);

            // sanity check(s)
            ACE_ASSERT (inherited2::threadIDs_.empty ());

            ACE_Thread_ID thread_id;
            thread_id.id (ACE_Thread::self ());
            ACE_hthread_t handle = ACE_INVALID_HANDLE;
            ACE_Thread::self (handle);
#if defined (ACE_WIN32) || defined (ACE_WIN64)
            HANDLE process_handle = ::GetCurrentProcess ();
            if (!::DuplicateHandle (process_handle,
                                    handle,
                                    process_handle,
                                    &handle,
                                    0,
                                    FALSE,
                                    DUPLICATE_CLOSE_SOURCE | DUPLICATE_SAME_ACCESS))
              ACE_DEBUG ((LM_ERROR,
                          ACE_TEXT ("failed to DuplicateHandle(0x%@): \"%s\", continuing\n"),
                          handle,
                          ACE_TEXT (Common_Tools::error2String (::GetLastError ()).c_str ())));
#endif
            thread_id.handle (handle);
            inherited2::threadIDs_.push_back (thread_id);
          } // end lock scope

          result = svc ();
          if (result == -1) // *NOTE*: most probable reason: session aborted
            ACE_DEBUG ((LM_ERROR,
                        ACE_TEXT ("%s: failed to ACE_Task_Base::svc(): \"%m\", continuing\n"),
                        inherited2::mod_->name ()));
          result = close (0);
          if (result == -1)
            ACE_DEBUG ((LM_ERROR,
                        ACE_TEXT ("%s: failed to ACE_Task_Base::close(): \"%m\", continuing\n"),
                        inherited2::mod_->name ()));

          break;
        }
        case STREAM_HEADMODULECONCURRENCY_CONCURRENT:
        {
          // *IMPORTANT NOTE*: this means that there is no worker thread
          //                   driving this module (neither in-line, nor
          //                   dedicated, svc() is not used); data is processed
          //                   in-line in put() by dispatching threads

          // *NOTE*: if any of the modules failed to initialize, signal the
          //         controller

          // *NOTE*: in 'concurrent' (server-side-)scenarios there is a race
          //         condition when the connection is close()d asynchronously
          //         --> see below: line 2015
          bool release_lock = false;
          // sanity check(s)
          ACE_ASSERT (streamLock_);

          try {
            release_lock = streamLock_->lock (true);
          } catch (...) {
            ACE_DEBUG ((LM_ERROR,
                        ACE_TEXT ("%s: caught exception in Stream_ILock_T::lock(true), continuing\n"),
                        inherited2::mod_->name ()));
          }

          SessionDataContainerType* session_data_container_p =
            inherited2::sessionData_;
          if (session_data_container_p)
          {
            session_data_container_p->increase ();

            // *TODO*: remove type inferences
            SessionDataType& session_data_r =
                const_cast<SessionDataType&> (session_data_container_p->get ());

            { ACE_GUARD (ACE_SYNCH_MUTEX, aGuard_2, *session_data_r.lock);

              if (session_data_r.aborted)
                this->finished ();
            } // end lock scope

            session_data_container_p->decrease ();
          } // end IF

          if (release_lock)
          {
            try {
              streamLock_->unlock (false);
            } catch (...) {
              ACE_DEBUG ((LM_ERROR,
                          ACE_TEXT ("%s: caught exception in Stream_ILock_T::unlock(false), continuing\n"),
                          inherited2::mod_->name ()));
            }
          } // end IF

          break;
        }
        default:
          break;
      } // end SWITCH

      break;
    }
    case STREAM_STATE_PAUSED:
    {
      // suspend the worker(s) ?
      switch (concurrency_)
      {
        case STREAM_HEADMODULECONCURRENCY_ACTIVE:
        {
          result = inherited2::suspend ();
          if (result == -1)
            ACE_DEBUG ((LM_ERROR,
                        ACE_TEXT ("failed to ACE_Task::suspend(): \"%m\", continuing\n")));

          break;
        } // end IF
        case STREAM_HEADMODULECONCURRENCY_PASSIVE:
        {
          ACE_GUARD (ACE_SYNCH_MUTEX, aGuard_2, inherited2::lock_);

          // task object not active --> suspend the borrowed thread
          ACE_hthread_t handle = inherited2::threadIDs_[0].handle ();
#if defined (ACE_WIN32) || defined (ACE_WIN64)
          ACE_ASSERT (handle != ACE_INVALID_HANDLE);
#else
          ACE_ASSERT (static_cast<int> (handle) != ACE_INVALID_HANDLE);
#endif
          result = ACE_Thread::suspend (handle);
          if (result == -1)
            ACE_DEBUG ((LM_ERROR,
                        ACE_TEXT ("failed to ACE_Thread::suspend(): \"%m\", continuing\n")));

          break;
        }
        default:
          break;
      } // end SWITCH

      break;
    }
    case STREAM_STATE_STOPPED:
    {
      bool done = false;

      { ACE_GUARD (ACE_SYNCH_MUTEX_T, aGuard, *inherited::stateLock_);

        switch (inherited::state_)
        {
          case STREAM_STATE_PAUSED:
          {
            // resume worker ?
            switch (concurrency_)
            {
              case STREAM_HEADMODULECONCURRENCY_ACTIVE:
              {
                result = inherited2::resume ();
                if (result == -1)
                  ACE_DEBUG ((LM_ERROR,
                              ACE_TEXT ("failed to ACE_Task::resume(): \"%m\", continuing\n")));

                break;
              } // end IF
              case STREAM_HEADMODULECONCURRENCY_PASSIVE:
              {
                ACE_GUARD (ACE_SYNCH_MUTEX, aGuard_2, inherited2::lock_);

                // task is not 'active' --> resume the calling thread (i.e. the
                // thread that invoked start())
                ACE_hthread_t handle = inherited2::threadIDs_[0].handle ();
#if defined (ACE_WIN32) || defined (ACE_WIN64)
                ACE_ASSERT (handle != ACE_INVALID_HANDLE);
#else
                ACE_ASSERT (static_cast<int> (handle) != ACE_INVALID_HANDLE);
#endif
                result = ACE_Thread::resume (handle);
                if (result == -1)
                  ACE_DEBUG ((LM_ERROR,
                              ACE_TEXT ("failed to ACE_Thread::resume(): \"%m\", continuing\n")));

                break;
              }
              default:
                break;
            } // end SWITCH

            break;
          }
          case STREAM_STATE_FINISHED:
          {
            done = true;
            break;
          }
          default:
            break;
        } // end SWITCH
        if (done) break;

        // *NOTE*: in 'passive' mode the finished() method waits for the stream
        //         --> set the (intermediate) state early
        inherited::state_ = STREAM_STATE_STOPPED;
      } // end lock scope

      switch (concurrency_)
      {
        case STREAM_HEADMODULECONCURRENCY_ACTIVE:
        {
          inherited2::stop (false,  // wait ?
                            false); // N/A
          break;
        }
        case STREAM_HEADMODULECONCURRENCY_PASSIVE:
        {
          ACE_GUARD (ACE_SYNCH_MUTEX, aGuard_2, inherited2::lock_);

          if (!ACE_OS::thr_equal (ACE_OS::thr_self (),
                                  inherited2::threadIDs_[0].id ()))
            inherited2::stop (false,  // wait ?
                              false); // N/A
          break;
        }
        case STREAM_HEADMODULECONCURRENCY_CONCURRENT:
        {
          // signal the controller
          this->finished ();
          break;
        }
        default:
          break;
      } // end SWITCH

      break;
    }
    case STREAM_STATE_FINISHED:
    {
      { ACE_GUARD (ACE_SYNCH_MUTEX_T, aGuard, *inherited::stateLock_);

        // *NOTE*: modules processing the final session message (see below) may
        //         (indirectly) invoke wait() on the stream,
        //         which would deadlock if the implementation is 'passive'
        //         --> set the state early
        // *TODO*: this may not be the best way to handle that case (i.e. it
        //         could introduce other race conditions...)
        inherited::state_ = STREAM_STATE_FINISHED;
      } // end lock scope

      // send final session message downstream ?
      // *IMPORTANT NOTE*: the transition STOPPED --> FINISHED is automatic (see
      //                   above [*NOTE*: in 'active'/svc() based scenarios,
      //                   shutdown() triggers this transition]).
      //                   However, as the stream may be stop()/finished()-ed
      //                   concurrently (e.g. (safety/sanity) precaution during
      //                   shutdown, connection reset, ...), this transition
      //                   could trigger several times
      //                   --> ensure that only a single 'session end' message
      //                       is generated per session
      bool send_end_message = false;
      { ACE_GUARD (ACE_SYNCH_MUTEX, aGuard, inherited2::lock_);

        if (!sessionEndSent_ &&
            !sessionEndProcessed_) // already processed upstream 'session end' ?
        {
          sessionEndSent_ = true;
          send_end_message = true;
        } // end IF
      } // end lock scope
      if (generateSessionMessages_ &&
          send_end_message)
      {
        // *NOTE*: in inbound ('concurrent') scenarios, there is a race
        //         condition when the connection is close()d locally or reset by
        //         the peer:
        //         [- event dispatch: notify(STREAM_SESSION_MESSAGE_DISCONNECT)]
        //         - event dispatch: finished() --> STREAM_SESSION_MESSAGE_END
        //           --> this frees the session data, possibly prematurely
        //         - event dispatch: wait()
        //         --> grab the 'stream lock' while processing
        //             STREAM_SESSION_MESSAGE_END, and make sure all OOB access
        //             to the session data (see above) acquires the 'stream
        //             lock' first
        // *TODO*: prevent session messages being enqueued without valid data in
        //         this case
        bool release_lock = false;
        if (concurrency_ == STREAM_HEADMODULECONCURRENCY_CONCURRENT)
        {
          // sanity check(s)
          ACE_ASSERT (streamLock_);

          try {
            release_lock = streamLock_->lock (true);
          } catch (...) {
            ACE_DEBUG ((LM_ERROR,
                        ACE_TEXT ("%s: caught exception in Stream_ILock_T::lock(true), continuing\n"),
                        inherited2::mod_->name ()));
          }
        } // end IF

        SessionDataContainerType* session_data_container_p =
          inherited2::sessionData_;
        if (session_data_container_p)
          session_data_container_p->increase ();

        if (!inherited2::putSessionMessage (STREAM_SESSION_MESSAGE_END,                      // session message type
                                            session_data_container_p,                        // session data
                                            (streamState_ ? streamState_->userData : NULL))) // user data handle
        {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("failed to Stream_TaskBase_T::putSessionMessage(STREAM_SESSION_MESSAGE_END), continuing\n")));

          // clean up
          if (session_data_container_p)
            session_data_container_p->decrease ();
        } // end IF

        if (release_lock)
        {
          try {
            streamLock_->unlock (false);
          } catch (...) {
            ACE_DEBUG ((LM_ERROR,
                        ACE_TEXT ("%s: caught exception in Stream_ILock_T::unlock(false), continuing\n"),
                        inherited2::mod_->name ()));
          }
        } // end IF
      } // end IF

//       ACE_DEBUG ((LM_DEBUG,
//                   ACE_TEXT ("stream processing complete\n")));

      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid state transition: \"%s\" --> \"%s\", continuing\n"),
                  ACE_TEXT (inherited::state2String (inherited::state_).c_str ()),
                  ACE_TEXT (inherited::state2String (newState_in).c_str ())));
      break;
    }
  } // end SWITCH
}
