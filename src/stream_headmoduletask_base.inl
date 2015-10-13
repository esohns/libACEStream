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

#include "ace/Guard_T.h"
#include "ace/Log_Msg.h"
#include "ace/Message_Block.h"

#include "common_defines.h"
#include "common_timer_manager.h"
#include "common_tools.h"

#include "stream_defines.h"
#include "stream_iallocator.h"
#include "stream_macros.h"

template <typename TaskSynchType,
          typename TimePolicyType,
          typename SessionMessageType,
          typename ProtocolMessageType,
          typename ConfigurationType,
          typename StreamStateType,
          typename SessionDataType,
          typename SessionDataContainerType>
Stream_HeadModuleTaskBase_T<TaskSynchType,
                            TimePolicyType,
                            SessionMessageType,
                            ProtocolMessageType,
                            ConfigurationType,
                            StreamStateType,
                            SessionDataType,
                            SessionDataContainerType>::Stream_HeadModuleTaskBase_T (bool isActive_in,
                                                                                    bool autoStart_in,
                                                                                    bool runSvcRoutineOnStart_in)
 : configuration_ ()
// , isActive_ (isActive_in)
 , sessionData_ (NULL)
 , streamState_ (NULL)
 , autoStart_ (autoStart_in)
 , sessionEndSent_ (false)
 , runSvcRoutineOnStart_ (runSvcRoutineOnStart_in)
 , threadID_ ()
{
  STREAM_TRACE (ACE_TEXT ("Stream_HeadModuleTaskBase_T::Stream_HeadModuleTaskBase_T"));

  // *TODO*: remove type inference
  configuration_.active = isActive_in;

  // set group ID for worker thread(s)
  inherited2::grp_id (STREAM_MODULE_TASK_GROUP_ID);
}

template <typename TaskSynchType,
          typename TimePolicyType,
          typename SessionMessageType,
          typename ProtocolMessageType,
          typename ConfigurationType,
          typename StreamStateType,
          typename SessionDataType,
          typename SessionDataContainerType>
Stream_HeadModuleTaskBase_T<TaskSynchType,
                            TimePolicyType,
                            SessionMessageType,
                            ProtocolMessageType,
                            ConfigurationType,
                            StreamStateType,
                            SessionDataType,
                            SessionDataContainerType>::~Stream_HeadModuleTaskBase_T ()
{
  STREAM_TRACE (ACE_TEXT ("Stream_HeadModuleTaskBase_T::~Stream_HeadModuleTaskBase_T"));

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  if (threadID_.handle != ACE_INVALID_HANDLE)
    if (!::CloseHandle (threadID_.handle))
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to CloseHandle(0x%@): \"%s\", continuing\n"),
                  threadID_.handle,
                  ACE_TEXT (Common_Tools::error2String (GetLastError ()).c_str ())));
#endif
}

template <typename TaskSynchType,
          typename TimePolicyType,
          typename SessionMessageType,
          typename ProtocolMessageType,
          typename ConfigurationType,
          typename StreamStateType,
          typename SessionDataType,
          typename SessionDataContainerType>
int
Stream_HeadModuleTaskBase_T<TaskSynchType,
                            TimePolicyType,
                            SessionMessageType,
                            ProtocolMessageType,
                            ConfigurationType,
                            StreamStateType,
                            SessionDataType,
                            SessionDataContainerType>::put (ACE_Message_Block* messageBlock_in,
                                                            ACE_Time_Value* timeout_in)
{
  STREAM_TRACE (ACE_TEXT ("Stream_HeadModuleTaskBase_T::put"));

  int result = -1;

  // if active, simply drop the message into the queue
  // *TODO*: remove type inference
  if (configuration_.active)
  {
    result = inherited2::putq (messageBlock_in, timeout_in);
    if (result == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_Task::putq(): \"%m\", aborting\n")));
    return result;
  } // end IF

  // otherwise, process manually
  bool stop_processing = false;
  inherited2::handleMessage (messageBlock_in,
                             stop_processing);

  //return (stop_processing ? -1 : 0);
  return 0;
}

template <typename TaskSynchType,
          typename TimePolicyType,
          typename SessionMessageType,
          typename ProtocolMessageType,
          typename ConfigurationType,
          typename StreamStateType,
          typename SessionDataType,
          typename SessionDataContainerType>
int
Stream_HeadModuleTaskBase_T<TaskSynchType,
                            TimePolicyType,
                            SessionMessageType,
                            ProtocolMessageType,
                            ConfigurationType,
                            StreamStateType,
                            SessionDataType,
                            SessionDataContainerType>::open (void* arg_in)
{
  STREAM_TRACE (ACE_TEXT ("Stream_HeadModuleTaskBase_T::open"));

  int result = -1;

  //// step-1: *WORKAROUND*: for some odd reason, inherited::next_ is not updated
  ////                       correctly
  //if (inherited::mod_)
  //{
  //  Stream_Module_t* module_p = inherited::mod_->next ();
  //  if (module_p)
  //    inherited::next_ = module_p->writer ();
  //} // end IF

  // step0: initialize this
  sessionData_ = reinterpret_cast<SessionDataType*> (arg_in);

  // step1: (re-)activate() the message queue
  // *NOTE*: the first time around, the queue will have been open()ed
  //         from within the default ctor; this sets it into an ACTIVATED state
  //         The second time around (i.e. the stream has been stopped/started,
  //         the queue will have been deactivate()d in the process, and getq()
  //         (see svc()) would fail (ESHUTDOWN)
  //         --> (re-)activate() the queue here !
  // step1: (re-)activate() message queue
  result = inherited2::queue_.activate ();
  if (result == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Message_Queue::activate(): \"%m\", aborting\n")));
    return -1;
  } // end IF

  // standard usecase: being implicitly invoked by ACE_Stream::push()...
  // --> don't do anything, unless auto-starting
  if (autoStart_)
  {
    if (inherited2::module ())
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("auto-starting \"%s\"...\n"),
                  inherited2::name ()));
    else
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("auto-starting...\n")));

    try
    {
      start ();
    }
    catch (...)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("caught exception in Stream_IStreamControl_T::start(), aborting\n")));
      return -1;
    }
  } // end IF

  return 0;
}

