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
#include <sstream>
#include <string>

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include <initguid.h> // *NOTE*: this exports DEFINE_GUIDs (see stream_misc_common.h)
// *NOTE*: uuids.h doesn't have double include protection
#if defined (UUIDS_H)
#else
#define UUIDS_H
#include <uuids.h>
#endif // UUIDS_H
#endif // ACE_WIN32 || ACE_WIN64

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

#if defined (HAVE_CONFIG_H)
#include "Common_config.h"
#endif // HAVE_CONFIG_H

#include "common_file_tools.h"
#include "common_tools.h"

#include "common_log_tools.h"
#include "common_logger.h"

#include "common_signal_tools.h"

#include "common_timer_tools.h"

#if defined (GUI_SUPPORT)
#include "common_ui_defines.h"
#if defined (GTK_USE)
//#include "common_ui_glade_definition.h"
#include "common_ui_gtk_builder_definition.h"
#include "common_ui_gtk_manager_common.h"
#endif // GTK_USE
#endif // GUI_SUPPORT

#if defined (HAVE_CONFIG_H)
#include "ACEStream_config.h"
#endif // HAVE_CONFIG_H

#include "stream_allocatorheap.h"
#include "stream_macros.h"

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include "stream_dev_directshow_tools.h"
#endif // ACE_WIN32 || ACE_WIN64

#include "stream_misc_common.h"
#include "stream_misc_defines.h"

#include "test_i_common.h"
#include "test_i_defines.h"

#if defined (GUI_SUPPORT)
#if defined (GTK_USE)
#include "test_i_callbacks.h"
#endif // GTK_USE
#endif // GUI_SUPPORT
#include "test_i_common_modules.h"
#include "test_i_module_eventhandler.h"

#include "test_i_target_common.h"
#include "test_i_target_eventhandler.h"
#include "test_i_target_listener_common.h"
#include "test_i_target_signalhandler.h"
#include "test_i_target_stream.h"

const char stream_name_string_[] = ACE_TEXT_ALWAYS_CHAR ("CamStream");

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
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-b [VALUE]  : buffer size (byte(s)) [")
            << CAMSTREAM_DEFAULT_BUFFER_SIZE
            << ACE_TEXT_ALWAYS_CHAR ("])")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-c [VALUE]  : maximum number of connections [")
            << TEST_I_MAXIMUM_NUMBER_OF_OPEN_CONNECTIONS
            << ACE_TEXT_ALWAYS_CHAR ("]")
            << std::endl;
  std::string path = configuration_path;
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  path += ACE_TEXT_ALWAYS_CHAR (COMMON_LOCATION_CONFIGURATION_SUBDIRECTORY);
  std::string gtk_rc_file = path;
  gtk_rc_file += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  gtk_rc_file += ACE_TEXT_ALWAYS_CHAR (TEST_I_DEFAULT_GTK_RC_FILE);
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-e          : Gtk .rc file [\"")
            << gtk_rc_file
            << ACE_TEXT_ALWAYS_CHAR ("\"]")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-f[[STRING]]: (target) file name [\"")
            << ACE_TEXT_ALWAYS_CHAR (TEST_I_DEFAULT_OUTPUT_FILE)
            << ACE_TEXT_ALWAYS_CHAR ("\"] {\"\" --> do not save}")
            << std::endl;
  std::string UI_file = path;
  UI_file += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  UI_file += ACE_TEXT_ALWAYS_CHAR (TEST_I_DEFAULT_TARGET_GLADE_FILE);
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-g[[STRING]]: UI file [\"")
            << UI_file
            << ACE_TEXT_ALWAYS_CHAR ("\"] {\"\" --> no GUI}")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-h          : use thread pool [")
            << COMMON_EVENT_REACTOR_DEFAULT_USE_THREADPOOL
            << ACE_TEXT_ALWAYS_CHAR ("]")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-l          : log to a file [")
            << false
            << ACE_TEXT_ALWAYS_CHAR ("]")
            << std::endl;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-m          : use media foundation [")
            << (STREAM_LIB_DEFAULT_MEDIAFRAMEWORK == STREAM_MEDIAFRAMEWORK_MEDIAFOUNDATION)
            << ACE_TEXT_ALWAYS_CHAR ("]")
            << std::endl;
#endif
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-n [STRING] : network interface [\"")
            << ACE_TEXT_ALWAYS_CHAR (NET_INTERFACE_DEFAULT_ETHERNET)
            << ACE_TEXT_ALWAYS_CHAR ("\"]")
            << std::endl;
  // *TODO*: this doesn't really make sense (yet)
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-o          : use loopback device [")
            << false
            << ACE_TEXT_ALWAYS_CHAR ("]")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-p [VALUE]  : listening port [")
            << TEST_I_DEFAULT_PORT
            << ACE_TEXT_ALWAYS_CHAR ("]")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-r          : use reactor [")
            << (COMMON_EVENT_DEFAULT_DISPATCH == COMMON_EVENT_DISPATCH_REACTOR)
            << ACE_TEXT_ALWAYS_CHAR ("]")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-s [VALUE]  : statistic reporting interval (second(s)) [")
            << STREAM_DEFAULT_STATISTIC_REPORTING_INTERVAL
            << ACE_TEXT_ALWAYS_CHAR ("] [0: off]")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-t          : trace information [")
            << false
            << ACE_TEXT_ALWAYS_CHAR ("]")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-u          : use UDP [")
            << false
            << ACE_TEXT_ALWAYS_CHAR ("]")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-v          : print version information and exit [")
            << false
            << ACE_TEXT_ALWAYS_CHAR ("]")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-x [VALUE]  : #dispatch threads [")
            << TEST_I_DEFAULT_NUMBER_OF_DISPATCHING_THREADS
            << ACE_TEXT_ALWAYS_CHAR ("]")
            << std::endl;
  // *TODO*: implement a format negotiation handshake protocol
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-z [VALUE]  : frame size (byte(s)) [")
            << TEST_I_DEFAULT_FRAME_SIZE
            << ACE_TEXT_ALWAYS_CHAR ("]")
            << std::endl;
}

bool
do_processArguments (int argc_in,
                     ACE_TCHAR** argv_in, // cannot be const...
                     unsigned int& bufferSize_out,
                     unsigned int& maximumNumberOfConnections_out,
                     std::string& gtkRcFile_out,
                     std::string& outputFile_out,
                     std::string& gtkGladeFile_out,
                     bool& useThreadPool_out,
                     bool& logToFile_out,
#if defined (ACE_WIN32) || defined (ACE_WIN64)
                     enum Stream_MediaFramework_Type& mediaFramework_out,
#endif // ACE_WIN32 || ACE_WIN64
                     std::string& netWorkInterface_out,
                     bool& useLoopBack_out,
                     unsigned short& listeningPortNumber_out,
                     bool& useReactor_out,
                     unsigned int& statisticReportingInterval_out,
                     bool& traceInformation_out,
                     bool& useUDP_out,
                     bool& printVersionAndExit_out,
                     unsigned int& numberOfDispatchThreads_out,
                     unsigned int& frameSize_out)
{
  STREAM_TRACE (ACE_TEXT ("::do_processArguments"));

  std::string configuration_path =
    Common_File_Tools::getWorkingDirectory ();

  // initialize results
  std::string path = configuration_path;
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  path += ACE_TEXT_ALWAYS_CHAR (COMMON_LOCATION_CONFIGURATION_SUBDIRECTORY);
  bufferSize_out = CAMSTREAM_DEFAULT_BUFFER_SIZE;
  gtkRcFile_out = path;
  gtkRcFile_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  gtkRcFile_out += ACE_TEXT_ALWAYS_CHAR (TEST_I_DEFAULT_GTK_RC_FILE);
  maximumNumberOfConnections_out =
    TEST_I_MAXIMUM_NUMBER_OF_OPEN_CONNECTIONS;
  outputFile_out = ACE_TEXT_ALWAYS_CHAR (TEST_I_DEFAULT_OUTPUT_FILE);
  gtkGladeFile_out = path;
  gtkGladeFile_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  gtkGladeFile_out += ACE_TEXT_ALWAYS_CHAR (TEST_I_DEFAULT_TARGET_GLADE_FILE);
  useThreadPool_out = COMMON_EVENT_REACTOR_DEFAULT_USE_THREADPOOL;
  logToFile_out = false;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  mediaFramework_out = STREAM_LIB_DEFAULT_MEDIAFRAMEWORK;
#endif // ACE_WIN32 || ACE_WIN64
  netWorkInterface_out =
    ACE_TEXT_ALWAYS_CHAR (NET_INTERFACE_DEFAULT_ETHERNET);
  useLoopBack_out = false;
  listeningPortNumber_out = TEST_I_DEFAULT_PORT;
  useReactor_out =
          (COMMON_EVENT_DEFAULT_DISPATCH == COMMON_EVENT_DISPATCH_REACTOR);
  statisticReportingInterval_out = STREAM_DEFAULT_STATISTIC_REPORTING_INTERVAL;
  traceInformation_out = false;
  useUDP_out = false;
  printVersionAndExit_out = false;
  numberOfDispatchThreads_out =
    TEST_I_DEFAULT_NUMBER_OF_DISPATCHING_THREADS;
  frameSize_out = TEST_I_DEFAULT_FRAME_SIZE;

  ACE_Get_Opt argumentParser (argc_in,
                              argv_in,
#if defined (ACE_WIN32) || defined (ACE_WIN64)
                              ACE_TEXT ("b:c:e:f::g::hlmn:op:rs:tuvx:z:"),
#else
                              ACE_TEXT ("b:c:e:f::g::hln:op:rs:tuvx:z:"),
#endif // ACE_WIN32 || ACE_WIN64
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
      case 'b':
      {
        converter.clear ();
        converter.str (ACE_TEXT_ALWAYS_CHAR (""));
        converter << argumentParser.opt_arg ();
        converter >> bufferSize_out;
        break;
      }
      case 'c':
      {
        converter.clear ();
        converter.str (ACE_TEXT_ALWAYS_CHAR (""));
        converter << argumentParser.opt_arg ();
        converter >> maximumNumberOfConnections_out;
        break;
      }
      case 'e':
      {
        gtkRcFile_out = ACE_TEXT_ALWAYS_CHAR (argumentParser.opt_arg ());
        break;
      }
      case 'f':
      {
        ACE_TCHAR* opt_arg = argumentParser.opt_arg ();
        if (opt_arg)
          outputFile_out = ACE_TEXT_ALWAYS_CHAR (argumentParser.opt_arg ());
        else
          outputFile_out.clear ();
        break;
      }
      case 'g':
      {
        ACE_TCHAR* opt_arg = argumentParser.opt_arg ();
        if (opt_arg)
          gtkGladeFile_out = ACE_TEXT_ALWAYS_CHAR (opt_arg);
        else
          gtkGladeFile_out.clear ();
        break;
      }
      case 'h':
      {
        useThreadPool_out = true;
        break;
      }
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
      case 'n':
      {
        netWorkInterface_out = ACE_TEXT_ALWAYS_CHAR (argumentParser.opt_arg ());
        break;
      }
      case 'o':
      {
        useLoopBack_out = true;
        break;
      }
      case 'p':
      {
        converter.clear ();
        converter.str (ACE_TEXT_ALWAYS_CHAR (""));
        converter << argumentParser.opt_arg ();
        converter >> listeningPortNumber_out;
        break;
      }
      case 'r':
      {
        useReactor_out = true;
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
      case 'u':
      {
        useUDP_out = true;
        break;
      }
      case 'v':
      {
        printVersionAndExit_out = true;
        break;
      }
      case 'x':
      {
        converter.clear ();
        converter.str (ACE_TEXT_ALWAYS_CHAR (""));
        converter << argumentParser.opt_arg ();
        converter >> numberOfDispatchThreads_out;
        break;
      }
      case 'z':
      {
        converter.clear ();
        converter.str (ACE_TEXT_ALWAYS_CHAR (""));
        converter << argumentParser.opt_arg ();
        converter >> frameSize_out;
        break;
      }
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
                    ACE_TEXT (argumentParser.last_option ())));
        return false;
      }
      case 0:
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("found long option \"%s\", aborting\n"),
                    ACE_TEXT (argumentParser.long_option ())));
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
#endif // ACE_WIN32 || ACE_WIN64
}

#if defined (ACE_WIN32) || defined (ACE_WIN64)
bool
do_initialize_directshow (struct _AMMediaType*& mediaType_out,
                          bool coInitialize_in)
{
  STREAM_TRACE (ACE_TEXT ("::do_initialize_directshow"));

  HRESULT result = E_FAIL;
  bool is_COM_initialized = false;

  // sanity check(s)
  if (mediaType_out)
    Stream_MediaFramework_DirectShow_Tools::free (*mediaType_out);

  if (!coInitialize_in)
    goto continue_;

  result = CoInitializeEx (NULL,
                           (COINIT_MULTITHREADED    |
                            COINIT_DISABLE_OLE1DDE  |
                            COINIT_SPEED_OVER_MEMORY));
  if (FAILED (result))
  { // *NOTE*: most probable reason: RPC_E_CHANGED_MODE
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to CoInitializeEx(): \"%s\", continuing\n"),
                ACE_TEXT (Common_Error_Tools::errorToString (result).c_str ())));
  } // end IF
  else
    is_COM_initialized = true;

continue_:
  Stream_MediaFramework_Tools::initialize (STREAM_MEDIAFRAMEWORK_DIRECTSHOW);
  Stream_Device_DirectShow_Tools::initialize (coInitialize_in);

  if (!mediaType_out)
  {
    mediaType_out =
      static_cast<struct _AMMediaType*> (CoTaskMemAlloc (sizeof (struct _AMMediaType)));
    if (!mediaType_out)
    {
      ACE_DEBUG ((LM_CRITICAL,
                  ACE_TEXT ("failed to allocate memory, aborting\n")));
      goto error;
    } // end IF
    ACE_OS::memset (mediaType_out, 0, sizeof (struct _AMMediaType));
  } // end IF
  if (!mediaType_out->pbFormat)
  {
    mediaType_out->pbFormat =
      static_cast<BYTE*> (CoTaskMemAlloc (sizeof (struct tagVIDEOINFOHEADER)));
    if (!mediaType_out->pbFormat)
    {
      ACE_DEBUG ((LM_CRITICAL,
                  ACE_TEXT ("failed to CoTaskMemAlloc(%u): \"%m\", aborting\n"),
                  sizeof (struct tagVIDEOINFOHEADER)));
      goto error;
    } // end IF
    ACE_OS::memset (mediaType_out->pbFormat, 0, sizeof (struct tagVIDEOINFOHEADER));
  } // end IF

  struct tagVIDEOINFOHEADER* video_info_header_p =
    reinterpret_cast<struct tagVIDEOINFOHEADER*> (mediaType_out->pbFormat);
  ACE_ASSERT (video_info_header_p);

  // *NOTE*: empty --> use entire video
  BOOL result_2 = SetRectEmpty (&video_info_header_p->rcSource);
  ACE_ASSERT (result_2);
  // *NOTE*: empty --> fill entire buffer
  result_2 = SetRectEmpty (&video_info_header_p->rcTarget);
  ACE_ASSERT (result_2);

  // *NOTE*: 320x240 * 4 * 30 * 8
  // *TODO*: this formula applies to RGB format(s) only
  video_info_header_p->dwBitRate = 73728000;
  //video_info_header_p->dwBitErrorRate = 0;
  video_info_header_p->AvgTimePerFrame =
    MILLISECONDS_TO_100NS_UNITS (1000 / 30); // --> 30 fps

  // *TODO*: make this configurable (and part of a protocol)
  video_info_header_p->bmiHeader.biSize = sizeof (struct tagBITMAPINFOHEADER);
  video_info_header_p->bmiHeader.biWidth = 320;
  video_info_header_p->bmiHeader.biHeight = 240;
  video_info_header_p->bmiHeader.biPlanes = 1;
  video_info_header_p->bmiHeader.biBitCount = 32;
  video_info_header_p->bmiHeader.biCompression = BI_RGB;
  video_info_header_p->bmiHeader.biSizeImage =
    GetBitmapSize (&video_info_header_p->bmiHeader);
  //video_info_header_p->bmiHeader.biXPelsPerMeter;
  //video_info_header_p->bmiHeader.biYPelsPerMeter;
  //video_info_header_p->bmiHeader.biClrUsed;
  //video_info_header_p->bmiHeader.biClrImportant;

  mediaType_out->majortype = MEDIATYPE_Video;
  // work out the GUID for the subtype from the header info
  // *TODO*: cannot use GetBitmapSubtype(), as it returns MEDIASUBTYPE_RGB32
  //         for uncompressed RGB (the Color Space Converter expects
  //         MEDIASUBTYPE_ARGB32)
  //struct _GUID SubTypeGUID = MEDIASUBTYPE_RGB24;
  //struct _GUID SubTypeGUID = GetBitmapSubtype (&video_info_header_p->bmiHeader);
  //if (SubTypeGUID == GUID_NULL)
  //{
  //  ACE_DEBUG ((LM_WARNING,
  //              ACE_TEXT ("failed to GetBitmapSubtype(), falling back\n")));
  //  //SubTypeGUID = MEDIASUBTYPE_Avi; // fallback
  //  SubTypeGUID = MEDIASUBTYPE_RGB24; // fallback
  //} // end IF
  mediaType_out->subtype = MEDIASUBTYPE_RGB32;
  mediaType_out->bFixedSizeSamples = TRUE;
  mediaType_out->bTemporalCompression = FALSE;
  mediaType_out->lSampleSize = video_info_header_p->bmiHeader.biSizeImage;
  mediaType_out->formattype = FORMAT_VideoInfo;
  mediaType_out->cbFormat = sizeof (struct tagVIDEOINFOHEADER);

  return true;

error:
  //if (media_filter_p)
  //  media_filter_p->Release ();
  if (mediaType_out)
    Stream_MediaFramework_DirectShow_Tools::delete_ (mediaType_out);

  if (is_COM_initialized)
    CoUninitialize ();

  return false;
}
bool
do_initialize_mediafoundation (IMFMediaType*& mediaType_inout,
                               bool coInitialize_in)
{
  STREAM_TRACE (ACE_TEXT ("::do_initialize_mediafoundation"));

  HRESULT result = E_FAIL;

  struct _GUID subTypeGUID = GUID_NULL;

  // sanity check(s)
  if (mediaType_inout)
  {
    mediaType_inout->Release (); mediaType_inout = NULL;
  } // end IF

  if (!coInitialize_in)
    goto continue_;

  result = CoInitializeEx (NULL,
                           (COINIT_MULTITHREADED    |
                            COINIT_DISABLE_OLE1DDE  |
                            COINIT_SPEED_OVER_MEMORY));
  if (FAILED (result))
  { // *NOTE*: most probable reason: RPC_E_CHANGED_MODE
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to CoInitializeEx(): \"%s\", continuing\n"),
                ACE_TEXT (Common_Error_Tools::errorToString (result).c_str ())));
  } // end IF

  result = MFStartup (MF_VERSION,
                      MFSTARTUP_LITE);
  if (FAILED (result))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to MFStartup(): \"%s\", continuing\n"),
                ACE_TEXT (Common_Error_Tools::errorToString (result).c_str ())));
    goto error;
  } // end IF

