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

#include "common_tools.h"

#include "stream_defines.h"
#include "stream_macros.h"
#include "stream_session_message_base.h"

#include "stream_dec_tools.h"

#include "stream_dev_defines.h"
#include "stream_dev_tools.h"

#include "stream_misc_defines.h"

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename ConfigurationType,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType,
          typename SessionDataType,
          typename FilterConfigurationType,
          typename PinConfigurationType,
          typename MediaType>
Stream_Misc_DirectShow_Target_T<ACE_SYNCH_USE,
                                TimePolicyType,
                                ConfigurationType,
                                ControlMessageType,
                                DataMessageType,
                                SessionMessageType,
                                SessionDataType,
                                FilterConfigurationType,
                                PinConfigurationType,
                                MediaType>::Stream_Misc_DirectShow_Target_T ()
 : inherited ()
 //, mediaType_  (NULL)
 , push_ (MODULE_MISC_DS_WIN32_FILTER_SOURCE_DEFAULT_PUSH)
 , IGraphBuilder_ (NULL)
//, IMemAllocator_ (NULL)
//, IMemInputPin_ (NULL)
 , IMediaControl_ (NULL)
 , IMediaEventEx_ (NULL)
 , ROTID_ (0)
{
  STREAM_TRACE (ACE_TEXT ("Stream_Misc_DirectShow_Target_T::Stream_Misc_DirectShow_Target_T"));

}

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename ConfigurationType,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType,
          typename SessionDataType,
          typename FilterConfigurationType,
          typename PinConfigurationType,
          typename MediaType>
Stream_Misc_DirectShow_Target_T<ACE_SYNCH_USE,
                                TimePolicyType,
                                ConfigurationType,
                                ControlMessageType,
                                DataMessageType,
                                SessionMessageType,
                                SessionDataType,
                                FilterConfigurationType,
                                PinConfigurationType,
                                MediaType>::~Stream_Misc_DirectShow_Target_T ()
{
  STREAM_TRACE (ACE_TEXT ("Stream_Misc_DirectShow_Target_T::~Stream_Misc_DirectShow_Target_T"));

  int result = -1;

  //if (IMemAllocator_)
  //  IMemAllocator_->Release ();
  //if (IMemInputPin_)
  //  IMemInputPin_->Release ();
  //if (!push_)
  //{
    //result = inherited::queue_.flush ();
    //if (result == -1)
    //  ACE_DEBUG ((LM_ERROR,
    //              ACE_TEXT ("%s: failed to ACE_Message_Queue::flush() \"%m\", continuing\n"),
    //              inherited::mod_->name ()));
    inherited::queue_.waitForIdleState ();
  //} // end IF

  if (ROTID_)
  {
    IRunningObjectTable* ROT_p = NULL;
    HRESULT result = GetRunningObjectTable (0, &ROT_p);
    if (FAILED (result))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to GetRunningObjectTable() \"%s\", continuing\n"),
                  ACE_TEXT (Common_Tools::error2String (result).c_str ())));
      goto continue_;
    } // end IF
    ACE_ASSERT (ROT_p);
    result = ROT_p->Revoke (ROTID_);
    if (FAILED (result))
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to IRunningObjectTable::Revoke(%d) \"%s\", continuing\n"),
                  ROTID_,
                  ACE_TEXT (Common_Tools::error2String (result).c_str ())));
    ROT_p->Release ();
  } // end IF

continue_:
  if (IMediaEventEx_)
  {
    IMediaEventEx_->SetNotifyWindow (NULL, 0, 0);
    IMediaEventEx_->Release ();
  } // end IF
  if (IMediaControl_)
  {
    IMediaControl_->Stop ();
    IMediaControl_->Release ();
  } // end IF

  if (IGraphBuilder_)
    IGraphBuilder_->Release ();
}

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename ConfigurationType,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType,
          typename SessionDataType,
          typename FilterConfigurationType,
          typename PinConfigurationType,
          typename MediaType>
