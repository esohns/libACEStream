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

#include <string>

#include <ace/INET_Addr.h>
#include <ace/os_include/sys/os_socket.h>
#include <ace/Time_Value.h>

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include <d3d9.h>
#include <evr.h>
#include <mfapi.h>
//#include <mfidl.h>
//#include <mmeapi.h>
//#include <mtype.h>
#include <strmif.h>
#else
//#include <linux/videodev2.h>
#endif

#include <gtk/gtk.h>

#include "stream_control_message.h"

#include "stream_dec_defines.h"

#include "stream_dev_defines.h"
#include "stream_dev_tools.h"

#include "stream_misc_defines.h"

#include "net_defines.h"
#include "net_ilistener.h"

#include "test_i_defines.h"

#include "test_i_camstream_common.h"
#include "test_i_camstream_network.h"
#include "test_i_connection_manager_common.h"

// forward declarations
#if defined (ACE_WIN32) || defined (ACE_WIN64)
class Test_I_Target_DirectShow_Stream_Message;
class Test_I_Target_DirectShow_Stream_SessionMessage;
class Test_I_Target_MediaFoundation_Stream_Message;
class Test_I_Target_MediaFoundation_Stream_SessionMessage;
#else
struct v4l2_window;
#endif
class Test_I_Target_Stream_Message;
class Test_I_Target_Stream_SessionMessage;
template <typename ConfigurationType,
          typename ConnectionManagerType>
class Test_I_Target_SignalHandler_T;

#if defined (ACE_WIN32) || defined (ACE_WIN64)
struct Test_I_Target_DirectShow_MessageData
{
  inline Test_I_Target_DirectShow_MessageData ()
   : sample (NULL)
   , sampleTime (0.0)
  {};

  IMediaSample* sample;
  double        sampleTime;
};
struct Test_I_Target_MediaFoundation_MessageData
{
  inline Test_I_Target_MediaFoundation_MessageData ()
   : sample (NULL)
   , sampleTime (0)
  {};

  IMFMediaBuffer* sample;
  LONGLONG        sampleTime;
};
#endif

#if defined (ACE_WIN32) || defined (ACE_WIN64)
struct Test_I_Target_ConnectionConfiguration;
struct Test_I_Target_DirectShow_StreamConfiguration;

struct Test_I_Target_ConnectionConfiguration;
struct Test_I_Target_MediaFoundation_StreamConfiguration;
#endif
struct Test_I_Target_ConnectionConfiguration;
struct Test_I_Target_StreamConfiguration;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
struct Test_I_Target_DirectShow_UserData
 : Stream_UserData
{
  inline Test_I_Target_DirectShow_UserData ()
   : Stream_UserData ()
   , configuration (NULL)
   , streamConfiguration (NULL)
  {};

  struct Test_I_Target_ConnectionConfiguration*        configuration;
  struct Test_I_Target_DirectShow_StreamConfiguration* streamConfiguration;
};
struct Test_I_Target_MediaFoundation_UserData
 : Stream_UserData
{
  inline Test_I_Target_MediaFoundation_UserData ()
   : Stream_UserData ()
   , configuration (NULL)
   , streamConfiguration (NULL)
  {};

  struct Test_I_Target_ConnectionConfiguration*             configuration;
  struct Test_I_Target_MediaFoundation_StreamConfiguration* streamConfiguration;
};
#endif
struct Test_I_Target_ConnectionConfiguration;
struct Test_I_Target_UserData
 : Stream_UserData
{
  inline Test_I_Target_UserData ()
   : Stream_UserData ()
   , configuration (NULL)
   , streamConfiguration (NULL)
  {};

  struct Test_I_Target_ConnectionConfiguration* configuration;
  struct Test_I_Target_StreamConfiguration*     streamConfiguration;
};