continue_:
  Stream_Device_Tools::initialize ();

  //// work out the GUID for the subtype from the header info
  //// *TODO*: cannot use GetBitmapSubtype(), as it returns MEDIASUBTYPE_RGB32
  ////         for uncompressed RGB (the Color Space Converter expects
  ////         MEDIASUBTYPE_ARGB32)
  subTypeGUID = MFVideoFormat_ARGB32;
  if (InlineIsEqualGUID (subTypeGUID, GUID_NULL))
  {
    ACE_DEBUG ((LM_WARNING,
                ACE_TEXT ("failed to GetBitmapSubtype(), falling back\n")));
    //subTypeGUID = MEDIASUBTYPE_Avi; // fallback
    subTypeGUID = MFVideoFormat_RGB24; // fallback
  } // end IF

  HRESULT result_2 = MFCreateMediaType (&mediaType_inout);
  if (FAILED (result_2))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to MFCreateMediaType(): \"%s\", continuing\n"),
                ACE_TEXT (Common_Error_Tools::errorToString (result_2).c_str ())));
    return false;
  } // end IF
  ACE_ASSERT (mediaType_inout);

  result_2 = mediaType_inout->SetGUID (MF_MT_MAJOR_TYPE, MFMediaType_Video);
  ACE_ASSERT (SUCCEEDED (result_2));
  result_2 = mediaType_inout->SetGUID (MF_MT_SUBTYPE, subTypeGUID);
  ACE_ASSERT (SUCCEEDED (result_2));
  result_2 = mediaType_inout->SetUINT32 (MF_MT_DEFAULT_STRIDE, 320 * 4);
  ACE_ASSERT (SUCCEEDED (result_2));
  result_2 = MFSetAttributeSize (mediaType_inout,
                                 MF_MT_FRAME_RATE,
                                 30, 1);
  ACE_ASSERT (SUCCEEDED (result_2));
  //result_2 = mediaType_inout->SetUINT32 (MF_MT_AVG_BITRATE, 10000000);
  //ACE_ASSERT (SUCCEEDED (result_2));
  result_2 = MFSetAttributeSize (mediaType_inout,
                                 MF_MT_FRAME_SIZE,
                                 320, 240);
  ACE_ASSERT (SUCCEEDED (result_2));
  //result_2 = MFSetAttributeSize (mediaType_inout,
  //                               MF_MT_FRAME_RATE_RANGE_MAX,
  //                               30, 1);
  //ACE_ASSERT (SUCCEEDED (result_2));
  //result_2 = MFSetAttributeSize (mediaType_inout,
  //                               MF_MT_FRAME_RATE_RANGE_MIN,
  //                               15, 1);
  //ACE_ASSERT (SUCCEEDED (result_2));

  result_2 = mediaType_inout->SetUINT32 (MF_MT_INTERLACE_MODE,
                                         MFVideoInterlace_Progressive);
  ACE_ASSERT (SUCCEEDED (result_2));
  result_2 = mediaType_inout->SetUINT32 (MF_MT_ALL_SAMPLES_INDEPENDENT,
                                         1);
  ACE_ASSERT (SUCCEEDED (result_2));
  result_2 = MFSetAttributeRatio (mediaType_inout,
                                  MF_MT_PIXEL_ASPECT_RATIO,
                                  1, 1);
  ACE_ASSERT (SUCCEEDED (result_2));
  //result_2 = mediaType_inout->SetUINT32 (MF_MT_FIXED_SIZE_SAMPLES,
  //                                       1);
  //ACE_ASSERT (SUCCEEDED (result_2));
  //UINT32 frame_size = 0;
  //result_2 = MFCalculateImageSize (SubTypeGUID,
  //                                 320, 240,
  //                                 &frame_size);
  //ACE_ASSERT (SUCCEEDED (result_2));
  //result_2 = mediaType_inout->SetUINT32 (MF_MT_SAMPLE_SIZE,
  //                                       frame_size);
  //ACE_ASSERT (SUCCEEDED (result_2));
  //result_2 = mediaType_inout->SetUINT32 (MF_MT_MPEG2_PROFILE,
  //                                       eAVEncH264VProfile_Main);
  //ACE_ASSERT (SUCCEEDED (result_2));
  //result_2 = mediaType_inout->SetUINT32 (CODECAPI_AVEncCommonRateControlMode,
  //                                       eAVEncCommonRateControlMode_Quality);
  //ACE_ASSERT (SUCCEEDED (result_2));
  //result_2 = mediaType_inout->SetUINT32 (CODECAPI_AVEncCommonQuality,
  //                                       80);

  return true;

error:
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
do_finalize_directshow (struct Test_I_Target_DirectShow_UI_CBData& CBData_in)
{
  STREAM_TRACE (ACE_TEXT ("::do_finalize_directshow"));

  Test_I_Target_DirectShow_StreamConfiguration_t::ITERATOR_T iterator =
    CBData_in.configuration->streamConfiguration.find (ACE_TEXT_ALWAYS_CHAR (""));
  ACE_ASSERT (iterator != CBData_in.configuration->streamConfiguration.end ());

  if ((*iterator).second.second.builder)
  {
    (*iterator).second.second.builder->Release ();
    (*iterator).second.second.builder = NULL;
  } // end IF

  CoUninitialize ();
}
void
do_finalize_mediafoundation (struct Test_I_Target_MediaFoundation_UI_CBData& CBData_in)
{
  STREAM_TRACE (ACE_TEXT ("::do_finalize_mediafoundation"));

  Test_I_Target_MediaFoundation_StreamConfiguration_t::ITERATOR_T iterator =
    CBData_in.configuration->streamConfiguration.find (ACE_TEXT_ALWAYS_CHAR (""));
  ACE_ASSERT (iterator != CBData_in.configuration->streamConfiguration.end ());

  if ((*iterator).second.second.mediaSource)
  {
    (*iterator).second.second.mediaSource->Release (); (*iterator).second.second.mediaSource = NULL;
  } // end IF
  //if ((*iterator).second.sourceReader)
  //{
  //  (*iterator).second.sourceReader->Release (); (*iterator).second.sourceReader = NULL;
  //} // end IF
  if ((*iterator).second.second.session)
  {
    (*iterator).second.second.session->Release (); (*iterator).second.second.session = NULL;
  } // end IF

  HRESULT result = MFShutdown ();
  if (FAILED (result))
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to MFShutdown(): \"%s\", continuing\n"),
                ACE_TEXT (Common_Error_Tools::errorToString (result).c_str ())));

  CoUninitialize ();
}
#endif // ACE_WIN32 || ACE_WIN64

