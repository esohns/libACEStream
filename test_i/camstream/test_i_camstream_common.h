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

#ifndef TEST_I_CAMSTREAM_COMMON_H
#define TEST_I_CAMSTREAM_COMMON_H

#include <limits>
#include <map>
#include <set>
#include <string>

#include "ace/Synch_Traits.h"

#if defined (ACE_WIN32) || defined (ACE_WIN64)
//#include <cguid.h>
//#include <evr.h>
//#include <mfapi.h>
//#include <mfidl.h>
////#include "mfobjects.h>
//#include "mfreadwrite.h>
//#include "strmif.h>
#else
#include "linux/videodev2.h"

#include "gtk/gtk.h"
#endif

#include "common.h"
#include "common_istatistic.h"
#include "common_isubscribe.h"
#include "common_time_common.h"

#include "common_ui_common.h"

#include "stream_base.h"
#include "stream_common.h"
#include "stream_data_base.h"
#include "stream_inotify.h"
#include "stream_session_data.h"
#include "stream_statemachine_control.h"

#include "stream_dev_defines.h"
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include "stream_dev_directshow_tools.h"
#endif
#include "stream_dev_tools.h"

#include "stream_lib_common.h"
#include "stream_lib_defines.h"

#include "stream_net_common.h"

#include "net_configuration.h"
#include "net_defines.h"

#include "test_i_common.h"
#include "test_i_gtk_common.h"

#include "test_i_configuration.h"
#include "test_i_connection_common.h"
#include "test_i_connection_manager_common.h"
#include "test_i_defines.h"

// forward declarations
#if defined (ACE_WIN32) || defined (ACE_WIN64)
struct IGraphBuilder;
struct IVideoWindow;
#endif
class Stream_IAllocator;

typedef int Test_I_HeaderType_t;
typedef int Test_I_CommandType_t;

typedef Stream_Statistic Test_I_Statistic_t;

typedef Common_IStatistic_T<Test_I_Statistic_t> Test_I_StatisticReportingHandler_t;

struct Test_I_CamStream_ConnectionConfiguration;
struct Test_I_StreamConfiguration;
struct Test_I_CamStream_UserData
 : Test_I_UserData
{
  Test_I_CamStream_UserData ()
   : Test_I_UserData ()
   , configuration (NULL)
   , streamConfiguration (NULL)
  {};

  struct Test_I_CamStream_ConnectionConfiguration* configuration;
  struct Test_I_StreamConfiguration*               streamConfiguration;
};

