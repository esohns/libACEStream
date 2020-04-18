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
#include <limits>
#include <regex>
#include <string>

#include "ace/Get_Opt.h"
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include "ace/Init_ACE.h"
#endif // ACE_WIN32 || ACE_WIN64
#include "ace/Log_Msg.h"
#include "ace/Profile_Timer.h"
#include "ace/Sig_Handler.h"
#include "ace/Signal.h"
//#include "ace/Synch.h"
#include "ace/Version.h"

#if defined (HAVE_CONFIG_H)
#include "Common_config.h"
#endif // HAVE_CONFIG_H

#include "common.h"
//#include "common_file_tools.h"
#include "common_tools.h"

#include "common_log_tools.h"
#include "common_logger.h"

#include "common_signal_tools.h"

#include "common_timer_tools.h"

#include "stream_allocatorheap.h"
#include "stream_macros.h"

#if defined (HAVE_CONFIG_H)
#include "ACEStream_config.h"
#endif // HAVE_CONFIG_H

#include "stream_dec_common.h"

#include "stream_file_sink.h"

#include "stream_stat_common.h"

//#include "stream_net_http_defines.h"

#if defined (HAVE_CONFIG_H)
#include "ACENetwork_config.h"
#endif // HAVE_CONFIG_H

#include "net_common_tools.h"

//#include "test_i_common.h"
#include "test_i_defines.h"

//#include "test_i_module_databasewriter.h"

//#include "test_i_connection_manager_common.h"
#include "test_i_mp3player_common.h"
#include "test_i_mp3player_defines.h"
//#include "test_i_http_get_signalhandler.h"
#include "test_i_stream.h"

const char stream_name_string_[] = ACE_TEXT_ALWAYS_CHAR ("MP3PlayerStream");

