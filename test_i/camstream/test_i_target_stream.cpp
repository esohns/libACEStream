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
#include "test_i_target_stream.h"

#include "ace/Log_Msg.h"

#include "stream_macros.h"

#include "stream_dev_defines.h"
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include "stream_dev_directshow_tools.h"
#include "stream_dev_mediafoundation_tools.h"
#else
#include "stream_dev_tools.h"
#endif // ACE_WIN32 || ACE_WIN64

#include "stream_misc_defines.h"

#include "test_i_target_session_message.h"
#include "test_i_common_modules.h"
#include "test_i_source_stream.h"

#include "test_i_target_message.h"

#if defined (ACE_WIN32) || defined (ACE_WIN64)
Test_I_Target_DirectShow_Stream::Test_I_Target_DirectShow_Stream ()
 : inherited ()
{
  STREAM_TRACE (ACE_TEXT ("Test_I_Target_DirectShow_Stream::Test_I_Target_DirectShow_Stream"));

}

Test_I_Target_DirectShow_Stream::~Test_I_Target_DirectShow_Stream ()
{
  STREAM_TRACE (ACE_TEXT ("Test_I_Target_DirectShow_Stream::~Test_I_Target_DirectShow_Stream"));

  inherited::shutdown ();
}

bool
Test_I_Target_DirectShow_Stream::load (Stream_ILayout* layout_in,
                                       bool& delete_out)
{
  STREAM_TRACE (ACE_TEXT ("Test_I_Target_DirectShow_Stream::load"));

  if (!inherited::load (layout_in,
                        delete_out))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: failed to Stream_Module_Net_IO_Stream_T::load(), aborting\n")));
    return false;
  } // end IF
  ACE_ASSERT (delete_out);

  Stream_Module_t* module_p = NULL;
  //Test_I_Target_DirectShow_Module_AVIDecoder_Module            decoder_;
  ACE_NEW_RETURN (module_p,
                  Test_I_Target_DirectShow_Splitter_Module (this,
                                                            ACE_TEXT_ALWAYS_CHAR (STREAM_MISC_SPLITTER_DEFAULT_NAME_STRING)),
                  false);
  ACE_ASSERT (module_p);
  layout_in->append (module_p, NULL, 0);
  //module_p = NULL;
  //ACE_NEW_RETURN (module_p,
  //                Test_I_Target_DirectShow_StatisticReport_Module (this,
  //                                                                 ACE_TEXT_ALWAYS_CHAR (MODULE_STAT_REPORT_DEFAULT_NAME_STRING)),
  //                false);
  //ACE_ASSERT (module_p);
  //layout_in->append (module_p, NULL, 0);
  module_p = NULL;
  ACE_NEW_RETURN (module_p,
                  Test_I_Target_DirectShow_Display_Module (this,
                                                           ACE_TEXT_ALWAYS_CHAR (STREAM_VIS_DIRECTSHOW_DEFAULT_NAME_STRING)),
                  false);
  ACE_ASSERT (module_p);
  layout_in->append (module_p, NULL, 0);

  delete_out = true;

  return true;
}

