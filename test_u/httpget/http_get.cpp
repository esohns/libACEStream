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
#include "stdafx.h"

#include <iostream>
#include <limits>
#include <regex>
#include <string>

#include <ace/Get_Opt.h>
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include <ace/Init_ACE.h>
#endif
#include <ace/Log_Msg.h>
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
#include <ace/POSIX_Proactor.h>
#include <ace/Proactor.h>
#endif
#include <ace/Profile_Timer.h>
#include <ace/Sig_Handler.h>
#include <ace/Signal.h>
#include <ace/Synch.h>
#include <ace/Version.h>

#include "common.h"
#include "common_file_tools.h"
#include "common_logger.h"
#include "common_tools.h"

#include "stream_allocatorheap.h"
#include "stream_macros.h"

#ifdef HAVE_CONFIG_H
#include "libACEStream_config.h"
#endif

#include "net_common_tools.h"

#include "http_defines.h"

#include "test_u_defines.h"

#include "http_get_callbacks.h"
#include "http_get_common.h"
#include "http_get_connection_manager_common.h"
#include "http_get_defines.h"
#include "http_get_eventhandler.h"
#include "http_get_module_eventhandler.h"
#include "http_get_signalhandler.h"
#include "http_get_stream.h"

void
do_printUsage (const std::string& programName_in)
{
  STREAM_TRACE (ACE_TEXT ("::do_printUsage"));

  // enable verbatim boolean output
  std::cout.setf (std::ios::boolalpha);

  std::string working_directory =
    Common_File_Tools::getWorkingDirectory ();
  std::string temp_directory = Common_File_Tools::getTempDirectory ();
  std::string configuration_directory = working_directory;
  configuration_directory += ACE_DIRECTORY_SEPARATOR_STR_A;
  configuration_directory +=
    ACE_TEXT_ALWAYS_CHAR (COMMON_LOCATION_CONFIGURATION_DIRECTORY);

  std::cout << ACE_TEXT_ALWAYS_CHAR ("usage: ")
            << programName_in
            << ACE_TEXT_ALWAYS_CHAR (" [OPTIONS]")
            << std::endl
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("currently available options:")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-b [VALUE]  : buffer size (byte(s)) [")
            << HTTP_BUFFER_SIZE
            << ACE_TEXT_ALWAYS_CHAR ("])")
            << std::endl;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-c          : show console [")
            << false
            << ACE_TEXT_ALWAYS_CHAR ("])")
            << std::endl;
#endif
  std::string output_file_path = temp_directory;
  output_file_path += ACE_DIRECTORY_SEPARATOR_STR_A;
  output_file_path += ACE_TEXT_ALWAYS_CHAR (HTTP_GET_DEFAULT_OUTPUT_FILE);
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-f [STRING] : (output) file name [")
            << output_file_path
            << ACE_TEXT_ALWAYS_CHAR ("]")
            << std::endl;
  std::string UI_file_path = configuration_directory;
  UI_file_path += ACE_DIRECTORY_SEPARATOR_STR_A;
  UI_file_path += ACE_TEXT_ALWAYS_CHAR (HTTPGET_UI_DEFINITION_FILE_NAME);
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-g[[STRING]]: interface definition file [\"")
            << UI_file_path
            << ACE_TEXT_ALWAYS_CHAR ("\"] {\"\" --> no GUI}")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-l          : log to a file [")
            << false
            << ACE_TEXT_ALWAYS_CHAR ("]")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-o          : use thread pool [")
            << NET_EVENT_USE_THREAD_POOL
            << ACE_TEXT_ALWAYS_CHAR ("]")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-r          : use reactor [")
            << NET_EVENT_USE_REACTOR
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
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-u [STRING] : URL")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-v          : print version information and exit [")
            << false
            << ACE_TEXT_ALWAYS_CHAR ("]")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-x [VALUE]  : #dispatch threads [")
            << TEST_U_DEFAULT_NUMBER_OF_DISPATCHING_THREADS
            << ACE_TEXT_ALWAYS_CHAR ("]")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-z          : debug parser [")
            << STREAM_DECODER_DEFAULT_YACC_TRACE
            << ACE_TEXT_ALWAYS_CHAR ("]")
            << std::endl;
}

bool
do_processArguments (int argc_in,
                     ACE_TCHAR** argv_in, // cannot be const...
                     unsigned int& bufferSize_out,
#if defined (ACE_WIN32) || defined (ACE_WIN64)
                     bool& showConsole_out,
#endif
                     std::string& outputFileName_out,
                     std::string& interfaceDefinitionFile_out,
                     std::string& hostName_out,
                     bool& logToFile_out,
                     bool& useThreadPool_out,
                     unsigned short& port_out,
                     bool& useReactor_out,
                     unsigned int& statisticReportingInterval_out,
                     bool& traceInformation_out,
                     std::string& URI_out,
                     bool& printVersionAndExit_out,
                     unsigned int& numberOfDispatchThreads_out,
                     bool& debugParser_out)
{
  STREAM_TRACE (ACE_TEXT ("::do_processArguments"));

  std::string working_directory =
    Common_File_Tools::getWorkingDirectory ();
  std::string temp_directory = Common_File_Tools::getTempDirectory ();
  std::string configuration_directory = working_directory;
  configuration_directory += ACE_DIRECTORY_SEPARATOR_STR_A;
  configuration_directory +=
    ACE_TEXT_ALWAYS_CHAR (COMMON_LOCATION_CONFIGURATION_DIRECTORY);

  // initialize results
  bufferSize_out = HTTP_BUFFER_SIZE;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  showConsole_out = false;
#endif
  outputFileName_out = temp_directory;
  outputFileName_out += ACE_DIRECTORY_SEPARATOR_STR_A;
  outputFileName_out += ACE_TEXT_ALWAYS_CHAR (HTTP_GET_DEFAULT_OUTPUT_FILE);
  interfaceDefinitionFile_out = configuration_directory;
  interfaceDefinitionFile_out += ACE_DIRECTORY_SEPARATOR_STR_A;
  interfaceDefinitionFile_out +=
    ACE_TEXT_ALWAYS_CHAR (HTTPGET_UI_DEFINITION_FILE_NAME);
  hostName_out.clear ();
  logToFile_out = false;
  useThreadPool_out = NET_EVENT_USE_THREAD_POOL;
  port_out = 0;
  useReactor_out = NET_EVENT_USE_REACTOR;
  statisticReportingInterval_out = STREAM_DEFAULT_STATISTIC_REPORTING_INTERVAL;
  traceInformation_out = false;
  URI_out.clear ();
  printVersionAndExit_out = false;
  numberOfDispatchThreads_out =
    TEST_U_DEFAULT_NUMBER_OF_DISPATCHING_THREADS;
  debugParser_out = STREAM_DECODER_DEFAULT_YACC_TRACE;

  ACE_Get_Opt argument_parser (argc_in,
                               argv_in,
#if defined (ACE_WIN32) || defined (ACE_WIN64)
                               ACE_TEXT ("b:cf:g::lors:tu:vx:z"),
#else
                               ACE_TEXT ("b:f:g::lors:tu:vx:z"),
#endif
                               1,                         // skip command name
                               1,                         // report parsing errors
                               ACE_Get_Opt::PERMUTE_ARGS, // ordering
                               0);                        // for now, don't use long options

  int option = 0;
  std::stringstream converter;
  while ((option = argument_parser ()) != EOF)
  {
    switch (option)
    {
      case 'b':
      {
        converter.clear ();
        converter.str (ACE_TEXT_ALWAYS_CHAR (""));
        converter << argument_parser.opt_arg ();
        converter >> bufferSize_out;
        break;
      }
#if defined (ACE_WIN32) || defined (ACE_WIN64)
      case 'c':
      {
        showConsole_out = true;
        break;
      }
#endif
      case 'f':
      {
        outputFileName_out = ACE_TEXT_ALWAYS_CHAR (argument_parser.opt_arg ());
        break;
      }
      case 'g':
      {
        ACE_TCHAR* opt_arg = argument_parser.opt_arg ();
        if (opt_arg)
          interfaceDefinitionFile_out = ACE_TEXT_ALWAYS_CHAR (opt_arg);
        else
          interfaceDefinitionFile_out.clear ();
        break;
      }
      case 'l':
      {
        logToFile_out = true;
        break;
      }
      case 'o':
      {
        useThreadPool_out = true;
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
        converter << argument_parser.opt_arg ();
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
        // step1: parse URL
        URI_out = ACE_TEXT_ALWAYS_CHAR (argument_parser.opt_arg ());
        std::string regex_string =
          ACE_TEXT_ALWAYS_CHAR ("^(?:http://)?([[:alnum:]-.]+)(?:\\:([[:digit:]]{1,5}))?(.+)?$");
        std::regex regex (regex_string);
        std::smatch match_results;
        if (!std::regex_match (URI_out,
                               match_results,
                               regex,
                               std::regex_constants::match_default))
        {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("invalid URL string (was: \"%s\"), aborting\n"),
                      ACE_TEXT (URI_out.c_str ())));
          return false;
        } // end IF
        ACE_ASSERT (match_results.ready () && !match_results.empty ());

        ACE_ASSERT (match_results[1].matched);
        hostName_out = match_results[1];
        if (match_results[2].matched)
        {
          converter.clear ();
          converter.str (ACE_TEXT_ALWAYS_CHAR (""));
          converter << match_results[2].str ();
          converter >> port_out;
        } // end IF
        ACE_ASSERT (match_results[3].matched);
//        URI_out = match_results[3];

        // step2: validate address/verify host name exists
        //        --> resolve
        std::string dotted_decimal_string;
        // *TODO*: support IPv6 as well
        regex_string =
          ACE_TEXT_ALWAYS_CHAR ("^([[:digit:]]{1,3}\\.){4}$");
        regex = regex_string;
        std::smatch match_results_2;
        if (std::regex_match (hostName_out,
                              match_results_2,
                              regex,
                              std::regex_constants::match_default))
        {
          ACE_ASSERT (match_results_2.ready ()  &&
                      !match_results_2.empty () &&
                      match_results_2[1].matched);
          dotted_decimal_string = hostName_out;
        } // end IF
        if (!Net_Common_Tools::getAddress (hostName_out,
                                           dotted_decimal_string))
        {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("failed to Net_Common_Tools::getAddress(), aborting\n")));
          return false;
        } // end IF

        // step3: validate URI
//        regex_string =
//            ACE_TEXT_ALWAYS_CHAR ("^(\\/.+(?=\\/))*\\/(.+?)(\\.(html|htm))?$");
        regex_string =
            ACE_TEXT_ALWAYS_CHAR ("^(?:http://)?((.+\\.)+([^\\/]+))(\\/.+(?=\\/))*\\/(.+?)(\\.(html|htm))?$");
        regex.assign (regex_string,
                      (std::regex_constants::ECMAScript |
                       std::regex_constants::icase));
        std::smatch match_results_3;
        if (!std::regex_match (URI_out,
                               match_results_3,
                               regex,
                               std::regex_constants::match_default))
        {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("invalid URI (was: \"%s\"), aborting\n"),
                      ACE_TEXT (URI_out.c_str ())));
          return false;
        } // end IF
        ACE_ASSERT (match_results_3.ready () && !match_results_3.empty ());

        if (!match_results_3[2].matched)
          URI_out += ACE_TEXT_ALWAYS_CHAR (HTTP_GET_DEFAULT_URI);
        //else if (!match_results_3[3].matched)
        //  URI_out += ACE_TEXT_ALWAYS_CHAR (HTML_DEFAULT_SUFFIX);

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
        converter << argument_parser.opt_arg ();
        converter >> numberOfDispatchThreads_out;
        break;
      }
      case 'z':
      {
        debugParser_out = true;
        break;
      }
      // error handling
      case ':':
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("option \"%c\" requires an argument, aborting\n"),
                    argument_parser.opt_opt ()));
        return false;
      }
      case '?':
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("unrecognized option \"%s\", aborting\n"),
                    ACE_TEXT (argument_parser.last_option ())));
        return false;
      }
      case 0:
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("found long option \"%s\", aborting\n"),
                    ACE_TEXT (argument_parser.long_option ())));
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
do_initializeSignals (bool useReactor_in,
                      bool allowUserRuntimeConnect_in,
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
  signals_out.sig_del (SIGSTOP);           // 19      /* Stop process */

  // *IMPORTANT NOTE*: "...NPTL makes internal use of the first two real-time
  //                   signals (see also signal(7)); these signals cannot be
  //                   used in applications. ..." (see 'man 7 pthreads')
  // --> on POSIX platforms, make sure that ACE_SIGRTMIN == 34
  //  for (int i = ACE_SIGRTMIN;
  //       i <= ACE_SIGRTMAX;
  //       i++)
  //    signals_out.sig_del (i);

  if (!useReactor_in)
  {
    ACE_Proactor* proactor_p = ACE_Proactor::instance ();
    ACE_ASSERT (proactor_p);
    ACE_POSIX_Proactor* proactor_impl_p =
      dynamic_cast<ACE_POSIX_Proactor*> (proactor_p->implementation ());
    ACE_ASSERT (proactor_impl_p);
    if (proactor_impl_p->get_impl_type () == ACE_POSIX_Proactor::PROACTOR_SIG)
      signals_out.sig_del (COMMON_EVENT_PROACTOR_SIG_RT_SIGNAL);
  } // end IF
#endif

  // *NOTE*: gdb sends some signals (when running in an IDE ?)
  //         --> remove signals (and let IDE handle them)
  // *TODO*: clean this up
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
#if defined (DEBUG_DEBUGGER)
  //  signals_out.sig_del (SIGINT);
  signals_out.sig_del (SIGCONT);
  signals_out.sig_del (SIGHUP);
#endif
#endif

  // *TODO*: improve valgrind support
#ifdef LIBACESTREAM_ENABLE_VALGRIND_SUPPORT
  // *NOTE*: valgrind uses SIGRT32 (--> SIGRTMAX ?) and apparently will not work
  // if the application installs its own handler (see documentation)
  if (RUNNING_ON_VALGRIND)
    signals_out.sig_del (SIGRTMAX);        // 64
#endif
}

