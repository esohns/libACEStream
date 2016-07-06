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

#include "test_u_camsave_stream.h"

#include "ace/Log_Msg.h"

#include "stream_macros.h"

// initialize statics
ACE_Atomic_Op<ACE_Thread_Mutex,
              unsigned long> Stream_CamSave_Stream::currentSessionID = 0;

Stream_CamSave_Stream::Stream_CamSave_Stream ()
 : inherited (ACE_TEXT_ALWAYS_CHAR ("FileCopyStream"))
 , source_ (ACE_TEXT_ALWAYS_CHAR ("CamSource"),
            NULL,
            false)
 , runtimeStatistic_ (ACE_TEXT_ALWAYS_CHAR ("RuntimeStatistic"),
                      NULL,
                      false)
 , display_ (ACE_TEXT_ALWAYS_CHAR ("Display"),
             NULL,
             false)
#if defined (ACE_WIN32) || defined (ACE_WIN64)
 , displayNull_ (ACE_TEXT_ALWAYS_CHAR ("DisplayNull"),
                 NULL,
                 false)
#endif
 , encoder_ (ACE_TEXT_ALWAYS_CHAR ("AVIEncoder"),
             NULL,
             false)
 , fileWriter_ (ACE_TEXT_ALWAYS_CHAR ("FileWriter"),
                NULL,
                false)
#if defined (ACE_WIN32) || defined (ACE_WIN64)
 , mediaSession_ (NULL)
 , referenceCount_ (1)
#endif
{
  STREAM_TRACE (ACE_TEXT ("Stream_CamSave_Stream::Stream_CamSave_Stream"));

}

Stream_CamSave_Stream::~Stream_CamSave_Stream ()
{
  STREAM_TRACE (ACE_TEXT ("Stream_CamSave_Stream::~Stream_CamSave_Stream"));

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  HRESULT result = E_FAIL;
  if (mediaSession_)
  {
    result = mediaSession_->Shutdown ();
    if (FAILED (result))
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to IMFMediaSession::Shutdown(): \"%s\", continuing\n"),
                  ACE_TEXT (Common_Tools::error2String (result).c_str ())));
    mediaSession_->Release ();
  } // end IF
#endif

  // *NOTE*: this implements an ordered shutdown on destruction...
  inherited::shutdown ();
}

#if defined (ACE_WIN32) || defined (ACE_WIN64)
Stream_Module_t*
Stream_CamSave_Stream::find (const std::string& name_in) const
{
  STREAM_TRACE (ACE_TEXT ("Stream_CamSave_Stream::find"));

  if (ACE_OS::strcmp (name_in.c_str (),
                      ACE_TEXT_ALWAYS_CHAR ("DisplayNull")) == 0)
    return const_cast<Stream_CamSave_Module_DisplayNull_Module*> (&displayNull_);

  return inherited::find (name_in);
}
void
Stream_CamSave_Stream::start ()
{
  STREAM_TRACE (ACE_TEXT ("Stream_CamSave_Stream::start"));

  inherited::start ();

  // sanity check(s)
  ACE_ASSERT (mediaSession_);

  struct _GUID GUID_s = GUID_NULL;
  struct tagPROPVARIANT property_s;
  PropVariantInit (&property_s);
  //property_s.vt = VT_EMPTY;
  HRESULT result = mediaSession_->Start (&GUID_s,      // time format
                                         &property_s); // start position
  if (FAILED (result))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to IMFMediaSession::Start(): \"%s\", returning\n"),
                ACE_TEXT (Common_Tools::error2String (result).c_str ())));

    // clean up
    PropVariantClear (&property_s);

    return;
  } // end IF
  PropVariantClear (&property_s);

  result = mediaSession_->BeginGetEvent (this, NULL);
  if (FAILED (result))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to IMFMediaSession::BeginGetEvent(): \"%s\", returning\n"),
                ACE_TEXT (Common_Tools::error2String (result).c_str ())));
    return;
  } // end IF
}
void
Stream_CamSave_Stream::stop (bool waitForCompletion_in,
                             bool lockedAccess_in)
{
  STREAM_TRACE (ACE_TEXT ("Stream_CamSave_Stream::stop"));

  if (mediaSession_)
  {
    HRESULT result = mediaSession_->Stop ();
    if (FAILED (result))
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to IMFMediaSession::Stop(): \"%s\", continuing\n"),
                  ACE_TEXT (Common_Tools::error2String (result).c_str ())));
  } // end IF

  inherited::stop (waitForCompletion_in,
                   lockedAccess_in);
}

