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
#include "stdafx.h"

#include <iostream>
#include <string>

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include <initguid.h> // *NOTE*: this exports DEFINE_GUIDs (see stream_misc_common.h)
#include <mfapi.h>
#endif // ACE_WIN32 || ACE_WIN64

#if defined (GTK_SUPPORT)
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include "gdk/gdkwin32.h"
#endif // ACE_WIN32 || ACE_WIN64
#include "gtk/gtk.h"
#endif // GTK_SUPPORT

#include "ace/Get_Opt.h"
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include "ace/Init_ACE.h"
#endif // ACE_WIN32 || ACE_WIN64
#include "ace/Log_Msg.h"
#include "ace/Profile_Timer.h"
#include "ace/Sig_Handler.h"
#include "ace/Signal.h"
#include "ace/Synch.h"
#include "ace/Version.h"

#include "common_file_tools.h"
#include "common_logger.h"
#include "common_signal_tools.h"
#include "common_tools.h"

#include "common_log_tools.h"

#include "common_timer_manager_common.h"
#include "common_timer_tools.h"

#if defined (GUI_SUPPORT)
#include "common_ui_defines.h"
#if defined (GTK_USE)
#include "common_ui_gtk_builder_definition.h"
#include "common_ui_gtk_manager_common.h"
#elif defined (WXWIDGETS_USE)
#include "common_ui_wxwidgets_application.h"
#include "common_ui_wxwidgets_tools.h"
#endif
#endif // GUI_SUPPORT

#if defined (HAVE_CONFIG_H)
#include "libACEStream_config.h"
#endif // HAVE_CONFIG_H

#include "stream_allocatorheap.h"
#include "stream_control_message.h"
#include "stream_macros.h"

#include "stream_dev_defines.h"
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include "stream_dev_mediafoundation_tools.h"
#endif // ACE_WIN32 || ACE_WIN64
#include "stream_dev_tools.h"

#include "stream_lib_tools.h"

#include "stream_misc_defines.h"

#include "stream_vis_tools.h"

#include "test_u_common.h"
#include "test_u_defines.h"

#include "test_u_camsave_defines.h"
#include "test_u_camsave_eventhandler.h"
#if defined (GUI_SUPPORT)
#if defined (GTK_USE)
#include "test_u_camsave_gtk_callbacks.h"
#elif defined (WXWIDGETS_USE)
#include "test_u_camsave_ui.h"
#endif
#endif // GUI_SUPPORT
#include "test_u_camsave_signalhandler.h"
#include "test_u_camsave_stream.h"

const char stream_name_string_[] = ACE_TEXT_ALWAYS_CHAR ("CamSaveStream");
#if defined (GUI_SUPPORT)
#if defined (WXWIDGETS_SUPPORT)
const char toplevel_widget_classname_string_[] =
  ACE_TEXT_ALWAYS_CHAR (TEST_U_STREAM_UI_WXWIDGETS_TOPLEVEL_WIDGET_CLASS_NAME);
const char toplevel_widget_name_string_[] =
  ACE_TEXT_ALWAYS_CHAR (TEST_U_STREAM_UI_WXWIDGETS_TOPLEVEL_WIDGET_NAME);
#endif // WXWIDGETS_SUPPORT
#endif // GUI_SUPPORT

void
do_printUsage (const std::string& programName_in)
{
  STREAM_TRACE (ACE_TEXT ("::do_printUsage"));

  // enable verbatim boolean output
  std::cout.setf (std::ios::boolalpha);

  std::string configuration_path =
    Common_File_Tools::getWorkingDirectory ();

  std::cout << ACE_TEXT_ALWAYS_CHAR ("usage: ")
            << programName_in
            << ACE_TEXT_ALWAYS_CHAR (" [OPTIONS]")
            << std::endl
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("currently available options:")
            << std::endl;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-2          : use Direct2D renderer [")
            << (STREAM_VIS_RENDERER_VIDEO_DEFAULT == STREAM_VISUALIZATION_VIDEORENDERER_DIRECTDRAW_2D)
            << ACE_TEXT_ALWAYS_CHAR ("])")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-3          : use Direct3D renderer [")
            << (STREAM_VIS_RENDERER_VIDEO_DEFAULT == STREAM_VISUALIZATION_VIDEORENDERER_DIRECTDRAW_3D)
            << ACE_TEXT_ALWAYS_CHAR ("])")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-c          : show console [")
            << false
            << ACE_TEXT_ALWAYS_CHAR ("])")
            << std::endl;
#endif // ACE_WIN32 || ACE_WIN64
  std::string capture_device_identifier;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  switch (STREAM_LIB_DEFAULT_MEDIAFRAMEWORK)
  {
    case STREAM_MEDIAFRAMEWORK_DIRECTSHOW:
    {
      capture_device_identifier =
        Stream_Device_DirectShow_Tools::getDefaultCaptureDevice (CLSID_VideoInputDeviceCategory);
      capture_device_identifier =
        Stream_Device_DirectShow_Tools::devicePathToString (capture_device_identifier);
      break;
    }
    case STREAM_MEDIAFRAMEWORK_MEDIAFOUNDATION:
    {
      // *TODO*
      ACE_ASSERT (false);
      ACE_NOTSUP;
      ACE_NOTREACHED (return;)
      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown media framework (was: %d), returning\n"),
                  STREAM_LIB_DEFAULT_MEDIAFRAMEWORK));
      return;
    }
  } // end SWITCH
#else
  capture_device_identifier =
    ACE_TEXT_ALWAYS_CHAR (MODULE_DEV_DEVICE_DIRECTORY);
  capture_device_identifier += ACE_DIRECTORY_SEPARATOR_CHAR;
  capture_device_identifier +=
    ACE_TEXT_ALWAYS_CHAR (MODULE_DEV_DEFAULT_VIDEO_DEVICE);
#endif // ACE_WIN32 || ACE_WIN64
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-d [STRING] : device [\"")
            << capture_device_identifier
            << ACE_TEXT_ALWAYS_CHAR ("\"]")
            << std::endl;
  std::string path = Common_File_Tools::getTempDirectory ();
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  path += ACE_TEXT_ALWAYS_CHAR (TEST_U_STREAM_CAMSAVE_DEFAULT_OUTPUT_FILE);
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-f[[STRING]]: target filename [")
            << path
            << ACE_TEXT_ALWAYS_CHAR ("]")
            << std::endl;
  path = configuration_path;
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  path += ACE_TEXT_ALWAYS_CHAR (TEST_U_STREAM_CONFIGURATION_DIRECTORY);
#if defined (GUI_SUPPORT)
  std::string UI_file = path;
  UI_file += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  UI_file += ACE_TEXT_ALWAYS_CHAR (TEST_U_STREAM_CAMSAVE_UI_DEFINITION_FILE);
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-g[[STRING]]: UI file [\"")
            << UI_file
            << ACE_TEXT_ALWAYS_CHAR ("\"] {\"\" --> no GUI}")
            << std::endl;
#endif // GUI_SUPPORT
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-l          : log to a file [")
            << false
            << ACE_TEXT_ALWAYS_CHAR ("]")
            << std::endl;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-m          : use MediaFoundation framework [")
            << (STREAM_LIB_DEFAULT_MEDIAFRAMEWORK == STREAM_MEDIAFRAMEWORK_MEDIAFOUNDATION)
            << ACE_TEXT_ALWAYS_CHAR ("]")
            << std::endl;
#endif // ACE_WIN32 || ACE_WIN64
  struct Common_UI_DisplayDevice display_device_s =
    Common_UI_Tools::getDefaultDisplayDevice ();
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-o [STRING] : display device [\"")
            << display_device_s.description
            << ACE_TEXT_ALWAYS_CHAR ("\"]")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-s [VALUE]  : statistic reporting interval (second(s)) [")
            << STREAM_DEFAULT_STATISTIC_REPORTING_INTERVAL
            << ACE_TEXT_ALWAYS_CHAR ("] [0: off])")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-t          : trace information [")
            << false
            << ACE_TEXT_ALWAYS_CHAR ("]")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-v          : print version information and exit [")
            << false
            << ACE_TEXT_ALWAYS_CHAR ("]")
            << std::endl;
  //std::cout << ACE_TEXT_ALWAYS_CHAR ("-y          : run stress-test [")
  //  << false
  //  << ACE_TEXT_ALWAYS_CHAR ("]")
  //  << std::endl;
}

bool
do_processArguments (int argc_in,
                     ACE_TCHAR** argv_in, // cannot be const...
                     std::string& captureDeviceIdentifier_out,
#if defined (ACE_WIN32) || defined (ACE_WIN64)
                     bool& showConsole_out,
#endif // ACE_WIN32 || ACE_WIN64
                     std::string& targetFileName_out,
#if defined (GUI_SUPPORT)
                     std::string& UIFile_out,
#endif // GUI_SUPPORT
                     bool& logToFile_out,
#if defined (ACE_WIN32) || defined (ACE_WIN64)
                     enum Stream_MediaFramework_Type& mediaFramework_out,
#endif // ACE_WIN32 || ACE_WIN64
                     struct Common_UI_DisplayDevice& displayDevice_out,
                     enum Stream_Visualization_VideoRenderer& renderer_out,
                     unsigned int& statisticReportingInterval_out,
                     bool& traceInformation_out,
                     bool& printVersionAndExit_out)
{
  STREAM_TRACE (ACE_TEXT ("::do_processArguments"));

  std::string configuration_path =
    Common_File_Tools::getWorkingDirectory ();

  // initialize results
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  switch (mediaFramework_out)
  {
    case STREAM_MEDIAFRAMEWORK_DIRECTSHOW:
    {
      captureDeviceIdentifier_out =
        Stream_Device_DirectShow_Tools::getDefaultCaptureDevice (CLSID_VideoInputDeviceCategory);
      break;
    }
    case STREAM_MEDIAFRAMEWORK_MEDIAFOUNDATION:
    {
      // *TODO*
      ACE_ASSERT (false);
      ACE_NOTSUP_RETURN (false);
      ACE_NOTREACHED (return false;)
      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown media framework (was: %d), returning\n"),
                  mediaFramework_out));
      return false;
    }
  } // end SWITCH
  showConsole_out = false;
#else
  captureDeviceIdentifier_out =
    ACE_TEXT_ALWAYS_CHAR (MODULE_DEV_DEVICE_DIRECTORY);
  captureDeviceIdentifier_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  captureDeviceIdentifier_out +=
    ACE_TEXT_ALWAYS_CHAR (MODULE_DEV_DEFAULT_VIDEO_DEVICE);
#endif // ACE_WIN32 || ACE_WIN64
  std::string path = Common_File_Tools::getTempDirectory ();
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  path += ACE_TEXT_ALWAYS_CHAR (TEST_U_STREAM_CAMSAVE_DEFAULT_OUTPUT_FILE);
  targetFileName_out = path;
  path = configuration_path;
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  path += ACE_TEXT_ALWAYS_CHAR (TEST_U_STREAM_CONFIGURATION_DIRECTORY);
#if defined (GUI_SUPPORT)
  UIFile_out = path;
  UIFile_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  UIFile_out += ACE_TEXT_ALWAYS_CHAR (TEST_U_STREAM_CAMSAVE_UI_DEFINITION_FILE);
#endif // GUI_SUPPORT
  logToFile_out = false;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  mediaFramework_out = STREAM_LIB_DEFAULT_MEDIAFRAMEWORK;
#endif // ACE_WIN32 || ACE_WIN64
  displayDevice_out = Common_UI_Tools::getDefaultDisplayDevice ();
  renderer_out = STREAM_VIS_RENDERER_VIDEO_DEFAULT;
  statisticReportingInterval_out = STREAM_DEFAULT_STATISTIC_REPORTING_INTERVAL;
  traceInformation_out = false;
  printVersionAndExit_out = false;

  ACE_Get_Opt argumentParser (argc_in,
                              argv_in,
#if defined (GUI_SUPPORT)
#if defined (ACE_WIN32) || defined (ACE_WIN64)
                              ACE_TEXT ("3cd:f::g::hlmo:s:tv"),
#else
                              ACE_TEXT ("d:f::g::hlo:s:tv"),
#endif // ACE_WIN32 || ACE_WIN64
#else
#if defined (ACE_WIN32) || defined (ACE_WIN64)
                              ACE_TEXT ("3cd:f::hlmo:s:tv"),
#else
                              ACE_TEXT ("d:f::hlo:s:tv"),
#endif // ACE_WIN32 || ACE_WIN64
#endif // GUI_SUPPORT
                              1,                          // skip command name
                              1,                          // report parsing errors
                              ACE_Get_Opt::PERMUTE_ARGS,  // ordering
                              0);                         // for now, don't use long options

  int option = 0;
  std::stringstream converter;
  while ((option = argumentParser ()) != EOF)
  {
    switch (option)
    {
#if defined (ACE_WIN32) || defined (ACE_WIN64)
      case '2':
      {
        renderer_out = STREAM_VISUALIZATION_VIDEORENDERER_DIRECTDRAW_2D;
        break;
      }
      case '3':
      {
        renderer_out = STREAM_VISUALIZATION_VIDEORENDERER_DIRECTDRAW_3D;
        break;
      }
      case 'c':
      {
        showConsole_out = true;
        break;
      }
#endif // ACE_WIN32 || ACE_WIN64
      case 'd':
      {
        captureDeviceIdentifier_out =
            ACE_TEXT_ALWAYS_CHAR (argumentParser.opt_arg ());
        break;
      }
      case 'f':
      {
        ACE_TCHAR* opt_arg = argumentParser.opt_arg ();
        if (opt_arg)
          targetFileName_out = ACE_TEXT_ALWAYS_CHAR (opt_arg);
        else
          targetFileName_out.clear ();
        break;
      }
#if defined (GUI_SUPPORT)
      case 'g':
      {
        ACE_TCHAR* opt_arg = argumentParser.opt_arg ();
        if (opt_arg)
          UIFile_out = ACE_TEXT_ALWAYS_CHAR (opt_arg);
        else
          UIFile_out.clear ();
        break;
      }
#endif // GUI_SUPPORT
      case 'l':
      {
        logToFile_out = true;
        break;
      }
#if defined (ACE_WIN32) || defined (ACE_WIN64)
      case 'm':
      {
        mediaFramework_out = STREAM_MEDIAFRAMEWORK_MEDIAFOUNDATION;
        break;
      }
#endif // ACE_WIN32 || ACE_WIN64
      case 'o':
      {
        displayDevice_out =
          Common_UI_Tools::getDisplayDevice (ACE_TEXT_ALWAYS_CHAR (argumentParser.opt_arg ()));
        break;
      }
      case 's':
      {
        converter.clear ();
        converter.str (ACE_TEXT_ALWAYS_CHAR (""));
        converter << argumentParser.opt_arg ();
        converter >> statisticReportingInterval_out;
        break;
      }
      case 't':
      {
        traceInformation_out = true;
        break;
      }
      case 'v':
      {
        printVersionAndExit_out = true;
        break;
      }
      //case 'y':
      //{
      //  runStressTest_out = true;
      //  break;
      //}
      // error handling
      case ':':
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("option \"%c\" requires an argument, aborting\n"),
                    argumentParser.opt_opt ()));
        return false;
      }
      case '?':
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("unrecognized option \"%s\", aborting\n"),
                    argumentParser.last_option ()));
        return false;
      }
      case 0:
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("found long option \"%s\", aborting\n"),
                    argumentParser.long_option ()));
        return false;
      }
      default:
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("parse error, aborting\n")));
        return false;
      }
    } // end SWITCH
  } // end WHILE

  return true;
}

