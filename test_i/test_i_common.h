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

#ifndef TEST_I_COMMON_H
#define TEST_I_COMMON_H

#include <deque>
#include <list>
#include <map>
#include <set>
#include <string>

#include "ace/Synch_Traits.h"

#include "gtk/gtk.h"

#include "common.h"
#include "common_inotify.h"
#include "common_istatistic.h"
#include "common_isubscribe.h"
#include "common_time_common.h"

#include "common_ui_common.h"

#include "stream_base.h"
#include "stream_common.h"
#include "stream_messageallocatorheap_base.h"
#include "stream_session_data_base.h"
#include "stream_statemachine_control.h"

#include "stream_module_net_common.h"

#include "net_defines.h"

#include "test_i_connection_common.h"
#include "test_i_connection_manager_common.h"
#include "test_i_defines.h"
//#include "test_i_message.h"
//#include "test_i_session_message.h"

// forward declarations
class Stream_IAllocator;
class Stream_Message;
class Stream_SessionMessage;
struct Test_I_ConnectionState;

enum Stream_GTK_Event
{
  STREAM_GKTEVENT_INVALID = -1,
  // ------------------------------------
  STREAM_GTKEVENT_START = 0,
  STREAM_GTKEVENT_DATA,
  STREAM_GTKEVENT_END,
  STREAM_GTKEVENT_STATISTIC,
  // ------------------------------------
  STREAM_GTKEVENT_MAX
};
typedef std::deque<Stream_GTK_Event> Stream_GTK_Events_t;
typedef Stream_GTK_Events_t::const_iterator Stream_GTK_EventsIterator_t;

typedef int Stream_HeaderType_t;
typedef int Stream_CommandType_t;

typedef Stream_Statistic Test_I_RuntimeStatistic_t;

typedef Common_IStatistic_T<Test_I_RuntimeStatistic_t> Test_I_StatisticReportingHandler_t;

struct Test_I_Stream_SessionData
 : Stream_SessionData
{
  inline Test_I_Stream_SessionData ()
   : Stream_SessionData ()
   , connectionState (NULL)
   , fileName ()
   , size (0)
  {};

  Test_I_ConnectionState* connectionState;
  std::string             fileName;
  unsigned int            size;
};
typedef Stream_SessionDataBase_T<Test_I_Stream_SessionData> Test_I_Stream_SessionData_t;

struct Test_I_Configuration;
struct Test_I_Stream_Configuration;
struct Test_I_Stream_UserData
{
  inline Test_I_Stream_UserData ()
   : configuration (NULL)
   , streamConfiguration (NULL)
  {};

  Test_I_Configuration*        configuration;
  Test_I_Stream_Configuration* streamConfiguration;
};

struct Test_I_Stream_SocketHandlerConfiguration
 : Stream_SocketHandlerConfiguration
{
  inline Test_I_Stream_SocketHandlerConfiguration ()
   : Stream_SocketHandlerConfiguration ()
     ////////////////////////////////////
   , userData (NULL)
  {};

  Test_I_Stream_UserData* userData;
};

// forward declarations
struct Test_I_Configuration;
typedef Stream_Base_T<ACE_MT_SYNCH,
                      Common_TimePolicy_t,
                      /////////////////
                      Stream_StateMachine_ControlState,
                      Test_I_Stream_State,
                      /////////////////
                      Test_I_Stream_Configuration,
                      /////////////////
                      Test_I_RuntimeStatistic_t,
                      /////////////////
                      Stream_ModuleConfiguration,
                      Test_I_Stream_ModuleHandlerConfiguration,
                      /////////////////
                      Test_I_Stream_SessionData,   // session data
                      Test_I_Stream_SessionData_t, // session data container (reference counted)
                      Stream_SessionMessage,
                      Stream_Message> Stream_Base_t;
struct Test_I_Stream_ModuleHandlerConfiguration
 : Stream_ModuleHandlerConfiguration
{
  inline Test_I_Stream_ModuleHandlerConfiguration ()
   : Stream_ModuleHandlerConfiguration ()
   , configuration (NULL)
   , connection (NULL)
   , connectionManager (NULL)
   , contextID (0)
   , fileName ()
   , inbound (false)
   , printProgressDot (false)
   , stream (NULL)
  {};

  Test_I_Configuration*                  configuration;
  Test_I_IConnection_t*                  connection; // TCP target/IO module
  Test_I_Stream_InetConnectionManager_t* connectionManager; // TCP IO module
  guint                                  contextID;
  std::string                            fileName; // file reader/writer module
  bool                                   inbound; // TCP IO module
  bool                                   printProgressDot;
  Stream_Base_t*                         stream;
};