template <typename TaskSynchType,
          typename TimePolicyType,
          typename SessionMessageType,
          typename ProtocolMessageType,
          typename ConfigurationType,
          typename StreamStateType,
          typename SessionDataType,
          typename SessionDataContainerType>
int
Stream_HeadModuleTaskBase_T<TaskSynchType,
                            TimePolicyType,
                            SessionMessageType,
                            ProtocolMessageType,
                            ConfigurationType,
                            StreamStateType,
                            SessionDataType,
                            SessionDataContainerType>::close (u_long arg_in)
{
  STREAM_TRACE (ACE_TEXT ("Stream_HeadModuleTaskBase_T::close"));

  // *NOTE*: this method may be invoked
  // - by an external thread closing down the active object
  //    --> should NEVER happen as a module !
  // - by the worker thread which calls this after returning from svc()
  //    --> in this case, this should be a NOP...
  switch (arg_in)
  {
    // called from ACE_Task_Base on clean-up
    case 0:
    {
//       if (inherited::module ())
//       {
//         ACE_DEBUG ((LM_DEBUG,
//                     ACE_TEXT ("\"%s\" worker thread (ID: %t) leaving...\n"),
//                     ACE_TEXT (inherited::name ())));
//       } // end IF
//       else
//       {
//         ACE_DEBUG ((LM_DEBUG,
//                     ACE_TEXT ("worker thread (ID: %t) leaving...\n")));
//       } // end ELSE

      break;
    }
    case 1:
    {
      // *WARNING*: SHOULD NEVER GET HERE
      // --> refer to module_closed () hook
      ACE_ASSERT (false);

      ACE_NOTSUP_RETURN (-1);
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

template <typename TaskSynchType,
          typename TimePolicyType,
          typename SessionMessageType,
          typename ProtocolMessageType,
          typename ConfigurationType,
          typename StreamStateType,
          typename SessionDataType,
          typename SessionDataContainerType>
int
Stream_HeadModuleTaskBase_T<TaskSynchType,
                            TimePolicyType,
                            SessionMessageType,
                            ProtocolMessageType,
                            ConfigurationType,
                            StreamStateType,
                            SessionDataType,
                            SessionDataContainerType>::module_closed (void)
{
  STREAM_TRACE (ACE_TEXT ("Stream_HeadModuleTaskBase_T::module_closed"));

  // *NOTE*: this will be a NOP IF the stream was
  //         stop()ped BEFORE it is deleted !

  // *NOTE*: this method is invoked by an external thread either:
  //         - from the ACE_Module dtor OR
  //         - during explicit ACE_Module::close()

  // *NOTE*: when control flow gets here, the stream SHOULD (!) already be in a
  //         final state...

  // sanity check
  // *WARNING*: this check CAN NOT prevent a potential race condition...
  if (isRunning ())
  {
    // *NOTE*: MAY happen after application receives a SIGINT
    //         select() returns -1, reactor invokes:
    //         remove_handler --> remove_reference --> delete this
    ACE_DEBUG ((LM_WARNING,
                ACE_TEXT ("stream still active in module_closed() --> check implementation !, continuing\n")));
    stop ();
  } // end IF

  return 0;
}

template <typename TaskSynchType,
          typename TimePolicyType,
          typename SessionMessageType,
          typename ProtocolMessageType,
          typename ConfigurationType,
          typename StreamStateType,
          typename SessionDataType,
          typename SessionDataContainerType>
int
Stream_HeadModuleTaskBase_T<TaskSynchType,
                            TimePolicyType,
                            SessionMessageType,
                            ProtocolMessageType,
                            ConfigurationType,
                            StreamStateType,
                            SessionDataType,
                            SessionDataContainerType>::svc (void)
{
  STREAM_TRACE (ACE_TEXT ("Stream_HeadModuleTaskBase_T::svc"));

  int result = -1;
  ACE_Message_Block* message_block_p = NULL;
  bool               stop_processing = false;

  // step0: increment thread count
  ++inherited2::thr_count_;

  // step1: start processing incoming data...
//   ACE_DEBUG ((LM_DEBUG,
//               ACE_TEXT ("entering processing loop...\n")));

  while (inherited2::getq (message_block_p,
                           NULL) != -1)
  {
    inherited2::handleMessage (message_block_p,
                               stop_processing);

    // finished ?
    if (stop_processing)
    {
      // *IMPORTANT NOTE*: message_block_p has already been released() !

//       ACE_DEBUG ((LM_DEBUG,
//                   ACE_TEXT ("leaving processing loop...\n")));

      result = 0;

      goto session_finished;
    } // end IF

    // clean up
    message_block_p = NULL;
  } // end WHILE
  result = -1;

  ACE_DEBUG ((LM_ERROR,
              ACE_TEXT ("worker thread (ID: %t) failed to ACE_Task::getq(): \"%m\", aborting\n")));

session_finished:
  // step2: send final session message downstream...
  if (!putSessionMessage (STREAM_SESSION_END,
                          sessionData_,
                          false))
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("putSessionMessage(SESSION_END) failed, continuing\n")));

  // signal the controller
  finished ();

  // decrement thread count
  --inherited2::thr_count_;

  return result;
}

template <typename TaskSynchType,
          typename TimePolicyType,
          typename SessionMessageType,
          typename ProtocolMessageType,
          typename ConfigurationType,
          typename StreamStateType,
          typename SessionDataType,
          typename SessionDataContainerType>
void
Stream_HeadModuleTaskBase_T<TaskSynchType,
                            TimePolicyType,
                            SessionMessageType,
                            ProtocolMessageType,
                            ConfigurationType,
                            StreamStateType,
                            SessionDataType,
                            SessionDataContainerType>::waitForIdleState () const
{
  STREAM_TRACE (ACE_TEXT ("Stream_HeadModuleTaskBase_T::waitForIdleState"));

  // delegate this to the queue
  try
  {
    inherited2::queue_.waitForIdleState ();
  }
  catch (...)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("caught exception in Stream_IMessageQueue::waitForIdleState, continuing\n")));
  }
}