void
do_printUsage (const std::string& programName_in)
{
  STREAM_TRACE (ACE_TEXT ("::do_printUsage"));

  // enable verbatim boolean output
  std::cout.setf (std::ios::boolalpha);

  std::string path =
    Common_File_Tools::getWorkingDirectory ();

  std::cout << ACE_TEXT_ALWAYS_CHAR ("usage: ")
            << programName_in
            << ACE_TEXT_ALWAYS_CHAR (" [OPTIONS]")
            << std::endl
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("currently available options:")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-b [VALUE]  : buffer size (byte(s)) [")
            << TEST_I_DEFAULT_BUFFER_SIZE
            << ACE_TEXT_ALWAYS_CHAR ("])")
            << std::endl;
  std::string input_file = path;
  input_file += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  input_file = ACE_TEXT_ALWAYS_CHAR (TEST_I_DEFAULT_INPUT_FILE);
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-f [STRING] : input file name [")
            << input_file
            << ACE_TEXT_ALWAYS_CHAR ("]")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-l          : log to a file [")
            << false
            << ACE_TEXT_ALWAYS_CHAR ("]")
            << std::endl;
  std::string output_file = Common_File_Tools::getTempDirectory ();
  output_file += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  output_file += ACE_TEXT_ALWAYS_CHAR (TEST_I_DEFAULT_OUTPUT_FILE);
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-o [STRING] : output file name [")
            << output_file
            << ACE_TEXT_ALWAYS_CHAR ("]")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-t          : trace information [")
            << false
            << ACE_TEXT_ALWAYS_CHAR ("]")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-v          : print version information and exit [")
            << false
            << ACE_TEXT_ALWAYS_CHAR ("]")
            << std::endl;
}

bool
do_processArguments (int argc_in,
                     ACE_TCHAR** argv_in, // cannot be const...
                     unsigned int& bufferSize_out,
                     std::string& inputFileName_out,
                     bool& logToFile_out,
                     std::string& outputFileName_out,
                     //unsigned int& statisticReportingInterval_out,
                     bool& traceInformation_out,
                     bool& printVersionAndExit_out)
{
  STREAM_TRACE (ACE_TEXT ("::do_processArguments"));

  std::string path = Common_File_Tools::getWorkingDirectory ();

  // initialize results
  bufferSize_out = TEST_I_DEFAULT_BUFFER_SIZE;
  inputFileName_out = path;
  inputFileName_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  inputFileName_out = ACE_TEXT_ALWAYS_CHAR (TEST_I_DEFAULT_INPUT_FILE);
  logToFile_out = false;
  outputFileName_out = Common_File_Tools::getTempDirectory ();
  outputFileName_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  outputFileName_out += ACE_TEXT_ALWAYS_CHAR (TEST_I_DEFAULT_OUTPUT_FILE);
  //statisticReportingInterval_out = STREAM_DEFAULT_STATISTIC_REPORTING_INTERVAL;
  traceInformation_out = false;
  printVersionAndExit_out = false;

  ACE_Get_Opt argumentParser (argc_in,
                              argv_in,
                              ACE_TEXT ("b:f:lo:tv"),
                              1,                         // skip command name
                              1,                         // report parsing errors
                              ACE_Get_Opt::PERMUTE_ARGS, // ordering
                              0);                        // for now, don't use long options

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
      case 'f':
      {
        inputFileName_out = ACE_TEXT_ALWAYS_CHAR (argumentParser.opt_arg ());
        break;
      }
      case 'l':
      {
        logToFile_out = true;
        break;
      }
      case 'o':
      {
        outputFileName_out = ACE_TEXT_ALWAYS_CHAR (argumentParser.opt_arg ());
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

//void
//do_initializeSignals (bool allowUserRuntimeConnect_in,
//                      ACE_Sig_Set& signals_out,
//                      ACE_Sig_Set& ignoredSignals_out)
//{
//  STREAM_TRACE (ACE_TEXT ("::do_initializeSignals"));
//
//  int result = -1;
//
//  // initialize return value(s)
//  result = signals_out.empty_set ();
//  if (result == -1)
//  {
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("failed to ACE_Sig_Set::empty_set(): \"%m\", returning\n")));
//    return;
//  } // end IF
//  result = ignoredSignals_out.empty_set ();
//  if (result == -1)
//  {
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("failed to ACE_Sig_Set::empty_set(): \"%m\", returning\n")));
//    return;
//  } // end IF
//
//  // *PORTABILITY*: on Windows(TM) platforms most signals are not defined, and
//  //                ACE_Sig_Set::fill_set() doesn't really work as specified
//  // --> add valid signals (see <signal.h>)...
//#if defined (ACE_WIN32) || defined (ACE_WIN64)
//  signals_out.sig_add (SIGINT);            // 2       /* interrupt */
//  signals_out.sig_add (SIGILL);            // 4       /* illegal instruction - invalid function image */
//  signals_out.sig_add (SIGFPE);            // 8       /* floating point exception */
//  //  signals_out.sig_add (SIGSEGV);           // 11      /* segment violation */
//  signals_out.sig_add (SIGTERM);           // 15      /* Software termination signal from kill */
//  if (allowUserRuntimeConnect_in)
//  {
//    signals_out.sig_add (SIGBREAK);        // 21      /* Ctrl-Break sequence */
//    ignoredSignals_out.sig_add (SIGBREAK); // 21      /* Ctrl-Break sequence */
//  } // end IF
//  signals_out.sig_add (SIGABRT);           // 22      /* abnormal termination triggered by abort call */
//  signals_out.sig_add (SIGABRT_COMPAT);    // 6       /* SIGABRT compatible with other platforms, same as SIGABRT */
//#else
//  result = signals_out.fill_set ();
//  if (result == -1)
//  {
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("failed to ACE_Sig_Set::fill_set(): \"%m\", returning\n")));
//    return;
//  } // end IF
//  // *NOTE*: cannot handle some signals --> registration fails for these...
//  signals_out.sig_del (SIGKILL);           // 9       /* Kill signal */
//  signals_out.sig_del (SIGSTOP);           // 19      /* Stop process */
//  // ---------------------------------------------------------------------------
//  if (!allowUserRuntimeConnect_in)
//  {
//    signals_out.sig_del (SIGUSR1);         // 10      /* User-defined signal 1 */
//    ignoredSignals_out.sig_add (SIGUSR1);  // 10      /* User-defined signal 1 */
//  } // end IF
//  // *NOTE* core dump on SIGSEGV
//  signals_out.sig_del (SIGSEGV);           // 11      /* Segmentation fault: Invalid memory reference */
//  // *NOTE* don't care about SIGPIPE
//  signals_out.sig_del (SIGPIPE);           // 12      /* Broken pipe: write to pipe with no readers */
//
//#ifdef ENABLE_VALGRIND_SUPPORT
//  // *NOTE*: valgrind uses SIGRT32 (--> SIGRTMAX ?) and apparently will not work
//  // if the application installs its own handler (see documentation)
//  if (RUNNING_ON_VALGRIND)
//    signals_out.sig_del (SIGRTMAX);        // 64
//#endif
//#endif
//}

void
do_work (unsigned int bufferSize_in,
         const std::string& inputFileName_in,
         const std::string& outputFileName_in)
         //unsigned int statisticReportingInterval_in,
         //const ACE_Sig_Set& signalSet_in,
         //const ACE_Sig_Set& ignoredSignalSet_in,
         //Common_SignalActions_t& previousSignalActions_inout,
         //Stream_Source_SignalHandler& signalHandler_in)
{
  STREAM_TRACE (ACE_TEXT ("::do_work"));

  // step0a: initialize configuration and stream
  struct Test_I_MP3Player_Configuration configuration;
  struct Stream_AllocatorConfiguration allocator_configuration;
  Test_I_Stream stream;
  Stream_AllocatorHeap_T<ACE_MT_SYNCH,
                         struct Common_AllocatorConfiguration> heap_allocator;
  if (!heap_allocator.initialize (allocator_configuration))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to initialize heap allocator, returning\n")));
    return;
  } // end IF
  Test_I_MessageAllocator_t message_allocator (TEST_I_MAX_MESSAGES, // maximum #buffers
                                               &heap_allocator,     // heap allocator handle
                                               true);               // block ?

  // ********************** stream configuration data **************************
  // ********************** module configuration data **************************
  struct Stream_ModuleConfiguration module_configuration;
  struct Test_I_MP3Player_ModuleHandlerConfiguration modulehandler_configuration;
  modulehandler_configuration.allocatorConfiguration =
    &configuration.allocatorConfiguration;
  modulehandler_configuration.fileIdentifier.identifier = inputFileName_in;
  //modulehandler_configuration.statisticReportingInterval =
  //  statisticReportingInterval_in;
  // ******************** (sub-)stream configuration data *********************
  if (bufferSize_in)
    allocator_configuration.defaultBufferSize = bufferSize_in;

  struct Test_I_MP3Player_StreamConfiguration stream_configuration;
  stream_configuration.messageAllocator = &message_allocator;
  //stream_configuration.module = module_p;
  stream_configuration.printFinalReport = true;
  stream_configuration.fileIdentifier.identifier = outputFileName_in;
  configuration.streamConfiguration.initialize (module_configuration,
                                                modulehandler_configuration,
                                                stream_configuration);

  //module_handler_p->initialize (configuration.moduleHandlerConfiguration);

  // step0d: initialize regular (global) statistic reporting
  Common_Timer_Manager_t* timer_manager_p =
      COMMON_TIMERMANAGER_SINGLETON::instance ();
  ACE_ASSERT (timer_manager_p);
  Common_TimerConfiguration timer_configuration;
  timer_manager_p->initialize (timer_configuration);
  ACE_thread_t thread_id = 0;
  timer_manager_p->start (thread_id);
  ACE_UNUSED_ARG (thread_id);
  Stream_StatisticHandler_t statistic_handler (COMMON_STATISTIC_ACTION_REPORT,
                                               &stream,
                                               false);
  long timer_id = -1;
  //if (statisticReportingInterval_in)
  //{
  //  ACE_Time_Value interval (statisticReportingInterval_in, 0);
  //  timer_id =
  //    timer_manager_p->schedule_timer (&statistic_handler,         // event handler handle
  //                                     NULL,                       // asynchronous completion token
  //                                     COMMON_TIME_NOW + interval, // first wakeup time
  //                                     interval);                  // interval
  //  if (timer_id == -1)
  //  {
  //    ACE_DEBUG ((LM_ERROR,
  //                ACE_TEXT ("failed to schedule timer: \"%m\", returning\n")));
  //    timer_manager_p->stop ();
  //    return;
  //  } // end IF
  //} // end IF

  // step0c: initialize signal handling
  //struct Common_EventDispatchState dispatch_state_s;
  //dispatch_state_s.configuration = &configuration.dispatchConfiguration;
  //configuration.signalHandlerConfiguration.dispatchState = &dispatch_state_s;
  //configuration.signalHandlerConfiguration.statisticReportingHandler =
  //  connection_manager_p;
  //configuration.signalHandlerConfiguration.statisticReportingTimerID = timer_id;
  //if (!signalHandler_in.initialize (configuration.signalHandlerConfiguration))
  //{
  //  ACE_DEBUG ((LM_ERROR,
  //              ACE_TEXT ("failed to initialize signal handler, returning\n")));
  //  timer_manager_p->stop ();
  //  return;
  //} // end IF
  //if (!Common_Signal_Tools::initialize ((useReactor_in ? COMMON_SIGNAL_DISPATCH_REACTOR
  //                                                     : COMMON_SIGNAL_DISPATCH_PROACTOR),
  //                                      signalSet_in,
  //                                      ignoredSignalSet_in,
  //                                      &signalHandler_in,
  //                                      previousSignalActions_inout))
  //{
  //  ACE_DEBUG ((LM_ERROR,
  //              ACE_TEXT ("failed to Common_Signal_Tools::initialize(), returning\n")));
  //  timer_manager_p->stop ();
  //  return;
  //} // end IF

  // step1: handle events (signals, incoming connections/data, timers, ...)
  // reactor/proactor event loop:
  // - dispatch connection attempts to acceptor
  // - dispatch socket events
  // timer events:
  // - perform statistics collecting/reporting

  // step1a: initialize worker(s)
  if (!stream.initialize (configuration.streamConfiguration))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to initialize stream, returning\n")));
    timer_manager_p->stop ();
    return;
  } // end IF

  // *NOTE*: this call blocks until the file has been sent (or an error
  //         occurs)
  stream.start ();
  //    if (!stream_p->isRunning ())
  //    {
  //      ACE_DEBUG ((LM_ERROR,
  //                  ACE_TEXT ("failed to start stream, aborting\n")));

  //      // clean up
  //      //timer_manager_p->stop ();

  //      return;
  //    } // end IF
  stream.wait (true,   // wait for any worker thread(s) ?
               false,  // wait for upstream (if any) ?
               false); // wait for downstream (if any) ?

  // clean up
  timer_manager_p->stop ();

  //result = event_handler.close (ACE_Module_Base::M_DELETE_NONE);
  //if (result == -1)
  //  ACE_DEBUG ((LM_ERROR,
  //              ACE_TEXT ("%s: failed to ACE_Module::close (): \"%m\", continuing\n"),
  //              event_handler.name ()));

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
#endif
  Common_Tools::initialize ();

  // *PROCESS PROFILE*
  ACE_Profile_Timer process_profile;
  // start profile timer...
  process_profile.start ();

  std::string configuration_path =
    Common_File_Tools::getWorkingDirectory ();

  // step1a set defaults
  unsigned int buffer_size = TEST_I_DEFAULT_BUFFER_SIZE;
  std::string path = configuration_path;
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  path += ACE_TEXT_ALWAYS_CHAR (COMMON_LOCATION_CONFIGURATION_SUBDIRECTORY);
  std::string input_file = ACE_TEXT_ALWAYS_CHAR (TEST_I_DEFAULT_INPUT_FILE);
  bool log_to_file = false;
  std::string output_file = ACE_TEXT_ALWAYS_CHAR (TEST_I_DEFAULT_OUTPUT_FILE);
  //unsigned int statistic_reporting_interval =
  //  STREAM_DEFAULT_STATISTIC_REPORTING_INTERVAL;
  bool trace_information = false;
  bool print_version_and_exit = false;

  // step1b: parse/process/validate configuration
  if (!do_processArguments (argc_in,
                            argv_in,
                            buffer_size,
                            input_file,
                            log_to_file,
                            output_file,
                            //statistic_reporting_interval,
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
#endif
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
//  if (use_reactor                      &&
//      (number_of_dispatch_threads > 1) &&
//      !use_thread_pool)
//  { // *NOTE*: see also: man (2) select
//    // *TODO*: verify this for MS Windows based systems
//    ACE_DEBUG ((LM_WARNING,
//                ACE_TEXT ("the select()-based reactor is not reentrant, using the thread-pool reactor instead...\n")));
//    use_thread_pool = true;
//  } // end IF
  if ((!input_file.empty () && !Common_File_Tools::isReadable (input_file)) ||
      false)
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
#endif
    return EXIT_FAILURE;
  } // end IF

  // step1d: initialize logging and/or tracing
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
                                            NULL))                                // logger ?
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_Log_Tools::initializeLogging(), aborting\n")));

    // *PORTABILITY*: on Windows, finalize ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    result = ACE::fini ();
    if (result == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE::fini(): \"%m\", continuing\n")));
