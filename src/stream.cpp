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

#include "stream.h"

#include "stream_common.h"
#include "stream_headmoduletask.h"
#include "stream_iallocator.h"
#include "stream_module_base.h"
#include "stream_session_message.h"

Stream::Stream ()
// *TODO*: use default ctor and rely on init/fini() ?
 : inherited (NULL, // argument to module open()
              NULL, // no head module --> ACE_Stream_Head !
              NULL) // no tail module --> ACE_Stream_Tail !
 , allocator_ (NULL)
 , availableModules_ ()
 , isInitialized_ (false)
 , state_ ()
{
  STREAM_TRACE (ACE_TEXT ("Stream::Stream"));

//  ACE_OS::memset (&state_, 0, sizeof (state_));
}

Stream::~Stream ()
{
  STREAM_TRACE (ACE_TEXT ("Stream::~Stream"));

}

bool
Stream::reset ()
{
  STREAM_TRACE (ACE_TEXT ("Stream::reset"));

  // sanity check: is running ?
  if (isRunning ())
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("cannot reset (currently running), aborting\n")));

    return false;
  } // end IF

  // pop/close all modules
  // *NOTE*: will implicitly (blocking !) wait for any active worker threads
  if (!finalize ())
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to finalize(), aborting\n")));

    return false;
  } // end IF

  // - reset reader/writers tasks for ALL modules
  // - reinit head/tail modules
  return initialize ();
}

bool
Stream::initialize ()
{
  STREAM_TRACE (ACE_TEXT ("Stream::initialize"));

  if (isInitialized_)
  {
    // *NOTE*: fini() invokes close() which will reset the writer/reader tasks
    // of the enqueued modules --> reset this !
    IMODULE_T* imodule_handle = NULL;
    // *NOTE*: cannot write this - it confuses gcc...
    for (MODULE_CONTAINER_ITERATOR_T iterator = availableModules_.begin ();
         iterator != availableModules_.end ();
         iterator++)
    {
      // need a downcast...
      imodule_handle = dynamic_cast<IMODULE_T*> (*iterator);
      if (!imodule_handle)
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("%s: dynamic_cast<Stream_IModule> failed, aborting\n"),
                    ACE_TEXT ((*iterator)->name ())));

        return false;
      } // end IF
      try
      {
        imodule_handle->reset ();
      }
      catch (...)
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("caught exception in Stream_IModule::reset(), continuing\n")));
      }
    } // end FOR
  } // end IF

  // delegate this to base class open()
  int result = -1;
  try
  {
    result = inherited::open (NULL,  // argument to module open()
                              NULL,  // no head module --> ACE_Stream_Head !
                              NULL); // no tail module --> ACE_Stream_Tail !
  }
  catch (...)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("caught exception in ACE_Stream::open(), aborting\n")));
  }
  if (result == -1)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Stream::open(): \"%m\", aborting\n")));

  return (result == 0);
}

bool
Stream::finalize ()
{
  STREAM_TRACE (ACE_TEXT ("Stream::finalize"));

  // OK: delegate this to base class close(ACE_Module_Base::M_DELETE_NONE)
  int result = -1;
  try
  {
    // *NOTE*: this will implicitly:
    // - unwind the stream, which pop()s all (pushed) modules
    // --> pop()ing a module will close() it
    // --> close()ing a module will module_closed() and flush() the associated
    //     tasks
    // --> flush()ing a task will close() its queue
    // --> close()ing a queue will deactivate() and flush() it
    result = inherited::close (ACE_Module_Base::M_DELETE_NONE);
  }
  catch (...)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("caught exception in ACE_Stream::close(M_DELETE_NONE), aborting\n")));
  }

  // OK ?
  if (result == -1)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Stream::close(M_DELETE_NONE): \"%m\", aborting\n")));

  return (result == 0);
}

