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

#include "stream_dev_defines.h"
#include "stream_dev_tools.h"

#include "test_i_target_session_message.h"
#include "test_i_common_modules.h"
#include "test_i_source_stream.h"

#include "test_i_target_message.h"

#if defined (ACE_WIN32) || defined (ACE_WIN64)
//Test_I_Target_DirectShow_Stream::Test_I_Target_DirectShow_Stream (const std::string& name_in)
// : inherited (name_in)
// , graphBuilder_ (NULL)
//{
//  STREAM_TRACE (ACE_TEXT ("Test_I_Target_DirectShow_Stream::Test_I_Target_DirectShow_Stream"));
//
//}
//
//Test_I_Target_DirectShow_Stream::~Test_I_Target_DirectShow_Stream ()
//{
//  STREAM_TRACE (ACE_TEXT ("Test_I_Target_DirectShow_Stream::~Test_I_Target_DirectShow_Stream"));
//
//  inherited::shutdown ();
//
//  if (_DEBUG && graphBuilder_)
//    Stream_Module_Device_Tools::debug (graphBuilder_,
//                                       std::string ());
//
//  if (graphBuilder_)
//    graphBuilder_->Release ();
//}
//
//bool
//Test_I_Target_DirectShow_Stream::load (Stream_ModuleList_t& modules_out,
//                                       bool& delete_out)
//{
//  STREAM_TRACE (ACE_TEXT ("Test_I_Target_DirectShow_Stream::load"));
//
////  // initialize return value(s)
////  modules_out.clear ();
////  delete_out = false;
//
//  Stream_Module_t* module_p = NULL;
//  ACE_NEW_RETURN (module_p,
//                  Test_I_Target_DirectShow_Module_Display_Module (ACE_TEXT_ALWAYS_CHAR ("Display"),
//                                                                  NULL,
//                                                                  false),
//                  false);
//  modules_out.push_back (module_p);
//#if defined (ACE_WIN32) || defined (ACE_WIN64)
////  ACE_NEW_RETURN (module_p,
////                  Test_I_Target_DirectShow_Module_DisplayNull_Module (ACE_TEXT_ALWAYS_CHAR ("DisplayNull"),
////                                                                      NULL,
////                                                                      false),
////                  false);
////  modules_out.push_back (module_p);
//#endif
//  module_p = NULL;
//#if defined (ACE_WIN32) || defined (ACE_WIN64)
//  //Test_I_Target_DirectShow_Module_DirectShowSource_Module directShowSource_;
//  //Test_I_Target_DirectShow_Module_MediaFoundationSource_Module mediaFoundationSource_;
//#endif
//  ACE_NEW_RETURN (module_p,
//                  Test_I_Target_DirectShow_Module_RuntimeStatistic_Module (ACE_TEXT_ALWAYS_CHAR ("RuntimeStatistic"),
//                                                                           NULL,
//                                                                           false),
//                  false);
//  modules_out.push_back (module_p);
//  module_p = NULL;
//  ACE_NEW_RETURN (module_p,
//                  Test_I_Target_DirectShow_Module_Splitter_Module (ACE_TEXT_ALWAYS_CHAR ("Splitter"),
//                                                                   NULL,
//                                                                   false),
//                  false);
//  modules_out.push_back (module_p);
//  //Test_I_Target_DirectShow_Module_AVIDecoder_Module            decoder_;
//  //Test_I_Target_DirectShow_Module_Net_IO_Module                source_;
//
//  if (!inherited::load (modules_out,
//                        delete_out))
//  {
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("%s: failed to Stream_Module_Net_IO_Stream_T::load(), aborting\n")));
//    return false;
//  } // end IF
//  ACE_ASSERT (delete_out);
//
//  delete_out = true;
//
//  return true;
//}
//
//bool
//Test_I_Target_DirectShow_Stream::initialize (const Test_I_Target_DirectShow_StreamConfiguration& configuration_in,
//                                             bool setupPipeline_in,
//                                             bool resetSessionData_in)
//{
//  STREAM_TRACE (ACE_TEXT ("Test_I_Target_DirectShow_Stream::initialize"));
//
//  // sanity check(s)
//  ACE_ASSERT (!isRunning ());
//
//  // allocate a new session state, reset stream
//  // sanity check(s)
//  ACE_ASSERT (configuration_in.moduleHandlerConfiguration);
//  if (!inherited::initialize (configuration_in,
//                              false,
//                              resetSessionData_in))
//  {
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("%s: failed to Stream_Base_T::initialize(), aborting\n"),
//                ACE_TEXT (inherited::name ().c_str ())));
//    return false;
//  } // end IF
//  ACE_ASSERT (inherited::sessionData_);
//  Test_I_Target_DirectShow_SessionData& session_data_r =
//    const_cast<Test_I_Target_DirectShow_SessionData&> (inherited::sessionData_->get ());
//  // *TODO*: remove type inferences
//  session_data_r.lock = &(inherited::sessionDataLock_);
//  inherited::state_.currentSessionData = &session_data_r;
//  // *TODO*: remove type inferences
//  ACE_ASSERT (configuration_in.moduleHandlerConfiguration);
//#if defined (ACE_WIN32) || defined (ACE_WIN64)
//  session_data_r.format = configuration_in.moduleHandlerConfiguration->format;
//#else
//  session_data_r.format = configuration_in.moduleHandlerConfiguration->format;
//#endif
//  session_data_r.sessionID = configuration_in.sessionID;
//  //session_data_r.targetFileName =
//  //  configuration_in.moduleHandlerConfiguration->targetFileName;
//
//  ACE_ASSERT (configuration_in.moduleConfiguration);
//  //  configuration_in.moduleConfiguration.streamState = &state_;
//  Test_I_Target_DirectShow_StreamConfiguration& configuration_r =
//      const_cast<Test_I_Target_DirectShow_StreamConfiguration&> (configuration_in);
//  configuration_r.moduleHandlerConfiguration->stateMachineLock =
//    &inherited::state_.stateMachineLock;
//
//  // ---------------------------------------------------------------------------
//
//  // ---------------------------------------------------------------------------
//
//  //Test_I_Stream_Module_CamSource* source_impl_p = NULL;
//
//  //// ******************* Camera Source ************************
//  //Stream_Module_t* module_p =
//  //  const_cast<Stream_Module_t*> (inherited::find (ACE_TEXT_ALWAYS_CHAR ("CamSource")));
//  //if (!module_p)
//  //{
//  //  ACE_DEBUG ((LM_ERROR,
//  //              ACE_TEXT ("failed to retrieve \"%s\" module handle, aborting\n"),
//  //              ACE_TEXT ("CamSource")));
//  //  return false;
//  //} // end IF
//  //source_impl_p =
//  //  dynamic_cast<Test_I_Stream_Module_CamSource*> (module_p->writer ());
//  //if (!source_impl_p)
//  //{
//  //  ACE_DEBUG ((LM_ERROR,
//  //              ACE_TEXT ("dynamic_cast<Test_I_Stream_Module_CamSource> failed, aborting\n")));
//  //  return false;
//  //} // end IF
//
//  // ---------------------------------------------------------------------------
//
//  // sanity check(s)
//  ACE_ASSERT (configuration_in.moduleHandlerConfiguration);
//
//  struct _AllocatorProperties allocator_properties;
//  IAMBufferNegotiation* buffer_negotiation_p = NULL;
//  bool COM_initialized = false;
//  bool release_builder = false;
//  HRESULT result = E_FAIL;
//  ULONG reference_count = 0;
//  IAMStreamConfig* stream_config_p = NULL;
//  IMediaFilter* media_filter_p = NULL;
//  //IDirect3DDeviceManager9* direct3D_manager_p = NULL;
//  //struct _D3DPRESENT_PARAMETERS_ d3d_presentation_parameters;
//  std::list<std::wstring> filter_pipeline;
//  IBaseFilter* filter_p = NULL;
//  ISampleGrabber* isample_grabber_p = NULL;
//  std::string log_file_name;
//
//  result = CoInitializeEx (NULL,
//                           (COINIT_MULTITHREADED     |
//                            COINIT_DISABLE_OLE1DDE   |
//                            COINIT_SPEED_OVER_MEMORY));
//  if (FAILED (result))
//  {
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("failed to CoInitializeEx(): \"%s\", aborting\n"),
//                ACE_TEXT (Common_Tools::error2String (result).c_str ())));
//    return false;
//  } // end IF
//  COM_initialized = true;
//
//  if (configuration_in.moduleHandlerConfiguration->builder)
//  {
//    reference_count =
//      configuration_in.moduleHandlerConfiguration->builder->AddRef ();
//    graphBuilder_ = configuration_in.moduleHandlerConfiguration->builder;
//
//    if (!Stream_Module_Device_Tools::clear (graphBuilder_))
//    {
//      ACE_DEBUG ((LM_ERROR,
//                  ACE_TEXT ("failed to Stream_Module_Device_Tools::resetDeviceGraph(): \"%s\", aborting\n")));
//      goto error;
//    } // end IF
//
//    if (!Stream_Module_Device_Tools::getBufferNegotiation (graphBuilder_,
//                                                           buffer_negotiation_p))
//    {
//      ACE_DEBUG ((LM_ERROR,
//                  ACE_TEXT ("failed to Stream_Module_Device_Tools::getBufferNegotiation(), aborting\n")));
//      goto error;
//    } // end IF
//    ACE_ASSERT (buffer_negotiation_p);
//
//    goto continue_;
//  } // end IF
//
//  if (!Stream_Module_Device_Tools::loadTargetRendererGraph (configuration_in.moduleHandlerConfiguration->window,
//                                                            graphBuilder_,
//                                                            buffer_negotiation_p,
//                                                            filter_pipeline))
//  {
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("failed to Stream_Module_Device_Tools::loadTargetRendererGraph(\"%@\"), aborting\n"),
//                configuration_in.moduleHandlerConfiguration->window));
//    goto error;
//  } // end IF
//  ACE_ASSERT (graphBuilder_);
//  ACE_ASSERT (buffer_negotiation_p);
//
//  reference_count = graphBuilder_->AddRef ();
//  configuration_in.moduleHandlerConfiguration->builder = graphBuilder_;
//  release_builder = true;
//
//continue_:
//  if (!Stream_Module_Device_Tools::setCaptureFormat (graphBuilder_,
//                                                     *configuration_in.moduleHandlerConfiguration->format))
//  {
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("failed to Stream_Module_Device_Tools::setCaptureFormat(), aborting\n")));
//    goto error;
//  } // end IF
//#if defined (_DEBUG)
//  ACE_DEBUG ((LM_DEBUG,
//              ACE_TEXT ("capture format: \"%s\"...\n"),
//              ACE_TEXT (Stream_Module_Device_Tools::mediaTypeToString (*configuration_in.moduleHandlerConfiguration->format).c_str ())));
//
//  log_file_name =
//    Common_File_Tools::getLogDirectory (std::string (),
//                                        0);
//  log_file_name += ACE_DIRECTORY_SEPARATOR_STR;
//  log_file_name += MODULE_DEV_DIRECTSHOW_LOGFILE_NAME;
//  Stream_Module_Device_Tools::debug (graphBuilder_,
//                                     log_file_name);
//#endif
//
//  //// sanity check(s)
//  //ACE_ASSERT (!session_data_r.direct3DDevice);
//
//  //if (!Stream_Module_Device_Tools::getDirect3DDevice (configuration_in.moduleHandlerConfiguration->window,
//  //                                                    configuration_in.moduleHandlerConfiguration->format,
//  //                                                    session_data_r.direct3DDevice,
//  //                                                    d3d_presentation_parameters,
//  //                                                    direct3D_manager_p,
//  //                                                    session_data_r.resetToken))
//  //{
//  //  ACE_DEBUG ((LM_ERROR,
//  //              ACE_TEXT ("failed to Stream_Module_Device_Tools::getDirect3DDevice(), aborting\n")));
//  //  goto error;
//  //} // end IF
//  //ACE_ASSERT (direct3D_manager_p);
//
//  if (session_data_r.format)
//    Stream_Module_Device_Tools::deleteMediaType (session_data_r.format);
//  ACE_ASSERT (!session_data_r.format);
//  if (!Stream_Module_Device_Tools::getOutputFormat (graphBuilder_,
//                                                    session_data_r.format))
//  {
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("failed to Stream_Module_Device_Tools::getOutputFormat(), aborting\n")));
//    goto error;
//  } // end IF
//  ACE_ASSERT (session_data_r.format);
//
//  //filter_pipeline.push_front (MODULE_DEV_CAM_WIN32_FILTER_NAME_CAPTURE_VIDEO);
//  //result_2 =
//  //  configuration_in.moduleHandlerConfiguration->builder->FindFilterByName (MODULE_DEV_CAM_WIN32_FILTER_NAME_GRAB,
//  //                                                                          &filter_p);
//  //if (FAILED (result_2))
//  //{
//  //  ACE_DEBUG ((LM_ERROR,
//  //              ACE_TEXT ("failed to IGraphBuilder::FindFilterByName(\"%s\"): \"%s\", aborting\n"),
//  //              ACE_TEXT_WCHAR_TO_TCHAR (MODULE_DEV_CAM_WIN32_FILTER_NAME_GRAB),
//  //              ACE_TEXT (Common_Tools::error2String (result_2).c_str ())));
//  //  goto error;
//  //} // end IF
//  //ACE_ASSERT (filter_p);
//  //result_2 = filter_p->QueryInterface (IID_PPV_ARGS (&isample_grabber_p));
//  //if (FAILED (result_2))
//  //{
//  //  ACE_DEBUG ((LM_ERROR,
//  //              ACE_TEXT ("failed to IBaseFilter::QueryInterface(IID_ISampleGrabber): \"%s\", aborting\n"),
//  //              ACE_TEXT (Common_Tools::error2String (result_2).c_str ())));
//  //  goto error;
//  //} // end IF
//  //ACE_ASSERT (isample_grabber_p);
//  //filter_p->Release ();
//  //filter_p = NULL;
//
//  //result_2 = isample_grabber_p->SetBufferSamples (false);
//  //if (FAILED (result_2))
//  //{
//  //  ACE_DEBUG ((LM_ERROR,
//  //              ACE_TEXT ("failed to ISampleGrabber::SetBufferSamples(false): \"%s\", aborting\n"),
//  //              ACE_TEXT (Common_Tools::error2String (result_2).c_str ())));
//  //  goto error;
//  //} // end IF
//  //result_2 = isample_grabber_p->SetCallback (source_impl_p, 0);
//  //if (FAILED (result_2))
//  //{
//  //  ACE_DEBUG ((LM_ERROR,
//  //              ACE_TEXT ("failed to ISampleGrabber::SetCallback(): \"%s\", aborting\n"),
//  //              ACE_TEXT (Common_Tools::error2String (result_2).c_str ())));
//  //  goto error;
//  //} // end IF
//  //isample_grabber_p->Release ();
//  //isample_grabber_p = NULL;
//
//  ACE_ASSERT (buffer_negotiation_p);
//  ACE_OS::memset (&allocator_properties, 0, sizeof (allocator_properties));
//  allocator_properties.cbAlign = -1; // <-- use default
//  allocator_properties.cbBuffer = -1; // <-- use default
//  allocator_properties.cbPrefix = -1; // <-- use default
//  allocator_properties.cBuffers =
//    MODULE_DEV_CAM_DIRECTSHOW_DEFAULT_DEVICE_BUFFERS;
//  result =
//      buffer_negotiation_p->SuggestAllocatorProperties (&allocator_properties);
//  if (FAILED (result))
//  {
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("failed to IAMBufferNegotiation::SuggestAllocatorProperties(): \"%s\", aborting\n"),
//                ACE_TEXT (Common_Tools::error2String (result).c_str ())));
//    goto error;
//  } // end IF
//
//  if (!Stream_Module_Device_Tools::connect (graphBuilder_,
//                                            filter_pipeline))
//  {
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("failed to Stream_Module_Device_Tools::connect(), aborting\n")));
//    goto error;
//  } // end IF
//  // *NOTE*: for some (unknown) reason, connect()ing the sample grabber to the
//  //         null renderer 'breaks' the connection between the AVI decompressor
//  //         and the sample grabber (go ahead, try it in with graphedit.exe)
//  //         --> reconnect the AVI decompressor to the (connected) sample
//  //             grabber; this seems to work
//  if (!Stream_Module_Device_Tools::connected (graphBuilder_))
//  {
//    ACE_DEBUG ((LM_DEBUG,
//                ACE_TEXT ("reconnecting...\n")));
//
//    if (!Stream_Module_Device_Tools::connectFirst (graphBuilder_))
//    {
//      ACE_DEBUG ((LM_ERROR,
//                  ACE_TEXT ("failed to Stream_Module_Device_Tools::connectFirst(), aborting\n")));
//      goto error;
//    } // end IF
//  } // end IF
//  ACE_ASSERT (Stream_Module_Device_Tools::connected (graphBuilder_));
//
//  // debug info
//  ACE_OS::memset (&allocator_properties, 0, sizeof (allocator_properties));
//  result =
//      buffer_negotiation_p->GetAllocatorProperties (&allocator_properties);
//  if (FAILED (result)) // E_FAIL (0x80004005)
//  {
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("failed to IAMBufferNegotiation::GetAllocatorProperties(): \"%s\", aborting\n"),
//                ACE_TEXT (Common_Tools::error2String (result).c_str ())));
//    goto error;
//  } // end IF
//  ACE_DEBUG ((LM_DEBUG,
//              ACE_TEXT ("allocator properties (buffers/size/alignment/prefix): %d/%d/%d/%d\n"),
//              allocator_properties.cBuffers,
//              allocator_properties.cbBuffer,
//              allocator_properties.cbAlign,
//              allocator_properties.cbPrefix));
//  buffer_negotiation_p->Release ();
//  buffer_negotiation_p = NULL;
//
//  result = graphBuilder_->QueryInterface (IID_PPV_ARGS (&media_filter_p));
//  if (FAILED (result))
//  {
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("failed to IGraphBuilder::QueryInterface(IID_IMediaFilter): \"%s\", aborting\n"),
//                ACE_TEXT (Common_Tools::error2String (result).c_str ())));
//    goto error;
//  } // end IF
//  ACE_ASSERT (media_filter_p);
//  result = media_filter_p->SetSyncSource (NULL);
//  if (FAILED (result))
//  {
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("failed to IMediaFilter::SetSyncSource(): \"%s\", aborting\n"),
//                ACE_TEXT (Common_Tools::error2String (result).c_str ())));
//    goto error;
//  } // end IF
//  media_filter_p->Release ();
//  media_filter_p = NULL;
//
//  // ---------------------------------------------------------------------------
//
//  //if (!source_impl_p->initialize (inherited::state_))
//  //{
//  //  ACE_DEBUG ((LM_ERROR,
//  //              ACE_TEXT ("%s: failed to initialize module writer, aborting\n"),
//  //              module_p->name ()));
//  //  goto error;
//  //} // end IF
//  ////fileReader_impl_p->reset ();
//  //// *NOTE*: push()ing the module will open() it
//  ////         --> set the argument that is passed along (head module expects a
//  ////             handle to the session data)
//  //module_p->arg (inherited::sessionData_);
//
//  if (setupPipeline_in)
//    if (!inherited::setup (configuration_in.notificationStrategy))
//    {
//      ACE_DEBUG ((LM_ERROR,
//                  ACE_TEXT ("failed to setup pipeline, aborting\n")));
//      goto error;
//    } // end IF
//
//  return true;
//
//error:
//  if (release_builder)
//  {
//    configuration_r.moduleHandlerConfiguration->builder->Release ();
//    configuration_r.moduleHandlerConfiguration->builder = NULL;
//  } // end IF
//  if (graphBuilder_)
//    graphBuilder_->Release ();
//  //if (direct3D_manager_p)
//  //  direct3D_manager_p->Release ();
//  if (session_data_r.direct3DDevice)
//  {
//    session_data_r.direct3DDevice->Release ();
//    session_data_r.direct3DDevice = NULL;
//  } // end IF
//  if (session_data_r.format)
//    Stream_Module_Device_Tools::deleteMediaType (session_data_r.format);
//  session_data_r.resetToken = 0;
//
//  if (COM_initialized)
//    CoUninitialize ();
//
//  return false;
//}
//
//void
//Test_I_Target_DirectShow_Stream::ping ()
//{
//  STREAM_TRACE (ACE_TEXT ("Test_I_Target_DirectShow_Stream::ping"));
//
//  ACE_ASSERT (false);
//  ACE_NOTSUP;
//
//  ACE_NOTREACHED (return;)
//}
//
//bool
//Test_I_Target_DirectShow_Stream::collect (Test_I_RuntimeStatistic_t& data_out)
//{
//  STREAM_TRACE (ACE_TEXT ("Test_I_Target_DirectShow_Stream::collect"));
//
//  // sanity check(s)
//  ACE_ASSERT (inherited::sessionData_);
//
//  int result = -1;
//  Test_I_Target_DirectShow_SessionData& session_data_r =
//    const_cast<Test_I_Target_DirectShow_SessionData&> (inherited::sessionData_->get ());
//  Stream_Module_t* module_p =
//    const_cast<Stream_Module_t*> (inherited::find (ACE_TEXT_ALWAYS_CHAR ("RuntimeStatistic")));
//  if (!module_p)
//  {
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("failed to retrieve \"%s\" module handle, aborting\n"),
//                ACE_TEXT ("RuntimeStatistic")));
//    return false;
//  } // end IF
//  Test_I_Target_DirectShow_Module_Statistic_WriterTask_t* runtimeStatistic_impl_p =
//    dynamic_cast<Test_I_Target_DirectShow_Module_Statistic_WriterTask_t*> (module_p->writer ());
//  if (!runtimeStatistic_impl_p)
//  {
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("dynamic_cast<Stream_Module_Statistic_WriterTask_T> failed, aborting\n")));
//    return false;
//  } // end IF
//
//  // synch access
//  if (session_data_r.lock)
//  {
//    result = session_data_r.lock->acquire ();
//    if (result == -1)
//    {
//      ACE_DEBUG ((LM_ERROR,
//                  ACE_TEXT ("failed to ACE_SYNCH_MUTEX::acquire(): \"%m\", aborting\n")));
//      return false;
//    } // end IF
//  } // end IF
//
//  session_data_r.currentStatistic.timeStamp = COMMON_TIME_NOW;
//
//  // delegate to the statistics module...
//  bool result_2 = false;
//  try {
//    result_2 = runtimeStatistic_impl_p->collect (data_out);
//  } catch (...) {
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("caught exception in Common_IStatistic_T::collect(), continuing\n")));
//  }
//  if (!result)
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("failed to Common_IStatistic_T::collect(), aborting\n")));
//  else
//    session_data_r.currentStatistic = data_out;
//
//  if (session_data_r.lock)
//  {
//    result = session_data_r.lock->release ();
//    if (result == -1)
//      ACE_DEBUG ((LM_ERROR,
//                  ACE_TEXT ("failed to ACE_SYNCH_MUTEX::release(): \"%m\", continuing\n")));
//  } // end IF
//
//  return result_2;
//}
//
//void
//Test_I_Target_DirectShow_Stream::report () const
//{
//  STREAM_TRACE (ACE_TEXT ("Test_I_Target_DirectShow_Stream::report"));
//
////   Net_Module_Statistic_ReaderTask_t* runtimeStatistic_impl = NULL;
////   runtimeStatistic_impl = dynamic_cast<Net_Module_Statistic_ReaderTask_t*> (//runtimeStatistic_.writer ());
////   if (!runtimeStatistic_impl)
////   {
////     ACE_DEBUG ((LM_ERROR,
////                 ACE_TEXT ("dynamic_cast<Net_Module_Statistic_ReaderTask_t> failed, returning\n")));
////     return;
////   } // end IF
////
////   // delegate to this module...
////   return (runtimeStatistic_impl->report ());
//
//  ACE_ASSERT (false);
//  ACE_NOTSUP;
//
//  ACE_NOTREACHED (return;)
//}
//
//Test_I_Target_MediaFoundation_Stream::Test_I_Target_MediaFoundation_Stream (const std::string& name_in)
// : inherited (name_in)
//#if defined (ACE_WIN32) || defined (ACE_WIN64)
// , mediaSession_ (NULL)
// , referenceCount_ (1)
//#endif
//{
//  STREAM_TRACE (ACE_TEXT ("Test_I_Target_MediaFoundation_Stream::Test_I_Target_MediaFoundation_Stream"));
//
//}
//
//Test_I_Target_MediaFoundation_Stream::~Test_I_Target_MediaFoundation_Stream ()
//{
//  STREAM_TRACE (ACE_TEXT ("Test_I_Target_MediaFoundation_Stream::~Test_I_Target_MediaFoundation_Stream"));
//
//  inherited::shutdown ();
//}
//
//bool
//Test_I_Target_MediaFoundation_Stream::load (Stream_ModuleList_t& modules_out,
//                                            bool& delete_out)
//{
//  STREAM_TRACE (ACE_TEXT ("Test_I_Target_MediaFoundation_Stream::load"));
//
////  // initialize return value(s)
////  modules_out.clear ();
////  delete_out = false;
//
//  Stream_Module_t* module_p = NULL;
//  ACE_NEW_RETURN (module_p,
//                  Test_I_Target_MediaFoundation_Module_Display_Module (ACE_TEXT_ALWAYS_CHAR ("Display"),
//                                                                       NULL,
//                                                                       false),
//                  false);
//  modules_out.push_back (module_p);
//#if defined (ACE_WIN32) || defined (ACE_WIN64)
////  ACE_NEW_RETURN (module_p,
////                  Test_I_Target_MediaFoundation_Module_DisplayNull_Module (ACE_TEXT_ALWAYS_CHAR ("DisplayNull"),
////                                                                           NULL,
////                                                                           false),
////                  false);
////  modules_out.push_back (module_p);
//#endif
//  module_p = NULL;
//#if defined (ACE_WIN32) || defined (ACE_WIN64)
//  //Test_I_Target_MediaFoundation_Module_DirectShowSource_Module directShowSource_;
//  //Test_I_Target_MediaFoundation_Module_MediaFoundationSource_Module mediaFoundationSource_;
//#endif
//  ACE_NEW_RETURN (module_p,
//                  Test_I_Target_MediaFoundation_Module_RuntimeStatistic_Module (ACE_TEXT_ALWAYS_CHAR ("RuntimeStatistic"),
//                                                                                NULL,
//                                                                                false),
//                  false);
//  modules_out.push_back (module_p);
//  module_p = NULL;
//  ACE_NEW_RETURN (module_p,
//                  Test_I_Target_MediaFoundation_Module_Splitter_Module (ACE_TEXT_ALWAYS_CHAR ("Splitter"),
//                                                                        NULL,
//                                                                        false),
//                  false);
//  modules_out.push_back (module_p);
//  //Test_I_Target_MediaFoundation_Module_AVIDecoder_Module            decoder_;
//  //Test_I_Target_MediaFoundation_Module_Net_IO_Module                source_;
//
//  if (!inherited::load (modules_out,
//                        delete_out))
//  {
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("%s: failed to Stream_Module_Net_IO_Stream_T::load(), aborting\n")));
//    return false;
//  } // end IF
//  ACE_ASSERT (delete_out);
//
//  delete_out = true;
//
//  return true;
//}
//
//bool
//Test_I_Target_MediaFoundation_Stream::initialize (const Test_I_Target_MediaFoundation_StreamConfiguration& configuration_in,
//                                                  bool setupPipeline_in,
//                                                  bool resetSessionData_in)
//{
//  STREAM_TRACE (ACE_TEXT ("Test_I_Target_MediaFoundation_Stream::initialize"));
//
//  // sanity check(s)
//  ACE_ASSERT (!isRunning ());
//
//  // allocate a new session state, reset stream
//  // sanity check(s)
//  ACE_ASSERT (configuration_in.moduleHandlerConfiguration);
//  if (!inherited::initialize (configuration_in,
//                              false,
//                              resetSessionData_in))
//  {
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("%s: failed to Stream_Base_T::initialize(), aborting\n"),
//                ACE_TEXT (inherited::name ().c_str ())));
//    return false;
//  } // end IF
//  ACE_ASSERT (inherited::sessionData_);
//  Test_I_Target_MediaFoundation_SessionData& session_data_r =
//    const_cast<Test_I_Target_MediaFoundation_SessionData&> (inherited::sessionData_->get ());
//  // *TODO*: remove type inferences
//  session_data_r.lock = &(inherited::sessionDataLock_);
//  inherited::state_.currentSessionData = &session_data_r;
//  // *TODO*: remove type inferences
//  ACE_ASSERT (configuration_in.moduleHandlerConfiguration);
//#if defined (ACE_WIN32) || defined (ACE_WIN64)
//  ACE_ASSERT (configuration_in.moduleHandlerConfiguration->format);
//  ACE_ASSERT (!session_data_r.format);
//
//  HRESULT result = E_FAIL;
//
//  session_data_r.format =
//    static_cast<struct _AMMediaType*> (CoTaskMemAlloc (sizeof (struct _AMMediaType)));
//  if (!session_data_r.format)
//  {
//    ACE_DEBUG ((LM_CRITICAL,
//                ACE_TEXT ("failed to allocate memory, continuing\n")));
//    return false;
//  } // end IF
//  ACE_OS::memset (session_data_r.format, 0, sizeof (struct _AMMediaType));
//  ACE_ASSERT (!session_data_r.format->pbFormat);
//  result =
//    MFInitAMMediaTypeFromMFMediaType (configuration_in.moduleHandlerConfiguration->format,
//                                      GUID_NULL,
//                                      session_data_r.format);
//  if (FAILED (result))
//  {
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("failed to MFInitAMMediaTypeFromMFMediaType(): \"%s\", aborting\n"),
//                ACE_TEXT (Common_Tools::error2String (result).c_str ())));
//
//    // clean up
//    Stream_Module_Device_Tools::deleteMediaType (session_data_r.format);
//
//    return false;
//  } // end IF
//#else
//  session_data_r.format = configuration_in.moduleHandlerConfiguration->format;
//#endif
//  session_data_r.sessionID = configuration_in.sessionID;
//  //session_data_r.targetFileName =
//  //  configuration_in.moduleHandlerConfiguration->targetFileName;
//
//  ACE_ASSERT (configuration_in.moduleConfiguration);
//  //  configuration_in.moduleConfiguration.streamState = &state_;
//  Test_I_Target_MediaFoundation_StreamConfiguration& configuration_r =
//      const_cast<Test_I_Target_MediaFoundation_StreamConfiguration&> (configuration_in);
//  configuration_r.moduleHandlerConfiguration->stateMachineLock =
//    &inherited::state_.stateMachineLock;
//
//  // ---------------------------------------------------------------------------
//
//  //Test_I_Stream_Module_CamSource* source_impl_p = NULL;
//
//  //// ******************* Camera Source ************************
//  //Stream_Module_t* module_p =
//  //  const_cast<Stream_Module_t*> (inherited::find (ACE_TEXT_ALWAYS_CHAR ("CamSource")));
//  //if (!module_p)
//  //{
//  //  ACE_DEBUG ((LM_ERROR,
//  //              ACE_TEXT ("failed to retrieve \"%s\" module handle, aborting\n"),
//  //              ACE_TEXT ("CamSource")));
//  //  return false;
//  //} // end IF
//  //source_impl_p =
//  //  dynamic_cast<Test_I_Stream_Module_CamSource*> (module_p->writer ());
//  //if (!source_impl_p)
//  //{
//  //  ACE_DEBUG ((LM_ERROR,
//  //              ACE_TEXT ("dynamic_cast<Test_I_Stream_Module_CamSource> failed, aborting\n")));
//  //  return false;
//  //} // end IF
//
//  // ---------------------------------------------------------------------------
//
//  bool graph_loaded = false;
//  bool COM_initialized = false;
//  IMFTopology* topology_p = NULL;
//
//  result = CoInitializeEx (NULL,
//                           (COINIT_MULTITHREADED     |
//                            COINIT_DISABLE_OLE1DDE   |
//                            COINIT_SPEED_OVER_MEMORY));
//  if (FAILED (result))
//  {
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("failed to CoInitializeEx(): \"%s\", aborting\n"),
//                ACE_TEXT (Common_Tools::error2String (result).c_str ())));
//    return false;
//  } // end IF
//  COM_initialized = true;
//
//  UINT32 item_count = 0;
//  ULONG reference_count = 0;
//  std::string url_string = ACE_TEXT_ALWAYS_CHAR ("camstream");
//
//  // sanity check(s)
//  ACE_ASSERT (configuration_in.moduleHandlerConfiguration->format);
//
//  if (!Stream_Module_Device_Tools::loadTargetRendererTopology (url_string,
//                                                               configuration_in.moduleHandlerConfiguration->format,
//                                                               NULL,
//                                                               //configuration_in.moduleHandlerConfiguration->window,
//                                                               session_data_r.rendererNodeId,
//                                                               topology_p))
//  {
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("failed to Stream_Module_Device_Tools::loadTargetRendererTopology(\"%s\"), aborting\n"),
//                ACE_TEXT (configuration_in.moduleHandlerConfiguration->device.c_str ())));
//    goto error;
//  } // end IF
//  ACE_ASSERT (topology_p);
//  graph_loaded = true;
//
//#if defined (_DEBUG)
//  ACE_DEBUG ((LM_DEBUG,
//              ACE_TEXT ("input format: \"%s\"...\n"),
//              ACE_TEXT (Stream_Module_Device_Tools::mediaTypeToString (configuration_in.moduleHandlerConfiguration->format).c_str ())));
//#endif
//
//  if (mediaSession_)
//  {
//    // *TODO*: this crashes in CTopoNode::UnlinkInput ()...
//    //result = mediaSession_->Shutdown ();
//    //if (FAILED (result))
//    //  ACE_DEBUG ((LM_ERROR,
//    //              ACE_TEXT ("failed to IMFMediaSession::Shutdown(): \"%s\", continuing\n"),
//    //              ACE_TEXT (Common_Tools::error2String (result).c_str ())));
//    mediaSession_->Release ();
//    mediaSession_ = NULL;
//  } // end IF
//  if (configuration_in.moduleHandlerConfiguration->session)
//  {
//    reference_count =
//      configuration_in.moduleHandlerConfiguration->session->AddRef ();
//    mediaSession_ = configuration_in.moduleHandlerConfiguration->session;
//
//    if (!Stream_Module_Device_Tools::clear (mediaSession_))
//    {
//      ACE_DEBUG ((LM_ERROR,
//                  ACE_TEXT ("failed to Stream_Module_Device_Tools::clear(), aborting\n")));
//      goto error;
//    } // end IF
//  } // end IF
//
//  if (!Stream_Module_Device_Tools::setTopology (topology_p,
//                                                mediaSession_,
//                                                true))
//  {
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("failed to Stream_Module_Device_Tools::setTopology(), aborting\n")));
//    goto error;
//  } // end IF
//  topology_p->Release ();
//  topology_p = NULL;
//  ACE_ASSERT (mediaSession_);
//
//  if (!configuration_in.moduleHandlerConfiguration->session)
//  {
//    reference_count = mediaSession_->AddRef ();
//    configuration_in.moduleHandlerConfiguration->session = mediaSession_;
//  } // end IF
//
//  // ---------------------------------------------------------------------------
//
//  //if (!source_impl_p->initialize (inherited::state_))
//  //{
//  //  ACE_DEBUG ((LM_ERROR,
//  //              ACE_TEXT ("%s: failed to initialize module writer, aborting\n"),
//  //              module_p->name ()));
//  //  goto error;
//  //} // end IF
//  ////fileReader_impl_p->reset ();
//  //// *NOTE*: push()ing the module will open() it
//  ////         --> set the argument that is passed along (head module expects a
//  ////             handle to the session data)
//  //module_p->arg (inherited::sessionData_);
//
//  if (setupPipeline_in)
//    if (!inherited::setup (configuration_in.notificationStrategy))
//    {
//      ACE_DEBUG ((LM_ERROR,
//                  ACE_TEXT ("failed to setup pipeline, aborting\n")));
//      goto error;
//    } // end IF
//
//  // ---------------------------------------------------------------------------
//
//  return true;
//
//error:
//  if (session_data_r.direct3DDevice)
//  {
//    session_data_r.direct3DDevice->Release ();
//    session_data_r.direct3DDevice = NULL;
//  } // end IF
//  if (session_data_r.format)
//    Stream_Module_Device_Tools::deleteMediaType (session_data_r.format);
//  session_data_r.resetToken = 0;
//
//  if (COM_initialized)
//    CoUninitialize ();
//
//  return false;
//}
//
//void
//Test_I_Target_MediaFoundation_Stream::ping ()
//{
//  STREAM_TRACE (ACE_TEXT ("Test_I_Target_MediaFoundation_Stream::ping"));
//
//  ACE_ASSERT (false);
//  ACE_NOTSUP;
//
//  ACE_NOTREACHED (return;)
//}
//
//bool
//Test_I_Target_MediaFoundation_Stream::collect (Test_I_RuntimeStatistic_t& data_out)
//{
//  STREAM_TRACE (ACE_TEXT ("Test_I_Target_MediaFoundation_Stream::collect"));
//
//  // sanity check(s)
//  ACE_ASSERT (inherited::sessionData_);
//
//  int result = -1;
//  Test_I_Target_MediaFoundation_SessionData& session_data_r =
//    const_cast<Test_I_Target_MediaFoundation_SessionData&> (inherited::sessionData_->get ());
//  Stream_Module_t* module_p =
//    const_cast<Stream_Module_t*> (inherited::find (ACE_TEXT_ALWAYS_CHAR ("RuntimeStatistic")));
//  if (!module_p)
//  {
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("failed to retrieve \"%s\" module handle, aborting\n"),
//                ACE_TEXT ("RuntimeStatistic")));
//    return false;
//  } // end IF
//  Test_I_Target_MediaFoundation_Module_Statistic_WriterTask_t* runtimeStatistic_impl_p =
//    dynamic_cast<Test_I_Target_MediaFoundation_Module_Statistic_WriterTask_t*> (module_p->writer ());
//  if (!runtimeStatistic_impl_p)
//  {
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("dynamic_cast<Test_I_Target_MediaFoundation_Module_Statistic_WriterTask_t> failed, aborting\n")));
//    return false;
//  } // end IF
//
//  // synch access
//  if (session_data_r.lock)
//  {
//    result = session_data_r.lock->acquire ();
//    if (result == -1)
//    {
//      ACE_DEBUG ((LM_ERROR,
//                  ACE_TEXT ("failed to ACE_SYNCH_MUTEX::acquire(): \"%m\", aborting\n")));
//      return false;
//    } // end IF
//  } // end IF
//
//  session_data_r.currentStatistic.timeStamp = COMMON_TIME_NOW;
//
//  // delegate to the statistics module...
//  bool result_2 = false;
//  try {
//    result_2 = runtimeStatistic_impl_p->collect (data_out);
//  } catch (...) {
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("caught exception in Common_IStatistic_T::collect(), continuing\n")));
//  }
//  if (!result)
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("failed to Common_IStatistic_T::collect(), aborting\n")));
//  else
//    session_data_r.currentStatistic = data_out;
//
//  if (session_data_r.lock)
//  {
//    result = session_data_r.lock->release ();
//    if (result == -1)
//      ACE_DEBUG ((LM_ERROR,
//                  ACE_TEXT ("failed to ACE_SYNCH_MUTEX::release(): \"%m\", continuing\n")));
//  } // end IF
//
//  return result_2;
//}
//
//void
//Test_I_Target_MediaFoundation_Stream::report () const
//{
//  STREAM_TRACE (ACE_TEXT ("Test_I_Target_MediaFoundation_Stream::report"));
//
////   Net_Module_Statistic_ReaderTask_t* runtimeStatistic_impl = NULL;
////   runtimeStatistic_impl =
////     dynamic_cast<Net_Module_Statistic_ReaderTask_t*> (//runtimeStatistic_.writer ());
////   if (!runtimeStatistic_impl)
////   {
////     ACE_DEBUG ((LM_ERROR,
////                 ACE_TEXT ("dynamic_cast<Net_Module_Statistic_ReaderTask_t> failed, returning\n")));
////     return;
////   } // end IF
////
////   // delegate to this module
////   return (runtimeStatistic_impl->report ());
//
//  ACE_ASSERT (false);
//  ACE_NOTSUP;
//
//  ACE_NOTREACHED (return;)
//}
#endif
Test_I_Target_Stream::Test_I_Target_Stream (const std::string& name_in)
 : inherited (name_in)
{
  STREAM_TRACE (ACE_TEXT ("Test_I_Target_Stream::Test_I_Target_Stream"));

}

