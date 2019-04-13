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

#ifndef TEST_I_CAMSTREAM_NETWORK_H
#define TEST_I_CAMSTREAM_NETWORK_H

#include <map>
#include <string>

#include "ace/config-lite.h"
#include "ace/INET_Addr.h"
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
#include "ace/Netlink_Addr.h"
#endif
#include "ace/SOCK_Connector.h"
#include "ace/Synch_Traits.h"
#if defined (SSL_SUPPORT)
#include "ace/SSL/SSL_SOCK_Stream.h"
#endif // SSL_SUPPORT

#include "common_statistic_handler.h"

#include "common_timer_manager_common.h"

#include "stream_common.h"
#include "stream_configuration.h"
#include "stream_control_message.h"
#include "stream_session_data.h"

#include "stream_net_io_stream.h"

#include "net_asynch_tcpsockethandler.h"
#include "net_asynch_udpsockethandler.h"
#include "net_common.h"
#include "net_connection_manager.h"
#include "net_sock_connector.h"
#include "net_stream_tcpsocket_base.h"
#include "net_stream_udpsocket_base.h"
#include "net_tcpconnection_base.h"
#include "net_udpconnection_base.h"
#include "net_tcpsockethandler.h"
#include "net_udpsockethandler.h"

#include "net_client_asynchconnector.h"
#include "net_client_connector.h"
#if defined (SSL_SUPPORT)
#include "net_client_ssl_connector.h"
#endif // SSL_SUPPORT

#include "test_i_connection_common.h"

#include "test_i_camstream_defines.h"
//#include "test_i_source_common.h"

// forward declarations
typedef Stream_ControlMessage_T<enum Stream_ControlType,
                                enum Stream_ControlMessageType,
                                struct Test_I_AllocatorConfiguration> Test_I_ControlMessage_t;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
struct Test_I_Source_DirectShow_ConnectionConfiguration;
struct Test_I_Source_DirectShow_ConnectionState;
struct Test_I_Source_DirectShow_ModuleHandlerConfiguration;
class Test_I_Source_DirectShow_SessionData;
struct Test_I_Source_DirectShow_SocketHandlerConfiguration;
struct Test_I_Source_DirectShow_StreamConfiguration;
class Test_I_Source_DirectShow_Stream_Message;
class Test_I_Source_DirectShow_SessionMessage;
struct Test_I_Source_DirectShow_StreamState;
struct Test_I_Source_DirectShow_UserData;

struct Test_I_Target_DirectShow_ConnectionConfiguration;
struct Test_I_Target_DirectShow_ConnectionState;
struct Test_I_Target_DirectShow_ModuleHandlerConfiguration;
class Test_I_Target_DirectShow_SessionData;
struct Test_I_Target_DirectShow_SocketHandlerConfiguration;
class Test_I_Target_DirectShow_Stream;
class Test_I_Target_DirectShow_Stream_Message;
class Test_I_Target_DirectShow_SessionMessage;
struct Test_I_Target_DirectShow_StreamConfiguration;
struct Test_I_Target_DirectShow_StreamState;

typedef Stream_SessionData_T<Test_I_Source_DirectShow_SessionData> Test_I_Source_DirectShow_SessionData_t;

typedef Stream_SessionData_T<Test_I_Target_DirectShow_SessionData> Test_I_Target_DirectShow_SessionData_t;

struct Test_I_Source_MediaFoundation_Configuration;
struct Test_I_Source_MediaFoundation_ConnectionState;
struct Test_I_Source_MediaFoundation_ModuleHandlerConfiguration;
class Test_I_Source_MediaFoundation_SessionData;
struct Test_I_Source_MediaFoundation_SocketHandlerConfiguration;
class Test_I_Source_MediaFoundation_Stream_Message;
class Test_I_Source_MediaFoundation_SessionMessage;
struct Test_I_Source_MediaFoundation_StreamConfiguration;
struct Test_I_Source_MediaFoundation_StreamState;
struct Test_I_Source_MediaFoundation_UserData;

struct Test_I_Target_MediaFoundation_Configuration;
struct Test_I_Target_MediaFoundation_ConnectionState;
struct Test_I_Target_MediaFoundation_ModuleHandlerConfiguration;
class Test_I_Target_MediaFoundation_SessionData;
struct Test_I_Target_MediaFoundation_SocketHandlerConfiguration;
class Test_I_Target_MediaFoundation_Stream;
class Test_I_Target_MediaFoundation_Stream_Message;
class Test_I_Target_MediaFoundation_SessionMessage;
struct Test_I_Target_MediaFoundation_StreamConfiguration;
struct Test_I_Target_MediaFoundation_StreamState;

typedef Stream_SessionData_T<Test_I_Source_MediaFoundation_SessionData> Test_I_Source_MediaFoundation_SessionData_t;

typedef Stream_SessionData_T<Test_I_Target_MediaFoundation_SessionData> Test_I_Target_MediaFoundation_SessionData_t;
#else
struct Test_I_Source_V4L_ConnectionConfiguration;
struct Test_I_Source_V4L_ConnectionState;
struct Test_I_Source_V4L_ModuleHandlerConfiguration;
class Test_I_Source_V4L_SessionData;
struct Test_I_Source_V4L_SocketHandlerConfiguration;
class Test_I_Source_V4L_Stream_Message;
class Test_I_Source_V4L_SessionMessage;
struct Test_I_Source_V4L_StreamConfiguration;
struct Test_I_Source_V4L_StreamState;

typedef Stream_SessionData_T<Test_I_Source_V4L_SessionData> Test_I_Source_V4L_SessionData_t;
#endif // ACE_WIN32 || ACE_WIN64

#if defined (ACE_WIN32) || defined (ACE_WIN64)
struct Test_I_Target_DirectShow_ConnectionConfiguration;
struct Test_I_Target_MediaFoundation_ConnectionConfiguration;
struct Test_I_Target_DirectShow_ConnectionState;
struct Test_I_Target_MediaFoundation_ConnectionState;
#else
struct Test_I_Target_ConnectionConfiguration;
struct Test_I_Target_ConnectionState;
struct Test_I_Target_ModuleHandlerConfiguration;
class Test_I_Target_SessionData;
struct Test_I_Target_SocketHandlerConfiguration;
class Test_I_Target_Stream;
class Test_I_Target_Stream_Message;
class Test_I_Target_SessionMessage;
struct Test_I_Target_StreamConfiguration;
struct Test_I_Target_StreamState;

typedef Stream_SessionData_T<Test_I_Target_SessionData> Test_I_Target_SessionData_t;
#endif

#if defined (ACE_WIN32) || defined (ACE_WIN64)
//typedef Net_Connection_Manager_T<ACE_INET_Addr,
//                                 Test_I_Target_DirectShow_ConnectionConfiguration_t,
//                                 struct Test_I_Target_DirectShow_ConnectionState,
//                                 Test_I_Statistic_t,
//                                 struct Test_I_Target_UserData> Test_I_Target_DirectShow_InetConnectionManager_t;
//typedef Net_Connection_Manager_T<ACE_INET_Addr,
//                                 Test_I_Target_MediaFoundation_ConnectionConfiguration_t,
//                                 struct Test_I_Target_MediaFoundation_ConnectionState,
//                                 Test_I_Statistic_t,
//                                 struct Test_I_Target_MediaFoundation_UserData> Test_I_Target_MediaFoundation_InetConnectionManager_t;
#else
//typedef Net_Connection_Manager_T<ACE_MT_SYNCH,
//                                 ACE_INET_Addr,
//                                 Test_I_Target_ConnectionConfiguration_t,
//                                 struct Test_I_Target_ConnectionState,
//                                 Test_I_Statistic_t,
//                                 struct Test_I_Target_UserData> Test_I_Target_InetConnectionManager_t;
//typedef Net_IConnectionManager_T<ACE_MT_SYNCH,
//                                 ACE_INET_Addr,
//                                 Test_I_Target_ConnectionConfiguration_t,
//                                 struct Test_I_Target_ConnectionState,
//                                 Test_I_Statistic_t,
//                                 struct Test_I_Target_UserData> Test_I_Target_IInetConnectionManager_t;
#endif
typedef Stream_Statistic Test_I_Statistic_t;

#if defined (ACE_WIN32) || defined (ACE_WIN64)
typedef Common_StatisticHandler_T<Test_I_Statistic_t> Test_I_Source_StatisticHandler_t;

struct Test_I_Source_DirectShow_ConnectionConfiguration;
struct Test_I_Source_DirectShow_SocketHandlerConfiguration
 : Net_SocketHandlerConfiguration
{
  Test_I_Source_DirectShow_SocketHandlerConfiguration ()
   : Net_SocketHandlerConfiguration ()
   ///////////////////////////////////////
   , socketConfiguration_2 ()
   , socketConfiguration_3 ()
   , connectionConfiguration (NULL)
   , userData (NULL)
  {
    socketConfiguration = &socketConfiguration_2;
  }

  struct Net_TCPSocketConfiguration                        socketConfiguration_2;
  struct Net_UDPSocketConfiguration                        socketConfiguration_3;
  struct Test_I_Source_DirectShow_ConnectionConfiguration* connectionConfiguration;

  struct Test_I_Source_DirectShow_UserData*                userData;
};
struct Test_I_Source_MediaFoundation_ConnectionConfiguration;
struct Test_I_Source_MediaFoundation_SocketHandlerConfiguration
 : Net_SocketHandlerConfiguration
{
  Test_I_Source_MediaFoundation_SocketHandlerConfiguration ()
   : Net_SocketHandlerConfiguration ()
   ///////////////////////////////////////
   , socketConfiguration_2 ()
   , socketConfiguration_3 ()
   , connectionConfiguration (NULL)
   , userData (NULL)
  {
    socketConfiguration = &socketConfiguration_2;
  }

  struct Net_TCPSocketConfiguration                             socketConfiguration_2;
  struct Net_UDPSocketConfiguration                             socketConfiguration_3;
  struct Test_I_Source_MediaFoundation_ConnectionConfiguration* connectionConfiguration;

  struct Test_I_Source_MediaFoundation_UserData*                userData;
};

