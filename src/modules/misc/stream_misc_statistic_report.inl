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

#include <ace/Guard_T.h>
#include <ace/Log_Msg.h>

#include "common_timer_manager_common.h"

#include "stream_defines.h"
#include "stream_iallocator.h"
#include "stream_macros.h"
#include "stream_message_base.h"

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename ConfigurationType,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType,
          typename ProtocolCommandType,
          typename StatisticContainerType,
          typename SessionDataType,
          typename SessionDataContainerType>
Stream_Module_StatisticReport_WriterTask_T<ACE_SYNCH_USE,
                                           TimePolicyType,
                                           ConfigurationType,
                                           ControlMessageType,
                                           DataMessageType,
                                           SessionMessageType,
                                           ProtocolCommandType,
                                           StatisticContainerType,
                                           SessionDataType,
                                           SessionDataContainerType>::Stream_Module_StatisticReport_WriterTask_T ()
 : inherited ()
 , lock_ ()
 , inboundBytes_ (0.0F)
 , outboundBytes_ (0.0F)
 , inboundMessages_ (0)
 , outboundMessages_ (0)
 , lastBytesPerSecondCount_ (0)
 , lastDataMessagesPerSecondCount_ (0)
 , sessionMessages_ (0)
 /////////////////////////////////////////
 , resetTimeoutHandler_ (this)
 , resetTimeoutHandlerID_ (-1)
 , localReportingHandler_ (ACTION_REPORT,
                           this,
                           false)
 , localReportingHandlerID_ (-1)
 , reportingInterval_ (STREAM_DEFAULT_STATISTIC_REPORTING_INTERVAL, 0)
 , printFinalReport_ (false)
 , pushStatisticMessages_ (false)
 , byteCounter_ (0)
 , fragmentCounter_ (0)
 , messageCounter_ (0)
 , sessionMessageCounter_ (0)
 , messageTypeStatistic_ ()
 , allocator_ (NULL)
{
  STREAM_TRACE (ACE_TEXT ("Stream_Module_StatisticReport_WriterTask_T::Stream_Module_StatisticReport_WriterTask_T"));

}

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename ConfigurationType,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType,
          typename ProtocolCommandType,
          typename StatisticContainerType,
          typename SessionDataType,
          typename SessionDataContainerType>
Stream_Module_StatisticReport_WriterTask_T<ACE_SYNCH_USE,
                                           TimePolicyType,
                                           ConfigurationType,
                                           ControlMessageType,
                                           DataMessageType,
                                           SessionMessageType,
                                           ProtocolCommandType,
                                           StatisticContainerType,
                                           SessionDataType,
                                           SessionDataContainerType>::~Stream_Module_StatisticReport_WriterTask_T ()
{
  STREAM_TRACE (ACE_TEXT ("Stream_Module_StatisticReport_WriterTask_T::~Stream_Module_StatisticReport_WriterTask_T"));

  finiTimers (true);
}

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename ConfigurationType,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType,
          typename ProtocolCommandType,
          typename StatisticContainerType,
          typename SessionDataType,
          typename SessionDataContainerType>
