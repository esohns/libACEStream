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

#include "ace/Guard_T.h"
#include "ace/Log_Msg.h"
#include "ace/Module.h"
#include "ace/OS_Memory.h"

#include "stream_macros.h"

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename ConfigurationType,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType,
          typename SessionIdType,
          typename SessionDataType>
Stream_Module_MessageHandler_T<ACE_SYNCH_USE,
                               TimePolicyType,
                               ConfigurationType,
                               ControlMessageType,
                               DataMessageType,
                               SessionMessageType,
                               SessionIdType,
#if defined (ACE_WIN32) || defined (ACE_WIN64)
                               SessionDataType>::Stream_Module_MessageHandler_T (ISTREAM_T* stream_in)
#else
                               SessionDataType>::Stream_Module_MessageHandler_T (typename inherited::ISTREAM_T* stream_in)
#endif
 : inherited (stream_in)
 , delete_ (false)
 , lock_ (NULL)
 , subscribers_ (NULL)
{
  STREAM_TRACE (ACE_TEXT ("Stream_Module_MessageHandler_T::Stream_Module_MessageHandler_T"));

}

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename ConfigurationType,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType,
          typename SessionIdType,
          typename SessionDataType>
Stream_Module_MessageHandler_T<ACE_SYNCH_USE,
                               TimePolicyType,
                               ConfigurationType,
                               ControlMessageType,
                               DataMessageType,
                               SessionMessageType,
                               SessionIdType,
                               SessionDataType>::~Stream_Module_MessageHandler_T ()
{
  STREAM_TRACE (ACE_TEXT ("Stream_Module_MessageHandler_T::~Stream_Module_MessageHandler_T"));

  // clean up
  if (delete_)
  {
    delete lock_;
    delete subscribers_;
  } // end IF
}

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename ConfigurationType,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType,
          typename SessionIdType,
          typename SessionDataType>
bool
Stream_Module_MessageHandler_T<ACE_SYNCH_USE,
                               TimePolicyType,
                               ConfigurationType,
                               ControlMessageType,
                               DataMessageType,
                               SessionMessageType,
                               SessionIdType,
                               SessionDataType>::initialize (const ConfigurationType& configuration_in,
                                                             Stream_IAllocator* allocator_in)
{
  STREAM_TRACE (ACE_TEXT ("Stream_Module_MessageHandler_T::initialize"));

  // sanity check(s)
  ACE_ASSERT ((configuration_in.subscribers && configuration_in.subscribersLock) ||
              (!configuration_in.subscribers && !configuration_in.subscribersLock));

  if (inherited::isInitialized_)
  {
    if (delete_)
    {
      delete_ = false;

      delete lock_;
      lock_ = NULL;
      delete subscribers_;
      subscribers_ = NULL;
    } // end IF

    if (subscribers_)
      subscribers_->clear ();
  } // end IF

  // *TODO*: remove type inferences
  delete_ =
      (!configuration_in.subscribersLock && !configuration_in.subscribers);
  if (configuration_in.subscribersLock)
    lock_ = configuration_in.subscribersLock;
  else
  {
    if (lock_)
      goto continue_3;

    ACE_NEW_NORETURN (lock_,
                      typename ACE_SYNCH_USE::RECURSIVE_MUTEX (NULL, NULL));
    if (!lock_)
    {
      ACE_DEBUG ((LM_CRITICAL,
                  ACE_TEXT ("%s: failed to allocate memory: \"%m\", aborting\n"),
                  inherited::mod_->name ()));

      // clean up
      delete_ = false;

      return false;
    } // end IF
  } // end IF

continue_3:
  if (configuration_in.subscribers)
    subscribers_ = configuration_in.subscribers;
  else
  {
    if (subscribers_)
      goto continue_2;

    ACE_NEW_NORETURN (subscribers_,
                      SUBSCRIBERS_T ());
    if (!subscribers_)
    {
      ACE_DEBUG ((LM_CRITICAL,
                  ACE_TEXT ("%s: failed to allocate memory: \"%m\", aborting\n"),
                  inherited::mod_->name ()));

      // clean up
      if (delete_)
      {
        delete lock_;
        lock_ = NULL;
      } // end IF
      delete_ = false;

      return false;
    } // end IF
  } // end IF
  ACE_ASSERT (lock_ && subscribers_);

continue_2:
  if (configuration_in.subscriber)
  { ACE_GUARD_RETURN (typename ACE_SYNCH_USE::RECURSIVE_MUTEX, aGuard, *lock_, false);
    // *TODO*: remove type inference
    subscribers_->push_back (configuration_in.subscriber);
    subscribers_->unique (SUBSCRIBERS_IS_EQUAL_P ());
  } // end IF

//continue_:
  return inherited::initialize (configuration_in,
                                allocator_in);
}

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename ConfigurationType,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType,
          typename SessionIdType,
          typename SessionDataType>
