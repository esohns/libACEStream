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

#ifndef TEST_I_TARGET_COMMON_H
#define TEST_I_TARGET_COMMON_H

#include "ace/os_include/sys/os_socket.h"

#include "net_ilistener.h"

#include "test_i_common.h"
#include "test_i_connection_manager_common.h"

struct Test_I_Target_ListenerConfiguration
{
  inline Test_I_Target_ListenerConfiguration ()
   : addressFamily (ACE_ADDRESS_FAMILY_INET)
   , connectionManager (NULL)
   , messageAllocator (NULL)
   , portNumber (0)
   , socketHandlerConfiguration (NULL)
   , statisticReportingInterval (0)
   , useLoopBackDevice (false)
  {};

  int                                       addressFamily;
  Test_I_Stream_IInetConnectionManager_t*   connectionManager;
  Stream_IAllocator*                        messageAllocator;
  unsigned short                            portNumber;
  Test_I_Stream_SocketHandlerConfiguration* socketHandlerConfiguration;
  unsigned int                              statisticReportingInterval; // (second(s)) [0: off]
  bool                                      useLoopBackDevice;
};

typedef Net_IListener_T<Test_I_Target_ListenerConfiguration,
                        Test_I_Stream_SocketHandlerConfiguration> Net_IListener_t;

struct Test_I_Target_SignalHandlerConfiguration
 : Stream_SignalHandlerConfiguration
{
  inline Test_I_Target_SignalHandlerConfiguration ()
   : Stream_SignalHandlerConfiguration ()
   , listener (NULL)
   , statisticReportingHandler (NULL)
   , statisticReportingTimerID (-1)
  {};

  Net_IListener_t*                    listener;
  Test_I_StatisticReportingHandler_t* statisticReportingHandler;
  long                                statisticReportingTimerID;
};

struct Test_I_Target_Configuration
 : Test_I_Configuration
{
  inline Test_I_Target_Configuration ()
   : Test_I_Configuration ()
   , listener (NULL)
   , listenerConfiguration ()
   , signalHandlerConfiguration ()
  {};

  Net_IListener_t*                         listener;
  Test_I_Target_ListenerConfiguration      listenerConfiguration;
  Test_I_Target_SignalHandlerConfiguration signalHandlerConfiguration;
};

struct Test_I_Target_GTK_CBData
 : Stream_GTK_CBData
{
  inline Test_I_Target_GTK_CBData ()
   : Stream_GTK_CBData ()
   , configuration (NULL)
  {};

  Test_I_Target_Configuration* configuration;
};

#endif