HRESULT
Stream_CamSave_Stream::QueryInterface (const IID& IID_in,
                                       void** interface_out)
{
  STREAM_TRACE (ACE_TEXT ("Stream_CamSave_Stream::QueryInterface"));

  static const QITAB query_interface_table[] =
  {
    QITABENT (Stream_CamSave_Stream, IMFAsyncCallback),
    { 0 },
  };

  return QISearch (this,
                   query_interface_table,
                   IID_in,
                   interface_out);
}
ULONG
Stream_CamSave_Stream::AddRef ()
{
  STREAM_TRACE (ACE_TEXT ("Stream_CamSave_Stream::AddRef"));

  return InterlockedIncrement (&referenceCount_);
}
ULONG
Stream_CamSave_Stream::Release ()
{
  STREAM_TRACE (ACE_TEXT ("Stream_CamSave_Stream::Release"));

  ULONG count = InterlockedDecrement (&referenceCount_);
  if (count == 0);
  //delete this;

  return count;
}
HRESULT
Stream_CamSave_Stream::GetParameters (DWORD* flags_out,
                                      DWORD* queue_out)
{
  STREAM_TRACE (ACE_TEXT ("Stream_CamSave_Stream::GetParameters"));

  ACE_UNUSED_ARG (flags_out);
  ACE_UNUSED_ARG (queue_out);

  // *NOTE*: "...If you want default values for both parameters, return
  //         E_NOTIMPL. ..."
  return E_NOTIMPL;
}
HRESULT
Stream_CamSave_Stream::Invoke (IMFAsyncResult* result_in)
{
  STREAM_TRACE (ACE_TEXT ("Stream_CamSave_Stream::Invoke"));

  HRESULT result = E_FAIL;
  IMFMediaEvent* media_event_p = NULL;
  MediaEventType event_type = MEUnknown;
  HRESULT status = E_FAIL;
  struct tagPROPVARIANT value;
  PropVariantInit (&value);

  // sanity check(s)
  ACE_ASSERT (result_in);
  ACE_ASSERT (mediaSession_);
  ACE_ASSERT (inherited::sessionData_);

  //Stream_CamSave_SessionData& session_data_r =
  //  const_cast<Stream_CamSave_SessionData&> (inherited::sessionData_->get ());

  result = mediaSession_->EndGetEvent (result_in, &media_event_p);
  if (FAILED (result))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to IMFMediaSession::EndGetEvent(): \"%s\", aborting\n"),
                ACE_TEXT (Common_Tools::error2String (result).c_str ())));
    goto error;
  } // end IF
  result = media_event_p->GetType (&event_type);
  ACE_ASSERT (SUCCEEDED (result));
  result = media_event_p->GetStatus (&status);
  ACE_ASSERT (SUCCEEDED (result));
  result = media_event_p->GetValue (&value);
  ACE_ASSERT (SUCCEEDED (result));
  switch (event_type)
  {
  case MEEndOfPresentation:
  {
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("received MEEndOfPresentation...\n")));
    break;
  }
  case MEError:
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("received MEError: \"%s\"\n"),
                ACE_TEXT (Common_Tools::error2String (status).c_str ())));
    break;
  }
  case MESessionClosed:
  {
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("received MESessionClosed, shutting down...\n")));
    //IMFMediaSource* media_source_p = NULL;
    //if (!Stream_Module_Device_Tools::getMediaSource (mediaSession_,
    //                                                 media_source_p))
    //{
    //  ACE_DEBUG ((LM_ERROR,
    //              ACE_TEXT ("failed to Stream_Module_Device_Tools::getMediaSource(), continuing\n")));
    //  goto continue_;
    //} // end IF
    //ACE_ASSERT (media_source_p);
    //result = media_source_p->Shutdown ();
    //if (FAILED (result))
    //  ACE_DEBUG ((LM_ERROR,
    //              ACE_TEXT ("failed to IMFMediaSource::Shutdown(): \"%s\", continuing\n"),
    //              ACE_TEXT (Common_Tools::error2String (result).c_str ())));
    //media_source_p->Release ();
//continue_:
    // *TODO*: this crashes in CTopoNode::UnlinkInput ()...
    //result = mediaSession_->Shutdown ();
    //if (FAILED (result))
    //  ACE_DEBUG ((LM_ERROR,
    //              ACE_TEXT ("failed to IMFMediaSession::Shutdown(): \"%s\", continuing\n"),
    //              ACE_TEXT (Common_Tools::error2String (result).c_str ())));
    break;
  }
  case MESessionEnded:
  {
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("received MESessionEnded, closing sesion...\n")));
    result = mediaSession_->Close ();
    if (FAILED (result))
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to IMFMediaSession::Close(): \"%s\", continuing\n"),
                  ACE_TEXT (Common_Tools::error2String (result).c_str ())));
    break;
  }
  case MESessionCapabilitiesChanged:
  {
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("received MESessionCapabilitiesChanged...\n")));
    break;
  }
  case MESessionNotifyPresentationTime:
  {
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("received MESessionNotifyPresentationTime...\n")));
    break;
  }
  case MESessionStarted:
  { // status MF_E_INVALIDREQUEST: 0xC00D36B2L
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("received MESessionStarted...\n")));
    break;
  }
  case MESessionStopped:
  { // status MF_E_INVALIDREQUEST: 0xC00D36B2L
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("received MESessionStopped, stopping...\n")));
    if (isRunning ())
      stop (false,
            true);
    break;
  }
  case MESessionTopologySet:
  {
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("received MESessionTopologySet (status was: \"%s\")...\n"),
                ACE_TEXT (Common_Tools::error2String (status).c_str ())));
    break;
  }
  case MESessionTopologyStatus:
  {
    UINT32 attribute_value = 0;
    result = media_event_p->GetUINT32 (MF_EVENT_TOPOLOGY_STATUS,
                                       &attribute_value);
    ACE_ASSERT (SUCCEEDED (result));
    MF_TOPOSTATUS topology_status =
      static_cast<MF_TOPOSTATUS> (attribute_value);
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("received MESessionTopologyStatus: \"%s\"...\n"),
                ACE_TEXT (Stream_Module_Device_Tools::topologyStatusToString (topology_status).c_str ())));
    break;
  }
  default:
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("received unknown/invalid media session event (type was: %d), continuing\n"),
                event_type));
    break;
  }
  } // end SWITCH
  PropVariantClear (&value);
  media_event_p->Release ();

  result = mediaSession_->BeginGetEvent (this, NULL);
  if (FAILED (result))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to IMFMediaSession::BeginGetEvent(): \"%s\", aborting\n"),
                ACE_TEXT (Common_Tools::error2String (result).c_str ())));
    goto error;
  } // end IF

  return S_OK;