Test_I_Target_Stream::~Test_I_Target_Stream ()
{
  STREAM_TRACE (ACE_TEXT ("Test_I_Target_Stream::~Test_I_Target_Stream"));

  inherited::shutdown ();
}

bool
Test_I_Target_Stream::load (Stream_ModuleList_t& modules_out,
                            bool& delete_out)
{
  STREAM_TRACE (ACE_TEXT ("Test_I_Target_Stream::load"));

//  // initialize return value(s)
//  modules_out.clear ();
//  delete_out = false;

  Stream_Module_t* module_p = NULL;
  ACE_NEW_RETURN (module_p,
                  Test_I_Target_Module_Display_Module (ACE_TEXT_ALWAYS_CHAR ("Display"),
                                                       NULL,
                                                       false),
                  false);
  modules_out.push_back (module_p);
//  ACE_NEW_RETURN (module_p,
//                  Test_I_Target_Module_DisplayNull_Module (ACE_TEXT_ALWAYS_CHAR ("DisplayNull"),
//                                                           NULL,
//                                                           false),
//                  false);
//  modules_out.push_back (module_p);
  module_p = NULL;
  ACE_NEW_RETURN (module_p,
                  Test_I_Target_Module_RuntimeStatistic_Module (ACE_TEXT_ALWAYS_CHAR ("RuntimeStatistic"),
                                                                NULL,
                                                                false),
                  false);
  modules_out.push_back (module_p);
  module_p = NULL;
  ACE_NEW_RETURN (module_p,
                  Test_I_Target_Module_Splitter_Module (ACE_TEXT_ALWAYS_CHAR ("Splitter"),
                                                        NULL,
                                                        false),
                  false);
  modules_out.push_back (module_p);
  //Test_I_Target_Module_AVIDecoder_Module            decoder_;
  //Test_I_Target_Module_Net_IO_Module                source_;

  if (!inherited::load (modules_out,
                        delete_out))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: failed to Stream_Module_Net_IO_Stream_T::load(), aborting\n")));
    return false;
  } // end IF
  ACE_ASSERT (delete_out);

  delete_out = true;

  return true;
}

