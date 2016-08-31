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

#ifndef TEST_U_AUDIOEFFECT_COMMON_MODULES_H
#define TEST_U_AUDIOEFFECT_COMMON_MODULES_H

#include "ace/Synch_Traits.h"

#include "common_time_common.h"

#include "stream_common.h"
#include "stream_streammodule_base.h"

#include "stream_dec_avi_encoder.h"
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include "stream_dev_mic_source_directshow.h"
#include "stream_dev_mic_source_mediafoundation.h"
#endif
#include "stream_file_sink.h"
#include "stream_misc_runtimestatistic.h"

#include "test_u_audioeffect_common.h"
#include "test_u_audioeffect_message.h"
#include "test_u_audioeffect_session_message.h"

// declare module(s)
#if defined (ACE_WIN32) || defined (ACE_WIN64)
typedef Stream_Dev_Mic_Source_DirectShow_T<ACE_MT_SYNCH,
                                           Test_U_AudioEffect_DirectShow_ControlMessage_t,
                                           Test_U_AudioEffect_DirectShow_Message,
                                           Test_U_AudioEffect_DirectShow_SessionMessage,
                                           Test_U_AudioEffect_DirectShow_ModuleHandlerConfiguration,
                                           Stream_ControlType,
                                           Stream_SessionMessageType,
                                           Stream_State,
                                           Test_U_AudioEffect_DirectShow_SessionData,
                                           Test_U_AudioEffect_DirectShow_SessionData_t,
                                           Stream_Statistic> Test_U_Dev_Mic_Source_DirectShow;
DATASTREAM_MODULE_INPUT_ONLY (Test_U_AudioEffect_DirectShow_SessionData,                // session data type
                              Stream_SessionMessageType,                                // session event type
                              Test_U_AudioEffect_DirectShow_ModuleHandlerConfiguration, // module handler configuration type
                              Test_U_AudioEffect_IStreamNotify_t,                       // stream notification interface type
                              Test_U_Dev_Mic_Source_DirectShow);                        // writer type
typedef Stream_Dev_Mic_Source_MediaFoundation_T<ACE_MT_SYNCH,
                                                Test_U_AudioEffect_MediaFoundation_ControlMessage_t,
                                                Test_U_AudioEffect_MediaFoundation_Message,
                                                Test_U_AudioEffect_MediaFoundation_SessionMessage,
                                                Test_U_AudioEffect_MediaFoundation_ModuleHandlerConfiguration,
                                                Stream_ControlType,
                                                Stream_SessionMessageType,
                                                Stream_State,
                                                Test_U_AudioEffect_MediaFoundation_SessionData,
                                                Test_U_AudioEffect_MediaFoundation_SessionData_t,
                                                Stream_Statistic> Test_U_Dev_Mic_Source_MediaFoundation;
DATASTREAM_MODULE_INPUT_ONLY (Test_U_AudioEffect_MediaFoundation_SessionData,                // session data type
                              Stream_SessionMessageType,                                     // session event type
                              Test_U_AudioEffect_MediaFoundation_ModuleHandlerConfiguration, // module handler configuration type
                              Test_U_AudioEffect_IStreamNotify_t,                            // stream notification interface type
                              Test_U_Dev_Mic_Source_MediaFoundation);                        // writer type
#else
typedef Stream_Dev_Mic_Source_MediaFoundation_T<ACE_MT_SYNCH,
                                                Test_U_AudioEffect_ControlMessage_t,
                                                Test_U_AudioEffect_Message,
                                                Test_U_AudioEffect_SessionMessage,
                                                Test_U_AudioEffect_ModuleHandlerConfiguration,
                                                Stream_ControlType,
                                                Stream_SessionMessageType,
                                                Stream_State,
                                                Test_U_AudioEffect_SessionData,
                                                Test_U_AudioEffect_SessionData_t,
                                                Stream_Statistic> Test_U_Dev_Mic_Source_MediaFoundation;
DATASTREAM_MODULE_INPUT_ONLY (Test_U_AudioEffect_SessionData,                // session data type
                              Stream_SessionMessageType,                     // session event type
                              Test_U_AudioEffect_ModuleHandlerConfiguration, // module handler configuration type
                              Test_U_AudioEffect_IStreamNotify_t,            // stream notification interface type
                              Test_U_Dev_Mic_Source_MediaFoundation);        // writer type
