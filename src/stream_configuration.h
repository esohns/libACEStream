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

#ifndef STREAM_CONFIGURATION_H
#define STREAM_CONFIGURATION_H

#include <map>
#include <string>

#include "ace/Synch_Traits.h"
#include "ace/Time_Value.h"

#include "common_configuration.h"
#include "common_defines.h"
#include "common_idumpstate.h"
#include "common_iinitialize.h"

#include "common_timer_common.h"

#include "stream_common.h"
#include "stream_defines.h"

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include "stream_lib_common.h"
#include "stream_lib_defines.h"
#endif

// forward declarations
class Stream_IAllocator;

struct Stream_AllocatorConfiguration
 : Common_AllocatorConfiguration
{
  Stream_AllocatorConfiguration ()
   : Common_AllocatorConfiguration ()
  {
    defaultBufferSize = STREAM_MESSAGE_DATA_BUFFER_SIZE;
  };
};

struct Common_ParserConfiguration;
class Stream_IOutboundDataNotify;
struct Stream_ModuleHandlerConfiguration
{
  Stream_ModuleHandlerConfiguration ()
   : allocatorConfiguration (NULL)
   , bufferSize (STREAM_MESSAGE_DATA_BUFFER_SIZE)
   , concurrency (STREAM_HEADMODULECONCURRENCY_PASSIVE)
   , crunchMessages (STREAM_MODULE_DEFAULT_CRUNCH_MESSAGES)
   , demultiplex (false)
   , finishOnDisconnect (false)
   , hasHeader (false)
   , hasReentrantSynchronousSubDownstream (true)
   , inbound (false)
#if defined (ACE_WIN32) || defined (ACE_WIN64)
   , mediaFramework (MODULE_LIB_DEFAULT_MEDIAFRAMEWORK)
#endif
   , messageAllocator (NULL)
   , outboundNotificationHandle (NULL)
   , parserConfiguration (NULL)
   , passive (true)
   , printFinalReport (false)
   , pushStatisticMessages (true)
   , reportingInterval (0)
   , socketHandle (ACE_INVALID_HANDLE)
   , statisticCollectionInterval (ACE_Time_Value::zero)
   , statisticReportingInterval (STREAM_DEFAULT_STATISTIC_REPORTING_INTERVAL, 0)
   , subscribersLock (NULL)
   , timerManager (NULL)
  {};

  struct Stream_AllocatorConfiguration* allocatorConfiguration;
  unsigned int                          bufferSize;
  enum Stream_HeadModuleConcurrency     concurrency;                 // head module(s)
  // *NOTE*: this option may be useful for (downstream) modules that only work
  //         on CONTIGUOUS buffers (i.e. cannot parse chained message blocks)
  bool                                  crunchMessages;
  bool                                  demultiplex;                 // message handler module
  bool                                  finishOnDisconnect;          // header module(s)
  bool                                  hasHeader;
  // *WARNING*: when false, this 'locks down' the pipeline head module; i.e. it
  //            will hold the 'stream lock' during all message processing to
  //            support (down)stream synchronization. This really only makes
  //            sense in fully synchronous layouts with asynchronous sources, or
  //            'concurrent' scenarios, with non-reentrant modules. Note that
  //            this overhead is not negligible
  //            --> disable only if absolutely necessary
  bool                                  hasReentrantSynchronousSubDownstream; // head module(s)
  bool                                  inbound;                     // statistic[/IO] module(s)
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  enum Stream_MediaFramework_Type       mediaFramework;
#endif
  Stream_IAllocator*                    messageAllocator;
  Stream_IOutboundDataNotify*           outboundNotificationHandle;  // IO module(s)
  struct Common_ParserConfiguration*    parserConfiguration;         // parser module(s)
  bool                                  passive;                     // network/device/... module(s)
  bool                                  printFinalReport;            // statistic module(s)
  bool                                  pushStatisticMessages;       // source/statistic/... module(s)
  unsigned int                          reportingInterval;           // (statistic) reporting interval (second(s)) [0: off]
  ACE_HANDLE                            socketHandle;                // network module(s)
  ACE_Time_Value                        statisticCollectionInterval; // source/statistic/... module(s)
  ACE_Time_Value                        statisticReportingInterval;  // [ACE_Time_Value::zero: off]
  ACE_SYNCH_RECURSIVE_MUTEX*            subscribersLock;             // message handler module
  Common_ITimer_t*                      timerManager;
};