bool
Test_I_Target_Stream::initialize (const Test_I_Target_StreamConfiguration& configuration_in,
                                  bool setupPipeline_in,
                                  bool resetSessionData_in)
{
  STREAM_TRACE (ACE_TEXT ("Test_I_Target_Stream::initialize"));

  // sanity check(s)
  ACE_ASSERT (!isRunning ());

  // allocate a new session state, reset stream
  // sanity check(s)
  ACE_ASSERT (configuration_in.moduleHandlerConfiguration);
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
  Test_I_Target_SessionData& session_data_r =
    const_cast<Test_I_Target_SessionData&> (inherited::sessionData_->get ());
  // *TODO*: remove type inferences
  session_data_r.lock = &(inherited::sessionDataLock_);
  inherited::state_.currentSessionData = &session_data_r;
  // *TODO*: remove type inferences
  ACE_ASSERT (configuration_in.moduleHandlerConfiguration);
  session_data_r.format = configuration_in.moduleHandlerConfiguration->format;
  session_data_r.sessionID = configuration_in.sessionID;
  session_data_r.targetFileName =
    configuration_in.moduleHandlerConfiguration->targetFileName;

  ACE_ASSERT (configuration_in.moduleConfiguration);
  //  configuration_in.moduleConfiguration.streamState = &state_;
  Test_I_Target_StreamConfiguration& configuration_r =
      const_cast<Test_I_Target_StreamConfiguration&> (configuration_in);
  configuration_r.moduleHandlerConfiguration->stateMachineLock =
    &inherited::state_.stateMachineLock;

  // ---------------------------------------------------------------------------

  if (setupPipeline_in)
    if (!inherited::setup (configuration_in.notificationStrategy))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to setup pipeline, aborting\n")));
      goto error;
    } // end IF

  return true;