void
Stream_Module_MessageHandler_T<ACE_SYNCH_USE,
                               TimePolicyType,
                               ConfigurationType,
                               ControlMessageType,
                               DataMessageType,
                               SessionMessageType,
                               SessionIdType,
                               SessionDataType>::handleDataMessage (DataMessageType*& message_inout,
                                                                    bool& passMessageDownstream_out)
{
  STREAM_TRACE (ACE_TEXT ("Stream_Module_MessageHandler_T::handleDataMessage"));

  // don't care (implies yes per default, if part of a stream)
  ACE_UNUSED_ARG (passMessageDownstream_out);

  // sanity check(s)
  if (!inherited::sessionData_)
    return;
  ACE_ASSERT (lock_ && subscribers_);

  // forward the message to any subscriber(s)
  const SessionDataType& session_data_r = inherited::sessionData_->getR ();

  // synch access
  { ACE_GUARD (typename ACE_SYNCH_USE::RECURSIVE_MUTEX, aGuard, *lock_);
    // *WARNING* callees unsubscribe()ing within the callback invalidate the
    //           iterator
    //           --> use a slightly modified for-loop (advance before
    //               invoking the callback; works for MOST containers)
    // *NOTE*: this works because the lock is recursive
    for (SUBSCRIBERS_ITERATOR_T iterator = subscribers_->begin ();
         iterator != subscribers_->end ();
         )
    {
      try {
        // *TODO*: remove type inference
        (*iterator++)->notify (session_data_r.sessionId,
                               *message_inout);
      } catch (...) {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("%s: caught exception in Common_INotify_T::notify (), continuing\n"),
                    inherited::mod_->name ()));
      }
    } // end FOR
  } // end lock scope
}

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename ConfigurationType,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType,
          typename SessionIdType,
          typename SessionDataType>