void
do_initializeSignals (bool allowUserRuntimeConnect_in,
                      ACE_Sig_Set& signals_out,
                      ACE_Sig_Set& ignoredSignals_out)
{
  STREAM_TRACE (ACE_TEXT ("::do_initializeSignals"));

  int result = -1;

  // initialize return value(s)
  result = signals_out.empty_set ();
  if (result == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Sig_Set::empty_set(): \"%m\", returning\n")));
    return;
  } // end IF
  result = ignoredSignals_out.empty_set ();
  if (result == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Sig_Set::empty_set(): \"%m\", returning\n")));
    return;
  } // end IF

  // *PORTABILITY*: on Windows(TM) platforms most signals are not defined, and
  //                ACE_Sig_Set::fill_set() doesn't really work as specified
  // --> add valid signals (see <signal.h>)...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  signals_out.sig_add (SIGINT);            // 2       /* interrupt */
  signals_out.sig_add (SIGILL);            // 4       /* illegal instruction - invalid function image */
  signals_out.sig_add (SIGFPE);            // 8       /* floating point exception */
  //  signals_out.sig_add (SIGSEGV);           // 11      /* segment violation */
  signals_out.sig_add (SIGTERM);           // 15      /* Software termination signal from kill */
  if (allowUserRuntimeConnect_in)
  {
    signals_out.sig_add (SIGBREAK);        // 21      /* Ctrl-Break sequence */
    ignoredSignals_out.sig_add (SIGBREAK); // 21      /* Ctrl-Break sequence */
  } // end IF
  signals_out.sig_add (SIGABRT);           // 22      /* abnormal termination triggered by abort call */
  signals_out.sig_add (SIGABRT_COMPAT);    // 6       /* SIGABRT compatible with other platforms, same as SIGABRT */
#else
  result = signals_out.fill_set ();
  if (result == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Sig_Set::fill_set(): \"%m\", returning\n")));
    return;
  } // end IF
#if defined (DEBUG_DEBUGGER)
  signals_out.sig_del (SIGTRAP);           // 5       /* Trace trap (POSIX) */
#endif
  // *NOTE*: cannot handle some signals --> registration fails for these...
  signals_out.sig_del (SIGKILL);           // 9       /* Kill signal */
  signals_out.sig_del (SIGSTOP);           // 19      /* Stop process */
  // ---------------------------------------------------------------------------
  if (!allowUserRuntimeConnect_in)
  {
    signals_out.sig_del (SIGUSR1);         // 10      /* User-defined signal 1 */
    ignoredSignals_out.sig_add (SIGUSR1);  // 10      /* User-defined signal 1 */
  } // end IF
  // *NOTE* core dump on SIGSEGV
  signals_out.sig_del (SIGSEGV);           // 11      /* Segmentation fault: Invalid memory reference */
  // *NOTE* don't care about SIGPIPE
  signals_out.sig_del (SIGPIPE);           // 12      /* Broken pipe: write to pipe with no readers */

#ifdef ENABLE_VALGRIND_SUPPORT
  // *NOTE*: valgrind uses SIGRT32 (--> SIGRTMAX ?) and apparently will not work
  // if the application installs its own handler (see documentation)
  if (RUNNING_ON_VALGRIND)
    signals_out.sig_del (SIGRTMAX);        // 64
#endif
#endif
}

#if defined (ACE_WIN32) || defined (ACE_WIN64)
bool
do_initialize_directshow (const std::string& devicePath_in,
                          bool coInitialize_in,
                          bool hasUI_in,
                          IGraphBuilder*& IGraphBuilder_out,
                          IAMStreamConfig*& IAMStreamConfig_out,
                          struct _AMMediaType*& captureFormat_out,
                          struct _AMMediaType*& outputFormat_out) // directshow sample grabber-
{
  STREAM_TRACE (ACE_TEXT ("::do_initialize_directshow"));

  HRESULT result = E_FAIL;
  IAMBufferNegotiation* buffer_negotiation_p = NULL;
  Stream_MediaFramework_DirectShow_Graph_t graph_layout;
  Stream_MediaFramework_DirectShow_GraphConfiguration_t graph_configuration;
  BOOL result_2 = false;
  IMediaFilter* media_filter_p = NULL;

  // sanity check(s)
  ACE_ASSERT (!IGraphBuilder_out);
  ACE_ASSERT (!IAMStreamConfig_out);
  ACE_ASSERT (!outputFormat_out);

  Stream_MediaFramework_Tools::initialize (STREAM_MEDIAFRAMEWORK_DIRECTSHOW);
  Stream_Device_DirectShow_Tools::initialize (coInitialize_in);

  if (!Stream_Device_DirectShow_Tools::loadDeviceGraph (devicePath_in,
                                                        CLSID_VideoInputDeviceCategory,
                                                        IGraphBuilder_out,
                                                        buffer_negotiation_p,
                                                        IAMStreamConfig_out,
                                                        graph_layout))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Stream_Device_DirectShow_Tools::loadDeviceGraph(\"%s\"), aborting\n"),
                ACE_TEXT (devicePath_in.c_str ())));
    goto error;
  } // end IF
  ACE_ASSERT (IGraphBuilder_out);
  ACE_ASSERT (buffer_negotiation_p);
  ACE_ASSERT (IAMStreamConfig_out);
  buffer_negotiation_p->Release (); buffer_negotiation_p = NULL;

  if (!Stream_Device_DirectShow_Tools::getCaptureFormat (IGraphBuilder_out,
                                                                CLSID_VideoInputDeviceCategory,
                                                                captureFormat_out))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Stream_Device_DirectShow_Tools::getCaptureFormat(CLSID_VideoInputDeviceCategory), aborting\n")));
    goto error;
  } // end IF
  ACE_ASSERT (captureFormat_out);
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("\"%s\": default capture format: %s\n"),
              ACE_TEXT (Stream_Device_DirectShow_Tools::devicePathToString (devicePath_in).c_str ()),
              ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::toString (*captureFormat_out, true).c_str ())));

  outputFormat_out =
    Stream_MediaFramework_DirectShow_Tools::copy (*captureFormat_out);
  if (!outputFormat_out)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Stream_MediaFramework_DirectShow_Tools::copy(), aborting\n")));
    goto error;
  } // end IF
  ACE_ASSERT (outputFormat_out);

  // *NOTE*: the default save format is ARGB32
  ACE_ASSERT (InlineIsEqualGUID (outputFormat_out->majortype, MEDIATYPE_Video));
  outputFormat_out->subtype =
    STREAM_DEC_DIRECTSHOW_FILTER_VIDEO_RENDERER_DEFAULT_FORMAT;
  outputFormat_out->bFixedSizeSamples = TRUE;
  outputFormat_out->bTemporalCompression = FALSE;
  if (InlineIsEqualGUID (outputFormat_out->formattype, FORMAT_VideoInfo))
  { ACE_ASSERT (outputFormat_out->cbFormat == sizeof (struct tagVIDEOINFOHEADER));
    struct tagVIDEOINFOHEADER* video_info_header_p =
      reinterpret_cast<struct tagVIDEOINFOHEADER*> (outputFormat_out->pbFormat);
    // *NOTE*: empty --> use entire video
    result_2 = SetRectEmpty (&video_info_header_p->rcSource);
    ACE_ASSERT (SUCCEEDED (result_2));
    result_2 = SetRectEmpty (&video_info_header_p->rcTarget);
    // *NOTE*: empty --> fill entire buffer
    ACE_ASSERT (SUCCEEDED (result_2));
    ACE_ASSERT (video_info_header_p->dwBitErrorRate == 0);
    ACE_ASSERT (video_info_header_p->bmiHeader.biSize == sizeof (struct tagBITMAPINFOHEADER));
    ACE_ASSERT (video_info_header_p->bmiHeader.biPlanes == 1);
    video_info_header_p->bmiHeader.biBitCount = 32;
    video_info_header_p->bmiHeader.biCompression = BI_RGB;
    video_info_header_p->bmiHeader.biSizeImage =
      DIBSIZE (video_info_header_p->bmiHeader);
    ////video_info_header_p->bmiHeader.biXPelsPerMeter;
    ////video_info_header_p->bmiHeader.biYPelsPerMeter;
    ////video_info_header_p->bmiHeader.biClrUsed;
    ////video_info_header_p->bmiHeader.biClrImportant;
    ACE_ASSERT (video_info_header_p->AvgTimePerFrame);
    video_info_header_p->dwBitRate =
      (video_info_header_p->bmiHeader.biSizeImage * 8) *                         // bits / frame
      (NANOSECONDS / static_cast<DWORD> (video_info_header_p->AvgTimePerFrame)); // fps

    outputFormat_out->lSampleSize =
      video_info_header_p->bmiHeader.biSizeImage;
  } // end IF
  else if (InlineIsEqualGUID (outputFormat_out->formattype, FORMAT_VideoInfo2))
  {
    ACE_ASSERT (outputFormat_out->cbFormat == sizeof (struct tagVIDEOINFOHEADER2));
    struct tagVIDEOINFOHEADER2* video_info_header_p =
      reinterpret_cast<struct tagVIDEOINFOHEADER2*> (outputFormat_out->pbFormat);
    ACE_ASSERT (video_info_header_p->bmiHeader.biSize == sizeof (struct tagBITMAPINFOHEADER));
    ACE_ASSERT (video_info_header_p->bmiHeader.biPlanes == 1);
    video_info_header_p->bmiHeader.biBitCount = 32;
    video_info_header_p->bmiHeader.biCompression = BI_RGB;
    video_info_header_p->bmiHeader.biSizeImage =
      DIBSIZE (video_info_header_p->bmiHeader);
    ////video_info_header_p->bmiHeader.biXPelsPerMeter;
    ////video_info_header_p->bmiHeader.biYPelsPerMeter;
    ////video_info_header_p->bmiHeader.biClrUsed;
    ////video_info_header_p->bmiHeader.biClrImportant;
    ACE_ASSERT (video_info_header_p->AvgTimePerFrame);
    video_info_header_p->dwBitRate =
      (video_info_header_p->bmiHeader.biSizeImage * 8) *                         // bits / frame
      (NANOSECONDS / static_cast<DWORD> (video_info_header_p->AvgTimePerFrame)); // fps

    outputFormat_out->lSampleSize =
      video_info_header_p->bmiHeader.biSizeImage;
  } // end IF
  else
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("invalid/unknown media format type (was: \"%s\"), aborting\n"),
                ACE_TEXT (Stream_MediaFramework_Tools::mediaFormatTypeToString (outputFormat_out->formattype).c_str ())));
    goto error;
  } // end ELSE

  if (hasUI_in)
  {
    IAMStreamConfig_out->Release (); IAMStreamConfig_out = NULL;
    IGraphBuilder_out->Release (); IGraphBuilder_out = NULL;
    return true;
  } // end IF

  if (!Stream_Module_Decoder_Tools::loadVideoRendererGraph (CLSID_VideoInputDeviceCategory,
                                                            *captureFormat_out,
                                                            *outputFormat_out,
                                                            NULL,
                                                            IGraphBuilder_out,
                                                            graph_configuration))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Stream_Module_Decoder_Tools::loadVideoRendererGraph(), aborting\n")));
    goto error;
  } // end IF
  ACE_ASSERT (IGraphBuilder_out);
  result = IGraphBuilder_out->QueryInterface (IID_PPV_ARGS (&media_filter_p));
  if (FAILED (result))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to IGraphBuilder::QueryInterface(IID_IMediaFilter): \"%s\", aborting\n"),
                ACE_TEXT (Common_Error_Tools::errorToString (result).c_str ())));
    goto error;
  } // end IF
  ACE_ASSERT (media_filter_p);
  result = media_filter_p->SetSyncSource (NULL);
  if (FAILED (result))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to IMediaFilter::SetSyncSource(): \"%s\", aborting\n"),
                ACE_TEXT (Common_Error_Tools::errorToString (result).c_str ())));
    goto error;
  } // end IF
  media_filter_p->Release ();

  return true;