bool
Stream_Module_StatisticReport_WriterTask_T<ACE_SYNCH_USE,
                                           TimePolicyType,
                                           ConfigurationType,
                                           ControlMessageType,
                                           DataMessageType,
                                           SessionMessageType,
                                           ProtocolCommandType,
                                           StatisticContainerType,
                                           SessionDataType,
                                           SessionDataContainerType>::initialize (const ConfigurationType& configuration_in)
{
  STREAM_TRACE (ACE_TEXT ("Stream_Module_StatisticReport_WriterTask_T::initialize"));

  // sanity check(s)
  if (inherited::isInitialized_)
  {
    //ACE_DEBUG ((LM_DEBUG,
    //            ACE_TEXT ("re-initializing...\n")));

    // stop timers
    finiTimers (true);

    reportingInterval_ = ACE_Time_Value::zero;
    printFinalReport_ = false;
    pushStatisticMessages_ = false;

    // reset various counters...
    ACE_GUARD_RETURN (ACE_SYNCH_MUTEX_T, aGuard, lock_, false);

    inboundBytes_ = 0.0F;
    outboundBytes_ = 0.0F;
    inboundMessages_ = 0;
    outboundMessages_ = 0;
    lastBytesPerSecondCount_ = 0;
    lastDataMessagesPerSecondCount_ = 0;
    sessionMessages_ = 0;

    byteCounter_ = 0;
    fragmentCounter_ = 0;
    messageCounter_ = 0;
    sessionMessageCounter_ = 0;

    messageTypeStatistic_.clear ();

    allocator_ = NULL;
  } // end IF
  reportingInterval_ = configuration_in.reportingInterval;
  printFinalReport_ = configuration_in.printFinalReport;
  pushStatisticMessages_ = configuration_in.pushStatisticMessages;
  allocator_ = configuration_in.messageAllocator;

  // *NOTE*: if this is an 'outbound' stream, which means that the data (!) will
  //         eventually turn around and travel back upstream for dispatch
  //         --> account for it only once
  if (!configuration_in.inbound)
  {
    ACE_Task_Base* task_base_p = inherited::sibling ();
    if (!task_base_p)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("no sibling task: \"%m\", aborting\n")));
      return false;
    } // end IF
    READER_TASK_T* reader_p = dynamic_cast<READER_TASK_T*> (task_base_p);
    if (!reader_p)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to dynamic_cast<Stream_Module_StatisticReport_ReaderTask_T>: \"%m\", aborting\n")));
      return false;
    } // end IF
    reader_p->hasRoundTripData_ = true;
  } // end IF

  if ((reportingInterval_ != ACE_Time_Value::zero) ||
      pushStatisticMessages_)
  {
    Common_Timer_Manager_t* timer_manager_p =
        COMMON_TIMERMANAGER_SINGLETON::instance ();
    ACE_ASSERT (timer_manager_p);
    // schedule the second-granularity timer
    ACE_Time_Value one_second (1, 0); // one-second interval
    ACE_Event_Handler* event_handler_p = &resetTimeoutHandler_;
    resetTimeoutHandlerID_ =
      timer_manager_p->schedule_timer (event_handler_p,              // event handler
                                       NULL,                         // ACT
                                       COMMON_TIME_NOW + one_second, // first wakeup time
                                       one_second);                  // interval
    if (resetTimeoutHandlerID_ == -1)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to Common_ITimer::schedule_timer(%#T): \"%m\", aborting\n"),
                  &one_second));
      return false;
    } // end IF
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("%s: scheduled second-interval timer (ID: %d)...\n"),
                inherited::mod_->name (),
                resetTimeoutHandlerID_));
  } // end IF

  return inherited::initialize (configuration_in);
}

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename ConfigurationType,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType,
          typename ProtocolCommandType,
          typename StatisticContainerType,
          typename SessionDataType,
          typename SessionDataContainerType>
void
Stream_Module_StatisticReport_WriterTask_T<ACE_SYNCH_USE,
                                           TimePolicyType,
                                           ConfigurationType,
                                           ControlMessageType,
                                           DataMessageType,
                                           SessionMessageType,
                                           ProtocolCommandType,
                                           StatisticContainerType,
                                           SessionDataType,
                                           SessionDataContainerType>::handleDataMessage (DataMessageType*& message_inout,
                                                                                         bool& passMessageDownstream_out)
{
  STREAM_TRACE (ACE_TEXT ("Stream_Module_StatisticReport_WriterTask_T::handleDataMessage"));

  // don't care (implies yes per default, if part of a stream)
  ACE_UNUSED_ARG (passMessageDownstream_out);

  {
    ACE_GUARD (ACE_SYNCH_MUTEX_T, aGuard, lock_);

    // update counters...
    inboundBytes_ += message_inout->total_length ();
    byteCounter_ += message_inout->total_length ();

    ++inboundMessages_;
    for (ACE_Message_Block* message_block_p = message_inout;
         message_block_p;
         message_block_p = message_block_p->cont ())
      ++fragmentCounter_;
    ++messageCounter_;

    // add message to statistic
    messageTypeStatistic_[message_inout->command ()]++;
  } // end lock scope
}
template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename ConfigurationType,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType,
          typename ProtocolCommandType,
          typename StatisticContainerType,
          typename SessionDataType,
          typename SessionDataContainerType>
