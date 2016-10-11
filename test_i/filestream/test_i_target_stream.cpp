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

#include "test_i_target_stream.h"

#include "ace/Log_Msg.h"

#include "stream_macros.h"

#include "test_i_source_stream.h"

Test_I_Target_Stream::Test_I_Target_Stream (const std::string& name_in)
 : inherited (name_in)
 , netIO_ (ACE_TEXT_ALWAYS_CHAR ("NetIO"),
           NULL,
           false)
 , statisticReport_ (ACE_TEXT_ALWAYS_CHAR ("StatisticReport"),
                     NULL,
                     false)
 , fileWriter_ (ACE_TEXT_ALWAYS_CHAR ("FileWriter"),
                NULL,
                false)
{
  STREAM_TRACE (ACE_TEXT ("Test_I_Target_Stream::Test_I_Target_Stream"));

}

Test_I_Target_Stream::~Test_I_Target_Stream ()
{
  STREAM_TRACE (ACE_TEXT ("Test_I_Target_Stream::~Test_I_Target_Stream"));

  inherited::shutdown ();
}

void
Test_I_Target_Stream::ping ()
{
  STREAM_TRACE (ACE_TEXT ("Test_I_Target_Stream::ping"));

  ACE_ASSERT (false);
  ACE_NOTSUP;
  ACE_NOTREACHED (return;)
}

bool
Test_I_Target_Stream::load (Stream_ModuleList_t& modules_out,
                            bool& delete_out)
{
  STREAM_TRACE (ACE_TEXT ("Test_I_Target_Stream::load"));

  // initialize return value(s)
//  modules_out.clear ();
  delete_out = false;

  modules_out.push_back (&fileWriter_);
  modules_out.push_back (&statisticReport_);
  modules_out.push_back (&netIO_);

  return true;
}

bool
Test_I_Target_Stream::initialize (const Test_I_StreamConfiguration& configuration_in,
                                  bool setupPipeline_in,
                                  bool resetSessionData_in)
{
  STREAM_TRACE (ACE_TEXT ("Test_I_Target_Stream::initialize"));

  // sanity check(s)
  ACE_ASSERT (!isRunning ());

  // allocate a new session state, reset stream
  if (!inherited::initialize (configuration_in,
                              false,
                              resetSessionData_in))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: failed to Stream_Base_T::initialize(), aborting\n"),
                ACE_TEXT (inherited::name ().c_str ())));
    return false;
  } // end IF
  if (!inherited::sessionData_)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to allocate session data, aborting\n")));
    return false;
  } // end IF
  ACE_ASSERT (configuration_in.moduleHandlerConfiguration);
  // *TODO*: remove type inferences
  Test_I_Target_SessionData& session_data_r =
      const_cast<Test_I_Target_SessionData&> (inherited::sessionData_->get ());
//  session_data_r.fileName =
//    configuration_in.moduleHandlerConfiguration->fileName;
  session_data_r.sessionID = configuration_in.sessionID;
  session_data_r.targetFileName =
    configuration_in.moduleHandlerConfiguration->targetFileName;

  // things to be done here:
  // [- initialize base class]
  // ------------------------------------
  // - initialize modules
  // ------------------------------------
  // - push them onto the stream (tail-first) !
  // ------------------------------------

//  configuration_in.moduleConfiguration.streamState = &state_;

  // ---------------------------------------------------------------------------
  ACE_ASSERT (configuration_in.moduleConfiguration);
  //ACE_ASSERT (configuration_in.moduleHandlerConfiguration);

  // ---------------------------------------------------------------------------

  // ******************* Net IO ***********************
  //netIO_.initialize (*configuration_in.moduleConfiguration);
  Test_I_Target_Module_Net_Writer_t* netIO_impl_p =
    dynamic_cast<Test_I_Target_Module_Net_Writer_t*> (netIO_.writer ());
  if (!netIO_impl_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("dynamic_cast<Stream_Module_Net_Writer_T> failed, aborting\n")));
    return false;
  } // end IF
  //if (!netIO_impl_p->initialize (*configuration_in.moduleHandlerConfiguration))
  //{
  //  ACE_DEBUG ((LM_ERROR,
  //              ACE_TEXT ("failed to initialize module: \"%s\", aborting\n"),
  //              netIO_.name ()));
  //  return false;
  //} // end IF
  if (!netIO_impl_p->initialize (inherited::state_))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to initialize module: \"%s\", aborting\n"),
                netIO_.name ()));
    return false;
  } // end IF
//  netIO_impl_p->reset ();
  // *NOTE*: push()ing the module will open() it
  //         --> set the argument that is passed along (head module expects a
  //             handle to the session data)
  netIO_.arg (inherited::sessionData_);

  if (setupPipeline_in)
    if (!inherited::setup ())
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to setup pipeline, aborting\n")));
      return false;
    } // end IF

  // -------------------------------------------------------------

  // OK: all went well
  inherited::isInitialized_ = true;
  //inherited::dump_state ();

  return true;
}

bool
Test_I_Target_Stream::collect (Test_I_RuntimeStatistic_t& data_out)
{
  STREAM_TRACE (ACE_TEXT ("Test_I_Target_Stream::collect"));

  // sanity check(s)
  ACE_ASSERT (inherited::sessionData_);

  int result = -1;
  bool release_lock = false;

  Test_I_Target_SessionData& session_data_r =
        const_cast<Test_I_Target_SessionData&> (inherited::sessionData_->get ());

  Test_I_Target_Module_Statistic_WriterTask_t* statistic_impl =
    dynamic_cast<Test_I_Target_Module_Statistic_WriterTask_t*> (statisticReport_.writer ());
  if (!statistic_impl)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("dynamic_cast<Test_I_Target_Module_Statistic_WriterTask_t> failed, aborting\n")));
    return false;
  } // end IF

  // synch access
  if (session_data_r.lock)
  {
    result = session_data_r.lock->acquire ();
    if (result == -1)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_SYNCH_MUTEX::acquire(): \"%m\", aborting\n")));
      return false;
    } // end IF
    release_lock = true;
  } // end IF

  session_data_r.currentStatistic.timeStamp = COMMON_TIME_NOW;

  // delegate to the statistic module
  bool result_2 = false;
  try {
    result_2 = statistic_impl->collect (data_out);
  } catch (...) {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("caught exception in Common_IStatistic_T::collect(), continuing\n")));
  }
  if (!result)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_IStatistic_T::collect(), aborting\n")));
  else
    session_data_r.currentStatistic = data_out;

  if (release_lock)
  {
    ACE_ASSERT (session_data_r.lock);
    result = session_data_r.lock->release ();
    if (result == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_SYNCH_MUTEX::release(): \"%m\", continuing\n")));
  } // end IF

  return result_2;
}

void
Test_I_Target_Stream::report () const
{
  STREAM_TRACE (ACE_TEXT ("Test_I_Target_Stream::report"));

//   Net_Module_Statistic_ReaderTask_t* runtimeStatistic_impl = NULL;
//   runtimeStatistic_impl = dynamic_cast<Net_Module_Statistic_ReaderTask_t*> (//runtimeStatistic_.writer ());
//   if (!runtimeStatistic_impl)
//   {
//     ACE_DEBUG ((LM_ERROR,
//                 ACE_TEXT ("dynamic_cast<Net_Module_Statistic_ReaderTask_t> failed, returning\n")));
//
//     return;
//   } // end IF
//
//   // delegate to this module...
//   return (runtimeStatistic_impl->report ());

  ACE_ASSERT (false);
  ACE_NOTSUP;

  ACE_NOTREACHED (return;)
}