void
Stream::start ()
{
  STREAM_TRACE (ACE_TEXT ("Stream::start"));

  int result = -1;

  // sanity check: is initialized ?
  if (!isInitialized ())
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("not initialized, returning\n")));
    return;
  } // end IF

  // delegate to the head module
  MODULE_T* module_p = NULL;
  result = inherited::top (module_p);
  if ((result == -1) || !module_p)
  {
    //ACE_DEBUG ((LM_ERROR,
    //            ACE_TEXT ("no head module found: \"%m\", returning\n")));
    return;
  } // end IF

  ISTREAM_CONTROL_T* control_impl_p = NULL;
  control_impl_p = dynamic_cast<ISTREAM_CONTROL_T*> (module_p->writer ());
  if (!control_impl_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: dynamic_cast<Stream_IStreamControl> failed, returning\n"),
                ACE_TEXT (module_p->name ())));
    return;
  } // end IF

  try
  {
    control_impl_p->start ();
  }
  catch (...)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("caught exception in Stream_IStreamControl::start (module: \"%s\"), returning\n"),
                ACE_TEXT (module_p->name ())));
    return;
  }
}

void
Stream::stop ()
{
  STREAM_TRACE (ACE_TEXT ("Stream::stop"));

  int result = -1;

  // sanity check: is running ?
  if (!isRunning ())
  {
//     ACE_DEBUG ((LM_DEBUG,
//                 ACE_TEXT ("not running --> nothing to do, returning\n")));
    return;
  } // end IF

  // delegate to the head module
  MODULE_T* module_p = NULL;
  result = inherited::top (module_p);
  if ((result == -1) || !module_p)
  {
    //ACE_DEBUG ((LM_ERROR,
    //            ACE_TEXT ("no head module found: \"%m\", returning\n")));
    return;
  } // end IF

  ISTREAM_CONTROL_T* control_impl_p = NULL;
  control_impl_p = dynamic_cast<ISTREAM_CONTROL_T*> (module_p->writer ());
  if (!control_impl_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: dynamic_cast<Stream_IStreamControl) failed> (returning\n"),
                ACE_TEXT (module_p->name ())));
    return;
  } // end IF

  try
  {
    control_impl_p->stop ();
  }
  catch (...)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("caught exception in Stream_IStreamControl::stop (module: \"%s\"), returning\n"),
                ACE_TEXT (module_p->name ())));
    return;
  }
}

bool
Stream::isRunning () const
{
  STREAM_TRACE (ACE_TEXT ("Stream::isRunning"));

  int result = -1;

  // delegate to the head module
  MODULE_T* module_p = NULL;
  result = const_cast<Stream*> (this)->top (module_p);
  if ((result == -1) || !module_p)
  {
    //ACE_DEBUG ((LM_ERROR,
    //            ACE_TEXT ("no head module found: \"%m\", aborting\n")));
    return false;
  } // end IF

  ISTREAM_CONTROL_T* control_impl_p = NULL;
  control_impl_p = dynamic_cast<ISTREAM_CONTROL_T*> (module_p->writer ());
  if (!control_impl_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: dynamic_cast<Stream_IStreamControl> failed, returning\n"),
                ACE_TEXT (module_p->name ())));
    return false;
  } // end IF

  try
  {
    return control_impl_p->isRunning ();
  }
  catch (...)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("caught exception in Stream_IStreamControl::isRunning (module: \"%s\"), aborting\n"),
                ACE_TEXT (module_p->name ())));
  }

  return false;
}

void
Stream::pause ()
{
  STREAM_TRACE (ACE_TEXT ("Stream::pause"));

  int result = -1;

  // sanity check: is running ?
  if (!isRunning ())
  {
//     ACE_DEBUG ((LM_DEBUG,
//                 ACE_TEXT ("not running --> nothing to do, returning\n")));
    return;
  } // end IF

  // delegate to the head module
  MODULE_T* module_p = NULL;
  result = inherited::top (module_p);
  if ((result == -1) || !module_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Stream::top(): \"%m\", returning\n")));
    return;
  } // end IF

  ISTREAM_CONTROL_T* control_impl_p = NULL;
  control_impl_p = dynamic_cast<ISTREAM_CONTROL_T*> (module_p->writer ());
  if (!control_impl_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: dynamic_cast<Stream_IStreamControl) failed> (returning\n"),
                ACE_TEXT (module_p->name ())));
    return;
  } // end IF

  try
  {
    control_impl_p->pause ();
  }
  catch (...)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("caught exception in Stream_IStreamControl::pause (module: \"%s\"), returning\n"),
                ACE_TEXT (module_p->name ())));
    return;
  }
}