struct Test_I_Target_ConnectionState;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
struct Test_I_Target_SessionData
 : Test_I_CamStream_DirectShow_SessionData
{
  inline Test_I_Target_SessionData ()
   : Test_I_CamStream_DirectShow_SessionData ()
   , connectionState (NULL)
   , targetFileName ()
   , userData (NULL)
  {};

  inline struct Test_I_Target_SessionData& operator+= (const struct Test_I_Target_SessionData& rhs_in)
  {
    // *NOTE*: the idea is to 'merge' the data
    Test_I_CamStream_DirectShow_SessionData::operator+= (rhs_in);

    connectionState =
      (connectionState ? connectionState : rhs_in.connectionState);
    targetFileName =
      (targetFileName.empty () ? rhs_in.targetFileName : targetFileName);
    userData = (userData ? userData : rhs_in.userData);

    return *this;
  }

  struct Test_I_Target_ConnectionState* connectionState;
  std::string                           targetFileName;
  struct Test_I_Target_UserData*        userData;
};
typedef Stream_SessionData_T<struct Test_I_Target_SessionData> Test_I_Target_SessionData_t;
struct Test_I_Target_DirectShow_SessionData
 : Test_I_CamStream_DirectShow_SessionData
{
  inline Test_I_Target_DirectShow_SessionData ()
   : Test_I_CamStream_DirectShow_SessionData ()
   , connectionState (NULL)
   , userData (NULL)
  {};

  inline struct Test_I_Target_DirectShow_SessionData& operator+= (const struct Test_I_Target_DirectShow_SessionData& rhs_in)
  {
    // *NOTE*: the idea is to 'merge' the data
    Test_I_CamStream_DirectShow_SessionData::operator+= (rhs_in);

    connectionState =
      (connectionState ? connectionState : rhs_in.connectionState);
    userData = (userData ? userData : rhs_in.userData);

    return *this;
  }

  struct Test_I_Target_DirectShow_ConnectionState* connectionState;
  struct Test_I_Target_DirectShow_UserData*        userData;
};
typedef Stream_SessionData_T<struct Test_I_Target_DirectShow_SessionData> Test_I_Target_DirectShow_SessionData_t;
struct Test_I_Target_MediaFoundation_SessionData
 : Test_I_CamStream_MediaFoundation_SessionData
{
  inline Test_I_Target_MediaFoundation_SessionData ()
   : Test_I_CamStream_MediaFoundation_SessionData ()
   , connectionState (NULL)
   , userData (NULL)
  {};

  inline struct Test_I_Target_MediaFoundation_SessionData& operator+= (const struct Test_I_Target_MediaFoundation_SessionData& rhs_in)
  {
    // *NOTE*: the idea is to 'merge' the data
    Test_I_CamStream_MediaFoundation_SessionData::operator+= (rhs_in);

    connectionState =
      (connectionState ? connectionState : rhs_in.connectionState);
    userData = (userData ? userData : rhs_in.userData);

    return *this;
  }

  struct Test_I_Target_MediaFoundation_ConnectionState* connectionState;
  struct Test_I_Target_MediaFoundation_UserData*        userData;
};
typedef Stream_SessionData_T<struct Test_I_Target_MediaFoundation_SessionData> Test_I_Target_MediaFoundation_SessionData_t;
#else
struct Test_I_Target_SessionData
 : Test_I_CamStream_V4L2_SessionData
{
  inline Test_I_Target_SessionData ()
   : Test_I_CamStream_V4L2_SessionData ()
   , connectionState (NULL)
   , targetFileName ()
   , userData (NULL)
  {};

  inline struct Test_I_Target_SessionData& operator+= (const struct Test_I_Target_SessionData& rhs_in)
  {
    // *NOTE*: the idea is to 'merge' the data
    Test_I_CamStream_V4L2_SessionData::operator+= (rhs_in);

    connectionState =
      (connectionState ? connectionState : rhs_in.connectionState);
    targetFileName =
      (targetFileName.empty () ? rhs_in.targetFileName : targetFileName);
    userData = (userData ? userData : rhs_in.userData);

    return *this;
  }

  struct Test_I_Target_ConnectionState* connectionState;
  std::string                           targetFileName;
  struct Test_I_Target_UserData*        userData;
};
typedef Stream_SessionData_T<struct Test_I_Target_SessionData> Test_I_Target_SessionData_t;
#endif

#if defined (ACE_WIN32) || defined (ACE_WIN64)
struct Test_I_Target_DirectShow_SocketHandlerConfiguration
 : Net_SocketHandlerConfiguration
{
  inline Test_I_Target_DirectShow_SocketHandlerConfiguration ()
   : Net_SocketHandlerConfiguration ()
   ///////////////////////////////////////
   , userData (NULL)
  {};

  struct Test_I_Target_DirectShow_UserData* userData;
};
struct Test_I_Target_MediaFoundation_SocketHandlerConfiguration
 : Net_SocketHandlerConfiguration
{
  inline Test_I_Target_MediaFoundation_SocketHandlerConfiguration ()
   : Net_SocketHandlerConfiguration ()
   ///////////////////////////////////////
   , userData (NULL)
  {};

  struct Test_I_Target_MediaFoundation_UserData* userData;
};
#endif