void
do_work (unsigned int bufferSize_in,
         unsigned int maximumNumberOfConnections_in,
         const std::string& fileName_in,
         const std::string& UIDefinitionFilename_in,
         bool useThreadPool_in,
#if defined (ACE_WIN32) || defined (ACE_WIN64)
         enum Stream_MediaFramework_Type mediaFramework_in,
#endif // ACE_WIN32 || ACE_WIN64
         const std::string& networkInterface_in,
         bool useLoopBack_in,
         unsigned short listeningPortNumber_in,
         bool useReactor_in,
         unsigned int statisticReportingInterval_in,
         bool useUDP_in,
         unsigned int numberOfDispatchThreads_in,
         unsigned int frameSize_in,
#if defined (ACE_WIN32) || defined (ACE_WIN64)
         struct Test_I_Target_MediaFoundation_UI_CBData& mediaFoundationCBData_in,
         struct Test_I_Target_DirectShow_UI_CBData& directShowCBData_in,
#else
         struct Test_I_Target_UI_CBData& CBData_in,
#endif // ACE_WIN32 || ACE_WIN64
         const ACE_Sig_Set& signalSet_in,
         const ACE_Sig_Set& ignoredSignalSet_in,
         Common_SignalActions_t& previousSignalActions_inout,
         const sigset_t& previousSignalMask_in,
#if defined (ACE_WIN32) || defined (ACE_WIN64)
         Test_I_Target_DirectShow_SignalHandler_t& directShowSignalHandler_in,
         Test_I_Target_MediaFoundation_SignalHandler_t& mediaFoundationSignalHandler_in)
#else
         Test_I_Target_SignalHandler_t& signalHandler_in)
#endif // ACE_WIN32 || ACE_WIN64
{
  STREAM_TRACE (ACE_TEXT ("::do_work"));

  ACE_UNUSED_ARG (networkInterface_in);

  int result = -1;

  // step0a: initialize event dispatch
  struct Common_EventDispatchConfiguration event_dispatch_configuration_s;
  bool serialize_output = false;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  struct Test_I_Target_DirectShow_Configuration directshow_configuration;
  struct Test_I_Target_MediaFoundation_Configuration mediafoundation_configuration;
  switch (mediaFramework_in)
  {
    case STREAM_MEDIAFRAMEWORK_DIRECTSHOW:
    {
      directShowCBData_in.configuration = &directshow_configuration;
      serialize_output =
        directshow_configuration.streamConfiguration.configuration_.serializeOutput;
      break;
    }
    case STREAM_MEDIAFRAMEWORK_MEDIAFOUNDATION:
    {
      mediaFoundationCBData_in.configuration = &mediafoundation_configuration;
      serialize_output =
        mediafoundation_configuration.streamConfiguration.configuration_.serializeOutput;
      break;
    } // end IF
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown media framework (was: %d), returning\n"),
                  mediaFramework_in));
      return;
    } // end ELSE
  } // end SWITCH
#else
  struct Test_I_Target_Configuration configuration;
  CBData_in.configuration = &configuration;
  serialize_output =
    configuration.streamConfiguration.configuration_.serializeOutput;
#endif // ACE_WIN32 || ACE_WIN64
  ACE_UNUSED_ARG (serialize_output);
  event_dispatch_configuration_s.numberOfProactorThreads =
          (!useReactor_in ? numberOfDispatchThreads_in : 0);
  event_dispatch_configuration_s.numberOfReactorThreads =
          (useReactor_in ? numberOfDispatchThreads_in : 0);
  if (!Common_Tools::initializeEventDispatch (event_dispatch_configuration_s))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_Tools::initializeEventDispatch(), returning\n")));
    return;
  } // end IF
  struct Common_EventDispatchState event_dispatch_state_s;
  event_dispatch_state_s.configuration =
      &event_dispatch_configuration_s;

  // step0b: initialize configuration and stream
  struct Test_I_CamStream_Configuration* camstream_configuration_p = NULL;
  struct Test_I_AllocatorConfiguration* allocator_configuration_p =
    NULL;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  switch (mediaFramework_in)
  {
    case STREAM_MEDIAFRAMEWORK_DIRECTSHOW:
    {
      camstream_configuration_p = &directshow_configuration;
      directShowCBData_in.configuration = &directshow_configuration;
      allocator_configuration_p =
        &directshow_configuration.streamConfiguration.allocatorConfiguration_;
      break;
    }
    case STREAM_MEDIAFRAMEWORK_MEDIAFOUNDATION:
    {
      camstream_configuration_p = &mediafoundation_configuration;
      mediaFoundationCBData_in.configuration = &mediafoundation_configuration;
      allocator_configuration_p =
        &mediafoundation_configuration.streamConfiguration.allocatorConfiguration_;
      break;
    } // end IF
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown media framework (was: %d), returning\n"),
                  mediaFramework_in));
      return;
    } // end ELSE
  } // end SWITCH
#else
  camstream_configuration_p = &configuration;
  CBData_in.configuration = &configuration;
  allocator_configuration_p =
    &configuration.streamConfiguration.allocatorConfiguration_;
#endif // ACE_WIN32 || ACE_WIN64
  ACE_ASSERT (camstream_configuration_p);
  if (useReactor_in)
    camstream_configuration_p->dispatchConfiguration.numberOfReactorThreads =
      numberOfDispatchThreads_in;
  else
    camstream_configuration_p->dispatchConfiguration.numberOfProactorThreads =
      numberOfDispatchThreads_in;

  camstream_configuration_p->protocol = (useUDP_in ? NET_TRANSPORTLAYER_UDP
                                                   : NET_TRANSPORTLAYER_TCP);

  // ********************** module configuration data **************************
  struct Stream_ModuleConfiguration module_configuration;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  Test_I_Target_MediaFoundation_EventHandler_t mediafoundation_ui_event_handler (&mediaFoundationCBData_in);
  Test_I_Target_MediaFoundation_StreamConfiguration_t::ITERATOR_T mediafoundation_modulehandler_iterator;
  Test_I_Target_DirectShow_EventHandler_t directshow_ui_event_handler (&directShowCBData_in);
  Test_I_Target_DirectShow_StreamConfiguration_t::ITERATOR_T directshow_modulehandler_iterator;
  switch (mediaFramework_in)
  {
    case STREAM_MEDIAFRAMEWORK_DIRECTSHOW:
    {
      struct Test_I_Target_DirectShow_ModuleHandlerConfiguration modulehandler_configuration;
      modulehandler_configuration.configuration = &directshow_configuration;
      modulehandler_configuration.connectionConfigurations =
        &directshow_configuration.connectionConfigurations;

      modulehandler_configuration.filterConfiguration =
        &directshow_configuration.filterConfiguration;
      // *TODO*: specify the preferred media type
      //modulehandler_configuration.format = ;
      //modulehandler_configuration.format->lSampleSize = frameSize_in;

      //modulehandler_configuration.connectionManager = connection_manager_p;
      modulehandler_configuration.printProgressDot =
        UIDefinitionFilename_in.empty ();
      modulehandler_configuration.statisticReportingInterval =
          ACE_Time_Value (statisticReportingInterval_in, 0);
      modulehandler_configuration.streamConfiguration =
        &directshow_configuration.streamConfiguration;
      modulehandler_configuration.subscriber =
        &directshow_ui_event_handler;
      modulehandler_configuration.targetFileName = fileName_in;

      directshow_configuration.streamConfiguration.initialize (module_configuration,
                                                               modulehandler_configuration,
                                                               directshow_configuration.streamConfiguration.allocatorConfiguration_,
                                                               directshow_configuration.streamConfiguration.configuration_);
      directshow_modulehandler_iterator =
        directshow_configuration.streamConfiguration.find (ACE_TEXT_ALWAYS_CHAR (""));
      ACE_ASSERT (directshow_modulehandler_iterator != directshow_configuration.streamConfiguration.end ());
      break;
    }
    case STREAM_MEDIAFRAMEWORK_MEDIAFOUNDATION:
    {
      struct Test_I_Target_MediaFoundation_ModuleHandlerConfiguration modulehandler_configuration;
      modulehandler_configuration.configuration = &mediafoundation_configuration;
      modulehandler_configuration.connectionConfigurations =
        &mediafoundation_configuration.connectionConfigurations;

      //modulehandler_configuration.connectionManager = connection_manager_p;
      //result =
      //  modulehandler_configuration.format->SetUINT32 (MF_MT_SAMPLE_SIZE,
      //                                                 frameSize_in);
      //if (FAILED (result))
      //{
      //  ACE_DEBUG ((LM_ERROR,
      //              ACE_TEXT ("failed to IMFMediaType::SetUINT32(MF_MT_SAMPLE_SIZE,%u): \"%s\", returning\n"),
      //              frameSize_in,
      //              ACE_TEXT (Common_Error_Tools::errorToString (result).c_str ())));
      //  goto clean;
      //} // end IF
      modulehandler_configuration.printProgressDot =
        UIDefinitionFilename_in.empty ();
      modulehandler_configuration.statisticReportingInterval =
          ACE_Time_Value (statisticReportingInterval_in, 0);
      modulehandler_configuration.streamConfiguration =
        &mediafoundation_configuration.streamConfiguration;
      modulehandler_configuration.subscriber =
        &mediafoundation_ui_event_handler;
      modulehandler_configuration.targetFileName = fileName_in;

      mediafoundation_configuration.streamConfiguration.insert (std::make_pair (ACE_TEXT_ALWAYS_CHAR (""),
                                                                                std::make_pair (module_configuration,
                                                                                                modulehandler_configuration)));
      mediafoundation_modulehandler_iterator =
        mediafoundation_configuration.streamConfiguration.find (ACE_TEXT_ALWAYS_CHAR (""));
      ACE_ASSERT (mediafoundation_modulehandler_iterator != mediafoundation_configuration.streamConfiguration.end ());

      mediafoundation_configuration.streamConfiguration.initialize (module_configuration,
                                                                    (*mediafoundation_modulehandler_iterator).second.second,
                                                                    mediafoundation_configuration.streamConfiguration.allocatorConfiguration_,
                                                                    mediafoundation_configuration.streamConfiguration.configuration_);
      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown media framework (was: %d), returning\n"),
                  mediaFramework_in));
      return;
    } // end ELSE
  } // end SWITCH
#else
  struct Test_I_Target_ModuleHandlerConfiguration modulehandler_configuration;
  modulehandler_configuration.configuration = &configuration;
  modulehandler_configuration.connectionConfigurations =
    &configuration.connectionConfigurations;

  //modulehandler_configuration.connectionManager = connection_manager_p;
  //modulehandler_configuration.format.type =
  //  V4L2_BUF_TYPE_VIDEO_CAPTURE;
  //modulehandler_configuration.format.fmt.pix.bytesperline = 960;
  ////modulehandler_configuration.format.fmt.pix.field =
  ////      V4L2_FIELD_NONE;
  //modulehandler_configuration.format.fmt.pix.height = 240;
  //modulehandler_configuration.format.fmt.pix.pixelformat =
  //  V4L2_PIX_FMT_BGR24;
  //modulehandler_configuration.format.fmt.pix.sizeimage = 230400;
  //modulehandler_configuration.format.fmt.pix.width = 320;
  modulehandler_configuration.inbound = true;
  modulehandler_configuration.printProgressDot =
    UIDefinitionFilename_in.empty ();
  modulehandler_configuration.statisticReportingInterval =
    ACE_Time_Value (statisticReportingInterval_in, 0);
  modulehandler_configuration.streamConfiguration =
    &configuration.streamConfiguration;
//  modulehandler_configuration.subscriber = &ui_event_handler;
  modulehandler_configuration.targetFileName = fileName_in;

  configuration.streamConfiguration.insert (std::make_pair (ACE_TEXT_ALWAYS_CHAR (""),
                                                            std::make_pair (module_configuration,
                                                                            modulehandler_configuration)));
  Test_I_Target_StreamConfiguration_t::ITERATOR_T iterator =
      configuration.streamConfiguration.find (ACE_TEXT_ALWAYS_CHAR (""));
  ACE_ASSERT (iterator != configuration.streamConfiguration.end ());

  configuration.streamConfiguration.initialize (module_configuration,
                                                (*iterator).second.second,
                                                configuration.streamConfiguration.allocatorConfiguration_,
                                                configuration.streamConfiguration.configuration_);
#endif // ACE_WIN32 || ACE_WIN64

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  // *NOTE*: in UI mode, COM has already been initialized for this thread
  // *TODO*: where has that happened ?
  switch (mediaFramework_in)
  {
    case STREAM_MEDIAFRAMEWORK_DIRECTSHOW:
    {
      directshow_modulehandler_iterator =
        directShowCBData_in.configuration->streamConfiguration.find (ACE_TEXT_ALWAYS_CHAR (""));
      ACE_ASSERT (directshow_modulehandler_iterator != directShowCBData_in.configuration->streamConfiguration.end ());
      result =
        do_initialize_directshow ((*directshow_modulehandler_iterator).second.second.inputFormat,
          UIDefinitionFilename_in.empty ()); // initialize COM ?
      ACE_ASSERT ((*directshow_modulehandler_iterator).second.second.inputFormat);
      break;
    }
    case STREAM_MEDIAFRAMEWORK_MEDIAFOUNDATION:
    {
      mediafoundation_modulehandler_iterator =
        mediaFoundationCBData_in.configuration->streamConfiguration.find (ACE_TEXT_ALWAYS_CHAR (""));
      ACE_ASSERT (mediafoundation_modulehandler_iterator != mediaFoundationCBData_in.configuration->streamConfiguration.end ());
      result =
        do_initialize_mediafoundation ((*mediafoundation_modulehandler_iterator).second.second.inputFormat,
          UIDefinitionFilename_in.empty ()); // initialize COM ?
      break;
    } // end IF
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown media framework (was: %d), returning\n"),
                  mediaFramework_in));
      return;
    } // end ELSE
  } // end SWITCH
  if (!result)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to intialize media framework, returning\n")));
    return;
  } // end IF