void
Stream::rewind ()
{
  STREAM_TRACE (ACE_TEXT ("Stream::rewind"));

  int result = -1;

  // sanity check: is running ?
  if (isRunning ())
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("currently running, returning\n")));
    return;
  } // end IF

  // delegate to the head module
  MODULE_T* module_p = NULL;
  result = inherited::top (module_p);
  if ((result == -1) || !module_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Stream::top(): \"%m\", returning\n")));
    return;
  } // end IF

  ISTREAM_CONTROL_T* control_impl_p = NULL;
  control_impl_p = dynamic_cast<ISTREAM_CONTROL_T*> (module_p->writer ());
  if (!control_impl_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: dynamic_cast<Stream_IStreamControl> failed, returning\n"),
                ACE_TEXT (module_p->name ())));
    return;
  } // end IF

  try
  {
    control_impl_p->rewind ();
  }
  catch (...)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("caught exception in Stream_IStreamControl::rewind (module: \"%s\"), returning\n"),
                ACE_TEXT (module_p->name ())));
    return;
  }
}

void
Stream::waitForCompletion ()
{
  STREAM_TRACE (ACE_TEXT ("Stream::waitForCompletion"));

  int result = -1;

  // sanity check: is initialized ?
  if (!isInitialized ())
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("not initialized, returning\n")));
    return;
  } // end IF

  // OK: the logic here is this...
  // step1: wait for processing to finish
  // step2: wait for the pipelined messages to flush...

  // iterate over modules...
  const MODULE_T* module_p = NULL;
  ITERATOR_T iterator (*this);
  // skip over ACE_Stream_Head
  result = iterator.advance ();
  if (result)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("no ACE_Stream_Head module found: \"%m\", returning\n")));
    return;
  } // end IF

  // get head module
  result = iterator.next (module_p);
  if (result || !module_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("no head module found: \"%m\", returning\n")));
    return;
  } // end IF

  // sanity check: head == tail ? --> no modules have been push()ed (yet) !
  if (module_p == inherited::tail ())
  {
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("no modules have been enqueued yet --> nothing to do !, returning\n")));
    return;
  } // end IF

  // need to downcast
  Stream_HeadModuleTask* head_task_p = NULL;
  head_task_p =
    dynamic_cast<Stream_HeadModuleTask*> (const_cast<MODULE_T*> (module_p)->writer ());
  if (!head_task_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: dynamic_cast<Stream_HeadModuleTask> failed, returning\n"),
                ACE_TEXT (module_p->name ())));
    return;
  } // end IF

//   ACE_DEBUG ((LM_DEBUG,
//               ACE_TEXT ("waiting for module (\"%s\") to finish processing...\n"),
//               ACE_TEXT (module->name ())));

  // OK: got a handler... wait on it
  try
  {
    // step1: wait for state switch (xxx --> FINISHED)
    head_task_p->waitForCompletion ();

    // step2: wait for worker thread
    result = head_task_p->wait ();
    if (result == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("%s: failed to ACE_Task_Base::wait(): \"%m\", continuing\n"),
                  ACE_TEXT (module_p->name ())));
  }
  catch (...)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("caught exception in Stream_IStreamControl::waitForCompletion || wait (module: \"%s\"), returning\n"),
                ACE_TEXT (module_p->name ())));
    return;
  }