void
Stream_Module_StatisticReport_WriterTask_T<ACE_SYNCH_USE,
                                           TimePolicyType,
                                           ConfigurationType,
                                           ControlMessageType,
                                           DataMessageType,
                                           SessionMessageType,
                                           ProtocolCommandType,
                                           StatisticContainerType,
                                           SessionDataType,
                                           SessionDataContainerType>::handleSessionMessage (SessionMessageType*& message_inout,
                                                                                            bool& passMessageDownstream_out)
{
  STREAM_TRACE (ACE_TEXT ("Stream_Module_StatisticReport_WriterTask_T::handleSessionMessage"));

  // don't care (implies yes per default, if part of a stream)
  ACE_UNUSED_ARG (passMessageDownstream_out);

  // sanity check(s)
  ACE_ASSERT (inherited::isInitialized_);

  {
    ACE_GUARD (ACE_SYNCH_MUTEX_T, aGuard, lock_);

    // update counters
    ++inboundMessages_;
    ++messageCounter_;
    ++sessionMessageCounter_;
    ++sessionMessages_;
  } // end lock scope

  switch (message_inout->type ())
  {
    case STREAM_SESSION_MESSAGE_BEGIN:
    {
      // statistic reporting
      if (reportingInterval_ != ACE_Time_Value::zero)
      {
        // schedule the reporting interval timer
        ACE_ASSERT (localReportingHandlerID_ == -1);
        ACE_Event_Handler* event_handler_p = &localReportingHandler_;
        localReportingHandlerID_ =
          COMMON_TIMERMANAGER_SINGLETON::instance ()->schedule_timer (event_handler_p,                      // event handler
                                                                      NULL,                                 // ACT
                                                                      COMMON_TIME_NOW + reportingInterval_, // first wakeup time
                                                                      reportingInterval_);                  // interval
        if (localReportingHandlerID_ == -1)
        {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("failed to Common_ITimer::schedule_timer(%#T): \"%m\", returning\n"),
                      &reportingInterval_));
          return;
        } // end IF
        ACE_DEBUG ((LM_DEBUG,
                    ACE_TEXT ("%s: scheduled (local) reporting timer (ID: %d, interval: %#T)...\n"),
                    ACE_TEXT (inherited::name ()),
                    localReportingHandlerID_,
                    &reportingInterval_));
      } // end IF
      // *NOTE*: even if this doesn't report, it might still be triggered from
      //         outside

      break;
    }
    case STREAM_SESSION_MESSAGE_STATISTIC:
    {
      // sanity check(s)
      ACE_ASSERT (inherited::sessionData_);

      // *NOTE*: the message contains statistic information from some upstream
      //         module (e.g. some hardware capture device driver)
      //         --> aggregate this data so that any new (session) messages
      //             generated by this module carry up-to-date information
      typename SessionDataContainerType::DATA_T& session_data_r =
        const_cast<typename SessionDataContainerType::DATA_T&> (inherited::sessionData_->get ());
      const SessionDataContainerType& session_data_container_r =
        message_inout->get ();
      const typename SessionDataContainerType::DATA_T& session_data_2 =
        session_data_container_r.get ();

      ACE_ASSERT (session_data_r.lock);
      ACE_ASSERT (session_data_2.lock);
      ACE_ASSERT (&session_data_r != &session_data_2);
      {
        int result = -1;
        bool release_lock = false;

        ACE_GUARD (ACE_SYNCH_MUTEX, aGuard, *session_data_r.lock);
        if (session_data_r.lock != session_data_2.lock)
        {
          result = session_data_2.lock->acquire ();
          if (result == -1)
            ACE_DEBUG ((LM_ERROR,
                        ACE_TEXT ("%s: failed to ACE_SYNCH_MUTEX::acquire(): \"%m\", continuing\n"),
                        inherited::mod_->name ()));
          release_lock = true;
        } // end IF

        // *NOTE*: the idea is to 'effectively merge' the statistic data
        // *TODO*: it may be better to just copy the upstream data instead
        session_data_r.currentStatistic += session_data_2.currentStatistic;

        if (release_lock)
        {
          result = session_data_2.lock->release ();
          if (result == -1)
            ACE_DEBUG ((LM_ERROR,
                        ACE_TEXT ("%s: failed to ACE_SYNCH_MUTEX::release(): \"%m\", continuing\n"),
                        inherited::mod_->name ()));
        } // end IF
      } // end lock scope

      break;
    }
    case STREAM_SESSION_MESSAGE_END:
    {
      // stop (reporting) timer(s) (need to re-initialize() after this)
      finiTimers (true);

      // session finished --> print overall statistic ?
      if (printFinalReport_) finalReport ();

      break;
    }
    default:
      break;
  } // end SWITCH
}

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename ConfigurationType,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType,
          typename ProtocolCommandType,
          typename StatisticContainerType,
          typename SessionDataType,
          typename SessionDataContainerType>