bool
Stream_Misc_DirectShow_Target_T<ACE_SYNCH_USE,
                                TimePolicyType,
                                ConfigurationType,
                                ControlMessageType,
                                DataMessageType,
                                SessionMessageType,
                                SessionDataType,
                                FilterConfigurationType,
                                PinConfigurationType,
                                MediaType>::initialize (const ConfigurationType& configuration_in)
{
  STREAM_TRACE (ACE_TEXT ("Stream_Misc_DirectShow_Target_T::initialize"));

  // sanity check(s)
  // *TODO*: remove type inference
  ACE_ASSERT (configuration_in.filterConfiguration);
  ACE_ASSERT (configuration_in.filterConfiguration->pinConfiguration);

  // initialize COM ?
  HRESULT result = E_FAIL;
  static bool first_run = true;
  bool COM_initialized = false;
  if (first_run)
  {
    first_run = false;

    result = CoInitializeEx (NULL, COINIT_MULTITHREADED);
    if (FAILED (result))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to CoInitializeEx(COINIT_MULTITHREADED): \"%s\", aborting\n"),
                  ACE_TEXT (Common_Tools::error2String (result).c_str ())));
      return false;
    } // end IF
    COM_initialized = true;
  } // end IF

  if (inherited::isInitialized_)
  {
    //ACE_DEBUG ((LM_WARNING,
    //            ACE_TEXT ("re-initializing...\n")));

    // clean up ?
    //if (IMemAllocator_)
    //{
    //  IMemAllocator_->Release ();
    //  IMemAllocator_ = NULL;
    //} // end IF
    //if (IMemInputPin_)
    //{
    //  IMemInputPin_->Release ();
    //  IMemInputPin_ = NULL;
    //} // end IF
    //if (!push_)
    //{
      inherited::queue_.waitForIdleState ();
    //} // end IF

    if (ROTID_)
    {
      IRunningObjectTable* ROT_p = NULL;
      result = GetRunningObjectTable (0, &ROT_p);
      if (FAILED (result))
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to GetRunningObjectTable() \"%s\", continuing\n"),
                    ACE_TEXT (Common_Tools::error2String (result).c_str ())));

        // clean up
        ROTID_ = 0;

        goto continue_;
      } // end IF
      ACE_ASSERT (ROT_p);
      result = ROT_p->Revoke (ROTID_);
      if (FAILED (result))
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to IRunningObjectTable::Revoke(%d) \"%s\", continuing\n"),
                    ROTID_,
                    ACE_TEXT (Common_Tools::error2String (result).c_str ())));
      ROT_p->Release ();
      ROTID_ = 0;
    } // end IF

continue_:
    if (IMediaEventEx_)
    {
      IMediaEventEx_->SetNotifyWindow (NULL, 0, 0);
      IMediaEventEx_->Release ();
      IMediaEventEx_ = NULL;
    } // end IF
    if (IMediaControl_)
    {
      IMediaControl_->Stop ();
      IMediaControl_->Release ();
      IMediaControl_ = NULL;
    } // end IF

    if (IGraphBuilder_)
    {
      IGraphBuilder_->Release ();
      IGraphBuilder_ = NULL;
    } // end IF

    //mediaType_ = NULL;
  } // end IF

  //mediaType_ = &configuration_->mediaType;
  // *TODO*: remove type inference
  push_ = configuration_->push;
  configuration_in.filterConfiguration->module = inherited::mod_;
  configuration_in.filterConfiguration->pinConfiguration->queue =
    &(inherited::queue_);

  return inherited::initialize (configuration_in);
}

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename ConfigurationType,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType,
          typename SessionDataType,
          typename FilterConfigurationType,
          typename PinConfigurationType,
          typename MediaType>
void
Stream_Misc_DirectShow_Target_T<ACE_SYNCH_USE,
                                TimePolicyType,
                                ConfigurationType,
                                ControlMessageType,
                                DataMessageType,
                                SessionMessageType,
                                SessionDataType,
                                FilterConfigurationType,
                                PinConfigurationType,
                                MediaType>::handleDataMessage (DataMessageType*& message_inout,
                                                               bool& passMessageDownstream_out)
{
  STREAM_TRACE (ACE_TEXT ("Stream_Misc_DirectShow_Target_T::handleDataMessage"));

  // don't care (implies yes per default, if part of a stream)
  ACE_UNUSED_ARG (passMessageDownstream_out);

  ACE_Message_Block* message_block_p = message_inout->duplicate ();
  if (!message_block_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Message_Block::duplicate(): \"%m\", returning\n")));
    return;
  } // end IF
  int result = inherited::queue_.enqueue_tail (message_block_p, NULL);
  if (result == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Message_Queue::enqueue_tail(): \"%m\", returning\n")));

    // clean up
    message_block_p->release ();

    return;
  } // end IF

  return;
}

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename ConfigurationType,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType,
          typename SessionDataType,
          typename FilterConfigurationType,
          typename PinConfigurationType,
          typename MediaType>
