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

#include "ace/INET_Addr.h"
#include "ace/Log_Msg.h"

#include "common_ilock.h"
#include "common_timer_manager_common.h"

#include "stream_macros.h"
#include "stream_session_message_base.h"

#include "net_common.h"
#include "net_common_tools.h"
#include "net_configuration.h"
#include "net_iconnector.h"

#include "net_client_defines.h"

#include "stream_module_net_common.h"

//////////////////////////////////////////

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename ConfigurationType,
          typename ControlMessageType,
          typename ConnectionManagerType>
Stream_Module_Net_Source_Reader_T<ACE_SYNCH_USE,
                                  TimePolicyType,
                                  ConfigurationType,
                                  ControlMessageType,
                                  ConnectionManagerType>::Stream_Module_Net_Source_Reader_T ()
 : inherited (NULL)
{
  STREAM_TRACE (ACE_TEXT ("Stream_Module_Net_Source_Reader_T::Stream_Module_Net_Source_Reader_T"));

}

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename ConfigurationType,
          typename ControlMessageType,
          typename ConnectionManagerType>
Stream_Module_Net_Source_Reader_T<ACE_SYNCH_USE,
                                  TimePolicyType,
                                  ConfigurationType,
                                  ControlMessageType,
                                  ConnectionManagerType>::~Stream_Module_Net_Source_Reader_T ()
{
  STREAM_TRACE (ACE_TEXT ("Stream_Module_Net_Source_Reader_T::~Stream_Module_Net_Source_Reader_T"));

}

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename ConfigurationType,
          typename ControlMessageType,
          typename ConnectionManagerType>
void
Stream_Module_Net_Source_Reader_T<ACE_SYNCH_USE,
                                  TimePolicyType,
                                  ConfigurationType,
                                  ControlMessageType,
                                  ConnectionManagerType>::handleControlMessage (ACE_Message_Block& messageBlock_in)
{
  STREAM_TRACE (ACE_TEXT ("Stream_Module_Net_Source_Reader_T::handleControlMessage"));

  switch (messageBlock_in.msg_type ())
  {
    case STREAM_CONTROL_MESSAGE_DISCONNECT:
    {
      // *NOTE*: there is no need to do anything, the control is handled by the
      //         upstream Stream_Module_Net_IOReader_T task
      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("%s: unknown/invalid control message type (was: %d), returning\n"),
                  inherited::mod_->name (),
                  messageBlock_in.msg_type ()));
      return;
    }
  } // end SWITCH
}

//////////////////////////////////////////

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename ConfigurationType,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType,
          typename ConnectionConfigurationIteratorType,
          typename ConnectionManagerType,
          typename ConnectorType>
Stream_Module_Net_Source_Writer_T<ACE_SYNCH_USE,
                                  TimePolicyType,
                                  ConfigurationType,
                                  ControlMessageType,
                                  DataMessageType,
                                  SessionMessageType,
                                  ConnectionConfigurationIteratorType,
                                  ConnectionManagerType,
#if defined (ACE_WIN32) || defined (ACE_WIN64)
                                  ConnectorType>::Stream_Module_Net_Source_Writer_T (ISTREAM_T* stream_in)
#else
                                  ConnectorType>::Stream_Module_Net_Source_Writer_T (typename inherited::ISTREAM_T* stream_in)
#endif
 : inherited (stream_in)
 , address_ ()
 , connector_ (NULL,
               ACE_Time_Value::zero)
 , connection_ (NULL)
 , isOpen_ (false)
 , isPassive_ (false)
 , unlink_ (false)
{
  STREAM_TRACE (ACE_TEXT ("Stream_Module_Net_Source_Writer_T::Stream_Module_Net_Source_Writer_T"));

}

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename ConfigurationType,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType,
          typename ConnectionConfigurationIteratorType,
          typename ConnectionManagerType,
          typename ConnectorType>
Stream_Module_Net_Source_Writer_T<ACE_SYNCH_USE,
                                  TimePolicyType,
                                  ConfigurationType,
                                  ControlMessageType,
                                  DataMessageType,
                                  SessionMessageType,
                                  ConnectionConfigurationIteratorType,
                                  ConnectionManagerType,
                                  ConnectorType>::~Stream_Module_Net_Source_Writer_T ()
{
  STREAM_TRACE (ACE_TEXT ("Stream_Module_Net_Source_Writer_T::~Stream_Module_Net_Source_Writer_T"));

  if (connection_)
  {
    if (unlink_)
    {
      typename ConnectorType::ISTREAM_CONNECTION_T* istream_connection_p =
        dynamic_cast<typename ConnectorType::ISTREAM_CONNECTION_T*> (connection_);
      if (!istream_connection_p)
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to dynamic_cast<Net_IStreamConnection_T*>(%@): \"%m\", returning\n"),
                    connection_));
        return;
      } // end IF
      typename inherited::ISTREAM_T* istream_p =
        &const_cast<typename ConnectorType::STREAM_T&> (istream_connection_p->stream ());
      istream_p->_unlink ();
      ACE_DEBUG ((LM_WARNING,
                  ACE_TEXT ("%s: unlinked i/o stream(s) in dtor --> check implementation !\n"),
                  inherited::mod_->name ()));
    } // end IF

    if (!isPassive_ &&
        isOpen_)
    {
      connection_->close ();
      ACE_DEBUG ((LM_WARNING,
                  ACE_TEXT ("%s: closed connection to \"%s\" in dtor --> check implementation !\n"),
                  inherited::mod_->name (),
                  ACE_TEXT (Net_Common_Tools::IPAddressToString (address_).c_str ())));
    } // end IF

    connection_->decrease ();
  } // end IF
}

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename ConfigurationType,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType,
          typename ConnectionConfigurationIteratorType,
          typename ConnectionManagerType,
          typename ConnectorType>