#endif

#if defined (ACE_WIN32) || defined (ACE_WIN64)
typedef Stream_Module_Statistic_ReaderTask_T<ACE_MT_SYNCH,
                                             Common_TimePolicy_t,
                                             Test_U_AudioEffect_DirectShow_ModuleHandlerConfiguration,
                                             Test_U_AudioEffect_DirectShow_ControlMessage_t,
                                             Test_U_AudioEffect_DirectShow_Message,
                                             Test_U_AudioEffect_DirectShow_SessionMessage,
                                             Stream_CommandType_t,
                                             Stream_Statistic,
                                             Test_U_AudioEffect_DirectShow_SessionData,
                                             Test_U_AudioEffect_DirectShow_SessionData_t> Test_U_AudioEffect_DirectShow_Statistic_ReaderTask_t;
typedef Stream_Module_Statistic_WriterTask_T<ACE_MT_SYNCH,
                                             Common_TimePolicy_t,
                                             Test_U_AudioEffect_DirectShow_ModuleHandlerConfiguration,
                                             Test_U_AudioEffect_DirectShow_ControlMessage_t,
                                             Test_U_AudioEffect_DirectShow_Message,
                                             Test_U_AudioEffect_DirectShow_SessionMessage,
                                             Stream_CommandType_t,
                                             Stream_Statistic,
                                             Test_U_AudioEffect_DirectShow_SessionData,
                                             Test_U_AudioEffect_DirectShow_SessionData_t> Test_U_AudioEffect_DirectShow_Statistic_WriterTask_t;
DATASTREAM_MODULE_DUPLEX (Test_U_AudioEffect_DirectShow_SessionData,                // session data type
                          Stream_SessionMessageType,                                // session event type
                          Test_U_AudioEffect_DirectShow_ModuleHandlerConfiguration, // module handler configuration type
                          Test_U_AudioEffect_IStreamNotify_t,                       // stream notification interface type
                          Test_U_AudioEffect_DirectShow_Statistic_ReaderTask_t,     // reader type
                          Test_U_AudioEffect_DirectShow_Statistic_WriterTask_t,     // writer type
                          Test_U_AudioEffect_DirectShow_RuntimeStatistic);          // name
typedef Stream_Module_Statistic_ReaderTask_T<ACE_MT_SYNCH,
                                             Common_TimePolicy_t,
                                             Test_U_AudioEffect_MediaFoundation_ModuleHandlerConfiguration,
                                             Test_U_AudioEffect_MediaFoundation_ControlMessage_t,
                                             Test_U_AudioEffect_MediaFoundation_Message,
                                             Test_U_AudioEffect_MediaFoundation_SessionMessage,
                                             Stream_CommandType_t,
                                             Stream_Statistic,
                                             Test_U_AudioEffect_MediaFoundation_SessionData,
                                             Test_U_AudioEffect_MediaFoundation_SessionData_t> Test_U_AudioEffect_MediaFoundation_Statistic_ReaderTask_t;
typedef Stream_Module_Statistic_WriterTask_T<ACE_MT_SYNCH,
                                             Common_TimePolicy_t,
                                             Test_U_AudioEffect_MediaFoundation_ModuleHandlerConfiguration,
                                             Test_U_AudioEffect_MediaFoundation_ControlMessage_t,
                                             Test_U_AudioEffect_MediaFoundation_Message,
                                             Test_U_AudioEffect_MediaFoundation_SessionMessage,
                                             Stream_CommandType_t,
                                             Stream_Statistic,
                                             Test_U_AudioEffect_MediaFoundation_SessionData,
                                             Test_U_AudioEffect_MediaFoundation_SessionData_t> Test_U_AudioEffect_MediaFoundation_Statistic_WriterTask_t;
DATASTREAM_MODULE_DUPLEX (Test_U_AudioEffect_MediaFoundation_SessionData,                // session data type
                          Stream_SessionMessageType,                                     // session event type
                          Test_U_AudioEffect_MediaFoundation_ModuleHandlerConfiguration, // module handler configuration type
                          Test_U_AudioEffect_IStreamNotify_t,                            // stream notification interface type
                          Test_U_AudioEffect_MediaFoundation_Statistic_ReaderTask_t,     // reader type
                          Test_U_AudioEffect_MediaFoundation_Statistic_WriterTask_t,     // writer type
                          Test_U_AudioEffect_MediaFoundation_RuntimeStatistic);          // name