#endif // ACE_WIN32 || ACE_WIN64

  ACE_ASSERT (allocator_configuration_p);
  Stream_AllocatorHeap_T<ACE_MT_SYNCH,
                         struct Test_I_AllocatorConfiguration> heap_allocator;
  if (!heap_allocator.initialize (*allocator_configuration_p))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to intialize heap allocator, returning\n")));
    return;
  } // end IF
  Stream_IAllocator* allocator_p = NULL;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  Test_I_Target_DirectShow_MessageAllocator_t directshow_message_allocator (TEST_I_MAX_MESSAGES, // maximum #buffers
                                                                            &heap_allocator,     // heap allocator handle
                                                                            true);               // block ?
  Test_I_Target_MediaFoundation_MessageAllocator_t mediafoundation_message_allocator (TEST_I_MAX_MESSAGES, // maximum #buffers
                                                                                      &heap_allocator,     // heap allocator handle
                                                                                      true);               // block ?
  switch (mediaFramework_in)
  {
    case STREAM_MEDIAFRAMEWORK_DIRECTSHOW:
    {
      allocator_p = &directshow_message_allocator;
      break;
    }
    case STREAM_MEDIAFRAMEWORK_MEDIAFOUNDATION:
    {
      allocator_p = &mediafoundation_message_allocator;
      break;
    } // end IF
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown media framework (was: %d), returning\n"),
                  mediaFramework_in));
      return;
    } // end ELSE
  } // end SWITCH
#else
  Test_I_Target_MessageAllocator_t message_allocator (TEST_I_MAX_MESSAGES, // maximum #buffers
                                                      &heap_allocator,     // heap allocator handle
                                                      true);               // block ?
  allocator_p = &message_allocator;
#endif // ACE_WIN32 || ACE_WIN64
  ACE_ASSERT (allocator_p);

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  Test_I_Target_DirectShow_EventHandler_Module directshow_event_handler (NULL,
                                                                         ACE_TEXT_ALWAYS_CHAR (MODULE_MISC_MESSAGEHANDLER_DEFAULT_NAME_STRING));
  Test_I_Target_MediaFoundation_EventHandler_Module mediafoundation_event_handler (NULL,
                                                                                   ACE_TEXT_ALWAYS_CHAR (MODULE_MISC_MESSAGEHANDLER_DEFAULT_NAME_STRING));
#else
  Test_I_Target_EventHandler_t ui_event_handler (&CBData_in);
  Test_I_Target_Module_EventHandler_Module event_handler (NULL,
                                                          ACE_TEXT_ALWAYS_CHAR (MODULE_MISC_MESSAGEHANDLER_DEFAULT_NAME_STRING));
#endif // ACE_WIN32 || ACE_WIN64

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  Test_I_Target_DirectShow_EventHandler* directshow_event_handler_p =
    NULL;
  Test_I_Target_MediaFoundation_EventHandler* mediafoundation_event_handler_p =
    NULL;
#else
  Test_I_Target_Module_EventHandler* event_handler_p = NULL;
#endif // ACE_WIN32 || ACE_WIN64
  struct Common_TimerConfiguration timer_configuration;
  timer_configuration.dispatch =
    (useReactor_in ? COMMON_TIMER_DISPATCH_REACTOR
                   : COMMON_TIMER_DISPATCH_PROACTOR);
  Common_Timer_Manager_t* timer_manager_p =
        COMMON_TIMERMANAGER_SINGLETON::instance ();
  ACE_ASSERT (timer_manager_p);
  long timer_id = -1;
  int group_id = -1;
  Net_IConnectionManagerBase_t* iconnection_manager_p = NULL;
  Test_I_StatisticReportingHandler_t* report_handler_p = NULL;
  bool result_2 = false;
#if defined (GUI_SUPPORT)
#if defined (GTK_SUPPORT)
  Common_UI_GTK_Manager_t* gtk_manager_p = NULL;
#endif // GTK_SUPPORT
#endif // GUI_SUPPORT
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  Test_I_Target_MediaFoundation_ConnectionConfiguration_t mediafoundation_connection_configuration;
  Test_I_Target_DirectShow_ConnectionConfiguration_t directshow_connection_configuration;
  Test_I_Target_MediaFoundation_InetConnectionManager_t* mediafoundation_connection_manager_p =
    NULL;
  Test_I_Target_DirectShow_InetConnectionManager_t* directshow_connection_manager_p =
    NULL;
  Test_I_Target_MediaFoundation_ConnectionConfigurationIterator_t mediafoundation_connection_configuration_iterator;
  Test_I_Target_DirectShow_ConnectionConfigurationIterator_t directshow_connection_configuration_iterator;
  switch (mediaFramework_in)
  {
    case STREAM_MEDIAFRAMEWORK_DIRECTSHOW:
    {
      directshow_configuration.connectionConfigurations.insert (std::make_pair (ACE_TEXT_ALWAYS_CHAR (""),
                                                                                directshow_connection_configuration));
      directshow_connection_configuration_iterator =
        directshow_configuration.connectionConfigurations.find (ACE_TEXT_ALWAYS_CHAR (""));
      ACE_ASSERT (directshow_connection_configuration_iterator != directshow_configuration.connectionConfigurations.end ());
      (*directshow_connection_configuration_iterator).second.socketHandlerConfiguration.connectionConfiguration =
        &((*directshow_connection_configuration_iterator).second);

      directshow_connection_manager_p =
        TEST_I_TARGET_DIRECTSHOW_CONNECTIONMANAGER_SINGLETON::instance ();
      directshow_connection_manager_p->initialize (maximumNumberOfConnections_in ? maximumNumberOfConnections_in
                                                                                 : std::numeric_limits<unsigned int>::max ());
      (*directshow_modulehandler_iterator).second.second.connectionManager =
        directshow_connection_manager_p;
      iconnection_manager_p = directshow_connection_manager_p;
      report_handler_p = directshow_connection_manager_p;
      break;
    }
    case STREAM_MEDIAFRAMEWORK_MEDIAFOUNDATION:
    {
      mediafoundation_configuration.connectionConfigurations.insert (std::make_pair (ACE_TEXT_ALWAYS_CHAR (""),
                                                                                     mediafoundation_connection_configuration));
      mediafoundation_connection_configuration_iterator =
        mediafoundation_configuration.connectionConfigurations.find (ACE_TEXT_ALWAYS_CHAR (""));
      ACE_ASSERT (mediafoundation_connection_configuration_iterator != mediafoundation_configuration.connectionConfigurations.end ());
      (*mediafoundation_connection_configuration_iterator).second.socketHandlerConfiguration.connectionConfiguration =
        &((*mediafoundation_connection_configuration_iterator).second);

      mediafoundation_connection_manager_p =
        TEST_I_TARGET_MEDIAFOUNDATION_CONNECTIONMANAGER_SINGLETON::instance ();
      mediafoundation_connection_manager_p->initialize (maximumNumberOfConnections_in ? maximumNumberOfConnections_in
                                                                                      : std::numeric_limits<unsigned int>::max ());
      (*mediafoundation_modulehandler_iterator).second.second.connectionManager =
        mediafoundation_connection_manager_p;
      iconnection_manager_p = mediafoundation_connection_manager_p;
      report_handler_p = mediafoundation_connection_manager_p;
      break;
    } // end IF
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown media framework (was: %d), returning\n"),
                  mediaFramework_in));
      return;
    } // end ELSE
  } // end SWITCH
#else
  Test_I_Target_ConnectionConfiguration_t connection_configuration;
  Test_I_Target_InetConnectionManager_t* connection_manager_p =
    TEST_I_TARGET_CONNECTIONMANAGER_SINGLETON::instance ();
  connection_manager_p->initialize (maximumNumberOfConnections_in ? maximumNumberOfConnections_in
                                                                  : std::numeric_limits<unsigned int>::max ());
  iconnection_manager_p = connection_manager_p;
  report_handler_p = connection_manager_p;
  (*iterator).second.second.connectionManager = connection_manager_p;
#endif // ACE_WIN32 || ACE_WIN64
  ACE_ASSERT (iconnection_manager_p);
  ACE_ASSERT (report_handler_p);
  Test_I_StatisticHandler_t statistic_handler (COMMON_STATISTIC_ACTION_REPORT,
                                               report_handler_p,
                                               false);
  ACE_Event_Handler* event_handler_2 = NULL;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  switch (mediaFramework_in)
  {
    case STREAM_MEDIAFRAMEWORK_DIRECTSHOW:
    {
      directshow_event_handler_p =
        dynamic_cast<Test_I_Target_DirectShow_EventHandler*> (directshow_event_handler.writer ());
      break;
    }
    case STREAM_MEDIAFRAMEWORK_MEDIAFOUNDATION:
    {
      mediafoundation_event_handler_p =
        dynamic_cast<Test_I_Target_MediaFoundation_EventHandler*> (mediafoundation_event_handler.writer ());
      break;
    } // end IF
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown media framework (was: %d), returning\n"),
                  mediaFramework_in));
      return;
    } // end ELSE
  } // end SWITCH
  ACE_ASSERT (mediafoundation_event_handler_p || directshow_event_handler_p);
#else
  Test_I_Target_ConnectionConfigurationIterator_t iterator_2;
  event_handler_p =
    dynamic_cast<Test_I_Target_Module_EventHandler*> (event_handler.writer ());
  if (!event_handler_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to dynamic_cast<Test_I_Target_Module_EventHandler>, returning\n")));
    goto clean;
  } // end IF
#endif // ACE_WIN32 || ACE_WIN64

  // ********************** socket configuration data **************************
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  switch (mediaFramework_in)
  {
    case STREAM_MEDIAFRAMEWORK_DIRECTSHOW:
    {
      (*directshow_connection_configuration_iterator).second.socketHandlerConfiguration.socketConfiguration_2.address.set_port_number (listeningPortNumber_in,
                                                                                                                                       1);
      (*directshow_connection_configuration_iterator).second.socketHandlerConfiguration.socketConfiguration_2.bufferSize =
        bufferSize_in;
      (*directshow_connection_configuration_iterator).second.socketHandlerConfiguration.socketConfiguration_2.useLoopBackDevice =
        useLoopBack_in;
      if ((*directshow_connection_configuration_iterator).second.socketHandlerConfiguration.socketConfiguration_2.useLoopBackDevice)
      {
        result =
          (*directshow_connection_configuration_iterator).second.socketHandlerConfiguration.socketConfiguration_2.address.set (listeningPortNumber_in,
                                                                                                                               INADDR_LOOPBACK,
                                                                                                                               1,
                                                                                                                               0);
        if (result == -1)
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("failed to ACE_INET_Addr::set(): \"%m\", continuing\n")));
      } // end IF
      if (bufferSize_in)
        (*directshow_connection_configuration_iterator).second.PDUSize =
          bufferSize_in;
      (*directshow_connection_configuration_iterator).second.socketHandlerConfiguration.statisticReportingInterval =
        statisticReportingInterval_in;
      (*directshow_connection_configuration_iterator).second.socketHandlerConfiguration.userData =
        &directshow_configuration.userData;
      (*directshow_connection_configuration_iterator).second.messageAllocator =
        &directshow_message_allocator;
      (*directshow_connection_configuration_iterator).second.userData =
        &directshow_configuration.userData;
      (*directshow_connection_configuration_iterator).second.initialize (*allocator_configuration_p,
                                                                         directshow_configuration.streamConfiguration);

      directshow_connection_manager_p->set ((*directshow_connection_configuration_iterator).second,
                                            &directshow_configuration.userData);
      break;
    }
    case STREAM_MEDIAFRAMEWORK_MEDIAFOUNDATION:
    {
      (*mediafoundation_connection_configuration_iterator).second.socketHandlerConfiguration.socketConfiguration_2.address.set_port_number (listeningPortNumber_in,
                                                                                                                                            1);
      (*mediafoundation_connection_configuration_iterator).second.socketHandlerConfiguration.socketConfiguration_2.bufferSize =
        bufferSize_in;
      (*mediafoundation_connection_configuration_iterator).second.socketHandlerConfiguration.socketConfiguration_2.useLoopBackDevice =
        useLoopBack_in;
      if ((*mediafoundation_connection_configuration_iterator).second.socketHandlerConfiguration.socketConfiguration_2.useLoopBackDevice)
      {
        result =
          (*mediafoundation_connection_configuration_iterator).second.socketHandlerConfiguration.socketConfiguration_2.address.set (listeningPortNumber_in,
                                                                                                                                    INADDR_LOOPBACK,
                                                                                                                                    1,
                                                                                                                                    0);
        if (result == -1)
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("failed to ACE_INET_Addr::set(): \"%m\", continuing\n")));
      } // end IF
      if (bufferSize_in)
        (*mediafoundation_connection_configuration_iterator).second.PDUSize =
          bufferSize_in;
      (*mediafoundation_connection_configuration_iterator).second.socketHandlerConfiguration.statisticReportingInterval =
        statisticReportingInterval_in;

      (*mediafoundation_connection_configuration_iterator).second.socketHandlerConfiguration.userData =
        &mediafoundation_configuration.userData;
      (*mediafoundation_connection_configuration_iterator).second.messageAllocator =
        &mediafoundation_message_allocator;
      (*mediafoundation_connection_configuration_iterator).second.userData =
        &mediafoundation_configuration.userData;
      (*mediafoundation_connection_configuration_iterator).second.initialize (*allocator_configuration_p,
                                                                              mediafoundation_configuration.streamConfiguration);

      mediafoundation_connection_manager_p->set ((*mediafoundation_connection_configuration_iterator).second,
                                                 &mediafoundation_configuration.userData);
      break;
    } // end IF
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown media framework (was: %d), returning\n"),
                  mediaFramework_in));
      return;
    } // end ELSE
  } // end SWITCH
