﻿/***************************************************************************
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

#ifndef TEST_I_IMAGESAVE_COMMON_H
#define TEST_I_IMAGESAVE_COMMON_H

#include <list>
#include <map>
#include <string>

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include <BaseTyps.h>
#include <OAIdl.h>
#include <control.h>
#include <CGuid.h>
#include <Guiddef.h>
#include <d3d9.h>
#include <evr.h>
#include <mfapi.h>
#include <mfidl.h>
#include <strmif.h>
#else
#include "linux/videodev2.h"

#ifdef __cplusplus
extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavutil/pixfmt.h"
}
#endif // __cplusplus
#endif // ACE_WIN32 || ACE_WIN64

#if defined (GUI_SUPPORT)
#if defined (GTK_USE)
#include "gtk/gtk.h"
#elif defined (WXWIDGETS_USE)
#include "wx/apptrait.h"
#include "wx/window.h"
#endif
#endif // GUI_SUPPORT

#include "ace/Singleton.h"
#include "ace/Synch_Traits.h"

#include "common_isubscribe.h"
#include "common_tools.h"

#if defined (GUI_SUPPORT)
#include "common_ui_common.h"
#if defined (GTK_USE)
#include "common_ui_gtk_builder_definition.h"
#include "common_ui_gtk_common.h"
#include "common_ui_gtk_manager.h"
#include "common_ui_gtk_manager_common.h"
#elif defined (WXWIDGETS_USE)
#include "common_ui_wxwidgets_application.h"
#include "common_ui_wxwidgets_common.h"
#include "common_ui_wxwidgets_xrc_definition.h"
#endif
#endif // GUI_SUPPORT

#include "stream_common.h"
#include "stream_control_message.h"
#include "stream_inotify.h"
#include "stream_isessionnotify.h"
#include "stream_istreamcontrol.h"
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include "stream_messageallocatorheap_base.h"
#else
#include "stream_messageallocatorheap_base.h"
#endif // ACE_WIN32 || ACE_WIN64
#include "stream_session_data.h"

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include "stream_lib_directdraw_common.h"
#include "stream_lib_directshow_tools.h"
#else
#include "stream_lib_ffmpeg_common.h"
#endif // ACE_WIN32 || ACE_WIN64

#include "stream_dev_common.h"
#include "stream_dev_defines.h"

#include "stream_vis_common.h"
#include "stream_vis_defines.h"

#include "test_i_common.h"
#include "test_i_configuration.h"
#if defined (GUI_SUPPORT)
#if defined (GTK_USE)
#include "test_i_gtk_common.h"
#elif defined (QT_USE)
#include "test_i_qt_common.h"
#elif defined (WXWIDGETS_USE)
#include "test_i_wxwidgets_common.h"

#include "camsave_wxwidgets_ui.h"
#endif
#endif // GUI_SUPPORT

// forward declarations
#if defined (ACE_WIN32) || defined (ACE_WIN64)
struct ISampleGrabber;
#endif // ACE_WIN32 || ACE_WIN64
class Stream_IAllocator;
template <typename NotificationType,
          typename DataMessageType,
#if defined (GUI_SUPPORT)
          typename UIStateType,
#if defined (WXWIDGETS_USE)
          typename InterfaceType, // implements Common_UI_wxWidgets_IApplicationBase_T
#endif // WXWIDGETS_USE
#endif // GUI_SUPPORT
          typename SessionMessageType>
class Test_I_EventHandler_T;
#if defined (GUI_SUPPORT)
#if defined (WXWIDGETS_USE)
template <typename WidgetBaseClassType,
          typename InterfaceType,
          typename StreamType>
class Test_I_WxWidgetsDialog_T;
#endif // WXWIDGETS_USE
#endif // GUI_SUPPORT

struct Test_I_StatisticData
 : Stream_Statistic
{
  Test_I_StatisticData ()
   : Stream_Statistic ()
#if defined (ACE_WIN32) || defined (ACE_WIN64)
   , capturedFrames (0)
#endif // ACE_WIN32 || ACE_WIN64
  {}

  struct Test_I_StatisticData operator+= (const struct Test_I_StatisticData& rhs_in)
  {
    Stream_Statistic::operator+= (rhs_in);
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    capturedFrames += rhs_in.capturedFrames;
#endif // ACE_WIN32 || ACE_WIN64

    return *this;
  }

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  unsigned int capturedFrames;
#endif // ACE_WIN32 || ACE_WIN64
};
typedef Common_StatisticHandler_T<struct Test_I_StatisticData> Test_I_ImageSave_StatisticHandler_t;

struct Test_I_ImageSave_StreamState;
class Test_I_ImageSave_SessionData
 : public Stream_SessionDataMediaBase_T<struct Test_I_SessionData,
                                        struct Stream_MediaFramework_FFMPEG_VideoMediaType,
                                        struct Test_I_ImageSave_StreamState,
                                        struct Test_I_StatisticData,
                                        struct Stream_UserData>
{
 public:
  Test_I_ImageSave_SessionData ()
   : Stream_SessionDataMediaBase_T<struct Test_I_SessionData,
                                   struct Stream_MediaFramework_FFMPEG_VideoMediaType,
                                   struct Test_I_ImageSave_StreamState,
                                   struct Test_I_StatisticData,
                                   struct Stream_UserData> ()
  {}

  Test_I_ImageSave_SessionData& operator+= (const Test_I_ImageSave_SessionData& rhs_in)
  {
    // *NOTE*: the idea is to 'merge' the data
    Stream_SessionDataMediaBase_T<struct Test_I_SessionData,
                                  struct Stream_MediaFramework_FFMPEG_VideoMediaType,
                                  struct Test_I_ImageSave_StreamState,
                                  struct Test_I_StatisticData,
                                  struct Stream_UserData>::operator+= (rhs_in);

    return *this;
  }

 private:
  //ACE_UNIMPLEMENTED_FUNC (Test_I_ImageSave_SessionData (const Test_I_ImageSave_SessionData&))
  ACE_UNIMPLEMENTED_FUNC (Test_I_ImageSave_SessionData& operator= (const Test_I_ImageSave_SessionData&))
};
typedef Stream_SessionData_T<Test_I_ImageSave_SessionData> Test_I_ImageSave_SessionData_t;

struct Test_I_ImageSave_SignalHandlerConfiguration
 : Common_SignalHandlerConfiguration
{
  Test_I_ImageSave_SignalHandlerConfiguration ()
   : Common_SignalHandlerConfiguration ()
   , actionTimerId (-1)
   , messageAllocator (NULL)
   , statisticReportingInterval (0)
  {}

  long               actionTimerId;
  Stream_IAllocator* messageAllocator;
  unsigned int       statisticReportingInterval; // (statistic) reporting interval (second(s)) [0: off]
};

class Test_I_Message;
template <typename DataMessageType,
          typename SessionDataType>
class Test_I_SessionMessage_T;
typedef Test_I_SessionMessage_T<Test_I_Message,
                                Test_I_ImageSave_SessionData_t> Test_I_SessionMessage_t;

typedef Stream_ISessionDataNotify_T<Stream_SessionId_t,
                                    Test_I_ImageSave_SessionData,
                                    enum Stream_SessionMessageType,
                                    Test_I_Message,
                                    Test_I_SessionMessage_t> Test_I_ISessionNotify_t;
typedef std::list<Test_I_ISessionNotify_t*> Test_I_Subscribers_t;
typedef Test_I_Subscribers_t::iterator Test_I_SubscribersIterator_t;

struct Test_I_ImageSave_ModuleHandlerConfiguration
 : Test_I_ModuleHandlerConfiguration
{
  Test_I_ImageSave_ModuleHandlerConfiguration ()
   : Test_I_ModuleHandlerConfiguration ()
   , codecId (AV_CODEC_ID_NONE)
#if defined (ACE_WIN32) || defined (ACE_WIN64)
   , direct3DConfiguration (NULL)
//   , window (NULL)
#else
   , display ()
   , fullScreen (false)
   , window (None)
   , X11Display (NULL)
#endif // ACE_WIN32 || ACE_WIN64
   , outputFormat ()
   , subscriber (NULL)
   , subscribers (NULL)
   , targetFileName ()
  {
    concurrency = STREAM_HEADMODULECONCURRENCY_ACTIVE;
    hasHeader = true;
  }

  enum AVCodecID                                     codecId;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  struct Stream_MediaFramework_Direct3D_Configuration* direct3DConfiguration;
//  HWND                                               window;
#else
  struct Common_UI_DisplayDevice                     display;
  bool                                               fullScreen;
  Window                                             window;
  Display*                                           X11Display;
#endif // ACE_WIN32 || ACE_WIN64
  struct Stream_MediaFramework_FFMPEG_VideoMediaType outputFormat;
  Test_I_ISessionNotify_t*                           subscriber;
  Test_I_Subscribers_t*                              subscribers;
  std::string                                        targetFileName;
};

struct Test_I_ImageSave_StreamState
 : Stream_State
{
  Test_I_ImageSave_StreamState ()
   : Stream_State ()
   , sessionData (NULL)
   , userData (NULL)
  {}

  Test_I_ImageSave_SessionData* sessionData;

  struct Stream_UserData*       userData;
};

struct Test_I_ImageSave_StreamConfiguration
 : Stream_Configuration
{
  Test_I_ImageSave_StreamConfiguration ()
   : Stream_Configuration ()
   , format ()
  {
    printFinalReport = true;
  }

  struct Stream_MediaFramework_FFMPEG_VideoMediaType format;
};
//extern const char stream_name_string_[];
typedef Stream_Configuration_T<//stream_name_string_,
                               struct Test_I_ImageSave_StreamConfiguration,
                               struct Test_I_ImageSave_ModuleHandlerConfiguration> Test_I_StreamConfiguration_t;

typedef Stream_IStreamControl_T<enum Stream_ControlType,
                                enum Stream_SessionMessageType,
                                enum Stream_StateMachine_ControlState,
                                struct Test_I_ImageSave_StreamState> Test_I_IStreamControl_t;

struct Test_I_ImageSave_Configuration
#if defined (GUI_SUPPORT)
#if defined (GTK_USE)
 : Test_I_GTK_Configuration
#else
 : Test_I_Configuration
#endif // GTK_USE
#endif // GUI_SUPPORT
{
  Test_I_ImageSave_Configuration ()
#if defined (GUI_SUPPORT)
#if defined (GTK_USE)
   : Test_I_GTK_Configuration ()
#else
   : Test_I_Configuration ()
#endif // GTK_USE
#endif // GUI_SUPPORT
   , signalHandlerConfiguration ()
#if defined (ACE_WIN32) || defined (ACE_WIN64)
   , direct3DConfiguration ()
#endif // ACE_WIN32 || ACE_WIN64
   , streamConfiguration ()
   , userData ()
  {}

  // **************************** signal data **********************************
  struct Test_I_SignalHandlerConfiguration            signalHandlerConfiguration;
  // **************************** stream data **********************************
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  struct Stream_MediaFramework_Direct3D_Configuration direct3DConfiguration;
#endif // ACE_WIN32 || ACE_WIN64
  Test_I_StreamConfiguration_t                        streamConfiguration;

  struct Stream_UserData                              userData;
};

typedef Stream_MessageAllocatorHeapBase_T<ACE_MT_SYNCH,
                                          struct Common_AllocatorConfiguration,
                                          Stream_ControlMessage_t,
                                          Test_I_Message,
                                          Test_I_SessionMessage_t> Test_I_MessageAllocator_t;

#if defined (GUI_SUPPORT)
#if defined (WXWIDGETS_USE)
struct Test_I_UI_CBData;
typedef Common_UI_wxWidgets_IApplication_T<struct Common_UI_wxWidgets_State,
                                           struct Test_I_UI_CBData> Test_I_WxWidgetsIApplication_t;
#endif // WXWIDGETS_USE
#endif // GUI_SUPPORT
typedef Common_ISubscribe_T<Test_I_ISessionNotify_t> Test_I_ISubscribe_t;

typedef Test_I_EventHandler_T<Test_I_ISessionNotify_t,
                              Test_I_Message,
#if defined (GUI_SUPPORT)
#if defined (GTK_USE)
                              Common_UI_GTK_State_t,
#elif defined (WXWIDGETS_USE)
                              struct Common_UI_wxWidgets_State,
                              Common_UI_wxWidgets_IApplicationBase_t,
#endif
#endif // GUI_SUPPORT
                              Test_I_SessionMessage_t> Test_I_EventHandler_t;

//////////////////////////////////////////

#if defined (GUI_SUPPORT)
struct Test_I_ImageSave_ProgressData
#if defined (GTK_USE)
 : Test_I_GTK_ProgressData
#elif defined (QT_USE)
 : Test_I_Qt_ProgressData
#elif defined (WXWIDGETS_USE)
 : Test_I_wxWidgets_ProgressData
#endif
{
  Test_I_ImageSave_ProgressData ()
#if defined (GTK_USE)
   : Test_I_GTK_ProgressData ()
#elif defined (QT_USE)
    : Test_I_Qt_ProgressData ()
#elif defined (WXWIDGETS_USE)
   : Test_I_wxWidgets_ProgressData ()
#endif
   , statistic ()
  {}

  struct Test_I_StatisticData statistic;
};

class Test_I_Stream;
struct Test_I_ImageSave_UI_CBData
#if defined (GTK_USE)
 : Test_I_GTK_CBData
#elif defined (QT_USE)
 : Test_I_Qt_CBData
#elif defined (WXWIDGETS_USE)
 : Test_I_wxWidgets_CBData
#endif
{
  Test_I_ImageSave_UI_CBData ()
#if defined (GTK_USE)
   : Test_I_GTK_CBData ()
#elif defined (QT_USE)
   : Test_I_Qt_CBData ()
#elif defined (WXWIDGETS_USE)
   : Test_I_wxWidgets_CBData ()
#endif
   , configuration (NULL)
   , progressData ()
   , stream (NULL)
   , subscribers ()
  {
    progressData.state = UIState;
  }

  struct Test_I_ImageSave_Configuration* configuration;
  struct Test_I_ImageSave_ProgressData   progressData;
  Test_I_Stream*                         stream;
  Test_I_Subscribers_t                   subscribers;
};

struct Test_I_ImageSave_UI_ThreadData
#if defined (GTK_USE)
 : Test_I_GTK_ThreadData
#elif defined (QT_USE)
 : Test_I_Qt_ThreadData
#elif defined (WXWIDGETS_USE)
 : Test_I_wxWidgets_ThreadData
#endif
{
  Test_I_ImageSave_UI_ThreadData ()
#if defined (GTK_USE)
   : Test_I_GTK_ThreadData ()
#elif defined (QT_USE)
   : Test_I_Qt_ThreadData ()
#elif defined (WXWIDGETS_USE)
   : Test_I_wxWidgets_ThreadData ()
#endif
   , CBData (NULL)
  {}

  struct Test_I_ImageSave_UI_CBData* CBData;
};

#if defined (GTK_USE)
#elif defined (WXWIDGETS_USE)
extern const char toplevel_widget_classname_string_[];
typedef Common_UI_WxWidgetsXRCDefinition_T<struct Common_UI_wxWidgets_State,
                                           toplevel_widget_classname_string_> Test_I_WxWidgetsXRCDefinition_t;
typedef Test_I_WxWidgetsDialog_T<wxDialog_main,
                                 Test_I_WxWidgetsIApplication_t,
                                 Test_I_Stream> Test_I_WxWidgetsDialog_t;
typedef Comon_UI_WxWidgets_Application_T<Test_I_WxWidgetsXRCDefinition_t,
                                         struct Common_UI_wxWidgets_State,
                                         struct Test_I_ImageSave_UI_CBData,
                                         Test_I_V4L_WxWidgetsDialog_t,
                                         wxGUIAppTraits> Test_I_WxWidgetsApplication_t;
#endif
#endif // GUI_SUPPORT

#endif