template <typename TaskSynchType,
          typename TimePolicyType,
          typename SessionMessageType,
          typename ProtocolMessageType,
          typename ConfigurationType,
          typename StreamStateType,
          typename SessionDataType,
          typename SessionDataContainerType>
void
Stream_HeadModuleTaskBase_T<TaskSynchType,
                            TimePolicyType,
                            SessionMessageType,
                            ProtocolMessageType,
                            ConfigurationType,
                            StreamStateType,
                            SessionDataType,
                            SessionDataContainerType>::handleControlMessage (ACE_Message_Block* controlMessage_in,
                                                                             bool& stopProcessing_out,
                                                                             bool& passMessageDownstream_out)
{
  STREAM_TRACE (ACE_TEXT ("Stream_HeadModuleTaskBase_T::handleControlMessage"));

  // initialize return value(s)
  stopProcessing_out = false;

  switch (controlMessage_in->msg_type ())
  {
    case ACE_Message_Block::MB_STOP:
    {
//      ACE_DEBUG ((LM_DEBUG,
//                  ACE_TEXT ("received MB_STOP message, shutting down...\n")));

      // clean up
      passMessageDownstream_out = false;
      controlMessage_in->release ();

      // *NOTE*: forward a SESSION_END message to notify any modules downstream
      stopProcessing_out = true;

      break;
    }
    default:
    {
      // ...otherwise, behave like a regular module...
      inherited2::handleControlMessage (controlMessage_in,
                                        stopProcessing_out,
                                        passMessageDownstream_out);

      break;
    }
  } // end SWITCH
}

template <typename TaskSynchType,
          typename TimePolicyType,
          typename SessionMessageType,
          typename ProtocolMessageType,
          typename ConfigurationType,
          typename StreamStateType,
          typename SessionDataType,
          typename SessionDataContainerType>
void
Stream_HeadModuleTaskBase_T<TaskSynchType,
                            TimePolicyType,
                            SessionMessageType,
                            ProtocolMessageType,
                            ConfigurationType,
                            StreamStateType,
                            SessionDataType,
                            SessionDataContainerType>::handleDataMessage (ProtocolMessageType*& message_inout,
                                                                          bool& passMessageDownstream_out)
{
  STREAM_TRACE (ACE_TEXT ("Stream_HeadModuleTaskBase_T::handleDataMessage"));

  ACE_UNUSED_ARG (message_inout);
  ACE_UNUSED_ARG (passMessageDownstream_out);
}

template <typename TaskSynchType,
          typename TimePolicyType,
          typename SessionMessageType,
          typename ProtocolMessageType,
          typename ConfigurationType,
          typename StreamStateType,
          typename SessionDataType,
          typename SessionDataContainerType>
const ConfigurationType&
Stream_HeadModuleTaskBase_T<TaskSynchType,
                            TimePolicyType,
                            SessionMessageType,
                            ProtocolMessageType,
                            ConfigurationType,
                            StreamStateType,
                            SessionDataType,
                            SessionDataContainerType>::get () const
{
  STREAM_TRACE (ACE_TEXT ("Stream_HeadModuleTaskBase_T::get"));

  return configuration_;
}
template <typename TaskSynchType,
          typename TimePolicyType,
          typename SessionMessageType,
          typename ProtocolMessageType,
          typename ConfigurationType,
          typename StreamStateType,
          typename SessionDataType,
          typename SessionDataContainerType>
bool
Stream_HeadModuleTaskBase_T<TaskSynchType,
                            TimePolicyType,
                            SessionMessageType,
                            ProtocolMessageType,
                            ConfigurationType,
                            StreamStateType,
                            SessionDataType,
                            SessionDataContainerType>::initialize (const ConfigurationType& configuration_in)
{
  STREAM_TRACE (ACE_TEXT ("Stream_HeadModuleTaskBase_T::initialize"));

  configuration_ = configuration_in;

//  inherited::change (STREAM_STATE_INITIALIZED);

  return true;
}

//template <typename TaskSynchType,
//          typename TimePolicyType,
//          typename SessionMessageType,
//          typename ProtocolMessageType,
//          typename ConfigurationType,
//          typename StreamStateType,
//          typename SessionDataType,
//          typename SessionDataContainerType>
//void
//Stream_HeadModuleTaskBase_T<TaskSynchType,
//                            TimePolicyType,
//                            SessionMessageType,
//                            ProtocolMessageType,
//                            ConfigurationType,
//                            StreamStateType,
//                            SessionDataType,
//                            SessionDataContainerType>::initialize ()
//{
//  STREAM_TRACE (ACE_TEXT ("Stream_HeadModuleTaskBase_T::initialize"));

//  ACE_ASSERT (false);
//  ACE_NOTSUP;
//  ACE_NOTREACHED (return;)
//}

template <typename TaskSynchType,
          typename TimePolicyType,
          typename SessionMessageType,
          typename ProtocolMessageType,
          typename ConfigurationType,
          typename StreamStateType,
          typename SessionDataType,
          typename SessionDataContainerType>
void
Stream_HeadModuleTaskBase_T<TaskSynchType,
                            TimePolicyType,
                            SessionMessageType,
                            ProtocolMessageType,
                            ConfigurationType,
                            StreamStateType,
                            SessionDataType,
                            SessionDataContainerType>::start ()
{
  STREAM_TRACE (ACE_TEXT ("Stream_HeadModuleTaskBase_T::start"));

  // *TODO*: remove type inferences
  if (sessionData_)
    sessionData_->startOfSession = COMMON_TIME_NOW;

  // --> start a worker thread, if active
  inherited::change (STREAM_STATE_RUNNING);
}