void
Stream_Misc_DirectShow_Target_T<ACE_SYNCH_USE,
                                TimePolicyType,
                                ConfigurationType,
                                ControlMessageType,
                                DataMessageType,
                                SessionMessageType,
                                SessionDataType,
                                FilterConfigurationType,
                                PinConfigurationType,
                                MediaType>::handleSessionMessage (SessionMessageType*& message_inout,
                                                                  bool& passMessageDownstream_out)
{
  STREAM_TRACE (ACE_TEXT ("Stream_Misc_DirectShow_Target_T::handleSessionMessage"));

  int result = -1;
  IRunningObjectTable* ROT_p = NULL;

  // don't care (implies yes per default, if part of a stream)
  ACE_UNUSED_ARG (passMessageDownstream_out);

  // sanity check(s)
  ACE_ASSERT (inherited::configuration_);

  switch (message_inout->type ())
  {
    case STREAM_SESSION_MESSAGE_BEGIN:
    {
      const typename SessionMessageType::DATA_T& session_data_container_r =
        message_inout->get ();
      SessionDataType& session_data_r =
        const_cast<SessionDataType&> (session_data_container_r.get ());

      // sanity check(s)
      // *TODO*: remove type inference
      ACE_ASSERT (inherited::configuration_->streamConfiguration);

      bool COM_initialized = false;
      bool is_running = false;
#if defined (_DEBUG)
      std::string log_file_name;
#endif
      IMoniker* moniker_p = NULL;
      WCHAR buffer[BUFSIZ];

      HRESULT result_2 = CoInitializeEx (NULL,
                                         COINIT_MULTITHREADED);
      if (FAILED (result_2))
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to CoInitializeEx(COINIT_MULTITHREADED): \"%s\", returning\n"),
                    ACE_TEXT (Common_Tools::error2String (result_2).c_str ())));
        goto error;
      } // end IF
      COM_initialized = true;

      if (!IGraphBuilder_)
      {
        if (session_data_r.graphBuilder)
        {
          ULONG reference_count = session_data_r.graphBuilder->AddRef ();
          IGraphBuilder_ = session_data_r.graphBuilder;
        } // end IF
        else
        {
          // sanity check(s)
          // *TODO*: remove type inferences
          ACE_ASSERT (inherited::configuration_->filterConfiguration);

          if (!initialize_DirectShow (inherited::configuration_->filterCLSID,
                                      *inherited::configuration_->filterConfiguration,
                                      *inherited::configuration_->format,
                                      inherited::configuration_->window,
                                      IGraphBuilder_))
          {
            ACE_DEBUG ((LM_ERROR,
                        ACE_TEXT ("failed to initialize_DirectShow(), returning\n")));
            goto error;
          } // end IF
          ACE_ASSERT (IGraphBuilder_);
        } // end ELSE
      } // end IF
      ACE_ASSERT (IGraphBuilder_);
#if defined (_DEBUG)
      log_file_name =
        Common_File_Tools::getLogDirectory (std::string (),
                                            0);
      log_file_name += ACE_DIRECTORY_SEPARATOR_STR;
      log_file_name += MODULE_DEV_DIRECTSHOW_LOGFILE_NAME;
      Stream_Module_Device_Tools::debug (IGraphBuilder_,
                                         log_file_name);