error:
  if (media_filter_p)
    media_filter_p->Release ();
  if (buffer_negotiation_p)
    buffer_negotiation_p->Release ();
  if (outputFormat_out)
    Stream_MediaFramework_DirectShow_Tools::delete_ (outputFormat_out);
  if (captureFormat_out)
    Stream_MediaFramework_DirectShow_Tools::delete_ (captureFormat_out);
  if (IAMStreamConfig_out)
  {
    IAMStreamConfig_out->Release (); IAMStreamConfig_out = NULL;
  } // end IF
  if (IGraphBuilder_out)
  {
    IGraphBuilder_out->Release (); IGraphBuilder_out = NULL;
  } // end IF

  if (coInitialize_in)
    CoUninitialize ();

  return false;
}

void
do_finalize_directshow (IAMStreamConfig*& streamConfiguration_inout)
{
  STREAM_TRACE (ACE_TEXT ("::do_finalize_directshow"));

  if (streamConfiguration_inout)
  {
    streamConfiguration_inout->Release (); streamConfiguration_inout = NULL;
  } // end IF

  Stream_Device_DirectShow_Tools::finalize (true);
}

bool
do_initialize_mediafoundation (const std::string& captureDeviceIdentifier_in,
                               HWND windowHandle_in,
#if COMMON_OS_WIN32_TARGET_PLATFORM(0x0600) // _WIN32_WINNT_VISTA
                               IMFMediaSession*& IMFMediaSession_out,
#endif // COMMON_OS_WIN32_TARGET_PLATFORM(0x0600)
                               bool loadDevice_in,
                               bool coInitialize_in)
{
  STREAM_TRACE (ACE_TEXT ("::do_initialize_mediafoundation"));

  HRESULT result = E_FAIL;
#if COMMON_OS_WIN32_TARGET_PLATFORM(0x0602) // _WIN32_WINNT_WIN8
  IMFMediaSourceEx* media_source_p = NULL;
#else
  IMFMediaSource* media_source_p = NULL;
#endif // COMMON_OS_WIN32_TARGET_PLATFORM(0x0602)
  IMFTopology* topology_p = NULL;

  // sanity check(s)
#if COMMON_OS_WIN32_TARGET_PLATFORM(0x0600) // _WIN32_WINNT_VISTA
  ACE_ASSERT (!IMFMediaSession_out);
#endif // COMMON_OS_WIN32_TARGET_PLATFORM(0x0600)

  if (!coInitialize_in)
    goto continue_;

  result = CoInitializeEx (NULL,
                           (COINIT_MULTITHREADED    |
                            COINIT_DISABLE_OLE1DDE  |
                            COINIT_SPEED_OVER_MEMORY));
  if (FAILED (result))
  {
    // *NOTE*: most probable reason: already initialized (happens in the debugger)
    //         --> continue
    ACE_DEBUG ((LM_WARNING,
                ACE_TEXT ("failed to CoInitializeEx(): \"%s\", continuing\n"),
                ACE_TEXT (Common_Error_Tools::errorToString (result).c_str ())));
  } // end IF

continue_:
  result = MFStartup (MF_VERSION,
                      MFSTARTUP_LITE);
  if (FAILED (result))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to MFStartup(): \"%s\", continuing\n"),
                ACE_TEXT (Common_Error_Tools::errorToString (result).c_str ())));
    goto error;
  } // end IF

  Stream_MediaFramework_Tools::initialize (STREAM_MEDIAFRAMEWORK_MEDIAFOUNDATION);
  Stream_Device_Tools::initialize (true); // initialize media frameworks ?

  if (!loadDevice_in)
    goto continue_2;

  //if (!Stream_Device_Tools::loadDeviceGraph (deviceName_in,
  //                                                  IGraphBuilder_out,
  //                                                  IAMBufferNegotiation_out,
  //                                                  IAMStreamConfig_out))
  //{
  //  ACE_DEBUG ((LM_ERROR,
  //              ACE_TEXT ("failed to Stream_Device_Tools::loadDeviceGraph(\"%s\"), aborting\n"),
  //              ACE_TEXT (deviceName_in.c_str ())));
  //  return false;
  //} // end IF
  //ACE_ASSERT (IGraphBuilder_out);
  //ACE_ASSERT (IAMBufferNegotiation_out);
  //ACE_ASSERT (IAMStreamConfig_out);

#if COMMON_OS_WIN32_TARGET_PLATFORM(0x0601) // _WIN32_WINNT_WIN7
  if (!Stream_Device_MediaFoundation_Tools::getMediaSource (captureDeviceIdentifier_in,
                                                                   MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_GUID,
                                                                   media_source_p))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Stream_Device_MediaFoundation_Tools::getMediaSource(\"%s\"), aborting\n"),
                ACE_TEXT (captureDeviceIdentifier_in.c_str ())));
    goto error;
  } // end IF
  ACE_ASSERT (media_source_p);

  if (!Stream_Device_MediaFoundation_Tools::loadDeviceTopology (captureDeviceIdentifier_in,
                                                                       MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_GUID,
                                                                       media_source_p,
                                                                       NULL,
                                                                       topology_p))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Stream_Device_MediaFoundation_Tools::loadDeviceTopology(), aborting\n")));
    goto error;
  } // end IF
  ACE_ASSERT (topology_p);
  media_source_p->Release (); media_source_p = NULL;
#endif // COMMON_OS_WIN32_TARGET_PLATFORM(0x0601)

continue_2:
#if COMMON_OS_WIN32_TARGET_PLATFORM(0x0600) // _WIN32_WINNT_VISTA
  IMFAttributes* attributes_p = NULL;
  result = MFCreateAttributes (&attributes_p, 4);
  if (FAILED (result))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to MFCreateAttributes(): \"%s\", aborting\n"),
                ACE_TEXT (Common_Error_Tools::errorToString (result).c_str ())));
    goto error;
  } // end IF
  result = attributes_p->SetUINT32 (MF_SESSION_GLOBAL_TIME, FALSE);
  ACE_ASSERT (SUCCEEDED (result));
  result = attributes_p->SetGUID (MF_SESSION_QUALITY_MANAGER, GUID_NULL);
  ACE_ASSERT (SUCCEEDED (result));
  //result = attributes_p->SetGUID (MF_SESSION_TOPOLOADER, );
  //ACE_ASSERT (SUCCEEDED (result));
#if COMMON_OS_WIN32_TARGET_PLATFORM(0x0602) // _WIN32_WINNT_WIN8
  result = attributes_p->SetUINT32 (MF_LOW_LATENCY, TRUE);
  ACE_ASSERT (SUCCEEDED (result));
#endif // COMMON_OS_WIN32_TARGET_PLATFORM(0x0602)
  result = MFCreateMediaSession (attributes_p,
                                 &IMFMediaSession_out);
  if (FAILED (result))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to MFCreateMediaSession(): \"%s\", aborting\n"),
                ACE_TEXT (Common_Error_Tools::errorToString (result).c_str ())));
    attributes_p->Release (); attributes_p = NULL;
    goto error;
  } // end IF
  attributes_p->Release (); attributes_p = NULL;

  ACE_ASSERT (topology_p);
  DWORD topology_flags = (MFSESSION_SETTOPOLOGY_IMMEDIATE);// |
                          //MFSESSION_SETTOPOLOGY_NORESOLUTION);// |
                          //MFSESSION_SETTOPOLOGY_CLEAR_CURRENT);
  result = IMFMediaSession_out->SetTopology (topology_flags,
                                             topology_p);
  if (FAILED (result))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to IMFMediaSession::SetTopology(): \"%s\", aborting\n"),
                ACE_TEXT (Common_Error_Tools::errorToString (result).c_str ())));
    goto error;
  } // end IF
#endif // COMMON_OS_WIN32_TARGET_PLATFORM(0x0600)
  topology_p->Release (); topology_p = NULL;

//continue_3:
  return true;

error:
  if (media_source_p)
    media_source_p->Release ();
  if (topology_p)
    topology_p->Release ();
#if COMMON_OS_WIN32_TARGET_PLATFORM(0x0600) // _WIN32_WINNT_VISTA
  if (IMFMediaSession_out)
  {
    IMFMediaSession_out->Release (); IMFMediaSession_out = NULL;
  } // end IF
#endif // COMMON_OS_WIN32_TARGET_PLATFORM(0x0600)

  result = MFShutdown ();
  if (FAILED (result))
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to MFShutdown(): \"%s\", continuing\n"),
                ACE_TEXT (Common_Error_Tools::errorToString (result).c_str ())));

  if (coInitialize_in)
    CoUninitialize ();

  return false;
}

void
do_finalize_mediafoundation (IMFMediaSession*& mediaSession_inout)
{
  STREAM_TRACE (ACE_TEXT ("::do_finalize_mediafoundation"));

  HRESULT result = E_FAIL;

  if (mediaSession_inout)
  {
    result = mediaSession_inout->Shutdown ();
    if (FAILED (result))
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to IMFMediaSession::Shutdown(): \"%s\", continuing\n"),
                  ACE_TEXT (Common_Error_Tools::errorToString (result).c_str ())));
    mediaSession_inout->Release (); mediaSession_inout = NULL;
  } // end IF

  result = MFShutdown ();
  if (FAILED (result))
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to MFShutdown(): \"%s\", continuing\n"),
                ACE_TEXT (Common_Error_Tools::errorToString (result).c_str ())));

  CoUninitialize ();
}
#endif // ACE_WIN32 || ACE_WIN64