error:
  if (media_event_p)
    media_event_p->Release ();
  PropVariantClear (&value);

  return E_FAIL;
}
#endif

bool
Stream_CamSave_Stream::load (Stream_ModuleList_t& modules_out)
{
  STREAM_TRACE (ACE_TEXT ("Stream_CamSave_Stream::load"));

  // initialize return value(s)
  for (Stream_ModuleListIterator_t iterator = modules_out.begin ();
       iterator != modules_out.end ();
       iterator++)
    delete *iterator;
  modules_out.clear ();

  // remember the 'owned' ones
  // *TODO*: clean this up
  // *NOTE*: one problem is that all modules which have NOT enqueued onto the
  //         stream (e.g. because initialize() failed...) need to be explicitly
  //         close()d
  modules_out.push_front (&source_);
  modules_out.push_front (&runtimeStatistic_);
  modules_out.push_front (&display_);
  //modules_out.push_front (&displayNull_);
  modules_out.push_front (&encoder_);
  modules_out.push_front (&fileWriter_);

  return true;
}

bool
Stream_CamSave_Stream::initialize (const Stream_CamSave_StreamConfiguration& configuration_in,
                                   bool setupPipeline_in,
                                   bool resetSessionData_in)
{
  STREAM_TRACE (ACE_TEXT ("Stream_CamSave_Stream::initialize"));

  // sanity check(s)
  ACE_ASSERT (!isRunning ());
  ACE_ASSERT (configuration_in.moduleHandlerConfiguration);

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
  ACE_ASSERT (inherited::sessionData_);
  Stream_CamSave_SessionData& session_data_r =
    const_cast<Stream_CamSave_SessionData&> (inherited::sessionData_->get ());
  // *TODO*: remove type inferences
  session_data_r.sessionID = ++Stream_CamSave_Stream::currentSessionID;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
  ACE_ASSERT (configuration_in.moduleHandlerConfiguration->fileDescriptor != -1);
  if (!Stream_Module_Device_Tools::getCaptureFormat (configuration_in.moduleHandlerConfiguration->fileDescriptor,
                                                     session_data_r.format))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Stream_Module_Device_Tools::getCaptureFormat(%d), aborting\n"),
                configuration_in.moduleHandlerConfiguration->fileDescriptor));
    return false;
  } // end IF
  if (!Stream_Module_Device_Tools::getFrameRate (configuration_in.moduleHandlerConfiguration->fileDescriptor,
                                                 session_data_r.frameRate))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Stream_Module_Device_Tools::getFrameRate(%d), aborting\n"),
                configuration_in.moduleHandlerConfiguration->fileDescriptor));
    return false;
  } // end IF
