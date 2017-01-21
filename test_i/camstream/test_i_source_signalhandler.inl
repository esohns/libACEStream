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

#include <ace/Log_Msg.h>

#include "stream_macros.h"

#include "test_i_source_common.h"

template <typename ConfigurationType>
Test_I_Source_SignalHandler_T<ConfigurationType>::Test_I_Source_SignalHandler_T ()
 : inherited (this) // event handler handle
{
  STREAM_TRACE (ACE_TEXT ("Test_I_Source_SignalHandler_T::Test_I_Source_SignalHandler_T"));

}

template <typename ConfigurationType>
Test_I_Source_SignalHandler_T<ConfigurationType>::~Test_I_Source_SignalHandler_T ()
{
  STREAM_TRACE (ACE_TEXT ("Test_I_Source_SignalHandler_T::~Test_I_Source_SignalHandler_T"));

}

template <typename ConfigurationType>
void
Test_I_Source_SignalHandler_T<ConfigurationType>::handle (int signal_in)
{
  STREAM_TRACE (ACE_TEXT ("Test_I_Source_SignalHandler_T::handle"));

//  int result = -1;

  // sanity check(s)
  ACE_ASSERT (inherited::configuration_);

  bool statistic = false;
  bool shutdown = false;
  switch (signal_in)
  {
    case SIGINT:
// *PORTABILITY*: on Windows SIGQUIT is not defined
#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
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
#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
    case SIGUSR1:
#else
    case SIGBREAK:
#endif
    {
      // print statistic
      statistic = true;

      break;
    }
#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
    case SIGHUP:
    case SIGUSR2:
#endif
    case SIGTERM:
    {
      // print statistic
      statistic = true;

      break;
    }
    default:
    {
      // *PORTABILITY*: tracing in a signal handler context is not portable
      // *TODO*
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("received invalid/unknown signal: \"%S\", returning\n"),
                  signal_in));
      return;
    }
  } // end SWITCH

  // ------------------------------------

  // print statistic ?
  if (statistic)
  {
    try {
      //handle = configuration_.connector->connect (configuration_.peerAddress);
    } catch (...) {
      //// *PORTABILITY*: tracing in a signal handler context is not portable
      //// *TODO*
      //ACE_DEBUG ((LM_ERROR,
      //            ACE_TEXT ("caught exception in Net_Client_IConnector::connect(): \"%m\", continuing\n")));
    }
  } // end IF

//check_shutdown:
  // ...shutdown ?
  if (shutdown)
  {
    // stop everything, i.e.
    // - stop processing stream
    // - leave event loop(s) handling signals, sockets, (maintenance) timers,
    //   exception handlers, ...
    // - activation timers (connection attempts, ...)
    // [- UI dispatch]
    if (inherited::configuration_->hasUI)
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    {
      if (inherited::configuration_->useMediaFoundation)
        TEST_I_SOURCE_MEDIAFOUNDATION_GTK_MANAGER_SINGLETON::instance ()->stop (false, // wait for completion ?
                                                                                true); // N/A
      else
        TEST_I_SOURCE_DIRECTSHOW_GTK_MANAGER_SINGLETON::instance ()->stop (false, // wait for completion ?
                                                                           true); // N/A
    } // end IF
#else
      TEST_I_SOURCE_GTK_MANAGER_SINGLETON::instance ()->stop (false, // wait for completion ?
                                                              true); // N/A
#endif

    // step1: stop processing stream
    ACE_ASSERT (inherited::configuration_->stream);
    inherited::configuration_->stream->stop (false, // don't block
                                             true); // locked access

    // step2: stop/abort(/wait) for connections
    //ConnectionManagerType* connection_manager_p =
    //    TEST_I_SOURCE_CONNECTIONMANAGER_SINGLETON::instance ();
    //ACE_ASSERT (connection_manager_p);
    //connection_manager_p->stop ();
    ACE_ASSERT (inherited::configuration_->connectionManager);
    inherited::configuration_->connectionManager->stop ();
//    connection_manager_p->abort ();

//    // step3: stop reactor (&& proactor, if applicable)
//    Common_Tools::finalizeEventDispatch (inherited::configuration_->useReactor,  // stop reactor ?
//                                         !inherited::configuration_->useReactor, // stop proactor ?
//                                         -1);                                    // group ID (--> don't block)
  } // end IF
}