void
do_work (unsigned int bufferSize_in,
         const std::string& fileName_in,
         const std::string& hostName_in,
         bool useThreadPool_in,
         unsigned short port_in,
         bool useReactor_in,
         unsigned int statisticReportingInterval_in,
         const std::string& URL_in,
         unsigned int numberOfDispatchThreads_in,
         bool debugParser_in,
         const std::string& interfaceDefinitionFile_in,
         struct HTTPGet_GtkCBData& CBData_in,
         const ACE_Sig_Set& signalSet_in,
         const ACE_Sig_Set& ignoredSignalSet_in,
         Common_SignalActions_t& previousSignalActions_inout,
#if defined (ACE_WIN32) || defined (ACE_WIN64)
         bool showConsole_in,
#endif
         HTTPGet_SignalHandler& signalHandler_in)
{
  STREAM_TRACE (ACE_TEXT ("::do_work"));

  // sanity check(s)
  ACE_ASSERT (CBData_in.configuration);

  // step0a: initialize configuration and stream
  HTTPGet_StreamBase_t* stream_base_p = NULL;
  HTTPGet_Stream_t stream;
  HTTPGet_AsynchStream_t asynch_stream;
  if (useReactor_in)
    stream_base_p = &stream;
  else
    stream_base_p = &asynch_stream;

  HTTPGet_EventHandler event_handler (&CBData_in,
                                      interfaceDefinitionFile_in.empty ());
  std::string module_name = ACE_TEXT_ALWAYS_CHAR ("EventHandler");
  HTTPGet_Module_EventHandler_Module event_handler_module (module_name,
                                                           NULL,
                                                           true);

  Stream_AllocatorHeap_T<struct HTTPGet_AllocatorConfiguration> heap_allocator;
  if (!heap_allocator.initialize (CBData_in.configuration->allocatorConfiguration))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to initialize message allocator, returning\n")));
    return;
  } // end IF
  HTTPGet_MessageAllocator_t message_allocator (NET_STREAM_MAX_MESSAGES, // maximum #buffers
                                                &heap_allocator,         // heap allocator handle
                                                true);                   // block ?

  HTTPGet_ConnectionManager_t* connection_manager_p =
    HTTPGET_CONNECTIONMANAGER_SINGLETON::instance ();
  ACE_ASSERT (connection_manager_p);
  connection_manager_p->initialize (std::numeric_limits<unsigned int>::max ());

  // *********************** socket configuration data ************************
  int result =
    CBData_in.configuration->socketConfiguration.address.set ((port_in ? port_in : HTTP_DEFAULT_SERVER_PORT),
                                                              hostName_in.c_str (),
                                                              1,
                                                              ACE_ADDRESS_FAMILY_INET);
  if (result == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_INET_Addr::set(\"%s:%u\"): \"%m\", returning\n"),
                ACE_TEXT (hostName_in.c_str ()),
                port_in));
    return;
  } // end IF
  CBData_in.configuration->socketConfiguration.useLoopBackDevice =
    CBData_in.configuration->socketConfiguration.address.is_loopback ();
  // ******************** socket handler configuration data *******************
  CBData_in.configuration->connectionConfiguration.streamConfiguration =
    &CBData_in.configuration->streamConfiguration;

  CBData_in.configuration->socketHandlerConfiguration.connectionConfiguration =
    &CBData_in.configuration->connectionConfiguration;
  CBData_in.configuration->socketHandlerConfiguration.messageAllocator =
      &message_allocator;
  CBData_in.configuration->socketHandlerConfiguration.PDUSize = bufferSize_in;
  CBData_in.configuration->socketHandlerConfiguration.socketConfiguration =
    &CBData_in.configuration->socketConfiguration;
  CBData_in.configuration->socketHandlerConfiguration.statisticReportingInterval =
      statisticReportingInterval_in;

  // ********************** stream configuration data **************************
  // ********************** prser configuration data ***************************
  CBData_in.configuration->parserConfiguration.debugParser = debugParser_in;
  if (debugParser_in)
    CBData_in.configuration->parserConfiguration.debugScanner = true;
  // ********************** module configuration data **************************
  CBData_in.configuration->moduleConfiguration.streamConfiguration =
      &CBData_in.configuration->streamConfiguration;

  CBData_in.configuration->moduleHandlerConfiguration.streamConfiguration =
      &CBData_in.configuration->streamConfiguration;

  CBData_in.configuration->moduleHandlerConfiguration.parserConfiguration =
      &CBData_in.configuration->parserConfiguration;
  CBData_in.configuration->moduleHandlerConfiguration.passive = false;
  CBData_in.configuration->moduleHandlerConfiguration.configuration =
    CBData_in.configuration;
  CBData_in.configuration->moduleHandlerConfiguration.connectionManager =
      connection_manager_p;
  CBData_in.configuration->moduleHandlerConfiguration.targetFileName =
    fileName_in;
  CBData_in.configuration->moduleHandlerConfiguration.hostName = hostName_in;
  CBData_in.configuration->moduleHandlerConfiguration.URL = URL_in;
  CBData_in.configuration->moduleHandlerConfiguration.socketConfiguration =
      &CBData_in.configuration->socketConfiguration;
  CBData_in.configuration->moduleHandlerConfiguration.socketHandlerConfiguration =
      &CBData_in.configuration->socketHandlerConfiguration;
  CBData_in.configuration->moduleHandlerConfiguration.stream = stream_base_p;
  // ******************** (sub-)stream configuration data *********************
  if (bufferSize_in)
    CBData_in.configuration->allocatorConfiguration.defaultBufferSize =
      bufferSize_in;

  CBData_in.configuration->streamConfiguration.allocatorConfiguration =
      &CBData_in.configuration->allocatorConfiguration;
  CBData_in.configuration->streamConfiguration.messageAllocator =
    &message_allocator;
  CBData_in.configuration->streamConfiguration.moduleConfiguration =
      &CBData_in.configuration->moduleConfiguration;
  CBData_in.configuration->streamConfiguration.moduleHandlerConfigurations.insert (std::make_pair (ACE_TEXT_ALWAYS_CHAR (""),
    &CBData_in.configuration->moduleHandlerConfiguration));
  CBData_in.configuration->streamConfiguration.printFinalReport = true;
  CBData_in.configuration->streamConfiguration.statisticReportingInterval =
      statisticReportingInterval_in;
  CBData_in.configuration->streamConfiguration.userData =
    &CBData_in.configuration->userData;

  //module_handler_p->initialize (configuration.moduleHandlerConfiguration);

  // step0c: initialize connection manager
  connection_manager_p->initialize (std::numeric_limits<unsigned int>::max ());
  connection_manager_p->set (CBData_in.configuration->connectionConfiguration,
    &CBData_in.configuration->userData);

  Common_Timer_Manager_t* timer_manager_p =
    COMMON_TIMERMANAGER_SINGLETON::instance ();
  ACE_ASSERT (timer_manager_p);
  struct Common_TimerConfiguration timer_configuration;
  int group_id = -1;
  struct Common_DispatchThreadData thread_data;

  HTTPGet_GTK_Manager_t* gtk_manager_p = NULL;

  // step0b: initialize event dispatch
  thread_data.numberOfDispatchThreads = numberOfDispatchThreads_in;
  thread_data.useReactor = useReactor_in;
  if (!Common_Tools::initializeEventDispatch (thread_data.useReactor,
                                              useThreadPool_in,
                                              thread_data.numberOfDispatchThreads,
                                              thread_data.proactorType, // *NOTE*: return value
                                              thread_data.reactorType, // *NOTE*: return value
                                              CBData_in.configuration->streamConfiguration.serializeOutput)) // *NOTE*: return value
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_Tools::initializeEventDispatch(), returning\n")));
    goto clean;
  } // end IF

  //// step0d: initialize regular (global) statistic reporting
  //Stream_StatisticHandler_Reactor_t statistic_handler (ACTION_REPORT,
  //                                                     connection_manager_p,
  //                                                     false);
  ////Stream_StatisticHandler_Proactor_t statistic_handler_proactor (ACTION_REPORT,
  ////                                                               connection_manager_p,
  ////                                                               false);
  //long timer_id = -1;
  //if (statisticReportingInterval_in)
  //{
  //  ACE_Event_Handler* handler_p = &statistic_handler;
  //  ACE_Time_Value interval (statisticReportingInterval_in, 0);
  //  timer_id =
  //    timer_manager_p->schedule_timer (handler_p,                  // event handler
  //                                     NULL,                       // ACT
  //                                     COMMON_TIME_NOW + interval, // first wakeup time
  //                                     interval);                  // interval
  //  if (timer_id == -1)
  //  {
  //    ACE_DEBUG ((LM_ERROR,
  //                ACE_TEXT ("failed to schedule timer: \"%m\", returning\n")));

  //    // clean up
  //    timer_manager_p->stop ();
  //    delete stream_p;

  //    return;
  //  } // end IF
  //} // end IF

  // step0c: initialize signal handling
  CBData_in.configuration->signalHandlerConfiguration.hasUI =
    !interfaceDefinitionFile_in.empty ();
  CBData_in.configuration->signalHandlerConfiguration.useReactor =
    useReactor_in;
  //configuration.signalHandlerConfiguration.statisticReportingHandler =
  //  connection_manager_p;
  //configuration.signalHandlerConfiguration.statisticReportingTimerID = timer_id;
  if (!signalHandler_in.initialize (CBData_in.configuration->signalHandlerConfiguration))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to initialize signal handler, returning\n")));
    goto clean;
  } // end IF
  if (!Common_Tools::initializeSignals (signalSet_in,
                                        ignoredSignalSet_in,
                                        &signalHandler_in,
                                        previousSignalActions_inout))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_Tools::initializeSignals(), returning\n")));
    goto clean;
  } // end IF

  // intialize timers
  timer_manager_p->initialize (timer_configuration);
  timer_manager_p->start ();

  // step1a: start GTK event loop ?
  gtk_manager_p =
      HTTPGET_UI_GTK_MANAGER_SINGLETON::instance ();
  ACE_ASSERT (gtk_manager_p);
  if (!interfaceDefinitionFile_in.empty ())
  {
    gtk_manager_p->start ();
    ACE_Time_Value delay (0,
                          HTTPGET_UI_INITIALIZATION_DELAY);
    result = ACE_OS::sleep (delay);
    if (result == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_OS::sleep(%#T): \"%m\", continuing\n"),
                  &delay));
    if (!gtk_manager_p->isRunning ())
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to start GTK event dispatch, returning\n")));
      goto clean;
    } // end IF

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
#endif
  } // end IF

  // *WARNING*: from this point on, clean up any remote connections !

  // step1: handle events (signals, incoming connections/data, timers, ...)
  // reactor/proactor event loop:
  // - dispatch connection attempts to acceptor
  // - dispatch socket events
  // timer events:
  // - perform statistics collecting/reporting

  // step1a: initialize worker(s)
  if (!Common_Tools::startEventDispatch (thread_data,
                                         group_id))
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

    goto clean;
  } // end IF

  if (interfaceDefinitionFile_in.empty ())
  {
    // initialize processing stream
    if (!stream_base_p->initialize (CBData_in.configuration->streamConfiguration))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to initialize stream, returning\n")));
      goto clean;
    } // end IF

    stream_base_p->start ();
    //if (!stream_base_p->isRunning ())
    //{
    //  ACE_DEBUG ((LM_ERROR,
    //              ACE_TEXT ("failed to start stream, aborting\n")));
    //  goto clean;
    //} // end IF
    stream_base_p->wait (true,   // wait for any worker thread(s) ?
                         false,  // wait for upstream (if any) ?
                         false); // wait for downstream (if any) ?
  } // end IF
  else
    gtk_manager_p->wait ();

  // step3: clean up
  connection_manager_p->stop ();
  //Common_Tools::finalizeEventDispatch (useReactor_in,
  //                                     !useReactor_in,
  //                                     group_id);
  connection_manager_p->wait ();

  timer_manager_p->stop ();

  //		{ // synch access
  //			ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard(CBData_in.lock);

  //			for (Net_GTK_EventSourceIDsIterator_t iterator = CBData_in.event_source_ids.begin();
  //					 iterator != CBData_in.event_source_ids.end();
  //					 iterator++)
  //				g_source_remove(*iterator);
  //		} // end lock scope
  //timer_manager_p->stop ();

  //  { // synch access
  //    ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard(CBData_in.lock);

  //		for (Net_GTK_EventSourceIDsIterator_t iterator = CBData_in.event_source_ids.begin();
  //				 iterator != CBData_in.event_source_ids.end();
  //				 iterator++)
  //			g_source_remove(*iterator);
  //	} // end lock scope

  //result = event_handler.close (ACE_Module_Base::M_DELETE_NONE);
  //if (result == -1)
  //  ACE_DEBUG ((LM_ERROR,
  //              ACE_TEXT ("%s: failed to ACE_Module::close (): \"%m\", continuing\n"),
  //              event_handler.name ()));

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("finished working...\n")));

  return;