//extern const char stream_name_string_[];
struct Test_I_Source_DirectShow_StreamConfiguration;
struct Test_I_Source_DirectShow_ModuleHandlerConfiguration;
typedef Stream_Configuration_T<//stream_name_string_,
                               struct Test_I_AllocatorConfiguration,
                               struct Test_I_Source_DirectShow_StreamConfiguration,
                               struct Stream_ModuleConfiguration,
                               struct Test_I_Source_DirectShow_ModuleHandlerConfiguration> Test_I_Source_DirectShow_StreamConfiguration_t;

struct Test_I_Source_DirectShow_ConnectionConfiguration;
typedef Net_ConnectionConfiguration_T<struct Test_I_Source_DirectShow_ConnectionConfiguration,
                                      struct Stream_AllocatorConfiguration,
                                      Test_I_Source_DirectShow_StreamConfiguration_t> Test_I_Source_DirectShow_ConnectionConfiguration_t;
typedef Net_IConnectionManager_T<ACE_MT_SYNCH,
                                 ACE_INET_Addr,
                                 Test_I_Source_DirectShow_ConnectionConfiguration_t,
                                 struct Test_I_Source_DirectShow_ConnectionState,
                                 struct Test_I_Source_Stream_StatisticData,
                                 struct Test_I_Source_DirectShow_UserData> Test_I_Source_DirectShow_IInetConnectionManager_t;
struct Test_I_Source_DirectShow_UserData;
struct Test_I_Source_DirectShow_ConnectionConfiguration
 : Net_ConnectionConfiguration
{
  Test_I_Source_DirectShow_ConnectionConfiguration ()
   : Net_ConnectionConfiguration ()
   ///////////////////////////////////////
   , connectionManager (NULL)
   , socketHandlerConfiguration ()
   , userData (NULL)
  {}

  Test_I_Source_DirectShow_IInetConnectionManager_t*         connectionManager;
  struct Test_I_Source_DirectShow_SocketHandlerConfiguration socketHandlerConfiguration;

  struct Test_I_Source_DirectShow_UserData*                  userData;
};
typedef std::map<std::string,
                 Test_I_Source_DirectShow_ConnectionConfiguration_t> Test_I_Source_DirectShow_ConnectionConfigurations_t;
typedef Test_I_Source_DirectShow_ConnectionConfigurations_t::iterator Test_I_Source_DirectShow_ConnectionConfigurationIterator_t;

typedef Net_Connection_Manager_T<ACE_MT_SYNCH,
                                 ACE_INET_Addr,
                                 Test_I_Source_DirectShow_ConnectionConfiguration_t,
                                 struct Test_I_Source_DirectShow_ConnectionState,
                                 struct Test_I_Source_Stream_StatisticData,
                                 struct Test_I_Source_DirectShow_UserData> Test_I_Source_DirectShow_InetConnectionManager_t;

struct Test_I_Source_MediaFoundation_SocketHandlerConfiguration;
struct Test_I_Source_MediaFoundation_StreamConfiguration;
struct Test_I_Source_MediaFoundation_ModuleHandlerConfiguration;
typedef Stream_Configuration_T<//stream_name_string_,
                               struct Test_I_AllocatorConfiguration,
                               struct Test_I_Source_MediaFoundation_StreamConfiguration,
                               struct Stream_ModuleConfiguration,
                               struct Test_I_Source_MediaFoundation_ModuleHandlerConfiguration> Test_I_Source_MediaFoundation_StreamConfiguration_t;
struct Test_I_Source_MediaFoundation_ConnectionConfiguration;
typedef Net_ConnectionConfiguration_T<struct Test_I_Source_MediaFoundation_ConnectionConfiguration,
                                      struct Stream_AllocatorConfiguration,
                                      Test_I_Source_MediaFoundation_StreamConfiguration_t> Test_I_Source_MediaFoundation_ConnectionConfiguration_t;
typedef Net_IConnectionManager_T<ACE_MT_SYNCH,
                                 ACE_INET_Addr,
                                 Test_I_Source_MediaFoundation_ConnectionConfiguration_t,
                                 struct Test_I_Source_MediaFoundation_ConnectionState,
                                 struct Test_I_Source_Stream_StatisticData,
                                 struct Test_I_Source_MediaFoundation_UserData> Test_I_Source_MediaFoundation_IInetConnectionManager_t;
struct Test_I_Source_MediaFoundation_UserData;
struct Test_I_Source_MediaFoundation_ConnectionConfiguration
 : Net_ConnectionConfiguration
{
  Test_I_Source_MediaFoundation_ConnectionConfiguration ()
   : Net_ConnectionConfiguration ()
   ///////////////////////////////////////
   , connectionManager (NULL)
   , socketHandlerConfiguration ()
   , userData (NULL)
  {}

  Test_I_Source_MediaFoundation_IInetConnectionManager_t*         connectionManager;
  struct Test_I_Source_MediaFoundation_SocketHandlerConfiguration socketHandlerConfiguration;

  struct Test_I_Source_MediaFoundation_UserData*                  userData;
};
typedef std::map<std::string,
                 Test_I_Source_MediaFoundation_ConnectionConfiguration_t> Test_I_Source_MediaFoundation_ConnectionConfigurations_t;
typedef Test_I_Source_MediaFoundation_ConnectionConfigurations_t::iterator Test_I_Source_MediaFoundation_ConnectionConfigurationIterator_t;

typedef Net_Connection_Manager_T<ACE_MT_SYNCH,
                                 ACE_INET_Addr,
                                 Test_I_Source_MediaFoundation_ConnectionConfiguration_t,
                                 struct Test_I_Source_MediaFoundation_ConnectionState,
                                 struct Test_I_Source_Stream_StatisticData,
                                 struct Test_I_Source_MediaFoundation_UserData> Test_I_Source_MediaFoundation_InetConnectionManager_t;
#else
//extern const char stream_name_string_[];
struct Test_I_Source_V4L_StreamConfiguration;
struct Test_I_Source_V4L_ModuleHandlerConfiguration;
typedef Stream_Configuration_T<//stream_name_string_,
                               struct Test_I_AllocatorConfiguration,
                               struct Test_I_Source_V4L_StreamConfiguration,
                               struct Stream_ModuleConfiguration,
                               struct Test_I_Source_V4L_ModuleHandlerConfiguration> Test_I_Source_V4L_StreamConfiguration_t;
struct Test_I_Source_V4L_ConnectionConfiguration;
typedef Net_ConnectionConfiguration_T<struct Stream_AllocatorConfiguration,
                                      Test_I_Source_V4L_StreamConfiguration_t,
                                      NET_TRANSPORTLAYER_TCP> Test_I_Source_V4L_TCPConnectionConfiguration_t;
typedef Net_ConnectionConfiguration_T<struct Stream_AllocatorConfiguration,
                                      Test_I_Source_V4L_StreamConfiguration_t,
                                      NET_TRANSPORTLAYER_UDP> Test_I_Source_V4L_UDPConnectionConfiguration_t;

typedef Net_IConnectionManager_T<ACE_MT_SYNCH,
                                 ACE_INET_Addr,
                                 Test_I_Source_V4L_TCPConnectionConfiguration_t,
                                 struct Test_I_Source_V4L_ConnectionState,
                                 struct Test_I_Source_Stream_StatisticData,
                                 struct Net_UserData> Test_I_Source_V4L_ITCPConnectionManager_t;
typedef Net_IConnectionManager_T<ACE_MT_SYNCH,
                                 ACE_INET_Addr,
                                 Test_I_Source_V4L_UDPConnectionConfiguration_t,
                                 struct Test_I_Source_V4L_ConnectionState,
                                 struct Test_I_Source_Stream_StatisticData,
                                 struct Net_UserData> Test_I_Source_V4L_IUDPConnectionManager_t;

typedef Net_Connection_Manager_T<ACE_MT_SYNCH,
                                 ACE_INET_Addr,
                                 Test_I_Source_V4L_TCPConnectionConfiguration_t,
                                 struct Test_I_Source_V4L_ConnectionState,
                                 struct Test_I_Source_Stream_StatisticData,
                                 struct Net_UserData> Test_I_Source_V4L_TCPConnectionManager_t;
typedef Net_Connection_Manager_T<ACE_MT_SYNCH,
                                 ACE_INET_Addr,
                                 Test_I_Source_V4L_UDPConnectionConfiguration_t,
                                 struct Test_I_Source_V4L_ConnectionState,
                                 struct Test_I_Source_Stream_StatisticData,
                                 struct Net_UserData> Test_I_Source_V4L_UDPConnectionManager_t;
#endif