void
Stream_Module_StatisticReport_WriterTask_T<ACE_SYNCH_USE,
                                           TimePolicyType,
                                           ConfigurationType,
                                           ControlMessageType,
                                           DataMessageType,
                                           SessionMessageType,
                                           ProtocolCommandType,
                                           StatisticContainerType,
                                           SessionDataType,
                                           SessionDataContainerType>::reset ()
{
  STREAM_TRACE (ACE_TEXT ("Stream_Module_StatisticReport_WriterTask_T::reset"));

  // *NOTE*: reset() occurs every second (roughly)

  bool in_session = false;

  {
    ACE_GUARD (ACE_SYNCH_MUTEX_T, aGuard, lock_);

    // remember this result (support asynchronous API)
    lastBytesPerSecondCount_ = byteCounter_;
    lastDataMessagesPerSecondCount_ = messageCounter_ - sessionMessageCounter_;

    // reset counters
    byteCounter_ = 0;
    fragmentCounter_ = 0;
    messageCounter_ = 0;
    sessionMessageCounter_ = 0;

    // update session data ?
    if (!inherited::sessionData_) goto continue_;

    typename SessionMessageType::DATA_T::DATA_T& session_data_r =
        const_cast<typename SessionMessageType::DATA_T::DATA_T&> (inherited::sessionData_->get ());
    ACE_ASSERT (session_data_r.lock);
    {
      ACE_GUARD (ACE_SYNCH_MUTEX, aGuard_2, *session_data_r.lock);

      // *TODO*: remove type inferences
      session_data_r.currentStatistic.bytes = inboundBytes_ + outboundBytes_;
      session_data_r.currentStatistic.dataMessages =
          inboundMessages_ + outboundMessages_ - sessionMessages_;
      // *NOTE*: if this is an 'outbound' stream, which means that the data (!)
      //         will eventually turn around and travel back upstream for
      //         dispatch
      //         --> account for it only once
      if (!inherited::configuration_->inbound)
      {
        session_data_r.currentStatistic.bytes -= outboundBytes_;
        session_data_r.currentStatistic.dataMessages -= outboundMessages_;
      } // end IF
      //session_data_r.currentStatistic.droppedMessages = 0;
      session_data_r.currentStatistic.bytesPerSecond =
          static_cast<float> (lastBytesPerSecondCount_);
      session_data_r.currentStatistic.messagesPerSecond =
          static_cast<float> (lastDataMessagesPerSecondCount_);
      session_data_r.currentStatistic.timeStamp = COMMON_TIME_NOW;
    } // end lock scope

    in_session = true;
  } // end lock scope

continue_:
  if (in_session && pushStatisticMessages_)
    if (!putStatisticMessage ())
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to Stream_Module_StatisticReport_WriterTask_T::putStatisticMessage(), continuing\n")));
}

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename ConfigurationType,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType,
          typename ProtocolCommandType,
          typename StatisticContainerType,
          typename SessionDataType,
          typename SessionDataContainerType>