#else
  connection_configuration.socketHandlerConfiguration.socketConfiguration_2.address.set_port_number (listeningPortNumber_in,
                                                                                                     1);
  connection_configuration.socketHandlerConfiguration.socketConfiguration_2.bufferSize =
    bufferSize_in;
  connection_configuration.socketHandlerConfiguration.socketConfiguration_2.useLoopBackDevice =
    useLoopBack_in;
  if (connection_configuration.socketHandlerConfiguration.socketConfiguration_2.useLoopBackDevice)
  {
    result =
      connection_configuration.socketHandlerConfiguration.socketConfiguration_2.address.set (listeningPortNumber_in,
                                                                                             INADDR_LOOPBACK,
                                                                                             1,
                                                                                             0);
    if (result == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_INET_Addr::set(): \"%m\", continuing\n")));
  } // end IF
  if (bufferSize_in)
    connection_configuration.PDUSize = bufferSize_in;
  connection_configuration.socketHandlerConfiguration.statisticReportingInterval =
    statisticReportingInterval_in;
  connection_configuration.socketHandlerConfiguration.userData =
    &configuration.userData;
  connection_configuration.messageAllocator = &message_allocator;
  connection_configuration.userData = &configuration.userData;
  connection_configuration.initialize (*allocator_configuration_p,
                                       configuration.streamConfiguration);

  configuration.connectionConfigurations.insert (std::make_pair (ACE_TEXT_ALWAYS_CHAR (""),
                                                                 connection_configuration));
  iterator_2 =
    configuration.connectionConfigurations.find (ACE_TEXT_ALWAYS_CHAR (""));
  ACE_ASSERT (iterator_2 != configuration.connectionConfigurations.end ());
  (*iterator_2).second.socketHandlerConfiguration.connectionConfiguration =
    &((*iterator_2).second);

  connection_manager_p->set ((*iterator_2).second,
                             &configuration.userData);
#endif // ACE_WIN32 || ACE_WIN64

  // **************************** stream data **********************************
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  // ******************** DirectShow configuration data ************************
  switch (mediaFramework_in)
  {
    case STREAM_MEDIAFRAMEWORK_DIRECTSHOW:
    {
      //directshow_configuration.pinConfiguration.bufferSize = bufferSize_in;
      ACE_ASSERT (!directshow_configuration.pinConfiguration.format);
      directshow_configuration.pinConfiguration.format =
        Stream_MediaFramework_DirectShow_Tools::copy (*(*directshow_modulehandler_iterator).second.second.inputFormat);
      ACE_ASSERT (directshow_configuration.pinConfiguration.format);

      //ACE_ASSERT (!directshow_configuration.filterConfiguration.format);
      //Stream_Device_DirectShow_Tools::copy (*directshow_configuration.moduleHandlerConfiguration.format,
      //                                                      directshow_configuration.filterConfiguration.format);
      //ACE_ASSERT (directshow_configuration.filterConfiguration.format);
      directshow_configuration.filterConfiguration.pinConfiguration =
        &directshow_configuration.pinConfiguration;
      break;
    }
    case STREAM_MEDIAFRAMEWORK_MEDIAFOUNDATION:
      break;
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown media framework (was: %d), returning\n"),
                  mediaFramework_in));
      return;
    } // end ELSE
  } // end SWITCH
#endif // ACE_WIN32 || ACE_WIN64

  // ******************** (sub-)stream configuration data **********************
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  switch (mediaFramework_in)
  {
    case STREAM_MEDIAFRAMEWORK_DIRECTSHOW:
    {
      directshow_configuration.streamConfiguration.allocatorConfiguration_.defaultBufferSize =
        (*directshow_modulehandler_iterator).second.second.inputFormat->lSampleSize;
      if (bufferSize_in)
      { ACE_ASSERT (bufferSize_in >= (*directshow_modulehandler_iterator).second.second.inputFormat->lSampleSize);
        directshow_configuration.streamConfiguration.allocatorConfiguration_.defaultBufferSize =
          bufferSize_in;
      } // end IF
      directshow_configuration.streamConfiguration.configuration_.cloneModule =
        true;
      directshow_configuration.streamConfiguration.configuration_.messageAllocator =
        allocator_p;
      if (!UIDefinitionFilename_in.empty ())
        directshow_configuration.streamConfiguration.configuration_.module =
          &directshow_event_handler;
      directshow_configuration.streamConfiguration.configuration_.printFinalReport =
        true;
      break;
    }
    case STREAM_MEDIAFRAMEWORK_MEDIAFOUNDATION:
    {
      if (bufferSize_in)
        mediafoundation_configuration.streamConfiguration.allocatorConfiguration_.defaultBufferSize =
          bufferSize_in;
      mediafoundation_configuration.streamConfiguration.configuration_.cloneModule =
        true;
      mediafoundation_configuration.streamConfiguration.configuration_.messageAllocator =
        allocator_p;
      if (!UIDefinitionFilename_in.empty ())
        mediafoundation_configuration.streamConfiguration.configuration_.module =
          &mediafoundation_event_handler;
      //mediafoundation_configuration.streamConfiguration.mediaFoundationConfiguration =
      //  &mediafoundation_configuration.mediaFoundationConfiguration;
      mediafoundation_configuration.streamConfiguration.configuration_.printFinalReport =
        true;
      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown media framework (was: %d), returning\n"),
                  mediaFramework_in));
      return;
    } // end ELSE
  } // end SWITCH
#else
  if (bufferSize_in)
    configuration.streamConfiguration.allocatorConfiguration_.defaultBufferSize =
        bufferSize_in;

  configuration.streamConfiguration.configuration_.cloneModule = true;
  configuration.streamConfiguration.configuration_.messageAllocator =
      allocator_p;
  if (!UIDefinitionFilename_in.empty ())
    configuration.streamConfiguration.configuration_.module = &event_handler;
  configuration.streamConfiguration.configuration_.printFinalReport = true;
#endif // ACE_WIN32 || ACE_WIN64

  // ********************* listener configuration data *************************
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  switch (mediaFramework_in)
  {
    case STREAM_MEDIAFRAMEWORK_DIRECTSHOW:
    {
      directshow_configuration.listenerConfiguration.connectionConfiguration =
        &((*directshow_connection_configuration_iterator).second);
      directshow_configuration.listenerConfiguration.connectionManager =
        directshow_connection_manager_p;
      directshow_configuration.listenerConfiguration.statisticReportingInterval =
        statisticReportingInterval_in;
      (*directshow_connection_configuration_iterator).second.socketHandlerConfiguration.socketConfiguration_2.useLoopBackDevice =
        useLoopBack_in;
      break;
    }
    case STREAM_MEDIAFRAMEWORK_MEDIAFOUNDATION:
    {
      mediafoundation_configuration.listenerConfiguration.connectionConfiguration =
        &((*mediafoundation_connection_configuration_iterator).second);
      mediafoundation_configuration.listenerConfiguration.connectionManager =
        mediafoundation_connection_manager_p;
      mediafoundation_configuration.listenerConfiguration.statisticReportingInterval =
        statisticReportingInterval_in;
      (*mediafoundation_connection_configuration_iterator).second.socketHandlerConfiguration.socketConfiguration_2.useLoopBackDevice =
        useLoopBack_in;
      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown media framework (was: %d), returning\n"),
                  mediaFramework_in));
      return;
    } // end ELSE
  } // end SWITCH
#else
  configuration.listenerConfiguration.connectionConfiguration =
      &((*iterator_2).second);
  configuration.listenerConfiguration.connectionManager = connection_manager_p;
  configuration.listenerConfiguration.statisticReportingInterval =
    statisticReportingInterval_in;
  (*iterator_2).second.socketHandlerConfiguration.socketConfiguration_2.useLoopBackDevice =
      useLoopBack_in;
#endif // ACE_WIN32 || ACE_WIN64

  // step0d: initialize regular (global) statistic reporting
  timer_manager_p->initialize (timer_configuration);
  timer_manager_p->start ();
  if (statisticReportingInterval_in)
  {
    ACE_Time_Value interval (statisticReportingInterval_in, 0);
    timer_id =
      timer_manager_p->schedule_timer (&statistic_handler,         // event handler handle
                                       NULL,                       // asynchrnous completion token
                                       COMMON_TIME_NOW + interval, // first wakeup time
                                       interval);                  // interval
    if (timer_id == -1)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to schedule timer: \"%m\", returning\n")));
      timer_manager_p->stop ();
      goto clean;
    } // end IF
  } // end IF

  // step0e: initialize signal handling
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  switch (mediaFramework_in)
  {
    case STREAM_MEDIAFRAMEWORK_DIRECTSHOW:
    {
      directshow_configuration.signalHandlerConfiguration.connectionManager =
        TEST_I_TARGET_DIRECTSHOW_CONNECTIONMANAGER_SINGLETON::instance ();
      directshow_configuration.signalHandlerConfiguration.dispatchState =
        &event_dispatch_state_s;
      directshow_configuration.signalHandlerConfiguration.hasUI =
        !UIDefinitionFilename_in.empty ();
      if (useReactor_in)
        directshow_configuration.signalHandlerConfiguration.listener =
          TEST_I_TARGET_DIRECTSHOW_LISTENER_SINGLETON::instance ();
      else
        directshow_configuration.signalHandlerConfiguration.listener =
          TEST_I_TARGET_DIRECTSHOW_ASYNCHLISTENER_SINGLETON::instance ();
      directshow_configuration.signalHandlerConfiguration.statisticReportingHandler =
        report_handler_p;
      directshow_configuration.signalHandlerConfiguration.statisticReportingTimerId =
        timer_id;
      result =
        directShowSignalHandler_in.initialize (directshow_configuration.signalHandlerConfiguration);
      event_handler_2 = &directShowSignalHandler_in;
      break;
    }
    case STREAM_MEDIAFRAMEWORK_MEDIAFOUNDATION:
    {
      mediafoundation_configuration.signalHandlerConfiguration.connectionManager =
        TEST_I_TARGET_MEDIAFOUNDATION_CONNECTIONMANAGER_SINGLETON::instance ();
      mediafoundation_configuration.signalHandlerConfiguration.dispatchState =
        &event_dispatch_state_s;
      mediafoundation_configuration.signalHandlerConfiguration.hasUI =
        !UIDefinitionFilename_in.empty ();
      if (useReactor_in)
        mediafoundation_configuration.signalHandlerConfiguration.listener =
          TEST_I_TARGET_MEDIAFOUNDATION_LISTENER_SINGLETON::instance ();
      else
        mediafoundation_configuration.signalHandlerConfiguration.listener =
          TEST_I_TARGET_MEDIAFOUNDATION_ASYNCHLISTENER_SINGLETON::instance ();
      mediafoundation_configuration.signalHandlerConfiguration.statisticReportingHandler =
        report_handler_p;
      mediafoundation_configuration.signalHandlerConfiguration.statisticReportingTimerId =
        timer_id;
      result =
        mediaFoundationSignalHandler_in.initialize (mediafoundation_configuration.signalHandlerConfiguration);
      event_handler_2 = &mediaFoundationSignalHandler_in;
      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown media framework (was: %d), returning\n"),
                  mediaFramework_in));
      return;
    } // end ELSE
  } // end SWITCH
#else
  configuration.signalHandlerConfiguration.connectionManager =
      connection_manager_p;
  configuration.signalHandlerConfiguration.dispatchState =
      &event_dispatch_state_s;
  configuration.signalHandlerConfiguration.hasUI =
      !UIDefinitionFilename_in.empty ();
  if (useReactor_in)
    configuration.signalHandlerConfiguration.listener =
        TEST_I_TARGET_LISTENER_SINGLETON::instance ();
  else
    configuration.signalHandlerConfiguration.listener =
        TEST_I_TARGET_ASYNCHLISTENER_SINGLETON::instance ();
  configuration.signalHandlerConfiguration.statisticReportingHandler =
      report_handler_p;
  configuration.signalHandlerConfiguration.statisticReportingTimerId =
      timer_id;
  result =
      signalHandler_in.initialize (configuration.signalHandlerConfiguration);
  event_handler_2 = &signalHandler_in;