#if defined (ACE_WIN32) || defined (ACE_WIN64)
struct Test_I_Target_DirectShow_ConnectionConfiguration;
struct Test_I_Target_DirectShow_SocketHandlerConfiguration
 : Net_SocketHandlerConfiguration
{
  Test_I_Target_DirectShow_SocketHandlerConfiguration ()
   : Net_SocketHandlerConfiguration ()
   ///////////////////////////////////////
   , socketConfiguration_2 ()
   , socketConfiguration_3 ()
   , connectionConfiguration (NULL)
   , userData (NULL)
  {
    socketConfiguration = &socketConfiguration_2;
  }

  struct Net_TCPSocketConfiguration                        socketConfiguration_2;
  struct Net_UDPSocketConfiguration                        socketConfiguration_3;
  struct Test_I_Target_DirectShow_ConnectionConfiguration* connectionConfiguration;

  struct Test_I_Target_UserData*                           userData;
};

struct Test_I_Target_DirectShow_StreamConfiguration;
struct Test_I_Target_DirectShow_ModuleHandlerConfiguration;
typedef Stream_Configuration_T<//stream_name_string_,
                               struct Test_I_AllocatorConfiguration,
                               struct Test_I_Target_DirectShow_StreamConfiguration,
                               struct Stream_ModuleConfiguration,
                               struct Test_I_Target_DirectShow_ModuleHandlerConfiguration> Test_I_Target_DirectShow_StreamConfiguration_t;
struct Test_I_Target_DirectShow_ConnectionConfiguration;
typedef Net_ConnectionConfiguration_T<struct Test_I_Target_DirectShow_ConnectionConfiguration,
                                      struct Stream_AllocatorConfiguration,
                                      Test_I_Target_DirectShow_StreamConfiguration_t> Test_I_Target_DirectShow_ConnectionConfiguration_t;
typedef Net_IConnectionManager_T<ACE_MT_SYNCH,
                                 ACE_INET_Addr,
                                 Test_I_Target_DirectShow_ConnectionConfiguration_t,
                                 struct Test_I_Target_DirectShow_ConnectionState,
                                 Test_I_Statistic_t,
                                 struct Test_I_Target_UserData> Test_I_Target_DirectShow_IInetConnectionManager_t;

struct Test_I_Target_DirectShow_ConnectionConfiguration
 : Net_ConnectionConfiguration
{
  Test_I_Target_DirectShow_ConnectionConfiguration ()
   : Net_ConnectionConfiguration ()
   ///////////////////////////////////////
   , connectionManager (NULL)
   , socketHandlerConfiguration ()
   , userData (NULL)
  {
    PDUSize = TEST_I_DEFAULT_FRAME_SIZE;
  }

  Test_I_Target_DirectShow_IInetConnectionManager_t*         connectionManager;
  struct Test_I_Target_DirectShow_SocketHandlerConfiguration socketHandlerConfiguration;

  struct Test_I_Target_UserData*                             userData;
};
typedef std::map<std::string,
                 Test_I_Target_DirectShow_ConnectionConfiguration_t> Test_I_Target_DirectShow_ConnectionConfigurations_t;
typedef Test_I_Target_DirectShow_ConnectionConfigurations_t::iterator Test_I_Target_DirectShow_ConnectionConfigurationIterator_t;

typedef Net_Connection_Manager_T<ACE_MT_SYNCH,
                                 ACE_INET_Addr,
                                 Test_I_Target_DirectShow_ConnectionConfiguration_t,
                                 struct Test_I_Target_DirectShow_ConnectionState,
                                 Test_I_Statistic_t,
                                 struct Test_I_Target_UserData> Test_I_Target_DirectShow_InetConnectionManager_t;

struct Test_I_Target_MediaFoundation_ConnectionConfiguration;
struct Test_I_Target_MediaFoundation_SocketHandlerConfiguration
 : Net_SocketHandlerConfiguration
{
  Test_I_Target_MediaFoundation_SocketHandlerConfiguration ()
   : Net_SocketHandlerConfiguration ()
   ///////////////////////////////////////
   , socketConfiguration_2 ()
   , socketConfiguration_3 ()
   , connectionConfiguration (NULL)
   , userData (NULL)
  {
    socketConfiguration = &socketConfiguration_2;
  }

  struct Net_TCPSocketConfiguration                             socketConfiguration_2;
  struct Net_UDPSocketConfiguration                             socketConfiguration_3;
  struct Test_I_Target_MediaFoundation_ConnectionConfiguration* connectionConfiguration;

  struct Test_I_Target_UserData*                                userData;
};

struct Test_I_Target_MediaFoundation_StreamConfiguration;
struct Test_I_Target_MediaFoundation_ModuleHandlerConfiguration;
typedef Stream_Configuration_T<//stream_name_string_,
                               struct Test_I_AllocatorConfiguration,
                               struct Test_I_Target_MediaFoundation_StreamConfiguration,
                               struct Stream_ModuleConfiguration,
                               struct Test_I_Target_MediaFoundation_ModuleHandlerConfiguration> Test_I_Target_MediaFoundation_StreamConfiguration_t;
struct Test_I_Target_MediaFoundation_ConnectionConfiguration;
typedef Net_ConnectionConfiguration_T<struct Test_I_Target_MediaFoundation_ConnectionConfiguration,
                                      struct Stream_AllocatorConfiguration,
                                      Test_I_Target_MediaFoundation_StreamConfiguration_t> Test_I_Target_MediaFoundation_ConnectionConfiguration_t;
typedef Net_IConnectionManager_T<ACE_MT_SYNCH,
                                 ACE_INET_Addr,
                                 Test_I_Target_MediaFoundation_ConnectionConfiguration_t,
                                 struct Test_I_Target_MediaFoundation_ConnectionState,
                                 Test_I_Statistic_t,
                                 struct Test_I_Target_UserData> Test_I_Target_MediaFoundation_IInetConnectionManager_t;
struct Test_I_Target_MediaFoundation_ConnectionConfiguration
 : Net_ConnectionConfiguration
{
  Test_I_Target_MediaFoundation_ConnectionConfiguration ()
   : Net_ConnectionConfiguration ()
   ///////////////////////////////////////
   , connectionManager (NULL)
   , socketHandlerConfiguration ()
   , streamConfiguration (NULL)
   , userData (NULL)
  {
    PDUSize = TEST_I_DEFAULT_FRAME_SIZE;
  }

  Test_I_Target_MediaFoundation_IInetConnectionManager_t*         connectionManager;
  struct Test_I_Target_MediaFoundation_SocketHandlerConfiguration socketHandlerConfiguration;
  Test_I_Target_MediaFoundation_StreamConfiguration_t*            streamConfiguration;

  struct Test_I_Target_UserData*                                  userData;
};
typedef std::map<std::string,
                 Test_I_Target_MediaFoundation_ConnectionConfiguration_t> Test_I_Target_MediaFoundation_ConnectionConfigurations_t;
typedef Test_I_Target_MediaFoundation_ConnectionConfigurations_t::iterator Test_I_Target_MediaFoundation_ConnectionConfigurationIterator_t;

typedef Net_Connection_Manager_T<ACE_MT_SYNCH,
                                 ACE_INET_Addr,
                                 Test_I_Target_MediaFoundation_ConnectionConfiguration_t,
                                 struct Test_I_Target_MediaFoundation_ConnectionState,
                                 Test_I_Statistic_t,
                                 struct Test_I_Target_UserData> Test_I_Target_MediaFoundation_InetConnectionManager_t;
#else
struct Test_I_Target_StreamConfiguration;
struct Test_I_Target_ModuleHandlerConfiguration;
typedef Stream_Configuration_T<//stream_name_string_,
                               struct Test_I_AllocatorConfiguration,
                               struct Test_I_Target_StreamConfiguration,
                               struct Stream_ModuleConfiguration,
                               struct Test_I_Target_ModuleHandlerConfiguration> Test_I_Target_StreamConfiguration_t;
typedef Net_ConnectionConfiguration_T<struct Stream_AllocatorConfiguration,
                                      Test_I_Target_StreamConfiguration_t,
                                      NET_TRANSPORTLAYER_TCP> Test_I_Target_TCPConnectionConfiguration_t;
typedef Net_ConnectionConfiguration_T<struct Stream_AllocatorConfiguration,
                                      Test_I_Target_StreamConfiguration_t,
                                      NET_TRANSPORTLAYER_UDP> Test_I_Target_UDPConnectionConfiguration_t;

typedef Net_IConnectionManager_T<ACE_MT_SYNCH,
                                 ACE_INET_Addr,
                                 Test_I_Target_TCPConnectionConfiguration_t,
                                 struct Test_I_Target_ConnectionState,
                                 Test_I_Statistic_t,
                                 struct Net_UserData> Test_I_Target_ITCPConnectionManager_t;
typedef Net_IConnectionManager_T<ACE_MT_SYNCH,
                                 ACE_INET_Addr,
                                 Test_I_Target_UDPConnectionConfiguration_t,
                                 struct Test_I_Target_ConnectionState,
                                 Test_I_Statistic_t,
                                 struct Net_UserData> Test_I_Target_IUDPConnectionManager_t;
typedef Net_Connection_Manager_T<ACE_MT_SYNCH,
                                 ACE_INET_Addr,
                                 Test_I_Target_TCPConnectionConfiguration_t,
                                 struct Test_I_Target_ConnectionState,
                                 Test_I_Statistic_t,
                                 struct Net_UserData> Test_I_Target_TCPConnectionManager_t;
typedef Net_Connection_Manager_T<ACE_MT_SYNCH,
                                 ACE_INET_Addr,
                                 Test_I_Target_UDPConnectionConfiguration_t,
                                 struct Test_I_Target_ConnectionState,
                                 Test_I_Statistic_t,
                                 struct Net_UserData> Test_I_Target_UDPConnectionManager_t;