template <typename TaskSynchType,
          typename TimePolicyType,
          typename SessionMessageType,
          typename ProtocolMessageType,
          typename ConfigurationType,
          typename StreamStateType,
          typename SessionDataType,
          typename SessionDataContainerType>
void
Stream_HeadModuleTaskBase_T<TaskSynchType,
                            TimePolicyType,
                            SessionMessageType,
                            ProtocolMessageType,
                            ConfigurationType,
                            StreamStateType,
                            SessionDataType,
                            SessionDataContainerType>::stop (bool waitForCompletion_in,
                                                             bool lockedAccess_in)
{
  STREAM_TRACE (ACE_TEXT ("Stream_HeadModuleTaskBase_T::stop"));

  ACE_UNUSED_ARG (lockedAccess_in);

  // (try to) change state
  inherited::change (STREAM_STATE_STOPPED);

  if (waitForCompletion_in)
    waitForCompletion ();
}

template <typename TaskSynchType,
          typename TimePolicyType,
          typename SessionMessageType,
          typename ProtocolMessageType,
          typename ConfigurationType,
          typename StreamStateType,
          typename SessionDataType,
          typename SessionDataContainerType>
bool
Stream_HeadModuleTaskBase_T<TaskSynchType,
                            TimePolicyType,
                            SessionMessageType,
                            ProtocolMessageType,
                            ConfigurationType,
                            StreamStateType,
                            SessionDataType,
                            SessionDataContainerType>::isRunning () const
{
  STREAM_TRACE (ACE_TEXT ("Stream_HeadModuleTaskBase_T::isRunning"));

  const Stream_StateMachine_ControlState& status_r = inherited::current ();
  return ((status_r == STREAM_STATE_PAUSED) ||
          (status_r == STREAM_STATE_RUNNING));
}

template <typename TaskSynchType,
          typename TimePolicyType,
          typename SessionMessageType,
          typename ProtocolMessageType,
          typename ConfigurationType,
          typename StreamStateType,
          typename SessionDataType,
          typename SessionDataContainerType>
void
Stream_HeadModuleTaskBase_T<TaskSynchType,
                            TimePolicyType,
                            SessionMessageType,
                            ProtocolMessageType,
                            ConfigurationType,
                            StreamStateType,
                            SessionDataType,
                            SessionDataContainerType>::flush (bool /* flushUpStream_in */)
{
  STREAM_TRACE (ACE_TEXT ("Stream_HeadModuleTaskBase_T::flush"));

  ACE_ASSERT (false);
  ACE_NOTSUP;
  ACE_NOTREACHED (return;)
}

template <typename TaskSynchType,
          typename TimePolicyType,
          typename SessionMessageType,
          typename ProtocolMessageType,
          typename ConfigurationType,
          typename StreamStateType,
          typename SessionDataType,
          typename SessionDataContainerType>
void
Stream_HeadModuleTaskBase_T<TaskSynchType,
                            TimePolicyType,
                            SessionMessageType,
                            ProtocolMessageType,
                            ConfigurationType,
                            StreamStateType,
                            SessionDataType,
                            SessionDataContainerType>::pause ()
{
  STREAM_TRACE (ACE_TEXT ("Stream_HeadModuleTaskBase_T::pause"));

  // (try to) change state
  inherited::change (STREAM_STATE_PAUSED);
}

template <typename TaskSynchType,
          typename TimePolicyType,
          typename SessionMessageType,
          typename ProtocolMessageType,
          typename ConfigurationType,
          typename StreamStateType,
          typename SessionDataType,
          typename SessionDataContainerType>
void
Stream_HeadModuleTaskBase_T<TaskSynchType,
                            TimePolicyType,
                            SessionMessageType,
                            ProtocolMessageType,
                            ConfigurationType,
                            StreamStateType,
                            SessionDataType,
                            SessionDataContainerType>::rewind ()
{
  STREAM_TRACE (ACE_TEXT ("Stream_HeadModuleTaskBase_T::rewind"));

  ACE_ASSERT (false);
  ACE_NOTSUP;

  ACE_NOTREACHED (return;)
}

template <typename TaskSynchType,
          typename TimePolicyType,
          typename SessionMessageType,
          typename ProtocolMessageType,
          typename ConfigurationType,
          typename StreamStateType,
          typename SessionDataType,
          typename SessionDataContainerType>
void
Stream_HeadModuleTaskBase_T<TaskSynchType,
                            TimePolicyType,
                            SessionMessageType,
                            ProtocolMessageType,
                            ConfigurationType,
                            StreamStateType,
                            SessionDataType,
                            SessionDataContainerType>::upstream (Stream_Base_t* upStream_in)
{
  STREAM_TRACE (ACE_TEXT ("Stream_HeadModuleTaskBase_T::upstream"));

  ACE_UNUSED_ARG (upStream_in);

  ACE_ASSERT (false);
  ACE_NOTSUP;

  ACE_NOTREACHED (return;)
}
template <typename TaskSynchType,
          typename TimePolicyType,
          typename SessionMessageType,
          typename ProtocolMessageType,
          typename ConfigurationType,
          typename StreamStateType,
          typename SessionDataType,
          typename SessionDataContainerType>
Stream_Base_t*
Stream_HeadModuleTaskBase_T<TaskSynchType,
                            TimePolicyType,
                            SessionMessageType,
                            ProtocolMessageType,
                            ConfigurationType,
                            StreamStateType,
                            SessionDataType,
                            SessionDataContainerType>::upstream () const
{
  STREAM_TRACE (ACE_TEXT ("Stream_HeadModuleTaskBase_T::upstream"));

  ACE_ASSERT (false);
  ACE_NOTSUP_RETURN (NULL);

  ACE_NOTREACHED (return NULL;)
}

