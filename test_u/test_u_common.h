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

#ifndef TEST_U_STREAM_COMMON_H
#define TEST_U_STREAM_COMMON_H

#include <deque>
#include <list>

#include "common.h"

#include "ace/Synch_Traits.h"

#include "common_inotify.h"
#include "common_isubscribe.h"

#include "common_ui_common.h"

#include "stream_common.h"
#include "stream_messageallocatorheap_base.h"
#include "stream_session_message.h"

// forward declarations
struct Stream_Test_U_Configuration;

enum Stream_GTK_Event
{
  STREAM_GTKEVENT_START = 0,
  STREAM_GTKEVENT_DATA,
  STREAM_GTKEVENT_END,
  STREAM_GTKEVENT_STATISTICS,
  // ----------------------
  STREAM_GTKEVENT_MAX,
  STREAM_GKTEVENT_INVALID
};
typedef std::deque<Stream_GTK_Event> Stream_GTK_Events_t;
typedef Stream_GTK_Events_t::const_iterator Stream_GTK_EventsIterator_t;

//struct Stream_Test_U_SessionData
//{
//  inline Stream_SessionData ()
//   : aborted (false)
//   , currentStatistic ()
//   , lock (NULL)
//   , sessionID (0)
//   , startOfSession (ACE_Time_Value::zero)
//  {};
//
//  bool             aborted;
//
//  Stream_Statistic currentStatistic;
//  ACE_SYNCH_MUTEX* lock;
//
//  unsigned int     sessionID; // (== socket handle !)
//  ACE_Time_Value   startOfSession;
//};

//struct Stream_Test_U_UserData
//{
//  inline Stream_Test_U_UserData ()
//   : configuration (NULL)
//  {};
//
//  Stream_Test_U_Configuration* configuration;
//};
//
//struct Stream_Test_U_StreamState
//{
//  inline Stream_Test_U_StreamState ()
//   : currentSessionData (NULL)
//   , userData (NULL)
//  {};
//
//  Stream_Test_U_SessionData* currentSessionData;
//  Stream_Test_U_UserData*    userData;
//};

typedef Stream_MessageBase Stream_Message_t;
typedef Stream_MessageAllocatorHeapBase_T<Stream_Message_t,
                                          Stream_SessionMessage> Stream_MessageAllocator_t;

typedef Stream_SessionDataBase_T<Stream_SessionData> Stream_SessionData_t;

typedef Common_INotify_T<Stream_SessionData,
                         Stream_Message_t> Stream_IStreamNotify_t;
typedef std::list<Stream_IStreamNotify_t*> Stream_Subscribers_t;
typedef Stream_Subscribers_t::iterator Net_SubscribersIterator_t;

typedef Common_ISubscribe_T<Stream_IStreamNotify_t> Stream_ISubscribe_t;

struct Stream_Test_U_Configuration
{
  inline Stream_Test_U_Configuration ()
   : moduleConfiguration_2 ()
   , moduleHandlerConfiguration_2 ()
   , streamConfiguration ()
   , streamUserData ()
  {};

  Stream_ModuleConfiguration        moduleConfiguration_2;
  Stream_ModuleHandlerConfiguration moduleHandlerConfiguration_2;
  Stream_Configuration              streamConfiguration;
  Stream_UserData                   streamUserData;
};

struct Stream_GTK_CBData
 : Common_UI_GTKState
{
  inline Stream_GTK_CBData ()
   : Common_UI_GTKState ()
   , allowUserRuntimeStatistic (true)
   , configuration (NULL)
   , eventStack ()
   , logStack ()
   , stackLock ()
   , subscribers ()
   , subscribersLock ()
  {};

  bool                         allowUserRuntimeStatistic;
  Stream_Test_U_Configuration* configuration;
  Stream_GTK_Events_t          eventStack;
  Common_MessageStack_t        logStack;
  ACE_SYNCH_RECURSIVE_MUTEX    stackLock;
  Stream_Subscribers_t         subscribers;
  ACE_SYNCH_RECURSIVE_MUTEX    subscribersLock;
};

#endif