//   ACE_DEBUG ((LM_DEBUG,
//               ACE_TEXT ("waiting for module (\"%s\") to finish processing...DONE\n"),
//               ACE_TEXT (module->name ())));
//   ACE_DEBUG ((LM_DEBUG,
//               ACE_TEXT ("waiting for stream to flush...\n")));

  Stream_Task_t* task_p = NULL;
  for (iterator.advance ();
       iterator.next (module_p);
       iterator.advance ())
  {
    // skip stream tail (last module)
    if (module_p == inherited::tail ())
    {
      // skip last module (stream tail)
      continue;
    } // end IF

//     ACE_DEBUG ((LM_DEBUG,
//                 ACE_TEXT ("waiting for module (\"%s\") to finish processing...\n"),
//                 ACE_TEXT (module->name ())));

    // OK: got a handle... wait
    task_p = const_cast<MODULE_T*> (module_p)->writer ();
    ACE_ASSERT (task_p);
    result = task_p->wait ();
    if (result == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("%s: failed to ACE_Task_Base::wait(): \"%m\", continuing\n"),
                  ACE_TEXT (module_p->name ())));

//     ACE_DEBUG ((LM_DEBUG,
//                 ACE_TEXT ("waiting for module (\"%s\") to finish processing...DONE\n"),
//                 ACE_TEXT (module->name ())));

    module_p = NULL;
  } // end FOR

  // OK: iff (!) no new messages have been enqueued IN THE MEANTIME, the queue should be empty...
//   ACE_DEBUG ((LM_DEBUG,
//               ACE_TEXT ("waiting for stream to flush...DONE\n")));
}

Stream_State_t*
Stream::getState ()
{
  STREAM_TRACE (ACE_TEXT ("Stream::getState"));

  return &state_;
}

void
Stream::dump_state () const
{
  STREAM_TRACE (ACE_TEXT ("Stream::dump_state"));

  std::string stream_layout;
  const MODULE_T* module = NULL;
  for (ITERATOR_T iterator (*this);
       iterator.next (module);
       iterator.advance ())
  {
    // silently ignore ACE head/tail modules...
    if ((module == const_cast<Stream*>(this)->tail ()) ||
        (module == const_cast<Stream*>(this)->head ()))
      continue;

    stream_layout.append (ACE_TEXT_ALWAYS_CHAR (module->name ()));

    // avoid trailing "-->"...
    if (const_cast<MODULE_T*> (module)->next () !=
        const_cast<Stream*> (this)->tail ())
      stream_layout += ACE_TEXT_ALWAYS_CHAR (" --> ");

    module = NULL;
  } // end FOR

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("stream layout: \"%s\"\n"),
              ACE_TEXT (stream_layout.c_str ())));
}

bool
Stream::isInitialized () const
{
  STREAM_TRACE (ACE_TEXT ("Stream::isInitialized"));

  return isInitialized_;
}

void
Stream::shutdown ()
{
  STREAM_TRACE (ACE_TEXT ("Stream::shutdown"));

//   ACE_DEBUG ((LM_DEBUG,
//               ACE_TEXT ("shutting down stream...\n")));

  // if we haven't properly initialized, we need to deactivate any hitherto enqueued
  // ACTIVE modules, or we will wait forever during closure...
  // --> possible scenarios:
  // - (re-)init() failed halfway through (i.e. MAYBE some modules push()ed correctly)
  if (!isInitialized_)
  {
    // sanity check: have we actually successfully pushed() any modules ?
    if (inherited::head ()->next () != inherited::tail ())
    {
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("not initialized - deactivating module(s)...\n")));

      deactivateModules ();

//       ACE_DEBUG ((LM_DEBUG,
//                   ACE_TEXT ("not initialized - deactivating module(s)...WAITING\n")));

      // ...and wait for it to complete ?
      // *NOTE*: children can synchronize by calling waitForCompletion...

      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("not initialized - deactivating module(s)...DONE\n")));
    } // end IF
  } // end IF

  // step1: retrieve a list of modules which are NOT on the stream
  // --> need to close() these manually (before they do so in their dtors...)