template <typename TaskSynchType,
          typename TimePolicyType,
          typename SessionMessageType,
          typename ProtocolMessageType,
          typename ConfigurationType,
          typename StreamStateType,
          typename SessionDataType,
          typename SessionDataContainerType>
const Stream_StateMachine_ControlState&
Stream_HeadModuleTaskBase_T<TaskSynchType,
                            TimePolicyType,
                            SessionMessageType,
                            ProtocolMessageType,
                            ConfigurationType,
                            StreamStateType,
                            SessionDataType,
                            SessionDataContainerType>::status () const
{
  STREAM_TRACE (ACE_TEXT ("Stream_HeadModuleTaskBase_T::status"));

  Stream_StateMachine_ControlState result = inherited::current ();

  return result;
}

template <typename TaskSynchType,
          typename TimePolicyType,
          typename SessionMessageType,
          typename ProtocolMessageType,
          typename ConfigurationType,
          typename StreamStateType,
          typename SessionDataType,
          typename SessionDataContainerType>
void
Stream_HeadModuleTaskBase_T<TaskSynchType,
                            TimePolicyType,
                            SessionMessageType,
                            ProtocolMessageType,
                            ConfigurationType,
                            StreamStateType,
                            SessionDataType,
                            SessionDataContainerType>::waitForCompletion (bool waitForThreads_in,
                                                                          bool waitForUpStream_in)
{
  STREAM_TRACE (ACE_TEXT ("Stream_HeadModuleTaskBase_T::waitForCompletion"));

  ACE_UNUSED_ARG (waitForUpStream_in);

  int result = -1;

  // step1: wait for final state
  inherited::wait (NULL); // <-- block

  // step2: wait for worker(s) to join ?
  if (waitForThreads_in)
  {
    ACE_thread_t thread_id = ACE_Thread::self ();

    // *TODO*: remove type inference
    if (configuration_.active ||
        (runSvcRoutineOnStart_ && !ACE_OS::thr_equal (thread_id,
                                                      threadID_.id)))
    {
      if (configuration_.active)
      {
        result = inherited2::wait ();
        if (result == -1)
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("failed to ACE_Task_Base::wait(): \"%m\", continuing\n")));
      } // end IF
      else
      {
        ACE_Guard<ACE_SYNCH_MUTEX> aGuard (inherited2::lock_);

        thread_id = threadID_.id;
        ACE_THR_FUNC_RETURN status;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
        if (threadID_.handle != ACE_INVALID_HANDLE)
        {
          result = ACE_Thread::join (threadID_.handle, &status);
          // *NOTE*: successful join()s close the thread handle
          //         (see OS_NS_Thread.inl:2971)
          if (result == 0) threadID_.handle = ACE_INVALID_HANDLE;
          threadID_.id = std::numeric_limits<DWORD>::max ();
        } // end IF
        else
          result = 0;
#else
        if (threadID_.id != -1)
        {
          result = ACE_Thread::join (threadID_.id, NULL, &status);
          threadID_.id = -1;
        } // end IF
        else
          result = 0;
#endif
        if (result == -1)
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("failed to ACE_Thread::join(%d): \"%m\", continuing\n"),
                      thread_id));
      } // end IF
    } // end IF
  } // end IF
}

template <typename TaskSynchType,
          typename TimePolicyType,
          typename SessionMessageType,
          typename ProtocolMessageType,
          typename ConfigurationType,
          typename StreamStateType,
          typename SessionDataType,
          typename SessionDataContainerType>
std::string
Stream_HeadModuleTaskBase_T<TaskSynchType,
                            TimePolicyType,
                            SessionMessageType,
                            ProtocolMessageType,
                            ConfigurationType,
                            StreamStateType,
                            SessionDataType,
                            SessionDataContainerType>::name () const
{
  STREAM_TRACE (ACE_TEXT ("Stream_HeadModuleTaskBase_T::name"));

  std::string result = ACE_TEXT_ALWAYS_CHAR (inherited2::name ());
  return result;
}

template <typename TaskSynchType,
          typename TimePolicyType,
          typename SessionMessageType,
          typename ProtocolMessageType,
          typename ConfigurationType,
          typename StreamStateType,
          typename SessionDataType,
          typename SessionDataContainerType>
const StreamStateType&
Stream_HeadModuleTaskBase_T<TaskSynchType,
                            TimePolicyType,
                            SessionMessageType,
                            ProtocolMessageType,
                            ConfigurationType,
                            StreamStateType,
                            SessionDataType,
                            SessionDataContainerType>::state () const
{
  STREAM_TRACE (ACE_TEXT ("Stream_HeadModuleTaskBase_T::state"));

  ACE_ASSERT (false);
  ACE_NOTSUP_RETURN (StreamStateType ());

  ACE_NOTREACHED (return StreamStateType ());
}

template <typename TaskSynchType,
          typename TimePolicyType,
          typename SessionMessageType,
          typename ProtocolMessageType,
          typename ConfigurationType,
          typename StreamStateType,
          typename SessionDataType,
          typename SessionDataContainerType>
bool
Stream_HeadModuleTaskBase_T<TaskSynchType,
                            TimePolicyType,
                            SessionMessageType,
                            ProtocolMessageType,
                            ConfigurationType,
                            StreamStateType,
                            SessionDataType,
                            SessionDataContainerType>::initialize (const StreamStateType& streamState_in)
{
  STREAM_TRACE (ACE_TEXT ("Stream_HeadModuleTaskBase_T::initialize"));

  streamState_ = &const_cast<StreamStateType&> (streamState_in);
//  sessionData_ = &const_cast<SessionDataType&> (sessionData_in);

  inherited::change (STREAM_STATE_INITIALIZED);

  return true;
}