bool
Stream_Module_Net_Source_Writer_T<ACE_SYNCH_USE,
                                  TimePolicyType,
                                  ConfigurationType,
                                  ControlMessageType,
                                  DataMessageType,
                                  SessionMessageType,
                                  ConnectionConfigurationIteratorType,
                                  ConnectionManagerType,
                                  ConnectorType>::initialize (const ConfigurationType& configuration_in,
                                                              Stream_IAllocator* allocator_in)
{
  STREAM_TRACE (ACE_TEXT ("Stream_Module_Net_Source_Writer_T::initialize"));

  if (inherited::isInitialized_)
  {
    if (unlink_)
    { ACE_ASSERT (connection_);
      typename ConnectorType::ISTREAM_CONNECTION_T* stream_connection_p =
        dynamic_cast<typename ConnectorType::ISTREAM_CONNECTION_T*> (connection_);
      if (!stream_connection_p)
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("%s: failed to dynamic_cast<Net_IStreamConnection_T> (%@): \"%m\", aborting\n"),
                    inherited::mod_->name (),
                    connection_));
        return false;
      } // end IF
      typename inherited::ISTREAM_T* istream_p =
        &const_cast<typename ConnectorType::STREAM_T&> (stream_connection_p->stream ());
      istream_p->_unlink ();

      unlink_ = false;
    } // end IF

    if (!isPassive_ &&
        isOpen_)
    { ACE_ASSERT (connection_);
      ACE_HANDLE handle = ACE_INVALID_HANDLE;
      typename ConnectorType::ADDRESS_T local_address, peer_address;
      connection_->info (handle,
                         local_address, peer_address);

      Net_ConnectionId_t id = connection_->id ();
      connection_->close ();
      isOpen_ = false;
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("%s: closed connection to %s (id was: %u)\n"),
                  inherited::mod_->name (),
                  ACE_TEXT (Net_Common_Tools::IPAddressToString (peer_address).c_str ()),
                  id));
    } // end IF
    isOpen_ = false;

    if (connection_)
    {
      connection_->decrease ();
      connection_ = NULL;
    } // end IF
  } // end IF

  // *TODO*: remove type inferences
  if (configuration_in.connection &&
      configuration_in.passive)
  {
    configuration_in.connection->increase ();
    connection_ = configuration_in.connection;
    isPassive_ = true;
  } // end IF
  else
    isPassive_ = false;

  // sanity check(s)
  ACE_ASSERT (configuration_in.connectionConfigurations);
  ACE_ASSERT (!configuration_in.connectionConfigurations->empty ());

  ConnectionConfigurationIteratorType iterator =
    inherited::configuration_->connectionConfigurations->find (inherited::mod_->name ());
  if (iterator == inherited::configuration_->connectionConfigurations->end ())
    iterator =
      inherited::configuration_->connectionConfigurations->find (ACE_TEXT_ALWAYS_CHAR (""));
  //else
  //  ACE_DEBUG ((LM_ERROR,
  //              ACE_TEXT ("%s: applying connection configuration\n"),
  //              inherited::mod_->name ()));
  ACE_ASSERT (iterator != inherited::configuration_->connectionConfigurations->end ());
  ACE_ASSERT ((*iterator).second.socketHandlerConfiguration.socketConfiguration);
  switch (connector_.transportLayer ())
  {
    case NET_TRANSPORTLAYER_TCP:
    {
      struct Net_TCPSocketConfiguration* socket_configuration_p =
          dynamic_cast<struct Net_TCPSocketConfiguration*> ((*iterator).second.socketHandlerConfiguration.socketConfiguration);
      ACE_ASSERT (socket_configuration_p);
      address_ = socket_configuration_p->address;
      break;
    }
    case NET_TRANSPORTLAYER_UDP:
    {
      struct Net_UDPSocketConfiguration* socket_configuration_p =
          dynamic_cast<struct Net_UDPSocketConfiguration*> ((*iterator).second.socketHandlerConfiguration.socketConfiguration);
      ACE_ASSERT (socket_configuration_p);
      address_ = socket_configuration_p->address;
      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("%s: invalid/unknown transport layer type (was: %d), aborting\n"),
                  inherited::mod_->name (),
                  connector_.transportLayer ()));
      break;
    }
  } // end SWITCH

  return inherited::initialize (configuration_in,
                                allocator_in);
}

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename ConfigurationType,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType,
          typename ConnectionConfigurationIteratorType,
          typename ConnectionManagerType,
          typename ConnectorType>
void
Stream_Module_Net_Source_Writer_T<ACE_SYNCH_USE,
                                  TimePolicyType,
                                  ConfigurationType,
                                  ControlMessageType,
                                  DataMessageType,
                                  SessionMessageType,
                                  ConnectionConfigurationIteratorType,
                                  ConnectionManagerType,
                                  ConnectorType>::handleSessionMessage (SessionMessageType*& message_inout,
                                                                        bool& passMessageDownstream_out)
{
  STREAM_TRACE (ACE_TEXT ("Stream_Module_Net_Source_Writer_T::handleSessionMessage"));

  // don't care (implies yes per default, if part of a stream)
  ACE_UNUSED_ARG (passMessageDownstream_out);

  // sanity check(s)
  ACE_ASSERT (inherited::configuration_);

  switch (message_inout->type ())
  {
    case STREAM_SESSION_MESSAGE_ABORT:
    {
      // sanity check(s)
      ACE_ASSERT (inherited::sessionData_);

      typename SessionMessageType::DATA_T::DATA_T& session_data_r =
          const_cast<typename SessionMessageType::DATA_T::DATA_T&> (inherited::sessionData_->get ());

      if (isOpen_ &&
          !isPassive_)
      { ACE_ASSERT (connection_);
        connection_->close ();
        ACE_DEBUG ((LM_DEBUG,
                    ACE_TEXT ("%s: session (id was: %u) aborted, closed connection to %s\n"),
                    inherited::mod_->name (),
                    session_data_r.sessionID,
                    ACE_TEXT (Net_Common_Tools::IPAddressToString (address_).c_str ())));
      } // end IF
      isOpen_ = false;

      break;
    }
    case STREAM_SESSION_MESSAGE_BEGIN:
    {
      // sanity check(s)
      ACE_ASSERT (inherited::sessionData_);
      ACE_ASSERT (!isOpen_);

      typename SessionMessageType::DATA_T::DATA_T& session_data_r =
          const_cast<typename SessionMessageType::DATA_T::DATA_T&> (inherited::sessionData_->get ());

      // *TODO*: remove type inferences
      typename ConnectorType::ICONNECTOR_T* iconnector_p = &connector_;
      typename ConnectionManagerType::INTERFACE_T* iconnection_manager_p =
        (inherited::configuration_->connectionManager ? inherited::configuration_->connectionManager
                                                      : NULL);
      typename ConnectorType::ISTREAM_CONNECTION_T* istream_connection_p = NULL;
      typename ConnectorType::STREAM_T* stream_p = NULL;
      typename ConnectorType::STREAM_T::MODULE_T* module_p = NULL;
      typename SessionMessageType::DATA_T* session_data_container_p = NULL;
      ACE_HANDLE handle = ACE_INVALID_HANDLE;
      bool clone_module, delete_module;
      bool notify_connect = false;
      ConnectionConfigurationIteratorType iterator;
      // *NOTE*: (currently,) this could be a TCP (--> test peer address),
      //         or a UDP (--> test local address) connection
//      bool is_peer_address =
//          (iconnector_p->transportLayer () == NET_TRANSPORTLAYER_TCP);

      if (isPassive_)
      {
        // sanity check(s)
        if (connection_)
          goto continue_; // --> using configured connection

        // --> using session connection

        // sanity check(s)
        ACE_ASSERT (iconnection_manager_p);
#if defined (ACE_WIN32) || defined (ACE_WIN64)
        ACE_ASSERT (reinterpret_cast<ACE_HANDLE> (session_data_r.sessionID) != ACE_INVALID_HANDLE);
#else
        ACE_ASSERT (static_cast<ACE_HANDLE> (session_data_r.sessionID) != ACE_INVALID_HANDLE);
#endif

        connection_ =
          iconnection_manager_p->get (static_cast<Net_ConnectionId_t> (session_data_r.sessionID));
        if (!connection_)
        {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("%s: failed to retrieve connection (id was: %u), aborting\n"),
                      inherited::mod_->name (),
                      session_data_r.sessionID));
          goto error;
        } // end IF

        goto continue_;
      } // end IF

      // --> open new connection

      //// step1: initialize connection manager
      //    typename ConnectionManagerType::CONFIGURATION_T original_configuration;
      //    typename SessionMessageType::USER_DATA_T* user_data_p = NULL;
      //    configuration_in.connectionManager->get (original_configuration,
      //                                             user_data_p);
      //    typename ConnectionManagerType::CONFIGURATION_T configuration =
      //        original_configuration;
      //    configuration.streamConfiguration.cloneModule = false;
      //    configuration.streamConfiguration.deleteModule = false;
      //    configuration.streamConfiguration.module = NULL;
      //    iconnection_manager_p->set (configuration,
      //                                user_data_p);

      // step2: initialize connector
      // sanity check(s)
      // *TODO*: remove type inference
      ACE_ASSERT (inherited::configuration_->connectionConfigurations);
      ACE_ASSERT (inherited::configuration_->streamConfiguration);

      iterator =
        inherited::configuration_->connectionConfigurations->find (inherited::mod_->name ());
      if (iterator == inherited::configuration_->connectionConfigurations->end ())
        iterator =
          inherited::configuration_->connectionConfigurations->find (ACE_TEXT_ALWAYS_CHAR (""));
      //else
      //  ACE_DEBUG ((LM_ERROR,
      //              ACE_TEXT ("%s: applying connection configuration\n"),
      //              inherited::mod_->name ()));
      ACE_ASSERT (iterator != inherited::configuration_->connectionConfigurations->end ());

      clone_module =
        inherited::configuration_->streamConfiguration->configuration_.cloneModule;
      delete_module =
        inherited::configuration_->streamConfiguration->configuration_.deleteModule;
      module_p =
        inherited::configuration_->streamConfiguration->configuration_.module;
      inherited::configuration_->streamConfiguration->configuration_.cloneModule =
          false;
      inherited::configuration_->streamConfiguration->configuration_.deleteModule =
          false;
      inherited::configuration_->streamConfiguration->configuration_.module =
          NULL;

      if (!iconnector_p->initialize ((*iterator).second))
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("%s: failed to ConnectorType::INTERFACE_T::initialize(): \"%m\", aborting\n"),
                    inherited::mod_->name ()));
        goto reset;
      } // end IF

      // step3: connect
      ACE_ASSERT (!connection_);
      handle = iconnector_p->connect (address_);
      if (iconnector_p->useReactor ())
      {
        if (handle != ACE_INVALID_HANDLE)
#if defined (ACE_WIN32) || defined (ACE_WIN64)
          connection_ =
            iconnection_manager_p->get (reinterpret_cast<Net_ConnectionId_t> (handle));
#else
          connection_ =
            iconnection_manager_p->get (static_cast<Net_ConnectionId_t> (handle));
#endif
      } // end IF
      else
      {
        // step1: wait for the connection to register with the manager
        // *TODO*: avoid these tight loops
        ACE_Time_Value timeout (NET_CLIENT_DEFAULT_ASYNCH_CONNECT_TIMEOUT, 0);
        ACE_Time_Value deadline = COMMON_TIME_NOW + timeout;
        // *TODO*: this may not be accurate/applicable for/to all protocols
        bool is_peer_address =
          (iconnector_p->transportLayer () == NET_TRANSPORTLAYER_TCP);
        enum Net_Connection_Status status = NET_CONNECTION_STATUS_INVALID;
        do
        {
          // *TODO*: avoid these tight loops
          connection_ = iconnection_manager_p->get (address_,
                                                    is_peer_address);
          if (!connection_)
            continue;
          istream_connection_p =
              dynamic_cast<typename ConnectorType::ISTREAM_CONNECTION_T*> (connection_);
          if (!istream_connection_p)
          {
            ACE_DEBUG ((LM_ERROR,
                        ACE_TEXT ("%s: failed to dynamic_cast<Net_IStreamConnection_T>(0x%@), aborting\n"),
                        inherited::mod_->name (),
                        connection_));
            goto reset;
          } // end IF

          // step2: wait for the connection to finish initializing
          do
          {
            status = connection_->status ();
            if (status == NET_CONNECTION_STATUS_OK)
              break;
          } while (COMMON_TIME_NOW < deadline);
          if (status != NET_CONNECTION_STATUS_OK)
            break;

          // step3: wait for the connection stream to finish initializing
          istream_connection_p->wait (STREAM_STATE_RUNNING,
                                      NULL); // <-- block
          break;
        } while (COMMON_TIME_NOW < deadline);
      } // end ELSE
      if (!connection_)
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("%s: failed to connect to %s, aborting\n"),
                    inherited::mod_->name (),
                    ACE_TEXT (Net_Common_Tools::IPAddressToString (address_).c_str ())));

        // clean up
        iconnector_p->abort ();

        goto reset;
      } // end IF
      isOpen_ = true;
      //ACE_DEBUG ((LM_DEBUG,
      //            ACE_TEXT ("%s: connected to %s (id: %u)\n"),
      //            inherited::mod_->name (),
      //            ACE_TEXT (Net_Common_Tools::IPAddressToString (address_).c_str ()),
      //            connection_->id ()));
      notify_connect = true;