bool
Test_I_Target_DirectShow_Stream::initialize (const CONFIGURATION_T& configuration_in,
                                             ACE_HANDLE handle_in)
{
  STREAM_TRACE (ACE_TEXT ("Test_I_Target_DirectShow_Stream::initialize"));

  // sanity check(s)
  ACE_ASSERT (!isRunning ());

  bool result = false;
  bool setup_pipeline = configuration_in.configuration_.setupPipeline;
  bool reset_setup_pipeline = false;
  inherited::CONFIGURATION_T::ITERATOR_T iterator;

  // allocate a new session state, reset stream
  const_cast<inherited::CONFIGURATION_T&> (configuration_in).configuration_.setupPipeline =
    false;
  reset_setup_pipeline = true;
  if (!inherited::initialize (configuration_in,
                              handle_in))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: failed to Stream_Module_Net_IO_Stream_T::initialize(), aborting\n"),
                ACE_TEXT (stream_name_string_)));
    goto error;
  } // end IF
  const_cast<inherited::CONFIGURATION_T&> (configuration_in).configuration_.setupPipeline =
    setup_pipeline;
  reset_setup_pipeline = false;
  ACE_ASSERT (inherited::sessionData_);
  Test_I_Target_DirectShow_SessionData& session_data_r =
    const_cast<Test_I_Target_DirectShow_SessionData&> (inherited::sessionData_->getR ());
  // *TODO*: remove type inferences
  session_data_r.lock = &(inherited::sessionDataLock_);
  inherited::state_.sessionData = &session_data_r;
  iterator =
    const_cast<inherited::CONFIGURATION_T&> (configuration_in).find (ACE_TEXT_ALWAYS_CHAR (""));
  ACE_ASSERT (iterator != configuration_in.end ());
  struct Test_I_Target_DirectShow_ModuleHandlerConfiguration* configuration_p =
    dynamic_cast<struct Test_I_Target_DirectShow_ModuleHandlerConfiguration*> (&(*iterator).second.second);
  ACE_ASSERT (configuration_p);
  //session_data_r.sessionID = configuration_in.sessionID;
  //session_data_r.targetFileName = configuration_p->targetFileName;

  //ACE_ASSERT (configuration_in.moduleConfiguration);
  //  configuration_in.moduleConfiguration.streamState = &state_;
  //configuration_p->stateMachineLock = inherited::state_.stateMachineLock;

  // ---------------------------------------------------------------------------

  // ******************* Display Handler ***************************************
  Stream_Module_t* module_p =
    const_cast<Stream_Module_t*> (inherited::find (ACE_TEXT_ALWAYS_CHAR (STREAM_VIS_DIRECTSHOW_DEFAULT_NAME_STRING)));
  if (!module_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to retrieve \"%s\" module handle, aborting\n"),
                ACE_TEXT (STREAM_VIS_DIRECTSHOW_DEFAULT_NAME_STRING)));
    goto error;
  } // end IF

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  Test_I_Target_DirectShow_Display* directshow_display_impl_p =
    dynamic_cast<Test_I_Target_DirectShow_Display*> (module_p->writer ());
  if (!directshow_display_impl_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("dynamic_cast<Test_I_Target_DirectShow_Display*> failed, aborting\n")));
    goto error;
  } // end IF

  //// sanity check(s)
  //// *TODO*: remove type inference
  //ACE_ASSERT (configuration_in.moduleHandlerConfiguration);
  //ACE_ASSERT (configuration_in.moduleHandlerConfiguration->format);

  //if (configuration_in.useMediaFoundation)
  //{
  //  // sanity check(s)
  //  ACE_ASSERT (mediafoundation_display_impl_p);

  //  if (!mediafoundation_display_impl_p->initialize (configuration_in.moduleHandlerConfiguration->format))
  //  {
  //    ACE_DEBUG ((LM_ERROR,
  //                ACE_TEXT ("failed to pre-initialize \"%s\" module, aborting\n"),
  //                ACE_TEXT ("Display")));
  //    return false;
  //  } // end IF
  //} // end IF
  //else
  //{
  //  // sanity check(s)
  //  ACE_ASSERT (directshow_display_impl_p);

  //  if (!directshow_display_impl_p->initialize (*configuration_in.moduleHandlerConfiguration->format))
  //  {
  //    ACE_DEBUG ((LM_ERROR,
  //                ACE_TEXT ("failed to pre-initialize \"%s\" module, aborting\n"),
  //                ACE_TEXT ("Display")));
  //    return false;
  //  } // end IF
  //} // end IF

  // ---------------------------------------------------------------------------

  //// sanity check(s)
  //ACE_ASSERT (configuration_in.moduleHandlerConfiguration);

  //struct _AllocatorProperties allocator_properties;
  //IAMBufferNegotiation* buffer_negotiation_p = NULL;
  //bool COM_initialized = false;
  //bool release_builder = false;
  //HRESULT result = E_FAIL;
  //ULONG reference_count = 0;
  //IAMStreamConfig* stream_config_p = NULL;
  //IMediaFilter* media_filter_p = NULL;
  ////IDirect3DDeviceManager9* direct3D_manager_p = NULL;
  ////struct _D3DPRESENT_PARAMETERS_ d3d_presentation_parameters;
  //std::wstring filter_name;
  //Stream_Module_Device_DirectShow_Graph_t graph_configuration;
  //struct Stream_Module_Device_DirectShow_GraphEntry graph_entry;
  //IBaseFilter* filter_p = NULL;
  //ISampleGrabber* isample_grabber_p = NULL;
  //std::string log_file_name;

  //result = CoInitializeEx (NULL,
  //                         (COINIT_MULTITHREADED     |
  //                          COINIT_DISABLE_OLE1DDE   |
  //                          COINIT_SPEED_OVER_MEMORY));
  //if (FAILED (result))
  //{
  //  ACE_DEBUG ((LM_ERROR,
  //              ACE_TEXT ("failed to CoInitializeEx(): \"%s\", aborting\n"),
  //              ACE_TEXT (Common_Error_Tools::errorToString (result).c_str ())));
  //  return false;
  //} // end IF
  //COM_initialized = true;

  //if (configuration_in.moduleHandlerConfiguration->graphBuilder)
  //{
  //  reference_count =
  //    configuration_in.moduleHandlerConfiguration->graphBuilder->AddRef ();
  //  graphBuilder_ = configuration_in.moduleHandlerConfiguration->graphBuilder;

  //  if (!Stream_Module_Device_DirectShow_Tools::clear (graphBuilder_))
  //  {
  //    ACE_DEBUG ((LM_ERROR,
  //                ACE_TEXT ("failed to Stream_Module_Device_DirectShow_Tools::resetDeviceGraph(): \"%s\", aborting\n")));
  //    goto error;
  //  } // end IF

  //  if (!Stream_Module_Device_DirectShow_Tools::getBufferNegotiation (graphBuilder_,
  //                                                                    filter_name,
  //                                                                    buffer_negotiation_p))
  //  {
  //    ACE_DEBUG ((LM_ERROR,
  //                ACE_TEXT ("failed to Stream_Module_Device_DirectShow_Tools::getBufferNegotiation(), aborting\n")));
  //    goto error;
  //  } // end IF
  //  ACE_ASSERT (buffer_negotiation_p);

  //  goto continue_;
  //} // end IF

  //if (!IsEqualGUID (configuration_in.moduleHandlerConfiguration->filterIdentifier,
  //                  GUID_NULL))
  //{
  //  result =
  //    CoCreateInstance (configuration_in.moduleHandlerConfiguration->filterIdentifier, NULL,
  //                      CLSCTX_INPROC_SERVER,
  //                      IID_PPV_ARGS (&filter_p));
  //  if (FAILED (result)) // REGDB_E_CLASSNOTREG: 0x80040154
  //  {
  //    ACE_DEBUG ((LM_ERROR,
  //                ACE_TEXT ("failed to CoCreateInstance(%s): \"%s\", aborting\n"),
  //                ACE_TEXT (Stream_Module_Decoder_Tools::GUIDToString (configuration_in.moduleHandlerConfiguration->filterIdentifier).c_str ()),
  //                ACE_TEXT (Common_Error_Tools::errorToString (result, true).c_str ())));
  //    goto error;
  //  } // end IF
  //} // end IF
  //else
  //{
  //  //CUnknown* unknown_p =
  //  //  (configuration_in.moduleHandlerConfiguration->push ? Test_I_Target_DirectShowFilter_t::CreateInstance (NULL, &result)
  //  //                                                     : Test_I_Target_AsynchDirectShowFilter_t::CreateInstance (NULL, &result));
  //  //if (!unknown_p)
  //  //{
  //  //  ACE_DEBUG ((LM_CRITICAL,
  //  //              ACE_TEXT ("failed to allocate memory: \"%m\", aborting\n")));
  //  //  goto error;
  //  //} // end IF
  //  //result = unknown_p->NonDelegatingQueryInterface (IID_PPV_ARGS (&filter_p));
  //  //if (FAILED (result))
  //  //{
  //  //  ACE_DEBUG ((LM_ERROR,
  //  //              ACE_TEXT ("failed to CUnknown::NonDelegatingQueryInterface(IID_IBaseFilter): \"%s\", aborting\n"),
  //  //              ACE_TEXT (Common_Error_Tools::errorToString (result).c_str ())));

  //  //  // clean up
  //  //  unknown_p->NonDelegatingRelease ();

  //  //  goto error;
  //  //} // end IF
  //  //unknown_p->NonDelegatingRelease ();
  //  ACE_ASSERT (directshow_display_impl_p);
  //  ULONG reference_count = directshow_display_impl_p->AddRef ();
  //  filter_p = directshow_display_impl_p;
  //} // end ELSE
  //ACE_ASSERT (filter_p);

  //// *TODO*: remove type inference
  //if (!Stream_Module_Device_DirectShow_Tools::loadTargetRendererGraph (filter_p,
  //                                                                     (configuration_in.moduleHandlerConfiguration->push ? STREAM_MISC_DS_WIN32_FILTER_NAME_SOURCE_L
  //                                                                                                                        : STREAM_MISC_DS_WIN32_FILTER_NAME_ASYNCH_SOURCE_L),
  //                                                                     configuration_in.moduleHandlerConfiguration->window,
  //                                                                     graphBuilder_,
  //                                                                     buffer_negotiation_p,
  //                                                                     graph_configuration))
  //{
  //  ACE_DEBUG ((LM_ERROR,
  //              ACE_TEXT ("failed to Stream_Module_Device_DirectShow_Tools::loadTargetRendererGraph(\"%@\"), aborting\n"),
  //              configuration_in.moduleHandlerConfiguration->window));
  //  goto error;
  //} // end IF
  //filter_p->Release ();
  //filter_p = NULL;
  //ACE_ASSERT (graphBuilder_);
  //ACE_ASSERT (buffer_negotiation_p);

  //// *NOTE*: connect() (see below) will call SetFormat on the source filter.
  ////         This will assert, as the module has not been initialized, so the
  ////         default format has not been set
  ////         --> do so here
  //setFormat (graphBuilder_,
  //           graph_configuration.front ().filterName,
  //           *configuration_in.moduleHandlerConfiguration->format);

  //reference_count = graphBuilder_->AddRef ();
  //configuration_in.moduleHandlerConfiguration->graphBuilder = graphBuilder_;
  //release_builder = true;