bool
Stream_Module_StatisticReport_WriterTask_T<ACE_SYNCH_USE,
                                           TimePolicyType,
                                           ConfigurationType,
                                           ControlMessageType,
                                           DataMessageType,
                                           SessionMessageType,
                                           ProtocolCommandType,
                                           StatisticContainerType,
                                           SessionDataType,
                                           SessionDataContainerType>::collect (StatisticContainerType& data_out)
{
  STREAM_TRACE (ACE_TEXT ("Stream_Module_StatisticReport_WriterTask_T::collect"));

  // *NOTE*: external call; fill the argument with meaningful values
  // *TODO*: the temaplate must not know about StatisticContainerType
  //         --> should be overriden by derived classes

  // initialize return value(s)
  ACE_OS::memset (&data_out, 0, sizeof (StatisticContainerType));

  {
    ACE_GUARD_RETURN (ACE_SYNCH_MUTEX_T, aGuard, lock_, false);

    // *TODO*: remove type inferences
    data_out.bytes = inboundBytes_ + outboundBytes_;
    data_out.dataMessages =
        inboundMessages_ + outboundMessages_ - sessionMessages_;
    // *NOTE*: if this is an 'outbound' stream, which means that the data (!)
    //         will eventually turn around and travel back upstream for
    //         dispatch
    //         --> account for it only once
    if (!inherited::configuration_->inbound)
    {
      data_out.bytes -= outboundBytes_;
      data_out.dataMessages -= outboundMessages_;
    } // end IF
  } // end lock scope

  return true;
}

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename ConfigurationType,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType,
          typename ProtocolCommandType,
          typename StatisticContainerType,
          typename SessionDataType,
          typename SessionDataContainerType>
void
Stream_Module_StatisticReport_WriterTask_T<ACE_SYNCH_USE,
                                           TimePolicyType,
                                           ConfigurationType,
                                           ControlMessageType,
                                           DataMessageType,
                                           SessionMessageType,
                                           ProtocolCommandType,
                                           StatisticContainerType,
                                           SessionDataType,
                                           SessionDataContainerType>::report () const
{
  STREAM_TRACE (ACE_TEXT ("Stream_Module_StatisticReport_WriterTask_T::report"));

  int result = -1;
  SessionDataType* session_data_p = NULL;

  ACE_GUARD (ACE_SYNCH_MUTEX_T, aGuard, lock_);

  if (inherited::sessionData_)
    session_data_p =
        &const_cast<SessionDataType&> (inherited::sessionData_->get ());

  if (session_data_p)
    if (session_data_p->lock)
    {
      result = session_data_p->lock->acquire ();
      if (result == -1)
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ACE_SYNCH_MUTEX::acquire(): \"%m\", returning\n")));
        return;
      } // end IF
    } // end IF

  // *NOTE*: if this is an 'outbound' stream, which means that the data (!)
  //         will eventually turn around and travel back upstream for
  //         dispatch
  //         --> account for it only once
  unsigned int data_messages =
      inboundMessages_ + outboundMessages_ - sessionMessages_;
  unsigned int total_messages = inboundMessages_ + outboundMessages_;
  if (!inherited::configuration_->inbound)
  {
    data_messages -= outboundMessages_;
    total_messages -= outboundMessages_;
  } // end IF
  // *TODO*: remove type inferences
  ACE_DEBUG ((LM_INFO,
              ACE_TEXT ("*** [session: %d] RUNTIME STATISTICS ***\n--> Stream Statistics <--\n\tmessages/sec: %u\n\tmessages total [in/out]): %u/%u (data: %.2f%%)\n\tbytes/sec: %u\n\tbytes total: %.0f\n--> Cache Statistics <--\n\tcurrent cache usage [%u messages / %u byte(s) allocated]\n*** RUNTIME STATISTICS ***\\END\n"),
              (session_data_p ? static_cast<int> (session_data_p->sessionID) : -1),
              lastDataMessagesPerSecondCount_, inboundMessages_, outboundMessages_,
              (static_cast<float> (data_messages) /
               static_cast<float> (total_messages) *
               100.0F),
              lastBytesPerSecondCount_, inboundBytes_ + outboundBytes_,
              (allocator_ ? allocator_->cache_size () : 0), (allocator_ ? allocator_->cache_depth () : 0)));

  if (session_data_p)
    if (session_data_p->lock)
    {
      result = session_data_p->lock->release ();
      if (result == -1)
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ACE_SYNCH_MUTEX::release(): \"%m\", continuing\n")));
    } // end IF
}

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename ConfigurationType,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType,
          typename ProtocolCommandType,
          typename StatisticContainerType,
          typename SessionDataType,
          typename SessionDataContainerType>