//template <typename TaskSynchType,
//          typename TimePolicyType,
//          typename SessionMessageType,
//          typename ProtocolMessageType,
//          typename ConfigurationType,
//          typename StreamStateType,
//          typename SessionDataType,
//          typename SessionDataContainerType>
//void
//Stream_HeadModuleTaskBase_T<TaskSynchType,
//                            TimePolicyType,
//                            SessionMessageType,
//                            ProtocolMessageType,
//                            ConfigurationType,
//                            StreamStateType,
//                            SessionDataType,
//                            SessionDataContainerType>::finished ()
//{
//  STREAM_TRACE (ACE_TEXT ("Stream_HeadModuleTaskBase_T::finished"));
//
//  // (try to) set new state
//  inherited::change (STREAM_STATE_FINISHED);
//}

template <typename TaskSynchType,
          typename TimePolicyType,
          typename SessionMessageType,
          typename ProtocolMessageType,
          typename ConfigurationType,
          typename StreamStateType,
          typename SessionDataType,
          typename SessionDataContainerType>
void
Stream_HeadModuleTaskBase_T<TaskSynchType,
                            TimePolicyType,
                            SessionMessageType,
                            ProtocolMessageType,
                            ConfigurationType,
                            StreamStateType,
                            SessionDataType,
                            SessionDataContainerType>::onChange (Stream_StateType_t newState_in)
{
  STREAM_TRACE (ACE_TEXT ("Stream_HeadModuleTaskBase_T::onChange"));

  int result = -1;

  switch (newState_in)
  {
    case STREAM_STATE_INITIALIZED:
    {
      ACE_Guard<ACE_SYNCH_MUTEX> aGuard (inherited2::lock_);

      sessionEndSent_ = false;

#if defined (ACE_WIN32) || defined (ACE_WIN64)
      if (threadID_.handle != ACE_INVALID_HANDLE)
        if (!::CloseHandle (threadID_.handle))
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("failed to CloseHandle(0x%@): \"%s\", continuing\n"),
                      threadID_.handle,
                      ACE_TEXT (Common_Tools::error2String (GetLastError ()).c_str ())));
      threadID_.handle = ACE_INVALID_HANDLE;
      threadID_.id = std::numeric_limits<DWORD>::max ();
#else
      threadID_.id = -1;
#endif

      //ACE_DEBUG ((LM_DEBUG,
      //            ACE_TEXT ("\"%s\" head module (re-)initialized...\n"),
      //            inherited2::name ()));

      break;
    }
    case STREAM_STATE_RUNNING:
    {
      // *NOTE*: implement tape-recorder logic:
      //         transition PAUSED --> PAUSED is mapped to PAUSED --> RUNNING
      //         --> check for this condition before doing anything else...
      if (inherited::current () == STREAM_STATE_PAUSED)
      {
        // resume worker ?
        // *TODO*: remove type inference
        if (configuration_.active)
        {
          result = inherited2::resume ();
          if (result == -1)
            ACE_DEBUG ((LM_ERROR,
                        ACE_TEXT ("failed to ACE_Task::resume(): \"%m\", continuing\n")));
        } // end IF
        else
        {
          // task object not active --> resume the borrowed thread
          ACE_ASSERT (threadID_.handle != ACE_INVALID_HANDLE);
          result = ACE_Thread::resume (threadID_.handle);
          if (result == -1)
            ACE_DEBUG ((LM_ERROR,
                        ACE_TEXT ("failed to ACE_Thread::resume(): \"%m\", continuing\n")));
        } // end ELSE

        break;
      } // end IF
      else
      {
        // send initial session message downstream...
        if (!putSessionMessage (STREAM_SESSION_BEGIN, // type
                                sessionData_,         // session data handle
                                false))               // do not delete the session data
        {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("putSessionMessage(SESSION_BEGIN) failed, continuing\n")));
          break;
        } // end IF

        // *TODO*: remove type inference
        if (configuration_.active)
        {
          // OK: start worker
          ACE_hthread_t thread_handles[1];
          thread_handles[0] = 0;
          ACE_thread_t thread_ids[1];
          thread_ids[0] = 0;
          char thread_name[BUFSIZ];
          ACE_OS::memset (thread_name, 0, sizeof (thread_name));
          ACE_OS::strcpy (thread_name, STREAM_MODULE_DEFAULT_HEAD_THREAD_NAME);
          const char* thread_names[1];
          thread_names[0] = thread_name;
          result =
            inherited2::activate ((THR_NEW_LWP      |
                                   THR_JOINABLE     |
                                   THR_INHERIT_SCHED),                // flags
                                  STREAM_MODULE_DEFAULT_HEAD_THREADS, // number of threads
                                  0,                                  // force spawning
                                  ACE_DEFAULT_THREAD_PRIORITY,        // priority
                                  inherited2::grp_id (),              // group id (see above)
                                  NULL,                               // corresp. task --> use 'this'
                                  thread_handles,                     // thread handle(s)
                                  NULL,                               // thread stack(s)
                                  NULL,                               // thread stack size(s)
                                  thread_ids,                         // thread id(s)
                                  thread_names);                      // thread name(s)
          if (result == -1)
          {
            ACE_DEBUG ((LM_ERROR,
                        ACE_TEXT ("failed to ACE_Task_Base::activate(): \"%m\", continuing\n")));
            break;
          } // end IF

          //       if (inherited::module ())
          //         ACE_DEBUG ((LM_DEBUG,
          //                     ACE_TEXT ("module \"%s\" started worker thread (group: %d, id: %u)...\n"),
          //                     ACE_TEXT (inherited::name ()),
          //                     inherited::grp_id (),
          //                     thread_ids[0]));
          //       else
          //         ACE_DEBUG ((LM_DEBUG,
          //                     ACE_TEXT ("started worker thread (group: %d, id: %u)...\n"),
          //                     inherited::grp_id (),
          //                     thread_ids[0]));
        } // end IF
        else if (runSvcRoutineOnStart_)
        {
          {
            ACE_Guard<ACE_SYNCH_RECURSIVE_MUTEX> aGuard (stateLock_);

            // *NOTE*: if the implementation is 'passive', the whole operation
            //         pertaining to newState_in is processed 'inline' by the
            //         calling thread and would complete before the state
            //         actually has been set to 'running'
            //         --> in this case set the state early
            // *TODO*: this may not be the best way to implement that case (i.e. there
            //         could be intermediate states...)
            inherited::state_ = STREAM_STATE_RUNNING;
          } // end lock scope

          {
            ACE_Guard<ACE_SYNCH_MUTEX> aGuard (inherited2::lock_);

            threadID_.id = ACE_Thread::self ();
            ACE_Thread::self (threadID_.handle);
#if defined (ACE_WIN32) || defined (ACE_WIN64)
            HANDLE process_handle = ::GetCurrentProcess ();
            if (!::DuplicateHandle (process_handle,
                                    threadID_.handle,
                                    process_handle,
                                    &threadID_.handle,
                                    0,
                                    FALSE,
                                    DUPLICATE_CLOSE_SOURCE | DUPLICATE_SAME_ACCESS))
              ACE_DEBUG ((LM_ERROR,
                          ACE_TEXT ("failed to DuplicateHandle(0x%@): \"%s\", continuing\n"),
                          threadID_.handle,
                          ACE_TEXT (Common_Tools::error2String (GetLastError ()).c_str ())));
#endif
          } // end lock scope

          result = svc ();
          if (result == -1)
            ACE_DEBUG ((LM_ERROR,
                        ACE_TEXT ("failed to ACE_Task_Base::svc(): \"%m\", continuing\n")));

  //        // send initial session message downstream...
  //        if (!putSessionMessage (STREAM_SESSION_END,
  //                                sessionData_,
  //                                false))
  //        {
  //          ACE_DEBUG ((LM_ERROR,
  //                      ACE_TEXT ("putSessionMessage(SESSION_END) failed, continuing\n")));
  //          break;
  //        } // end IF
        } // end IF
      } // end ELSE

      break;
    }
    case STREAM_STATE_PAUSED:
    {
      // suspend the worker(s) ?

      // *TODO*: remove type inference
      if (configuration_.active)
      {
        result = inherited2::suspend ();
        if (result == -1)
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("failed to ACE_Task::suspend(): \"%m\", continuing\n")));
      } // end IF
      else
      {
        // task object not active --> suspend the borrowed thread
        ACE_ASSERT (threadID_.handle != ACE_INVALID_HANDLE);
        result = ACE_Thread::suspend (threadID_.handle);
        if (result == -1)
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("failed to ACE_Thread::suspend(): \"%m\", continuing\n")));
      } // end ELSE

      break;
    }
    case STREAM_STATE_STOPPED:
    {
      ACE_thread_t thread_id = ACE_Thread::self ();

      // *TODO*: remove type inference
      if (configuration_.active ||
          (runSvcRoutineOnStart_  && !ACE_OS::thr_equal (thread_id,
                                                         threadID_.id)))
      {
        //// OK: drop a control message into the queue...
        //// *TODO*: use ACE_Stream::control() instead ?
        //ACE_Message_Block* message_block_p = NULL;
        //ACE_NEW_NORETURN (message_block_p,
        //                  ACE_Message_Block (0,                                  // size
        //                                     ACE_Message_Block::MB_STOP,         // type
        //                                     NULL,                               // continuation
        //                                     NULL,                               // data
        //                                     NULL,                               // buffer allocator
        //                                     NULL,                               // locking strategy
        //                                     ACE_DEFAULT_MESSAGE_BLOCK_PRIORITY, // priority
        //                                     ACE_Time_Value::zero,               // execution time
        //                                     ACE_Time_Value::max_time,           // deadline time
        //                                     NULL,                               // data block allocator
        //                                     NULL));                             // message allocator
        //if (!message_block_p)
        //{
        //  ACE_DEBUG ((LM_CRITICAL,
        //              ACE_TEXT ("failed to allocate memory: \"%m\", continuing\n")));
        //  break;
        //} // end IF

        //result = inherited2::putq (message_block_p, NULL);
        //if (result == -1)
        //{
        //  ACE_DEBUG ((LM_ERROR,
        //              ACE_TEXT ("failed to ACE_Task::putq(): \"%m\", continuing\n")));

        //  // clean up
        //  message_block_p->release ();
        //} // end IF
        inherited2::shutdown ();
      } // end IF
      else
      {
        //if (runSvcRoutineOnStart_)
        //{
        //  ACE_ASSERT (threadID_ != -1);
        //  result = ACE_Thread::kill (threadID_, SIGKILL);
        //  if (result == -1)
        //    ACE_DEBUG ((LM_ERROR,
        //                ACE_TEXT ("ACE_Thread::kill(%d, \"%S\") failed: \"%m\", continuing\n"),
        //                threadID_, SIGKILL));
        //} // end IF

        {
          ACE_Guard<ACE_SYNCH_RECURSIVE_MUTEX> aGuard (stateLock_);

          // *NOTE*: 'passive' mode; the finished() method waits for the stream
          //         --> set the (intermediate) state early
          inherited::state_ = STREAM_STATE_STOPPED;
        } // end lock scope

        // signal the controller
        finished ();
      } // end ELSE

      break;
    }
    case STREAM_STATE_FINISHED:
    {
      {
        ACE_Guard<ACE_SYNCH_RECURSIVE_MUTEX> aGuard (stateLock_);

        // *NOTE*: modules processing the final session message (see below) may
        //         (indirectly) invoke waitForCompletion() on the stream,
        //         which would deadlock if the implementation is 'passive'
        //         --> set the state early
        // *TODO*: this may not be the best way to handle that case (i.e. it
        //         could introduce race conditions...)
        inherited::state_ = STREAM_STATE_FINISHED;
      } // end lock scope

      // send final session message downstream ?
      // *IMPORTANT NOTE*: as in 'passive' mode the transition STOPPED -->
      //                   FINISHED is automatic (see above), and the stream may
      //                   be stop()ed several times (safety precaution during
      //                   shutdown), this transition could occur several times
      //                   --> ensure that only one (!) session end message is
      //                       generated per session
      {
        ACE_Guard<ACE_SYNCH_MUTEX> aGuard (inherited2::lock_);

        if (!sessionEndSent_)
          if (!putSessionMessage (STREAM_SESSION_END,
                                  sessionData_,
                                  false))
            ACE_DEBUG ((LM_ERROR,
                        ACE_TEXT ("putSessionMessage(SESSION_END) failed, continuing\n")));
        sessionEndSent_ = true;
      } // end lock scope

//       ACE_DEBUG ((LM_DEBUG,
//                   ACE_TEXT ("stream processing complete\n")));

      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid state transition: \"%s\" --> \"%s\", continuing\n"),
                  ACE_TEXT (inherited::state2String (inherited::current ()).c_str ()),
                  ACE_TEXT (inherited::state2String (newState_in).c_str ())));
      break;
    }
  } // end SWITCH
}