struct Stream_SignalHandlerConfiguration
{
  inline Stream_SignalHandlerConfiguration ()
   : //messageAllocator (NULL)
   /*,*/ statisticReportingInterval (0)
  {};

  //Stream_IAllocator* messageAllocator;
  unsigned int       statisticReportingInterval; // statistics collecting interval (second(s)) [0: off]
};

struct Test_I_Stream_Configuration
 : Stream_Configuration
{
  inline Test_I_Stream_Configuration ()
   : Stream_Configuration ()
   , moduleConfiguration_2 ()
   , moduleHandlerConfiguration_2 ()
  {};

  Stream_ModuleConfiguration               moduleConfiguration_2;
  Test_I_Stream_ModuleHandlerConfiguration moduleHandlerConfiguration_2;
};

struct Test_I_Stream_State
{
  inline Test_I_Stream_State ()
   : currentSessionData (NULL)
   , userData (NULL)
  {};

  Test_I_Stream_SessionData* currentSessionData;
  Test_I_Stream_UserData*    userData;
};

struct Test_I_Configuration
{
  inline Test_I_Configuration ()
   : signalHandlerConfiguration ()
   , socketConfiguration ()
   , socketHandlerConfiguration ()
   , streamConfiguration ()
   , streamUserData ()
  {};

  // **************************** signal data **********************************
  Stream_SignalHandlerConfiguration        signalHandlerConfiguration;
  // **************************** socket data **********************************
  Net_SocketConfiguration                  socketConfiguration;
  Test_I_Stream_SocketHandlerConfiguration socketHandlerConfiguration;
  // **************************** stream data **********************************
  Test_I_Stream_Configuration              streamConfiguration;
  Test_I_Stream_UserData                   streamUserData;
};

typedef Stream_MessageAllocatorHeapBase_T<Stream_Message,
                                          Stream_SessionMessage> Stream_MessageAllocator_t;

typedef Common_INotify_T<Test_I_Stream_SessionData,
                         Stream_Message,
                         Stream_SessionMessage> Stream_IStreamNotify_t;
typedef std::list<Stream_IStreamNotify_t*> Stream_Subscribers_t;
typedef Stream_Subscribers_t::iterator Stream_SubscribersIterator_t;

typedef Common_ISubscribe_T<Stream_IStreamNotify_t> Stream_ISubscribe_t;

struct Stream_ThreadID
{
 inline Stream_ThreadID ()
  : handle (ACE_INVALID_HANDLE)
  , id (-1)
 {};

  ACE_hthread_t handle;
  ACE_thread_t  id;
};

typedef std::map<guint, Stream_ThreadID> Stream_PendingActions_t;
typedef Stream_PendingActions_t::iterator Stream_PendingActionsIterator_t;
typedef std::set<guint> Stream_CompletedActions_t;
typedef Stream_CompletedActions_t::iterator Stream_CompletedActionsIterator_t;
struct Stream_GTK_ProgressData
{
  inline Stream_GTK_ProgressData ()
   : completedActions ()
//   , cursorType (GDK_LAST_CURSOR)
   , GTKState (NULL)
   , pendingActions ()
   , sent (0)
   , size (0)
  {};

  Stream_CompletedActions_t completedActions;
//  GdkCursorType                      cursorType;
  Common_UI_GTKState*       GTKState;
  Stream_PendingActions_t   pendingActions;
  unsigned int              sent; // bytes
  unsigned int              size; // bytes
};

struct Stream_GTK_CBData
 : Common_UI_GTKState
{
  inline Stream_GTK_CBData ()
   : Common_UI_GTKState ()
   , configuration (NULL)
   , eventStack ()
   , logStack ()
   , progressData ()
   , stream (NULL)
   , subscribers ()
   , subscribersLock ()
  {};

  Test_I_Configuration*     configuration;
  Stream_GTK_Events_t       eventStack;
  Common_MessageStack_t     logStack;
  Stream_GTK_ProgressData   progressData;
  Stream_Base_t*            stream;
  Stream_Subscribers_t      subscribers;
  ACE_SYNCH_RECURSIVE_MUTEX subscribersLock;
};

struct Stream_ThreadData
{
  inline Stream_ThreadData ()
   : CBData (NULL)
   , eventSourceID (0)
  {};

  Stream_GTK_CBData* CBData;
  guint              eventSourceID;
};

#endif