void
Stream_Module_StatisticReport_WriterTask_T<ACE_SYNCH_USE,
                                           TimePolicyType,
                                           ConfigurationType,
                                           ControlMessageType,
                                           DataMessageType,
                                           SessionMessageType,
                                           ProtocolCommandType,
                                           StatisticContainerType,
                                           SessionDataType,
                                           SessionDataContainerType>::finalReport () const
{
  STREAM_TRACE (ACE_TEXT ("Stream_Module_StatisticReport_WriterTask_T::finalReport"));

  int result = -1;
  SessionDataType* session_data_p = NULL;
  if (inherited::sessionData_)
    session_data_p =
        &const_cast<SessionDataType&> (inherited::sessionData_->get ());

  if (session_data_p)
    if (session_data_p->lock)
    {
      result = session_data_p->lock->acquire ();
      if (result == -1)
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ACE_SYNCH_MUTEX::acquire(): \"%m\", returning\n")));
        return;
      } // end IF
    } // end IF

  if (inboundMessages_ + outboundMessages_)
  {
    // *TODO*: remove type inferences
    ACE_DEBUG ((LM_INFO,
                ACE_TEXT ("*** [session: %d] SESSION STATISTIC ***\n\ttotal # data message(s) [in/out]: %u/%u\n --> Protocol Info <--\n"),
                (session_data_p ? static_cast<int> (session_data_p->sessionID) : -1),
                inboundMessages_ - sessionMessages_, outboundMessages_ - sessionMessages_));

    // *NOTE*: if this is an 'outbound' stream, which means that the data (!)
    //         will eventually turn around and travel back upstream for
    //         dispatch
    //         --> account for it only once
    unsigned int data_messages =
        inboundMessages_ + outboundMessages_ - sessionMessages_;
    if (!inherited::configuration_->inbound)
      data_messages -= outboundMessages_;
    for (STATISTIC_ITERATOR_T iterator = messageTypeStatistic_.begin ();
         iterator != messageTypeStatistic_.end ();
         iterator++)
      ACE_DEBUG ((LM_INFO,
                  ACE_TEXT ("\t\"%s\": %u --> %.2f %%\n"),
                  ACE_TEXT (DataMessageType::CommandType2String (iterator->first).c_str ()),
                  iterator->second,
                  (static_cast<float> (iterator->second) * 100.0F) /
                  static_cast<float> (data_messages)));
    ACE_DEBUG ((LM_INFO,
                ACE_TEXT ("------------------------------------------\n\ttotal byte(s) [in/out]: %.0f/%.0f\n\tbytes/s: %u\n"),
                inboundBytes_, outboundBytes_,
                lastBytesPerSecondCount_)); // *TODO*: compute average
  } // end IF

  if (session_data_p)
    if (session_data_p->lock)
    {
      result = session_data_p->lock->release ();
      if (result == -1)
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ACE_SYNCH_MUTEX::release(): \"%m\", continuing\n")));
    } // end IF
}

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename ConfigurationType,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType,
          typename ProtocolCommandType,
          typename StatisticContainerType,
          typename SessionDataType,
          typename SessionDataContainerType>
void
Stream_Module_StatisticReport_WriterTask_T<ACE_SYNCH_USE,
                                           TimePolicyType,
                                           ConfigurationType,
                                           ControlMessageType,
                                           DataMessageType,
                                           SessionMessageType,
                                           ProtocolCommandType,
                                           StatisticContainerType,
                                           SessionDataType,
                                           SessionDataContainerType>::finiTimers (bool cancelAllTimers_in)
{
  STREAM_TRACE (ACE_TEXT ("Stream_Module_StatisticReport_WriterTask_T::finiTimers"));

  int result = -1;
  const void* act_p = NULL;
  Common_ITimer* itimer_p = COMMON_TIMERMANAGER_SINGLETON::instance ();
  ACE_ASSERT (itimer_p);

  if (cancelAllTimers_in)
  {
    if (resetTimeoutHandlerID_ != -1)
    {
      result = itimer_p->cancel_timer (resetTimeoutHandlerID_,
                                       &act_p);
      if (result <= 0)
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to Common_ITimer::cancel_timer(%d): \"%m\", continuing\n"),
                    resetTimeoutHandlerID_));
      resetTimeoutHandlerID_ = -1;
    } // end IF
  } // end IF

  if (localReportingHandlerID_ != -1)
  {
    act_p = NULL;
    result = itimer_p->cancel_timer (localReportingHandlerID_,
                                     &act_p);
    if (result <= 0)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to Common_ITimer::cancel_timer(%d): \"%m\", continuing\n"),
                  localReportingHandlerID_));
    localReportingHandlerID_ = -1;
  } // end IF
}

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename ConfigurationType,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType,
          typename ProtocolCommandType,
          typename StatisticContainerType,
          typename SessionDataType,
          typename SessionDataContainerType>