#endif
    return EXIT_FAILURE;
  } // end IF

//  // step1e: pre-initialize signal handling
//  ACE_Sig_Set signal_set (0);
//  ACE_Sig_Set ignored_signal_set (0);
//  do_initializeSignals (true, // allow SIGUSR1/SIGBREAK
//                        signal_set,
//                        ignored_signal_set);
//  Common_SignalActions_t previous_signal_actions;
//  sigset_t previous_signal_mask;
//  result = ACE_OS::sigemptyset (&previous_signal_mask);
//  if (result == -1)
//  {
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("failed to ACE_OS::sigemptyset(): \"%m\", aborting\n")));
//
//    Common_Log_Tools::finalizeLogging ();
//    // *PORTABILITY*: on Windows, finalize ACE...
//#if defined (ACE_WIN32) || defined (ACE_WIN64)
//    result = ACE::fini ();
//    if (result == -1)
//      ACE_DEBUG ((LM_ERROR,
//                  ACE_TEXT ("failed to ACE::fini(): \"%m\", continuing\n")));
//#endif
//    return EXIT_FAILURE;
//  } // end IF
//  if (!Common_Signal_Tools::preInitialize (signal_set,
//                                           use_reactor,
//                                           previous_signal_actions,
//                                           previous_signal_mask))
//  {
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("failed to Common_Signal_Tools::preInitialize(), aborting\n")));
//
//    Common_Log_Tools::finalizeLogging ();
//    // *PORTABILITY*: on Windows, finalize ACE...
//#if defined (ACE_WIN32) || defined (ACE_WIN64)
//    result = ACE::fini ();
//    if (result == -1)
//      ACE_DEBUG ((LM_ERROR,
//                  ACE_TEXT ("failed to ACE::fini(): \"%m\", continuing\n")));
//#endif
//    return EXIT_FAILURE;
//  } // end IF
//  ACE_SYNCH_RECURSIVE_MUTEX signal_lock;
//  Stream_Source_SignalHandler signal_handler ((use_reactor ? COMMON_SIGNAL_DISPATCH_REACTOR
//                                                           : COMMON_SIGNAL_DISPATCH_PROACTOR),
//                                              &signal_lock);

  // step1f: handle specific program modes
  if (print_version_and_exit)
  {
    do_printVersion (ACE::basename (argv_in[0]));

    //Common_Signal_Tools::finalize ((use_reactor ? COMMON_SIGNAL_DISPATCH_REACTOR
    //                                            : COMMON_SIGNAL_DISPATCH_PROACTOR),
    //                               signal_set,
    //                               previous_signal_actions,
    //                               previous_signal_mask);
    Common_Log_Tools::finalizeLogging ();
    // *PORTABILITY*: on Windows, finalize ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    result = ACE::fini ();
    if (result == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE::fini(): \"%m\", continuing\n")));
