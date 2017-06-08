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

#include <ace/Synch.h>
#include "test_u_riffdecoder_stream.h"

#include "ace/Log_Msg.h"

#include "stream_macros.h"

// initialize statics
ACE_Atomic_Op<ACE_Thread_Mutex,
              unsigned long> Test_U_RIFFDecoder_Stream::currentSessionID = 0;

Test_U_RIFFDecoder_Stream::Test_U_RIFFDecoder_Stream ()
 : inherited (ACE_TEXT_ALWAYS_CHAR ("RIFFDecoderStream"))
 , source_ (this,
            ACE_TEXT_ALWAYS_CHAR ("FileSource"),
            NULL,
            false)
 , decoder_ (this,
             ACE_TEXT_ALWAYS_CHAR ("Decoder"),
             NULL,
             false)
 , statistic_ (this,
               ACE_TEXT_ALWAYS_CHAR ("StatisticReport"),
               NULL,
               false)
{
  STREAM_TRACE (ACE_TEXT ("Test_U_RIFFDecoder_Stream::Test_U_RIFFDecoder_Stream"));

}

Test_U_RIFFDecoder_Stream::~Test_U_RIFFDecoder_Stream ()
{
  STREAM_TRACE (ACE_TEXT ("Test_U_RIFFDecoder_Stream::~Test_U_RIFFDecoder_Stream"));

  // *NOTE*: this implements an ordered shutdown on destruction
  inherited::shutdown ();
}

bool
Test_U_RIFFDecoder_Stream::load (Stream_ModuleList_t& modules_out,
                                 bool& delete_out)
{
  STREAM_TRACE (ACE_TEXT ("Test_U_RIFFDecoder_Stream::load"));

  // initialize return value(s)
  modules_out.clear ();
  delete_out = false;

  modules_out.push_back (&statistic_);
  modules_out.push_back (&decoder_);
  modules_out.push_back (&source_);

  return true;
}

bool
Test_U_RIFFDecoder_Stream::initialize (const struct Test_U_RIFFDecoder_StreamConfiguration& configuration_in)
{
  STREAM_TRACE (ACE_TEXT ("Test_U_RIFFDecoder_Stream::initialize"));

  // sanity check(s)
  ACE_ASSERT (!isRunning ());

//  bool result = false;
  bool setup_pipeline = configuration_in.setupPipeline;
  bool reset_setup_pipeline = false;
  struct Test_U_RIFFDecoder_SessionData* session_data_p = NULL;
  Test_U_RIFFDecoder_Module_Source* source_impl_p = NULL;

  // allocate a new session state, reset stream
  const_cast<struct Test_U_RIFFDecoder_StreamConfiguration&> (configuration_in).setupPipeline =
    false;
  reset_setup_pipeline = true;
  if (!inherited::initialize (configuration_in))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: failed to Stream_Base_T::initialize(), aborting\n"),
                ACE_TEXT (inherited::name_.c_str ())));
    goto error;
  } // end IF
  const_cast<struct Test_U_RIFFDecoder_StreamConfiguration&> (configuration_in).setupPipeline =
    setup_pipeline;
  reset_setup_pipeline = false;
  ACE_ASSERT (inherited::sessionData_);
  session_data_p =
    &const_cast<struct Test_U_RIFFDecoder_SessionData&> (inherited::sessionData_->get ());
  // *TODO*: remove type inferences
  session_data_p->sessionID = ++Test_U_RIFFDecoder_Stream::currentSessionID;

  // things to be done here:
  // [- initialize base class]
  // ------------------------------------
  // - initialize notification strategy (if any)
  // ------------------------------------
  // - push the final module onto the stream (if any)
  // ------------------------------------
  // - initialize modules
  // - push them onto the stream (tail-first) !
  // ------------------------------------