void
do_work (const std::string& captureDeviceIdentifier_in,
#if defined (ACE_WIN32) || defined (ACE_WIN64)
         bool showConsole_in,
#endif // ACE_WIN32 || ACE_WIN64
         const std::string& targetFilename_in,
#if defined (ACE_WIN32) || defined (ACE_WIN64)
         enum Stream_MediaFramework_Type mediaFramework_in,
#endif // ACE_WIN32 || ACE_WIN64
         const struct Common_UI_DisplayDevice& displayDevice_in,
         unsigned int statisticReportingInterval_in,
#if defined (ACE_WIN32) || defined (ACE_WIN64)
         struct Stream_CamSave_DirectShow_Configuration& directShowConfiguration_in,
         struct Stream_CamSave_MediaFoundation_Configuration& mediaFoundationConfiguration_in,
#else
         struct Stream_CamSave_Configuration& configuration_in,
#endif // ACE_WIN32 || ACE_WIN64
#if defined (GUI_SUPPORT)
         const std::string& UIDefinitionFilename_in,
#if defined (ACE_WIN32) || defined (ACE_WIN64)
         struct Stream_CamSave_DirectShow_UI_CBData& directShowCBData_in,
         struct Stream_CamSave_MediaFoundation_UI_CBData& mediaFoundationCBData_in,
#else
         struct Stream_CamSave_V4L_UI_CBData& CBData_in,
#endif // ACE_WIN32 || ACE_WIN64
#if defined (WXWIDGETS_USE)
         Common_UI_wxWidgets_IApplicationBase_t* iapplication_in,
#endif // WXWIDGETS_USE
#endif // GUI_SUPPORT
         enum Stream_Visualization_VideoRenderer renderer_in,
         const ACE_Sig_Set& signalSet_in,
         const ACE_Sig_Set& ignoredSignalSet_in,
         Common_SignalActions_t& previousSignalActions_inout,
         Stream_CamSave_SignalHandler& signalHandler_in)
{
  STREAM_TRACE (ACE_TEXT ("::do_work"));

  // ********************** module configuration data **************************
  struct Stream_ModuleConfiguration module_configuration;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  struct Stream_CamSave_DirectShow_ModuleHandlerConfiguration directshow_modulehandler_configuration;
#if defined (GUI_SUPPORT)
  Stream_CamSave_DirectShow_EventHandler_t directshow_ui_event_handler (&directShowCBData_in
#if defined (GTK_USE)
                                                                       );
#elif defined (WXWIDGETS_USE)
                                                                        ,iapplication_in);
#endif
#else
  Stream_CamSave_DirectShow_EventHandler_t directshow_ui_event_handler;
#endif // GUI_SUPPORT
  struct Stream_CamSave_MediaFoundation_ModuleHandlerConfiguration mediafoundation_modulehandler_configuration;
#if defined (GUI_SUPPORT)
  Stream_CamSave_MediaFoundation_EventHandler_t mediafoundation_ui_event_handler (&mediaFoundationCBData_in
#if defined (GTK_USE)
                                                                                 );
#elif defined (WXWIDGETS_USE)
                                                                                  ,iapplication_in);
#endif
#else
  Stream_CamSave_MediaFoundation_EventHandler_t mediafoundation_ui_event_handler;
#endif // GUI_SUPPORT
#else
  struct Stream_CamSave_V4L_ModuleHandlerConfiguration modulehandler_configuration;
#if defined (GUI_SUPPORT)
  Stream_CamSave_EventHandler_t ui_event_handler (&CBData_in
#if defined (GTK_USE)
                                                 );
#elif defined (WXWIDGETS_USE)
                                                  ,iapplication_in);
#endif
#else
  Stream_CamSave_EventHandler_t ui_event_handler;
#endif // GUI_SUPPORT
#endif // ACE_WIN32 || ACE_WIN64

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  Stream_CamSave_DirectShow_StreamConfiguration_t::ITERATOR_T directshow_stream_iterator;
  Stream_CamSave_MediaFoundation_StreamConfiguration_t::ITERATOR_T mediafoundation_stream_iterator;
  switch (mediaFramework_in)
  {
    case STREAM_MEDIAFRAMEWORK_DIRECTSHOW:
    {
      directshow_modulehandler_configuration.allocatorConfiguration =
        &directShowConfiguration_in.streamConfiguration.allocatorConfiguration_;
      directshow_modulehandler_configuration.deviceIdentifier =
        captureDeviceIdentifier_in;
      directshow_modulehandler_configuration.direct3DConfiguration =
        &directShowConfiguration_in.direct3DConfiguration;
      //directshow_modulehandler_configuration.pixelBufferLock =
      //  directShowCBData_in.pixelBufferLock;

      if (statisticReportingInterval_in)
      {
        directshow_modulehandler_configuration.statisticCollectionInterval.set (0,
                                                                                MODULE_DEV_CAM_STATISTIC_COLLECTION_INTERVAL * 1000);
        directshow_modulehandler_configuration.statisticReportingInterval =
          statisticReportingInterval_in;
      } // end IF
      directshow_modulehandler_configuration.subscriber =
        &directshow_ui_event_handler;
      directshow_modulehandler_configuration.targetFileName = targetFilename_in;
      break;
    }
    case STREAM_MEDIAFRAMEWORK_MEDIAFOUNDATION:
    {
      mediafoundation_modulehandler_configuration.allocatorConfiguration =
        &mediaFoundationConfiguration_in.streamConfiguration.allocatorConfiguration_;
      mediafoundation_modulehandler_configuration.deviceIdentifier =
        captureDeviceIdentifier_in;
      mediafoundation_modulehandler_configuration.direct3DConfiguration =
        &mediaFoundationConfiguration_in.direct3DConfiguration;
      //mediafoundation_modulehandler_configuration.pixelBufferLock =
        //mediaFoundationCBData_in.pixelBufferLock;

      if (statisticReportingInterval_in)
      {
        mediafoundation_modulehandler_configuration.statisticCollectionInterval.set (0,
                                                                                     MODULE_DEV_CAM_STATISTIC_COLLECTION_INTERVAL * 1000);
        mediafoundation_modulehandler_configuration.statisticReportingInterval =
          statisticReportingInterval_in;
      } // end IF
      mediafoundation_modulehandler_configuration.subscriber =
        &mediafoundation_ui_event_handler;
      mediafoundation_modulehandler_configuration.targetFileName =
        targetFilename_in;
      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown media framework (was: %d), returning\n"),
                  mediaFramework_in));
      return;
    }
  } // end SWITCH
#else
  Stream_CamSave_V4L_StreamConfiguration_t::ITERATOR_T v4l_stream_iterator;
  modulehandler_configuration.allocatorConfiguration =
    &configuration_in.streamConfiguration.allocatorConfiguration_;
  modulehandler_configuration.deviceIdentifier = captureDeviceIdentifier_in;
  modulehandler_configuration.pixelBufferLock =
    configuration_in.pixelBufferLock;
  // *TODO*: turn these into an option
  modulehandler_configuration.buffers =
    MODULE_DEV_CAM_V4L_DEFAULT_DEVICE_BUFFERS;
  modulehandler_configuration.v4l2Method = V4L2_MEMORY_MMAP;

  if (statisticReportingInterval_in)
  {
    modulehandler_configuration.statisticCollectionInterval.set (0,
                                                                 MODULE_DEV_CAM_STATISTIC_COLLECTION_INTERVAL * 1000);
    modulehandler_configuration.statisticReportingInterval =
      statisticReportingInterval_in;
  } // end IF
  modulehandler_configuration.subscriber = &ui_event_handler;
  modulehandler_configuration.targetFileName = targetFilename_in;
#endif // ACE_WIN32 || ACE_WIN64

  Stream_AllocatorHeap_T<ACE_MT_SYNCH,
                         struct Stream_AllocatorConfiguration> heap_allocator;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  Stream_CamSave_DirectShow_MessageAllocator_t directshow_message_allocator (TEST_U_MAX_MESSAGES, // maximum #buffers
                                                                             &heap_allocator,     // heap allocator handle
                                                                             true);               // block ?
  Stream_CamSave_DirectShow_Stream directshow_stream;
  Stream_CamSave_DirectShow_MessageHandler_Module directshow_message_handler (&directshow_stream,
                                                                              ACE_TEXT_ALWAYS_CHAR (MODULE_MISC_MESSAGEHANDLER_DEFAULT_NAME_STRING));

  Stream_CamSave_MediaFoundation_MessageAllocator_t mediafoundation_message_allocator (TEST_U_MAX_MESSAGES, // maximum #buffers
                                                                                       &heap_allocator,     // heap allocator handle
                                                                                       true);               // block ?
  Stream_CamSave_MediaFoundation_Stream mediafoundation_stream;
  Stream_CamSave_MediaFoundation_MessageHandler_Module mediafoundation_message_handler (&mediafoundation_stream,
                                                                                        ACE_TEXT_ALWAYS_CHAR (MODULE_MISC_MESSAGEHANDLER_DEFAULT_NAME_STRING));
  switch (mediaFramework_in)
  {
    case STREAM_MEDIAFRAMEWORK_DIRECTSHOW:
    {
      if (!heap_allocator.initialize (directShowConfiguration_in.streamConfiguration.allocatorConfiguration_))
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to initialize heap allocator, returning\n")));
        return;
      } // end IF

      //if (bufferSize_in)
      //  directShowCBData_in.configuration->streamConfiguration.allocatorConfiguration_.defaultBufferSize =
      //      bufferSize_in;
      directShowConfiguration_in.streamConfiguration.configuration_.messageAllocator =
          &directshow_message_allocator;
#if defined (GUI_SUPPORT)
      directShowConfiguration_in.streamConfiguration.configuration_.module =
          (!UIDefinitionFilename_in.empty () ? &directshow_message_handler
                                             : NULL);
#endif // GUI_SUPPORT
      directShowConfiguration_in.streamConfiguration.configuration_.renderer =
        renderer_in;

      directShowConfiguration_in.streamConfiguration.initialize (module_configuration,
                                                                 directshow_modulehandler_configuration,
                                                                 directShowConfiguration_in.streamConfiguration.allocatorConfiguration_,
                                                                 directShowConfiguration_in.streamConfiguration.configuration_);
      directshow_modulehandler_configuration.deviceIdentifier =
        displayDevice_in.device;
      directShowConfiguration_in.streamConfiguration.insert (std::make_pair (ACE_TEXT_ALWAYS_CHAR (Stream_Visualization_Tools::rendererToModuleName (renderer_in).c_str ()),
                                                                             std::make_pair (module_configuration,
                                                                                             directshow_modulehandler_configuration)));
      directshow_stream_iterator =
        directShowConfiguration_in.streamConfiguration.find (ACE_TEXT_ALWAYS_CHAR (""));
      ACE_ASSERT (directshow_stream_iterator != directShowConfiguration_in.streamConfiguration.end ());
      break;
    }
    case STREAM_MEDIAFRAMEWORK_MEDIAFOUNDATION:
    {
      if (!heap_allocator.initialize (mediaFoundationConfiguration_in.streamConfiguration.allocatorConfiguration_))
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to initialize heap allocator, returning\n")));
        return;
      } // end IF

      //if (bufferSize_in)
      //  mediaFoundationCBData_in.configuration->streamConfiguration.allocatorConfiguration_.defaultBufferSize =
      //      bufferSize_in;
      mediaFoundationConfiguration_in.streamConfiguration.configuration_.messageAllocator =
          &mediafoundation_message_allocator;
#if defined (GUI_SUPPORT)
      mediaFoundationConfiguration_in.streamConfiguration.configuration_.module =
          (!UIDefinitionFilename_in.empty () ? &mediafoundation_message_handler
                                             : NULL);
#endif // GUI_SUPPORT
      mediaFoundationConfiguration_in.streamConfiguration.configuration_.renderer =
        renderer_in;

      mediaFoundationConfiguration_in.streamConfiguration.initialize (module_configuration,
                                                                      mediafoundation_modulehandler_configuration,
                                                                      mediaFoundationConfiguration_in.streamConfiguration.allocatorConfiguration_,
                                                                      mediaFoundationConfiguration_in.streamConfiguration.configuration_);
      mediafoundation_stream_iterator =
        mediaFoundationConfiguration_in.streamConfiguration.find (ACE_TEXT_ALWAYS_CHAR (""));
      ACE_ASSERT (mediafoundation_stream_iterator != mediaFoundationConfiguration_in.streamConfiguration.end ());
      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown media framework (was: %d), returning\n"),
                  mediaFramework_in));
      return;
    }
  } // end SWITCH