template <typename TaskSynchType,
          typename TimePolicyType,
          typename SessionMessageType,
          typename ProtocolMessageType,
          typename ConfigurationType,
          typename StreamStateType,
          typename SessionDataType,
          typename SessionDataContainerType>
bool
Stream_HeadModuleTaskBase_T<TaskSynchType,
                            TimePolicyType,
                            SessionMessageType,
                            ProtocolMessageType,
                            ConfigurationType,
                            StreamStateType,
                            SessionDataType,
                            SessionDataContainerType>::putSessionMessage (Stream_SessionMessageType messageType_in,
                                                                          SessionDataContainerType*& sessionData_inout,
                                                                          Stream_IAllocator* allocator_in) const
{
  STREAM_TRACE (ACE_TEXT ("Stream_HeadModuleTaskBase_T::putSessionMessage"));

  // sanity check(s)
  ACE_ASSERT (streamState_);

  int result = -1;

  // create session message
  SessionMessageType* session_message_p = NULL;
  if (allocator_in)
  {
allocate:
    try
    {
      // *IMPORTANT NOTE*: 0 --> session message !
      session_message_p =
        static_cast<SessionMessageType*> (allocator_in->malloc (0));
    }
    catch (...)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("caught exception in Stream_IAllocator::malloc(0), aborting\n")));

      // clean up
      sessionData_inout->decrease ();
      sessionData_inout = NULL;

      return false;
    }

    // keep retrying ?
    if (!session_message_p &&
        !allocator_in->block ())
      goto allocate;
  } // end IF
  else
  {
    // *IMPORTANT NOTE*: session message assumes responsibility for
    //                   sessionData_inout
    // *TODO*: remove type inference
    ACE_NEW_NORETURN (session_message_p,
                      SessionMessageType (messageType_in,
                                          sessionData_inout,
                                          streamState_->userData));
  } // end ELSE
  if (!session_message_p)
  {
    if (allocator_in)
    {
      if (allocator_in->block ())
        ACE_DEBUG ((LM_CRITICAL,
                    ACE_TEXT ("failed to allocate SessionMessageType: \"%m\", aborting\n")));
    } // end IF
    else
      ACE_DEBUG ((LM_CRITICAL,
                  ACE_TEXT ("failed to allocate SessionMessageType: \"%m\", aborting\n")));

    // clean up
    sessionData_inout->decrease ();
    sessionData_inout = NULL;

    return false;
  } // end IF
  if (allocator_in)
  {
    // *IMPORTANT NOTE*: session message assumes responsibility for
    //                   sessionData_inout
    // *TODO*: remove type inference
    session_message_p->initialize (messageType_in,
                                   sessionData_inout,
                                   streamState_->userData);
  } // end IF

  // pass message downstream...
  result = const_cast<OWN_TYPE_T*> (this)->put (session_message_p,
                                                NULL);
  if (result == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Task::put(): \"%m\", aborting\n")));

    // clean up
    session_message_p->release ();

    return false;
  } // end IF

  //ACE_DEBUG ((LM_DEBUG,
  //            ACE_TEXT ("enqueued session message...\n")));

  return true;
}