reset:
      inherited::configuration_->streamConfiguration->configuration_.cloneModule =
          clone_module;
      inherited::configuration_->streamConfiguration->configuration_.deleteModule =
          delete_module;
      inherited::configuration_->streamConfiguration->configuration_.module =
          module_p;
      if (!connection_)
        goto error;

      // link processing streams
      istream_connection_p =
        dynamic_cast<typename ConnectorType::ISTREAM_CONNECTION_T*> (connection_);
      if (!istream_connection_p)
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("%s: failed to dynamic_cast<Net_IStreamConnection_T>(0x%@): \"%m\", aborting\n"),
                    inherited::mod_->name (),
                    connection_));
        goto error;
      } // end IF
      stream_p =
        &const_cast<typename ConnectorType::STREAM_T&> (istream_connection_p->stream ());
      ACE_ASSERT (inherited::stream_);
      if (!inherited::stream_->link (stream_p))
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("%s: failed to Stream_IStream::link(): \"%m\", aborting\n"),
                    inherited::mod_->name ()));
        goto error;
      } // end IF
      unlink_ = true;

      // update session data in the current session message
      // *WARNING*: this works iff (!) the STREAM_SESSION_LINK message has
      //            already been processed at this point (i.e. as long as
      //            upstream is completely synchronous)
      inherited::sessionData_->increase ();
      session_data_container_p = inherited::sessionData_;
      message_inout->initialize (STREAM_SESSION_MESSAGE_BEGIN,
                                 session_data_container_p,
                                 &const_cast<typename SessionMessageType::USER_DATA_T&> (message_inout->data ()));
      session_data_r =
        const_cast<typename SessionMessageType::DATA_T::DATA_T&> (inherited::sessionData_->get ());

      goto continue_;

error:
      if (unlink_)
      {
        typename ConnectorType::ISTREAM_CONNECTION_T* stream_connection_p =
          dynamic_cast<typename ConnectorType::ISTREAM_CONNECTION_T*> (connection_);
        if (!stream_connection_p)
        {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("%s: failed to dynamic_cast<Net_IStreamConnection_T>(0x%@): \"%m\", aborting\n"),
                      inherited::mod_->name (),
                      connection_));
          goto error_2;
        } // end IF
        typename inherited::ISTREAM_T* istream_p =
          &const_cast<typename ConnectorType::STREAM_T&> (stream_connection_p->stream ());
        istream_p->_unlink ();

        unlink_ = false;
      } // end IF

error_2:
      if (isOpen_ &&
          !isPassive_)
      { ACE_ASSERT (connection_);
        Net_ConnectionId_t id = connection_->id ();
        connection_->close ();
        ACE_DEBUG ((LM_DEBUG,
                    ACE_TEXT ("%s: closed connection to %s (id was: %u)\n"),
                    inherited::mod_->name (),
                    ACE_TEXT (Net_Common_Tools::IPAddressToString (address_).c_str ()),
                    id));
      } // end IF
      isOpen_ = false;

      if (connection_)
      {
        connection_->decrease ();
        connection_ = NULL;
      } // end IF

      inherited::notify (STREAM_SESSION_MESSAGE_ABORT);

      break;