#endif
      // sanity check(s)
      ACE_ASSERT (!IMediaControl_);
      ACE_ASSERT (!IMediaEventEx_);

      // retrieve interfaces for media control and the video window
      result_2 =
        IGraphBuilder_->QueryInterface (IID_PPV_ARGS (&IMediaControl_));
      if (FAILED (result_2)) goto error_2;
      result_2 =
        IGraphBuilder_->QueryInterface (IID_PPV_ARGS (&IMediaEventEx_));
      if (FAILED (result_2)) goto error_2;

      goto do_run;
error_2:
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to IGraphBuilder::QueryInterface(): \"%s\", aborting\n"),
                  ACE_TEXT (Common_Tools::error2String (result_2).c_str ())));
      goto error;

do_run:
      ACE_ASSERT (IMediaControl_);
      ACE_ASSERT (IMediaEventEx_);

      // start forwarding data
      result_2 = IMediaControl_->Run ();
      if (FAILED (result_2))
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to IMediaControl::Run(): \"%s\", returning\n"),
                    ACE_TEXT (Common_Tools::error2String (result_2).c_str ())));
        goto error;
      } // end IF
      is_running = true;

      // register graph in the ROT (GraphEdit.exe)
      result_2 = GetRunningObjectTable (0, &ROT_p);
      if (FAILED (result_2))
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to GetRunningObjectTable(): \"%s\", returning\n"),
                    ACE_TEXT (Common_Tools::error2String (result_2).c_str ())));
        goto error;
      } // end IF
      ACE_ASSERT (ROT_p);
      result_2 =
        ::StringCchPrintfW (buffer, NUMELMS (buffer),
                            ACE_TEXT_ALWAYS_WCHAR ("FilterGraph %08x [PID: %08x]\0"),
                            (DWORD_PTR)IGraphBuilder_, ACE_OS::getpid ());
      result_2 = CreateItemMoniker (ACE_TEXT_ALWAYS_WCHAR ("!"), buffer,
                                    &moniker_p);
      if (FAILED (result_2))
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to CreateItemMoniker(): \"%s\", returning\n"),
                    ACE_TEXT (Common_Tools::error2String (result_2).c_str ())));

        // clean up
        ROT_p->Release ();

        goto error;
      } // end IF

      // Use the ROTFLAGS_REGISTRATIONKEEPSALIVE to ensure a strong reference
      // to the object.  Using this flag will cause the object to remain
      // registered until it is explicitly revoked with the Revoke() method.
      // Not using this flag means that if GraphEdit remotely connects
      // to this graph and then GraphEdit exits, this object registration
      // will be deleted, causing future attempts by GraphEdit to fail until
      // this application is restarted or until the graph is registered again.
      result_2 =
        ROT_p->Register (ROTFLAGS_REGISTRATIONKEEPSALIVE,
                         IGraphBuilder_, moniker_p,
                         &ROTID_);
      if (FAILED (result_2))
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to IRunningObjectTable::Register(): \"%s\", aborting\n"),
                    ACE_TEXT (Common_Tools::error2String (result_2).c_str ())));

        // clean up
        ROT_p->Release ();
        moniker_p->Release ();

        goto error;
      } // end IF
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("registered graph in running object table (ID: %d)\n"),
                  ROTID_));

      // clean up
      ROT_p->Release ();
      moniker_p->Release ();

      break;

error:
      if (is_running)
      { ACE_ASSERT (IMediaControl_);
        result_2 = IMediaControl_->Stop ();
        if (FAILED (result_2))
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("failed to IMediaControl::Stop(): \"%s\", continuing\n"),
                      ACE_TEXT (Common_Tools::error2String (result_2).c_str ())));
      } // end IF
      if (COM_initialized)
        CoUninitialize ();

      notify (STREAM_SESSION_MESSAGE_ABORT);

      break;
    }
    case STREAM_SESSION_MESSAGE_END:
    {
      bool COM_initialized = false;
      HRESULT result_2 = CoInitializeEx (NULL, COINIT_MULTITHREADED);
      if (FAILED (result_2))
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to CoInitializeEx(COINIT_MULTITHREADED): \"%s\", aborting\n"),
                    ACE_TEXT (Common_Tools::error2String (result_2).c_str ())));
        break;
      } // end IF
      COM_initialized = true;

      // deregister graph from the ROT (GraphEdit.exe) ?
      if (ROTID_)
      {
        result_2 = GetRunningObjectTable (0, &ROT_p);
        if (FAILED (result_2))
        {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("failed to GetRunningObjectTable(): \"%s\", continuing\n"),
                      ACE_TEXT (Common_Tools::error2String (result_2).c_str ())));

          // clean up
          ROTID_ = 0;

          goto continue_;
        } // end IF
        ACE_ASSERT (ROT_p);
        result_2 = ROT_p->Revoke (ROTID_);
        if (FAILED (result_2))
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("failed to IRunningObjectTable::Revoke(%d): \"%s\", continuing\n"),
                      ROTID_,
                      ACE_TEXT (Common_Tools::error2String (result_2).c_str ())));
        else
          ACE_DEBUG ((LM_DEBUG,
                      ACE_TEXT ("removed graph from running object table (ID was: %d)\n"),
                      ROTID_));

        ROT_p->Release ();
        ROTID_ = 0;
      } // end IF