//continue_:
  //ACE_ASSERT (!graph_configuration.empty ());
  //struct Stream_Module_Device_DirectShow_GraphEntry& graph_entry_r =
  //  graph_configuration.front ();
  //ACE_ASSERT (!graph_entry_r.mediaType);
  //if (!Stream_Module_Device_DirectShow_Tools::copy (*configuration_in.moduleHandlerConfiguration->format,
  //                                                           graph_entry_r.mediaType))
  //{
  //  ACE_DEBUG ((LM_ERROR,
  //              ACE_TEXT ("failed to Stream_Module_Device_DirectShow_Tools::copy(), aborting\n")));
  //  goto error;
  //} // end IF
  //ACE_ASSERT (graph_entry_r.mediaType);
#if defined (_DEBUG)
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("source format: %s\n"),
              ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::toString (configuration_p->sourceFormat, true).c_str ())));

  //log_file_name =
  //  Common_File_Tools::getLogDirectory (std::string (),
  //                                      0);
  //log_file_name += ACE_DIRECTORY_SEPARATOR_STR;
  //log_file_name += STREAM_DEV_DIRECTSHOW_LOGFILE_NAME;
  //Stream_Module_Device_DirectShow_Tools::debug (graphBuilder_,
  //                                              log_file_name);
#endif // _DEBUG

  //// sanity check(s)
  //ACE_ASSERT (!session_data_r.direct3DDevice);

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

  //ACE_ASSERT (buffer_negotiation_p);
  //ACE_OS::memset (&allocator_properties, 0, sizeof (allocator_properties));
  //// *TODO*: IMemAllocator::SetProperties returns VFW_E_BADALIGN (0x8004020e)
  ////         if this is -1/0 (why ?)
  //// *NOTE*: somewhere in the DirectShow baseclasses code (amfilter.cpp), there
  ////         is a mention of 0 making no sense...
  //allocator_properties.cbAlign = 1;
  ////allocator_properties.cbAlign = -1; // <-- use default
  //allocator_properties.cbBuffer = configuration_in.bufferSize;
  //allocator_properties.cbPrefix = 0;
  ////allocator_properties.cbPrefix = -1; // <-- use default
  //allocator_properties.cBuffers =
  //  STREAM_DEV_CAM_DIRECTSHOW_DEFAULT_DEVICE_BUFFERS;
  //result =
  //    buffer_negotiation_p->SuggestAllocatorProperties (&allocator_properties);
  //if (FAILED (result))
  //{
  //  ACE_DEBUG ((LM_ERROR,
  //              ACE_TEXT ("failed to IAMBufferNegotiation::SuggestAllocatorProperties(): \"%s\", aborting\n"),
  //              ACE_TEXT (Common_Error_Tools::errorToString (result).c_str ())));
  //  goto error;
  //} // end IF

  //if (!Stream_Module_Device_DirectShow_Tools::connect (graphBuilder_,
  //                                                     graph_configuration))
  //{
  //  ACE_DEBUG ((LM_ERROR,
  //              ACE_TEXT ("failed to Stream_Module_Device_DirectShow_Tools::connect(), aborting\n")));
  //  goto error;
  //} // end IF
  ////// *NOTE*: for some (unknown) reason, connect()ing the sample grabber to the
  //////         null renderer 'breaks' the connection between the AVI decompressor
  //////         and the sample grabber (go ahead, try it in with graphedit.exe)
  //////         --> reconnect the AVI decompressor to the (connected) sample
  //////             grabber; this seems to work
  ////if (!Stream_Module_Device_DirectShow_Tools::connected (graphBuilder_,
  ////                                                       graph_configuration.front ().filterName))
  ////{
  ////  ACE_DEBUG ((LM_DEBUG,
  ////              ACE_TEXT ("reconnecting...\n")));

  ////  if (!Stream_Module_Device_DirectShow_Tools::connectFirst (graphBuilder_,
  ////                                                            graph_configuration.front ().filterName))
  ////  {
  ////    ACE_DEBUG ((LM_ERROR,
  ////                ACE_TEXT ("failed to Stream_Module_Device_DirectShow_Tools::connectFirst(), aborting\n")));
  ////    goto error;
  ////  } // end IF
  ////} // end IF
  //ACE_ASSERT (Stream_Module_Device_DirectShow_Tools::connected (graphBuilder_,
  //                                                              graph_configuration.front ().filterName));
  //for (Stream_Module_Device_DirectShow_GraphIterator_t iterator = graph_configuration.begin ();
  //     iterator != graph_configuration.end ();
  //     ++iterator)
  //  if ((*iterator).mediaType)
  //    Stream_Module_Device_DirectShow_Tools::delete_ ((*iterator).mediaType);

  //// debug info
  //ACE_OS::memset (&allocator_properties, 0, sizeof (allocator_properties));
  //result =
  //    buffer_negotiation_p->GetAllocatorProperties (&allocator_properties);
  //if (FAILED (result)) // E_FAIL (0x80004005)
  //{
  //  ACE_DEBUG ((LM_ERROR,
  //              ACE_TEXT ("failed to IAMBufferNegotiation::GetAllocatorProperties(): \"%s\", aborting\n"),
  //              ACE_TEXT (Common_Error_Tools::errorToString (result).c_str ())));
  //  goto error;
  //} // end IF
  //ACE_DEBUG ((LM_DEBUG,
  //            ACE_TEXT ("allocator properties (buffers/size/alignment/prefix): %d/%d/%d/%d\n"),
  //            allocator_properties.cBuffers,
  //            allocator_properties.cbBuffer,
  //            allocator_properties.cbAlign,
  //            allocator_properties.cbPrefix));
  //buffer_negotiation_p->Release ();
  //buffer_negotiation_p = NULL;

  //result = graphBuilder_->QueryInterface (IID_PPV_ARGS (&media_filter_p));
  //if (FAILED (result))
  //{
  //  ACE_DEBUG ((LM_ERROR,
  //              ACE_TEXT ("failed to IGraphBuilder::QueryInterface(IID_IMediaFilter): \"%s\", aborting\n"),
  //              ACE_TEXT (Common_Error_Tools::errorToString (result).c_str ())));
  //  goto error;
  //} // end IF
  //ACE_ASSERT (media_filter_p);
  //result = media_filter_p->SetSyncSource (NULL);
  //if (FAILED (result))
  //{
  //  ACE_DEBUG ((LM_ERROR,
  //              ACE_TEXT ("failed to IMediaFilter::SetSyncSource(): \"%s\", aborting\n"),
  //              ACE_TEXT (Common_Error_Tools::errorToString (result).c_str ())));
  //  goto error;
  //} // end IF
  //media_filter_p->Release (); media_filter_p = NULL;