#if defined (ACE_WIN32) || defined (ACE_WIN64)
struct Test_I_Target_DirectShow_PinConfiguration
{
  inline Test_I_Target_DirectShow_PinConfiguration ()
   : format (NULL)
   , queue (NULL)
  {};

  struct _AMMediaType*    format; // (preferred) media type handle
  ACE_Message_Queue_Base* queue;  // (inbound) buffer queue handle
};
struct Test_I_Target_DirectShow_FilterConfiguration
{
  inline Test_I_Target_DirectShow_FilterConfiguration ()
   : format (NULL)
   , module (NULL)
   , pinConfiguration (NULL)
  {};

  // *TODO*: specify this as part of the network protocol header/handshake
  struct _AMMediaType*                              format; // handle
  Stream_Module_t*                                  module; // handle
  struct Test_I_Target_DirectShow_PinConfiguration* pinConfiguration; // handle
};
#endif
typedef Stream_ISessionDataNotify_T<Stream_SessionId_t,
                                    struct Test_I_Target_SessionData,
                                    enum Stream_SessionMessageType,
                                    Test_I_Target_Stream_Message,
                                    Test_I_Target_Stream_SessionMessage> Test_I_Target_ISessionNotify_t;
typedef std::list<Test_I_Target_ISessionNotify_t*> Test_I_Target_Subscribers_t;
typedef Test_I_Target_Subscribers_t::iterator Test_I_Target_SubscribersIterator_t;
struct Test_I_Target_ModuleHandlerConfiguration
 : Test_I_CamStream_ModuleHandlerConfiguration
{
  inline Test_I_Target_ModuleHandlerConfiguration ()
   : Test_I_CamStream_ModuleHandlerConfiguration ()
   , area ()
   , connectionManager (NULL)
   , contextID (0)
#if defined (ACE_WIN32) || defined (ACE_WIN64)
   , format (NULL)
#else
   //, connection (NULL)
   , format ()
#endif
   , queue (NULL)
   , socketHandlerConfiguration (NULL)
   , targetFileName ()
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
   , subscriber (NULL)
   , subscribers (NULL)
   , v4l2Window (NULL)
#endif
   , window (NULL)
  {};

  GdkRectangle                                     area;
  Test_I_Target_InetConnectionManager_t*           connectionManager; // Net IO module
  guint                                            contextID;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  struct _AMMediaType*                             format; // handle
#else
  //Test_I_Target_IConnection_t*                         connection; // Net source/IO module
  struct v4l2_format                               format; // splitter module
#endif
  ACE_Message_Queue_Base*                          queue;  // (inbound) buffer queue handle
  struct Test_I_Target_SocketHandlerConfiguration* socketHandlerConfiguration;
  std::string                                      targetFileName; // file writer module
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
  Test_I_Target_ISessionNotify_t*                  subscriber;
  Test_I_Target_Subscribers_t*                     subscribers;
  struct v4l2_window*                              v4l2Window;
#endif
  GdkWindow*                                       window;
};
#if defined (ACE_WIN32) || defined (ACE_WIN64)
struct Test_I_Target_DirectShow_ModuleHandlerConfiguration
 : Test_I_CamStream_ModuleHandlerConfiguration
{
  inline Test_I_Target_DirectShow_ModuleHandlerConfiguration ()
   : Test_I_CamStream_ModuleHandlerConfiguration ()
   , area ()
   , builder (NULL)
   , connection (NULL)
   , connectionManager (NULL)
   , contextID (0)
   , device ()
   , filterCLSID ()
   , filterConfiguration (NULL)
   , format (NULL)
   , push (MODULE_MISC_DS_WIN32_FILTER_SOURCE_DEFAULT_PUSH)
   , queue (NULL)
   , socketHandlerConfiguration (NULL)
   , window (NULL)
   , windowController (NULL)
  {
    format =
      static_cast<struct _AMMediaType*> (CoTaskMemAlloc (sizeof (struct _AMMediaType)));
    if (!format)
    {
      ACE_DEBUG ((LM_CRITICAL,
                  ACE_TEXT ("failed to allocate memory, continuing\n")));
    } // end IF
    else
      ACE_OS::memset (format, 0, sizeof (struct _AMMediaType));
  };

  struct tagRECT                                              area;
  IGraphBuilder*                                              builder;
  Test_I_Target_DirectShow_IConnection_t*                     connection; // Net source/IO module
  Test_I_Target_DirectShow_InetConnectionManager_t*           connectionManager; // Net IO module
  guint                                                       contextID;
  std::string                                                 device; // FriendlyName
  struct Test_I_Target_DirectShow_FilterConfiguration*        filterConfiguration;
  struct _GUID                                                filterCLSID;
  struct _AMMediaType*                                        format; // splitter module
  bool                                                        push; // media sample passing strategy
  ACE_Message_Queue_Base*                                     queue;  // (inbound) buffer queue handle
  struct Test_I_Target_DirectShow_SocketHandlerConfiguration* socketHandlerConfiguration;
  HWND                                                        window;
  IVideoWindow*                                               windowController;
};
struct Test_I_Target_MediaFoundation_ModuleHandlerConfiguration
 : Test_I_CamStream_ModuleHandlerConfiguration
{
  inline Test_I_Target_MediaFoundation_ModuleHandlerConfiguration ()
   : Test_I_CamStream_ModuleHandlerConfiguration ()
   , area ()
   , connection (NULL)
   , connectionManager (NULL)
   , contextID (0)
   , device ()
   , format (NULL)
   , mediaSource (NULL)
   , queue (NULL)
   //, sourceReader (NULL)
   , session (NULL)
   , socketHandlerConfiguration (NULL)
   , window (NULL)
   , windowController (NULL)
  {
    HRESULT result = MFCreateMediaType (&format);
    if (FAILED (result))
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to MFCreateMediaType(): \"%s\", continuing\n"),
                  ACE_TEXT (Common_Tools::error2String (result).c_str ())));
  };

  struct tagRECT                                                   area;
  Test_I_Target_MediaFoundation_IConnection_t*                     connection; // Net source/IO module
  Test_I_Target_MediaFoundation_InetConnectionManager_t*           connectionManager; // Net IO module
  guint                                                            contextID;
  std::string                                                      device; // FriendlyName
  IMFMediaType*                                                    format;
  IMFMediaSource*                                                  mediaSource;
  ACE_Message_Queue_Base*                                          queue;  // (inbound) buffer queue handle
  //IMFSourceReaderEx*                                             sourceReader;
  IMFMediaSession*                                                 session;
  struct Test_I_Target_MediaFoundation_SocketHandlerConfiguration* socketHandlerConfiguration;
  HWND                                                             window;
  IMFVideoDisplayControl*                                          windowController;
};
#endif