continue_:
      if (IMediaEventEx_)
      {
        result_2 = IMediaEventEx_->SetNotifyWindow (NULL, 0, 0);
        if (FAILED (result_2))
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("failed to IMediaEventEx::SetNotifyWindow(): \"%s\", continuing\n"),
                      ACE_TEXT (Common_Tools::error2String (result_2).c_str ())));
        IMediaEventEx_->Release ();
        IMediaEventEx_ = NULL;
      } // end IF

      if (IMediaControl_)
      {
        // stop previewing video data (blocks)
        result_2 = IMediaControl_->Stop ();
        if (FAILED (result_2))
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("failed to IMediaControl::Stop(): \"%s\", continuing\n"),
                      ACE_TEXT (Common_Tools::error2String (result_2).c_str ())));

        IMediaControl_->Release ();
        IMediaControl_ = NULL;
      } // end IF

      //if (!Stream_Module_Device_Tools::disconnect (IGraphBuilder_))
      //  ACE_DEBUG ((LM_ERROR,
      //              ACE_TEXT ("failed to Stream_Module_Device_Tools::disconnect(), continuing\n")));

      if (IGraphBuilder_)
      {
        IGraphBuilder_->Release ();
        IGraphBuilder_ = NULL;
      } // end IF

      if (COM_initialized)
        CoUninitialize ();

      break;
    }
    default:
      break;
  } // end SWITCH
}

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename ConfigurationType,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType,
          typename SessionDataType,
          typename FilterConfigurationType,
          typename PinConfigurationType,
          typename MediaType>