continue_:
      // sanity check(s)
      ACE_ASSERT (connection_);

      // re-set session ID, retain state information
      // *TODO*: remove type inferences
      ACE_ASSERT (session_data_r.lock);
      { ACE_GUARD (ACE_SYNCH_MUTEX, aGuard, *session_data_r.lock);
        session_data_r.connectionState =
          &const_cast<typename ConnectionManagerType::STATE_T&> (connection_->state ());
        session_data_r.sessionID = connection_->id ();
      } // end lock scope
      //ACE_DEBUG ((LM_DEBUG,
      //            ACE_TEXT ("%s: reset session id to %u\n"),
      //            inherited::mod_->name (),
      //            session_data_r.sessionID));

      if (notify_connect)
        inherited::notify (STREAM_SESSION_MESSAGE_CONNECT);

      break;
    }
    case STREAM_SESSION_MESSAGE_END:
    {
      // sanity check(s)
      ACE_ASSERT (inherited::sessionData_);

      typename SessionMessageType::DATA_T::DATA_T& session_data_r =
          const_cast<typename SessionMessageType::DATA_T::DATA_T&> (inherited::sessionData_->get ());

      // *NOTE*: control reaches this point because either:
      //         - the connection has been closed and the processing stream is
      //           finished()-ing (see: ACE_Svc_Handler::handle_close()
      //           derivates)
      //           --> wait for upstream data processing to complete before
      //               unlinking
      //         - the session is being aborted by the user/some module
      //           --> do NOT wait for upstream data processing to complete
      //               before unlinking, i.e. flush the connection stream
      // *TODO*: never flush any server-side inbound data

      if (connection_)
      {
        typename ConnectorType::STREAM_T* stream_p = NULL;
        typename ConnectorType::ISTREAM_CONNECTION_T* istream_connection_p =
          dynamic_cast<typename ConnectorType::ISTREAM_CONNECTION_T*> (connection_);
        if (!istream_connection_p)
        {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("%s: failed to dynamic_cast<Net_IStreamConnection_T>(0x%@): \"%m\", returning\n"),
                      inherited::mod_->name (),
                      connection_));
          goto flush;
        } // end IF
        stream_p =
          &const_cast<typename ConnectorType::STREAM_T&> (istream_connection_p->stream ());

        // wait for upstream data processing to complete before unlinking ?
        { ACE_GUARD (ACE_SYNCH_MUTEX, aGuard, *session_data_r.lock);
          if (session_data_r.aborted)
            goto flush;
        } // end lock scope

        // *NOTE*: finalize the (connection) stream state so waitForCompletion()
        //         does not block forever
        // *NOTE*: stop()ping the connection stream will also unlink it
        //         --> send the disconnect notification early
        inherited::notify (STREAM_SESSION_MESSAGE_DISCONNECT);
        // *TODO*: this shouldn't be necessary (--> only wait for data to flush)
        stream_p->stop (false, // wait for completion ?
                        false, // recurse upstream ?
                        true); // locked access ?
        connection_->waitForCompletion (false); // --> data only

        goto continue_2;

flush:
        stream_p->flush (true,   // flush inbound data ?
                         false,  // flush session messages ?
                         false); // flush upstream (if any) ?
      } // end IF

continue_2:
      if (unlink_)
      { ACE_ASSERT (inherited::stream_);
        inherited::stream_->_unlink ();

        unlink_ = false;
      } // end IF

      if (isOpen_ &&
          !isPassive_)
      { ACE_ASSERT (connection_);
        Net_ConnectionId_t id = connection_->id ();
        connection_->close ();
        ACE_DEBUG ((LM_DEBUG,
                    ACE_TEXT ("%s: closed connection to %s (id was: %u)\n"),
                    inherited::mod_->name (),
                    ACE_TEXT (Net_Common_Tools::IPAddressToString (address_).c_str ()),
                    id));
      } // end IF
      isOpen_ = false;

      if (connection_)
      {
        connection_->decrease ();
        connection_ = NULL;
      } // end IF

      break;
    }
    case STREAM_SESSION_MESSAGE_DISCONNECT:
    {
      if (isOpen_)
      {
        // *TODO*: the stream can control several connections, so this may be
        //         wrong...
        //isOpen_ = false;

        //ACE_DEBUG ((LM_DEBUG,
        //            ACE_TEXT ("%s: disconnected\n"),
        //            inherited::mod_->name ()));
      } // end IF

      break;
    }
    case STREAM_SESSION_MESSAGE_LINK:
    {
      //ACE_DEBUG ((LM_DEBUG,
      //            ACE_TEXT ("%s: linked i/o stream(s)\n"),
      //            inherited::mod_->name ()));
#if defined (_DEBUG)
      ACE_ASSERT (inherited::stream_);
      inherited::stream_->dump_state ();
#endif
      break;
    }
    case STREAM_SESSION_MESSAGE_UNLINK:
    {
      // sanity check(s)
      if (!inherited::linked_)
      { // *TODO*: clean this up
        break;
      } // end IF

      // *NOTE*: most probable reason: the stream has been stop()ped
      // *TODO*: the stream can have several substreams that are (un-)linked
      //         dynamically, so this may be wrong...
      unlink_ = false;

      //ACE_DEBUG ((LM_DEBUG,
      //            ACE_TEXT ("%s: unlinked i/o stream(s)\n"),
      //            inherited::mod_->name ()));
#if defined (_DEBUG)
      ACE_ASSERT (inherited::stream_);
      inherited::stream_->dump_state ();
#endif
      break;
    }
    default:
      break;
  } // end SWITCH
}

//////////////////////////////////////////

template <ACE_SYNCH_DECL,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType,
          typename ConfigurationType,
          typename StreamControlType,
          typename StreamNotificationType,
          typename StreamStateType,
          typename SessionDataType,
          typename SessionDataContainerType,
          typename StatisticContainerType,
          typename StatisticHandlerType,
          typename ConnectionConfigurationIteratorType,
          typename ConnectionManagerType,
          typename ConnectorType,
          typename UserDataType>
Stream_Module_Net_SourceH_T<ACE_SYNCH_USE,
                            ControlMessageType,
                            DataMessageType,
                            SessionMessageType,
                            ConfigurationType,
                            StreamControlType,
                            StreamNotificationType,
                            StreamStateType,
                            SessionDataType,
                            SessionDataContainerType,
                            StatisticContainerType,
                            StatisticHandlerType,
                            ConnectionConfigurationIteratorType,
                            ConnectionManagerType,
                            ConnectorType,