#else
  Stream_CamSave_MessageAllocator_t message_allocator (TEST_U_MAX_MESSAGES, // maximum #buffers
                                                       &heap_allocator,     // heap allocator handle
                                                       true);               // block ?
  Stream_CamSave_Stream stream;
  Stream_CamSave_MessageHandler_Module message_handler (&stream,
                                                        ACE_TEXT_ALWAYS_CHAR (MODULE_MISC_MESSAGEHANDLER_DEFAULT_NAME_STRING));

  //if (bufferSize_in)
  //  CBData_in.configuration->streamConfiguration.allocatorConfiguration_.defaultBufferSize =
  //      bufferSize_in;
  configuration_in.streamConfiguration.configuration_.messageAllocator =
      &message_allocator;
#if defined (GUI_SUPPORT)
  configuration_in.streamConfiguration.configuration_.module =
      (!UIDefinitionFilename_in.empty () ? &message_handler
                                          : NULL);
#endif // GUI_SUPPORT
   configuration_in.streamConfiguration.configuration_.renderer =
     renderer_in;

  configuration_in.streamConfiguration.initialize (module_configuration,
                                                   modulehandler_configuration,
                                                   configuration_in.streamConfiguration.allocatorConfiguration_,
                                                   configuration_in.streamConfiguration.configuration_);
  v4l_stream_iterator =
    configuration_in.streamConfiguration.find (ACE_TEXT_ALWAYS_CHAR (""));
  ACE_ASSERT (v4l_stream_iterator != configuration_in.streamConfiguration.end ());

  if (!heap_allocator.initialize (configuration_in.streamConfiguration.allocatorConfiguration_))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to initialize heap allocator, returning\n")));
    return;
  } // end IF
#endif // ACE_WIN32 || ACE_WIN64

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  HWND window_handle = NULL;
  //if ((*iterator).second.second.window)
  //{
  //  ACE_ASSERT (gdk_win32_window_is_win32 ((*iterator).second.second.window));
  //  window_handle =
  //    //gdk_win32_window_get_impl_hwnd (configuration.moduleHandlerConfiguration.window);
  //    //gdk_win32_drawable_get_handle (GDK_DRAWABLE (configuration.moduleHandlerConfiguration.window));
  //    static_cast<HWND> (GDK_WINDOW_HWND ((*iterator).second.second.window));
  //} // end IF
  //IAMBufferNegotiation* buffer_negotiation_p = NULL;
  IAMStreamConfig* stream_config_p = NULL;
  IMFMediaSession* media_session_p = NULL;
  bool load_device = UIDefinitionFilename_in.empty ();
  bool initialize_COM = UIDefinitionFilename_in.empty ();
  switch (mediaFramework_in)
  {
    case STREAM_MEDIAFRAMEWORK_DIRECTSHOW:
    {
      if (!do_initialize_directshow (captureDeviceIdentifier_in,
                                     UIDefinitionFilename_in.empty (),  // initialize COM ?
                                     !UIDefinitionFilename_in.empty (), // has UI ?
                                     (*directshow_stream_iterator).second.second.builder,
                                     stream_config_p,
                                     (*directshow_stream_iterator).second.second.sourceFormat,
                                     (*directshow_stream_iterator).second.second.inputFormat))
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ::do_initialize_directshow(), returning\n")));
        return;
      } // end IF
      //ACE_ASSERT (stream_config_p);
      ACE_ASSERT ((*directshow_stream_iterator).second.second.inputFormat);
#if defined (GUI_SUPPORT)
#if defined (GTK_USE)
      directShowCBData_in.streamConfiguration = stream_config_p;
#elif defined (WXWIDGETS_USE)

#endif
#endif // GUI_SUPPORT
      break;
    }
    case STREAM_MEDIAFRAMEWORK_MEDIAFOUNDATION:
    {
      if (!do_initialize_mediafoundation (captureDeviceIdentifier_in,
                                          window_handle,
#if COMMON_OS_WIN32_TARGET_PLATFORM(0x0600) // _WIN32_WINNT_VISTA
                                          (*mediafoundation_stream_iterator).second.second.session,
#endif // COMMON_OS_WIN32_TARGET_PLATFORM(0x0600)
                                          load_device,     // load device ?
                                          initialize_COM)) // initialize COM ?
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ::do_initialize_mediafoundation(), returning\n")));
        return;
      } // end IF
#if COMMON_OS_WIN32_TARGET_PLATFORM(0x0600) // _WIN32_WINNT_VISTA
      ACE_ASSERT ((*mediafoundation_stream_iterator).second.second.session);
#endif // COMMON_OS_WIN32_TARGET_PLATFORM(0x0600)
      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown media framework (was: %d), returning\n"),
                  mediaFramework_in));
      return;
    }
  } // end SWITCH
#endif // ACE_WIN32 || ACE_WIN64

  switch (renderer_in)
  {
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    case STREAM_VISUALIZATION_VIDEORENDERER_DIRECTDRAW_2D:
      break;
    case STREAM_VISUALIZATION_VIDEORENDERER_DIRECTDRAW_3D:
    { ACE_ASSERT ((*directshow_stream_iterator).second.second.sourceFormat);
      Common_UI_Resolution_t resolution_s = 
        Stream_MediaFramework_DirectShow_Tools::toResolution (*(*directshow_stream_iterator).second.second.sourceFormat);
      struct _D3DDISPLAYMODE display_mode_s =
        Stream_MediaFramework_DirectDraw_Tools::getDisplayMode (directShowConfiguration_in.direct3DConfiguration.adapter,
                                                                STREAM_LIB_DIRECTDRAW_3D_DEFAULT_FORMAT,
                                                                resolution_s);
      ACE_ASSERT (!directShowConfiguration_in.direct3DConfiguration.presentationParameters.hDeviceWindow);
      directShowConfiguration_in.direct3DConfiguration.focusWindow =
        GetConsoleWindow ();
      IDirect3DDeviceManager9* direct3D_manager_p = NULL;
      UINT reset_token = 0;
      if (!Stream_MediaFramework_DirectDraw_Tools::getDevice (directShowConfiguration_in.direct3DConfiguration,
                                                              direct3D_manager_p,
                                                              reset_token))
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to Stream_MediaFramework_DirectDraw_Tools::getDevice(), returning\n")));
        return;
      } // end IF
      ACE_ASSERT (directShowConfiguration_in.direct3DConfiguration.handle);
      ACE_ASSERT (direct3D_manager_p);
      ACE_ASSERT (reset_token);
      direct3D_manager_p->Release (); direct3D_manager_p = NULL;
      reset_token = 0;
      break;
    }
    case STREAM_VISUALIZATION_VIDEORENDERER_DIRECTSHOW:
      break;
    case STREAM_VISUALIZATION_VIDEORENDERER_GDI:
      break;
    case STREAM_VISUALIZATION_VIDEORENDERER_MEDIAFOUNDATION:
      break;
    case STREAM_VISUALIZATION_VIDEORENDERER_NULL:
#else
    case STREAM_VISUALIZATION_VIDEORENDERER_NULL:
#endif // ACE_WIN32 || ACE_WIN64
      break;
    case STREAM_VISUALIZATION_VIDEORENDERER_GTK_CAIRO:
      break;
    case STREAM_VISUALIZATION_VIDEORENDERER_GTK_PIXBUF:
      break;
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown video renderer (was: %d), returning\n"),
                  renderer_in));
      return;
    }
  } // end SWITCH

  struct Common_TimerConfiguration timer_configuration;
  Common_Timer_Manager_t* timer_manager_p = NULL;
#if defined (GTK_USE)
  Common_UI_GTK_Manager_t* gtk_manager_p = NULL;
#endif // GTK_USE
  bool result = false;

  // step0e: initialize signal handling
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  switch (mediaFramework_in)
  {
    case STREAM_MEDIAFRAMEWORK_DIRECTSHOW:
    {
#if defined (GUI_SUPPORT)
      directShowConfiguration_in.signalHandlerConfiguration.hasUI =
        !UIDefinitionFilename_in.empty ();
#endif // GUI_SUPPORT
      directShowConfiguration_in.signalHandlerConfiguration.messageAllocator =
        &directshow_message_allocator;
      signalHandler_in.initialize (directShowConfiguration_in.signalHandlerConfiguration);
      break;
    }
    case STREAM_MEDIAFRAMEWORK_MEDIAFOUNDATION:
    {
#if defined (GUI_SUPPORT)
      mediaFoundationConfiguration_in.signalHandlerConfiguration.hasUI =
        !UIDefinitionFilename_in.empty ();
#endif // GUI_SUPPORT
      mediaFoundationConfiguration_in.signalHandlerConfiguration.messageAllocator =
        &mediafoundation_message_allocator;
      signalHandler_in.initialize (mediaFoundationConfiguration_in.signalHandlerConfiguration);
      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown media framework (was: %d), returning\n"),
                  mediaFramework_in));
      return;
    }
  } // end SWITCH
#else
#if defined (GUI_SUPPORT)
  configuration_in.signalHandlerConfiguration.hasUI =
    !UIDefinitionFilename_in.empty ();
#endif // GUI_SUPPORT
  configuration_in.signalHandlerConfiguration.messageAllocator =
    &message_allocator;
  signalHandler_in.initialize (configuration_in.signalHandlerConfiguration);
#endif // ACE_WIN32 || ACE_WIN64
  if (!Common_Signal_Tools::initialize (COMMON_SIGNAL_DISPATCH_SIGNAL,
                                        signalSet_in,
                                        ignoredSignalSet_in,
                                        &signalHandler_in,
                                        previousSignalActions_inout))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_Signal_Tools::initialize(), returning\n")));
    return;
  } // end IF

  // intialize timers
  timer_manager_p = COMMON_TIMERMANAGER_SINGLETON::instance ();
  ACE_ASSERT (timer_manager_p);
  timer_manager_p->initialize (timer_configuration);
  timer_manager_p->start ();

  // step0f: (initialize) processing stream

  // event loop(s):
  // - catch SIGINT/SIGQUIT/SIGTERM/... signals (connect / perform orderly shutdown)
  // [- signal timer expiration to perform server queries] (see above)

  // step1a: start UI event loop ?
#if defined (GUI_SUPPORT)
  if (!UIDefinitionFilename_in.empty ())
  {
#if defined (GTK_USE)
    gtk_manager_p = COMMON_UI_GTK_MANAGER_SINGLETON::instance ();
    ACE_ASSERT (gtk_manager_p);
#endif // GTK_USE
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    switch (mediaFramework_in)
    {
      case STREAM_MEDIAFRAMEWORK_DIRECTSHOW:
      {
        directShowCBData_in.stream = &directshow_stream;
#if defined (GTK_USE)
        directShowCBData_in.UIState.eventHooks.finiHook = idle_finalize_UI_cb;
        directShowCBData_in.UIState.eventHooks.initHook = idle_initialize_UI_cb;
        //directShowCBData_in.UIState.gladeXML[ACE_TEXT_ALWAYS_CHAR (COMMON_UI_DEFINITION_DESCRIPTOR_MAIN)] =
        //  std::make_pair (UIDefinitionFile_in, static_cast<GladeXML*> (NULL));
        directShowCBData_in.UIState.builders[ACE_TEXT_ALWAYS_CHAR (COMMON_UI_DEFINITION_DESCRIPTOR_MAIN)] =
          std::make_pair (UIDefinitionFilename_in, static_cast<GtkBuilder*> (NULL));
#elif defined (WXWIDGETS_USE)
        struct Common_UI_wxWidgets_State& state_r =
          const_cast<struct Common_UI_wxWidgets_State&> (iapplication_in->getR ());
        state_r.resources[ACE_TEXT_ALWAYS_CHAR (COMMON_UI_DEFINITION_DESCRIPTOR_MAIN)] =
          std::make_pair (UIDefinitionFilename_in, static_cast<wxObject*> (NULL));
#endif
        break;
      }
      case STREAM_MEDIAFRAMEWORK_MEDIAFOUNDATION:
      {
        mediaFoundationCBData_in.stream = &mediafoundation_stream;
#if defined (GTK_USE)
        mediaFoundationCBData_in.UIState.eventHooks.finiHook =
          idle_finalize_UI_cb;
        mediaFoundationCBData_in.UIState.eventHooks.initHook =
          idle_initialize_UI_cb;
        //mediaFoundationCBData_in.UIState.gladeXML[ACE_TEXT_ALWAYS_CHAR (COMMON_UI_DEFINITION_DESCRIPTOR_MAIN)] =
        //  std::make_pair (UIDefinitionFile_in, static_cast<GladeXML*> (NULL));
        mediaFoundationCBData_in.UIState.builders[ACE_TEXT_ALWAYS_CHAR (COMMON_UI_DEFINITION_DESCRIPTOR_MAIN)] =
          std::make_pair (UIDefinitionFilename_in, static_cast<GtkBuilder*> (NULL));
#elif defined (WXWIDGETS_USE)
        struct Common_UI_wxWidgets_State& state_r =
          const_cast<struct Common_UI_wxWidgets_State&> (iapplication_in->getR ());
        state_r.resources[ACE_TEXT_ALWAYS_CHAR (COMMON_UI_DEFINITION_DESCRIPTOR_MAIN)] =
          std::make_pair (UIDefinitionFilename_in, static_cast<wxObject*> (NULL));
#endif
        break;
      }
      default:
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("invalid/unknown media framework (was: %d), returning\n"),
                    mediaFramework_in));
        return;
      }
    } // end SWITCH
