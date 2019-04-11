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

#ifndef TEST_I_HTTP_GET_CONNECTION_COMMON_H
#define TEST_I_HTTP_GET_CONNECTION_COMMON_H

#include <map>
#include <string>

#include "ace/INET_Addr.h"
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
#include "ace/Netlink_Addr.h"
#endif
#include "ace/Synch_Traits.h"

#if defined (SSL_SUPPORT)
#include "ace/SSL/SSL_SOCK_Stream.h"
#endif

#include "common_timer_manager_common.h"

#include "stream_common.h"
#include "stream_control_message.h"
#include "stream_session_data.h"

#include "stream_net_io_stream.h"

#include "net_asynch_tcpsockethandler.h"
#include "net_configuration.h"
#include "net_connection_manager.h"
#include "net_sock_connector.h"
#include "net_stream_asynch_tcpsocket_base.h"
#include "net_stream_tcpsocket_base.h"
#include "net_tcpconnection_base.h"
#include "net_tcpsockethandler.h"

#include "net_client_asynchconnector.h"
#include "net_client_connector.h"
#include "net_client_ssl_connector.h"

#include "test_i_common.h"
#include "test_i_connection_common.h"

// forward declarations
class Test_I_Stream_Message;
class Test_I_Stream_SessionMessage;
typedef Stream_ControlMessage_T<enum Stream_ControlType,
                                enum Stream_ControlMessageType,
                                struct Common_FlexParserAllocatorConfiguration> Test_I_ControlMessage_t;
struct Test_I_HTTPGet_StreamConfiguration;
struct Test_I_HTTPGet_ModuleHandlerConfiguration;
struct Test_I_HTTPGet_SessionData;
typedef Stream_SessionData_T<struct Test_I_HTTPGet_SessionData> Test_I_HTTPGet_SessionData_t;
struct Test_I_HTTPGet_StreamState;
struct Stream_UserData;

//////////////////////////////////////////

struct Test_I_HTTPGet_Configuration;
struct Test_I_HTTPGet_ConnectionState
 : Test_I_ConnectionState
{
  Test_I_HTTPGet_ConnectionState ()
   : Test_I_ConnectionState ()
   , configuration (NULL)
   , userData (NULL)
  {};

  // *TODO*: consider making this a separate entity (i.e. a pointer)
  struct Test_I_HTTPGet_Configuration* configuration;

  struct Stream_UserData*      userData;
};

//extern const char stream_name_string_[];
struct Test_I_HTTPGet_StreamConfiguration;
struct Test_I_HTTPGet_ModuleHandlerConfiguration;
typedef Stream_Configuration_T<//stream_name_string_,
                               struct Common_FlexParserAllocatorConfiguration,
                               struct Test_I_HTTPGet_StreamConfiguration,
                               struct Stream_ModuleConfiguration,
                               struct Test_I_HTTPGet_ModuleHandlerConfiguration> Test_I_HTTPGet_StreamConfiguration_t;
typedef Net_ConnectionConfiguration_T<struct Common_FlexParserAllocatorConfiguration,
                                      Test_I_HTTPGet_StreamConfiguration_t,
                                      NET_TRANSPORTLAYER_TCP> Test_I_HTTPGet_ConnectionConfiguration_t;
typedef Net_Connection_Manager_T<ACE_MT_SYNCH,
                                 ACE_INET_Addr,
                                 Test_I_HTTPGet_ConnectionConfiguration_t,
                                 struct Test_I_HTTPGet_ConnectionState,
                                 Test_I_Statistic_t,
                                 struct Stream_UserData> Test_I_HTTPGet_InetConnectionManager_t;

//////////////////////////////////////////

typedef Net_IConnection_T<ACE_INET_Addr,
                          Test_I_HTTPGet_ConnectionConfiguration_t,
                          struct Test_I_HTTPGet_ConnectionState,
                          Test_I_Statistic_t> Test_I_IConnection_t;

//////////////////////////////////////////