bool
Stream_Module_StatisticReport_WriterTask_T<ACE_SYNCH_USE,
                                           TimePolicyType,
                                           ConfigurationType,
                                           ControlMessageType,
                                           DataMessageType,
                                           SessionMessageType,
                                           ProtocolCommandType,
                                           StatisticContainerType,
                                           SessionDataType,
                                           SessionDataContainerType>::putStatisticMessage ()
{
  STREAM_TRACE (ACE_TEXT ("Stream_Module_StatisticReport_WriterTask_T::putStatisticMessage"));

  SessionDataContainerType* session_data_container_p = NULL;
  if (inherited::sessionData_)
  {
    inherited::sessionData_->increase ();
    session_data_container_p = inherited::sessionData_;
  } // end IF
  else
  {
    SessionDataType* session_data_p = NULL;
    ACE_NEW_NORETURN (session_data_p,
                      SessionDataType ());
    if (!session_data_p)
    {
      ACE_DEBUG ((LM_CRITICAL,
                  ACE_TEXT ("failed to allocate SessionDataType: \"%m\", aborting\n")));
      return false;
    } // end IF

    // *IMPORTANT NOTE*: fire-and-forget session_data_p
    ACE_NEW_NORETURN (session_data_container_p,
                      SessionDataContainerType (session_data_p));
    if (!session_data_container_p)
    {
      ACE_DEBUG ((LM_CRITICAL,
                  ACE_TEXT ("failed to allocate SessionDataContainerType: \"%m\", aborting\n")));
      return false;
    } // end IF
  } // end ELSE
  ACE_ASSERT (session_data_container_p);

  SessionDataType* session_data_p =
    &const_cast<SessionDataType&> (session_data_container_p->get ());

  // create session message
  SessionMessageType* session_message_p = NULL;
  if (allocator_)
  {
allocate:
    try {
      // *IMPORTANT NOTE*: 0 --> session message !
      session_message_p =
        static_cast<SessionMessageType*> (allocator_->malloc (0));
    } catch (...) {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("caught exception in Stream_IAllocator::malloc(0), aborting\n")));

      // clean up
      session_data_container_p->decrease ();

      return false;
    }

    // keep retrying ?
    if (!session_message_p &&
        !allocator_->block ())
      goto allocate;
  } // end IF
  else
  {
    // *TODO*: remove type inference
    // *IMPORTANT NOTE*: fire-and-forget session_data_container_p
    ACE_NEW_NORETURN (session_message_p,
                      SessionMessageType (STREAM_SESSION_MESSAGE_STATISTIC,
                                          session_data_container_p,
                                          (session_data_p ? session_data_p->userData
                                                          : NULL)));
  } // end ELSE
  if (!session_message_p)
  {
    if (allocator_)
    {
      if (allocator_->block ())
        ACE_DEBUG ((LM_CRITICAL,
                    ACE_TEXT ("failed to allocate SessionMessageType: \"%m\", aborting\n")));
    } // end IF
    else
      ACE_DEBUG ((LM_CRITICAL,
                  ACE_TEXT ("failed to allocate SessionMessageType: \"%m\", aborting\n")));

    // clean up
    session_data_container_p->decrease ();

    return false;
  } // end IF
  if (allocator_)
  {
    // *TODO*: remove type inference
    // *IMPORTANT NOTE*: fire-and-forget session_data_container_p
    session_message_p->initialize (STREAM_SESSION_MESSAGE_STATISTIC,
                                   session_data_container_p,
                                   (session_data_p ? session_data_p->userData
                                                   : NULL));
  } // end IF

  // pass message downstream
  int result = inherited::put_next (session_message_p, NULL);
  if (result == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Task::put_next(): \"%m\", aborting\n")));

    // clean up
    session_message_p->release ();

    return false;
  } // end IF

  return true;
}

