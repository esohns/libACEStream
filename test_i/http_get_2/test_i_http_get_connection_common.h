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

#include <ace/INET_Addr.h>
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
#include <ace/Netlink_Addr.h>
#endif
#include <ace/Synch_Traits.h>
#include <ace/SSL/SSL_SOCK_Stream.h>

#include "stream_common.h"
#include "stream_control_message.h"
#include "stream_module_io_stream.h"
#include "stream_session_data.h"

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


#include "test_i_connection_common.h"

// forward declarations
struct Test_I_AllocatorConfiguration;
class Test_I_Stream_Message;
class Test_I_Stream_SessionMessage;
typedef Stream_ControlMessage_T<enum Stream_ControlMessageType,
                                struct Test_I_AllocatorConfiguration,
                                Test_I_Stream_Message,
                                Test_I_Stream_SessionMessage> Test_I_ControlMessage_t;
struct Test_I_HTTPGet_Configuration;
struct Test_I_HTTPGet_ConnectionState;
typedef Stream_Statistic Test_I_RuntimeStatistic_t;
struct Test_I_HTTPGet_StreamConfiguration;
struct Test_I_HTTPGet_ModuleHandlerConfiguration;
struct Test_I_HTTPGet_SessionData;
typedef Stream_SessionData_T<Test_I_HTTPGet_SessionData> Test_I_HTTPGet_SessionData_t;
struct Test_I_HTTPGet_StreamState;
struct Test_I_HTTPGet_UserData;
typedef Net_Connection_Manager_T<ACE_INET_Addr,
                                 Test_I_HTTPGet_Configuration,
                                 Test_I_HTTPGet_ConnectionState,
                                 Test_I_RuntimeStatistic_t,
                                 Test_I_HTTPGet_UserData> Test_I_HTTPGet_InetConnectionManager_t;

//////////////////////////////////////////

typedef Stream_Module_Net_IO_Stream_T<ACE_MT_SYNCH,
                                      ACE_MT_SYNCH,
                                      Common_TimePolicy_t,
                                      int,
                                      enum Stream_SessionMessageType,
                                      enum Stream_StateMachine_ControlState,
                                      struct Test_I_HTTPGet_StreamState,
                                      struct Test_I_HTTPGet_StreamConfiguration,
                                      Test_I_RuntimeStatistic_t,
                                      struct Stream_ModuleConfiguration,
                                      struct Test_I_HTTPGet_ModuleHandlerConfiguration,
                                      struct Test_I_HTTPGet_SessionData,
                                      Test_I_HTTPGet_SessionData_t,
                                      Test_I_ControlMessage_t,
                                      Test_I_Stream_Message,
                                      Test_I_Stream_SessionMessage,
                                      ACE_INET_Addr,
                                      Test_I_HTTPGet_InetConnectionManager_t> Test_I_NetStream_t;

//////////////////////////////////////////

struct Test_I_HTTPGet_SocketHandlerConfiguration
 : Net_SocketHandlerConfiguration
{
  inline Test_I_HTTPGet_SocketHandlerConfiguration ()
   : Net_SocketHandlerConfiguration ()
   ///////////////////////////////////////
   , userData (NULL)
  {};

  struct Test_I_HTTPGet_UserData* userData;
};

struct Test_I_HTTPGet_ConnectionState
 : Test_I_ConnectionState
{
  inline Test_I_HTTPGet_ConnectionState ()
   : Test_I_ConnectionState ()
   , configuration (NULL)
   , userData (NULL)
  {};

  // *TODO*: consider making this a separate entity (i.e. a pointer)
  struct Test_I_HTTPGet_Configuration* configuration;

  struct Test_I_HTTPGet_UserData*      userData;
};

//////////////////////////////////////////

typedef Net_IConnection_T<ACE_INET_Addr,
                          struct Test_I_HTTPGet_Configuration,
                          struct Test_I_HTTPGet_ConnectionState,
                          Test_I_RuntimeStatistic_t> Test_I_IConnection_t;

//////////////////////////////////////////

// outbound
typedef Net_StreamTCPSocketBase_T<Net_TCPSocketHandler_T<struct Test_I_HTTPGet_SocketHandlerConfiguration,
                                                         ACE_SOCK_STREAM>,
                                  ACE_INET_Addr,
                                  struct Test_I_HTTPGet_Configuration,
                                  struct Test_I_HTTPGet_ConnectionState,
                                  Test_I_RuntimeStatistic_t,
                                  Test_I_NetStream_t,
                                  struct Test_I_HTTPGet_UserData,
                                  struct Stream_ModuleConfiguration,
                                  struct Test_I_HTTPGet_ModuleHandlerConfiguration> Test_I_TCPHandler_t;