#endif

//////////////////////////////////////////

extern const char stream_name_string_[];

#if defined (ACE_WIN32) || defined (ACE_WIN64)
typedef Stream_Module_Net_IO_Stream_T<ACE_MT_SYNCH,
                                      Common_TimePolicy_t,
                                      stream_name_string_,
                                      enum Stream_ControlType,
                                      enum Stream_SessionMessageType,
                                      enum Stream_StateMachine_ControlState,
                                      struct Test_I_Source_DirectShow_StreamState,
                                      struct Test_I_Source_DirectShow_StreamConfiguration,
                                      struct Test_I_Source_Stream_StatisticData,
                                      Common_Timer_Manager_t,
                                      struct Test_I_AllocatorConfiguration,
                                      struct Stream_ModuleConfiguration,
                                      struct Test_I_Source_DirectShow_ModuleHandlerConfiguration,
                                      Test_I_Source_DirectShow_SessionData,
                                      Test_I_Source_DirectShow_SessionData_t,
                                      Test_I_ControlMessage_t,
                                      Test_I_Source_DirectShow_Stream_Message,
                                      Test_I_Source_DirectShow_SessionMessage,
                                      ACE_INET_Addr,
                                      Test_I_Source_DirectShow_InetConnectionManager_t,
                                      struct Test_I_Source_DirectShow_UserData> Test_I_Source_DirectShow_NetStream_t;
typedef Stream_Module_Net_IO_Stream_T<ACE_MT_SYNCH,
                                      Common_TimePolicy_t,
                                      stream_name_string_,
                                      enum Stream_ControlType,
                                      enum Stream_SessionMessageType,
                                      enum Stream_StateMachine_ControlState,
                                      struct Test_I_Source_MediaFoundation_StreamState,
                                      struct Test_I_Source_MediaFoundation_StreamConfiguration,
                                      struct Test_I_Source_Stream_StatisticData,
                                      Common_Timer_Manager_t,
                                      struct Test_I_AllocatorConfiguration,
                                      struct Stream_ModuleConfiguration,
                                      struct Test_I_Source_MediaFoundation_ModuleHandlerConfiguration,
                                      Test_I_Source_MediaFoundation_SessionData,
                                      Test_I_Source_MediaFoundation_SessionData_t,
                                      Test_I_ControlMessage_t,
                                      Test_I_Source_MediaFoundation_Stream_Message,
                                      Test_I_Source_MediaFoundation_SessionMessage,
                                      ACE_INET_Addr,
                                      Test_I_Source_MediaFoundation_InetConnectionManager_t,
                                      struct Test_I_Source_MediaFoundation_UserData> Test_I_Source_MediaFoundation_NetStream_t;
#else
typedef Stream_Module_Net_IO_Stream_T<ACE_MT_SYNCH,
                                      Common_TimePolicy_t,
                                      stream_name_string_,
                                      enum Stream_ControlType,
                                      enum Stream_SessionMessageType,
                                      enum Stream_StateMachine_ControlState,
                                      struct Test_I_Source_V4L_StreamState,
                                      struct Test_I_Source_V4L_StreamConfiguration,
                                      struct Test_I_Source_Stream_StatisticData,
                                      Common_Timer_Manager_t,
                                      struct Test_I_AllocatorConfiguration,
                                      struct Stream_ModuleConfiguration,
                                      struct Test_I_Source_V4L_ModuleHandlerConfiguration,
                                      Test_I_Source_V4L_SessionData,
                                      Test_I_Source_V4L_SessionData_t,
                                      Test_I_ControlMessage_t,
                                      Test_I_Source_V4L_Stream_Message,
                                      Test_I_Source_V4L_SessionMessage,
                                      ACE_INET_Addr,
                                      Test_I_Source_V4L_TCPConnectionManager_t,
                                      struct Stream_UserData> Test_I_Source_V4L_Net_TCPStream_t;
typedef Stream_Module_Net_IO_Stream_T<ACE_MT_SYNCH,
                                      Common_TimePolicy_t,
                                      stream_name_string_,
                                      enum Stream_ControlType,
                                      enum Stream_SessionMessageType,
                                      enum Stream_StateMachine_ControlState,
                                      struct Test_I_Source_V4L_StreamState,
                                      struct Test_I_Source_V4L_StreamConfiguration,
                                      struct Test_I_Source_Stream_StatisticData,
                                      Common_Timer_Manager_t,
                                      struct Test_I_AllocatorConfiguration,
                                      struct Stream_ModuleConfiguration,
                                      struct Test_I_Source_V4L_ModuleHandlerConfiguration,
                                      Test_I_Source_V4L_SessionData,
                                      Test_I_Source_V4L_SessionData_t,
                                      Test_I_ControlMessage_t,
                                      Test_I_Source_V4L_Stream_Message,
                                      Test_I_Source_V4L_SessionMessage,
                                      ACE_INET_Addr,
                                      Test_I_Source_V4L_UDPConnectionManager_t,
                                      struct Stream_UserData> Test_I_Source_V4L_Net_UDPStream_t;
#endif

//////////////////////////////////////////

#if defined (ACE_WIN32) || defined (ACE_WIN64)
struct Test_I_Source_DirectShow_ConnectionState
 : Test_I_ConnectionState
{
  Test_I_Source_DirectShow_ConnectionState ()
   : Test_I_ConnectionState ()
   , configuration (NULL)
   , userData (NULL)
  {}

  struct Test_I_Source_DirectShow_ConnectionConfiguration* configuration;

  struct Test_I_Source_DirectShow_UserData*                userData;
};
struct Test_I_Source_MediaFoundation_ConnectionState
 : Test_I_ConnectionState
{
  Test_I_Source_MediaFoundation_ConnectionState ()
   : Test_I_ConnectionState ()
   , configuration (NULL)
   , userData (NULL)
  {}

  struct Test_I_Source_MediaFoundation_ConnectionConfiguration* configuration;

  struct Test_I_Source_MediaFoundation_UserData*                userData;
};

struct Test_I_Target_DirectShow_ConnectionState
 : Test_I_ConnectionState
{
  Test_I_Target_DirectShow_ConnectionState ()
   : Test_I_ConnectionState ()
   , configuration (NULL)
   , userData (NULL)
  {}

  struct Test_I_Target_DirectShow_ConnectionConfiguration* configuration;

  struct Test_I_Target_UserData*                           userData;
};
struct Test_I_Target_MediaFoundation_ConnectionState
 : Test_I_ConnectionState
{
  Test_I_Target_MediaFoundation_ConnectionState ()
   : Test_I_ConnectionState ()
   , configuration (NULL)
   , userData (NULL)
  {}

  struct Test_I_Target_MediaFoundation_ConnectionConfiguration* configuration;

  struct Test_I_Target_UserData*                                userData;
};
#else
struct Test_I_Source_V4L_TCPConnectionState
 : Test_I_ConnectionState
{
  Test_I_Source_V4L_TCPConnectionState ()
   : Test_I_ConnectionState ()
   , configuration (NULL)
  {}

  Test_I_Source_V4L_TCPConnectionConfiguration_t* configuration;
};
struct Test_I_Source_V4L_UDPConnectionState
 : Test_I_ConnectionState
{
  Test_I_Source_V4L_UDPConnectionState ()
   : Test_I_ConnectionState ()
   , configuration (NULL)
  {}

  Test_I_Source_V4L_UDPConnectionConfiguration_t* configuration;
};

struct Test_I_Target_TCPConnectionState
 : Test_I_ConnectionState
{
  Test_I_Target_TCPConnectionState ()
   : Test_I_ConnectionState ()
   , configuration (NULL)
  {}

  Test_I_Target_TCPConnectionConfiguration_t* configuration;
};
struct Test_I_Target_UDPConnectionState
 : Test_I_ConnectionState
{
  Test_I_Target_UDPConnectionState ()
   : Test_I_ConnectionState ()
   , configuration (NULL)
  {}

  Test_I_Target_UDPConnectionConfiguration_t* configuration;
};
#endif

//////////////////////////////////////////

#if defined (ACE_WIN32) || defined (ACE_WIN64)
typedef Net_IConnection_T<ACE_INET_Addr,
                          Test_I_Source_DirectShow_ConnectionConfiguration_t,
                          struct Test_I_Source_DirectShow_ConnectionState,
                          struct Test_I_Source_Stream_StatisticData> Test_I_Source_DirectShow_IConnection_t;
typedef Net_IConnection_T<ACE_INET_Addr,
                          Test_I_Target_DirectShow_ConnectionConfiguration_t,
                          struct Test_I_Target_DirectShow_ConnectionState,
                          Test_I_Statistic_t> Test_I_Target_DirectShow_IConnection_t;
typedef Net_IConnection_T<ACE_INET_Addr,
                          Test_I_Source_MediaFoundation_ConnectionConfiguration_t,
                          struct Test_I_Source_MediaFoundation_ConnectionState,
                          struct Test_I_Source_Stream_StatisticData> Test_I_Source_MediaFoundation_IConnection_t;
typedef Net_IConnection_T<ACE_INET_Addr,
                          Test_I_Target_MediaFoundation_ConnectionConfiguration_t,
                          struct Test_I_Target_MediaFoundation_ConnectionState,
                          Test_I_Statistic_t> Test_I_Target_MediaFoundation_IConnection_t;