bool
Stream_Misc_DirectShow_Target_T<ACE_SYNCH_USE,
                                TimePolicyType,
                                ConfigurationType,
                                ControlMessageType,
                                DataMessageType,
                                SessionMessageType,
                                SessionDataType,
                                FilterConfigurationType,
                                PinConfigurationType,
                                MediaType>::initialize_DirectShow (const struct _GUID& CLSID_in,
                                                                   const FilterConfigurationType& filterConfiguration_in,
                                                                   const struct _AMMediaType& mediaType_in,
                                                                   const HWND windowHandle_in,
                                                                   IGraphBuilder*& IGraphBuilder_out)
{
  STREAM_TRACE (ACE_TEXT ("Stream_Misc_DirectShow_Target_T::initialize_DirectShow"));

  ACE_UNUSED_ARG (mediaType_in);

  // sanity check(s)
  if (IGraphBuilder_out)
    IGraphBuilder_out->Release ();

  // initialize return value(s)
  IGraphBuilder_out = NULL;

  IAMBufferNegotiation* buffer_negotiation_p;
  Stream_Module_Device_DirectShow_Graph_t graph_configuration;
  struct Stream_Module_Device_DirectShow_GraphEntry graph_entry;
  if (!Stream_Module_Device_Tools::loadTargetRendererGraph (windowHandle_in,
                                                            IGraphBuilder_out,
                                                            buffer_negotiation_p,
                                                            graph_configuration))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Stream_Module_Device_Tools::loadTargetRendererGraph(), aborting\n")));
    return false;
  } // end IF
  ACE_ASSERT (IGraphBuilder_out);
  ACE_ASSERT (buffer_negotiation_p);
  buffer_negotiation_p->Release ();

  //// *NOTE*: (re-)connect()ion of the video renderer input pin fails
  ////         consistently, so, apparently, reuse is not foreseen
  ////         --> rebuild the whole graph each session
  //if (!Stream_Module_Device_Tools::reset (IGraphBuilder_out))
  //{
  //  ACE_DEBUG ((LM_ERROR,
  //              ACE_TEXT ("failed to Stream_Module_Device_Tools::reset(), aborting\n")));
  //  return false;
  //} // end IF

  IBaseFilter* ibase_filter_p = NULL;
  //ACE_NEW_NORETURN (ibase_filter_p,
  //                  FILTER_T ());
  //if (!filter_p)
  //{
  //  ACE_DEBUG ((LM_CRITICAL,
  //              ACE_TEXT ("failed to allocate memory: \"%m\", aborting\n")));
  //  return false;
  //} // end IF
  HRESULT result =
    CoCreateInstance (CLSID_in, NULL,
                      CLSCTX_INPROC_SERVER, IID_IBaseFilter,
                      (void**)&ibase_filter_p);
  if (FAILED (result))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to CoCreateInstance(\"%s\"): \"%s\", aborting\n"),
                ACE_TEXT (Stream_Module_Decoder_Tools::GUIDToString (CLSID_in).c_str ()),
                ACE_TEXT (Common_Tools::error2String (result).c_str ())));
    return false;
  } // end IF
  ACE_ASSERT (ibase_filter_p);
  IINITIALIZE_FILTER_T* iinitialize_p =
    dynamic_cast<IINITIALIZE_FILTER_T*> (ibase_filter_p);
  if (!iinitialize_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to dynamic_cast<Common_IInitialize_T*>(%@), aborting\n"),
                ibase_filter_p));

    // clean up
    ibase_filter_p->Release ();
    IGraphBuilder_out->Release ();
    IGraphBuilder_out = NULL;

    return false;
  } // end IF
  // *TODO*: remove type inference
  if (!iinitialize_p->initialize (filterConfiguration_in))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_IInitialize_T::initialize(), aborting\n")));

    // clean up
    ibase_filter_p->Release ();
    IGraphBuilder_out->Release ();
    IGraphBuilder_out = NULL;

    return false;
  } // end IF

  graph_entry.filterName =
    (push_ ? MODULE_MISC_DS_WIN32_FILTER_NAME_SOURCE_L
           : MODULE_MISC_DS_WIN32_FILTER_NAME_ASYNCH_SOURCE_L);
  //ACE_TEXT_ALWAYS_WCHAR (Stream_Module_Device_Tools::name (ibase_filter_p).c_str ());
  result = IGraphBuilder_out->AddFilter (ibase_filter_p,
                                         graph_entry.filterName.c_str ());
  if (FAILED (result))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to IGraphBuilder::AddFilter(\"%s\"): \"%s\", aborting\n"),
                ACE_TEXT_WCHAR_TO_TCHAR (graph_entry.filterName.c_str ()),
                ACE_TEXT (Common_Tools::error2String (result).c_str ())));

    // clean up
    ibase_filter_p->Release ();
    IGraphBuilder_out->Release ();
    IGraphBuilder_out = NULL;

    return false;
  } // end IF
  graph_configuration.push_front (graph_entry);
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("added \"%s\"...\n"),
              ACE_TEXT_WCHAR_TO_TCHAR (graph_entry.filterName.c_str ())));

  if (!Stream_Module_Device_Tools::connect (IGraphBuilder_out,
                                            graph_configuration))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Stream_Module_Device_Tools::connect(), aborting\n")));

    // clean up
    IGraphBuilder_out->Release ();
    IGraphBuilder_out = NULL;

    return false;
  } // end IF

  // retrieve interfaces for media control and the video window
  result =
    IGraphBuilder_out->QueryInterface (IID_PPV_ARGS (&IMediaControl_));
  if (FAILED (result))
    goto error;
  result = IGraphBuilder_out->QueryInterface (IID_PPV_ARGS (&IMediaEventEx_));
  if (FAILED (result))
    goto error;

  goto continue_;