//static constexpr const char network_io_stream_name_string_[] =
//    ACE_TEXT_ALWAYS_CHAR ("NetworkIOStream");
extern const char stream_name_string_[];
typedef Stream_Module_Net_IO_Stream_T<ACE_MT_SYNCH,
                                      Common_TimePolicy_t,
                                      stream_name_string_,
                                      enum Stream_ControlType,
                                      enum Stream_SessionMessageType,
                                      enum Stream_StateMachine_ControlState,
                                      struct Test_I_HTTPGet_StreamState,
                                      struct Test_I_HTTPGet_StreamConfiguration,
                                      Test_I_Statistic_t,
                                      Common_Timer_Manager_t,
                                      struct Common_FlexParserAllocatorConfiguration,
                                      struct Stream_ModuleConfiguration,
                                      struct Test_I_HTTPGet_ModuleHandlerConfiguration,
                                      struct Test_I_HTTPGet_SessionData,
                                      Test_I_HTTPGet_SessionData_t,
                                      Test_I_ControlMessage_t,
                                      Test_I_Stream_Message,
                                      Test_I_Stream_SessionMessage,
                                      ACE_INET_Addr,
                                      Test_I_HTTPGet_InetConnectionManager_t,
                                      struct Stream_UserData> Test_I_NetStream_t;

//////////////////////////////////////////

// outbound
typedef Net_TCPConnectionBase_T<ACE_MT_SYNCH,
                                Test_I_HTTPGet_ConnectionConfiguration_t,
                                struct Test_I_HTTPGet_ConnectionState,
                                Test_I_Statistic_t,
                                Test_I_NetStream_t,
                                Common_Timer_Manager_t,
                                struct Stream_UserData> Test_I_TCPConnection_t;
#if defined (SSL_SUPPORT)
typedef Net_SSLConnectionBase_T<ACE_MT_SYNCH,
                                Test_I_HTTPGet_ConnectionConfiguration_t,
                                struct Test_I_HTTPGet_ConnectionState,
                                Test_I_Statistic_t,
                                Test_I_NetStream_t,
                                Common_Timer_Manager_t,
                                struct Stream_UserData> Test_I_SSLTCPConnection_t;
#endif
typedef Net_AsynchTCPConnectionBase_T<Test_I_HTTPGet_ConnectionConfiguration_t,
                                      struct Test_I_HTTPGet_ConnectionState,
                                      Test_I_Statistic_t,
                                      Test_I_NetStream_t,
                                      Common_Timer_Manager_t,
                                      struct Stream_UserData> Test_I_AsynchTCPConnection_t;

//////////////////////////////////////////

typedef Net_IConnector_T<ACE_INET_Addr,
                         Test_I_HTTPGet_ConnectionConfiguration_t> Test_I_HTTPGet_IInetConnector_t;

//////////////////////////////////////////

// outbound
typedef Net_Client_Connector_T<ACE_MT_SYNCH,
                               Test_I_TCPConnection_t,
                               Net_SOCK_Connector,
                               ACE_INET_Addr,
                               Test_I_HTTPGet_ConnectionConfiguration_t,
                               struct Test_I_HTTPGet_ConnectionState,
                               Test_I_Statistic_t,
                               Test_I_HTTPGet_ConnectionConfiguration_t,
                               Test_I_HTTPGet_ConnectionConfiguration_t,
                               Test_I_NetStream_t,
                               struct Stream_UserData> Test_I_HTTPGet_TCPConnector_t;
#if defined (SSL_SUPPORT)
typedef Net_Client_SSL_Connector_T<Test_I_SSLTCPConnection_t,
                                   ACE_SSL_SOCK_Connector,
                                   ACE_INET_Addr,
                                   Test_I_HTTPGet_ConnectionConfiguration_t,
                                   struct Test_I_HTTPGet_ConnectionState,
                                   Test_I_Statistic_t,
                                   Net_TCPSocketConfiguration_t,
                                   Net_TCPSocketConfiguration_t,
                                   Test_I_NetStream_t,
                                   struct Stream_UserData> Test_I_HTTPGet_SSLTCPConnector_t;
#endif
typedef Net_Client_AsynchConnector_T<Test_I_AsynchTCPConnection_t,
                                     ACE_INET_Addr,
                                     Test_I_HTTPGet_ConnectionConfiguration_t,
                                     struct Test_I_HTTPGet_ConnectionState,
                                     Test_I_Statistic_t,
                                     Test_I_HTTPGet_ConnectionConfiguration_t,
                                     Test_I_HTTPGet_ConnectionConfiguration_t,
                                     Test_I_NetStream_t,
                                     struct Stream_UserData> Test_I_HTTPGet_TCPAsynchConnector_t;

#endif