struct Stream_Configuration;
struct Stream_ModuleConfiguration
{
  Stream_ModuleConfiguration ()
   : generateUniqueNames (false) // module-
   , notify (NULL)
   , stream (NULL)
  {};

  bool              generateUniqueNames;
  Stream_INotify_t* notify; // *WARNING*: automatically set; DON'T TOUCH
  // *NOTE*: modules can use this to temporarily relinquish the stream lock
  //         while they wait on some condition, in order to avoid deadlocks
  //         --> to be used primarily in 'non-concurrent' (see above) scenarios
  Stream_IStream_t* stream; // *WARNING*: automatically set; DON'T TOUCH
};

struct Stream_Configuration
{
  Stream_Configuration ()
   : cloneModule (false) // *NOTE*: cloneModule ==> deleteModule
   , deleteModule (false)
   , finishOnDisconnect (false)
#if defined (ACE_WIN32) || defined (ACE_WIN64)
   , mediaFramework (MODULE_LIB_DEFAULT_MEDIAFRAMEWORK)
#endif
   , messageAllocator (NULL)
   , module (NULL)
   , notificationStrategy (NULL)
   , printFinalReport (false)
   , resetSessionData (true)
   , serializeOutput (false)
   , sessionId (0)
   , setupPipeline (true)
   //, useReactor ()
   , userData (NULL)
  {};

  bool                            cloneModule; // final-
  bool                            deleteModule; // final-
  bool                            finishOnDisconnect; // (network) i/o streams
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  enum Stream_MediaFramework_Type mediaFramework;
#endif
  Stream_IAllocator*              messageAllocator;
  Stream_Module_t*                module; // final-
  ACE_Notification_Strategy*      notificationStrategy;
  bool                            printFinalReport;
  bool                            resetSessionData;
  // *IMPORTANT NOTE*: in a multi-threaded environment, threads MAY be
  //                   dispatching the reactor notification queue concurrently
  //                   (most notably, ACE_TP_Reactor)
  //                   --> enforce proper serialization
  bool                            serializeOutput;
  Stream_SessionId_t              sessionId;
  bool                            setupPipeline;
  //bool                       useReactor;

  struct Stream_UserData*         userData;
};

template <//const char* StreamName,
          ////////////////////////////////
          typename AllocatorConfigurationType,
          typename ConfigurationType,
          typename ModuleConfigurationType,
          typename ModuleHandlerConfigurationType>
class Stream_Configuration_T
 : public std::map<std::string,                                // key:   module name
                   std::pair<ModuleConfigurationType,
                             ModuleHandlerConfigurationType> > // value: (pair of) module/handler configuration
 , public Common_IDumpState
{
  typedef std::map<std::string,
                   std::pair<ModuleConfigurationType,
                             ModuleHandlerConfigurationType> > inherited;

 public:
  // convenient types
  typedef std::map<std::string,
                   std::pair<ModuleConfigurationType,
                             ModuleHandlerConfigurationType> > MAP_T;
  typedef typename MAP_T::iterator ITERATOR_T;
  typedef typename MAP_T::const_iterator CONST_ITERATOR_T;

  Stream_Configuration_T ();
  inline virtual ~Stream_Configuration_T () {}

  bool initialize (const ModuleConfigurationType&,        // 'default' module configuration
                   const ModuleHandlerConfigurationType&, // 'default' module handler configuration
                   const AllocatorConfigurationType&,
                   const ConfigurationType&);

  virtual void dump_state () const;

  AllocatorConfigurationType allocatorConfiguration_;
  ConfigurationType          configuration_;
  bool                       isInitialized_;
};

// include template definition
#include "stream_configuration.inl"

//////////////////////////////////////////

//extern const char empty_string_[];
typedef Stream_Configuration_T<//empty_string_,
                               struct Stream_AllocatorConfiguration,
                               struct Stream_Configuration,
                               struct Stream_ModuleConfiguration,
                               struct Stream_ModuleHandlerConfiguration> Stream_Configuration_t;

#endif