void
Stream_Module_MessageHandler_T<ACE_SYNCH_USE,
                               TimePolicyType,
                               ConfigurationType,
                               ControlMessageType,
                               DataMessageType,
                               SessionMessageType,
                               SessionIdType,
                               SessionDataType>::handleSessionMessage (SessionMessageType*& message_inout,
                                                                       bool& passMessageDownstream_out)
{
  STREAM_TRACE (ACE_TEXT ("Stream_Module_MessageHandler_T::handleSessionMessage"));

  // don't care (implies yes per default, if part of a stream)
  ACE_UNUSED_ARG (passMessageDownstream_out);

  // sanity check(s)
  ACE_ASSERT (lock_ && subscribers_);

  const SessionDataType* session_data_p = NULL;
  switch (message_inout->type ())
  {
    case STREAM_SESSION_MESSAGE_BEGIN:
    { ACE_ASSERT (inherited::sessionData_);
      session_data_p = &inherited::sessionData_->getR ();

      { ACE_GUARD (typename ACE_SYNCH_USE::RECURSIVE_MUTEX, aGuard, *lock_);
        // *NOTE*: this works because the lock is recursive
        // *WARNING* if callees unsubscribe() within the callback bad things
        //           happen, as the current iterator is invalidated
        //           --> use a slightly modified for-loop (advance first before
        //               invoking the callback (works for MOST containers...)
        for (SUBSCRIBERS_ITERATOR_T iterator = subscribers_->begin ();
             iterator != subscribers_->end ();
             )
        {
          try {
            // *TODO*: remove type inference
            (*iterator++)->start (session_data_p->sessionId,
                                  *session_data_p);
          } catch (...) {
            ACE_DEBUG ((LM_ERROR,
                        ACE_TEXT ("%s: caught exception in Common_INotify_T::start(), continuing\n"),
                        inherited::mod_->name ()));
            goto error;
          }
        } // end FOR
      } // end lock scope

      break;

error:
      this->notify (STREAM_SESSION_MESSAGE_ABORT);

      return;
    }
    case STREAM_SESSION_MESSAGE_END:
    {
      // *NOTE*: the modules' session data handle may have gone away already
      //         --> use the messages' reference instead
      const typename SessionMessageType::DATA_T& session_data_container_r =
        message_inout->getR ();
      session_data_p = &session_data_container_r.getR ();

      { ACE_GUARD (typename ACE_SYNCH_USE::RECURSIVE_MUTEX, aGuard, *lock_);
        // *WARNING* if callees unsubscribe() within the callback bad things
        //           happen, as the current iterator is invalidated
        //           --> use a slightly modified for-loop (advance first before
        //               invoking the callback (works for MOST containers...)
        // *NOTE*: this works because the lock is recursive
        for (SUBSCRIBERS_ITERATOR_T iterator = subscribers_->begin ();
             iterator != subscribers_->end ();
             )
        {
          try {
            // *TODO*: remove type inference
            (*(iterator++))->end (session_data_p->sessionId);
          } catch (...) {
            ACE_DEBUG ((LM_ERROR,
                        ACE_TEXT ("%s: caught exception in Common_INotify_T::end(), continuing\n"),
                        inherited::mod_->name ()));
          }
        } // end FOR
      } // end lock scope

      break;
    }
    default:
    {
      // *NOTE*: the modules' session data handle may not have been set yet
      //         --> use the messages' reference instead
      const typename SessionMessageType::DATA_T& session_data_container_r =
        message_inout->getR ();
      session_data_p = &session_data_container_r.getR ();

      { ACE_GUARD (typename ACE_SYNCH_USE::RECURSIVE_MUTEX, aGuard, *lock_);
        // *WARNING* callees unsubscribe()ing within the callback invalidate the
        //           iterator
        //           --> use a slightly modified for-loop (advance before
        //               invoking the callback; works for MOST containers)
        // *NOTE*: this works because the lock is recursive
        for (SUBSCRIBERS_ITERATOR_T iterator = subscribers_->begin ();
             iterator != subscribers_->end ();
             )
        {
          try {
            // *TODO*: remove type inference
            (*(iterator++))->notify (session_data_p->sessionId,
                                     *message_inout);
          } catch (...) {
            ACE_DEBUG ((LM_ERROR,
                        ACE_TEXT ("%s: caught exception in Common_INotify_T::notify(), continuing\n"),
                        inherited::mod_->name ()));
          }
        } // end FOR
      } // end lock scope

      break;
    }
  } // end SWITCH
}

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename ConfigurationType,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType,
          typename SessionIdType,
          typename SessionDataType>
void
Stream_Module_MessageHandler_T<ACE_SYNCH_USE,
                               TimePolicyType,
                               ConfigurationType,
                               ControlMessageType,
                               DataMessageType,
                               SessionMessageType,
                               SessionIdType,
                               SessionDataType>::subscribe (INOTIFY_T* interfaceHandle_in)
{
  STREAM_TRACE (ACE_TEXT ("Stream_Module_MessageHandler_T::subscribe"));

  // sanity check(s)
  ACE_ASSERT (lock_ && subscribers_);
  ACE_ASSERT (interfaceHandle_in);
  //if (!interfaceHandle_in)
  //{
  //  ACE_DEBUG ((LM_ERROR,
  //              ACE_TEXT ("invalid argument (was: 0x%@), returning\n"),
  //              interfaceHandle_in));
  //  return;
  //} // end IF

  { ACE_GUARD (typename ACE_SYNCH_USE::RECURSIVE_MUTEX, aGuard, *lock_);
    subscribers_->push_back (interfaceHandle_in);
  } // end lock scope
}

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename ConfigurationType,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType,
          typename SessionIdType,
          typename SessionDataType>