#if defined (ACE_WIN32) || defined (ACE_WIN64)
                            UserDataType>::Stream_Module_Net_SourceH_T (ISTREAM_T* stream_in,
#else
                            UserDataType>::Stream_Module_Net_SourceH_T (typename inherited::ISTREAM_T* stream_in,
#endif
                                                                        bool generateSessionMessages_in,
                                                                        ConnectionManagerType* connectionManager_in,
                                                                        bool isPassive_in)
 : inherited (stream_in,
              false,
              STREAM_HEADMODULECONCURRENCY_CONCURRENT,
              generateSessionMessages_in)
 , address_ ()
 , connector_ (connectionManager_in,
               ACE_Time_Value::zero)
 , connection_ (NULL)
 , isOpen_ (false)
 , isPassive_ (isPassive_in)
 , unlink_ (false)
{
  STREAM_TRACE (ACE_TEXT ("Stream_Module_Net_SourceH_T::Stream_Module_Net_SourceH_T"));

}

template <ACE_SYNCH_DECL,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType,
          typename ConfigurationType,
          typename StreamControlType,
          typename StreamNotificationType,
          typename StreamStateType,
          typename SessionDataType,
          typename SessionDataContainerType,
          typename StatisticContainerType,
          typename StatisticHandlerType,
          typename ConnectionConfigurationIteratorType,
          typename ConnectionManagerType,
          typename ConnectorType,
          typename UserDataType>
Stream_Module_Net_SourceH_T<ACE_SYNCH_USE,
                            ControlMessageType,
                            DataMessageType,
                            SessionMessageType,
                            ConfigurationType,
                            StreamControlType,
                            StreamNotificationType,
                            StreamStateType,
                            SessionDataType,
                            SessionDataContainerType,
                            StatisticContainerType,
                            StatisticHandlerType,
                            ConnectionConfigurationIteratorType,
                            ConnectionManagerType,
                            ConnectorType,
                            UserDataType>::~Stream_Module_Net_SourceH_T ()
{
  STREAM_TRACE (ACE_TEXT ("Stream_Module_Net_SourceH_T::~Stream_Module_Net_SourceH_T"));

  if (connection_)
  {
    if (unlink_)
    {
      typename ConnectorType::ISTREAM_CONNECTION_T* istream_connection_p =
        dynamic_cast<typename ConnectorType::ISTREAM_CONNECTION_T*> (connection_);
      if (!istream_connection_p)
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("%s: failed to dynamic_cast<Net_IStreamConnection_T> (%@): \"%m\", returning\n"),
                    inherited::mod_->name (),
                    connection_));
        return;
      } // end IF
      typename inherited::ISTREAM_T* istream_p =
        &const_cast<typename ConnectorType::STREAM_T&> (istream_connection_p->stream ());
      istream_p->_unlink ();
      ACE_DEBUG ((LM_WARNING,
                  ACE_TEXT ("%s: unlinked i/o stream(s) in dtor --> check implementation !\n"),
                  inherited::mod_->name ()));
    } // end IF

    if (isOpen_ &&
        !isPassive_)
    {
      connection_->close ();
      ACE_DEBUG ((LM_WARNING,
                  ACE_TEXT ("%s: closed connection to \"%s\" in dtor --> check implementation !\n"),
                  inherited::mod_->name (),
                  ACE_TEXT (Net_Common_Tools::IPAddressToString (address_).c_str ())));
    } // end IF

    connection_->decrease ();
  } // end IF
}

template <ACE_SYNCH_DECL,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType,
          typename ConfigurationType,
          typename StreamControlType,
          typename StreamNotificationType,
          typename StreamStateType,
          typename SessionDataType,
          typename SessionDataContainerType,
          typename StatisticContainerType,
          typename StatisticHandlerType,
          typename ConnectionConfigurationIteratorType,
          typename ConnectionManagerType,
          typename ConnectorType,
          typename UserDataType>
bool
Stream_Module_Net_SourceH_T<ACE_SYNCH_USE,
                            ControlMessageType,
                            DataMessageType,
                            SessionMessageType,
                            ConfigurationType,
                            StreamControlType,
                            StreamNotificationType,
                            StreamStateType,
                            SessionDataType,
                            SessionDataContainerType,
                            StatisticContainerType,
                            StatisticHandlerType,
                            ConnectionConfigurationIteratorType,
                            ConnectionManagerType,
                            ConnectorType,
                            UserDataType>::initialize (const ConfigurationType& configuration_in,
                                                       Stream_IAllocator* allocator_in)
{
  STREAM_TRACE (ACE_TEXT ("Stream_Module_Net_SourceH_T::initialize"));

  if (inherited::isInitialized_)
  {
    if (unlink_)
    { ACE_ASSERT (connection_);
      typename ConnectorType::ISTREAM_CONNECTION_T* istream_connection_p =
        dynamic_cast<typename ConnectorType::ISTREAM_CONNECTION_T*> (connection_);
      if (!istream_connection_p)
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("%s: failed to dynamic_cast<Net_IStreamConnection_T>(0x%@): \"%m\", aborting\n"),
                    inherited::mod_->name (),
                    connection_));
        return false;
      } // end IF
      typename inherited::ISTREAM_T* istream_p =
        &const_cast<typename ConnectorType::STREAM_T&> (istream_connection_p->stream ());
      istream_p->_unlink ();

      unlink_ = false;
    } // end IF

    if (isOpen_ &&
        !isPassive_)
    { ACE_ASSERT (connection_);
      Net_ConnectionId_t id = connection_->id ();
      connection_->close ();
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("%s: closed connection to %s (id was: %u)\n"),
                  inherited::mod_->name (),
                  ACE_TEXT (Net_Common_Tools::IPAddressToString (address_).c_str ()),
                  id));
    } // end IF
    isOpen_ = false;

    if (connection_)
    {
      connection_->decrease ();
      connection_ = NULL;
    } // end IF
  } // end IF

  if (configuration_in.connection &&
      configuration_in.passive)
  {
    configuration_in.connection->increase ();
    connection_ = configuration_in.connection;

    isPassive_ = true;
  } // end IF
  else
    isPassive_ = false;

  // sanity check(s)
  // *TODO*: remove type inferences
  ACE_ASSERT (configuration_in.connectionConfigurations);
  ConnectionConfigurationIteratorType iterator =
    configuration_in.connectionConfigurations->find (ACE_TEXT_ALWAYS_CHAR (inherited::mod_->name ()));
  if (iterator == configuration_in.connectionConfigurations->end ())
    iterator =
      configuration_in.connectionConfigurations->find (ACE_TEXT_ALWAYS_CHAR (""));
  //else
  //  ACE_DEBUG ((LM_DEBUG,
  //              ACE_TEXT ("%s: applying connection configuration\n"),
  //              inherited::mod_->name ()));
  ACE_ASSERT (iterator != configuration_in.connectionConfigurations->end ());
  ACE_ASSERT ((*iterator).second.socketHandlerConfiguration.socketConfiguration);
  switch (connector_.transportLayer ())
  {
    case NET_TRANSPORTLAYER_TCP:
    {
      struct Net_TCPSocketConfiguration* socket_configuration_p =
          dynamic_cast<struct Net_TCPSocketConfiguration*> ((*iterator).second.socketHandlerConfiguration.socketConfiguration);
      ACE_ASSERT (socket_configuration_p);
      address_ = socket_configuration_p->address;
      break;
    }
    case NET_TRANSPORTLAYER_UDP:
    {
      struct Net_UDPSocketConfiguration* socket_configuration_p =
          dynamic_cast<struct Net_UDPSocketConfiguration*> ((*iterator).second.socketHandlerConfiguration.socketConfiguration);
      ACE_ASSERT (socket_configuration_p);
      address_ = socket_configuration_p->address;
      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("%s: invalid/unknown transport layer type (was: %d), aborting\n"),
                  inherited::mod_->name (),
                  connector_.transportLayer ()));
      break;
    }
  } // end SWITCH

  return inherited::initialize (configuration_in,
                                allocator_in);
}

template <ACE_SYNCH_DECL,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType,
          typename ConfigurationType,
          typename StreamControlType,
          typename StreamNotificationType,
          typename StreamStateType,
          typename SessionDataType,
          typename SessionDataContainerType,
          typename StatisticContainerType,
          typename StatisticHandlerType,
          typename ConnectionConfigurationIteratorType,
          typename ConnectionManagerType,
          typename ConnectorType,
          typename UserDataType>