#else
    CBData_in.stream = &stream;
#if defined (GTK_USE)
    CBData_in.UIState.eventHooks.finiHook = idle_finalize_UI_cb;
    CBData_in.UIState.eventHooks.initHook = idle_initialize_UI_cb;
    //CBData_in.UIState.gladeXML[ACE_TEXT_ALWAYS_CHAR (COMMON_UI_DEFINITION_DESCRIPTOR_MAIN)] =
    //  std::make_pair (UIDefinitionFile_in, static_cast<GladeXML*> (NULL));
    CBData_in.UIState.builders[ACE_TEXT_ALWAYS_CHAR (COMMON_UI_DEFINITION_DESCRIPTOR_MAIN)] =
      std::make_pair (UIDefinitionFilename_in, static_cast<GtkBuilder*> (NULL));
#elif defined (WXWIDGETS_USE)
    struct Common_UI_wxWidgets_State& state_r =
      const_cast<struct Common_UI_wxWidgets_State&> (iapplication_in->getR ());
    state_r.resources[ACE_TEXT_ALWAYS_CHAR (COMMON_UI_DEFINITION_DESCRIPTOR_MAIN)] =
      std::make_pair (UIDefinitionFilename_in, static_cast<wxObject*> (NULL));
#endif
#endif // ACE_WIN32 || ACE_WIN64

#if defined (ACE_WIN32) || defined (ACE_WIN64)
    HWND window_p = GetConsoleWindow ();
    if (!window_p)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ::GetConsoleWindow(), returning\n")));
      goto clean;
    } // end IF
    BOOL was_visible_b = false;
    if (!showConsole_in)
      was_visible_b = ShowWindow (window_p, SW_HIDE);
    ACE_UNUSED_ARG (was_visible_b);
#endif // ACE_WIN32 || ACE_WIN64

#if defined (GTK_USE)
    ACE_ASSERT (gtk_manager_p);
    gtk_manager_p->start ();
    ACE_Time_Value timeout (0,
                            COMMON_UI_GTK_TIMEOUT_DEFAULT_MANAGER_INITIALIZATION * 1000);
    int result = ACE_OS::sleep (timeout);
    if (result == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_OS::sleep(%#T): \"%m\", continuing\n"),
                  &timeout));
    if (!gtk_manager_p->isRunning ())
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to start GTK event dispatch, returning\n")));
      goto clean;
    } // end IF
    gtk_manager_p->wait ();
#elif (WXWIDGETS_USE)
    result = iapplication_in->run ();
    if (unlikely (!result))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to Common_UI_wxWidgets_IApplicationBase_T::run(), returning\n")));
      goto clean;
    } // end IF
#endif
  } // end IF
  else
  {
#endif // GUI_SUPPORT
    Stream_IStreamControlBase* stream_p = NULL;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    switch (mediaFramework_in)
    {
      case STREAM_MEDIAFRAMEWORK_DIRECTSHOW:
      {
        if (!directshow_stream.initialize (directShowConfiguration_in.streamConfiguration))
        {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("failed to initialize stream, returning\n")));
          goto clean;
        } // end IF
        stream_p = &directshow_stream;
        break;
      }
      case STREAM_MEDIAFRAMEWORK_MEDIAFOUNDATION:
      {
        if (!mediafoundation_stream.initialize (mediaFoundationConfiguration_in.streamConfiguration))
        {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("failed to initialize stream, returning\n")));
          goto clean;
        } // end IF
        stream_p = &mediafoundation_stream;
        break;
      }
      default:
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("invalid/unknown media framework (was: %d), returning\n"),
                    mediaFramework_in));
        return;
      }
    } // end SWITCH
#else
    if (!stream.initialize (configuration_in.streamConfiguration))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to initialize stream, returning\n")));
      goto clean;
    } // end IF
    stream_p = &stream;
#endif // ACE_WIN32 || ACE_WIN64
    ACE_ASSERT (stream_p);
    // *NOTE*: this will block until the file has been copied...
    stream_p->start ();
//    if (!stream_p->isRunning ())
//    {
//      ACE_DEBUG ((LM_ERROR,
//                  ACE_TEXT ("failed to start stream, aborting\n")));
//      //timer_manager_p->stop ();
//      return;
//    } // end IF
    stream_p->wait (true, false, false);
#if defined (GUI_SUPPORT)
  } // end ELSE
#endif // GUI_SUPPORT

  // step3: clean up
clean:
#if defined (GUI_SUPPORT)
#if defined (GTK_USE)
  gtk_manager_p->stop (true);
#endif // GTK_USE
#endif // GUI_SUPPORT
  timer_manager_p->stop ();

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  switch (mediaFramework_in)
  {
    case STREAM_MEDIAFRAMEWORK_DIRECTSHOW:
#if defined (GUI_SUPPORT)
#if defined (GTK_USE)
      do_finalize_directshow (directShowCBData_in.streamConfiguration);
#elif defined (WXWIDGETS_USE)
      do_finalize_directshow (stream_config_p);
#endif
#endif // GUI_SUPPORT
      break;
    case STREAM_MEDIAFRAMEWORK_MEDIAFOUNDATION:
    {
      Stream_CamSave_MediaFoundation_StreamConfiguration_t::ITERATOR_T iterator =
      mediaFoundationConfiguration_in.streamConfiguration.find (ACE_TEXT_ALWAYS_CHAR (""));
      ACE_ASSERT (iterator != mediaFoundationConfiguration_in.streamConfiguration.end ());
      do_finalize_mediafoundation ((*iterator).second.second.session);
      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown media framework (was: %d), returning\n"),
                  mediaFramework_in));
      return;
    }
  } // end SWITCH
#endif // ACE_WIN32 || ACE_WIN64

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("finished working...\n")));
}

void
do_printVersion (const std::string& programName_in)
{
  STREAM_TRACE (ACE_TEXT ("::do_printVersion"));

  std::ostringstream converter;

  // compiler version string...
  converter << ACE::compiler_major_version ();
  converter << ACE_TEXT (".");
  converter << ACE::compiler_minor_version ();
  converter << ACE_TEXT (".");
  converter << ACE::compiler_beta_version ();

  std::cout << programName_in
            << ACE_TEXT (" compiled on ")
            << ACE::compiler_name ()
            << ACE_TEXT (" ")
            << converter.str ()
            << std::endl << std::endl;

  std::cout << ACE_TEXT ("libraries: ")
            << std::endl
#ifdef HAVE_CONFIG_H
            << ACE_TEXT (ACESTREAM_PACKAGE_NAME)
            << ACE_TEXT (": ")
            << ACE_TEXT (ACESTREAM_PACKAGE_VERSION)
            << std::endl
#endif // HAVE_CONFIG_H
            ;

  converter.str ("");
  // ACE version string...
  converter << ACE::major_version ();
  converter << ACE_TEXT (".");
  converter << ACE::minor_version ();
  converter << ACE_TEXT (".");
  converter << ACE::beta_version ();

  // *NOTE*: cannot use ACE_VERSION, as it doesn't contain the (potential) beta
  // version number... Need this, as the library soname is compared to this
  // string
  std::cout << ACE_TEXT ("ACE: ")
//             << ACE_VERSION
            << converter.str ()
            << std::endl;
}

int
ACE_TMAIN (int argc_in,
           ACE_TCHAR* argv_in[])
{
  STREAM_TRACE (ACE_TEXT ("::main"));

  int result = -1;

  // step0: initialize
  // *PORTABILITY*: on Windows, initialize ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  result = ACE::init ();
  if (result == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE::init(): \"%m\", aborting\n")));
    return EXIT_FAILURE;
  } // end IF
#endif // ACE_WIN32 || ACE_WIN64

  // *PROCESS PROFILE*
  ACE_Profile_Timer process_profile;
  // start profile timer...
  process_profile.start ();

  std::string configuration_path =
    Common_File_Tools::getWorkingDirectory ();

  // initialize framework(s)
  Common_Tools::initialize (false); // initialize random number generator ?
#if defined (GUI_SUPPORT)
#if defined (WXWIDGETS_USE)
  if (!Common_UI_WxWidgets_Tools::initialize ())
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_UI_WxWidgets_Tools::initialize(), aborting\n")));

    // *PORTABILITY*: on Windows, finalize ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    result = ACE::fini ();
    if (result == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE::fini(): \"%m\", continuing\n")));
#endif // ACE_WIN32 || ACE_WIN64
    return EXIT_FAILURE;
  } // end IF
#endif // WXWIDGETS_USE
#endif // GUI_SUPPORT
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  Stream_MediaFramework_Tools::initialize (STREAM_LIB_DEFAULT_MEDIAFRAMEWORK);
#endif // ACE_WIN32 || ACE_WIN64

  // step1a set defaults
  //unsigned int buffer_size = TEST_U_STREAM_CAMSAVE_DEFAULT_BUFFER_SIZE;
  std::string capture_device_identifier;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  switch (STREAM_LIB_DEFAULT_MEDIAFRAMEWORK)
  {
    case STREAM_MEDIAFRAMEWORK_DIRECTSHOW:
    {
      capture_device_identifier =
        Stream_Device_DirectShow_Tools::getDefaultCaptureDevice (CLSID_VideoInputDeviceCategory);
      break;
    }
    case STREAM_MEDIAFRAMEWORK_MEDIAFOUNDATION:
    {
      // *TODO*
      ACE_ASSERT (false);
      ACE_NOTSUP_RETURN (false);
      ACE_NOTREACHED (return false;)
      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown media framework (was: %d), returning\n"),
                  STREAM_LIB_DEFAULT_MEDIAFRAMEWORK));
      return false;
    }
  } // end SWITCH
  bool show_console = false;
#else
  capture_device_identifier =
    ACE_TEXT_ALWAYS_CHAR (MODULE_DEV_DEVICE_DIRECTORY);
  capture_device_identifier += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  capture_device_identifier +=
    ACE_TEXT_ALWAYS_CHAR (MODULE_DEV_DEFAULT_VIDEO_DEVICE);
#endif // ACE_WIN32 || ACE_WIN64
  std::string path = Common_File_Tools::getTempDirectory ();
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  path += ACE_TEXT_ALWAYS_CHAR (TEST_U_STREAM_CAMSAVE_DEFAULT_OUTPUT_FILE);
  std::string target_filename = path;
  path = configuration_path;
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  path += ACE_TEXT_ALWAYS_CHAR (TEST_U_STREAM_CONFIGURATION_DIRECTORY);
#if defined (GUI_SUPPORT)
  std::string UI_definition_filename = path;
  UI_definition_filename += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  UI_definition_filename +=
    ACE_TEXT_ALWAYS_CHAR (TEST_U_STREAM_CAMSAVE_UI_DEFINITION_FILE);
#endif // GUI_SUPPORT
  bool log_to_file = false;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  enum Stream_MediaFramework_Type media_framework_e =
    STREAM_LIB_DEFAULT_MEDIAFRAMEWORK;