#endif
  session_data_r.targetFileName =
    configuration_in.moduleHandlerConfiguration->targetFileName;

  // ---------------------------------------------------------------------------
  // sanity check(s)
  ACE_ASSERT (configuration_in.moduleConfiguration);

  // ******************* File Writer ************************
  fileWriter_.initialize (*configuration_in.moduleConfiguration);
  Stream_CamSave_Module_FileWriter* fileWriter_impl_p =
    dynamic_cast<Stream_CamSave_Module_FileWriter*> (fileWriter_.writer ());
  if (!fileWriter_impl_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("dynamic_cast<Stream_CamSave_Module_FileWriter> failed, aborting\n")));
    return false;
  } // end IF
  if (!fileWriter_impl_p->initialize (*configuration_in.moduleHandlerConfiguration))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to initialize module: \"%s\", aborting\n"),
                fileWriter_.name ()));
    return false;
  } // end IF

  // ******************* AVI Encoder ************************
  encoder_.initialize (*configuration_in.moduleConfiguration);
  Stream_CamSave_Module_AVIEncoder_WriterTask_t* encoder_impl_p =
    dynamic_cast<Stream_CamSave_Module_AVIEncoder_WriterTask_t*> (encoder_.writer ());
  if (!encoder_impl_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("dynamic_cast<Stream_CamSave_Module_AVIEncoder_WriterTask_T> failed, aborting\n")));
    return false;
  } // end IF
  if (!encoder_impl_p->initialize (*configuration_in.moduleHandlerConfiguration))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to initialize module: \"%s\", aborting\n"),
                encoder_.name ()));
    return false;
  } // end IF

  // ******************* Display ************************
  display_.initialize (*configuration_in.moduleConfiguration);
  Stream_CamSave_Module_Display* display_impl_p =
    dynamic_cast<Stream_CamSave_Module_Display*> (display_.writer ());
  if (!display_impl_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("dynamic_cast<Stream_CamSave_Module_Display> failed, aborting\n")));
    return false;
  } // end IF
  if (!display_impl_p->initialize (*configuration_in.moduleHandlerConfiguration))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to initialize module: \"%s\", aborting\n"),
                display_.name ()));
    return false;
  } // end IF

  // ******************* Runtime Statistics ************************
  runtimeStatistic_.initialize (*configuration_in.moduleConfiguration);
  Stream_CamSave_Module_Statistic_WriterTask_t* runtimeStatistic_impl_p =
      dynamic_cast<Stream_CamSave_Module_Statistic_WriterTask_t*> (runtimeStatistic_.writer ());
  if (!runtimeStatistic_impl_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("dynamic_cast<Stream_CamSave_Module_RuntimeStatistic> failed, aborting\n")));
    return false;
  } // end IF
  if (!runtimeStatistic_impl_p->initialize (configuration_in.statisticReportingInterval, // reporting interval (seconds)
                                            true,                                        // push 1-second interval statistic messages downstream ?
                                            configuration_in.printFinalReport,           // print final report ?
                                            configuration_in.messageAllocator))          // message allocator handle
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to initialize module: \"%s\", aborting\n"),
                runtimeStatistic_.name ()));
    return false;
  } // end IF

  // ******************* Camera Source ************************
  Stream_CamSave_Module_Source* source_impl_p =
    dynamic_cast<Stream_CamSave_Module_Source*> (source_.writer ());
  if (!source_impl_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("dynamic_cast<Strean_CamSave_Module_CamSource> failed, aborting\n")));
    return false;
  } // end IF
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  //std::list<std::wstring> filter_pipeline;
  bool graph_loaded = false;
  bool COM_initialized = false;
  //struct _AllocatorProperties allocator_properties;
  //IAMBufferNegotiation* buffer_negotiation_p = NULL;
  HRESULT result_2 = E_FAIL;
  IMFTopology* topology_p = NULL;

  //if (configuration_in.moduleHandlerConfiguration->builder)
  //{
  //  // *NOTE*: Stream_Module_Device_Tools::loadRendererGraph() resets the graph
  //  //         (see below)
  //  //if (!Stream_Module_Device_Tools::resetDeviceGraph (configuration_in.moduleHandlerConfiguration->builder))
  //  //{
  //  //  ACE_DEBUG ((LM_ERROR,
  //  //              ACE_TEXT ("failed to Stream_Module_Device_Tools::resetDeviceGraph(): \"%s\", aborting\n")));
  //  //  return false;
  //  //} // end IF

  //  //if (!Stream_Module_Device_Tools::getBufferNegotiation (configuration_in.moduleHandlerConfiguration->builder,
  //  //                                                       buffer_negotiation_p))
  //  //{
  //  //  ACE_DEBUG ((LM_ERROR,
  //  //              ACE_TEXT ("failed to Stream_Module_Device_Tools::getBufferNegotiation(): \"%s\", aborting\n")));
  //  //  return false;
  //  //} // end IF
  //  //ACE_ASSERT (buffer_negotiation_p);

  //  goto continue_;
  //} // end IF
  //else
  //{
    result_2 = CoInitializeEx (NULL,
                               (COINIT_MULTITHREADED    |
                                COINIT_DISABLE_OLE1DDE  |
                                COINIT_SPEED_OVER_MEMORY));
    if (FAILED (result_2))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to CoInitializeEx(): \"%s\", aborting\n"),
                  ACE_TEXT (Common_Tools::error2String (result_2).c_str ())));
      return false;
    } // end IF
    COM_initialized = true;

    //IAMStreamConfig* stream_config_p = NULL;
    //if (!Stream_Module_Device_Tools::loadDeviceGraph (configuration_in.moduleHandlerConfiguration->device,
    //                                                  configuration_in.moduleHandlerConfiguration->builder,
    //                                                  buffer_negotiation_p,
    //                                                  stream_config_p))
    //{
    //  ACE_DEBUG ((LM_ERROR,
    //              ACE_TEXT ("failed to Stream_Module_Device_Tools::loadDeviceGraph(\"%s\"), aborting\n"),
    //              ACE_TEXT (configuration_in.moduleHandlerConfiguration->device.c_str ())));
    //  goto error;
    //} // end IF
    //ACE_ASSERT (configuration_in.moduleHandlerConfiguration->builder);
    //ACE_ASSERT (buffer_negotiation_p);
    //ACE_ASSERT (stream_config_p);

    // sanity check(s)
    //ACE_ASSERT (!session_data_r.direct3DDevice);

    //IDirect3DDeviceManager9* direct3D_manager_p = NULL;
    //struct _D3DPRESENT_PARAMETERS_ d3d_presentation_parameters;
    //if (!Stream_Module_Device_Tools::getDirect3DDevice (configuration_in.moduleHandlerConfiguration->window,
    //                                                    configuration_in.moduleHandlerConfiguration->format,
    //                                                    session_data_r.direct3DDevice,
    //                                                    d3d_presentation_parameters,
    //                                                    direct3D_manager_p,
    //                                                    session_data_r.resetToken))
    //{
    //  ACE_DEBUG ((LM_ERROR,
    //              ACE_TEXT ("failed to Stream_Module_Device_Tools::getDirect3DDevice(), aborting\n")));
    //  goto error;
    //} // end IF
    //ACE_ASSERT (direct3D_manager_p);

    //WCHAR* symbolic_link_p = NULL;
    //UINT32 symbolic_link_size = 0;
    //IMFMediaSource* media_source_p = NULL;
    //if (!Stream_Module_Device_Tools::getMediaSource (configuration_in.moduleHandlerConfiguration->device,
    //                                                 media_source_p,
    //                                                 symbolic_link_p,
    //                                                 symbolic_link_size))
    //{
    //  ACE_DEBUG ((LM_ERROR,
    //              ACE_TEXT ("failed to Stream_Module_Device_Tools::getMediaSource(\"%s\"), aborting\n"),
    //              ACE_TEXT (configuration_in.moduleHandlerConfiguration->device.c_str ())));
    //  goto error;
    //} // end IF
    //CoTaskMemFree (symbolic_link_p);
    //symbolic_link_p = NULL;
    //symbolic_link_size = 0;
    //ACE_ASSERT (media_source_p);
    // *TODO*: using the Direct3D device manager (used by the EVR renderer) is
    //         currently broken
    //         --> pass NULL and use a different visualization module (e.g. the
    //             Direct3D module)
    //direct3D_manager_p->Release ();
    //direct3D_manager_p = NULL;
    //if (!Stream_Module_Device_Tools::getSourceReader (configuration_in.moduleHandlerConfiguration->mediaSource,
    //                                                  symbolic_link_p,
    //                                                  symbolic_link_size,
    //                                                  direct3D_manager_p,
    //                                                  source_impl_p,
    //                                                  Stream_Module_Device_Tools::isChromaLuminance (configuration_in.moduleHandlerConfiguration->format),
    //                                                  configuration_in.moduleHandlerConfiguration->sourceReader))
    //{
    //  ACE_DEBUG ((LM_ERROR,
    //              ACE_TEXT ("failed to Stream_Module_Device_Tools::getSourceReader(\"%s\"), aborting\n"),
    //              ACE_TEXT (configuration_in.moduleHandlerConfiguration->device.c_str ())));

    //  // clean up
    //  configuration_in.moduleHandlerConfiguration->mediaSource->Release ();
    //  configuration_in.moduleHandlerConfiguration->mediaSource = NULL;

    //  goto error;
    //} // end IF
    if (!Stream_Module_Device_Tools::loadRendererTopology (configuration_in.moduleHandlerConfiguration->device,
                                                           configuration_in.moduleHandlerConfiguration->format,
                                                           source_impl_p,
                                                           NULL,
                                                           //configuration_in.moduleHandlerConfiguration->window,
                                                           configuration_in.moduleHandlerConfiguration->sampleGrabberNodeId,
                                                           configuration_in.moduleHandlerConfiguration->rendererNodeId,
                                                           topology_p))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to Stream_Module_Device_Tools::loadRendererTopology(\"%s\"), aborting\n"),
                  ACE_TEXT (configuration_in.moduleHandlerConfiguration->device.c_str ())));
      goto error;
    } // end IF
    //direct3D_manager_p->Release ();
    //direct3D_manager_p = NULL;
    ACE_ASSERT (topology_p);
    graph_loaded = true;

    if (!Stream_Module_Device_Tools::setCaptureFormat (topology_p,
                                                       configuration_in.moduleHandlerConfiguration->format))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to Stream_Module_Device_Tools::setCaptureFormat(), aborting\n")));
      goto error;
    } // end IF
    if (_DEBUG)
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("capture format: \"%s\"...\n"),
                  ACE_TEXT (Stream_Module_Device_Tools::mediaTypeToString (configuration_in.moduleHandlerConfiguration->format).c_str ())));

    // clean up
    //stream_config_p->Release ();

    //if (_DEBUG)
    //{
    //  std::string log_file_name =
    //    Common_File_Tools::getLogDirectory (std::string (),
    //                                        0);
    //  log_file_name += ACE_DIRECTORY_SEPARATOR_STR;
    //  log_file_name += MODULE_DEV_DIRECTSHOW_LOGFILE_NAME;
    //  Stream_Module_Device_Tools::debug (configuration_in.moduleHandlerConfiguration->builder,
    //                                     log_file_name);
    //} // end IF
  //} // end ELSE