void
Stream_Module_MessageHandler_T<ACE_SYNCH_USE,
                               TimePolicyType,
                               ConfigurationType,
                               ControlMessageType,
                               DataMessageType,
                               SessionMessageType,
                               SessionIdType,
                               SessionDataType>::unsubscribe (INOTIFY_T* interfaceHandle_in)
{
  STREAM_TRACE (ACE_TEXT ("Stream_Module_MessageHandler_T::unsubscribe"));

  // sanity check(s)
  ACE_ASSERT (lock_ && subscribers_);
  ACE_ASSERT (interfaceHandle_in);

  { ACE_GUARD (typename ACE_SYNCH_USE::RECURSIVE_MUTEX, aGuard, *lock_);
    SUBSCRIBERS_ITERATOR_T iterator = subscribers_->begin ();
    for (;
         iterator != subscribers_->end ();
         iterator++)
      if ((*iterator) == interfaceHandle_in)
        break;

    if (iterator != subscribers_->end ())
      subscribers_->erase (iterator);
    else
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("%s: invalid argument (was: %@), continuing\n"),
                  inherited::mod_->name (),
                  interfaceHandle_in));
  } // end lock scope
}

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename ConfigurationType,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType,
          typename SessionIdType,
          typename SessionDataType>
bool
Stream_Module_MessageHandler_T<ACE_SYNCH_USE,
                               TimePolicyType,
                               ConfigurationType,
                               ControlMessageType,
                               DataMessageType,
                               SessionMessageType,
                               SessionIdType,
                               SessionDataType>::postClone (ACE_Module<ACE_SYNCH_USE,
                                                                       TimePolicyType>* original_in,
                                                            bool initialize_in)
{
  STREAM_TRACE (ACE_TEXT ("Stream_Module_MessageHandler_T::postClone"));

  if (!initialize_in) return true;

  // sanity check(s)
  ACE_ASSERT (original_in);

  OWN_TYPE_T* message_handler_impl_p =
    dynamic_cast<OWN_TYPE_T*> (original_in->writer ());
  if (!message_handler_impl_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: dynamic_cast<Stream_Module_MessageHandler_T> failed, aborting\n"),
                inherited::mod_->name ()));
    return false;
  } // end IF

  // sanity check(s)
  ACE_ASSERT (message_handler_impl_p->configuration_);

  if (!inherited::initialize (*message_handler_impl_p->configuration_,
                               message_handler_impl_p->allocator_))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: failed to Stream_Module_MessageHandler_T::initialize(), aborting\n"),
                inherited::mod_->name ()));
    return false;
  } // end IF

  return true;
}

//////////////////////////////////////////

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename ConfigurationType,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType,
          typename SessionIdType,
          typename SessionDataType>
Stream_Module_MessageHandlerA_T<ACE_SYNCH_USE,
                                TimePolicyType,
                                ConfigurationType,
                                ControlMessageType,
                                DataMessageType,
                                SessionMessageType,
                                SessionIdType,
#if defined (ACE_WIN32) || defined (ACE_WIN64)
                                SessionDataType>::Stream_Module_MessageHandlerA_T (ISTREAM_T* stream_in)
#else
                                SessionDataType>::Stream_Module_MessageHandlerA_T (typename inherited::ISTREAM_T* stream_in)
#endif
 : inherited (stream_in)
 , delete_ (false)
 , subscribers_ (NULL)
 , subscribersLock_ (NULL)
{
  STREAM_TRACE (ACE_TEXT ("Stream_Module_MessageHandlerA_T::Stream_Module_MessageHandlerA_T"));

}

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename ConfigurationType,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType,
          typename SessionIdType,
          typename SessionDataType>
Stream_Module_MessageHandlerA_T<ACE_SYNCH_USE,
                                TimePolicyType,
                                ConfigurationType,
                                ControlMessageType,
                                DataMessageType,
                                SessionMessageType,
                                SessionIdType,
                                SessionDataType>::~Stream_Module_MessageHandlerA_T ()
{
  STREAM_TRACE (ACE_TEXT ("Stream_Module_MessageHandlerA_T::~Stream_Module_MessageHandlerA_T"));

  // clean up
  if (delete_)
  {
    delete subscribers_;
    delete subscribersLock_;
  } // end IF
}

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename ConfigurationType,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType,
          typename SessionIdType,
          typename SessionDataType>