#else
typedef Stream_Module_Statistic_ReaderTask_T<ACE_MT_SYNCH,
                                             Common_TimePolicy_t,
                                             Test_U_AudioEffect_ModuleHandlerConfiguration,
                                             ACE_Message_Block,
                                             Test_U_AudioEffect_Message,
                                             Test_U_AudioEffect_SessionMessage,
                                             Stream_CommandType_t,
                                             Stream_Statistic,
                                             Test_U_AudioEffect_SessionData,
                                             Test_U_AudioEffect_SessionData_t> Test_U_AudioEffect_Module_Statistic_ReaderTask_t;
typedef Stream_Module_Statistic_WriterTask_T<ACE_MT_SYNCH,
                                             Common_TimePolicy_t,
                                             Test_U_AudioEffect_ModuleHandlerConfiguration,
                                             ACE_Message_Block,
                                             Test_U_AudioEffect_Message,
                                             Test_U_AudioEffect_SessionMessage,
                                             Stream_CommandType_t,
                                             Stream_Statistic,
                                             Test_U_AudioEffect_SessionData,
                                             Test_U_AudioEffect_SessionData_t> Test_U_AudioEffect_Module_Statistic_WriterTask_t;
DATASTREAM_MODULE_DUPLEX (Test_U_AudioEffect_SessionData,                   // session data type
                          Stream_SessionMessageType,                        // session event type
                          Test_U_AudioEffect_ModuleHandlerConfiguration,    // module handler configuration type
                          Test_U_AudioEffect_IStreamNotify_t,               // stream notification interface type
                          Test_U_AudioEffect_Module_Statistic_ReaderTask_t, // reader type
                          Test_U_AudioEffect_Module_Statistic_WriterTask_t, // writer type
                          Test_U_AudioEffect_Module_RuntimeStatistic);      // name
#endif

#if defined (ACE_WIN32) || defined (ACE_WIN64)
typedef Stream_Decoder_WAVEncoder_T<ACE_MT_SYNCH,
                                    Common_TimePolicy_t,
                                    Test_U_AudioEffect_DirectShow_ModuleHandlerConfiguration,
                                    Test_U_AudioEffect_DirectShow_ControlMessage_t,
                                    Test_U_AudioEffect_DirectShow_Message,
                                    Test_U_AudioEffect_DirectShow_SessionMessage,
                                    Test_U_AudioEffect_DirectShow_SessionData_t,
                                    Test_U_AudioEffect_DirectShow_SessionData> Test_U_AudioEffect_DirectShow_WAVEncoder;
DATASTREAM_MODULE_INPUT_ONLY (Test_U_AudioEffect_DirectShow_SessionData,                // session data type
                              Stream_SessionMessageType,                                // session event type
                              Test_U_AudioEffect_DirectShow_ModuleHandlerConfiguration, // module handler configuration type
                              Test_U_AudioEffect_IStreamNotify_t,                       // stream notification interface type
                              Test_U_AudioEffect_DirectShow_WAVEncoder);                // writer type
typedef Stream_Decoder_WAVEncoder_T<ACE_MT_SYNCH,
                                    Common_TimePolicy_t,
                                    Test_U_AudioEffect_MediaFoundation_ModuleHandlerConfiguration,
                                    Test_U_AudioEffect_MediaFoundation_ControlMessage_t,
                                    Test_U_AudioEffect_MediaFoundation_Message,
                                    Test_U_AudioEffect_MediaFoundation_SessionMessage,
                                    Test_U_AudioEffect_MediaFoundation_SessionData_t,
                                    Test_U_AudioEffect_MediaFoundation_SessionData> Test_U_AudioEffect_MediaFoundation_WAVEncoder;
DATASTREAM_MODULE_INPUT_ONLY (Test_U_AudioEffect_MediaFoundation_SessionData,                // session data type
                              Stream_SessionMessageType,                                     // session event type
                              Test_U_AudioEffect_MediaFoundation_ModuleHandlerConfiguration, // module handler configuration type
                              Test_U_AudioEffect_IStreamNotify_t,                            // stream notification interface type
                              Test_U_AudioEffect_MediaFoundation_WAVEncoder);                // writer type