#endif // ACE_WIN32 || ACE_WIN64

  // ---------------------------------------------------------------------------

  //Test_I_Stream_Module_CamSource* source_impl_p = NULL;

  //// ******************* Camera Source ************************
  //Stream_Module_t* module_p =
  //  const_cast<Stream_Module_t*> (inherited::find (ACE_TEXT_ALWAYS_CHAR (STREAM_DEV_CAM_SOURCE_DIRECTSHOW_DEFAULT_NAME_STRING)));
  //if (!module_p)
  //{
  //  ACE_DEBUG ((LM_ERROR,
  //              ACE_TEXT ("failed to retrieve \"%s\" module handle, aborting\n"),
  //              ACE_TEXT (STREAM_DEV_CAM_SOURCE_DIRECTSHOW_DEFAULT_NAME_STRING)));
  //  return false;
  //} // end IF
  //source_impl_p =
  //  dynamic_cast<Test_I_Stream_Module_CamSource*> (module_p->writer ());
  //if (!source_impl_p)
  //{
  //  ACE_DEBUG ((LM_ERROR,
  //              ACE_TEXT ("dynamic_cast<Test_I_Stream_Module_CamSource> failed, aborting\n")));
  //  return false;
  //} // end IF

  // ---------------------------------------------------------------------------

  ACE_ASSERT (session_data_r.formats.empty ());
  session_data_r.formats.push_back (*Stream_MediaFramework_DirectShow_Tools::copy (configuration_p->sourceFormat));

  // ---------------------------------------------------------------------------

  //if (!source_impl_p->initialize (inherited::state_))
  //{
  //  ACE_DEBUG ((LM_ERROR,
  //              ACE_TEXT ("%s: failed to initialize module writer, aborting\n"),
  //              module_p->name ()));
  //  goto error;
  //} // end IF
  ////fileReader_impl_p->reset ();
  //// *NOTE*: push()ing the module will open() it
  ////         --> set the argument that is passed along (head module expects a
  ////             handle to the session data)
  //module_p->arg (inherited::sessionData_);

  if (configuration_in.configuration_.setupPipeline)
    if (!inherited::setup (configuration_in.configuration_.notificationStrategy))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("%s: failed to set up pipeline, aborting\n"),
                  ACE_TEXT (stream_name_string_)));
      goto error;
    } // end IF

  return true;