//  int result = -1;
//  inherited::MODULE_T* module_p = NULL;
//  if (configuration_in.notificationStrategy)
//  {
//    module_p = inherited::head ();
//    if (!module_p)
//    {
//      ACE_DEBUG ((LM_ERROR,
//                  ACE_TEXT ("no head module found, aborting\n")));
//      return false;
//    } // end IF
//    inherited::TASK_T* task_p = module_p->reader ();
//    if (!task_p)
//    {
//      ACE_DEBUG ((LM_ERROR,
//                  ACE_TEXT ("no head module reader task found, aborting\n")));
//      return false;
//    } // end IF
//    inherited::QUEUE_T* queue_p = task_p->msg_queue ();
//    if (!queue_p)
//    {
//      ACE_DEBUG ((LM_ERROR,
//                  ACE_TEXT ("no head module reader task queue found, aborting\n")));
//      return false;
//    } // end IF
//    queue_p->notification_strategy (configuration_in.notificationStrategy);
//  } // end IF
//  configuration_in.moduleConfiguration.streamState = &state_;

  // ---------------------------------------------------------------------------

  // ******************* Runtime Statistic ************************
  //runtimeStatistic_.initialize (*configuration_in.moduleConfiguration);
  //Test_U_RIFFDecoder_Module_Statistic_WriterTask_t* runtimeStatistic_impl_p =
  //    dynamic_cast<Test_U_RIFFDecoder_Module_Statistic_WriterTask_t*> (runtimeStatistic_.writer ());
  //if (!runtimeStatistic_impl_p)
  //{
  //  ACE_DEBUG ((LM_ERROR,
  //              ACE_TEXT ("dynamic_cast<Test_U_RIFFDecoder_Module_RuntimeStatistic> failed, aborting\n")));
  //  return false;
  //} // end IF
  //if (!runtimeStatistic_impl_p->initialize (configuration_in.statisticReportingInterval, // reporting interval (seconds)
  //                                          true,                                        // push 1-second interval statistic messages downstream ?
  //                                          configuration_in.printFinalReport,           // print final report ?
  //                                          configuration_in.messageAllocator))          // message allocator handle
  //{
  //  ACE_DEBUG ((LM_ERROR,
  //              ACE_TEXT ("failed to initialize module: \"%s\", aborting\n"),
  //              runtimeStatistic_.name ()));
  //  return false;
  //} // end IF

  // ******************* Decoder ************************
  //decoder_.initialize (*configuration_in.moduleConfiguration);
  //Test_U_RIFFDecoder_Module_Decoder* decoder_impl_p =
  //  dynamic_cast<Test_U_RIFFDecoder_Module_Decoder*> (decoder_.writer ());
  //if (!decoder_impl_p)
  //{
  //  ACE_DEBUG ((LM_ERROR,
  //              ACE_TEXT ("dynamic_cast<Test_U_RIFFDecoder_Module_Decoder> failed, aborting\n")));
  //  return false;
  //} // end IF
  //if (!decoder_impl_p->initialize (*configuration_in.moduleHandlerConfiguration))
  //{
  //  ACE_DEBUG ((LM_ERROR,
  //              ACE_TEXT ("failed to initialize module: \"%s\", aborting\n"),
  //              decoder_.name ()));
  //  return false;
  //} // end IF

  // ******************* File Source ************************
  source_impl_p =
    dynamic_cast<Test_U_RIFFDecoder_Module_Source*> (source_.writer ());
  if (!source_impl_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: dynamic_cast<Test_U_RIFFDecoder_Module_CamSource> failed, aborting\n"),
                ACE_TEXT (inherited::name_.c_str ())));
    goto error;
  } // end IF
//  // *TODO*: remove type inference
//  if (!source_impl_p->initialize (*configuration_in.moduleHandlerConfiguration,
//                                  configuration_in.messageAllocator))
//  {
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("failed to initialize module: \"%s\", aborting\n"),
//                source_.name ()));
//    return false;
//  } // end IF
  source_impl_p->set (&(inherited::state_));

  // *NOTE*: push()ing the module will open() it
  //         --> set the argument that is passed along (head module expects a
  //             handle to the session data)
  source_.arg (inherited::sessionData_);

  if (configuration_in.setupPipeline)
    if (!inherited::setup ())
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("%s: failed to set up pipeline, aborting\n"),
                  ACE_TEXT (inherited::name_.c_str ())));
      goto error;
    } // end IF

  // -------------------------------------------------------------

  inherited::isInitialized_ = true;
  //inherited::dump_state ();

  return true;

error:
  if (reset_setup_pipeline)
    const_cast<struct Test_U_RIFFDecoder_StreamConfiguration&> (configuration_in).setupPipeline =
      setup_pipeline;

  return false;
}

bool
Test_U_RIFFDecoder_Stream::collect (Stream_Statistic& data_out)
{
  STREAM_TRACE (ACE_TEXT ("Test_U_RIFFDecoder_Stream::collect"));

  // sanity check(s)
  ACE_ASSERT (inherited::sessionData_);

  int result = -1;

  Test_U_RIFFDecoder_Module_Statistic_WriterTask_t* statistic_impl =
    dynamic_cast<Test_U_RIFFDecoder_Module_Statistic_WriterTask_t*> (statistic_.writer ());
  if (!statistic_impl)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: dynamic_cast<Test_U_RIFFDecoder_Module_Statistic_WriterTask_t> failed, aborting\n"),
                ACE_TEXT (inherited::name_.c_str ())));
    return false;
  } // end IF

  // synch access
  struct Test_U_RIFFDecoder_SessionData& session_data_r =
    const_cast<struct Test_U_RIFFDecoder_SessionData&> (inherited::sessionData_->get ());
  if (session_data_r.lock)
  {
    result = session_data_r.lock->acquire ();
    if (result == -1)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("%s: failed to ACE_SYNCH_MUTEX::acquire(): \"%m\", aborting\n"),
                  ACE_TEXT (inherited::name_.c_str ())));
      return false;
    } // end IF
  } // end IF

  session_data_r.currentStatistic.timeStamp = COMMON_TIME_NOW;

  // delegate to the statistic module
  bool result_2 = false;
  try {
    result_2 = statistic_impl->collect (data_out);
  } catch (...) {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: caught exception in Common_IStatistic_T::collect(), continuing\n"),
                ACE_TEXT (inherited::name_.c_str ())));
  }
  if (!result)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: failed to Common_IStatistic_T::collect(), aborting\n"),
                ACE_TEXT (inherited::name_.c_str ())));
  else
    session_data_r.currentStatistic = data_out;

  if (session_data_r.lock)
  {
    result = session_data_r.lock->release ();
    if (result == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("%s: failed to ACE_SYNCH_MUTEX::release(): \"%m\", continuing\n"),
                  ACE_TEXT (inherited::name_.c_str ())));
  } // end IF

  return result_2;
}

void
Test_U_RIFFDecoder_Stream::report () const
{
  STREAM_TRACE (ACE_TEXT ("Test_U_RIFFDecoder_Stream::report"));

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
//   // delegate to this module
//   return (runtimeStatistic_impl->report ());

  ACE_ASSERT (false);
  ACE_NOTSUP;

  ACE_NOTREACHED (return;)
}