bool
Stream_Module_MessageHandlerA_T<ACE_SYNCH_USE,
                                TimePolicyType,
                                ConfigurationType,
                                ControlMessageType,
                                DataMessageType,
                                SessionMessageType,
                                SessionIdType,
                                SessionDataType>::initialize (const ConfigurationType& configuration_in,
                                                              Stream_IAllocator* allocator_in)
{
  STREAM_TRACE (ACE_TEXT ("Stream_Module_MessageHandlerA_T::initialize"));

  // sanity check(s)
  ACE_ASSERT ((configuration_in.subscribers && configuration_in.subscribersLock) ||
              (!configuration_in.subscribers && !configuration_in.subscribersLock));

  if (inherited::isInitialized_)
    goto continue_2;

  // *TODO*: remove type inferences
  delete_ =
      (!configuration_in.subscribersLock && !configuration_in.subscribers);
  if (configuration_in.subscribersLock)
    subscribersLock_ = configuration_in.subscribersLock;
  else
  {
    if (subscribersLock_)
      goto continue_3;

    ACE_NEW_NORETURN (subscribersLock_,
                      typename ACE_SYNCH_USE::RECURSIVE_MUTEX (NULL, NULL));
    if (!subscribersLock_)
    {
      ACE_DEBUG ((LM_CRITICAL,
                  ACE_TEXT ("%s: failed to allocate memory: \"%m\", aborting\n"),
                  inherited::mod_->name ()));

      // clean up
      delete_ = false;

      return false;
    } // end IF
  } // end IF

continue_3:
  if (configuration_in.subscribers)
    subscribers_ = configuration_in.subscribers;
  else
  {
    if (subscribers_)
      goto continue_2;

    ACE_NEW_NORETURN (subscribers_,
                      SUBSCRIBERS_T ());
    if (!subscribers_)
    {
      ACE_DEBUG ((LM_CRITICAL,
                  ACE_TEXT ("%s: failed to allocate memory: \"%m\", aborting\n"),
                  inherited::mod_->name ()));

      // clean up
      if (delete_)
      {
        delete subscribersLock_;
        subscribersLock_ = NULL;
      } // end IF
      delete_ = false;

      return false;
    } // end IF
  } // end IF
  ACE_ASSERT (subscribersLock_ && subscribers_);

continue_2:
  if (configuration_in.subscriber)
  { ACE_GUARD_RETURN (typename ACE_SYNCH_USE::RECURSIVE_MUTEX, aGuard, *subscribersLock_, false);
    // *NOTE*: do not allow 'duplicates'
    for (SUBSCRIBERS_ITERATOR_T iterator = subscribers_->begin ();
         iterator != subscribers_->end ();
         ++iterator)
      if (configuration_in.subscriber == *iterator)
        goto continue_;

    // *TODO*: remove type inference
    subscribers_->push_back (configuration_in.subscriber);
  } // end IF

continue_:
  return inherited::initialize (configuration_in,
                                allocator_in);
}

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename ConfigurationType,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType,
          typename SessionIdType,
          typename SessionDataType>
void
Stream_Module_MessageHandlerA_T<ACE_SYNCH_USE,
                                TimePolicyType,
                                ConfigurationType,
                                ControlMessageType,
                                DataMessageType,
                                SessionMessageType,
                                SessionIdType,
                                SessionDataType>::handleDataMessage (DataMessageType*& message_inout,
                                                                     bool& passMessageDownstream_out)
{
  STREAM_TRACE (ACE_TEXT ("Stream_Module_MessageHandlerA_T::handleDataMessage"));

  // don't care (implies yes per default, if part of a stream)
  ACE_UNUSED_ARG (passMessageDownstream_out);

  // sanity check(s)
  ACE_ASSERT (subscribersLock_ && subscribers_);

  // forward the message to any subscriber(s)
  { ACE_GUARD (typename ACE_SYNCH_USE::RECURSIVE_MUTEX, aGuard, *subscribersLock_);
    // *WARNING* callees unsubscribe()ing within the callback invalidate the
    //           iterator
    //           --> use a slightly modified for-loop (advance before
    //               invoking the callback; works for MOST containers)
    // *NOTE*: this works because the lock is recursive
    for (SUBSCRIBERS_ITERATOR_T iterator = subscribers_->begin ();
         iterator != subscribers_->end ();
         )
    {
      try {
        // *TODO*: remove type inference
        (*iterator++)->notify (message_inout->sessionId (),
                               *message_inout);
      } catch (...) {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("%s: caught exception in Common_INotify_T::notify (), continuing\n"),
                    inherited::mod_->name ()));
      }
    } // end FOR
  } // end lock scope
}

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename ConfigurationType,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType,
          typename SessionIdType,
          typename SessionDataType>