#if defined (ACE_WIN32) || defined (ACE_WIN64)
//struct Test_I_Target_DirectShow_ListenerConfiguration
// : Net_ListenerConfiguration
//{
//  inline Test_I_Target_DirectShow_ListenerConfiguration ()
//   : Net_ListenerConfiguration ()
//   , connectionManager (NULL)
//   , socketHandlerConfiguration (NULL)
//   , statisticReportingInterval (NET_STREAM_DEFAULT_STATISTIC_REPORTING_INTERVAL, 0)
//  {
//    address.set_port_number (TEST_I_DEFAULT_PORT, 1);
//  };
//
//  Test_I_Target_DirectShow_InetConnectionManager_t*    connectionManager;
//  Test_I_Target_DirectShow_SocketHandlerConfiguration* socketHandlerConfiguration;
//  ACE_Time_Value                                       statisticReportingInterval; // [ACE_Time_Value::zero: off]
//};
//typedef Net_IListener_T<Test_I_Target_DirectShow_ListenerConfiguration,
//                        Test_I_Target_DirectShow_SocketHandlerConfiguration> Test_I_Target_DirectShow_IListener_t;
//struct Test_I_Target_MediaFoundation_ListenerConfiguration
// : Net_ListenerConfiguration
//{
//  inline Test_I_Target_MediaFoundation_ListenerConfiguration ()
//   : Net_ListenerConfiguration ()
//   , connectionManager (NULL)
//   , socketHandlerConfiguration (NULL)
//   , statisticReportingInterval (NET_STREAM_DEFAULT_STATISTIC_REPORTING_INTERVAL, 0)
//  {
//    address.set_port_number (TEST_I_DEFAULT_PORT, 1);
//  };
//
//  Test_I_Target_MediaFoundation_InetConnectionManager_t*    connectionManager;
//  Test_I_Target_MediaFoundation_SocketHandlerConfiguration* socketHandlerConfiguration;
//  ACE_Time_Value                                            statisticReportingInterval; // [ACE_Time_Value::zero: off]
//};
//typedef Net_IListener_T<Test_I_Target_MediaFoundation_ListenerConfiguration,
//                        Test_I_Target_MediaFoundation_SocketHandlerConfiguration> Test_I_Target_MediaFoundation_IListener_t;
#endif
struct Test_I_Target_ListenerConfiguration
 : Net_ListenerConfiguration
{
  inline Test_I_Target_ListenerConfiguration ()
   : Net_ListenerConfiguration ()
   , connectionManager (NULL)
   , socketHandlerConfiguration (NULL)
   , statisticReportingInterval (NET_STREAM_DEFAULT_STATISTIC_REPORTING_INTERVAL, 0)
  {
    address.set_port_number (TEST_I_DEFAULT_PORT, 1);
  };

  Test_I_Target_InetConnectionManager_t*           connectionManager;
  struct Test_I_Target_SocketHandlerConfiguration* socketHandlerConfiguration;
  ACE_Time_Value                                   statisticReportingInterval; // [ACE_Time_Value::zero: off]
};
typedef Net_IListener_T<struct Test_I_Target_ListenerConfiguration,
                        struct Test_I_Target_SocketHandlerConfiguration> Test_I_Target_IListener_t;

