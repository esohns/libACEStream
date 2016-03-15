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

#include "ace/Synch.h"
#include "test_i_target_signalhandler.h"

#include "ace/Log_Msg.h"

#include "common_timer_manager.h"
#include "common_tools.h"

#include "stream_macros.h"

#include "test_i_connection_manager_common.h"

Stream_Target_SignalHandler::Stream_Target_SignalHandler (bool useReactor_in)
 : inherited (this,          // event handler handle
              useReactor_in) // use reactor ?
 , configuration_ ()
{
  STREAM_TRACE (ACE_TEXT ("Stream_Target_SignalHandler::Stream_Target_SignalHandler"));

}

Stream_Target_SignalHandler::~Stream_Target_SignalHandler ()
{
  STREAM_TRACE (ACE_TEXT ("Stream_Target_SignalHandler::~Stream_Target_SignalHandler"));

}

bool
Stream_Target_SignalHandler::initialize (const Test_I_Target_SignalHandlerConfiguration& configuration_in)
{
  STREAM_TRACE (ACE_TEXT ("Stream_Target_SignalHandler::initialize"));

  configuration_ = configuration_in;

  return true;
}

bool
Stream_Target_SignalHandler::handleSignal (int signal_in)
{
  STREAM_TRACE (ACE_TEXT ("Stream_Target_SignalHandler::handleSignal"));

  int result = -1;
  bool close_all = false;
  bool shutdown = false;
  bool statistic = false;
  switch (signal_in)
  {
// *PORTABILITY*: on Windows SIGQUIT is not defined
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    case SIGINT:
#else
    case SIGINT:
      break;
    case SIGQUIT:
#endif
    {
//       // *PORTABILITY*: tracing in a signal handler context is not portable
//       // *TODO*
      //ACE_DEBUG((LM_DEBUG,
      //           ACE_TEXT("shutting down...\n")));

      shutdown = true;
      break;
    }
// *PORTABILITY*: on Windows SIGUSRx are not defined
// --> use SIGBREAK (21) and SIGTERM (15) instead...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    case SIGBREAK:
#else
    case SIGUSR1:
#endif
    {
      // print statistic
      statistic = true;
      break;
    }
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
    case SIGHUP:
    case SIGUSR2:
#endif
    case SIGTERM:
    {
      // close
      close_all = true;
      break;
    }
    default:
    {
      // *PORTABILITY*: tracing in a signal handler context is not portable
      // *TODO*
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("received invalid/unknown signal: \"%S\", aborting\n"),
                  signal_in));
      return false;
    }
  } // end SWITCH

  // ------------------------------------

  // print statistic ?
  if (statistic && configuration_.statisticReportingHandler)
  {
    try
    {
      configuration_.statisticReportingHandler->report ();
    }
    catch (...)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("caught exception in Common_IStatistic::report(), aborting\n")));
      return false;
    }
  } // end IF

  Test_I_Target_IInetConnectionManager_t* connection_manager_p =
    TEST_I_TARGET_CONNECTIONMANAGER_SINGLETON::instance ();
  ACE_ASSERT (connection_manager_p);
  if (close_all)
    connection_manager_p->abort (false);

//check_shutdown:
  // ...shutdown ?
  if (shutdown)
  {
    // stop everything, i.e.
    // - leave event loop(s) handling signals, sockets, (maintenance) timers,
    //   exception handlers, ...
    // - activation timers (connection attempts, ...)
    // [- UI dispatch]

    // step1: stop GTK event processing
    // *NOTE*: triggering UI shutdown from a widget callback is more consistent,
    //         compared to doing it here
//    COMMON_UI_GTK_MANAGER_SINGLETON::instance ()->stop (false, true);

    // step2: invoke controller (if any)
    if (configuration_.listener)
    {
      try
      {
        configuration_.listener->stop ();
      }
      catch (...)
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("caught exception in Common_IControl::stop(), aborting\n")));
        return false;
      }
    } // end IF

    // step3: stop timer
    if (configuration_.statisticReportingTimerID >= 0)
    {
      const void* act_p = NULL;
      result =
          COMMON_TIMERMANAGER_SINGLETON::instance ()->cancel (configuration_.statisticReportingTimerID,
                                                              &act_p);
      if (result <= 0)
      {
        ACE_DEBUG ((LM_DEBUG,
                    ACE_TEXT ("failed to cancel timer (ID: %d): \"%m\", aborting\n"),
                    configuration_.statisticReportingTimerID));

        // clean up
        configuration_.statisticReportingTimerID = -1;

        return false;
      } // end IF
      configuration_.statisticReportingTimerID = -1;
    } // end IF

    // step4: stop/abort(/wait) for connections
    connection_manager_p->stop ();
    connection_manager_p->abort ();

    // step5: stop reactor (&& proactor, if applicable)
    Common_Tools::finalizeEventDispatch (inherited::useReactor_,  // stop reactor ?
                                         !inherited::useReactor_, // stop proactor ?
                                         -1);                     // group ID (--> don't block)

    // *IMPORTANT NOTE*: there is no real reason to wait here
  } // end IF

  return true;
}