clean:
  Common_Tools::finalizeEventDispatch (useReactor_in,
                                       !useReactor_in,
                                       group_id);
  timer_manager_p->stop ();
  if (!interfaceDefinitionFile_in.empty ())
    HTTPGET_UI_GTK_MANAGER_SINGLETON::instance ()->stop ();
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
            << ACE_TEXT (LIBACESTREAM_PACKAGE)
            << ACE_TEXT (": ")
            << ACE_TEXT (LIBACESTREAM_PACKAGE_VERSION)
            << std::endl
#endif
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

  int result;

  unsigned int buffer_size;
  bool use_reactor;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  bool show_console;
#endif
  bool log_to_file;
  bool use_thread_pool;
  unsigned int statistic_reporting_interval;
  unsigned int number_of_dispatch_threads;
  bool debug_parser;
  bool trace_information;
  bool print_version_and_exit;
  std::string configuration_directory;
  std::string working_directory;
  std::string temp_directory;
  std::string output_file_path;
  std::string UI_file_path;
  std::string host_name;
  unsigned short port;
  std::string URL;
  ACE_Sig_Set signal_set (0);
  ACE_Sig_Set ignored_signal_set (0);
  Common_SignalActions_t previous_signal_actions;
  sigset_t previous_signal_mask;
  std::string log_file_name;
  struct HTTPGet_GtkCBData gtk_cb_data;
  //Common_Logger_t logger (&gtk_cb_data.logStack,
  //                        &gtk_cb_data.lock);
  HTTPGet_GtkBuilderDefinition_t ui_definition (argc_in,
                                                argv_in);
  struct HTTPGet_GtkProgressData gtk_progress_data;
  HTTPGet_SignalHandler signal_handler;
  struct HTTPGet_Configuration configuration;
  //struct HTTPGet_UserData user_data;
  ACE_Profile_Timer process_profile;
  ACE_High_Res_Timer timer;
  std::string working_time_string;
  ACE_Time_Value working_time;
  ACE_Time_Value user_time;
  ACE_Time_Value system_time;
  std::string user_time_string;
  std::string system_time_string;
  ACE_Profile_Timer::ACE_Elapsed_Time elapsed_time;
  ACE_Profile_Timer::Rusage elapsed_rusage;

  // step0: initialize
  // *PORTABILITY*: on Windows, initialize ACE
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  result = ACE::init ();
  if (result == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE::init(): \"%m\", aborting\n")));
    return EXIT_FAILURE;
  } // end IF