error:
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

bool
Test_I_Target_Stream::collect (Test_I_RuntimeStatistic_t& data_out)
{
  STREAM_TRACE (ACE_TEXT ("Test_I_Target_Stream::collect"));

  // sanity check(s)
  ACE_ASSERT (inherited::sessionData_);

  int result = -1;
  Test_I_Target_SessionData& session_data_r =
    const_cast<Test_I_Target_SessionData&> (inherited::sessionData_->get ());
  Stream_Module_t* module_p =
    const_cast<Stream_Module_t*> (inherited::find (ACE_TEXT_ALWAYS_CHAR ("RuntimeStatistic")));
  if (!module_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to retrieve \"%s\" module handle, aborting\n"),
                ACE_TEXT ("RuntimeStatistic")));
    return false;
  } // end IF
  Test_I_Target_Module_Statistic_WriterTask_t* runtimeStatistic_impl_p =
    dynamic_cast<Test_I_Target_Module_Statistic_WriterTask_t*> (module_p->writer ());
  if (!runtimeStatistic_impl_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("dynamic_cast<Stream_Module_Statistic_WriterTask_T> failed, aborting\n")));
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
  } // end IF

  session_data_r.currentStatistic.timeStamp = COMMON_TIME_NOW;

  // delegate to the statistics module
  bool result_2 = false;
  try {
    result_2 = runtimeStatistic_impl_p->collect (data_out);
  } catch (...) {
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
Test_I_Target_Stream::report () const
{
  STREAM_TRACE (ACE_TEXT ("Test_I_Target_Stream::report"));

//   Net_Module_Statistic_ReaderTask_t* runtimeStatistic_impl = NULL;
//   runtimeStatistic_impl = dynamic_cast<Net_Module_Statistic_ReaderTask_t*> (//runtimeStatistic_.writer ());
//   if (!runtimeStatistic_impl)
//   {
//     ACE_DEBUG ((LM_ERROR,
//                 ACE_TEXT ("dynamic_cast<Net_Module_Statistic_ReaderTask_t> failed, returning\n")));
//     return;
//   } // end IF
//
//   // delegate to this module
//   return (runtimeStatistic_impl->report ());

  ACE_ASSERT (false);
  ACE_NOTSUP;

  ACE_NOTREACHED (return;)
}