void
Stream_Module_MessageHandlerA_T<ACE_SYNCH_USE,
                                TimePolicyType,
                                ConfigurationType,
                                ControlMessageType,
                                DataMessageType,
                                SessionMessageType,
                                SessionIdType,
                                SessionDataType>::handleSessionMessage (SessionMessageType*& message_inout,
                                                                        bool& passMessageDownstream_out)
{
  STREAM_TRACE (ACE_TEXT ("Stream_Module_MessageHandlerA_T::handleSessionMessage"));

  // don't care (implies yes per default, if part of a stream)
  ACE_UNUSED_ARG (passMessageDownstream_out);

  // sanity check(s)
  ACE_ASSERT (subscribersLock_ && subscribers_);

  inherited::handleSessionMessage (message_inout,
                                   passMessageDownstream_out);
  if (!passMessageDownstream_out)
    return;

  switch (message_inout->type ())
  {
    case STREAM_SESSION_MESSAGE_BEGIN:
    {
      // *NOTE*: the module may be handling multiple sessions in parallel
      //         --> use the messages' session data reference
      const typename SessionMessageType::DATA_T& session_data_container_r =
          message_inout->getR ();
      const SessionDataType& session_data_r = session_data_container_r.getR ();

      { ACE_GUARD (typename ACE_SYNCH_USE::RECURSIVE_MUTEX, aGuard, *subscribersLock_);
        // *WARNING* callees unsubscribe()ing within the callback invalidate the
        //           iterator
        //           --> use a slightly modified for-loop (advance before
        //               invoking the callback; works for MOST containers)
        // *NOTE*: this works because the lock is recursive
        for (SUBSCRIBERS_ITERATOR_T iterator = subscribers_->begin ();
             iterator != subscribers_->end ();
             )
        {
          try {
            // *TODO*: remove type inference
            (*iterator++)->start (message_inout->sessionId (),
                                  session_data_r);
          } catch (...) {
            ACE_DEBUG ((LM_ERROR,
                        ACE_TEXT ("%s: caught exception in Common_INotify_T::start(), continuing\n"),
                        inherited::mod_->name ()));
            goto error;
          }
        } // end FOR
      } // end lock scope

      break;

error:
      this->notify (STREAM_SESSION_MESSAGE_ABORT);

      return;
    }
    case STREAM_SESSION_MESSAGE_END:
    {
      { ACE_GUARD (typename ACE_SYNCH_USE::RECURSIVE_MUTEX, aGuard, *subscribersLock_);
        // *WARNING* callees unsubscribe()ing within the callback invalidate the
        //           iterator
        //           --> use a slightly modified for-loop (advance before
        //               invoking the callback; works for MOST containers)
        // *NOTE*: this works because the lock is recursive
        for (SUBSCRIBERS_ITERATOR_T iterator = subscribers_->begin ();
             iterator != subscribers_->end ();
             )
        {
          try {
            // *TODO*: remove type inference
            (*(iterator++))->end (message_inout->sessionId ());
          } catch (...) {
            ACE_DEBUG ((LM_ERROR,
                        ACE_TEXT ("%s: caught exception in Common_INotify_T::end(), continuing\n"),
                        inherited::mod_->name ()));
          }
        } // end FOR
      } // end lock scope

      break;
    }
    default:
    {
      { ACE_GUARD (typename ACE_SYNCH_USE::RECURSIVE_MUTEX, aGuard, *subscribersLock_);
        // *WARNING* callees unsubscribe()ing within the callback invalidate the
        //           iterator
        //           --> use a slightly modified for-loop (advance before
        //               invoking the callback; works for MOST containers)
        // *NOTE*: this works because the lock is recursive
        for (SUBSCRIBERS_ITERATOR_T iterator = subscribers_->begin ();
             iterator != subscribers_->end ();
             )
        {
          try {
            // *TODO*: remove type inference
            (*(iterator++))->notify (message_inout->sessionId (),
                                     *message_inout);
          } catch (...) {
            ACE_DEBUG ((LM_ERROR,
                        ACE_TEXT ("%s: caught exception in Common_INotify_T::notify(), continuing\n"),
                        inherited::mod_->name ()));
          }
        } // end FOR
      } // end lock scope

      break;
    }
  } // end SWITCH
}

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename ConfigurationType,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType,
          typename SessionIdType,
          typename SessionDataType>