#if defined (ACE_WIN32) || defined (ACE_WIN64)
//struct Test_I_Target_DirectShow_SignalHandlerConfiguration
// : Common_SignalHandlerConfiguration
//{
//  inline Test_I_Target_DirectShow_SignalHandlerConfiguration ()
//   : Common_SignalHandlerConfiguration ()
//   , connectionManager (NULL)
//   , listener (NULL)
//   , statisticReportingHandler (NULL)
//   , statisticReportingTimerID (-1)
//  {};
//
//  Test_I_Target_DirectShow_InetConnectionManager_t* connectionManager;
//  Test_I_Target_DirectShow_IListener_t*             listener;
//  Test_I_StatisticReportingHandler_t*               statisticReportingHandler;
//  long                                              statisticReportingTimerID;
//};
//typedef Test_I_Target_SignalHandler_T<struct Test_I_Target_DirectShow_SignalHandlerConfiguration,
//                                      Test_I_Target_DirectShow_InetConnectionManager_t> Test_I_Target_DirectShow_SignalHandler_t;
//struct Test_I_Target_MediaFoundation_SignalHandlerConfiguration
// : Common_SignalHandlerConfiguration
//{
//  inline Test_I_Target_MediaFoundation_SignalHandlerConfiguration ()
//   : Common_SignalHandlerConfiguration ()
//   , connectionManager (NULL)
//   , listener (NULL)
//   , statisticReportingHandler (NULL)
//   , statisticReportingTimerID (-1)
//  {};
//
//  Test_I_Target_MediaFoundation_InetConnectionManager_t* connectionManager;
//  Test_I_Target_MediaFoundation_IListener_t*             listener;
//  Test_I_StatisticReportingHandler_t*                    statisticReportingHandler;
//  long                                                   statisticReportingTimerID;
//};
//typedef Test_I_Target_SignalHandler_T<struct Test_I_Target_MediaFoundation_SignalHandlerConfiguration,
//                                      Test_I_Target_MediaFoundation_InetConnectionManager_t> Test_I_Target_MediaFoundation_SignalHandler_t;
#endif
struct Test_I_Target_SignalHandlerConfiguration
 : Common_SignalHandlerConfiguration
{
  inline Test_I_Target_SignalHandlerConfiguration ()
   : Common_SignalHandlerConfiguration ()
   , connectionManager (NULL)
   , listener (NULL)
   , statisticReportingHandler (NULL)
   , statisticReportingTimerID (-1)
  {};

  Test_I_Target_InetConnectionManager_t* connectionManager;
  Test_I_Target_IListener_t*             listener;
  Test_I_StatisticReportingHandler_t*    statisticReportingHandler;
  long                                   statisticReportingTimerID;
};
typedef Test_I_Target_SignalHandler_T<struct Test_I_Target_SignalHandlerConfiguration,
                                      Test_I_Target_InetConnectionManager_t> Test_I_Target_SignalHandler_t;