//continue_:
  if (session_data_r.format)
  {
    session_data_r.format->Release ();
    session_data_r.format = NULL;
  } // end IF
  //IMediaFilter* media_filter_p = NULL;

  //if (!Stream_Module_Device_Tools::getCaptureFormat (configuration_in.moduleHandlerConfiguration->builder,
  //                                                   session_data_r.format))
  //{
  //  ACE_DEBUG ((LM_ERROR,
  //              ACE_TEXT ("failed to Stream_Module_Device_Tools::getCaptureFormat(), aborting\n")));
  //  goto error;
  //} // end IF
  //ACE_ASSERT (session_data_r.format);
  //if (!Stream_Module_Device_Tools::loadRendererGraph (*session_data_r.format,
  //                                                    configuration_in.moduleHandlerConfiguration->window,
  //                                                    configuration_in.moduleHandlerConfiguration->builder,
  //                                                    filter_pipeline))
  //{
  //  ACE_DEBUG ((LM_ERROR,
  //              ACE_TEXT ("failed to Stream_Module_Device_Tools::loadRendererGraph(), aborting\n")));

  //  // clean up
  //  Stream_Module_Device_Tools::deleteMediaType (session_data_r.format);
  //  session_data_r.format = NULL;

  //  goto error;
  //} // end IF
  //Stream_Module_Device_Tools::deleteMediaType (session_data_r.format);
  //filter_pipeline.push_front (MODULE_DEV_CAM_WIN32_FILTER_NAME_CAPTURE_VIDEO);

  //IBaseFilter* filter_p = NULL;
  //result_2 =
  //  configuration_in.moduleHandlerConfiguration->builder->FindFilterByName (MODULE_DEV_CAM_WIN32_FILTER_NAME_GRAB,
  //                                                                          &filter_p);
  //if (FAILED (result_2))
  //{
  //  ACE_DEBUG ((LM_ERROR,
  //              ACE_TEXT ("failed to IGraphBuilder::FindFilterByName(\"%s\"): \"%s\", aborting\n"),
  //              ACE_TEXT_WCHAR_TO_TCHAR (MODULE_DEV_CAM_WIN32_FILTER_NAME_GRAB),
  //              ACE_TEXT (Common_Tools::error2String (result_2).c_str ())));
  //  goto error;
  //} // end IF
  //ACE_ASSERT (filter_p);
  //ISampleGrabber* isample_grabber_p = NULL;
  //result_2 = filter_p->QueryInterface (IID_ISampleGrabber,
  //                                     (void**)&isample_grabber_p);
  //if (FAILED (result_2))
  //{
  //  ACE_DEBUG ((LM_ERROR,
  //              ACE_TEXT ("failed to IBaseFilter::QueryInterface(IID_ISampleGrabber): \"%s\", aborting\n"),
  //              ACE_TEXT (Common_Tools::error2String (result_2).c_str ())));
  //  goto error;
  //} // end IF
  //ACE_ASSERT (isample_grabber_p);
  //filter_p->Release ();
  //filter_p = NULL;

  //result_2 = isample_grabber_p->SetBufferSamples (false);
  //if (FAILED (result_2))
  //{
  //  ACE_DEBUG ((LM_ERROR,
  //              ACE_TEXT ("failed to ISampleGrabber::SetBufferSamples(false): \"%s\", aborting\n"),
  //              ACE_TEXT (Common_Tools::error2String (result_2).c_str ())));
  //  goto error;
  //} // end IF

  //result_2 = isample_grabber_p->SetCallback (source_impl_p, 0);
  //if (FAILED (result_2))
  //{
  //  ACE_DEBUG ((LM_ERROR,
  //              ACE_TEXT ("failed to ISampleGrabber::SetCallback(): \"%s\", aborting\n"),
  //              ACE_TEXT (Common_Tools::error2String (result_2).c_str ())));
  //  goto error;
  //} // end IF
  //isample_grabber_p->Release ();
  //isample_grabber_p = NULL;

  //ACE_ASSERT (buffer_negotiation_p);
  //ACE_OS::memset (&allocator_properties, 0, sizeof (allocator_properties));
  //allocator_properties.cbAlign = -1; // <-- use default
  //allocator_properties.cbBuffer = -1; // <-- use default
  //allocator_properties.cbPrefix = -1; // <-- use default
  //allocator_properties.cBuffers =
  //  MODULE_DEV_CAM_DIRECTSHOW_DEFAULT_DEVICE_BUFFERS;
  //result_2 =
  //    buffer_negotiation_p->SuggestAllocatorProperties (&allocator_properties);
  //if (FAILED (result_2))
  //{
  //  ACE_DEBUG ((LM_ERROR,
  //              ACE_TEXT ("failed to IAMBufferNegotiation::SuggestAllocatorProperties(): \"%s\", aborting\n"),
  //              ACE_TEXT (Common_Tools::error2String (result_2).c_str ())));
  //  goto error;
  //} // end IF

  //if (!Stream_Module_Device_Tools::connect (configuration_in.moduleHandlerConfiguration->builder,
  //                                          filter_pipeline))
  //{
  //  ACE_DEBUG ((LM_ERROR,
  //              ACE_TEXT ("failed to Stream_Module_Device_Tools::connect(), aborting\n")));
  //  goto error;
  //} // end IF
  //// *NOTE*: for some (unknown) reason, connect()ing the sample grabber to the
  ////         null renderer 'breaks' the connection between the AVI decompressor
  ////         and the sample grabber (go ahead, try it in with graphedit.exe)
  ////         --> reconnect the AVI decompressor to the (connected) sample
  ////             grabber; this seems to work
  //if (!Stream_Module_Device_Tools::connected (configuration_in.moduleHandlerConfiguration->builder))
  //{
  //  ACE_DEBUG ((LM_DEBUG,
  //              ACE_TEXT ("reconnecting...\n")));

  //  if (!Stream_Module_Device_Tools::connectFirst (configuration_in.moduleHandlerConfiguration->builder))
  //  {
  //    ACE_DEBUG ((LM_ERROR,
  //                ACE_TEXT ("failed to Stream_Module_Device_Tools::connectFirst(), aborting\n")));
  //    goto error;
  //  } // end IF
  //} // end IF
  //ACE_ASSERT (Stream_Module_Device_Tools::connected (configuration_in.moduleHandlerConfiguration->builder));

  //// debug info
  ////ACE_OS::memset (&allocator_properties, 0, sizeof (allocator_properties));
  ////result_2 =
  ////    buffer_negotiation_p->GetAllocatorProperties (&allocator_properties);
  ////if (FAILED (result_2)) // E_FAIL (0x80004005)
  ////{
  ////  ACE_DEBUG ((LM_ERROR,
  ////              ACE_TEXT ("failed to IAMBufferNegotiation::GetAllocatorProperties(): \"%s\", aborting\n"),
  ////              ACE_TEXT (Common_Tools::error2String (result_2).c_str ())));
  ////  goto error;
  ////} // end IF
  ////ACE_DEBUG ((LM_DEBUG,
  ////            ACE_TEXT ("allocator properties (buffers/size/alignment/prefix): %d/%d/%d/%d\n"),
  ////            allocator_properties.cBuffers,
  ////            allocator_properties.cbBuffer,
  ////            allocator_properties.cbAlign,
  ////            allocator_properties.cbPrefix));
  //buffer_negotiation_p->Release ();
  //buffer_negotiation_p = NULL;

  ACE_ASSERT (!session_data_r.format);
  //if (!Stream_Module_Device_Tools::getOutputFormat (configuration_in.moduleHandlerConfiguration->builder,
  if (!Stream_Module_Device_Tools::getOutputFormat (topology_p,
                                                    configuration_in.moduleHandlerConfiguration->sampleGrabberNodeId,
                                                    session_data_r.format))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Stream_Module_Device_Tools::getOutputFormat(), aborting\n")));
    goto error;
  } // end IF
  ACE_ASSERT (session_data_r.format);

  //HRESULT result = E_FAIL;
  if (mediaSession_)
  {
    // *TODO*: this crashes in CTopoNode::UnlinkInput ()...
    //result = mediaSession_->Shutdown ();
    //if (FAILED (result))
    //  ACE_DEBUG ((LM_ERROR,
    //              ACE_TEXT ("failed to IMFMediaSession::Shutdown(): \"%s\", continuing\n"),
    //              ACE_TEXT (Common_Tools::error2String (result).c_str ())));
    mediaSession_->Release ();
    mediaSession_ = NULL;
  } // end IF
  ULONG reference_count = 0;
  if (configuration_in.moduleHandlerConfiguration->session)
  {
    reference_count =
      configuration_in.moduleHandlerConfiguration->session->AddRef ();
    mediaSession_ = configuration_in.moduleHandlerConfiguration->session;

    if (!Stream_Module_Device_Tools::clear (mediaSession_))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to Stream_Module_Device_Tools::clear(), aborting\n")));
      goto error;
    } // end IF
  } // end IF

  if (!Stream_Module_Device_Tools::setTopology (topology_p,
                                                mediaSession_,
                                                true))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Stream_Module_Device_Tools::setTopology(), aborting\n")));
    goto error;
  } // end IF
  topology_p->Release ();

  if (!configuration_in.moduleHandlerConfiguration->session)
  {
    reference_count = mediaSession_->AddRef ();
    configuration_in.moduleHandlerConfiguration->session = mediaSession_;
  } // end IF

  //result_2 =
  //  configuration_in.moduleHandlerConfiguration->builder->QueryInterface (IID_IMediaFilter,
  //                                                                        (void**)&media_filter_p);
  //if (FAILED (result_2))
  //{
  //  ACE_DEBUG ((LM_ERROR,
  //              ACE_TEXT ("failed to IGraphBuilder::QueryInterface(IID_IMediaFilter): \"%s\", aborting\n"),
  //              ACE_TEXT (Common_Tools::error2String (result_2).c_str ())));
  //  goto error;
  //} // end IF
  //ACE_ASSERT (media_filter_p);
  //result_2 = media_filter_p->SetSyncSource (NULL);
  //if (FAILED (result_2))
  //{
  //  ACE_DEBUG ((LM_ERROR,
  //              ACE_TEXT ("failed to IMediaFilter::SetSyncSource(): \"%s\", aborting\n"),
  //              ACE_TEXT (Common_Tools::error2String (result_2).c_str ())));
  //  goto error;
  //} // end IF
  //media_filter_p->Release ();
  //media_filter_p = NULL;