void
Stream_Module_MessageHandlerA_T<ACE_SYNCH_USE,
                                TimePolicyType,
                                ConfigurationType,
                                ControlMessageType,
                                DataMessageType,
                                SessionMessageType,
                                SessionIdType,
                                SessionDataType>::subscribe (INOTIFY_T* interfaceHandle_in)
{
  STREAM_TRACE (ACE_TEXT ("Stream_Module_MessageHandlerA_T::subscribe"));

  // sanity check(s)
  ACE_ASSERT (subscribersLock_ && subscribers_);
  ACE_ASSERT (interfaceHandle_in);

  { ACE_GUARD (typename ACE_SYNCH_USE::RECURSIVE_MUTEX, aGuard, *subscribersLock_);
    subscribers_->push_back (interfaceHandle_in);
  } // end lock scope
}

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename ConfigurationType,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType,
          typename SessionIdType,
          typename SessionDataType>
void
Stream_Module_MessageHandlerA_T<ACE_SYNCH_USE,
                                TimePolicyType,
                                ConfigurationType,
                                ControlMessageType,
                                DataMessageType,
                                SessionMessageType,
                                SessionIdType,
                                SessionDataType>::unsubscribe (INOTIFY_T* interfaceHandle_in)
{
  STREAM_TRACE (ACE_TEXT ("Stream_Module_MessageHandlerA_T::unsubscribe"));

  // sanity check(s)
  ACE_ASSERT (subscribersLock_ && subscribers_);
  ACE_ASSERT (interfaceHandle_in);

  SUBSCRIBERS_ITERATOR_T iterator;
  { ACE_GUARD (typename ACE_SYNCH_USE::RECURSIVE_MUTEX, aGuard, *subscribersLock_);
    iterator = subscribers_->begin ();
    for (;
         iterator != subscribers_->end ();
         iterator++)
      if ((*iterator) == interfaceHandle_in)
        break;

    if (iterator != subscribers_->end ())
      subscribers_->erase (iterator);
    else
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("%s: invalid argument (was: %@), continuing\n"),
                  inherited::mod_->name (),
                  interfaceHandle_in));
  } // end lock scope
}

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename ConfigurationType,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType,
          typename SessionIdType,
          typename SessionDataType>
bool
Stream_Module_MessageHandlerA_T<ACE_SYNCH_USE,
                                TimePolicyType,
                                ConfigurationType,
                                ControlMessageType,
                                DataMessageType,
                                SessionMessageType,
                                SessionIdType,
                                SessionDataType>::postClone (ACE_Module<ACE_SYNCH_USE,
                                                                        TimePolicyType>* original_in,
                                                             bool initialize_in)
{
  STREAM_TRACE (ACE_TEXT ("Stream_Module_MessageHandlerA_T::postClone"));

  if (!initialize_in)
    return true;

  // sanity check(s)
  ACE_ASSERT (original_in);

  OWN_TYPE_T* message_handler_impl_p =
    dynamic_cast<OWN_TYPE_T*> (original_in->writer ());
  if (!message_handler_impl_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: dynamic_cast<Stream_Module_MessageHandlerA_T> failed, aborting\n"),
                inherited::mod_->name ()));
    return false;
  } // end IF

  // sanity check(s)
  ACE_ASSERT (message_handler_impl_p->configuration_);

  if (!inherited::initialize (*message_handler_impl_p->configuration_,
                              message_handler_impl_p->allocator_))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: failed to Stream_Module_MessageHandlerA_T::initialize(), aborting\n"),
                inherited::mod_->name ()));
    return false;
  } // end IF

  return true;
}