//   ACE_DEBUG ((LM_DEBUG,
//               ACE_TEXT ("deactivating offline module(s)...\n")));

  for (MODULE_CONTAINER_ITERATOR_T iter = availableModules_.begin ();
       iter != availableModules_.end ();
       iter++)
  {
    if ((*iter)->next () == NULL)
    {
//       ACE_DEBUG ((LM_DEBUG,
//                   ACE_TEXT ("manually closing module: \"%s\"\n"),
//                   ACE_TEXT ((*iter)->name ())));

      try
      {
        (*iter)->close (ACE_Module_Base::M_DELETE_NONE);
      }
      catch (...)
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("caught exception in ACE_Module::close(M_DELETE_NONE), continuing\n")));
      }
    } // end IF
  } // end FOR

//   ACE_DEBUG ((LM_DEBUG,
//               ACE_TEXT ("deactivating offline module(s)...DONE\n")));

  // step2: shutdown stream
  // check the ACE documentation on ACE_Stream to see why this is needed !!!
  // Note: ONLY do this if stream_head != 0 !!! (warning: obsolete ?)
  // *NOTE*: this will NOT destroy all modules in the current stream
  // as this leads to exceptions in debug builds under windows (can't delete
  // objects in a different DLL where it was created...)
  // --> we need to do this ourselves !
  // all this does is call close() on each one (--> wait for the worker thread to return)
//   ACE_DEBUG ((LM_DEBUG,
//               ACE_TEXT ("deactivating module(s)...\n")));

  finalize ();

//   ACE_DEBUG ((LM_DEBUG,
//               ACE_TEXT ("deactivating module(s)...DONE\n")));

  // *NOTE*: every ACTIVE module will join with its worker thread in close()
  // --> ALL stream-related threads should have returned by now !

//   ACE_DEBUG ((LM_DEBUG,
//               ACE_TEXT ("shutting down stream...FINISHED\n")));
}

void
Stream::deactivateModules ()
{
  STREAM_TRACE (ACE_TEXT ("Stream::deactivateModules"));

  // allocate session data
  Stream_SessionData_t* session_data_p = NULL;
  ACE_NEW_NORETURN (session_data_p,
                    Stream_SessionData_t (NULL,
                                          false));
  if (!session_data_p)
  {
    ACE_DEBUG ((LM_CRITICAL,
                ACE_TEXT ("failed to allocate Stream_SessionData_t: \"%m\", returning\n")));

    return;
  } // end IF

  // allocate SESSION_END session message
  Stream_SessionMessage* message_p = NULL;
  if (allocator_)
  {
    try
    { // *NOTE*: 0 --> allocate a session message
      message_p = static_cast<Stream_SessionMessage*> (allocator_->malloc (0));
    }
    catch (...)
    {
      ACE_DEBUG ((LM_CRITICAL,
                 ACE_TEXT ("caught exception in Stream_IAllocator::malloc(0), aborting\n")));

      // clean up
      delete session_data_p;

      return;
    }
  } // end IF
  else
  { // *NOTE*: session message assumes responsibility for session_data_p
    ACE_NEW_NORETURN (message_p,
                      Stream_SessionMessage (SESSION_END,
                                             &state_,
                                             session_data_p));
  } // end ELSE
  if (!message_p)
  {
    ACE_DEBUG ((LM_CRITICAL,
                ACE_TEXT ("failed to allocate Stream_SessionMessage: \"%m\", returning\n")));

    // clean up
    delete session_data_p;

    return;
  } // end IF
  if (allocator_)
  { // *NOTE*: session message assumes responsibility for session_data_p
    message_p->initialize (SESSION_END,
                           &state_,
                           session_data_p);
  } // end IF

  // send message downstream...
  if (inherited::put (message_p, NULL) == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Stream::put(): \"%m\", returning\n")));

    // clean up
    message_p->release ();

    return;
  } // end IF
}