void
Stream_Module_Net_SourceH_T<ACE_SYNCH_USE,
                            ControlMessageType,
                            DataMessageType,
                            SessionMessageType,
                            ConfigurationType,
                            StreamControlType,
                            StreamNotificationType,
                            StreamStateType,
                            SessionDataType,
                            SessionDataContainerType,
                            StatisticContainerType,
                            StatisticHandlerType,
                            ConnectionConfigurationIteratorType,
                            ConnectionManagerType,
                            ConnectorType,
                            UserDataType>::handleSessionMessage (SessionMessageType*& message_inout,
                                                                 bool& passMessageDownstream_out)
{
  STREAM_TRACE (ACE_TEXT ("Stream_Module_Net_SourceH_T::handleSessionMessage"));

  int result = -1;

  // don't care (implies yes per default, if part of a stream)
  ACE_UNUSED_ARG (passMessageDownstream_out);

  // sanity check(s)
  ACE_ASSERT (inherited::configuration_);
  // *TODO*: remove type inference
  ACE_ASSERT (inherited::configuration_->streamConfiguration);

  switch (message_inout->type ())
  {
    case STREAM_SESSION_MESSAGE_ABORT:
    {
      // sanity check(s)
      ACE_ASSERT (inherited::sessionData_);

      typename SessionMessageType::DATA_T::DATA_T& session_data_r =
          const_cast<typename SessionMessageType::DATA_T::DATA_T&> (inherited::sessionData_->get ());

      if (isOpen_ &&
          !isPassive_)
      { ACE_ASSERT (connection_);

        connection_->close ();
        ACE_DEBUG ((LM_DEBUG,
                    ACE_TEXT ("%s: session (id was: %u) aborted, closed connection to %s\n"),
                    inherited::mod_->name (),
                    session_data_r.sessionID,
                    ACE_TEXT (Net_Common_Tools::IPAddressToString (address_).c_str ())));
      } // end IF
      isOpen_ = false;

      break;
    }
    case STREAM_SESSION_MESSAGE_BEGIN:
    {
      // *NOTE*: the connection processing stream is link()ed after the
      //         connection has been established (and its' stream becomes part
      //         of upstream). Ignore the session begin message that appears
      //         when this happens
      if (connection_)
        break;

      // sanity check(s)
      ACE_ASSERT (inherited::sessionData_);

      typename SessionMessageType::DATA_T::DATA_T& session_data_r =
          const_cast<typename SessionMessageType::DATA_T::DATA_T&> (inherited::sessionData_->get ());
      typename ConnectorType::ICONNECTOR_T* iconnector_p = &connector_;

      // schedule regular statistic collection ?
      if (inherited::configuration_->statisticReportingInterval !=
          ACE_Time_Value::zero)
      {
        ACE_Time_Value interval (STREAM_DEFAULT_STATISTIC_COLLECTION_INTERVAL,
                                 0);
        ACE_ASSERT (inherited::timerID_ == -1);
        typename StatisticHandlerType::HANDLER_T* handler_p =
          &(inherited::statisticCollectionHandler_);
        inherited::timerID_ =
            COMMON_TIMERMANAGER_SINGLETON::instance ()->schedule_timer (handler_p,                  // event handler
                                                                        NULL,                       // argument
                                                                        COMMON_TIME_NOW + interval, // first wakeup time
                                                                        interval);                  // interval
        if (inherited::timerID_ == -1)
        {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("%s: failed to Common_Timer_Manager::schedule_timer(): \"%m\", returning\n"),
                      inherited::mod_->name ()));
          return;
        } // end IF
//        ACE_DEBUG ((LM_DEBUG,
//                    ACE_TEXT ("%s: scheduled statistic collecting timer (ID: %d) for interval %#T\n"),
//                    inherited::mod_->name (),
//                    inherited::timerID_,
//                    &interval));
      } // end IF

      ACE_HANDLE handle = ACE_INVALID_HANDLE;
      // *TODO*: remove type inferences
      typename ConnectionManagerType::INTERFACE_T* iconnection_manager_p =
        inherited::configuration_->connectionManager;
      typename ConnectorType::ISTREAM_CONNECTION_T* istream_connection_p = NULL;
      typename ConnectorType::STREAM_T* stream_p = NULL;
      typename ConnectorType::STREAM_T::MODULE_T* module_p = NULL;
      SessionDataContainerType* session_data_container_p = NULL;
      bool notify_connect = false;
      bool clone_module, delete_module;
      ConnectionConfigurationIteratorType iterator_2;
      // *NOTE*: (currently,) this could be a TCP (--> test peer address),
      //         or a UDP (--> test local address) connection
      bool is_peer_address =
          (iconnector_p->transportLayer () == NET_TRANSPORTLAYER_TCP);

      if (isPassive_)
      {
        // sanity check(s)
        if (connection_)
          goto continue_; // --> using configured connection

        // --> using session connection

        // sanity check(s)
        ACE_ASSERT (iconnection_manager_p);

        // *TODO*: remove type inference
#if defined (ACE_WIN32) || defined (ACE_WIN64)
        ACE_ASSERT (reinterpret_cast<ACE_HANDLE> (session_data_r.sessionID) != ACE_INVALID_HANDLE);
#else
        ACE_ASSERT (static_cast<ACE_HANDLE> (session_data_r.sessionID) != ACE_INVALID_HANDLE);
#endif
        connection_ =
          iconnection_manager_p->get (static_cast<Net_ConnectionId_t> (session_data_r.sessionID),
                                      is_peer_address);
        if (!connection_)
        {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("%s: failed to retrieve connection (id was: %u), returning\n"),
                      inherited::mod_->name (),
                      session_data_r.sessionID));
//          return;
        } // end IF

        goto continue_;
      } // end IF

      // --> open new connection

      //// step1: initialize connection manager
      //    typename ConnectionManagerType::CONFIGURATION_T original_configuration;
      //    typename SessionMessageType::USER_DATA_T* user_data_p = NULL;
      //    configuration_in.connectionManager->get (original_configuration,
      //                                             user_data_p);
      //    typename ConnectionManagerType::CONFIGURATION_T configuration =
      //        original_configuration;
      //    configuration.streamConfiguration.cloneModule = false;
      //    configuration.streamConfiguration.deleteModule = false;
      //    configuration.streamConfiguration.module = NULL;
      //    iconnection_manager_p->set (configuration,
      //                                user_data_p);

      // step2: initialize connector
      // sanity check(s)
      ACE_ASSERT (inherited::configuration_->connectionConfigurations);
      ACE_ASSERT (inherited::configuration_->streamConfiguration);
      ACE_ASSERT (iconnection_manager_p);

      iterator_2 =
        inherited::configuration_->connectionConfigurations->find (ACE_TEXT_ALWAYS_CHAR (inherited::mod_->name ()));
      if (iterator_2 == inherited::configuration_->connectionConfigurations->end ())
        iterator_2 =
          inherited::configuration_->connectionConfigurations->find (ACE_TEXT_ALWAYS_CHAR (""));
      //else
      //  ACE_DEBUG ((LM_DEBUG,
      //              ACE_TEXT ("%s: applying connection configuration\n"),
      //              inherited::mod_->name ()));
      ACE_ASSERT (iterator_2 != inherited::configuration_->connectionConfigurations->end ());

      clone_module =
          inherited::configuration_->streamConfiguration->configuration_.cloneModule;
      delete_module =
          inherited::configuration_->streamConfiguration->configuration_.deleteModule;
      module_p =
          inherited::configuration_->streamConfiguration->configuration_.module;
      inherited::configuration_->streamConfiguration->configuration_.cloneModule =
          false;
      inherited::configuration_->streamConfiguration->configuration_.deleteModule =
          false;
      inherited::configuration_->streamConfiguration->configuration_.module =
          NULL;

      if (!iconnector_p->initialize ((*iterator_2).second))
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("%s: failed to ConnectorType::INTERFACE_T::initialize(): \"%m\", returning\n"),
                    inherited::mod_->name ()));
        goto reset;
      } // end IF

      // step3: connect
      ACE_ASSERT (!connection_);