#if defined (ACE_WIN32) || defined (ACE_WIN64)
struct Test_I_CamStream_DirectShow_SessionData
 : Test_I_SessionData
{
  Test_I_CamStream_DirectShow_SessionData ()
   : Test_I_SessionData ()
   , direct3DDevice (NULL)
   , inputFormat (NULL)
   , resetToken (0)
   , userData (NULL)
  {
    inputFormat =
      static_cast<struct _AMMediaType*> (CoTaskMemAlloc (sizeof (struct _AMMediaType)));
    if (!inputFormat)
      ACE_DEBUG ((LM_CRITICAL,
                  ACE_TEXT ("failed to allocate memory, continuing\n")));
    else
      ACE_OS::memset (inputFormat, 0, sizeof (struct _AMMediaType));
  };

  struct Test_I_CamStream_DirectShow_SessionData& operator+= (const struct Test_I_CamStream_DirectShow_SessionData& rhs_in)
  {
    // *NOTE*: the idea is to 'merge' the data
    Test_I_SessionData::operator+= (rhs_in);

    // sanity check(s)
    ACE_ASSERT (rhs_in.inputFormat);

    if (inputFormat)
      Stream_Module_Device_DirectShow_Tools::deleteMediaType (inputFormat);
    if (!Stream_Module_Device_DirectShow_Tools::copyMediaType (*rhs_in.inputFormat,
                                                               inputFormat))
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to Stream_Module_Device_DirectShow_Tools::copyMediaType(), continuing\n")));

    userData = (userData ? userData : rhs_in.userData);

    return *this;
  };

  IDirect3DDevice9Ex*               direct3DDevice;
  struct _AMMediaType*              inputFormat;
  UINT                              resetToken; // direct 3D manager 'id'
  struct Test_I_CamStream_UserData* userData;
};
typedef Stream_SessionData_T<struct Test_I_CamStream_DirectShow_SessionData> Test_I_CamStream_DirectShow_SessionData_t;
struct Test_I_CamStream_MediaFoundation_SessionData
 : Test_I_SessionData
{
  Test_I_CamStream_MediaFoundation_SessionData ()
   : Test_I_SessionData ()
   , direct3DDevice (NULL)
   , direct3DManagerResetToken (0)
   , inputFormat (NULL)
   , rendererNodeId (0)
   , session (NULL)
   //, topology (NULL)
   , userData (NULL)
  {
    inputFormat =
      static_cast<struct _AMMediaType*> (CoTaskMemAlloc (sizeof (struct _AMMediaType)));
    if (!inputFormat)
      ACE_DEBUG ((LM_CRITICAL,
                  ACE_TEXT ("failed to allocate memory, continuing\n")));
    else
      ACE_OS::memset (inputFormat, 0, sizeof (struct _AMMediaType));
  };

  struct Test_I_CamStream_MediaFoundation_SessionData& operator+= (const struct Test_I_CamStream_MediaFoundation_SessionData& rhs_in)
  {
    // *NOTE*: the idea is to 'merge' the data
    Test_I_SessionData::operator+= (rhs_in);

    // sanity check(s)
    ACE_ASSERT (rhs_in.inputFormat);

    if (inputFormat)
      Stream_Module_Device_DirectShow_Tools::deleteMediaType (inputFormat);
    if (!Stream_Module_Device_DirectShow_Tools::copyMediaType (*rhs_in.inputFormat,
                                                               inputFormat))
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to Stream_Module_Device_DirectShow_Tools::copyMediaType(), continuing\n")));

    userData = (userData ? userData : rhs_in.userData);

    return *this;
  };

  IDirect3DDevice9Ex*               direct3DDevice;
  UINT                              direct3DManagerResetToken;
  struct _AMMediaType*              inputFormat;
  TOPOID                            rendererNodeId;
  IMFMediaSession*                  session;
  //IMFTopology*               topology;

  struct Test_I_CamStream_UserData* userData;
};
typedef Stream_SessionData_T<struct Test_I_CamStream_MediaFoundation_SessionData> Test_I_CamStream_MediaFoundation_SessionData_t;
struct Test_I_CamStream_SessionData
 : Test_I_SessionData
{
  Test_I_CamStream_SessionData ()
   : Test_I_SessionData ()
   , userData (NULL)
  {};

  struct Test_I_CamStream_SessionData& operator+= (const struct Test_I_CamStream_SessionData& rhs_in)
  {
    // *NOTE*: the idea is to 'merge' the data
    Test_I_SessionData::operator+= (rhs_in);

    userData = (userData ? userData : rhs_in.userData);

    return *this;
  };

  struct Test_I_CamStream_UserData* userData;
};
typedef Stream_SessionData_T<struct Test_I_CamStream_SessionData> Test_I_CamStream_SessionData_t;
#else
struct Test_I_CamStream_V4L2_SessionData
 : Test_I_SessionData
{
  Test_I_CamStream_V4L2_SessionData ()
   : Test_I_SessionData ()
   , frameRate ()
   , inputFormat ()
   , userData (NULL)
  {};

  struct Test_I_CamStream_V4L2_SessionData& operator+= (const struct Test_I_CamStream_V4L2_SessionData& rhs_in)
  {
    // *NOTE*: the idea is to 'merge' the data
    Test_I_SessionData::operator+= (rhs_in);

    frameRate = rhs_in.frameRate;
    inputFormat = rhs_in.inputFormat;
    userData = (userData ? userData : rhs_in.userData);

    return *this;
  };

  struct v4l2_fract                 frameRate;
  struct v4l2_format                inputFormat;

  struct Test_I_CamStream_UserData* userData;
};
//typedef Stream_SessionData_T<struct Test_I_CamStream_SessionData> Test_I_CamStream_SessionData_t;
#endif