typedef Net_StreamTCPSocketBase_T<Net_TCPSocketHandler_T<struct Test_I_HTTPGet_SocketHandlerConfiguration,
                                                         ACE_SSL_SOCK_Stream>,
                                  ACE_INET_Addr,
                                  struct Test_I_HTTPGet_Configuration,
                                  struct Test_I_HTTPGet_ConnectionState,
                                  Test_I_RuntimeStatistic_t,
                                  Test_I_NetStream_t,
                                  struct Test_I_HTTPGet_UserData,
                                  struct Stream_ModuleConfiguration,
                                  struct Test_I_HTTPGet_ModuleHandlerConfiguration> Test_I_SSLTCPHandler_t;
typedef Net_StreamAsynchTCPSocketBase_T<Net_AsynchTCPSocketHandler_T<struct Test_I_HTTPGet_SocketHandlerConfiguration>,
                                        ACE_INET_Addr,
                                        struct Test_I_HTTPGet_Configuration,
                                        struct Test_I_HTTPGet_ConnectionState,
                                        Test_I_RuntimeStatistic_t,
                                        Test_I_NetStream_t,
                                        struct Test_I_HTTPGet_UserData,
                                        struct Stream_ModuleConfiguration,
                                        struct Test_I_HTTPGet_ModuleHandlerConfiguration> Test_I_AsynchTCPHandler_t;

typedef Net_TCPConnectionBase_T<Test_I_TCPHandler_t,
                                struct Test_I_HTTPGet_Configuration,
                                struct Test_I_HTTPGet_ConnectionState,
                                Test_I_RuntimeStatistic_t,
                                struct Test_I_HTTPGet_SocketHandlerConfiguration,
                                Test_I_NetStream_t,
                                struct Test_I_HTTPGet_UserData> Test_I_TCPConnection_t;
typedef Net_TCPConnectionBase_T<Test_I_SSLTCPHandler_t,
                                struct Test_I_HTTPGet_Configuration,
                                struct Test_I_HTTPGet_ConnectionState,
                                Test_I_RuntimeStatistic_t,
                                struct Test_I_HTTPGet_SocketHandlerConfiguration,
                                Test_I_NetStream_t,
                                struct Test_I_HTTPGet_UserData> Test_I_SSLTCPConnection_t;
typedef Net_AsynchTCPConnectionBase_T<Test_I_AsynchTCPHandler_t,
                                      struct Test_I_HTTPGet_Configuration,
                                      struct Test_I_HTTPGet_ConnectionState,
                                      Test_I_RuntimeStatistic_t,
                                      struct Test_I_HTTPGet_SocketHandlerConfiguration,
                                      Test_I_NetStream_t,
                                      struct Test_I_HTTPGet_UserData> Test_I_AsynchTCPConnection_t;

//////////////////////////////////////////

typedef Net_IConnector_T<ACE_INET_Addr,
                         struct Test_I_HTTPGet_SocketHandlerConfiguration> Test_I_HTTPGet_IInetConnector_t;

//////////////////////////////////////////

// outbound
typedef Net_Client_Connector_T<Test_I_TCPConnection_t,
                               Net_SOCK_Connector,
                               ACE_INET_Addr,
                               struct Test_I_HTTPGet_Configuration,
                               struct Test_I_HTTPGet_ConnectionState,
                               Test_I_RuntimeStatistic_t,
                               struct Test_I_HTTPGet_SocketHandlerConfiguration,
                               Test_I_NetStream_t,
                               struct Test_I_HTTPGet_UserData> Test_I_HTTPGet_TCPConnector_t;
typedef Net_Client_SSL_Connector_T<Test_I_SSLTCPConnection_t,
                                   ACE_SSL_SOCK_Connector,
                                   ACE_INET_Addr,
                                   struct Test_I_HTTPGet_Configuration,
                                   struct Test_I_HTTPGet_ConnectionState,
                                   Test_I_RuntimeStatistic_t,
                                   struct Test_I_HTTPGet_SocketHandlerConfiguration,
                                   Test_I_NetStream_t,
                                   struct Test_I_HTTPGet_UserData> Test_I_HTTPGet_SSLTCPConnector_t;
typedef Net_Client_AsynchConnector_T<Test_I_AsynchTCPConnection_t,
                                     ACE_INET_Addr,
                                     struct Test_I_HTTPGet_Configuration,
                                     struct Test_I_HTTPGet_ConnectionState,
                                     Test_I_RuntimeStatistic_t,
                                     struct Test_I_HTTPGet_SocketHandlerConfiguration,
                                     Test_I_NetStream_t,
                                     struct Test_I_HTTPGet_UserData> Test_I_HTTPGet_TCPAsynchConnector_t;

#endif