#else
typedef Net_IConnection_T<ACE_INET_Addr,
                          Test_I_Source_V4L_TCPConnectionConfiguration_t,
                          struct Test_I_Source_V4L_TCPConnectionState,
                          struct Test_I_Source_Stream_StatisticData> Test_I_Source_V4L_ITCPConnection_t;
typedef Net_IConnection_T<ACE_INET_Addr,
                          Test_I_Source_V4L_UDPConnectionConfiguration_t,
                          struct Test_I_Source_V4L_UDPConnectionState,
                          struct Test_I_Source_Stream_StatisticData> Test_I_Source_V4L_IUDPConnection_t;

typedef Net_IConnection_T<ACE_INET_Addr,
                          Test_I_Target_TCPConnectionConfiguration_t,
                          struct Test_I_Target_TCPConnectionState,
                          Test_I_Statistic_t> Test_I_Target_ITCPConnection_t;
typedef Net_IConnection_T<ACE_INET_Addr,
                          Test_I_Target_UDPConnectionConfiguration_t,
                          struct Test_I_Target_UDPConnectionState,
                          Test_I_Statistic_t> Test_I_Target_IUDPConnection_t;
#endif

//////////////////////////////////////////

// outbound
#if defined (ACE_WIN32) || defined (ACE_WIN64)
typedef Net_TCPSocketHandler_T<ACE_NULL_SYNCH,
                               ACE_SOCK_STREAM,
                               struct Test_I_Source_DirectShow_SocketHandlerConfiguration> Test_I_Source_DirectShow_TCPSocketHandler_t;
#if defined (SSL_SUPPORT)
typedef Net_TCPSocketHandler_T<ACE_NULL_SYNCH,
                               ACE_SSL_SOCK_Stream,
                               struct Test_I_Source_DirectShow_SocketHandlerConfiguration> Test_I_Source_DirectShow_SSLTCPSocketHandler_t;
#endif // SSL_SUPPORT
typedef Net_AsynchTCPSocketHandler_T<struct Test_I_Source_DirectShow_SocketHandlerConfiguration> Test_I_Source_DirectShow_AsynchTCPSocketHandler_t;
typedef Net_UDPSocketHandler_T<ACE_NULL_SYNCH,
                               Net_SOCK_Dgram,
                               struct Test_I_Source_DirectShow_SocketHandlerConfiguration> Test_I_Source_DirectShow_UDPSocketHandler_t;
typedef Net_AsynchUDPSocketHandler_T<Net_SOCK_Dgram,
                                     struct Test_I_Source_DirectShow_SocketHandlerConfiguration> Test_I_Source_DirectShow_AsynchUDPSocketHandler_t;

typedef Net_TCPSocketHandler_T<ACE_NULL_SYNCH,
                               ACE_SOCK_STREAM,
                               struct Test_I_Source_MediaFoundation_SocketHandlerConfiguration> Test_I_Source_MediaFoundation_TCPSocketHandler_t;
#if defined (SSL_SUPPORT)
typedef Net_TCPSocketHandler_T<ACE_NULL_SYNCH,
                               ACE_SSL_SOCK_Stream,
                               struct Test_I_Source_MediaFoundation_SocketHandlerConfiguration> Test_I_Source_MediaFoundation_SSLTCPSocketHandler_t;
#endif // SSL_SUPPORT
typedef Net_AsynchTCPSocketHandler_T<struct Test_I_Source_MediaFoundation_SocketHandlerConfiguration> Test_I_Source_MediaFoundation_AsynchTCPSocketHandler_t;
typedef Net_UDPSocketHandler_T<ACE_NULL_SYNCH,
                               Net_SOCK_Dgram,
                               struct Test_I_Source_MediaFoundation_SocketHandlerConfiguration> Test_I_Source_MediaFoundation_UDPSocketHandler_t;
typedef Net_AsynchUDPSocketHandler_T<Net_SOCK_Dgram,
                                     struct Test_I_Source_MediaFoundation_SocketHandlerConfiguration> Test_I_Source_MediaFoundation_AsynchUDPSocketHandler_t;
#else
#endif

#if defined (ACE_WIN32) || defined (ACE_WIN64)
typedef Net_TCPConnectionBase_T<ACE_NULL_SYNCH,
                                Test_I_Source_DirectShow_TCPSocketHandler_t,
                                Test_I_Source_DirectShow_ConnectionConfiguration_t,
                                struct Test_I_Source_DirectShow_ConnectionState,
                                struct Test_I_Source_Stream_StatisticData,
                                struct Test_I_Source_DirectShow_SocketHandlerConfiguration,
                                struct Test_I_Source_DirectShow_ListenerConfiguration,
                                Test_I_Source_DirectShow_NetStream_t,
                                Common_Timer_Manager_t,
                                struct Test_I_Source_DirectShow_UserData> Test_I_Source_DirectShow_TCPConnection_t;
#if defined (SSL_SUPPORT)
typedef Net_TCPConnectionBase_T<ACE_NULL_SYNCH,
                                Test_I_Source_DirectShow_SSLTCPSocketHandler_t,
                                Test_I_Source_DirectShow_ConnectionConfiguration_t,
                                struct Test_I_Source_DirectShow_ConnectionState,
                                struct Test_I_Source_Stream_StatisticData,
                                struct Test_I_Source_DirectShow_SocketHandlerConfiguration,
                                struct Test_I_Source_DirectShow_ListenerConfiguration,
                                Test_I_Source_DirectShow_NetStream_t,
                                Common_Timer_Manager_t,
                                struct Test_I_Source_DirectShow_UserData> Test_I_Source_DirectShow_SSLTCPConnection_t;
#endif // SSL_SUPPORT
typedef Net_AsynchTCPConnectionBase_T<Test_I_Source_DirectShow_AsynchTCPSocketHandler_t,
                                      Test_I_Source_DirectShow_ConnectionConfiguration_t,
                                      struct Test_I_Source_DirectShow_ConnectionState,
                                      struct Test_I_Source_Stream_StatisticData,
                                      struct Test_I_Source_DirectShow_SocketHandlerConfiguration,
                                      struct Test_I_Source_DirectShow_ListenerConfiguration,
                                      Test_I_Source_DirectShow_NetStream_t,
                                      Common_Timer_Manager_t,
                                      struct Test_I_Source_DirectShow_UserData> Test_I_Source_DirectShow_AsynchTCPConnection_t;

typedef Net_UDPConnectionBase_T<ACE_NULL_SYNCH,
                                Test_I_Source_DirectShow_UDPSocketHandler_t,
                                Test_I_Source_DirectShow_ConnectionConfiguration_t,
                                struct Test_I_Source_DirectShow_ConnectionState,
                                struct Test_I_Source_Stream_StatisticData,
                                struct Test_I_Source_DirectShow_SocketHandlerConfiguration,
                                Test_I_Source_DirectShow_NetStream_t,
                                Common_Timer_Manager_t,
                                struct Test_I_Source_DirectShow_UserData> Test_I_Source_DirectShow_UDPConnection_t;
typedef Net_AsynchUDPConnectionBase_T<Test_I_Source_DirectShow_AsynchUDPSocketHandler_t,
                                      Test_I_Source_DirectShow_ConnectionConfiguration_t,
                                      struct Test_I_Source_DirectShow_ConnectionState,
                                      struct Test_I_Source_Stream_StatisticData,
                                      struct Test_I_Source_DirectShow_SocketHandlerConfiguration,
                                      Test_I_Source_DirectShow_NetStream_t,
                                      Common_Timer_Manager_t,
                                      struct Test_I_Source_DirectShow_UserData> Test_I_Source_DirectShow_AsynchUDPConnection_t;

typedef Net_TCPConnectionBase_T<ACE_NULL_SYNCH,
                                Test_I_Source_MediaFoundation_TCPSocketHandler_t,
                                Test_I_Source_MediaFoundation_ConnectionConfiguration_t,
                                struct Test_I_Source_MediaFoundation_ConnectionState,
                                struct Test_I_Source_Stream_StatisticData,
                                struct Test_I_Source_MediaFoundation_SocketHandlerConfiguration,
                                struct Test_I_Source_MediaFoundation_ListenerConfiguration,
                                Test_I_Source_MediaFoundation_NetStream_t,
                                Common_Timer_Manager_t,
                                struct Test_I_Source_MediaFoundation_UserData> Test_I_Source_MediaFoundation_TCPConnection_t;
#if defined (SSL_SUPPORT)
typedef Net_TCPConnectionBase_T<ACE_NULL_SYNCH,
                                Test_I_Source_MediaFoundation_SSLTCPSocketHandler_t,
                                Test_I_Source_MediaFoundation_ConnectionConfiguration_t,
                                struct Test_I_Source_MediaFoundation_ConnectionState,
                                struct Test_I_Source_Stream_StatisticData,
                                struct Test_I_Source_MediaFoundation_SocketHandlerConfiguration,
                                struct Test_I_Source_MediaFoundation_ListenerConfiguration,
                                Test_I_Source_MediaFoundation_NetStream_t,
                                Common_Timer_Manager_t,
                                struct Test_I_Source_MediaFoundation_UserData> Test_I_Source_MediaFoundation_SSLTCPConnection_t;