// -----------------------------------------------------------------------------

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename ConfigurationType,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType,
          typename ProtocolCommandType,
          typename StatisticContainerType,
          typename SessionDataType,
          typename SessionDataContainerType>
Stream_Module_StatisticReport_ReaderTask_T<ACE_SYNCH_USE,
                                           TimePolicyType,
                                           ConfigurationType,
                                           ControlMessageType,
                                           DataMessageType,
                                           SessionMessageType,
                                           ProtocolCommandType,
                                           StatisticContainerType,
                                           SessionDataType,
                                           SessionDataContainerType>::Stream_Module_StatisticReport_ReaderTask_T ()
 : inherited ()
 , hasRoundTripData_ (false)
{
  STREAM_TRACE (ACE_TEXT ("Stream_Module_StatisticReport_ReaderTask_T::Stream_Module_StatisticReport_ReaderTask_T"));

  inherited::flags_ |= ACE_Task_Flags::ACE_READER;
}

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename ConfigurationType,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType,
          typename ProtocolCommandType,
          typename StatisticContainerType,
          typename SessionDataType,
          typename SessionDataContainerType>
Stream_Module_StatisticReport_ReaderTask_T<ACE_SYNCH_USE,
                                           TimePolicyType,
                                           ConfigurationType,
                                           ControlMessageType,
                                           DataMessageType,
                                           SessionMessageType,
                                           ProtocolCommandType,
                                           StatisticContainerType,
                                           SessionDataType,
                                           SessionDataContainerType>::~Stream_Module_StatisticReport_ReaderTask_T ()
{
  STREAM_TRACE (ACE_TEXT ("Stream_Module_StatisticReport_ReaderTask_T::~Stream_Module_StatisticReport_ReaderTask_T"));

}

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename ConfigurationType,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType,
          typename ProtocolCommandType,
          typename StatisticContainerType,
          typename SessionDataType,
          typename SessionDataContainerType>
int
Stream_Module_StatisticReport_ReaderTask_T<ACE_SYNCH_USE,
                                           TimePolicyType,
                                           ConfigurationType,
                                           ControlMessageType,
                                           DataMessageType,
                                           SessionMessageType,
                                           ProtocolCommandType,
                                           StatisticContainerType,
                                           SessionDataType,
                                           SessionDataContainerType>::put (ACE_Message_Block* messageBlock_in,
                                                                           ACE_Time_Value* timeValue_in)
{
  STREAM_TRACE (ACE_TEXT ("Stream_Module_StatisticReport_ReaderTask_T::put"));

  // pass the message to the sibling
  ACE_Task_Base* task_base_p = inherited::sibling ();
  if (!task_base_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("no sibling task: \"%m\", aborting\n")));
    return -1;
  } // end IF
  WRITER_TASK_T* writer_p = dynamic_cast<WRITER_TASK_T*> (task_base_p);
  if (!writer_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to dynamic_cast<Stream_Module_StatisticReport_WriterTask_T>: \"%m\", aborting\n")));
    return -1;
  } // end IF
  // *TODO*: support all message types travelling upstream
  DataMessageType* message_p = dynamic_cast<DataMessageType*> (messageBlock_in);
  if (!message_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to dynamic_cast<DataMessageType>(%@), aborting\n"),
                messageBlock_in));
    return -1;
  } // end IF

  {
    ACE_GUARD_RETURN (ACE_SYNCH_MUTEX_T, aGuard, writer_p->lock_, -1);

    // update counters
    if (hasRoundTripData_)
    {
      writer_p->outboundBytes_ = writer_p->inboundBytes_;
      writer_p->outboundMessages_ = writer_p->inboundMessages_;
      goto continue_;
    } // end IF

    writer_p->outboundBytes_ += messageBlock_in->total_length ();
    writer_p->outboundMessages_++;

    writer_p->byteCounter_ += messageBlock_in->total_length ();
    writer_p->messageCounter_++;
    // add message to statistic
    writer_p->messageTypeStatistic_[message_p->command ()]++;
  } // end lock scope

continue_:
  return inherited::put_next (messageBlock_in, timeValue_in);
}