template <typename TaskSynchType,
          typename TimePolicyType,
          typename SessionMessageType,
          typename ProtocolMessageType,
          typename ConfigurationType,
          typename StreamStateType,
          typename SessionDataType,
          typename SessionDataContainerType>
bool
Stream_HeadModuleTaskBase_T<TaskSynchType,
                            TimePolicyType,
                            SessionMessageType,
                            ProtocolMessageType,
                            ConfigurationType,
                            StreamStateType,
                            SessionDataType,
                            SessionDataContainerType>::putSessionMessage (Stream_SessionMessageType messageType_in,
                                                                          SessionDataType* sessionData_in,
                                                                          bool deleteSessionData_in) const
{
  STREAM_TRACE (ACE_TEXT ("Stream_HeadModuleTaskBase_T::putSessionMessage"));

  // sanity check(s)
  // *TODO*: remove type inference
  ACE_ASSERT (configuration_.streamConfiguration);

  // create session data
  SessionDataContainerType* session_data_container_p = NULL;
  switch (messageType_in)
  {
    case STREAM_SESSION_BEGIN:
    case STREAM_SESSION_STEP:
    case STREAM_SESSION_END:
    {
      ACE_NEW_NORETURN (session_data_container_p,
                        SessionDataContainerType (sessionData_in,
                                                  deleteSessionData_in));
      if (!session_data_container_p)
      {
        ACE_DEBUG ((LM_CRITICAL,
                    ACE_TEXT ("failed to allocate SessionDataContainerType: \"%m\", aborting\n")));
        return false;
      } // end IF

      break;
    }
    case STREAM_SESSION_STATISTIC:
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown session message type (was: %d), aborting\n"),
                  messageType_in));
      return false;
    }
  } // end SWITCH

  // *IMPORTANT NOTE*: "fire-and-forget" session_data_container_p
  // *TODO*: remove type inference
  return putSessionMessage (messageType_in,
                            session_data_container_p,
                            configuration_.streamConfiguration->messageAllocator);
}