//      ACE_DEBUG ((LM_DEBUG,
//                  ACE_TEXT ("%s: connecting to %s\n"),
//                  inherited::mod_->name (),
//                  ACE_TEXT (Net_Common_Tools::IPAddressToString (address_).c_str ())));

      try {
        handle = iconnector_p->connect (address_);
      } catch (...) {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("%s: caught exception in Net_IConnector::connect(%s), continuing\n"),
                    inherited::mod_->name (),
                    ACE_TEXT (Net_Common_Tools::IPAddressToString (address_).c_str ())));
      }
      if (iconnector_p->useReactor ())
      {
        if (handle != ACE_INVALID_HANDLE)
          connection_ =
#if defined (ACE_WIN32) || defined (ACE_WIN64)
            iconnection_manager_p->get (reinterpret_cast<Net_ConnectionId_t> (handle));
#else
            iconnection_manager_p->get (static_cast<Net_ConnectionId_t> (handle));
#endif
      } // end IF
      else
      {
        // step1: wait for the connection to register with the manager
        ACE_Time_Value timeout (NET_CLIENT_DEFAULT_ASYNCH_CONNECT_TIMEOUT, 0);
        ACE_Time_Value deadline = COMMON_TIME_NOW + timeout;
        // *TODO*: this may not be accurate/applicable for/to all protocols
        enum Net_Connection_Status status = NET_CONNECTION_STATUS_INVALID;
        do
        {
          // *TODO*: avoid these tight loops
          connection_ = iconnection_manager_p->get (address_,
                                                    is_peer_address);
          if (!connection_)
            continue;

          // step2: wait for the connection to finish initializing
          do
          {
            status = connection_->status ();
            if (status == NET_CONNECTION_STATUS_OK)
              break;
          } while (COMMON_TIME_NOW < deadline);
          if (status != NET_CONNECTION_STATUS_OK)
            break;

          // step3: wait for the connection stream to finish initializing
          istream_connection_p =
              dynamic_cast<typename ConnectorType::ISTREAM_CONNECTION_T*> (connection_);
          if (!istream_connection_p)
          {
            ACE_DEBUG ((LM_ERROR,
                        ACE_TEXT ("%s: failed to dynamic_cast<Net_IStreamConnection_T>(0x%@), aborting\n"),
                        inherited::mod_->name (),
                        connection_));
            goto reset;
          } // end IF
          istream_connection_p->wait (STREAM_STATE_RUNNING,
                                      NULL); // <-- block
          break;
        } while (COMMON_TIME_NOW < deadline);
      } // end ELSE
      if (!connection_)
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("%s: failed to connect to %s, aborting\n"),
                    inherited::mod_->name (),
                    ACE_TEXT (Net_Common_Tools::IPAddressToString (address_).c_str ())));

        // clean up
        iconnector_p->abort ();

        goto reset;
      } // end IF
      isOpen_ = true;
      //ACE_DEBUG ((LM_DEBUG,
      //            ACE_TEXT ("%s: connected to %s (id: %u)\n"),
      //            inherited::mod_->name (),
      //            ACE_TEXT (Net_Common_Tools::IPAddressToString (address_).c_str ()),
      //            connection_->id ()));
      notify_connect = true;

reset:
      inherited::configuration_->streamConfiguration->configuration_.cloneModule =
          clone_module;
      inherited::configuration_->streamConfiguration->configuration_.deleteModule =
          delete_module;
      inherited::configuration_->streamConfiguration->configuration_.module =
          module_p;
      if (!connection_)
        goto error;

      // link processing streams
      istream_connection_p =
          dynamic_cast<typename ConnectorType::ISTREAM_CONNECTION_T*> (connection_);
      if (!istream_connection_p)
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("%s: failed to dynamic_cast<Net_IStreamConnection_T>(0x%@): \"%m\", aborting\n"),
                    inherited::mod_->name (),
                    connection_));
        goto error;
      } // end IF
      stream_p =
          &const_cast<typename ConnectorType::STREAM_T&> (istream_connection_p->stream ());
      ACE_ASSERT (inherited::stream_);
      if (!inherited::stream_->link (stream_p))
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("%s: failed to ACE_Stream::link(): \"%m\", aborting\n"),
                    inherited::mod_->name ()));
        goto error;
      } // end IF
      unlink_ = true;

      // update session data in the current session message
      // *WARNING*: this works iff (!) the STREAM_SESSION_LINK message has been
      //            received by now (i.e. if upstream is entirely synchronous)
      inherited::sessionData_->increase ();
      session_data_container_p = inherited::sessionData_;
      message_inout->initialize (STREAM_SESSION_MESSAGE_BEGIN,
                                 session_data_container_p,
                                 &const_cast<typename SessionMessageType::USER_DATA_T&> (message_inout->data ()));
      session_data_r =
        const_cast<SessionDataType&> (inherited::sessionData_->get ());

      goto continue_;

error:
      if (unlink_)
      { ACE_ASSERT (connection_);
        istream_connection_p =
          dynamic_cast<typename ConnectorType::ISTREAM_CONNECTION_T*> (connection_);
        if (!istream_connection_p)
        {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("%s: failed to dynamic_cast<Net_IStreamConnection_T>(%@): \"%m\", returning\n"),
                      inherited::mod_->name (),
                      connection_));
          return;
        } // end IF
        typename inherited::ISTREAM_T* istream_p =
          &const_cast<typename ConnectorType::STREAM_T&> (istream_connection_p->stream ());
        istream_p->_unlink ();
      
        unlink_ = false;
      } // end IF

      if (isOpen_ &&
          !isPassive_)
      { ACE_ASSERT (connection_);
        Net_ConnectionId_t id = connection_->id ();
        connection_->close ();
        ACE_DEBUG ((LM_DEBUG,
                    ACE_TEXT ("%s: closed connection to %s (id was: %u)\n"),
                    inherited::mod_->name (),
                    ACE_TEXT (Net_Common_Tools::IPAddressToString (address_).c_str ()),
                    id));
      } // end IF
      isOpen_ = false;

      if (connection_)
      {
        connection_->decrease ();
        connection_ = NULL;
      } // end IF

      inherited::notify (STREAM_SESSION_MESSAGE_ABORT);

      break;