#endif // SSL_SUPPORT
typedef Net_AsynchTCPConnectionBase_T<Test_I_Source_MediaFoundation_AsynchTCPSocketHandler_t,
                                      Test_I_Source_MediaFoundation_ConnectionConfiguration_t,
                                      struct Test_I_Source_MediaFoundation_ConnectionState,
                                      struct Test_I_Source_Stream_StatisticData,
                                      struct Test_I_Source_MediaFoundation_SocketHandlerConfiguration,
                                      struct Test_I_Source_MediaFoundation_ListenerConfiguration,
                                      Test_I_Source_MediaFoundation_NetStream_t,
                                      Common_Timer_Manager_t,
                                      struct Test_I_Source_MediaFoundation_UserData> Test_I_Source_MediaFoundation_AsynchTCPConnection_t;

typedef Net_UDPConnectionBase_T<ACE_NULL_SYNCH,
                                Test_I_Source_MediaFoundation_UDPSocketHandler_t,
                                Test_I_Source_MediaFoundation_ConnectionConfiguration_t,
                                struct Test_I_Source_MediaFoundation_ConnectionState,
                                struct Test_I_Source_Stream_StatisticData,
                                struct Test_I_Source_MediaFoundation_SocketHandlerConfiguration,
                                Test_I_Source_MediaFoundation_NetStream_t,
                                Common_Timer_Manager_t,
                                struct Test_I_Source_MediaFoundation_UserData> Test_I_Source_MediaFoundation_UDPConnection_t;
typedef Net_AsynchUDPConnectionBase_T<Test_I_Source_MediaFoundation_AsynchUDPSocketHandler_t,
                                      Test_I_Source_MediaFoundation_ConnectionConfiguration_t,
                                      struct Test_I_Source_MediaFoundation_ConnectionState,
                                      struct Test_I_Source_Stream_StatisticData,
                                      struct Test_I_Source_MediaFoundation_SocketHandlerConfiguration,
                                      Test_I_Source_MediaFoundation_NetStream_t,
                                      Common_Timer_Manager_t,
                                      struct Test_I_Source_MediaFoundation_UserData> Test_I_Source_MediaFoundation_AsynchUDPConnection_t;
#else
typedef Net_TCPConnectionBase_T<ACE_NULL_SYNCH,
                                Test_I_Source_V4L_TCPConnectionConfiguration_t,
                                struct Test_I_Source_V4L_TCPConnectionState,
                                struct Test_I_Source_Stream_StatisticData,
                                Test_I_Source_V4L_Net_TCPStream_t,
                                Common_Timer_Manager_t,
                                struct Net_UserData> Test_I_Source_V4L_TCPConnection_t;
#if defined (SSL_SUPPORT)
typedef Net_SSLConnectionBase_T<ACE_NULL_SYNCH,
                                Test_I_Source_V4L_ConnectionConfiguration_t,
                                struct Test_I_Source_V4L_ConnectionState,
                                struct Test_I_Source_Stream_StatisticData,
                                Test_I_Source_V4L_NetStream_t,
                                Common_Timer_Manager_t,
                                struct Net_UserData> Test_I_Source_V4L_SSLTCPConnection_t;
#endif // SSL_SUPPORT
typedef Net_AsynchTCPConnectionBase_T<Test_I_Source_V4L_TCPConnectionConfiguration_t,
                                      struct Test_I_Source_V4L_TCPConnectionState,
                                      struct Test_I_Source_Stream_StatisticData,
                                      Test_I_Source_V4L_Net_TCPStream_t,
                                      Common_Timer_Manager_t,
                                      struct Net_UserData> Test_I_Source_V4L_AsynchTCPConnection_t;

typedef Net_UDPConnectionBase_T<ACE_NULL_SYNCH,
                                Net_UDPSocketHandler_t,
                                Test_I_Source_V4L_UDPConnectionConfiguration_t,
                                struct Test_I_Source_V4L_UDPConnectionState,
                                struct Test_I_Source_Stream_StatisticData,
                                Test_I_Source_V4L_Net_UDPStream_t,
                                Common_Timer_Manager_t,
                                struct Net_UserData> Test_I_Source_V4L_UDPConnection_t;
typedef Net_AsynchUDPConnectionBase_T<Net_AsynchUDPSocketHandler_t,
                                      Test_I_Source_V4L_UDPConnectionConfiguration_t,
                                      struct Test_I_Source_V4L_UDPConnectionState,
                                      struct Test_I_Source_Stream_StatisticData,
                                      Test_I_Source_V4L_Net_UDPStream_t,
                                      Common_Timer_Manager_t,
                                      struct Net_UserData> Test_I_Source_V4L_AsynchUDPConnection_t;
#endif

//////////////////////////////////////////

#if defined (ACE_WIN32) || defined (ACE_WIN64)
typedef Net_IConnector_T<ACE_INET_Addr,
                         Test_I_Source_DirectShow_ConnectionConfiguration_t> Test_I_Source_DirectShow_IInetConnector_t;
typedef Net_IConnector_T<ACE_INET_Addr,
                         Test_I_Source_MediaFoundation_ConnectionConfiguration_t> Test_I_Source_MediaFoundation_IInetConnector_t;
#else
typedef Net_IConnector_T<ACE_INET_Addr,
                         Test_I_Source_V4L_TCPConnectionConfiguration_t> Test_I_Source_V4L_ITCPConnector_t;
typedef Net_IConnector_T<ACE_INET_Addr,
                         Test_I_Source_V4L_UDPConnectionConfiguration_t> Test_I_Source_V4L_IUDPConnector_t;
#endif

//////////////////////////////////////////

// outbound
#if defined (ACE_WIN32) || defined (ACE_WIN64)
typedef Net_Client_Connector_T<ACE_NULL_SYNCH,
                               Test_I_Source_DirectShow_TCPConnection_t,
                               Net_SOCK_Connector,
                               ACE_INET_Addr,
                               Test_I_Source_DirectShow_ConnectionConfiguration_t,
                               struct Test_I_Source_DirectShow_ConnectionState,
                               struct Test_I_Source_Stream_StatisticData,
                               struct Net_TCPSocketConfiguration,
                               struct Test_I_Source_DirectShow_SocketHandlerConfiguration,
                               Test_I_Source_DirectShow_NetStream_t,
                               struct Test_I_Source_DirectShow_UserData> Test_I_Source_DirectShow_TCPConnector_t;
#if defined (SSL_SUPPORT)
typedef Net_Client_SSL_Connector_T<Test_I_Source_DirectShow_SSLTCPConnection_t,
                                   ACE_SSL_SOCK_Connector,
                                   ACE_INET_Addr,
                                   Test_I_Source_DirectShow_ConnectionConfiguration_t,
                                   struct Test_I_Source_DirectShow_ConnectionState,
                                   struct Test_I_Source_Stream_StatisticData,
                                   struct Test_I_Source_DirectShow_SocketHandlerConfiguration,
                                   Test_I_Source_DirectShow_NetStream_t,
                                   struct Test_I_Source_DirectShow_UserData> Test_I_Source_DirectShow_SSLTCPConnector_t;
#endif // SSL_SUPPORT
typedef Net_Client_AsynchConnector_T<Test_I_Source_DirectShow_AsynchTCPConnection_t,
                                     ACE_INET_Addr,
                                     Test_I_Source_DirectShow_ConnectionConfiguration_t,
                                     struct Test_I_Source_DirectShow_ConnectionState,
                                     struct Test_I_Source_Stream_StatisticData,
                                     struct Net_TCPSocketConfiguration,
                                     struct Test_I_Source_DirectShow_SocketHandlerConfiguration,
                                     Test_I_Source_DirectShow_NetStream_t,
                                     struct Test_I_Source_DirectShow_UserData> Test_I_Source_DirectShow_TCPAsynchConnector_t;

typedef Net_Client_Connector_T<ACE_NULL_SYNCH,
                               Test_I_Source_DirectShow_UDPConnection_t,
                               Net_SOCK_Dgram,
                               ACE_INET_Addr,
                               Test_I_Source_DirectShow_ConnectionConfiguration_t,
                               struct Test_I_Source_DirectShow_ConnectionState,
                               struct Test_I_Source_Stream_StatisticData,
                               struct Net_UDPSocketConfiguration,
                               struct Test_I_Source_DirectShow_SocketHandlerConfiguration,
                               Test_I_Source_DirectShow_NetStream_t,
                               struct Test_I_Source_DirectShow_UserData> Test_I_Source_DirectShow_UDPConnector_t;
typedef Net_Client_AsynchConnector_T<Test_I_Source_DirectShow_AsynchUDPConnection_t,
                                     ACE_INET_Addr,
                                     Test_I_Source_DirectShow_ConnectionConfiguration_t,
                                     struct Test_I_Source_DirectShow_ConnectionState,
                                     struct Test_I_Source_Stream_StatisticData,
                                     struct Net_UDPSocketConfiguration,
                                     struct Test_I_Source_DirectShow_SocketHandlerConfiguration,
                                     Test_I_Source_DirectShow_NetStream_t,
                                     struct Test_I_Source_DirectShow_UserData> Test_I_Source_DirectShow_UDPAsynchConnector_t;

typedef Net_Client_Connector_T<ACE_NULL_SYNCH,
                               Test_I_Source_MediaFoundation_TCPConnection_t,
                               Net_SOCK_Connector,
                               ACE_INET_Addr,
                               Test_I_Source_MediaFoundation_ConnectionConfiguration_t,
                               struct Test_I_Source_MediaFoundation_ConnectionState,
                               struct Test_I_Source_Stream_StatisticData,
                               struct Net_TCPSocketConfiguration,
                               struct Test_I_Source_MediaFoundation_SocketHandlerConfiguration,
                               Test_I_Source_MediaFoundation_NetStream_t,
                               struct Test_I_Source_MediaFoundation_UserData> Test_I_Source_MediaFoundation_TCPConnector_t;