#endif

  source_.initialize (*configuration_in.moduleConfiguration);
  if (!source_impl_p->initialize (*configuration_in.moduleHandlerConfiguration))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to initialize module: \"%s\", aborting\n"),
                source_.name ()));
    goto error;
  } // end IF
  if (!source_impl_p->initialize (inherited::state_))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to initialize module: \"%s\", aborting\n"),
                source_.name ()));
    goto error;
  } // end IF
  // *NOTE*: push()ing the module will open() it
  //         --> set the argument that is passed along (head module expects a
  //             handle to the session data)
  source_.arg (inherited::sessionData_);

  if (setupPipeline_in)
    if (!inherited::setup (NULL))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to setup pipeline, aborting\n")));
      goto error;
    } // end IF

  // -------------------------------------------------------------

  // OK: all went well
  inherited::isInitialized_ = true;

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  if (COM_initialized)
    CoUninitialize ();
#endif

  return true;

error:
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  //if (buffer_negotiation_p)
  //  buffer_negotiation_p->Release ();
  //if (isample_grabber_p)
  //  isample_grabber_p->Release ();
  //if (filter_p)
  //  filter_p->Release ();
  //if (media_filter_p)
  //  media_filter_p->Release ();
  //if (direct3D_manager_p)
  //  direct3D_manager_p->Release ();
  if (topology_p)
  {
    Stream_Module_Device_Tools::dump (topology_p);
    topology_p->Release ();
  } // end IF
  if (session_data_r.direct3DDevice)
  {
    session_data_r.direct3DDevice->Release ();
    session_data_r.direct3DDevice = NULL;
  } // end IF
  if (session_data_r.format)
  {
    //Stream_Module_Device_Tools::deleteMediaType (session_data_r.format);
    session_data_r.format->Release ();
    session_data_r.format = NULL;
  } // end IF
  session_data_r.resetToken = 0;
  if (session_data_r.session)
  {
    session_data_r.session->Release ();
    session_data_r.session = NULL;
  } // end IF
  if (mediaSession_)
  {
    mediaSession_->Release ();
    mediaSession_ = NULL;
  } // end IF

  if (COM_initialized)
    CoUninitialize ();