continue_:
      // sanity check(s)
      ACE_ASSERT (connection_);

      // set session ID
      // *TODO*: remove type inferences
      ACE_ASSERT (session_data_r.lock);
      { ACE_GUARD (ACE_SYNCH_MUTEX, aGuard, *session_data_r.lock);
        session_data_r.connectionState =
          &const_cast<typename ConnectionManagerType::STATE_T&> (connection_->state ());
        session_data_r.sessionID = connection_->id ();
      } // end lock scope
      //ACE_DEBUG ((LM_DEBUG,
      //            ACE_TEXT ("%s: reset session id to %u\n"),
      //            inherited::mod_->name (),
      //            session_data_r.sessionID));

      if (notify_connect)
        inherited::notify (STREAM_SESSION_MESSAGE_CONNECT);

      break;
    }
    case STREAM_SESSION_MESSAGE_DISCONNECT:
    {
      if (isOpen_)
      {
        // *TODO*: the stream can control several connections, so this may be
        //         wrong...
        //isOpen_ = false;

        //ACE_DEBUG ((LM_DEBUG,
        //            ACE_TEXT ("%s: disconnected\n"),
        //            inherited::mod_->name ()));
      } // end IF

      break;
    }
    case STREAM_SESSION_MESSAGE_END:
    {
      // *NOTE*: control reaches this point because either:
      //         - the connection has been closed and the processing stream is
      //           finished()-ing (see: ACE_Svc_Handler::handle_close()
      //           derivates)
      //           --> wait for upstream data processing to complete before
      //               unlinking
      //         - the session is being aborted by the user/some module
      //           --> do NOT wait for upstream data processing to complete
      //               before unlinking, i.e. flush the connection stream

      // *NOTE*: when the connection is (asynchronously) closed by the peer, the
      //         connection handler will finished() the connection processing
      //         stream. If it is linked to another processing stream (e.g.
      //         'this') at this stage, the 'session end' message is propagated.
      //         Note that when the (linked) processing stream (e.g. 'this')
      //         itself is finished() (see below), it sends its own session end
      //         message. Handle both scenarios (and race conditions) here, i.e.
      //         never process consecutive 'session end' messages
      { ACE_GUARD (ACE_SYNCH_MUTEX, aGuard, inherited::lock_);
        if (inherited::sessionEndProcessed_)
          break; // done
        inherited::sessionEndProcessed_ = true;
      } // end lock scope

      // sanity check(s)
      ACE_ASSERT (inherited::sessionData_);

      typename SessionMessageType::DATA_T::DATA_T& session_data_r =
          const_cast<typename SessionMessageType::DATA_T::DATA_T&> (inherited::sessionData_->get ());

      if (inherited::isRunning ())
      {
        { ACE_GUARD (ACE_SYNCH_MUTEX, aGuard, inherited::lock_);
          inherited::sessionEndSent_ = true;
        } // end lock scope
      } // end IF

      if (inherited::timerID_ != -1)
      {
        const void* act_p = NULL;
        result =
            COMMON_TIMERMANAGER_SINGLETON::instance ()->cancel_timer (inherited::timerID_,
                                                                      &act_p);
        if (result == -1)
        {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("%s: failed to cancel timer (ID: %d): \"%m\", continuing\n"),
                      inherited::mod_->name (),
                      inherited::timerID_));
          goto continue_2;
        } // end IF
      } // end IF

      if (connection_)
      {
        typename ConnectorType::STREAM_T* stream_p = NULL;
        typename ConnectorType::ISTREAM_CONNECTION_T* istream_connection_p =
          dynamic_cast<typename ConnectorType::ISTREAM_CONNECTION_T*> (connection_);
        if (!istream_connection_p)
        {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("%s: failed to dynamic_cast<Net_IStreamConnection_T>(0x%@): \"%m\", continuing\n"),
                      inherited::mod_->name (),
                      connection_));
          goto continue_2;
        } // end IF
        stream_p =
          &const_cast<typename ConnectorType::STREAM_T&> (istream_connection_p->stream ());

        // wait for upstream data processing to complete before unlinking ?
        { ACE_GUARD (ACE_SYNCH_MUTEX, aGuard, *session_data_r.lock);
          if (session_data_r.aborted)
            goto flush;
        } // end lock scope

        // *NOTE*: finalize the (connection) stream state so waitForCompletion()
        //         does not block forever
        // *NOTE*: stop()ping the connection stream will also unlink it
        //         --> send the disconnect notification early
        inherited::notify (STREAM_SESSION_MESSAGE_DISCONNECT);
        // *TODO*: this shouldn't be necessary (--> only wait for data to flush)
        stream_p->stop (false, // wait for completion ?
                        false, // recurse upstream ?
                        true); // locked access ?
        connection_->waitForCompletion (false); // --> data only

        goto continue_2;

flush:
        stream_p->flush (true,   // flush inbound data ?
                         false,  // flush session messages ?
                         false); // flush upstream (if any) ?
      } // end IF

continue_2:
      if (unlink_)
      { ACE_ASSERT (inherited::stream_);
        inherited::stream_->_unlink ();

        unlink_ = false;
      } // end IF

      if (isOpen_ &&
          !isPassive_)
      { ACE_ASSERT (connection_);
        Net_ConnectionId_t id = connection_->id ();
        connection_->close ();
        ACE_DEBUG ((LM_DEBUG,
                    ACE_TEXT ("%s: closed connection to %s (id was: %u)\n"),
                    inherited::mod_->name (),
                    ACE_TEXT (Net_Common_Tools::IPAddressToString (address_).c_str ()),
                    id));
      } // end IF
      isOpen_ = false;

      if (connection_)
      {
        connection_->decrease ();
        connection_ = NULL;
      } // end IF

      if (inherited::concurrency_ != STREAM_HEADMODULECONCURRENCY_CONCURRENT)
        inherited::TASK_BASE_T::stop (false,  // wait for completion ?
                                      false); // N/A

      break;
    }
    case STREAM_SESSION_MESSAGE_LINK:
    {
      //ACE_DEBUG ((LM_DEBUG,
      //            ACE_TEXT ("%s: linked i/o stream(s)\n"),
      //            inherited::mod_->name ()));
#if defined (_DEBUG)
      ACE_ASSERT (inherited::stream_);
      inherited::stream_->dump_state ();
#endif
      break;
    }
    case STREAM_SESSION_MESSAGE_UNLINK:
    {
      // sanity check(s)
      if (!inherited::linked_)
        break;

      // *NOTE*: most probable reasons:
      //         - upstream has been stop()ped because the session has ended
      //           due to user interaction (see below). On STOP the state
      //           machine currently invokes finished(), which unlink()s any
      //           upstream automatically
      //         - the stream has been stop()ped because the connection has
      //           close()d, finished()-ing the upstream connection stream,
      //           which in turn unlink()s any downstream during shutdown
      //         in the latter case, do not unlink
      // *TODO*: the stream could have several substreams that are (un-)linked
      //         dynamically and independently; to avoid race conditions a
      //         context reference test is required here
      unlink_ = false;

#if defined (_DEBUG)
      ACE_ASSERT (inherited::stream_);
      inherited::stream_->dump_state ();
#endif
      break;
    }
    default:
      break;
  } // end SWITCH
}

template <ACE_SYNCH_DECL,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType,
          typename ConfigurationType,
          typename StreamControlType,
          typename StreamNotificationType,
          typename StreamStateType,
          typename SessionDataType,
          typename SessionDataContainerType,
          typename StatisticContainerType,
          typename StatisticHandlerType,
          typename ConnectionConfigurationIteratorType,
          typename ConnectionManagerType,
          typename ConnectorType,
          typename UserDataType>
bool
Stream_Module_Net_SourceH_T<ACE_SYNCH_USE,
                            ControlMessageType,
                            DataMessageType,
                            SessionMessageType,
                            ConfigurationType,
                            StreamControlType,
                            StreamNotificationType,
                            StreamStateType,
                            SessionDataType,
                            SessionDataContainerType,
                            StatisticContainerType,
                            StatisticHandlerType,
                            ConnectionConfigurationIteratorType,
                            ConnectionManagerType,
                            ConnectorType,
                            UserDataType>::collect (StatisticContainerType& data_out)
{
  STREAM_TRACE (ACE_TEXT ("Stream_Module_Net_SourceH_T::collect"));

  // sanity check(s)
  ACE_ASSERT (inherited::isInitialized_);

  // step0: initialize container
//  data_out.dataMessages = 0;
//  data_out.droppedMessages = 0;
//  data_out.bytes = 0.0;
  data_out.timeStamp = COMMON_TIME_NOW;

  // *TODO*: collect socket statistic information
  //         (and propagate it downstream ?)

  // step1: send the container downstream
  if (!inherited::putStatisticMessage (data_out)) // data container
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: failed to putStatisticMessage(), aborting\n"),
                inherited::mod_->name ()));
    return false;
  } // end IF

  return true;
}