#endif

#if defined (LIBACESTREAM_ENABLE_VALGRIND_SUPPORT)
  if (RUNNING_ON_VALGRIND)
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("running on valgrind...\n")));
#endif

  // step0: process profile
  result = process_profile.start ();
  if (result == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Profile_Timer::start(): \"%m\", aborting\n")));
    goto error;
  } // end IF

  Common_Tools::initialize ();

  gtk_cb_data.configuration = &configuration;
  working_directory = Common_File_Tools::getWorkingDirectory ();
  temp_directory = Common_File_Tools::getTempDirectory ();
  configuration_directory = working_directory;
  configuration_directory += ACE_DIRECTORY_SEPARATOR_STR_A;
  configuration_directory +=
    ACE_TEXT_ALWAYS_CHAR (COMMON_LOCATION_CONFIGURATION_DIRECTORY);

  // step1a: set defaults
  buffer_size = HTTP_BUFFER_SIZE;
  output_file_path = temp_directory;
  output_file_path += ACE_DIRECTORY_SEPARATOR_STR_A;
  output_file_path +=
    ACE_TEXT_ALWAYS_CHAR (HTTP_GET_DEFAULT_OUTPUT_FILE);
  UI_file_path = configuration_directory;
  UI_file_path += ACE_DIRECTORY_SEPARATOR_STR_A;
  UI_file_path += ACE_TEXT_ALWAYS_CHAR (HTTPGET_UI_DEFINITION_FILE_NAME);
  log_to_file = false;
  use_thread_pool = NET_EVENT_USE_THREAD_POOL;
  use_reactor = NET_EVENT_USE_REACTOR;
  statistic_reporting_interval =
    STREAM_DEFAULT_STATISTIC_REPORTING_INTERVAL;
  trace_information = false;
  print_version_and_exit = false;
  number_of_dispatch_threads =
    TEST_U_DEFAULT_NUMBER_OF_DISPATCHING_THREADS;
  debug_parser = STREAM_DECODER_DEFAULT_YACC_TRACE;
  ACE_OS::memset (&elapsed_rusage, 0, sizeof (elapsed_rusage));

  // step1b: parse/process/validate configuration
  if (!do_processArguments (argc_in,
                            argv_in,
                            buffer_size,
#if defined (ACE_WIN32) || defined (ACE_WIN64)
                            show_console,
#endif
                            output_file_path,
                            UI_file_path,
                            host_name,
                            log_to_file,
                            use_thread_pool,
                            port,
                            use_reactor,
                            statistic_reporting_interval,
                            trace_information,
                            URL,
                            print_version_and_exit,
                            number_of_dispatch_threads,
                            debug_parser))
  {
    // help the user: print usage instructions
    do_printUsage (ACE::basename (argv_in[0]));

    goto error;
  } // end IF

  // step1c: validate arguments
  // *IMPORTANT NOTE*: iff the number of message buffers is limited, the
  //                   reactor/proactor thread could (dead)lock on the
  //                   allocator lock, as it cannot dispatch events that would
  //                   free slots
  if (NET_STREAM_MAX_MESSAGES)
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
  if ((!UI_file_path.empty () &&
       !Common_File_Tools::isReadable (UI_file_path))                      ||
      (host_name.empty () && URL.empty ())                                 ||
      (use_reactor && (number_of_dispatch_threads > 1) && !use_thread_pool))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("invalid arguments, aborting\n")));

    // help the user: print usage instructions
    do_printUsage (ACE::basename (argv_in[0]));

    goto error;
  } // end IF
  if (number_of_dispatch_threads == 0) number_of_dispatch_threads = 1;

  // step2: run program ?
  if (print_version_and_exit)
  {
    do_printVersion (ACE_TEXT_ALWAYS_CHAR (ACE::basename (argv_in[0],
                                                          ACE_DIRECTORY_SEPARATOR_CHAR)));
    goto done;
  } // end IF

  // step3: initialize logging and/or tracing
  if (log_to_file)
    log_file_name =
      Common_File_Tools::getLogFilename (ACE_TEXT_ALWAYS_CHAR (LIBACESTREAM_PACKAGE_NAME),
                                         ACE_TEXT_ALWAYS_CHAR (ACE::basename (argv_in[0],
                                                                              ACE_DIRECTORY_SEPARATOR_CHAR)));
  if (!Common_Tools::initializeLogging (ACE::basename (argv_in[0]),           // program name
                                        log_file_name,                        // log file name
                                        false,                                // log to syslog ?
                                        false,                                // trace messages ?
                                        trace_information,                    // debug messages ?
                                        NULL))
                                        //(interface_definition_file.empty () ? NULL
                                        //                                    : &logger))) // logger ?
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_Tools::initializeLogging(), aborting\n")));
    goto error;
  } // end IF

  // step4: pre-initialize signal handling
  do_initializeSignals (use_reactor,
                        true, // allow SIGUSR1/SIGBREAK
                        signal_set,
                        ignored_signal_set);
  result = ACE_OS::sigemptyset (&previous_signal_mask);
  if (result == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::sigemptyset(): \"%m\", aborting\n")));

    // clean up
    Common_Tools::finalizeLogging ();

    goto error;
  } // end IF
  if (!Common_Tools::preInitializeSignals (signal_set,
                                           use_reactor,
                                           previous_signal_actions,
                                           previous_signal_mask))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_Tools::preInitializeSignals(), aborting\n")));

    // clean up
    Common_Tools::finalizeLogging ();

    goto error;
  } // end IF

  // step5: set process resource limits
  // *NOTE*: settings will be inherited by any child processes
  if (!Common_Tools::setResourceLimits (false, // file descriptors
                                        true,  // stack traces
                                        true)) // pending signals
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_Tools::setResourceLimits(), aborting\n")));

    // clean up
    Common_Tools::finalizeSignals (signal_set,
                                   previous_signal_actions,
                                   previous_signal_mask);
    Common_Tools::finalizeLogging ();

    goto error;
  } // end IF

  // step6: initialize configuration
  //configuration.userData = &user_data;

  // step7: initialize user interface, if any
  if (UI_file_path.empty ())
    goto continue_;

  gtk_cb_data.argc = argc_in;
  gtk_cb_data.argv = argv_in;
  //ACE_OS::memset (&gtk_cb_data.clientSensorBias,
  //                0,
  //                sizeof (gtk_cb_data.clientSensorBias));
  gtk_cb_data.builders[ACE_TEXT_ALWAYS_CHAR (COMMON_UI_GTK_DEFINITION_DESCRIPTOR_MAIN)] =
    std::make_pair (UI_file_path, static_cast<GtkBuilder*> (NULL));
  gtk_cb_data.finalizationHook = idle_finalize_ui_cb;
  gtk_cb_data.initializationHook = idle_initialize_ui_cb;
  gtk_cb_data.progressData = &gtk_progress_data;
  gtk_cb_data.userData = &gtk_cb_data;
  gtk_progress_data.GTKState = &gtk_cb_data;
  HTTPGET_UI_GTK_MANAGER_SINGLETON::instance ()->initialize (argc_in,
                                                             argv_in,
                                                             &gtk_cb_data,
                                                             &ui_definition);