error:
  if (reset_setup_pipeline)
    const_cast<inherited::CONFIGURATION_T&> (configuration_in).configuration_.setupPipeline =
      setup_pipeline;
  //if (filter_p)
  //  filter_p->Release ();
  //if (release_builder)
  //{
  //  configuration_r.moduleHandlerConfiguration->graphBuilder->Release ();
  //  configuration_r.moduleHandlerConfiguration->graphBuilder = NULL;
  //} // end IF
  //for (Stream_Module_Device_DirectShow_GraphIterator_t iterator = graph_configuration.begin ();
  //     iterator != graph_configuration.end ();
  //     ++iterator)
  //  if ((*iterator).mediaType)
  //    Stream_Module_Device_DirectShow_Tools::delete_ ((*iterator).mediaType);
  //if (graphBuilder_)
  //  graphBuilder_->Release ();
  ////if (direct3D_manager_p)
  ////  direct3D_manager_p->Release ();
  //if (session_data_r.direct3DDevice)
  //{
  //  session_data_r.direct3DDevice->Release (); session_data_r.direct3DDevice = NULL;
  //} // end IF
  if (!session_data_r.formats.empty ())
    session_data_r.formats.clear ();
  //session_data_r.resetToken = 0;

  //if (COM_initialized)
  //  CoUninitialize ();

  return false;
}

void
Test_I_Target_DirectShow_Stream::setFormat (IGraphBuilder* builder_in,
                                            const std::wstring& sourceFilterName_in,
                                            const struct _AMMediaType& mediaType_in)
{
  STREAM_TRACE (ACE_TEXT ("Test_I_Target_DirectShow_Stream::setFormat"));

  // sanity check(s)
  ACE_ASSERT (builder_in);
  ACE_ASSERT (!sourceFilterName_in.empty ());

  IBaseFilter* filter_p = NULL;
  IPin* pin_p = NULL;
  HRESULT result =
    builder_in->FindFilterByName (sourceFilterName_in.c_str (),
                                  &filter_p);
  if (FAILED (result))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to IGraphBuilder::FindFilterByName(\"%s\"): \"%s\", returning\n"),
                ACE_TEXT_WCHAR_TO_TCHAR (sourceFilterName_in.c_str ()),
                ACE_TEXT (Common_Error_Tools::errorToString (result).c_str ())));
    goto error;
  } // end IF
  ACE_ASSERT (filter_p);

  pin_p = Stream_MediaFramework_DirectShow_Tools::pin (filter_p,
                                                       PINDIR_OUTPUT);
  if (!pin_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s has no output pin, aborting\n"),
                ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::name (filter_p).c_str ())));
    goto error;
  } // end IF

  Common_IInitialize_T<struct _AMMediaType>* iinitialize_p =
    dynamic_cast<Common_IInitialize_T<struct _AMMediaType>*> (pin_p);
  if (!iinitialize_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s/%s: failed to dynamic_cast<Common_IInitialize_T*> (0x%@), aborting\n"),
                ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::name (filter_p).c_str ()),
                ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::name (pin_p).c_str ()),
                pin_p));
    goto error;
  } // end IF
  // *TODO*: remove type inference
  if (!iinitialize_p->initialize (mediaType_in))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: failed to Stream_Misc_DirectShow_Source_Filter_OutputPin_T::initialize(), aborting\n"),
                ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::name (filter_p).c_str ())));
    goto error;
  } // end IF
  filter_p->Release (); filter_p = NULL;
  pin_p->Release (); pin_p = NULL;

  return;