#endif
    return EXIT_SUCCESS;
  } // end IF

  // step1g: set process resource limits
  // *NOTE*: settings will be inherited by any child processes
//  if (!Common_Tools::setResourceLimits (false, // file descriptors
//                                        true,  // stack traces
//                                        true)) // pending signals
//  {
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("failed to Common_Tools::setResourceLimits(), aborting\n")));
//
//    Common_Signal_Tools::finalize ((use_reactor ? COMMON_SIGNAL_DISPATCH_REACTOR
//                                                : COMMON_SIGNAL_DISPATCH_PROACTOR),
//                                   signal_set,
//                                   previous_signal_actions,
//                                   previous_signal_mask);
//    Common_Log_Tools::finalizeLogging ();
//    // *PORTABILITY*: on Windows, finalize ACE...
//#if defined (ACE_WIN32) || defined (ACE_WIN64)
//    result = ACE::fini ();
//    if (result == -1)
//      ACE_DEBUG ((LM_ERROR,
//                  ACE_TEXT ("failed to ACE::fini(): \"%m\", continuing\n")));
//#endif
//    return EXIT_FAILURE;
//  } // end IF

  ACE_High_Res_Timer timer;
  timer.start ();
  // step2: do actual work
  do_work (buffer_size,
           input_file,
           output_file);
           //statistic_reporting_interval,
           //signal_set,
           //ignored_signal_set,
           //previous_signal_actions,
           //signal_handler);
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

    //Common_Signal_Tools::finalize ((use_reactor ? COMMON_SIGNAL_DISPATCH_REACTOR
    //                                            : COMMON_SIGNAL_DISPATCH_PROACTOR),
    //                               signal_set,
    //                               previous_signal_actions,
    //                               previous_signal_mask);
    Common_Log_Tools::finalizeLogging ();
    // *PORTABILITY*: on Windows, finalize ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    result = ACE::fini ();
    if (result == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE::fini(): \"%m\", continuing\n")));
#endif
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
#endif

  //Common_Signal_Tools::finalize ((use_reactor ? COMMON_SIGNAL_DISPATCH_REACTOR
  //                                            : COMMON_SIGNAL_DISPATCH_PROACTOR),
  //                               signal_set,
  //                               previous_signal_actions,
  //                               previous_signal_mask);
  Common_Log_Tools::finalizeLogging ();
  // *PORTABILITY*: on Windows, finalize ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  result = ACE::fini ();
  if (result == -1)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE::fini(): \"%m\", continuing\n")));
#endif

  return EXIT_SUCCESS;
} // end main