#if defined (ACE_WIN32) || defined (ACE_WIN64)
struct Test_I_Target_DirectShow_StreamConfiguration
 : Stream_Configuration
{
  inline Test_I_Target_DirectShow_StreamConfiguration ()
   : Stream_Configuration ()
   , graphBuilder (NULL)
   , moduleHandlerConfiguration (NULL)
   , window (NULL)
  {};

  IGraphBuilder*                                              graphBuilder;
  struct Test_I_Target_DirectShow_ModuleHandlerConfiguration* moduleHandlerConfiguration;
  HWND                                                        window;
};
struct Test_I_Target_MediaFoundation_StreamConfiguration
  : Stream_Configuration
{
  inline Test_I_Target_MediaFoundation_StreamConfiguration ()
    : Stream_Configuration ()
    , moduleHandlerConfiguration (NULL)
  {};

  struct Test_I_Target_MediaFoundation_ModuleHandlerConfiguration* moduleHandlerConfiguration;
};
#endif
struct Test_I_Target_StreamConfiguration
 : Test_I_StreamConfiguration
{
  inline Test_I_Target_StreamConfiguration ()
   : Test_I_StreamConfiguration ()
   , moduleHandlerConfiguration (NULL)
   , window (NULL)
  {};

  struct Test_I_Target_ModuleHandlerConfiguration* moduleHandlerConfiguration;
  GdkWindow*                                       window;
};

#if defined (ACE_WIN32) || defined (ACE_WIN64)
struct Test_I_Target_DirectShow_StreamState
 : Stream_State
{
  inline Test_I_Target_DirectShow_StreamState ()
   : Stream_State ()
   , currentSessionData (NULL)
   , userData (NULL)
  {};

  struct Test_I_Target_DirectShow_SessionData* currentSessionData;
  struct Test_I_Target_DirectShow_UserData*    userData;
};
struct Test_I_Target_MediaFoundation_StreamState
 : Stream_State
{
  inline Test_I_Target_MediaFoundation_StreamState ()
   : Stream_State ()
   , currentSessionData (NULL)
   , userData (NULL)
  {};

  struct Test_I_Target_MediaFoundation_SessionData* currentSessionData;
  struct Test_I_Target_MediaFoundation_UserData*    userData;
};
#endif
struct Test_I_Target_StreamState
 : Stream_State
{
  inline Test_I_Target_StreamState ()
   : Stream_State ()
   , currentSessionData (NULL)
   , userData (NULL)
  {};

  struct Test_I_Target_SessionData* currentSessionData;
  struct Test_I_Target_UserData*    userData;
};