#else
typedef Stream_Decoder_WAVEncoder_T<ACE_MT_SYNCH,
                                    Common_TimePolicy_t,
                                    Test_U_AudioEffect_ModuleHandlerConfiguration,
                                    Test_U_AudioEffect_ControlMessage_t,
                                    Test_U_AudioEffect_Message,
                                    Test_U_AudioEffect_SessionMessage,
                                    Test_U_AudioEffect_SessionData_t,
                                    Test_U_AudioEffect_SessionData> Test_U_AudioEffect_WAVEncoder;
DATASTREAM_MODULE_INPUT_ONLY (Test_U_AudioEffect_SessionData,                // session data type
                              Stream_SessionMessageType,                     // session event type
                              Test_U_AudioEffect_ModuleHandlerConfiguration, // module handler configuration type
                              Test_U_AudioEffect_IStreamNotify_t,            // stream notification interface type
                              Test_U_AudioEffect_WAVEncoder);                // writer type
#endif

#if defined (ACE_WIN32) || defined (ACE_WIN64)
typedef Stream_Module_FileWriter_T<ACE_MT_SYNCH,
                                   Common_TimePolicy_t,
                                   Test_U_AudioEffect_DirectShow_ModuleHandlerConfiguration,
                                   Test_U_AudioEffect_DirectShow_ControlMessage_t,
                                   Test_U_AudioEffect_DirectShow_Message,
                                   Test_U_AudioEffect_DirectShow_SessionMessage,
                                   Test_U_AudioEffect_DirectShow_SessionData> Test_U_AudioEffect_DirectShow_FileWriter;
DATASTREAM_MODULE_INPUT_ONLY (Test_U_AudioEffect_DirectShow_SessionData,                // session data type
                              Stream_SessionMessageType,                                // session event type
                              Test_U_AudioEffect_DirectShow_ModuleHandlerConfiguration, // module handler configuration type
                              Test_U_AudioEffect_IStreamNotify_t,                       // stream notification interface type
                              Test_U_AudioEffect_DirectShow_FileWriter);                // writer type
typedef Stream_Module_FileWriter_T<ACE_MT_SYNCH,
                                   Common_TimePolicy_t,
                                   Test_U_AudioEffect_MediaFoundation_ModuleHandlerConfiguration,
                                   Test_U_AudioEffect_MediaFoundation_ControlMessage_t,
                                   Test_U_AudioEffect_MediaFoundation_Message,
                                   Test_U_AudioEffect_MediaFoundation_SessionMessage,
                                   Test_U_AudioEffect_MediaFoundation_SessionData> Test_U_AudioEffect_MediaFoundation_FileWriter;
DATASTREAM_MODULE_INPUT_ONLY (Test_U_AudioEffect_MediaFoundation_SessionData,                // session data type
                              Stream_SessionMessageType,                                     // session event type
                              Test_U_AudioEffect_MediaFoundation_ModuleHandlerConfiguration, // module handler configuration type
                              Test_U_AudioEffect_IStreamNotify_t,                            // stream notification interface type
                              Test_U_AudioEffect_MediaFoundation_FileWriter);                // writer type
#else
typedef Stream_Module_FileWriter_T<ACE_MT_SYNCH,
                                   Common_TimePolicy_t,
                                   Test_U_AudioEffect_ModuleHandlerConfiguration,
                                   ACE_Message_Block,
                                   Test_U_AudioEffect_Message,
                                   Test_U_AudioEffect_SessionMessage,
                                   Test_U_AudioEffect_SessionData> Test_U_AudioEffect_Module_FileWriter;
DATASTREAM_MODULE_INPUT_ONLY (Test_U_AudioEffect_SessionData,                // session data type
                              Stream_SessionMessageType,                  // session event type
                              Test_U_AudioEffect_ModuleHandlerConfiguration, // module handler configuration type
                              Test_U_AudioEffect_IStreamNotify_t,            // stream notification interface type
                              Test_U_AudioEffect_Module_FileWriter);         // writer type
#endif

#endif