#endif // ACE_WIN32 || ACE_WIN64
  enum Stream_Visualization_VideoRenderer video_renderer_e =
    STREAM_VIS_RENDERER_VIDEO_DEFAULT;
  struct Common_UI_DisplayDevice display_device_s =
    Common_UI_Tools::getDefaultDisplayDevice ();
  unsigned int statistic_reporting_interval =
    STREAM_DEFAULT_STATISTIC_REPORTING_INTERVAL;
  bool trace_information = false;
  bool print_version_and_exit = false;
  //bool run_stress_test = false;
  bool result_2 = false;

  // step1b: parse/process/validate configuration
  if (!do_processArguments (argc_in,
                            argv_in,
                            capture_device_identifier,
#if defined (ACE_WIN32) || defined (ACE_WIN64)
                            show_console,
#endif // ACE_WIN32 || ACE_WIN64
                            target_filename,
#if defined (GUI_SUPPORT)
                            UI_definition_filename,
#endif // GUI_SUPPORT
                            log_to_file,
#if defined (ACE_WIN32) || defined (ACE_WIN64)
                            media_framework_e,
#endif // ACE_WIN32 || ACE_WIN64
                            display_device_s,
                            video_renderer_e,
                            statistic_reporting_interval,
                            trace_information,
                            print_version_and_exit))
  {
    do_printUsage (ACE::basename (argv_in[0]));
    // *PORTABILITY*: on Windows, finalize ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    result = ACE::fini ();
    if (result == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE::fini(): \"%m\", continuing\n")));
#endif // ACE_WIN32 || ACE_WIN64
    return EXIT_FAILURE;
  } // end IF

  // step1c: validate arguments
  // *IMPORTANT NOTE*: iff the number of message buffers is limited, the
  //                   reactor/proactor thread could (dead)lock on the
  //                   allocator lock, as it cannot dispatch events that would
  //                   free slots
  if (TEST_U_MAX_MESSAGES)
    ACE_DEBUG ((LM_WARNING,
                ACE_TEXT ("limiting the number of message buffers could (!) lead to a deadlock --> ensure the streaming elements are sufficiently efficient in this regard\n")));
  if (
#if defined (GUI_SUPPORT)
      (!UI_definition_filename.empty () &&
       !Common_File_Tools::isReadable (UI_definition_filename))
#else
      false
#endif // GUI_SUPPORT
     )
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("invalid arguments, aborting\n")));

    do_printUsage (ACE::basename (argv_in[0]));
    // *PORTABILITY*: on Windows, finalize ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    result = ACE::fini ();
    if (result == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE::fini(): \"%m\", continuing\n")));
#endif // ACE_WIN32 || ACE_WIN64
    return EXIT_FAILURE;
  } // end IF
  //if (run_stress_test)
  //  action_mode = Net_Client_TimeoutHandler::ACTION_STRESS;

  // step1d: initialize logging and/or tracing
  Common_MessageStack_t* logstack_p = NULL;
  ACE_SYNCH_MUTEX* lock_p = NULL;
#if defined (GUI_SUPPORT)
#if defined (GTK_USE)
  Common_UI_GTK_Manager_t* gtk_manager_p =
    COMMON_UI_GTK_MANAGER_SINGLETON::instance ();
  ACE_ASSERT (gtk_manager_p);
  Common_UI_GTK_State_t& state_r =
    const_cast<Common_UI_GTK_State_t&> (gtk_manager_p->getR_2 ());
  logstack_p = &state_r.logStack;
  lock_p = &state_r.logStackLock;
#endif // GTK_USE
#endif // GUI_SUPPORT

#if defined (GUI_SUPPORT)
#if defined (GTK_USE)
  Common_Logger_t logger (logstack_p,
                          lock_p);
#endif // GTK_USE
#endif // GUI_SUPPORT
  std::string log_file_name;
  if (log_to_file)
    log_file_name =
        Common_File_Tools::getLogFilename (ACE_TEXT_ALWAYS_CHAR (ACESTREAM_PACKAGE_NAME),
                                           ACE::basename (argv_in[0]));
  if (!Common_Log_Tools::initializeLogging (ACE::basename (argv_in[0]),                   // program name
                                            log_file_name,                                // log file name
                                            false,                                        // log to syslog ?
                                            false,                                        // trace messages ?
                                            trace_information,                            // debug messages ?
#if defined (GUI_SUPPORT)
#if defined (GTK_USE)
                                            (UI_definition_filename.empty () ? NULL
                                                                             : &logger))) // (ui) logger ?
#elif defined (WXWIDGETS_USE)
                                            NULL))                                        // (ui) logger ?
#endif // XXX_USE
#else
                                            NULL))                                        // (ui) logger ?
#endif // GUI_SUPPORT
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_Log_Tools::initializeLogging(), aborting\n")));

    // *PORTABILITY*: on Windows, finalize ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    result = ACE::fini ();
    if (result == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE::fini(): \"%m\", continuing\n")));
#endif // ACE_WIN32 || ACE_WIN64
    return EXIT_FAILURE;
  } // end IF

  Stream_Visualization_Tools::initialize (STREAM_VIS_FRAMEWORK_DEFAULT,
                                          true);

#if defined (GUI_SUPPORT)
  struct Stream_CamSave_UI_CBData* ui_cb_data_p = NULL;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  struct Stream_CamSave_DirectShow_Configuration directshow_configuration;
  struct Stream_CamSave_DirectShow_UI_CBData directshow_ui_cb_data;
  struct Stream_CamSave_MediaFoundation_Configuration mediafoundation_configuration;
  struct Stream_CamSave_MediaFoundation_UI_CBData mediafoundation_ui_cb_data;
  Stream_MediaFramework_Tools::initialize (media_framework_e);
  switch (media_framework_e)
  {
    case STREAM_MEDIAFRAMEWORK_DIRECTSHOW:
    {
      directshow_ui_cb_data.configuration = &directshow_configuration;
      directshow_ui_cb_data.mediaFramework = media_framework_e;
      ui_cb_data_p = &directshow_ui_cb_data;
      break;
    }
    case STREAM_MEDIAFRAMEWORK_MEDIAFOUNDATION:
    {
      mediafoundation_ui_cb_data.configuration = &mediafoundation_configuration;
      mediafoundation_ui_cb_data.mediaFramework = media_framework_e;
      ui_cb_data_p = &mediafoundation_ui_cb_data;
      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown media framework (was: %d), aborting\n"),
                  media_framework_e));

      Common_Log_Tools::finalizeLogging ();
      // *PORTABILITY*: on Windows, finalize ACE...
      result = ACE::fini ();
      if (result == -1)
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ACE::fini(): \"%m\", continuing\n")));
      return EXIT_FAILURE;
    }
  } // end SWITCH
#else
  struct Stream_CamSave_Configuration configuration;
  struct Stream_CamSave_V4L_UI_CBData ui_cb_data;
  ui_cb_data.configuration = &configuration;
  ui_cb_data_p = &ui_cb_data;
#endif // ACE_WIN32 || ACE_WIN64
  ACE_ASSERT (ui_cb_data_p);
#endif // GUI_SUPPORT

  // step1h: initialize UI framework
#if defined (GUI_SUPPORT)
#if defined (GTK_USE)
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  Stream_CamSave_DirectShow_GtkBuilderDefinition_t directshow_ui_definition (argc_in,
                                                                             argv_in,
                                                                             &directshow_ui_cb_data);
  Stream_CamSave_MediaFoundation_GtkBuilderDefinition_t mediafoundation_ui_definition (argc_in,
                                                                                       argv_in,
                                                                                       &mediafoundation_ui_cb_data);
#else
  Stream_CamSave_GtkBuilderDefinition_t ui_definition (argc_in,
                                                       argv_in,
                                                       &ui_cb_data);
#endif // ACE_WIN32 || ACE_WIN64
#elif defined (WXWIDGETS_USE)
  Common_UI_wxWidgets_IApplicationBase_t* iapplication_p = NULL;
  struct Common_UI_State* ui_state_p = NULL;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  switch (media_framework_e)
  {
    case STREAM_MEDIAFRAMEWORK_DIRECTSHOW:
    {
      ACE_NEW_NORETURN (iapplication_p,
                        Stream_CamSave_DirectShow_WxWidgetsApplication_t (toplevel_widget_name_string_,
                                                                          argc_in,
                                                                          Common_UI_WxWidgets_Tools::convertArgV (argc_in, argv_in),
                                                                          COMMON_UI_WXWIDGETS_APP_CMDLINE_DEFAULT_PARSE));
      Stream_CamSave_DirectShow_WxWidgetsApplication_t::IINITIALIZE_T* iinitialize_p =
        dynamic_cast<Stream_CamSave_DirectShow_WxWidgetsApplication_t::IINITIALIZE_T*> (iapplication_p);
      iinitialize_p->initialize (directshow_ui_cb_data);
      Stream_CamSave_DirectShow_WxWidgetsIApplication_t* iapplication_2 =
        dynamic_cast<Stream_CamSave_DirectShow_WxWidgetsIApplication_t*> (iapplication_p);
      ACE_ASSERT (iapplication_2);
      Stream_CamSave_DirectShow_WxWidgetsApplication_t::STATE_T& state_r =
        const_cast<Stream_CamSave_DirectShow_WxWidgetsApplication_t::STATE_T&> (iapplication_2->getR ());
      Stream_CamSave_DirectShow_WxWidgetsApplication_t::CONFIGURATION_T& configuration_r =
        const_cast<Stream_CamSave_DirectShow_WxWidgetsApplication_t::CONFIGURATION_T&> (iapplication_2->getR_2 ());
      configuration_r.UIState = &state_r;
      ACE_ASSERT (configuration_r.UIState);
      ui_state_p =
        const_cast<Stream_CamSave_DirectShow_WxWidgetsApplication_t::CONFIGURATION_T&> (configuration_r).UIState;
      break;
    }
    case STREAM_MEDIAFRAMEWORK_MEDIAFOUNDATION:
    {
      ACE_NEW_NORETURN (iapplication_p,
                        Stream_CamSave_MediaFoundation_WxWidgetsApplication_t (toplevel_widget_name_string_,
                                                                               argc_in,
                                                                               Common_UI_WxWidgets_Tools::convertArgV (argc_in,
                                                                                                                       argv_in),
                                                                               COMMON_UI_WXWIDGETS_APP_CMDLINE_DEFAULT_PARSE));
      Stream_CamSave_MediaFoundation_WxWidgetsApplication_t::IINITIALIZE_T* iinitialize_p =
        dynamic_cast<Stream_CamSave_MediaFoundation_WxWidgetsApplication_t::IINITIALIZE_T*> (iapplication_p);
      iinitialize_p->initialize (mediafoundation_ui_cb_data);
      Stream_CamSave_MediaFoundation_WxWidgetsIApplication_t* iapplication_2 =
        dynamic_cast<Stream_CamSave_MediaFoundation_WxWidgetsIApplication_t*> (iapplication_p);
      ACE_ASSERT (iapplication_2);
      const Stream_CamSave_MediaFoundation_WxWidgetsApplication_t::CONFIGURATION_T& configuration_r =
        iapplication_2->getR_2 ();
      ACE_ASSERT (configuration_r.UIState);
      ui_state_p =
        const_cast<Stream_CamSave_MediaFoundation_WxWidgetsApplication_t::CONFIGURATION_T&> (configuration_r).UIState;
      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown media framework (was: %d), returning\n"),
                  media_framework_e));

      Common_Log_Tools::finalizeLogging ();
      // *PORTABILITY*: on Windows, finalize ACE...
      result = ACE::fini ();
      if (result == -1)
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ACE::fini(): \"%m\", continuing\n")));
      return EXIT_FAILURE;
    }
  } // end SWITCH
#else
  ACE_NEW_NORETURN (iapplication_p,
                    Stream_CamSave_V4L_WxWidgetsApplication_t (toplevel_widget_name_string_,
                                                               argc_in,
                                                               argv_in,
                                                               COMMON_UI_WXWIDGETS_APP_CMDLINE_DEFAULT_PARSE));
  Stream_CamSave_V4L_WxWidgetsApplication_t::IINITIALIZE_T* iinitialize_p =
    dynamic_cast<Stream_CamSave_V4L_WxWidgetsApplication_t::IINITIALIZE_T*> (iapplication_p);
  iinitialize_p->initialize (ui_cb_data);
  Stream_CamSave_V4L_WxWidgetsIApplication_t* iapplication_2 =
    dynamic_cast<Stream_CamSave_V4L_WxWidgetsIApplication_t*> (iapplication_p);
  ACE_ASSERT (iapplication_2);
  const Stream_CamSave_V4L_WxWidgetsApplication_t::CONFIGURATION_T& configuration_r =
    iapplication_2->getR_2 ();
  ACE_ASSERT (configuration_r.UIState);
  ui_state_p =
    const_cast<Stream_CamSave_V4L_WxWidgetsApplication_t::CONFIGURATION_T&> (configuration_r).UIState;