#if defined (SSL_SUPPORT)
typedef Net_Client_SSL_Connector_T<Test_I_Source_MediaFoundation_SSLTCPConnection_t,
                                   ACE_SSL_SOCK_Connector,
                                   ACE_INET_Addr,
                                   Test_I_Source_MediaFoundation_ConnectionConfiguration_t,
                                   struct Test_I_Source_MediaFoundation_ConnectionState,
                                   struct Test_I_Source_Stream_StatisticData,
                                   struct Test_I_Source_MediaFoundation_SocketHandlerConfiguration,
                                   Test_I_Source_MediaFoundation_NetStream_t,
                                   struct Test_I_Source_MediaFoundation_UserData> Test_I_Source_MediaFoundation_SSLTCPConnector_t;
#endif // SSL_SUPPORT
typedef Net_Client_AsynchConnector_T<Test_I_Source_MediaFoundation_AsynchTCPConnection_t,
                                     ACE_INET_Addr,
                                     Test_I_Source_MediaFoundation_ConnectionConfiguration_t,
                                     struct Test_I_Source_MediaFoundation_ConnectionState,
                                     struct Test_I_Source_Stream_StatisticData,
                                     struct Net_TCPSocketConfiguration,
                                     struct Test_I_Source_MediaFoundation_SocketHandlerConfiguration,
                                     Test_I_Source_MediaFoundation_NetStream_t,
                                     struct Test_I_Source_MediaFoundation_UserData> Test_I_Source_MediaFoundation_TCPAsynchConnector_t;

typedef Net_Client_Connector_T<ACE_NULL_SYNCH,
                               Test_I_Source_MediaFoundation_UDPConnection_t,
                               Net_SOCK_Dgram,
                               ACE_INET_Addr,
                               Test_I_Source_MediaFoundation_ConnectionConfiguration_t,
                               struct Test_I_Source_MediaFoundation_ConnectionState,
                               struct Test_I_Source_Stream_StatisticData,
                               struct Net_UDPSocketConfiguration,
                               struct Test_I_Source_MediaFoundation_SocketHandlerConfiguration,
                               Test_I_Source_MediaFoundation_NetStream_t,
                               struct Test_I_Source_MediaFoundation_UserData> Test_I_Source_MediaFoundation_UDPConnector_t;
typedef Net_Client_AsynchConnector_T<Test_I_Source_MediaFoundation_AsynchUDPConnection_t,
                                     ACE_INET_Addr,
                                     Test_I_Source_MediaFoundation_ConnectionConfiguration_t,
                                     struct Test_I_Source_MediaFoundation_ConnectionState,
                                     struct Test_I_Source_Stream_StatisticData,
                                     struct Net_UDPSocketConfiguration,
                                     struct Test_I_Source_MediaFoundation_SocketHandlerConfiguration,
                                     Test_I_Source_MediaFoundation_NetStream_t,
                                     struct Test_I_Source_MediaFoundation_UserData> Test_I_Source_MediaFoundation_UDPAsynchConnector_t;
#else
typedef Net_Client_Connector_T<ACE_NULL_SYNCH,
                               Test_I_Source_V4L_TCPConnection_t,
                               Net_SOCK_Connector,
                               ACE_INET_Addr,
                               Test_I_Source_V4L_TCPConnectionConfiguration_t,
                               struct Test_I_Source_V4L_TCPConnectionState,
                               struct Test_I_Source_Stream_StatisticData,
                               Net_TCPSocketConfiguration_t,
                               Net_TCPSocketConfiguration_t,
                               Test_I_Source_V4L_Net_TCPStream_t,
                               struct Net_UserData> Test_I_Source_V4L_TCPConnector_t;
#if defined (SSL_SUPPORT)
typedef Net_Client_SSL_Connector_T<Test_I_Source_V4L_SSLTCPConnection_t,
                                   ACE_SSL_SOCK_Connector,
                                   ACE_INET_Addr,
                                   Test_I_Source_V4L_ConnectionConfiguration_t,
                                   struct Test_I_Source_V4L_ConnectionState,
                                   struct Test_I_Source_Stream_StatisticData,
                                   struct Test_I_Source_V4L_SocketHandlerConfiguration,
                                   Test_I_Source_V4L_NetStream_t,
                                   struct Net_UserData> Test_I_Source_V4L_SSLTCPConnector_t;
#endif // SSL_SUPPORT
typedef Net_Client_AsynchConnector_T<Test_I_Source_V4L_AsynchTCPConnection_t,
                                     ACE_INET_Addr,
                                     Test_I_Source_V4L_TCPConnectionConfiguration_t,
                                     struct Test_I_Source_V4L_TCPConnectionState,
                                     struct Test_I_Source_Stream_StatisticData,
                                     Net_TCPSocketConfiguration_t,
                                     Net_TCPSocketConfiguration_t,
                                     Test_I_Source_V4L_Net_TCPStream_t,
                                     struct Net_UserData> Test_I_Source_V4L_TCPAsynchConnector_t;

typedef Net_Client_Connector_T<ACE_NULL_SYNCH,
                               Test_I_Source_V4L_UDPConnection_t,
                               Net_SOCK_Dgram,
                               ACE_INET_Addr,
                               Test_I_Source_V4L_UDPConnectionConfiguration_t,
                               struct Test_I_Source_V4L_UDPConnectionState,
                               struct Test_I_Source_Stream_StatisticData,
                               Net_UDPSocketConfiguration_t,
                               Net_UDPSocketConfiguration_t,
                               Test_I_Source_V4L_Net_UDPStream_t,
                               struct Net_UserData> Test_I_Source_V4L_UDPConnector_t;
typedef Net_Client_AsynchConnector_T<Test_I_Source_V4L_AsynchUDPConnection_t,
                                     ACE_INET_Addr,
                                     Test_I_Source_V4L_UDPConnectionConfiguration_t,
                                     struct Test_I_Source_V4L_UDPConnectionState,
                                     struct Test_I_Source_Stream_StatisticData,
                                     Net_UDPSocketConfiguration_t,
                                     Net_UDPSocketConfiguration_t,
                                     Test_I_Source_V4L_Net_UDPStream_t,
                                     struct Net_UserData> Test_I_Source_V4L_UDPAsynchConnector_t;
#endif

//////////////////////////////////////////

// inbound
#if defined (ACE_WIN32) || defined (ACE_WIN64)
typedef Net_TCPSocketHandler_T<ACE_NULL_SYNCH,
                               ACE_SOCK_STREAM,
                               struct Test_I_Target_DirectShow_SocketHandlerConfiguration> Test_I_Target_DirectShow_TCPSocketHandler_t;
#if defined (SSL_SUPPORT)
typedef Net_TCPSocketHandler_T<ACE_NULL_SYNCH,
                               ACE_SSL_SOCK_Stream,
                               struct Test_I_Target_DirectShow_SocketHandlerConfiguration> Test_I_Target_DirectShow_SSLTCPSocketHandler_t;
#endif // SSL_SUPPORT
typedef Net_AsynchTCPSocketHandler_T<struct Test_I_Target_DirectShow_SocketHandlerConfiguration> Test_I_Target_DirectShow_AsynchTCPSocketHandler_t;
typedef Net_UDPSocketHandler_T<ACE_NULL_SYNCH,
                               Net_SOCK_Dgram,
                               struct Test_I_Target_DirectShow_SocketHandlerConfiguration> Test_I_Target_DirectShow_UDPSocketHandler_t;
typedef Net_AsynchUDPSocketHandler_T<Net_SOCK_Dgram,
                                     struct Test_I_Target_DirectShow_SocketHandlerConfiguration> Test_I_Target_DirectShow_AsynchUDPSocketHandler_t;

typedef Net_TCPSocketHandler_T<ACE_NULL_SYNCH,
                               ACE_SOCK_STREAM,
                               struct Test_I_Target_MediaFoundation_SocketHandlerConfiguration> Test_I_Target_MediaFoundation_TCPSocketHandler_t;
#if defined (SSL_SUPPORT)
typedef Net_TCPSocketHandler_T<ACE_NULL_SYNCH,
                               ACE_SSL_SOCK_Stream,
                               struct Test_I_Target_MediaFoundation_SocketHandlerConfiguration> Test_I_Target_MediaFoundation_SSLTCPSocketHandler_t;
#endif // SSL_SUPPORT
typedef Net_AsynchTCPSocketHandler_T<struct Test_I_Target_MediaFoundation_SocketHandlerConfiguration> Test_I_Target_MediaFoundation_AsynchTCPSocketHandler_t;
typedef Net_UDPSocketHandler_T<ACE_NULL_SYNCH,
                               Net_SOCK_Dgram,
                               struct Test_I_Target_MediaFoundation_SocketHandlerConfiguration> Test_I_Target_MediaFoundation_UDPSocketHandler_t;
typedef Net_AsynchUDPSocketHandler_T<Net_SOCK_Dgram,
                                     struct Test_I_Target_MediaFoundation_SocketHandlerConfiguration> Test_I_Target_MediaFoundation_AsynchUDPSocketHandler_t;
#else
#endif