continue_:
  timer.start ();
  // step8: run program
  do_work (buffer_size,
           output_file_path,
           host_name,
           use_thread_pool,
           port,
           use_reactor,
           statistic_reporting_interval,
           URL,
           number_of_dispatch_threads,
           debug_parser,
           UI_file_path,
           gtk_cb_data,
           signal_set,
           ignored_signal_set,
           previous_signal_actions,
#if defined (ACE_WIN32) || defined (ACE_WIN64)
           show_console,
#endif
           signal_handler);
  timer.stop ();

  // debug info
  timer.elapsed_time (working_time);
  Common_Tools::period2String (working_time,
                               working_time_string);

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("total working time (h:m:s.us): \"%s\"...\n"),
              ACE_TEXT (working_time_string.c_str ())));

done:
  process_profile.stop ();

  // debug info
  elapsed_time.real_time = 0.0;
  elapsed_time.user_time = 0.0;
  elapsed_time.system_time = 0.0;
  result = process_profile.elapsed_time (elapsed_time);
  if (result == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Profile_Timer::elapsed_time: \"%m\", aborting\n")));

    // clean up
    Common_Tools::finalizeSignals (signal_set,
                                   previous_signal_actions,
                                   previous_signal_mask);
    Common_Tools::finalizeLogging ();

    goto error;
  } // end IF
  process_profile.elapsed_rusage (elapsed_rusage);
  user_time.set (elapsed_rusage.ru_utime);
  system_time.set (elapsed_rusage.ru_stime);
  Common_Tools::period2String (user_time,
                               user_time_string);
  Common_Tools::period2String (system_time,
                               system_time_string);
#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
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
#else
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT (" --> Process Profile <--\nreal time = %A seconds\nuser time = %A seconds\nsystem time = %A seconds\n --> Resource Usage <--\nuser time used: %s\nsystem time used: %s\n"),
              elapsed_time.real_time,
              elapsed_time.user_time,
              elapsed_time.system_time,
              ACE_TEXT (user_time_string.c_str ()),
              ACE_TEXT (system_time_string.c_str ())));
#endif

  // step9: clean up
  Common_Tools::finalizeSignals (signal_set,
                                 previous_signal_actions,
                                 previous_signal_mask);
  Common_Tools::finalizeLogging ();

  // *PORTABILITY*: on Windows, finalize ACE
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  result = ACE::fini ();
  if (result == -1)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE::fini(): \"%m\", continuing\n")));
#endif

  return EXIT_SUCCESS;

error:
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  result = ACE::fini ();
  if (result == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE::fini(): \"%m\", aborting\n")));
    return EXIT_FAILURE;
  } // end IF
#endif

  return EXIT_FAILURE;
} // end main