error:
  if (filter_p)
    filter_p->Release ();
  if (pin_p)
    pin_p->Release ();
}

//////////////////////////////////////////

Test_I_Target_MediaFoundation_Stream::Test_I_Target_MediaFoundation_Stream ()
 : inherited ()
 , mediaSession_ (NULL)
 , referenceCount_ (1)
{
  STREAM_TRACE (ACE_TEXT ("Test_I_Target_MediaFoundation_Stream::Test_I_Target_MediaFoundation_Stream"));

}

Test_I_Target_MediaFoundation_Stream::~Test_I_Target_MediaFoundation_Stream ()
{
  STREAM_TRACE (ACE_TEXT ("Test_I_Target_MediaFoundation_Stream::~Test_I_Target_MediaFoundation_Stream"));

  inherited::shutdown ();
}

bool
Test_I_Target_MediaFoundation_Stream::load (Stream_ILayout* layout_in,
                                            bool& delete_out)
{
  STREAM_TRACE (ACE_TEXT ("Test_I_Target_MediaFoundation_Stream::load"));

    if (!inherited::load (layout_in,
                          delete_out))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: failed to Stream_Module_Net_IO_Stream_T::load(), aborting\n"),
                ACE_TEXT (stream_name_string_)));
    return false;
  } // end IF
  ACE_ASSERT (delete_out);

  Stream_Module_t* module_p = NULL;
  ACE_NEW_RETURN (module_p,
                  Test_I_Target_MediaFoundation_Splitter_Module (this,
                                                                 ACE_TEXT_ALWAYS_CHAR (STREAM_MISC_SPLITTER_DEFAULT_NAME_STRING)),
                  false);
  ACE_ASSERT (module_p);
  layout_in->append (module_p, NULL, 0);
  //Test_I_Target_MediaFoundation_Module_AVIDecoder_Module            decoder_;
  //Test_I_Target_MediaFoundation_MediaFoundationSource_Module mediaFoundationSource_;
  //module_p = NULL;
  //ACE_NEW_RETURN (module_p,
  //                Test_I_Target_MediaFoundation_StatisticReport_Module (this,
  //                                                                      ACE_TEXT_ALWAYS_CHAR (MODULE_STAT_REPORT_DEFAULT_NAME_STRING)),
  //                false);
  //ACE_ASSERT (module_p);
  //layout_in->append (module_p, NULL, 0);
  module_p = NULL;
  ACE_NEW_RETURN (module_p,
                  Test_I_Target_MediaFoundation_Display_Module (this,
                                                                ACE_TEXT_ALWAYS_CHAR (STREAM_VIS_MEDIAFOUNDATION_DEFAULT_NAME_STRING)),
                  false);
  ACE_ASSERT (module_p);
  layout_in->append (module_p, NULL, 0);

  delete_out = true;

  return true;
}

bool
Test_I_Target_MediaFoundation_Stream::initialize (const CONFIGURATION_T& configuration_in,
                                                  ACE_HANDLE handle_in)
{
  STREAM_TRACE (ACE_TEXT ("Test_I_Target_MediaFoundation_Stream::initialize"));

  // sanity check(s)
  ACE_ASSERT (!isRunning ());

  bool result = false;
  bool setup_pipeline = configuration_in.configuration_.setupPipeline;
  bool reset_setup_pipeline = false;
  inherited::CONFIGURATION_T::ITERATOR_T iterator;
  std::string url_string = ACE_TEXT_ALWAYS_CHAR ("camstream");

  // allocate a new session state, reset stream
  const_cast<inherited::CONFIGURATION_T&> (configuration_in).configuration_.setupPipeline =
    false;
  reset_setup_pipeline = true;
  if (!inherited::initialize (configuration_in,
                              handle_in))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: failed to Stream_Module_Net_IO_Stream_T::initialize(), aborting\n"),
                ACE_TEXT (stream_name_string_)));
    goto error;
  } // end IF
  const_cast<inherited::CONFIGURATION_T&> (configuration_in).configuration_.setupPipeline =
    setup_pipeline;
  reset_setup_pipeline = false;
  ACE_ASSERT (inherited::sessionData_);
  Test_I_Target_MediaFoundation_SessionData& session_data_r =
    const_cast<Test_I_Target_MediaFoundation_SessionData&> (inherited::sessionData_->getR ());
  // *TODO*: remove type inferences
  session_data_r.lock = &(inherited::sessionDataLock_);
  inherited::state_.sessionData = &session_data_r;
  iterator =
    const_cast<inherited::CONFIGURATION_T&> (configuration_in).find (ACE_TEXT_ALWAYS_CHAR (""));
  ACE_ASSERT (iterator != configuration_in.end ());
  struct Test_I_Target_MediaFoundation_ModuleHandlerConfiguration* configuration_p =
    dynamic_cast<struct Test_I_Target_MediaFoundation_ModuleHandlerConfiguration*> (&(*iterator).second.second);
  ACE_ASSERT (configuration_p);
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  ACE_ASSERT (configuration_p->sourceFormat);
  ACE_ASSERT (!session_data_r.sourceFormat);

  session_data_r.sourceFormat =
    Stream_MediaFramework_MediaFoundation_Tools::copy (configuration_p->sourceFormat);
  if (!session_data_r.sourceFormat)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: failed to Stream_MediaFramework_MediaFoundation_Tools::copy(), aborting\n"),
                ACE_TEXT (stream_name_string_)));
    goto error;
  } // end IF