#if defined (ACE_WIN32) || defined (ACE_WIN64)
//struct Test_I_Target_DirectShow_Configuration
// : Test_I_CamStream_Configuration
//{
//  inline Test_I_Target_DirectShow_Configuration ()
//   : Test_I_CamStream_Configuration ()
//   , socketHandlerConfiguration ()
//   , handle (ACE_INVALID_HANDLE)
//   //, listener (NULL)
//   , listenerConfiguration ()
//   , signalHandlerConfiguration ()
//   , pinConfiguration ()
//   , filterConfiguration ()
//   , moduleHandlerConfiguration ()
//   , streamConfiguration ()
//   , userData ()
//  {};
//
//  // **************************** socket data **********************************
//  Test_I_Target_DirectShow_SocketHandlerConfiguration socketHandlerConfiguration;
//  // **************************** listener data ********************************
//  ACE_HANDLE                                          handle;
//  //Test_I_Target_IListener_t*               listener;
//  Test_I_Target_DirectShow_ListenerConfiguration      listenerConfiguration;
//  // **************************** signal data **********************************
//  Test_I_Target_DirectShow_SignalHandlerConfiguration signalHandlerConfiguration;
//  // **************************** stream data **********************************
//  Test_I_Target_DirectShow_PinConfiguration           pinConfiguration;
//  Test_I_Target_DirectShow_FilterConfiguration        filterConfiguration;
//  Test_I_Target_DirectShow_ModuleHandlerConfiguration moduleHandlerConfiguration;
//  Test_I_Target_DirectShow_StreamConfiguration        streamConfiguration;
//
//  Test_I_Target_DirectShow_UserData                   userData;
//};
//struct Test_I_Target_MediaFoundation_Configuration
// : Test_I_CamStream_Configuration
//{
//  inline Test_I_Target_MediaFoundation_Configuration ()
//   : Test_I_CamStream_Configuration ()
//   , mediaFoundationConfiguration ()
//   , signalHandlerConfiguration ()
//   , socketHandlerConfiguration ()
//   , handle (ACE_INVALID_HANDLE)
//   //, listener (NULL)
//   , listenerConfiguration ()
//   , moduleHandlerConfiguration ()
//   , streamConfiguration ()
//   , userData ()
//  {};
//
//  // **************************** media foundation *****************************
//  Test_I_MediaFoundationConfiguration                      mediaFoundationConfiguration;
//  // **************************** signal data **********************************
//  Test_I_Target_MediaFoundation_SignalHandlerConfiguration signalHandlerConfiguration;
//  // **************************** socket data **********************************
//  Test_I_Target_MediaFoundation_SocketHandlerConfiguration socketHandlerConfiguration;
//  // **************************** listener data ********************************
//  ACE_HANDLE                                               handle;
//  //Test_I_Target_IListener_t*                             listener;
//  Test_I_Target_MediaFoundation_ListenerConfiguration      listenerConfiguration;
//  // **************************** stream data **********************************
//  Test_I_Target_MediaFoundation_ModuleHandlerConfiguration moduleHandlerConfiguration;
//  Test_I_Target_MediaFoundation_StreamConfiguration        streamConfiguration;
//
//  Test_I_Target_MediaFoundation_UserData                   userData;
//};
#endif
struct Test_I_Target_Configuration
 : Test_I_CamStream_Configuration
{
  inline Test_I_Target_Configuration ()
   : Test_I_CamStream_Configuration ()
   , socketHandlerConfiguration ()
   , connectionConfiguration ()
   , handle (ACE_INVALID_HANDLE)
   //, listener (NULL)
   , listenerConfiguration ()
   , signalHandlerConfiguration ()
   , moduleHandlerConfiguration ()
   , streamConfiguration ()
   , userData ()
  {};

  // **************************** socket data **********************************
  struct Test_I_Target_SocketHandlerConfiguration socketHandlerConfiguration;
  struct Test_I_Target_ConnectionConfiguration    connectionConfiguration;
  // **************************** listener data ********************************
  ACE_HANDLE                                      handle;
  //Test_I_Target_IListener_t*               listener;
  struct Test_I_Target_ListenerConfiguration      listenerConfiguration;
  // **************************** signal data **********************************
  struct Test_I_Target_SignalHandlerConfiguration signalHandlerConfiguration;
  // **************************** stream data **********************************
  struct Test_I_Target_ModuleHandlerConfiguration moduleHandlerConfiguration;
  struct Test_I_Target_StreamConfiguration        streamConfiguration;

  struct Test_I_Target_UserData                   userData;
};

#if defined (ACE_WIN32) || defined (ACE_WIN64)
typedef Stream_ControlMessage_T<enum Stream_ControlMessageType,
                                struct Stream_AllocatorConfiguration,
                                Test_I_Target_DirectShow_Stream_Message,
                                Test_I_Target_DirectShow_Stream_SessionMessage> Test_I_Target_DirectShow_ControlMessage_t;

typedef Stream_MessageAllocatorHeapBase_T<ACE_MT_SYNCH,
                                          struct Stream_AllocatorConfiguration,
                                          Test_I_Target_DirectShow_ControlMessage_t,
                                          Test_I_Target_DirectShow_Stream_Message,
                                          Test_I_Target_DirectShow_Stream_SessionMessage> Test_I_Target_DirectShow_MessageAllocator_t;

typedef Stream_ControlMessage_T<enum Stream_ControlMessageType,
                                struct Stream_AllocatorConfiguration,
                                Test_I_Target_MediaFoundation_Stream_Message,
                                Test_I_Target_MediaFoundation_Stream_SessionMessage> Test_I_Target_MediaFoundation_ControlMessage_t;

typedef Stream_MessageAllocatorHeapBase_T<ACE_MT_SYNCH,
                                          struct Stream_AllocatorConfiguration,
                                          Test_I_Target_MediaFoundation_ControlMessage_t,
                                          Test_I_Target_MediaFoundation_Stream_Message,
                                          Test_I_Target_MediaFoundation_Stream_SessionMessage> Test_I_Target_MediaFoundation_MessageAllocator_t;