#endif // ACE_WIN32 || ACE_WIN64
  if (!result)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to initialize signal handler, returning\n")));
    timer_manager_p->stop ();
    goto clean;
  } // end IF
  if (!Common_Signal_Tools::initialize ((useReactor_in ? COMMON_SIGNAL_DISPATCH_REACTOR
                                                       : COMMON_SIGNAL_DISPATCH_PROACTOR),
                                        signalSet_in,
                                        ignoredSignalSet_in,
                                        event_handler_2,
                                        previousSignalActions_inout))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_Signal_Tools::initialize(), returning\n")));
    timer_manager_p->stop ();
    goto clean;
  } // end IF

  // step1: handle events (signals, incoming connections/data, timers, ...)
  // reactor/proactor event loop:
  // - dispatch connection attempts to acceptor
  // - dispatch socket events
  // timer events:
  // - perform statistic collecting/reporting
  // [GTK events:]
  // - dispatch UI events (if any)

  // step1a: start GUI event loop ?
#if defined (GUI_SUPPORT)
  if (!UIDefinitionFilename_in.empty ())
  {
#if defined (GTK_USE)
    gtk_manager_p = COMMON_UI_GTK_MANAGER_SINGLETON::instance ();
    ACE_ASSERT (gtk_manager_p);
    Common_UI_GTK_State_t& state_r =
      const_cast<Common_UI_GTK_State_t&> (gtk_manager_p->getR_2 ());
    state_r.eventHooks.finiHook = idle_finalize_target_UI_cb;
    state_r.eventHooks.initHook = idle_initialize_target_UI_cb;
    //CBData_in.gladeXML[ACE_TEXT_ALWAYS_CHAR (COMMON_UI_DEFINITION_DESCRIPTOR_MAIN)] =
    //  std::make_pair (UIDefinitionFilename_in, static_cast<GladeXML*> (NULL));
    state_r.builders[ACE_TEXT_ALWAYS_CHAR (COMMON_UI_DEFINITION_DESCRIPTOR_MAIN)] =
      std::make_pair (UIDefinitionFilename_in, static_cast<GtkBuilder*> (NULL));

    gtk_manager_p->start ();
    ACE_Time_Value timeout (0,
                            COMMON_UI_GTK_TIMEOUT_DEFAULT_MANAGER_INITIALIZATION * 1000);
    result = ACE_OS::sleep (timeout);
    if (result == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_OS::sleep(%#T): \"%m\", continuing\n"),
                  &timeout));
    if (!gtk_manager_p->isRunning ())
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to start GTK event dispatch, returning\n")));
      timer_manager_p->stop ();
      goto clean;
    } // end IF
#endif // GTK_USE

#if defined (ACE_WIN32) || defined (ACE_WIN64)
    HWND window_p = ::GetConsoleWindow ();
    if (!window_p)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ::GetConsoleWindow(), returning\n")));

      // clean up
      timer_manager_p->stop ();
      gtk_manager_p->stop (true);

      goto clean;
    } // end IF
    BOOL was_visible_b = ::ShowWindow (window_p, SW_HIDE);
    ACE_UNUSED_ARG (was_visible_b);
#endif // ACE_WIN32 || ACE_WIN64
  } // end IF
#endif // GUI_SUPPORT

  // step1b: initialize worker(s)
  if (!Common_Tools::startEventDispatch (event_dispatch_state_s))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to start event dispatch, returning\n")));

    // clean up
    //		{ // synch access
    //			ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard(CBData_in.lock);

    //			for (Net_GTK_EventSourceIDsIterator_t iterator = CBData_in.event_source_ids.begin();
    //					 iterator != CBData_in.event_source_ids.end();
    //					 iterator++)
    //				g_source_remove(*iterator);
    //		} // end lock scope
#if defined (GUI_SUPPORT)
    if (!UIDefinitionFilename_in.empty ())
#if defined (GTK_USE)
      gtk_manager_p->stop ();
#else
      ;
#endif // GTK_USE
#endif // GUI_SUPPORT
    timer_manager_p->stop ();
    goto clean;
  } // end IF

  // step1c: start listening ?
  if (UIDefinitionFilename_in.empty ())
  {
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    Test_I_Target_DirectShow_IInetConnector_t* directshow_iconnector_p = NULL;
    Test_I_Target_MediaFoundation_IInetConnector_t* mediafoundation_iconnector_p =
      NULL;
#else
    Test_I_Target_IInetConnector_t* iconnector_p = NULL;
#endif // ACE_WIN32 || ACE_WIN64
    if (useUDP_in)
    {
#if defined (ACE_WIN32) || defined (ACE_WIN64)
      switch (mediaFramework_in)
      {
        case STREAM_MEDIAFRAMEWORK_DIRECTSHOW:
        {
          if (useReactor_in)
            ACE_NEW_NORETURN (directshow_iconnector_p,
                              Test_I_Target_DirectShow_UDPConnector_t (directshow_connection_manager_p,
                                                                       (statisticReportingInterval_in ? ACE_Time_Value (statisticReportingInterval_in, 0) : ACE_Time_Value::zero)));
          else
            ACE_NEW_NORETURN (directshow_iconnector_p,
                              Test_I_Target_DirectShow_UDPAsynchConnector_t (directshow_connection_manager_p,
                                                                             (statisticReportingInterval_in ? ACE_Time_Value (statisticReportingInterval_in, 0) : ACE_Time_Value::zero)));
          result_2 =
            directshow_iconnector_p->initialize ((*directshow_connection_configuration_iterator).second);
          break;
        }
        case STREAM_MEDIAFRAMEWORK_MEDIAFOUNDATION:
        {
          if (useReactor_in)
            ACE_NEW_NORETURN (mediafoundation_iconnector_p,
                              Test_I_Target_MediaFoundation_UDPConnector_t (mediafoundation_connection_manager_p,
                                                                            (statisticReportingInterval_in ? ACE_Time_Value (statisticReportingInterval_in, 0) : ACE_Time_Value::zero)));
          else
            ACE_NEW_NORETURN (mediafoundation_iconnector_p,
                              Test_I_Target_MediaFoundation_UDPAsynchConnector_t (mediafoundation_connection_manager_p,
                                                                                  (statisticReportingInterval_in ? ACE_Time_Value (statisticReportingInterval_in, 0) : ACE_Time_Value::zero)));
          result_2 =
            mediafoundation_iconnector_p->initialize ((*mediafoundation_connection_configuration_iterator).second);
          break;
        }
        default:
        {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("invalid/unknown media framework (was: %d), returning\n"),
                      mediaFramework_in));
          return;
        } // end ELSE
      } // end SWITCH
      if (!mediafoundation_iconnector_p && !directshow_iconnector_p)
#else
      if (useReactor_in)
        ACE_NEW_NORETURN (iconnector_p,
                          Test_I_Target_UDPConnector_t (connection_manager_p,
                                                        (statisticReportingInterval_in ? ACE_Time_Value (statisticReportingInterval_in, 0) : ACE_Time_Value::zero)));
      else
        ACE_NEW_NORETURN (iconnector_p,
                          Test_I_Target_UDPAsynchConnector_t (connection_manager_p,
                                                              (statisticReportingInterval_in ? ACE_Time_Value (statisticReportingInterval_in, 0) : ACE_Time_Value::zero)));
      result_2 = iconnector_p->initialize ((*iterator_2).second);
      if (!iconnector_p)
#endif // ACE_WIN32 || ACE_WIN64
      {
        ACE_DEBUG ((LM_CRITICAL,
                    ACE_TEXT ("failed to allocate memory, returning\n")));

        // clean up
        Common_Tools::finalizeEventDispatch (event_dispatch_state_s.proactorGroupId,
                                             event_dispatch_state_s.reactorGroupId,
                                             true);
        //		{ // synch access
        //			ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard(CBData_in.lock);

        //			for (Net_GTK_EventSourceIDsIterator_t iterator = CBData_in.event_source_ids.begin();
        //					 iterator != CBData_in.event_source_ids.end();
        //					 iterator++)
        //				g_source_remove(*iterator);
        //		} // end lock scope
#if defined (GUI_SUPPORT)
        if (!UIDefinitionFilename_in.empty ())
#if defined (GTK_USE)
          gtk_manager_p->stop ();
#else
          ;
#endif // GTK_USE
#endif // GUI_SUPPORT
        timer_manager_p->stop ();
        goto clean;
      } // end IF
      //  Stream_IInetConnector_t* iconnector_p = &connector;
      if (!result_2)
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to initialize connector: \"%m\", returning\n")));

        // clean up
        Common_Tools::finalizeEventDispatch (event_dispatch_state_s.proactorGroupId,
                                             event_dispatch_state_s.reactorGroupId,
                                             true);
        //		{ // synch access
        //			ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard(CBData_in.lock);

        //			for (Net_GTK_EventSourceIDsIterator_t iterator = CBData_in.event_source_ids.begin();
        //					 iterator != CBData_in.event_source_ids.end();
        //					 iterator++)
        //				g_source_remove(*iterator);
        //		} // end lock scope
#if defined (GUI_SUPPORT)
        if (!UIDefinitionFilename_in.empty ())
#if defined (GTK_USE)
          gtk_manager_p->stop ();
#else
          ;
#endif // GTK_USE
#endif // GUI_SUPPORT
        timer_manager_p->stop ();
#if defined (ACE_WIN32) || defined (ACE_WIN64)
        switch (mediaFramework_in)
        {
          case STREAM_MEDIAFRAMEWORK_DIRECTSHOW:
          {
            delete directshow_iconnector_p;
            break;
          }
          case STREAM_MEDIAFRAMEWORK_MEDIAFOUNDATION:
          {
            delete mediafoundation_iconnector_p;
            break;
          }
          default:
          {
            ACE_DEBUG ((LM_ERROR,
                        ACE_TEXT ("invalid/unknown media framework (was: %d), returning\n"),
                        mediaFramework_in));
            return;
          } // end ELSE
        } // end SWITCH
#else
        delete iconnector_p;
#endif // ACE_WIN32 || ACE_WIN64
        goto clean;
      } // end IF

      // connect
      ACE_INET_Addr peer_address;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
      switch (mediaFramework_in)
      {
        case STREAM_MEDIAFRAMEWORK_DIRECTSHOW:
        {
          peer_address =
            (*directshow_connection_configuration_iterator).second.socketHandlerConfiguration.socketConfiguration_2.address;
          break;
        }
        case STREAM_MEDIAFRAMEWORK_MEDIAFOUNDATION:
        {
          peer_address =
            (*mediafoundation_connection_configuration_iterator).second.socketHandlerConfiguration.socketConfiguration_2.address;
          break;
        }
        default:
        {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("invalid/unknown media framework (was: %d), returning\n"),
                      mediaFramework_in));
          return;
        } // end ELSE
      } // end SWITCH
#else
      peer_address =
          (*iterator_2).second.socketHandlerConfiguration.socketConfiguration_2.address;
#endif // ACE_WIN32 || ACE_WIN64
      if (result == -1)
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ACE_INET_Addr::addr_to_string(): \"%m\", continuing\n")));
      // *TODO*: support one-thread operation by scheduling a signal and manually
      //         running the dispatch loop for a limited time...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
      switch (mediaFramework_in)
      {
        case STREAM_MEDIAFRAMEWORK_DIRECTSHOW:
        {
          directshow_configuration.handle =
            directshow_iconnector_p->connect ((*directshow_connection_configuration_iterator).second.socketHandlerConfiguration.socketConfiguration_2.address);
          break;
        }
        case STREAM_MEDIAFRAMEWORK_MEDIAFOUNDATION:
        {
          mediafoundation_configuration.handle =
            mediafoundation_iconnector_p->connect ((*mediafoundation_connection_configuration_iterator).second.socketHandlerConfiguration.socketConfiguration_2.address);
          break;
        }
        default:
        {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("invalid/unknown media framework (was: %d), returning\n"),
                      mediaFramework_in));
          return;
        } // end ELSE
      } // end SWITCH
#else
      configuration.handle =
        iconnector_p->connect ((*iterator_2).second.socketHandlerConfiguration.socketConfiguration_2.address);