// forward declarations
struct Test_I_CamStream_Configuration;
struct Test_I_CamStream_ModuleHandlerConfiguration
 : Test_I_ModuleHandlerConfiguration
{
  Test_I_CamStream_ModuleHandlerConfiguration ()
   : Test_I_ModuleHandlerConfiguration ()
   , configuration (NULL)
   , contextId (0)
   , fullScreen (false)
#if defined (ACE_WIN32) || defined (ACE_WIN64)
   , interfaceIdentifier (GUID_NULL)
   , mediaFramework (MODULE_LIB_DEFAULT_MEDIAFRAMEWORK)
#else
   , interfaceIdentifier (ACE_TEXT_ALWAYS_CHAR (MODULE_DEV_DEFAULT_VIDEO_DEVICE))
#endif
   , pixelBuffer (NULL)
   , pixelBufferLock (NULL)
#if defined (ACE_WIN32) || defined (ACE_WIN64)
   , window ()
#else
   , window (NULL)
#endif
  {};

  struct Test_I_CamStream_Configuration* configuration;
  guint                                  contextId;
  bool                                   fullScreen;
  // *PORTABILITY*: UNIX: v4l2 device file (e.g. "/dev/video0" (Linux))
  //                Win32: interface GUID
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  struct _GUID                           interfaceIdentifier;
  enum Stream_MediaFramework_Type        mediaFramework;
#else
  std::string                            interfaceIdentifier;
#endif
  GdkPixbuf*                             pixelBuffer;
  ACE_SYNCH_MUTEX*                       pixelBufferLock;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  HWND                                   window;
#else
  GdkWindow*                             window;
#endif
};

struct Test_I_CamStream_Configuration
 : Test_I_Configuration
{
  Test_I_CamStream_Configuration ()
   : Test_I_Configuration ()
   , protocol (TEST_I_DEFAULT_TRANSPORT_LAYER)
  {};

  // *************************** protocol data *********************************
  enum Net_TransportLayerType protocol;
};

struct Test_I_CamStream_GTK_ProgressData
 : Test_I_GTK_ProgressData
{
  Test_I_CamStream_GTK_ProgressData ()
   : Test_I_GTK_ProgressData ()
   , transferred (0)
  {};

  size_t transferred; // bytes
};

struct Test_I_CamStream_GTK_CBData
 : Test_I_GTK_CBData
{
  Test_I_CamStream_GTK_CBData ()
   : Test_I_GTK_CBData ()
   , configuration (NULL)
   , isFirst (true)
#if defined (ACE_WIN32) || defined (ACE_WIN64)
   , mediaFramework (MODULE_LIB_DEFAULT_MEDIAFRAMEWORK)
#endif
   , pixelBuffer (NULL)
   , progressData ()
  {
    progressData.state = this;
  };

  struct Test_I_CamStream_Configuration*   configuration;
  bool                                     isFirst; // first activation ?
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  enum Stream_MediaFramework_Type          mediaFramework;
#endif
  GdkPixbuf*                               pixelBuffer;
  struct Test_I_CamStream_GTK_ProgressData progressData;
};

struct Test_I_CamStream_ThreadData
 : Test_I_ThreadData
{
  Test_I_CamStream_ThreadData ()
   : Test_I_ThreadData ()
   , CBData (NULL)
#if defined (ACE_WIN32) || defined (ACE_WIN64)
   , mediaFramework (MODULE_LIB_DEFAULT_MEDIAFRAMEWORK)
#endif
  {};

  struct Test_I_CamStream_GTK_CBData* CBData;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  enum Stream_MediaFramework_Type     mediaFramework;
#endif
};

#endif