#else
  session_data_r.sourceFormat = configuration_p->format;
#endif // ACE_WIN32 || ACE_WIN64

  // ---------------------------------------------------------------------------

  bool graph_loaded = false;
  bool COM_initialized = false;
  IMFTopology* topology_p = NULL;
  UINT32 item_count = 0;
  ULONG reference_count = 0;
  HRESULT result_2 = CoInitializeEx (NULL,
                                     (COINIT_MULTITHREADED     |
                                      COINIT_DISABLE_OLE1DDE   |
                                      COINIT_SPEED_OVER_MEMORY));
  if (FAILED (result_2))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: failed to CoInitializeEx(): \"%s\", aborting\n"),
                ACE_TEXT (stream_name_string_),
                ACE_TEXT (Common_Error_Tools::errorToString (result_2).c_str ())));
    goto error;
  } // end IF
  COM_initialized = true;

  // sanity check(s)
  ACE_ASSERT (configuration_p->sourceFormat);

  if (!Stream_Module_Decoder_Tools::loadTargetRendererTopology (url_string,
                                                                configuration_p->sourceFormat,
                                                                NULL,
                                                                //configuration_p->window,
                                                                session_data_r.rendererNodeId,
                                                                topology_p))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: failed to Stream_Module_Decoder_Tools::loadTargetRendererTopology(\"%s\"), aborting\n"),
                ACE_TEXT (stream_name_string_),
                ACE_TEXT (configuration_p->deviceIdentifier.c_str ())));
    goto error;
  } // end IF
  ACE_ASSERT (topology_p);
  graph_loaded = true;

#if defined (_DEBUG)
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("%s: source format: \"%s\"\n"),
              ACE_TEXT (Stream_MediaFramework_MediaFoundation_Tools::toString (configuration_p->sourceFormat).c_str ()),
              ACE_TEXT (stream_name_string_)));
#endif // _DEBUG

#if COMMON_OS_WIN32_TARGET_PLATFORM(0x0600) // _WIN32_WINNT_VISTA
  if (mediaSession_)
  {
    // *TODO*: this crashes in CTopoNode::UnlinkInput ()...
    //result = mediaSession_->Shutdown ();
    //if (FAILED (result))
    //  ACE_DEBUG ((LM_ERROR,
    //              ACE_TEXT ("failed to IMFMediaSession::Shutdown(): \"%s\", continuing\n"),
    //              ACE_TEXT (Common_Error_Tools::errorToString (result).c_str ())));
    mediaSession_->Release (); mediaSession_ = NULL;
  } // end IF
  if (configuration_p->session)
  {
    reference_count = configuration_p->session->AddRef ();
    mediaSession_ = configuration_p->session;

    if (!Stream_MediaFramework_MediaFoundation_Tools::clear (mediaSession_))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("%s: failed to Stream_MediaFramework_MediaFoundation_Tools::clear(), aborting\n"),
                  ACE_TEXT (stream_name_string_)));
      goto error;
    } // end IF
  } // end IF

  if (!Stream_MediaFramework_MediaFoundation_Tools::setTopology (topology_p,
                                                                 mediaSession_,
                                                                 true))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: failed to Stream_MediaFramework_MediaFoundation_Tools::setTopology(), aborting\n"),
                ACE_TEXT (stream_name_string_)));
    goto error;
  } // end IF
  ACE_ASSERT (mediaSession_);

  if (!configuration_p->session)
  {
    reference_count = mediaSession_->AddRef ();
    configuration_p->session = mediaSession_;
  } // end IF
#endif // COMMON_OS_WIN32_TARGET_PLATFORM(0x0600)
  topology_p->Release (); topology_p = NULL;

  // ---------------------------------------------------------------------------

  if (configuration_in.configuration_.setupPipeline)
    if (!inherited::setup (configuration_in.configuration_.notificationStrategy))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("%s: failed to set up pipeline, aborting\n"),
                  ACE_TEXT (stream_name_string_)));
      goto error;
    } // end IF

  // ---------------------------------------------------------------------------

  return true;

error:
  if (reset_setup_pipeline)
    const_cast<inherited::CONFIGURATION_T&> (configuration_in).configuration_.setupPipeline =
      setup_pipeline;
  if (session_data_r.direct3DDevice)
  {
    session_data_r.direct3DDevice->Release (); session_data_r.direct3DDevice = NULL;
  } // end IF
  if (session_data_r.sourceFormat)
  {
    session_data_r.sourceFormat->Release (); session_data_r.sourceFormat = NULL;
  } // end IF
  session_data_r.direct3DManagerResetToken = 0;

  if (COM_initialized)
    CoUninitialize ();

  return false;
}
#else
Test_I_Target_Stream::Test_I_Target_Stream ()
 : inherited ()
{
  STREAM_TRACE (ACE_TEXT ("Test_I_Target_Stream::Test_I_Target_Stream"));

}