#endif // ACE_WIN32 || ACE_WIN64
      if (!useReactor_in)
      {
        // *TODO*: avoid tight loop here
        ACE_Time_Value timeout (NET_CLIENT_DEFAULT_ASYNCH_CONNECT_TIMEOUT, 0);
        //result = ACE_OS::sleep (timeout);
        //if (result == -1)
        //  ACE_DEBUG ((LM_ERROR,
        //              ACE_TEXT ("failed to ACE_OS::sleep(%#T): \"%m\", continuing\n"),
        //              &timeout));
        ACE_Time_Value deadline = COMMON_TIME_NOW + timeout;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
        Test_I_Target_MediaFoundation_UDPAsynchConnector_t::ICONNECTION_T* mediafoundation_connection_p =
          NULL;
        Test_I_Target_DirectShow_UDPAsynchConnector_t::ICONNECTION_T* directshow_connection_p =
          NULL;
#else
        typename Test_I_Target_UDPAsynchConnector_t::ICONNECTION_T* connection_p =
            NULL;
#endif // ACE_WIN32 || ACE_WIN64
//        bool done = false;
        do
        {
#if defined (ACE_WIN32) || defined (ACE_WIN64)
          switch (mediaFramework_in)
          {
            case STREAM_MEDIAFRAMEWORK_DIRECTSHOW:
            {
              directshow_connection_p =
                directshow_connection_manager_p->get (peer_address);
              break;
            }
            case STREAM_MEDIAFRAMEWORK_MEDIAFOUNDATION:
            {
              mediafoundation_connection_p =
                mediafoundation_connection_manager_p->get (peer_address);
              break;
            }
            default:
            {
              ACE_DEBUG ((LM_ERROR,
                          ACE_TEXT ("invalid/unknown media framework (was: %d), returning\n"),
                          mediaFramework_in));
              return;
            } // end ELSE
          } // end SWITCH
#else
          connection_p = connection_manager_p->get (peer_address);
#endif // ACE_WIN32 || ACE_WIN64
#if defined (ACE_WIN32) || defined (ACE_WIN64)
          switch (mediaFramework_in)
          {
            case STREAM_MEDIAFRAMEWORK_DIRECTSHOW:
            {
              if (directshow_connection_p)
              {
                directshow_configuration.handle =
                  reinterpret_cast<ACE_HANDLE> (directshow_connection_p->id ());
                directshow_connection_p->decrease ();
                done = true;
                break;
              } // end IF
              break;
            }
            case STREAM_MEDIAFRAMEWORK_MEDIAFOUNDATION:
            {
              if (mediafoundation_connection_p)
              {
                mediafoundation_configuration.handle =
                  reinterpret_cast<ACE_HANDLE> (mediafoundation_connection_p->id ());
                mediafoundation_connection_p->decrease ();
                done = true;
                break;
              } // end IF
              break;
            }
            default:
            {
              ACE_DEBUG ((LM_ERROR,
                          ACE_TEXT ("invalid/unknown media framework (was: %d), returning\n"),
                          mediaFramework_in));
              return;
            } // end ELSE
          } // end SWITCH
          if (done)
            break;
#else
          if (connection_p)
          {
            configuration.handle =
                static_cast<ACE_HANDLE> (connection_p->id ());
            connection_p->decrease ();
            break;
          } // end IF
#endif // ACE_WIN32 || ACE_WIN64
        } while (COMMON_TIME_NOW < deadline);
      } // end IF
#if defined (ACE_WIN32) || defined (ACE_WIN64)
      switch (mediaFramework_in)
      {
        case STREAM_MEDIAFRAMEWORK_DIRECTSHOW:
        {
          result_2 = (directshow_configuration.handle == ACE_INVALID_HANDLE);
          break;
        }
        case STREAM_MEDIAFRAMEWORK_MEDIAFOUNDATION:
        {
          result_2 =
            (mediafoundation_configuration.handle == ACE_INVALID_HANDLE);
          break;
        }
        default:
        {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("invalid/unknown media framework (was: %d), returning\n"),
                      mediaFramework_in));
          return;
        } // end ELSE
      } // end SWITCH
#else
      result_2 = (configuration.handle == ACE_INVALID_HANDLE);
#endif // ACE_WIN32 || ACE_WIN64
      if (!result_2)
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to connect to %s, returning\n"),
                    ACE_TEXT (Net_Common_Tools::IPAddressToString (peer_address).c_str ())));

        // clean up
#if defined (ACE_WIN32) || defined (ACE_WIN64)
        //if (useMediaFoundation_in)
        //  mediafoundation_iconnector_p->abort ();
        //else
        //  directshow_iconnector_p->abort ();
#else
#endif // ACE_WIN32 || ACE_WIN64
        Common_Tools::finalizeEventDispatch (event_dispatch_state_s.proactorGroupId,
                                             event_dispatch_state_s.reactorGroupId,
                                             true);
        //		{ // synch access
        //			ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard(CBData_in.lock);

        //			for (Net_GTK_EventSourceIDsIterator_t iterator = CBData_in.event_source_ids.begin();
        //					 iterator != CBData_in.event_source_ids.end();
        //					 iterator++)
        //				g_source_remove(*iterator);
        //		} // end lock scope
#if defined (GUI_SUPPORT)
        if (!UIDefinitionFilename_in.empty ())
#if defined (GTK_USE)
          gtk_manager_p->stop ();
#else
          ;
#endif // GTK_USE
#endif // GUI_SUPPORT
        timer_manager_p->stop ();
#if defined (ACE_WIN32) || defined (ACE_WIN64)
        switch (mediaFramework_in)
        {
          case STREAM_MEDIAFRAMEWORK_DIRECTSHOW:
          {
            delete directshow_iconnector_p;
            break;
          }
          case STREAM_MEDIAFRAMEWORK_MEDIAFOUNDATION:
          {
            delete mediafoundation_iconnector_p;
            break;
          }
          default:
          {
            ACE_DEBUG ((LM_ERROR,
                        ACE_TEXT ("invalid/unknown media framework (was: %d), returning\n"),
                        mediaFramework_in));
            return;
          } // end ELSE
        } // end SWITCH
#else
        delete iconnector_p;
#endif // ACE_WIN32 || ACE_WIN64
        goto clean;
      } // end IF
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("listening to UDP \"%s\"...\n"),
                  ACE_TEXT (Net_Common_Tools::IPAddressToString (peer_address).c_str ())));

      // clean up
#if defined (ACE_WIN32) || defined (ACE_WIN64)
      switch (mediaFramework_in)
      {
        case STREAM_MEDIAFRAMEWORK_DIRECTSHOW:
        {
          delete directshow_iconnector_p;
          break;
        }
        case STREAM_MEDIAFRAMEWORK_MEDIAFOUNDATION:
        {
          delete mediafoundation_iconnector_p;
          break;
        }
        default:
        {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("invalid/unknown media framework (was: %d), returning\n"),
                      mediaFramework_in));
          return;
        } // end ELSE
      } // end SWITCH
#else
      delete iconnector_p;
#endif // ACE_WIN32 || ACE_WIN64
    } // end IF
    else
    {
#if defined (ACE_WIN32) || defined (ACE_WIN64)
      switch (mediaFramework_in)
      {
        case STREAM_MEDIAFRAMEWORK_DIRECTSHOW:
        {
          result_2 =
            directShowCBData_in.configuration->signalHandlerConfiguration.listener->initialize (directshow_configuration.listenerConfiguration);
          break;
        }
        case STREAM_MEDIAFRAMEWORK_MEDIAFOUNDATION:
        {
          result_2 =
            mediaFoundationCBData_in.configuration->signalHandlerConfiguration.listener->initialize (mediafoundation_configuration.listenerConfiguration);
          break;
        }
        default:
        {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("invalid/unknown media framework (was: %d), returning\n"),
                      mediaFramework_in));
          return;
        } // end ELSE
      } // end SWITCH
#else
      result_2 =
        CBData_in.configuration->signalHandlerConfiguration.listener->initialize (configuration.listenerConfiguration);
#endif // ACE_WIN32 || ACE_WIN64
      if (!result_2)
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to initialize listener, returning\n")));

        Common_Tools::finalizeEventDispatch (event_dispatch_state_s.proactorGroupId,
                                             event_dispatch_state_s.reactorGroupId,
                                             true);
        //		{ // synch access
        //			ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard(CBData_in.lock);

        //			for (Net_GTK_EventSourceIDsIterator_t iterator = CBData_in.event_source_ids.begin();
        //					 iterator != CBData_in.event_source_ids.end();
        //					 iterator++)
        //				g_source_remove(*iterator);
        //		} // end lock scope
#if defined (GUI_SUPPORT)
        if (!UIDefinitionFilename_in.empty ())
#if defined (GTK_USE)
          gtk_manager_p->stop ();
#else
          ;
#endif // GTK_USE
#endif // GUI_SUPPORT
        timer_manager_p->stop ();
        goto clean;
      } // end IF
#if defined (ACE_WIN32) || defined (ACE_WIN64)
      switch (mediaFramework_in)
      {
        case STREAM_MEDIAFRAMEWORK_DIRECTSHOW:
        {
          directShowCBData_in.configuration->signalHandlerConfiguration.listener->start ();
          result_2 =
            directShowCBData_in.configuration->signalHandlerConfiguration.listener->isRunning ();
          break;
        }
        case STREAM_MEDIAFRAMEWORK_MEDIAFOUNDATION:
        {
          mediaFoundationCBData_in.configuration->signalHandlerConfiguration.listener->start ();
          result_2 =
            mediaFoundationCBData_in.configuration->signalHandlerConfiguration.listener->isRunning ();
          break;
        }
        default:
        {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("invalid/unknown media framework (was: %d), returning\n"),
                      mediaFramework_in));
          return;
        } // end ELSE
      } // end SWITCH
#else
      CBData_in.configuration->signalHandlerConfiguration.listener->start ();
      result_2 =
        CBData_in.configuration->signalHandlerConfiguration.listener->isRunning ();
#endif // ACE_WIN32 || ACE_WIN64
      if (!result_2)
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to start listener (port: %u), returning\n"),
                    listeningPortNumber_in));

        Common_Tools::finalizeEventDispatch (event_dispatch_state_s.proactorGroupId,
                                             event_dispatch_state_s.reactorGroupId,
                                             true);
        //		{ // synch access
        //			ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard(CBData_in.lock);

        //			for (Net_GTK_EventSourceIDsIterator_t iterator = CBData_in.event_source_ids.begin();
        //					 iterator != CBData_in.event_source_ids.end();
        //					 iterator++)
        //				g_source_remove(*iterator);
        //		} // end lock scope
#if defined (GUI_SUPPORT)
        if (!UIDefinitionFilename_in.empty ())
#if defined (GTK_USE)
          gtk_manager_p->stop ();
#else
          ;
#endif // GTK_USE
#endif // GUI_SUPPORT
        timer_manager_p->stop ();
        goto clean;
      } // end IF
    } // end ELSE
  } // end IF

  // *NOTE*: from this point on, clean up any remote connections !

//  Common_Tools::dispatchEvents (useReactor_in,
//                                group_id);

  // clean up
clean:
  // *NOTE*: listener has stopped, interval timer has been cancelled,
  // and connections have been aborted...
  //		{ // synch access
  //			ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard(CBData_in.lock);

  //			for (Net_GTK_EventSourceIDsIterator_t iterator = CBData_in.event_source_ids.begin();
  //					 iterator != CBData_in.event_source_ids.end();
  //					 iterator++)
  //				g_source_remove(*iterator);
  //		} // end lock scope
#if defined (GUI_SUPPORT)
  if (!UIDefinitionFilename_in.empty ())
  {
#if defined (GTK_USE)
    gtk_manager_p->wait ();
#endif // GTK_USE
#endif // GUI_SUPPORT
//    connection_manager_p->abort ();
  } // end IF
  else
    Common_Tools::dispatchEvents (useReactor_in,
                                  group_id);

  // wait for connection processing to complete
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  //  connection_manager_p->stop ();
  //  connection_manager_p->abort ();
  switch (mediaFramework_in)
  {
    case STREAM_MEDIAFRAMEWORK_DIRECTSHOW:
    {
      directshow_connection_manager_p->wait ();
      break;
    }
    case STREAM_MEDIAFRAMEWORK_MEDIAFOUNDATION:
    {
      mediafoundation_connection_manager_p->wait ();
      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown media framework (was: %d), returning\n"),
                  mediaFramework_in));
      return;
    } // end ELSE
  } // end SWITCH
#else
  connection_manager_p->wait ();
#endif // ACE_WIN32 || ACE_WIN64

  timer_manager_p->stop ();

  Common_Tools::finalizeEventDispatch (event_dispatch_state_s.proactorGroupId,
                                             event_dispatch_state_s.reactorGroupId,
                                             true);

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  switch (mediaFramework_in)
  {
    case STREAM_MEDIAFRAMEWORK_DIRECTSHOW:
    {
      result = directshow_event_handler.close (ACE_Module_Base::M_DELETE_NONE);
      break;
    }
    case STREAM_MEDIAFRAMEWORK_MEDIAFOUNDATION:
    {
      result =
        mediafoundation_event_handler.close (ACE_Module_Base::M_DELETE_NONE);
      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown media framework (was: %d), returning\n"),
                  mediaFramework_in));
      return;
    } // end ELSE
  } // end SWITCH
#else
  result = event_handler.close (ACE_Module_Base::M_DELETE_NONE);
#endif // ACE_WIN32 || ACE_WIN64
  if (result == -1)
  {
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    switch (mediaFramework_in)
    {
      case STREAM_MEDIAFRAMEWORK_DIRECTSHOW:
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("%s: failed to ACE_Module::close (): \"%m\", continuing\n"),
                    directshow_event_handler.name ()));
        break;
      }
      case STREAM_MEDIAFRAMEWORK_MEDIAFOUNDATION:
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("%s: failed to ACE_Module::close (): \"%m\", continuing\n"),
                    mediafoundation_event_handler.name ()));
        break;
      }
      default:
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("invalid/unknown media framework (was: %d), returning\n"),
                    mediaFramework_in));
        return;
      } // end ELSE
    } // end SWITCH
#else
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: failed to ACE_Module::close (): \"%m\", continuing\n"),
                event_handler.name ()));
#endif // ACE_WIN32 || ACE_WIN64
  } // end IF

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  switch (mediaFramework_in)
  {
    case STREAM_MEDIAFRAMEWORK_DIRECTSHOW:
    {
      do_finalize_directshow (directShowCBData_in);
      break;
    }
    case STREAM_MEDIAFRAMEWORK_MEDIAFOUNDATION:
    {
      do_finalize_mediafoundation (mediaFoundationCBData_in);
      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown media framework (was: %d), returning\n"),
                  mediaFramework_in));
      return;
    } // end ELSE
  } // end SWITCH