#endif

  return false;
}

bool
Stream_CamSave_Stream::collect (Stream_CamSave_StatisticData& data_out)
{
  STREAM_TRACE (ACE_TEXT ("Stream_CamSave_Stream::collect"));

  // sanity check(s)
  ACE_ASSERT (inherited::sessionData_);

  int result = -1;

  Stream_CamSave_Module_Statistic_WriterTask_t* runtimeStatistic_impl =
    dynamic_cast<Stream_CamSave_Module_Statistic_WriterTask_t*> (runtimeStatistic_.writer ());
  if (!runtimeStatistic_impl)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("dynamic_cast<Stream_CamSave_Module_Statistic_WriterTask_t> failed, aborting\n")));
    return false;
  } // end IF

  // synch access
  Stream_CamSave_SessionData& session_data_r =
    const_cast<Stream_CamSave_SessionData&> (inherited::sessionData_->get ());
  if (session_data_r.lock)
  {
    result = session_data_r.lock->acquire ();
    if (result == -1)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_SYNCH_MUTEX::acquire(): \"%m\", aborting\n")));
      return false;
    } // end IF
  } // end IF

  session_data_r.currentStatistic.timeStamp = COMMON_TIME_NOW;

  // delegate to the statistic module
  bool result_2 = false;
  try
  {
    result_2 = runtimeStatistic_impl->collect (data_out);
  }
  catch (...)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("caught exception in Common_IStatistic_T::collect(), continuing\n")));
  }
  if (!result)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_IStatistic_T::collect(), aborting\n")));
  else
    session_data_r.currentStatistic = data_out;

  if (session_data_r.lock)
  {
    result = session_data_r.lock->release ();
    if (result == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_SYNCH_MUTEX::release(): \"%m\", continuing\n")));
  } // end IF

  return result_2;
}

void
Stream_CamSave_Stream::report () const
{
  STREAM_TRACE (ACE_TEXT ("Stream_CamSave_Stream::report"));

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