error:
  ACE_DEBUG ((LM_ERROR,
              ACE_TEXT ("failed to IGraphBuilder::QueryInterface(): \"%s\", aborting\n"),
              ACE_TEXT (Common_Tools::error2String (result).c_str ())));
  return false;

continue_:
  ACE_ASSERT (IMediaControl_);
  ACE_ASSERT (IMediaEventEx_);
  // set the window handle used to process graph events
  result =
    IMediaEventEx_->SetNotifyWindow ((OAHWND)windowHandle_in,
                                     MODULE_DEV_CAM_UI_WIN32_WM_GRAPHNOTIFY, 0);
  if (FAILED (result))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to IMediaEventEx::SetNotifyWindow(): \"%s\", aborting\n"),
                ACE_TEXT (Common_Tools::error2String (result).c_str ())));
    return false;
  } // end IF

  ibase_filter_p = NULL;
  result = IGraphBuilder_out->FindFilterByName (graph_entry.filterName.c_str (),
                                                &ibase_filter_p);
  if (FAILED (result))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to IGraphBuilder::FindFilterByName(\"%s\"): \"%s\", aborting\n"),
                ACE_TEXT_WCHAR_TO_TCHAR (graph_entry.filterName.c_str ()),
                ACE_TEXT (Common_Tools::error2String (result).c_str ())));
    return false;
  } // end IF
  ACE_ASSERT (ibase_filter_p);

  IEnumPins* enumerator_p = NULL;
  result = ibase_filter_p->EnumPins (&enumerator_p);
  if (FAILED (result))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: failed to IBaseFilter::EnumPins(): \"%s\", aborting\n"),
                ACE_TEXT_WCHAR_TO_TCHAR (graph_entry.filterName.c_str ()),
                ACE_TEXT (Common_Tools::error2String (result).c_str ())));

    // clean up
    ibase_filter_p->Release ();

    return false;
  } // end IF
  ACE_ASSERT (enumerator_p);
  ibase_filter_p->Release ();
  IPin* pin_p = NULL;
  PIN_DIRECTION pin_direction;
  while (enumerator_p->Next (1, &pin_p, NULL) == S_OK)
  {
    ACE_ASSERT (pin_p);

    result = pin_p->QueryDirection (&pin_direction);
    if (FAILED (result))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("%s: failed to IPin::QueryDirection(): \"%s\", aborting\n"),
                  ACE_TEXT_WCHAR_TO_TCHAR (graph_entry.filterName.c_str ()),
                  ACE_TEXT (Common_Tools::error2String (result).c_str ())));

      // clean up
      pin_p->Release ();
      enumerator_p->Release ();

      return false;
    } // end IF
    if (pin_direction != PINDIR_INPUT)
    {
      pin_p->Release ();
      pin_p = NULL;

      continue;
    } // end IF

    break;
  } // end WHILE
  enumerator_p->Release ();
  if (!pin_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: has no input pin, aborting\n"),
                ACE_TEXT_WCHAR_TO_TCHAR (graph_entry.filterName.c_str ())));
    return false;
  } // end IF

  //result = pin_p->QueryInterface (IID_PPV_ARGS (&IMemInputPin_));
  //if (FAILED (result))
  //{
  //  ACE_DEBUG ((LM_ERROR,
  //              ACE_TEXT ("failed to IPin::QueryInterface(IID_IMemInputPin): \"%s\", aborting\n"),
  //              ACE_TEXT (Common_Tools::error2String (result).c_str ())));

  //  // clean up
  //  pin_p->Release ();

  //  return false;
  //} // end IF
  //ACE_ASSERT (IMemInputPin_);
  pin_p->Release ();

  //result = IMemInputPin_->GetAllocator (&IMemAllocator_);
  //if (FAILED (result))
  //{
  //  ACE_DEBUG ((LM_ERROR,
  //              ACE_TEXT ("failed to IMemInputPin::GetAllocator(): \"%s\", aborting\n"),
  //              ACE_TEXT (Common_Tools::error2String (result).c_str ())));

  //  // clean up
  //  IMemInputPin_->Release ();
  //  IMemInputPin_ = NULL;

  //  return false;
  //} // end IF
  //ACE_ASSERT (IMemAllocator_);

  return true;
}