#endif // ACE_WIN32 || ACE_WIN64

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("finished working...\n")));
}

COMMON_DEFINE_PRINTVERSION_FUNCTION(do_printVersion,STREAM_MAKE_VERSION_STRING_VARIABLE(programName_in,ACE_TEXT_ALWAYS_CHAR (ACEStream_PACKAGE_VERSION_FULL),version_string),version_string)

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
  Common_Tools::initialize ();

  // *PROCESS PROFILE*
  ACE_Profile_Timer process_profile;
  // start profile timer...
  process_profile.start ();

  std::string configuration_path =
    Common_File_Tools::getWorkingDirectory ();

  // step1a set defaults
  unsigned int buffer_size = CAMSTREAM_DEFAULT_BUFFER_SIZE;
  unsigned int maximum_number_of_connections =
    TEST_I_MAXIMUM_NUMBER_OF_OPEN_CONNECTIONS;
  std::string path = configuration_path;
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  path += ACE_TEXT_ALWAYS_CHAR (COMMON_LOCATION_CONFIGURATION_SUBDIRECTORY);
  std::string gtk_rc_file = path;
  gtk_rc_file += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  gtk_rc_file += ACE_TEXT_ALWAYS_CHAR (TEST_I_DEFAULT_GTK_RC_FILE);
  std::string output_file = ACE_TEXT_ALWAYS_CHAR (TEST_I_DEFAULT_OUTPUT_FILE);
  std::string gtk_glade_file = path;
  gtk_glade_file += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  gtk_glade_file +=
      ACE_TEXT_ALWAYS_CHAR (TEST_I_DEFAULT_TARGET_GLADE_FILE);
  bool use_thread_pool = COMMON_EVENT_REACTOR_DEFAULT_USE_THREADPOOL;
  bool log_to_file = false;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  enum Stream_MediaFramework_Type media_framework_e =
    STREAM_LIB_DEFAULT_MEDIAFRAMEWORK;
#endif // ACE_WIN32 || ACE_WIN64
  std::string network_interface =
    ACE_TEXT_ALWAYS_CHAR (NET_INTERFACE_DEFAULT_ETHERNET);
  bool use_loopback = false;
  unsigned short listening_port_number = TEST_I_DEFAULT_PORT;
  bool use_reactor =
          (COMMON_EVENT_DEFAULT_DISPATCH == COMMON_EVENT_DISPATCH_REACTOR);
  unsigned int statistic_reporting_interval =
      STREAM_DEFAULT_STATISTIC_REPORTING_INTERVAL;
  bool trace_information = false;
  bool use_UDP = false;
  bool print_version_and_exit = false;
  unsigned int number_of_dispatch_threads =
    TEST_I_DEFAULT_NUMBER_OF_DISPATCHING_THREADS;
  unsigned int frame_size = TEST_I_DEFAULT_FRAME_SIZE;

  // step1b: parse/process/validate configuration
  if (!do_processArguments (argc_in,
                            argv_in,
                            buffer_size,
                            maximum_number_of_connections,
                            gtk_rc_file,
                            output_file,
                            gtk_glade_file,
                            use_thread_pool,
                            log_to_file,
#if defined (ACE_WIN32) || defined (ACE_WIN64)
                            media_framework_e,
#endif // ACE_WIN32 || ACE_WIN64
                            network_interface,
                            use_loopback,
                            listening_port_number,
                            use_reactor,
                            statistic_reporting_interval,
                            trace_information,
                            use_UDP,
                            print_version_and_exit,
                            number_of_dispatch_threads,
                            frame_size))
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
  if (TEST_I_MAX_MESSAGES)
    ACE_DEBUG ((LM_WARNING,
                ACE_TEXT ("limiting the number of message buffers could (!) lead to deadlocks --> make sure you know what you are doing...\n")));
  if (use_reactor                      &&
      (number_of_dispatch_threads > 1) &&
      !use_thread_pool)
  { // *NOTE*: see also: man (2) select
    // *TODO*: verify this for MS Windows based systems
    ACE_DEBUG ((LM_WARNING,
                ACE_TEXT ("the select()-based reactor is not reentrant, using the thread-pool reactor instead...\n")));
    use_thread_pool = true;
  } // end IF
  if ((frame_size > buffer_size)                                           ||
      (gtk_glade_file.empty () &&
       !Common_File_Tools::isValidFilename (output_file))                  ||
      (!gtk_glade_file.empty () &&
       !Common_File_Tools::isReadable (gtk_glade_file))                    ||
      //(!gtk_rc_file_name.empty () &&
      // !Common_File_Tools::isReadable (gtk_rc_file_name))                   ||
      (use_thread_pool && !use_reactor)                                    ||
      (use_reactor && (number_of_dispatch_threads > 1) && !use_thread_pool))
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
  if (number_of_dispatch_threads == 0)
    number_of_dispatch_threads = 1;

  Common_MessageStack_t* logstack_p = NULL;
  ACE_SYNCH_MUTEX* lock_p = NULL;
  ACE_SYNCH_RECURSIVE_MUTEX* lock_2 = NULL;
#if defined (GUI_SUPPORT)
  struct Test_I_CamStream_UI_CBData* ui_cb_data_p = NULL;
#if defined (GTK_USE)
  Common_UI_GTK_Manager_t* gtk_manager_p =
    COMMON_UI_GTK_MANAGER_SINGLETON::instance ();
  ACE_ASSERT (gtk_manager_p);
  Common_UI_GTK_State_t& state_r =
    const_cast<Common_UI_GTK_State_t&> (gtk_manager_p->getR_2 ());
  logstack_p = &state_r.logStack;
  lock_p = &state_r.logStackLock;
  lock_2 = &state_r.subscribersLock;
#endif // GTK_USE
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  struct Test_I_Target_DirectShow_UI_CBData directshow_ui_cb_data;
  struct Test_I_Target_MediaFoundation_UI_CBData mediafoundation_ui_cb_data;
  switch (media_framework_e)
  {
    case STREAM_MEDIAFRAMEWORK_DIRECTSHOW:
    {
      //directshow_ui_cb_data.progressData.state = &directshow_ui_cb_data;
      ui_cb_data_p = &directshow_ui_cb_data;
      break;
    }
    case STREAM_MEDIAFRAMEWORK_MEDIAFOUNDATION:
    {
      //mediafoundation_ui_cb_data.progressData.state =
      //  &mediafoundation_ui_cb_data;
      ui_cb_data_p = &mediafoundation_ui_cb_data;
      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown media framework (was: %d), aborting\n"),
                  media_framework_e));
            
      // *PORTABILITY*: on Windows, finalize ACE...
      result = ACE::fini ();
      if (result == -1)
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ACE::fini(): \"%m\", continuing\n")));
      return EXIT_FAILURE;
    }
  } // end SWITCH
#if defined (GTK_USE)
  Test_I_Target_DirectShow_GtkBuilderDefinition_t directshow_ui_definition (argc_in,
                                                                            argv_in,
                                                                            &directshow_ui_cb_data);
  Test_I_Target_MediaFoundation_GtkBuilderDefinition_t mediafoundation_ui_definition (argc_in,
                                                                                      argv_in,
                                                                                      &mediafoundation_ui_cb_data);
#endif // GTK_USE
#else
  struct Test_I_Target_UI_CBData ui_cb_data;
  ui_cb_data_p = &ui_cb_data;
#if defined (GTK_USE)
  Test_I_Target_GtkBuilderDefinition_t ui_definition (argc_in,
                                                      argv_in,
                                                      &ui_cb_data);
#endif // GTK_USE
#endif // ACE_WIN32 || ACE_WIN64
  ACE_ASSERT (ui_cb_data_p);
#endif // GUI_SUPPORT

  // step1d: initialize logging and/or tracing
  Common_Logger_t logger (logstack_p,
                          lock_p);
  std::string log_file_name;
  if (log_to_file)
    log_file_name =
      Common_Log_Tools::getLogFilename (ACE_TEXT_ALWAYS_CHAR (ACEStream_PACKAGE_NAME),
                                        ACE::basename (argv_in[0]));
  if (!Common_Log_Tools::initializeLogging (ACE::basename (argv_in[0]),           // program name
                                            log_file_name,                        // log file name
                                            false,                                // log to syslog ?
                                            false,                                // trace messages ?
                                            trace_information,                    // debug messages ?
                                            (gtk_glade_file.empty () ? NULL
                                                                     : &logger))) // logger ?
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

  // step1e: pre-initialize signal handling
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  Test_I_Target_DirectShow_SignalHandler_t directshow_signal_handler ((use_reactor ? COMMON_SIGNAL_DISPATCH_REACTOR
                                                                                   : COMMON_SIGNAL_DISPATCH_PROACTOR),
                                                                      lock_2);
  Test_I_Target_MediaFoundation_SignalHandler_t mediafoundation_signal_handler ((use_reactor ? COMMON_SIGNAL_DISPATCH_REACTOR
                                                                                             : COMMON_SIGNAL_DISPATCH_PROACTOR),
                                                                                lock_2);
#else
  Test_I_Target_SignalHandler_t signal_handler ((use_reactor ? COMMON_SIGNAL_DISPATCH_REACTOR
                                                             : COMMON_SIGNAL_DISPATCH_PROACTOR),
                                                lock_2);
#endif // ACE_WIN32 || ACE_WIN64
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
                                           use_reactor,
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

  // step1f: handle specific program modes
  if (print_version_and_exit)
  {
    do_printVersion (ACE::basename (argv_in[0]));

    Common_Signal_Tools::finalize ((use_reactor ? COMMON_SIGNAL_DISPATCH_REACTOR
                                                : COMMON_SIGNAL_DISPATCH_PROACTOR),
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
  if (!Common_Tools::setResourceLimits (true,  // file descriptors
                                        true,  // stack traces
                                        true)) // pending signals
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_Tools::setResourceLimits(), aborting\n")));

    Common_Signal_Tools::finalize ((use_reactor ? COMMON_SIGNAL_DISPATCH_REACTOR
                                                : COMMON_SIGNAL_DISPATCH_PROACTOR),
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
#if defined (GTK_USE)
  // step1h: initialize GLIB / G(D|T)K[+] / GNOME ?
  bool result_2 = false;
  if (gtk_glade_file.empty ())
    goto continue_;

  state_r.RCFiles.push_back (gtk_rc_file);
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  switch (media_framework_e)
  {
    case STREAM_MEDIAFRAMEWORK_DIRECTSHOW:
    {
      result_2 = gtk_manager_p->initialize (argc_in,
                                            argv_in,
                                            &directshow_ui_definition);
      break;
    }
    case STREAM_MEDIAFRAMEWORK_MEDIAFOUNDATION:
    {
      result_2 = gtk_manager_p->initialize (argc_in,
                                            argv_in,
                                            &mediafoundation_ui_definition);
      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown media framework (was: %d), aborting\n"),
                  media_framework_e));
      break;
    }
  } // end SWITCH
#else
  result_2 = gtk_manager_p->initialize (argc_in,
                                        argv_in,
                                        &ui_definition);
#endif // ACE_WIN32 || ACE_WIN64
  if (!result_2)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_UI_GTK_Manager::initialize(), aborting\n")));

    Common_Signal_Tools::finalize ((use_reactor ? COMMON_SIGNAL_DISPATCH_REACTOR
                                                : COMMON_SIGNAL_DISPATCH_PROACTOR),
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
#endif // GTK_USE
#endif // GUI_SUPPORT

//continue_:
  ACE_High_Res_Timer timer;
  timer.start ();
  // step2: do actual work
  do_work (buffer_size,
           maximum_number_of_connections,
           output_file,
           gtk_glade_file,
           use_thread_pool,
#if defined (ACE_WIN32) || defined (ACE_WIN64)
           media_framework_e,
#endif // ACE_WIN32 || ACE_WIN64
           network_interface,
           use_loopback,
           listening_port_number,
           use_reactor,
           statistic_reporting_interval,
           use_UDP,
           number_of_dispatch_threads,
           frame_size,
#if defined (ACE_WIN32) || defined (ACE_WIN64)
           mediafoundation_ui_cb_data,
           directshow_ui_cb_data,
#else
           ui_cb_data,
#endif // ACE_WIN32 || ACE_WIN64
           signal_set,
           ignored_signal_set,
           previous_signal_actions,
           previous_signal_mask,
#if defined (ACE_WIN32) || defined (ACE_WIN64)
           directshow_signal_handler,
           mediafoundation_signal_handler);
#else
           signal_handler);
#endif // ACE_WIN32 || ACE_WIN64
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

    Common_Signal_Tools::finalize ((use_reactor ? COMMON_SIGNAL_DISPATCH_REACTOR
                                                : COMMON_SIGNAL_DISPATCH_PROACTOR),
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

  Common_Signal_Tools::finalize ((use_reactor ? COMMON_SIGNAL_DISPATCH_REACTOR
                                              : COMMON_SIGNAL_DISPATCH_PROACTOR),
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