#if defined (ACE_WIN32) || defined (ACE_WIN64)
typedef Net_TCPConnectionBase_T<ACE_NULL_SYNCH,
                                Test_I_Target_DirectShow_TCPSocketHandler_t,
                                Test_I_Target_DirectShow_ConnectionConfiguration_t,
                                struct Test_I_Target_DirectShow_ConnectionState,
                                Test_I_Statistic_t,
                                struct Test_I_Target_DirectShow_SocketHandlerConfiguration,
                                struct Test_I_Target_DirectShow_ListenerConfiguration,
                                Test_I_Target_DirectShow_Stream,
                                Common_Timer_Manager_t,
                                struct Net_UserData> Test_I_Target_DirectShow_TCPConnection_t;
#if defined (SSL_SUPPORT)
typedef Net_TCPConnectionBase_T<ACE_NULL_SYNCH,
                                Test_I_Target_DirectShow_SSLTCPSocketHandler_t,
                                Test_I_Target_DirectShow_ConnectionConfiguration_t,
                                struct Test_I_Target_DirectShow_ConnectionState,
                                Test_I_Statistic_t,
                                struct Test_I_Target_DirectShow_SocketHandlerConfiguration,
                                struct Test_I_Target_DirectShow_ListenerConfiguration,
                                Test_I_Target_DirectShow_Stream,
                                Common_Timer_Manager_t,
                                struct Net_UserData> Test_I_Target_DirectShow_SSLTCPConnection_t;
#endif // SSL_SUPPORT
typedef Net_AsynchTCPConnectionBase_T<Test_I_Target_DirectShow_AsynchTCPSocketHandler_t,
                                      Test_I_Target_DirectShow_ConnectionConfiguration_t,
                                      struct Test_I_Target_DirectShow_ConnectionState,
                                      Test_I_Statistic_t,
                                      struct Test_I_Target_DirectShow_SocketHandlerConfiguration,
                                      struct Test_I_Target_DirectShow_ListenerConfiguration,
                                      Test_I_Target_DirectShow_Stream,
                                      Common_Timer_Manager_t,
                                      struct Net_UserData> Test_I_Target_DirectShow_AsynchTCPConnection_t;

typedef Net_UDPConnectionBase_T<ACE_NULL_SYNCH,
                                Test_I_Target_DirectShow_UDPSocketHandler_t,
                                Test_I_Target_DirectShow_ConnectionConfiguration_t,
                                struct Test_I_Target_DirectShow_ConnectionState,
                                Test_I_Statistic_t,
                                struct Test_I_Target_DirectShow_SocketHandlerConfiguration,
                                Test_I_Target_DirectShow_Stream,
                                Common_Timer_Manager_t,
                                struct Net_UserData> Test_I_Target_DirectShow_UDPConnection_t;
typedef Net_AsynchUDPConnectionBase_T<Test_I_Target_DirectShow_AsynchUDPSocketHandler_t,
                                      Test_I_Target_DirectShow_ConnectionConfiguration_t,
                                      struct Test_I_Target_DirectShow_ConnectionState,
                                      Test_I_Statistic_t,
                                      struct Test_I_Target_DirectShow_SocketHandlerConfiguration,
                                      Test_I_Target_DirectShow_Stream,
                                      Common_Timer_Manager_t,
                                      struct Net_UserData> Test_I_Target_DirectShow_AsynchUDPConnection_t;

typedef Net_TCPConnectionBase_T<ACE_NULL_SYNCH,
                                Test_I_Target_MediaFoundation_TCPSocketHandler_t,
                                Test_I_Target_MediaFoundation_ConnectionConfiguration_t,
                                struct Test_I_Target_MediaFoundation_ConnectionState,
                                Test_I_Statistic_t,
                                struct Test_I_Target_MediaFoundation_SocketHandlerConfiguration,
                                struct Test_I_Target_MediaFoundation_ListenerConfiguration,
                                Test_I_Target_MediaFoundation_Stream,
                                Common_Timer_Manager_t,
                                struct Net_UserData> Test_I_Target_MediaFoundation_TCPConnection_t;
#if defined (SSL_SUPPORT)
typedef Net_TCPConnectionBase_T<ACE_NULL_SYNCH,
                                Test_I_Target_MediaFoundation_SSLTCPSocketHandler_t,
                                Test_I_Target_MediaFoundation_ConnectionConfiguration_t,
                                struct Test_I_Target_MediaFoundation_ConnectionState,
                                Test_I_Statistic_t,
                                struct Test_I_Target_MediaFoundation_SocketHandlerConfiguration,
                                struct Test_I_Target_MediaFoundation_ListenerConfiguration,
                                Test_I_Target_MediaFoundation_Stream,
                                Common_Timer_Manager_t,
                                struct Net_UserData> Test_I_Target_MediaFoundation_SSLTCPConnection_t;
#endif // SSL_SUPPORT
typedef Net_AsynchTCPConnectionBase_T<Test_I_Target_MediaFoundation_AsynchTCPSocketHandler_t,
                                      Test_I_Target_MediaFoundation_ConnectionConfiguration_t,
                                      struct Test_I_Target_MediaFoundation_ConnectionState,
                                      Test_I_Statistic_t,
                                      struct Test_I_Target_MediaFoundation_SocketHandlerConfiguration,
                                      struct Test_I_Target_MediaFoundation_ListenerConfiguration,
                                      Test_I_Target_MediaFoundation_Stream,
                                      Common_Timer_Manager_t,
                                      struct Net_UserData> Test_I_Target_MediaFoundation_AsynchTCPConnection_t;

typedef Net_UDPConnectionBase_T<ACE_NULL_SYNCH,
                                Test_I_Target_MediaFoundation_UDPSocketHandler_t,
                                Test_I_Target_MediaFoundation_ConnectionConfiguration_t,
                                struct Test_I_Target_MediaFoundation_ConnectionState,
                                Test_I_Statistic_t,
                                struct Test_I_Target_MediaFoundation_SocketHandlerConfiguration,
                                Test_I_Target_MediaFoundation_Stream,
                                Common_Timer_Manager_t,
                                struct Net_UserData> Test_I_Target_MediaFoundation_UDPConnection_t;
typedef Net_AsynchUDPConnectionBase_T<Test_I_Target_MediaFoundation_AsynchUDPSocketHandler_t,
                                      Test_I_Target_MediaFoundation_ConnectionConfiguration_t,
                                      struct Test_I_Target_MediaFoundation_ConnectionState,
                                      Test_I_Statistic_t,
                                      struct Test_I_Target_MediaFoundation_SocketHandlerConfiguration,
                                      Test_I_Target_MediaFoundation_Stream,
                                      Common_Timer_Manager_t,
                                      struct Net_UserData> Test_I_Target_MediaFoundation_AsynchUDPConnection_t;
#else
typedef Net_TCPConnectionBase_T<ACE_NULL_SYNCH,
                                Test_I_Target_TCPConnectionConfiguration_t,
                                struct Test_I_Target_TCPConnectionState,
                                Test_I_Statistic_t,
                                Test_I_Target_Stream,
                                Common_Timer_Manager_t,
                                struct Net_UserData> Test_I_Target_TCPConnection_t;
#if defined (SSL_SUPPORT)
typedef Net_SSLConnectionBase_T<ACE_NULL_SYNCH,
                                Test_I_Target_ConnectionConfiguration_t,
                                struct Test_I_Target_ConnectionState,
                                Test_I_Statistic_t,
                                Test_I_Target_Stream,
                                Common_Timer_Manager_t,
                                struct Net_UserData> Test_I_Target_SSLTCPConnection_t;
#endif // SSL_SUPPORT
typedef Net_AsynchTCPConnectionBase_T<Test_I_Target_TCPConnectionConfiguration_t,
                                      struct Test_I_Target_TCPConnectionState,
                                      Test_I_Statistic_t,
                                      Test_I_Target_Stream,
                                      Common_Timer_Manager_t,
                                      struct Net_UserData> Test_I_Target_AsynchTCPConnection_t;

typedef Net_UDPConnectionBase_T<ACE_NULL_SYNCH,
                                Net_UDPSocketHandler_t,
                                Test_I_Target_UDPConnectionConfiguration_t,
                                struct Test_I_Target_UDPConnectionState,
                                Test_I_Statistic_t,
                                Test_I_Target_Stream,
                                Common_Timer_Manager_t,
                                struct Net_UserData> Test_I_Target_UDPConnection_t;
typedef Net_AsynchUDPConnectionBase_T<Net_AsynchUDPSocketHandler_t,
                                      Test_I_Target_UDPConnectionConfiguration_t,
                                      struct Test_I_Target_UDPConnectionState,
                                      Test_I_Statistic_t,
                                      Test_I_Target_Stream,
                                      Common_Timer_Manager_t,
                                      struct Net_UserData> Test_I_Target_AsynchUDPConnection_t;
#endif

//////////////////////////////////////////

#if defined (ACE_WIN32) || defined (ACE_WIN64)
typedef Net_IConnector_T<ACE_INET_Addr,
                         Test_I_Target_DirectShow_ConnectionConfiguration_t> Test_I_Target_DirectShow_IInetConnector_t;
typedef Net_IConnector_T<ACE_INET_Addr,
                         Test_I_Target_MediaFoundation_ConnectionConfiguration_t> Test_I_Target_MediaFoundation_IInetConnector_t;
#else
typedef Net_IConnector_T<ACE_INET_Addr,
                         Test_I_Target_TCPConnectionConfiguration_t> Test_I_Target_ITCPConnector_t;
typedef Net_IConnector_T<ACE_INET_Addr,
                         Test_I_Target_UDPConnectionConfiguration_t> Test_I_Target_IUDPConnector_t;
#endif // ACE_WIN32 || ACE_WIN64

#endif