Test_I_Target_Stream::~Test_I_Target_Stream ()
{
  STREAM_TRACE (ACE_TEXT ("Test_I_Target_Stream::~Test_I_Target_Stream"));

  inherited::shutdown ();
}

bool
Test_I_Target_Stream::load (Stream_ILayout* layout_in,
                            bool& delete_out)
{
  STREAM_TRACE (ACE_TEXT ("Test_I_Target_Stream::load"));

//  // initialize return value(s)
//  modules_out.clear ();
//  delete_out = false;
  ACE_ASSERT (delete_out);

  Stream_Module_t* module_p = NULL;

  if (!inherited::load (layout_in,
                        delete_out))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: failed to Stream_Module_Net_IO_Stream_T::load(), aborting\n")));
    return false;
  } // end IF

  ACE_NEW_RETURN (module_p,
                  Test_I_Target_Splitter_Module (this,
                                                 ACE_TEXT_ALWAYS_CHAR (STREAM_MISC_SPLITTER_DEFAULT_NAME_STRING)),
                  false);
  layout_in->append (module_p, NULL, 0);
  module_p = NULL;
  //Test_I_Target_Module_AVIDecoder_Module            decoder_;
  ACE_NEW_RETURN (module_p,
                  Test_I_Target_StatisticReport_Module (this,
                                                        ACE_TEXT_ALWAYS_CHAR (MODULE_STAT_REPORT_DEFAULT_NAME_STRING)),
                  false);
  layout_in->append (module_p, NULL, 0);
  module_p = NULL;
#if defined (GUI_SUPPORT)
#if defined (GTK_USE)
  ACE_NEW_RETURN (module_p,
                  Test_I_Target_Display_Module (this,
                                                ACE_TEXT_ALWAYS_CHAR (STREAM_VIS_GTK_PIXBUF_DEFAULT_NAME_STRING)),
                  false);
  layout_in->append (module_p, NULL, 0);
#endif // GTK_USE
#endif // GUI_SUPPORT
  module_p = NULL;

  delete_out = true;

  return true;
}

bool
Test_I_Target_Stream::initialize (const typename inherited::CONFIGURATION_T& configuration_in,
                                  ACE_HANDLE handle_in)
{
  STREAM_TRACE (ACE_TEXT ("Test_I_Target_Stream::initialize"));

  // sanity check(s)
  ACE_ASSERT (!isRunning ());

//  bool result = false;
  bool setup_pipeline = configuration_in.configuration_.setupPipeline;
  bool reset_setup_pipeline = false;

  // allocate a new session state, reset stream
  const_cast<Test_I_Target_StreamConfiguration_t&> (configuration_in).configuration_.setupPipeline =
      false;
  reset_setup_pipeline = true;
  if (!inherited::initialize (configuration_in,
                              handle_in))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: failed to Stream_Base_T::initialize(), aborting\n"),
                ACE_TEXT (stream_name_string_)));
    return false;
  } // end IF
  const_cast<Test_I_Target_StreamConfiguration_t&> (configuration_in).configuration_.setupPipeline =
      setup_pipeline;
  reset_setup_pipeline = false;

  // sanity check(s)
  ACE_ASSERT (inherited::sessionData_);

  struct Test_I_Target_SessionData& session_data_r =
    const_cast<struct Test_I_Target_SessionData&> (inherited::sessionData_->getR ());
  // *TODO*: remove type inferences
  session_data_r.lock = &(inherited::sessionDataLock_);
  inherited::state_.sessionData = &session_data_r;
  Test_I_Target_StreamConfiguration_t::ITERATOR_T iterator =
      const_cast<Test_I_Target_StreamConfiguration_t&> (configuration_in).find (ACE_TEXT_ALWAYS_CHAR (""));
  ACE_ASSERT (iterator != configuration_in.end ());
  struct Test_I_Target_ModuleHandlerConfiguration* configuration_p =
      dynamic_cast<struct Test_I_Target_ModuleHandlerConfiguration*> (&((*iterator).second.second));
  ACE_ASSERT (configuration_p);

  ACE_ASSERT (session_data_r.formats.empty ());
  session_data_r.formats.push_front (configuration_in.configuration_.format);
  //  session_data_r.sessionId = configuration_p->sessionId;
  session_data_r.targetFileName = configuration_p->targetFileName;

  //  configuration_in.moduleConfiguration.streamState = &state_;
//  configuration_p->stateMachineLock = &inherited::state_.stateMachineLock;

  // ---------------------------------------------------------------------------

  if (configuration_in.configuration_.setupPipeline)
    if (!inherited::setup (configuration_in.configuration_.notificationStrategy))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("%s: failed to set up pipeline, aborting\n"),
                  ACE_TEXT (stream_name_string_)));
      goto error;
    } // end IF

  return true;

error:
  if (reset_setup_pipeline)
    const_cast<Test_I_Target_StreamConfiguration_t&> (configuration_in).configuration_.setupPipeline =
      setup_pipeline;

  return false;
}

void
Test_I_Target_Stream::ping ()
{
  STREAM_TRACE (ACE_TEXT ("Test_I_Target_Stream::ping"));

  ACE_ASSERT (false);
  ACE_NOTSUP;

  ACE_NOTREACHED (return;)
}
#endif
