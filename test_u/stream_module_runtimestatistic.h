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

#ifndef STREAM_MODULE_RUNTIMESTATISTIC_H
#define STREAM_MODULE_RUNTIMESTATISTIC_H

#include <set>
#include <map>

#include "ace/Global_Macros.h"
#include "ace/Stream_Modules.h"
#include "ace/Synch_Traits.h"

#include "common_icounter.h"
#include "common_istatistic.h"

#include "stream_common.h"
#include "stream_defines.h"
#include "stream_resetcounterhandler.h"
#include "stream_statistichandler.h"
#include "stream_streammodule_base.h"
#include "stream_task_base_synch.h"

// forward declaration(s)
class ACE_Message_Block;
class ACE_Time_Value;
class Stream_IAllocator;
template <typename TaskSynchType,
          typename TimePolicyType,
          typename SessionMessageType,
          typename ProtocolMessageType,
          typename ProtocolCommandType,
          typename StatisticContainerType> class Stream_Module_Statistic_WriterTask_T;

template <typename TaskSynchType,
          typename TimePolicyType,
          typename SessionMessageType,
          typename ProtocolMessageType,
          typename ProtocolCommandType,
          typename StatisticContainerType>
class Stream_Module_Statistic_ReaderTask_T
 : public ACE_Thru_Task<TaskSynchType,
                        TimePolicyType>
{
 public:
  Stream_Module_Statistic_ReaderTask_T ();
  virtual ~Stream_Module_Statistic_ReaderTask_T ();

  virtual int put (ACE_Message_Block*,      // message
                   ACE_Time_Value* = NULL); // time

 private:
  typedef ACE_Thru_Task<TaskSynchType,
                        TimePolicyType> inherited;
  typedef Stream_Module_Statistic_WriterTask_T<TaskSynchType,
                                               TimePolicyType,
                                               SessionMessageType,
                                               ProtocolMessageType,
                                               ProtocolCommandType,
                                               StatisticContainerType> WRITER_TASK_T;
  typedef ProtocolMessageType MESSAGE_TYPE_T;
  typedef ProtocolCommandType COMMAND_TYPE_T;

  ACE_UNIMPLEMENTED_FUNC (Stream_Module_Statistic_ReaderTask_T (const Stream_Module_Statistic_ReaderTask_T&))
  ACE_UNIMPLEMENTED_FUNC (Stream_Module_Statistic_ReaderTask_T& operator= (const Stream_Module_Statistic_ReaderTask_T&))
};

template <typename TaskSynchType,
          typename TimePolicyType,
          typename SessionMessageType,
          typename ProtocolMessageType,
          typename ProtocolCommandType,
          typename StatisticContainerType>
class Stream_Module_Statistic_WriterTask_T
 : public Stream_TaskBaseSynch_T<TimePolicyType,
                                 SessionMessageType,
                                 ProtocolMessageType>
 , public Common_ICounter
 , public Common_IStatistic_T<StatisticContainerType>
{
 friend class Stream_Module_Statistic_ReaderTask_T<TaskSynchType,
                                                   TimePolicyType,
                                                   SessionMessageType,
                                                   ProtocolMessageType,
                                                   ProtocolCommandType,
                                                   StatisticContainerType>;
 public:
  Stream_Module_Statistic_WriterTask_T ();
  virtual ~Stream_Module_Statistic_WriterTask_T ();

  // initialization
  bool initialize (unsigned int = STREAM_DEFAULT_STATISTICS_REPORTING, // (local) reporting interval (second(s)) [0: off]
                   bool = false,                                       // print final report ?
                   const Stream_IAllocator* = NULL);                   // report cache usage ?

  // implement (part of) Stream_ITaskBase
  virtual void handleDataMessage (ProtocolMessageType*&, // data message handle
                                  bool&);                // return value: pass message downstream ?

  // implement this so we can print overall statistics after session completes...
  virtual void handleSessionMessage (SessionMessageType*&, // session message handle
                                     bool&);               // return value: pass message downstream ?

  // implement Stream_ICounter
  virtual void reset ();

  // implement Common_IStatistic
  virtual bool collect (StatisticContainerType&); // return value: info
  // *NOTE*: this also implements locally triggered reporting !
  virtual void report () const;

 private:
  typedef Stream_TaskBaseSynch_T<TimePolicyType,
                                 SessionMessageType,
                                 ProtocolMessageType> inherited;

  // message type counters
//  typedef std::set<ProtocolCommandType> Net_Messages_t;
//  typedef typename Net_Messages_t::const_iterator Net_MessagesIterator_t;
  typedef std::map<ProtocolCommandType,
                   unsigned int> MESSAGE_STATISTIC_T;
  typedef typename MESSAGE_STATISTIC_T::const_iterator MESSAGE_STATISTICITERATOR_T;
  typedef std::pair<ProtocolCommandType,
                    unsigned int> MESSAGE_STATISTIC_RECORD_T;

  ACE_UNIMPLEMENTED_FUNC (Stream_Module_Statistic_WriterTask_T (const Stream_Module_Statistic_WriterTask_T&))
  ACE_UNIMPLEMENTED_FUNC (Stream_Module_Statistic_WriterTask_T& operator= (const Stream_Module_Statistic_WriterTask_T&))

  // helper method(s)
  void final_report () const;
  void fini_timers (bool = true); // cancel both timers ? (false --> cancel only localReportingHandlerID_)

  bool                              isInitialized_;

  // timer stuff
  Stream_ResetCounterHandler        resetTimeoutHandler_;
  long                              resetTimeoutHandlerID_;
  Stream_StatisticHandler_Reactor_t localReportingHandler_;
  long                              localReportingHandlerID_;
  unsigned int                      reportingInterval_; // second(s) {0 --> OFF}
  bool                              printFinalReport_;

  // *GENERIC STATS*
  mutable ACE_SYNCH_MUTEX           lock_;
  unsigned int                      sessionID_;

  // *NOTE*: data messages == (myNumTotalMessages - myNumSessionMessages)
  unsigned int                      numInboundMessages_;
  unsigned int                      numOutboundMessages_;
  unsigned int                      numSessionMessages_;
  // used to compute message throughput...
  unsigned int                      messageCounter_;
  // *NOTE: support asynchronous collecting/reporting of data...
  unsigned int                      lastMessagesPerSecondCount_;

  float                             numInboundBytes_;
  float                             numOutboundBytes_;
  // used to compute data throughput...
  unsigned int                      byteCounter_;
  // *NOTE: support asynchronous collecting/reporting of data...
  unsigned int                      lastBytesPerSecondCount_;

  // *MESSAGE TYPE STATS*
  MESSAGE_STATISTIC_T               messageTypeStatistic_;

  // *CACHE STATS*
  const Stream_IAllocator*          allocator_;
};

// include template implementation
#include "stream_module_runtimestatistic.inl"

#endif