#endif
typedef Stream_ControlMessage_T<enum Stream_ControlMessageType,
                                struct Stream_AllocatorConfiguration,
                                Test_I_Target_Stream_Message,
                                Test_I_Target_Stream_SessionMessage> Test_I_Target_ControlMessage_t;

typedef Stream_MessageAllocatorHeapBase_T<ACE_MT_SYNCH,
                                          struct Stream_AllocatorConfiguration,
                                          Test_I_Target_ControlMessage_t,
                                          Test_I_Target_Stream_Message,
                                          Test_I_Target_Stream_SessionMessage> Test_I_Target_MessageAllocator_t;

//////////////////////////////////////////

#if defined (ACE_WIN32) || defined (ACE_WIN64)
typedef Stream_ISessionDataNotify_T<Stream_SessionId_t,
                                    struct Test_I_Target_DirectShow_SessionData,
                                    enum Stream_SessionMessageType,
                                    Test_I_Target_DirectShow_Stream_Message,
                                    Test_I_Target_DirectShow_Stream_SessionMessage> Test_I_Target_DirectShow_ISessionNotify_t;
typedef std::list<Test_I_Target_DirectShow_ISessionNotify_t*> Test_I_Target_DirectShow_Subscribers_t;
typedef Test_I_Target_DirectShow_Subscribers_t::iterator Test_I_Target_DirectShow_SubscribersIterator_t;
typedef Common_ISubscribe_T<Test_I_Target_DirectShow_ISessionNotify_t> Test_I_Target_DirectShow_ISubscribe_t;

typedef Stream_ISessionDataNotify_T<Stream_SessionId_t,
                                    struct Test_I_Target_MediaFoundation_SessionData,
                                    enum Stream_SessionMessageType,
                                    Test_I_Target_MediaFoundation_Stream_Message,
                                    Test_I_Target_MediaFoundation_Stream_SessionMessage> Test_I_Target_MediaFoundation_ISessionNotify_t;
typedef std::list<Test_I_Target_MediaFoundation_ISessionNotify_t*> Test_I_Target_MediaFoundation_Subscribers_t;
typedef Test_I_Target_MediaFoundation_Subscribers_t::iterator Test_I_Target_MediaFoundation_SubscribersIterator_t;
typedef Common_ISubscribe_T<Test_I_Target_MediaFoundation_ISessionNotify_t> Test_I_Target_MediaFoundation_ISubscribe_t;
#endif
typedef Common_ISubscribe_T<Test_I_Target_ISessionNotify_t> Test_I_Target_ISubscribe_t;

#if defined (ACE_WIN32) || defined (ACE_WIN64)
//struct Test_I_Target_DirectShow_GTK_CBData
// : Test_I_CamStream_GTK_CBData
//{
//  inline Test_I_Target_DirectShow_GTK_CBData ()
//   : Test_I_CamStream_GTK_CBData ()
//   , configuration (NULL)
//   , progressEventSourceID (0)
//   , subscribers ()
//   , subscribersLock ()
//  {};
//
//  Test_I_Target_DirectShow_Configuration* configuration;
//  guint                                   progressEventSourceID;
//  Test_I_Target_DirectShow_Subscribers_t  subscribers;
//  ACE_SYNCH_RECURSIVE_MUTEX               subscribersLock;
//};
//struct Test_I_Target_MediaFoundation_GTK_CBData
// : Test_I_CamStream_GTK_CBData
//{
//  inline Test_I_Target_MediaFoundation_GTK_CBData ()
//   : Test_I_CamStream_GTK_CBData ()
//   , configuration (NULL)
//   , progressEventSourceID (0)
//   , subscribers ()
//   , subscribersLock ()
//  {};
//
//  Test_I_Target_MediaFoundation_Configuration* configuration;
//  guint                                        progressEventSourceID;
//  Test_I_Target_MediaFoundation_Subscribers_t  subscribers;
//  ACE_SYNCH_RECURSIVE_MUTEX                    subscribersLock;
//};
#endif
struct Test_I_Target_GTK_CBData
 : Test_I_CamStream_GTK_CBData
{
  inline Test_I_Target_GTK_CBData ()
   : Test_I_CamStream_GTK_CBData ()
   , configuration (NULL)
   , progressEventSourceID (0)
   , subscribers ()
  {};

  struct Test_I_Target_Configuration* configuration;
  guint                               progressEventSourceID;
  Test_I_Target_Subscribers_t         subscribers;
};

#endif