#endif // ACE_WIN32 || ACE_WIN64
  if (!iapplication_p)
  {
    ACE_DEBUG ((LM_CRITICAL,
                ACE_TEXT ("failed to allocate memory: %m, aborting\n")));

    Common_Log_Tools::finalizeLogging ();
    // *PORTABILITY*: on Windows, finalize ACE...
    result = ACE::fini ();
    if (result == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE::fini(): \"%m\", continuing\n")));
    return EXIT_FAILURE;
  } // end IF
#endif
  ACE_ASSERT (ui_state_p);
#endif // GUI_SUPPORT

  // step1e: pre-initialize signal handling
  ACE_Sig_Set signal_set (0);
  ACE_Sig_Set ignored_signal_set (0);
  do_initializeSignals (true, // allow SIGUSR1/SIGBREAK
                        signal_set,
                        ignored_signal_set);
  Common_SignalActions_t previous_signal_actions;
  sigset_t previous_signal_mask;
  result = ACE_OS::sigemptyset (&previous_signal_mask);
  if (result == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::sigemptyset(): \"%m\", aborting\n")));

    Common_Log_Tools::finalizeLogging ();
    // *PORTABILITY*: on Windows, finalize ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    result = ACE::fini ();
    if (result == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE::fini(): \"%m\", continuing\n")));
#endif // ACE_WIN32 || ACE_WIN64
    return EXIT_FAILURE;
  } // end IF
  if (!Common_Signal_Tools::preInitialize (signal_set,
                                           true,
                                           previous_signal_actions,
                                           previous_signal_mask))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_Signal_Tools::preInitialize(), aborting\n")));

    Common_Log_Tools::finalizeLogging ();
    // *PORTABILITY*: on Windows, finalize ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    result = ACE::fini ();
    if (result == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE::fini(): \"%m\", continuing\n")));
#endif // ACE_WIN32 || ACE_WIN64
    return EXIT_FAILURE;
  } // end IF
  ACE_SYNCH_RECURSIVE_MUTEX* lock_2 = NULL;
#if defined (GUI_SUPPORT)
#if defined (GTK_USE)
  lock_2 = &state_r.subscribersLock;
#endif // GTK_USE
#endif // GUI_SUPPORT
  Stream_CamSave_SignalHandler signal_handler (COMMON_SIGNAL_DISPATCH_SIGNAL,
                                               lock_2);

  // step1f: handle specific program modes
  if (print_version_and_exit)
  {
    do_printVersion (ACE::basename (argv_in[0]));

    Common_Signal_Tools::finalize (COMMON_SIGNAL_DISPATCH_SIGNAL,
                                   signal_set,
                                   previous_signal_actions,
                                   previous_signal_mask);
    Common_Log_Tools::finalizeLogging ();
    // *PORTABILITY*: on Windows, finalize ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    result = ACE::fini ();
    if (result == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE::fini(): \"%m\", continuing\n")));
#endif // ACE_WIN32 || ACE_WIN64
    return EXIT_SUCCESS;
  } // end IF

  // step1g: set process resource limits
  // *NOTE*: settings will be inherited by any child processes
  if (!Common_Tools::setResourceLimits (false,  // file descriptors
                                        true,   // stack traces
                                        false)) // pending signals
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_Tools::setResourceLimits(), aborting\n")));

    Common_Signal_Tools::finalize (COMMON_SIGNAL_DISPATCH_SIGNAL,
                                   signal_set,
                                   previous_signal_actions,
                                   previous_signal_mask);
    Common_Log_Tools::finalizeLogging ();
    // *PORTABILITY*: on Windows, finalize ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    result = ACE::fini ();
    if (result == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE::fini(): \"%m\", continuing\n")));
#endif // ACE_WIN32 || ACE_WIN64
    return EXIT_FAILURE;
  } // end IF

#if defined (GUI_SUPPORT)
  if (!UI_definition_filename.empty ())
  {
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    switch (media_framework_e)
    {
      case STREAM_MEDIAFRAMEWORK_DIRECTSHOW:
      {
#if defined (GTK_USE)
        result_2 = gtk_manager_p->initialize (argc_in,
                                              argv_in,
                                              &directshow_ui_cb_data.UIState,
                                              &directshow_ui_definition);
#endif // GTK_USE
        break;
      }
      case STREAM_MEDIAFRAMEWORK_MEDIAFOUNDATION:
      {
#if defined (GTK_USE)
        result_2 =
          gtk_manager_p->initialize (argc_in,
                                     argv_in,
                                     &mediafoundation_ui_cb_data.UIState,
                                     &mediafoundation_ui_definition);
#endif // GTK_USE
        break;
      }
      default:
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("invalid/unknown media framework (was: %d), aborting\n"),
                    media_framework_e));

        Common_Signal_Tools::finalize (COMMON_SIGNAL_DISPATCH_SIGNAL,
                                       signal_set,
                                       previous_signal_actions,
                                       previous_signal_mask);
        Common_Log_Tools::finalizeLogging ();
        Common_Tools::finalize ();
        // *PORTABILITY*: on Windows, finalize ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
        result = ACE::fini ();
        if (result == -1)
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("failed to ACE::fini(): \"%m\", continuing\n")));
#endif // ACE_WIN32 || ACE_WIN64
        return EXIT_FAILURE;
      }
    } // end SWITCH
#else
#if defined (GTK_USE)
    result_2 = gtk_manager_p->initialize (argc_in,
                                          argv_in,
                                          &ui_cb_data.UIState,
                                          &ui_definition);
#endif // GTK_USE
#endif // ACE_WIN32 || ACE_WIN64
#if defined (GTK_USE)
    if (!result_2)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to Common_UI_GTK_Manager_T::initialize(), aborting\n")));

      Common_Signal_Tools::finalize (COMMON_SIGNAL_DISPATCH_SIGNAL,
                                     signal_set,
                                     previous_signal_actions,
                                     previous_signal_mask);
      Common_Log_Tools::finalizeLogging ();
      Common_Tools::finalize ();
      // *PORTABILITY*: on Windows, finalize ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
      result = ACE::fini ();
      if (result == -1)
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ACE::fini(): \"%m\", continuing\n")));
#endif // ACE_WIN32 || ACE_WIN64
      return EXIT_FAILURE;
    } // end IF
#endif // GTK_USE
  } // end IF
#endif // GUI_SUPPORT

  ACE_High_Res_Timer timer;
  timer.start ();
  // step2: do actual work
  do_work (capture_device_identifier,
#if defined (ACE_WIN32) || defined (ACE_WIN64)
           show_console,
#endif // ACE_WIN32 || ACE_WIN64
           target_filename,
#if defined (ACE_WIN32) || defined (ACE_WIN64)
           media_framework_e,
#endif // ACE_WIN32 || ACE_WIN64
           display_device_s,
           statistic_reporting_interval,
#if defined (ACE_WIN32) || defined (ACE_WIN64)
           directshow_configuration,
           mediafoundation_configuration,
#else
           ui_configuration,
#endif // ACE_WIN32 || ACE_WIN64
#if defined (GUI_SUPPORT)
           UI_definition_filename,
#if defined (ACE_WIN32) || defined (ACE_WIN64)
           directshow_ui_cb_data,
           mediafoundation_ui_cb_data,
#else
           ui_cb_data,
#endif // ACE_WIN32 || ACE_WIN64
#if defined (WXWIDGETS_USE)
           iapplication_p,
#endif // WXWIDGETS_USE
#endif // GUI_SUPPORT
           video_renderer_e,
           signal_set,
           ignored_signal_set,
           previous_signal_actions,
           signal_handler);
  timer.stop ();

  // debug info
  std::string working_time_string;
  ACE_Time_Value working_time;
  timer.elapsed_time (working_time);
  working_time_string = Common_Timer_Tools::periodToString (working_time);

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("total working time (h:m:s.us): \"%s\"...\n"),
              ACE_TEXT (working_time_string.c_str ())));

  // stop profile timer...
  process_profile.stop ();

  // only process profile left to do...
  ACE_Profile_Timer::ACE_Elapsed_Time elapsed_time;
  elapsed_time.real_time = 0.0;
  elapsed_time.user_time = 0.0;
  elapsed_time.system_time = 0.0;
  if (process_profile.elapsed_time (elapsed_time) == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Profile_Timer::elapsed_time: \"%m\", aborting\n")));

    Common_Signal_Tools::finalize (COMMON_SIGNAL_DISPATCH_SIGNAL,
                                   signal_set,
                                   previous_signal_actions,
                                   previous_signal_mask);
    Common_Log_Tools::finalizeLogging ();
    // *PORTABILITY*: on Windows, finalize ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    result = ACE::fini ();
    if (result == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE::fini(): \"%m\", continuing\n")));
#endif // ACE_WIN32 || ACE_WIN64
    return EXIT_FAILURE;
  } // end IF
  ACE_Profile_Timer::Rusage elapsed_rusage;
  ACE_OS::memset (&elapsed_rusage, 0, sizeof (elapsed_rusage));
  process_profile.elapsed_rusage (elapsed_rusage);
  ACE_Time_Value user_time (elapsed_rusage.ru_utime);
  ACE_Time_Value system_time (elapsed_rusage.ru_stime);
  std::string user_time_string;
  std::string system_time_string;
  user_time_string = Common_Timer_Tools::periodToString (user_time);
  system_time_string = Common_Timer_Tools::periodToString (system_time);

  // debug info
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT (" --> Process Profile <--\nreal time = %A seconds\nuser time = %A seconds\nsystem time = %A seconds\n --> Resource Usage <--\nuser time used: %s\nsystem time used: %s\n"),
              elapsed_time.real_time,
              elapsed_time.user_time,
              elapsed_time.system_time,
              ACE_TEXT (user_time_string.c_str ()),
              ACE_TEXT (system_time_string.c_str ())));
#else
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT (" --> Process Profile <--\nreal time = %A seconds\nuser time = %A seconds\nsystem time = %A seconds\n --> Resource Usage <--\nuser time used: %s\nsystem time used: %s\nmaximum resident set size = %d\nintegral shared memory size = %d\nintegral unshared data size = %d\nintegral unshared stack size = %d\npage reclaims = %d\npage faults = %d\nswaps = %d\nblock input operations = %d\nblock output operations = %d\nmessages sent = %d\nmessages received = %d\nsignals received = %d\nvoluntary context switches = %d\ninvoluntary context switches = %d\n"),
              elapsed_time.real_time,
              elapsed_time.user_time,
              elapsed_time.system_time,
              ACE_TEXT (user_time_string.c_str ()),
              ACE_TEXT (system_time_string.c_str ()),
              elapsed_rusage.ru_maxrss,
              elapsed_rusage.ru_ixrss,
              elapsed_rusage.ru_idrss,
              elapsed_rusage.ru_isrss,
              elapsed_rusage.ru_minflt,
              elapsed_rusage.ru_majflt,
              elapsed_rusage.ru_nswap,
              elapsed_rusage.ru_inblock,
              elapsed_rusage.ru_oublock,
              elapsed_rusage.ru_msgsnd,
              elapsed_rusage.ru_msgrcv,
              elapsed_rusage.ru_nsignals,
              elapsed_rusage.ru_nvcsw,
              elapsed_rusage.ru_nivcsw));
#endif // ACE_WIN32 || ACE_WIN64

  Common_Signal_Tools::finalize (COMMON_SIGNAL_DISPATCH_SIGNAL,
                                 signal_set,
                                 previous_signal_actions,
                                 previous_signal_mask);
  Common_Log_Tools::finalizeLogging ();

  // *PORTABILITY*: on Windows, finalize ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  result = ACE::fini ();
  if (result == -1)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE::fini(): \"%m\", continuing\n")));
#endif // ACE_WIN32 || ACE_WIN64

  return EXIT_SUCCESS;
} // end main
