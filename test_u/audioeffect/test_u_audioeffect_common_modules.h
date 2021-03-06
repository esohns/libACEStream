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
#include "common_timer_manager_common.h"

#include "stream_common.h"
#include "stream_streammodule_base.h"

#include "stream_dec_wav_encoder.h"
#if defined (ACE_WIN32) || defined (ACE_WIN64)
//#include "stream_dev_mic_source_directshow.h"
#include "stream_dev_mic_source_mediafoundation.h"
#include "stream_dev_mic_source_wavein.h"
#else
#include "stream_dec_sox_effect.h"
#include "stream_dev_mic_source_alsa.h"
#include "stream_dev_target_alsa.h"
#if defined (GUI_SUPPORT)
#if defined (GTK_USE)
#include "stream_vis_gtk_cairo.h"
#endif // GTK_USE
#endif // GUI_SUPPORT
#endif // ACE_WIN32 || ACE_WIN64
#include "stream_file_sink.h"

#include "stream_stat_statistic_analysis.h"
#include "stream_stat_statistic_report.h"

#if defined (GUI_SUPPORT)
#if defined (GTK_USE)
#include "stream_vis_gtk_cairo_spectrum_analyzer.h"
#endif // GTK_USE
#endif // GUI_SUPPORT

#include "test_u_audioeffect_common.h"
#include "test_u_audioeffect_message.h"
#include "test_u_audioeffect_session_message.h"

// declare module(s)
#if defined (ACE_WIN32) || defined (ACE_WIN64)
//typedef Stream_Dev_Mic_Source_DirectShow_T<ACE_MT_SYNCH,
//                                           Stream_ControlMessage_t,
//                                           Test_U_AudioEffect_DirectShow_Message,
//                                           Test_U_AudioEffect_DirectShow_SessionMessage,
//                                           struct Test_U_AudioEffect_DirectShow_ModuleHandlerConfiguration,
//                                           enum Stream_ControlType,
//                                           enum Stream_SessionMessageType,
//                                           struct Test_U_AudioEffect_DirectShow_StreamState,
//                                           Test_U_AudioEffect_DirectShow_SessionData,
//                                           Test_U_AudioEffect_DirectShow_SessionData_t,
//                                           struct Test_U_AudioEffect_Statistic,
//                                           Common_Timer_Manager_t> Test_U_Dev_Mic_Source_DirectShow;
//DATASTREAM_MODULE_INPUT_ONLY (Test_U_AudioEffect_DirectShow_SessionData,                // session data type
//                              enum Stream_SessionMessageType,                                  // session event type
//                              struct Test_U_AudioEffect_DirectShow_ModuleHandlerConfiguration, // module handler configuration type
//                              libacestream_default_dev_mic_source_directshow_module_name_string,
//                              Stream_INotify_t,                                                // stream notification interface type
//                              Test_U_Dev_Mic_Source_DirectShow);                               // writer type
typedef Stream_Dev_Mic_Source_MediaFoundation_T<ACE_MT_SYNCH,
                                                Stream_ControlMessage_t,
                                                Test_U_AudioEffect_MediaFoundation_Message,
                                                Test_U_AudioEffect_MediaFoundation_SessionMessage,
                                                struct Test_U_AudioEffect_MediaFoundation_ModuleHandlerConfiguration,
                                                enum Stream_ControlType,
                                                enum Stream_SessionMessageType,
                                                Test_U_AudioEffect_MediaFoundation_StreamState,
                                                Test_U_AudioEffect_MediaFoundation_SessionData,
                                                Test_U_AudioEffect_MediaFoundation_SessionData_t,
                                                struct Test_U_AudioEffect_Statistic,
                                                Common_Timer_Manager_t> Test_U_Dev_Mic_Source_MediaFoundation;
DATASTREAM_MODULE_INPUT_ONLY (Test_U_AudioEffect_MediaFoundation_SessionData,                // session data type
                              enum Stream_SessionMessageType,                                       // session event type
                              struct Test_U_AudioEffect_MediaFoundation_ModuleHandlerConfiguration, // module handler configuration type
                              libacestream_default_dev_mic_source_mediafoundation_module_name_string,
                              Stream_INotify_t,                                                     // stream notification interface type
                              Test_U_Dev_Mic_Source_MediaFoundation);                               // writer type
typedef Stream_Dev_Mic_Source_WaveIn_T<ACE_MT_SYNCH,
                                       Stream_ControlMessage_t,
                                       Test_U_AudioEffect_DirectShow_Message,
                                       Test_U_AudioEffect_DirectShow_SessionMessage,
                                       struct Test_U_AudioEffect_DirectShow_ModuleHandlerConfiguration,
                                       enum Stream_ControlType,
                                       enum Stream_SessionMessageType,
                                       struct Test_U_AudioEffect_DirectShow_StreamState,
                                       Test_U_AudioEffect_DirectShow_SessionData,
                                       Test_U_AudioEffect_DirectShow_SessionData_t,
                                       struct Test_U_AudioEffect_Statistic,
                                       Common_Timer_Manager_t> Test_U_Dev_Mic_Source_WaveIn;
DATASTREAM_MODULE_INPUT_ONLY (Test_U_AudioEffect_DirectShow_SessionData,                // session data type
                              enum Stream_SessionMessageType,                                  // session event type
                              struct Test_U_AudioEffect_DirectShow_ModuleHandlerConfiguration, // module handler configuration type
                              libacestream_default_dev_mic_source_wavein_module_name_string,
                              Stream_INotify_t,                                                // stream notification interface type
                              Test_U_Dev_Mic_Source_WaveIn);                                   // writer type
#else
typedef Stream_Dev_Mic_Source_ALSA_T<ACE_MT_SYNCH,
                                     Stream_ControlMessage_t,
                                     Test_U_AudioEffect_Message,
                                     Test_U_AudioEffect_SessionMessage,
                                     struct Test_U_AudioEffect_ALSA_ModuleHandlerConfiguration,
                                     enum Stream_ControlType,
                                     enum Stream_SessionMessageType,
                                     struct Test_U_AudioEffect_StreamState,
                                     struct Test_U_AudioEffect_SessionData,
                                     Test_U_AudioEffect_SessionData_t,
                                     struct Test_U_AudioEffect_Statistic,
                                     Common_Timer_Manager_t> Test_U_Dev_Mic_Source_ALSA;
DATASTREAM_MODULE_INPUT_ONLY (struct Test_U_AudioEffect_SessionData,                // session data type
                              enum Stream_SessionMessageType,                       // session event type
                              struct Test_U_AudioEffect_ALSA_ModuleHandlerConfiguration, // module handler configuration type
                              libacestream_default_dev_mic_source_alsa_module_name_string,
                              Stream_INotify_t,                                     // stream notification interface type
                              Test_U_Dev_Mic_Source_ALSA);                          // writer type
#endif // ACE_WIN32 || ACE_WIN64

#if defined (ACE_WIN32) || defined (ACE_WIN64)
typedef Stream_Statistic_StatisticAnalysis_T<ACE_MT_SYNCH,
                                             Common_TimePolicy_t,
                                             struct Test_U_AudioEffect_DirectShow_ModuleHandlerConfiguration,
                                             Stream_ControlMessage_t,
                                             Test_U_AudioEffect_DirectShow_Message,
                                             Test_U_AudioEffect_DirectShow_SessionMessage,
                                             struct Test_U_AudioEffect_Statistic,
                                             Test_U_AudioEffect_DirectShow_SessionData,
                                             Test_U_AudioEffect_DirectShow_SessionData_t,
                                             struct _AMMediaType,
                                             double, 1> Test_U_AudioEffect_DirectShow_StatisticAnalysis;
DATASTREAM_MODULE_INPUT_ONLY (Test_U_AudioEffect_DirectShow_SessionData,                // session data type
                              enum Stream_SessionMessageType,                                  // session event type
                              struct Test_U_AudioEffect_DirectShow_ModuleHandlerConfiguration, // module handler configuration type
                              libacestream_default_stat_analysis_module_name_string,
                              Stream_INotify_t,                                                // stream notification interface type
                              Test_U_AudioEffect_DirectShow_StatisticAnalysis);                // name

typedef Stream_Statistic_StatisticAnalysis_T<ACE_MT_SYNCH,
                                             Common_TimePolicy_t,
                                             struct Test_U_AudioEffect_MediaFoundation_ModuleHandlerConfiguration,
                                             Stream_ControlMessage_t,
                                             Test_U_AudioEffect_MediaFoundation_Message,
                                             Test_U_AudioEffect_MediaFoundation_SessionMessage,
                                             struct Test_U_AudioEffect_Statistic,
                                             Test_U_AudioEffect_MediaFoundation_SessionData,
                                             Test_U_AudioEffect_MediaFoundation_SessionData_t,
                                             IMFMediaType*,
                                             double, 1> Test_U_AudioEffect_MediaFoundation_StatisticAnalysis;
DATASTREAM_MODULE_INPUT_ONLY (Test_U_AudioEffect_MediaFoundation_SessionData,                // session data type
                              enum Stream_SessionMessageType,                                       // session event type
                              struct Test_U_AudioEffect_MediaFoundation_ModuleHandlerConfiguration, // module handler configuration type
                              libacestream_default_stat_analysis_module_name_string,
                              Stream_INotify_t,                                                     // stream notification interface type
                              Test_U_AudioEffect_MediaFoundation_StatisticAnalysis);                // name

typedef Stream_Statistic_StatisticReport_ReaderTask_T<ACE_MT_SYNCH,
                                                      Common_TimePolicy_t,
                                                      struct Test_U_AudioEffect_DirectShow_ModuleHandlerConfiguration,
                                                      Stream_ControlMessage_t,
                                                      Test_U_AudioEffect_DirectShow_Message,
                                                      Test_U_AudioEffect_DirectShow_SessionMessage,
                                                      Stream_CommandType_t,
                                                      struct Test_U_AudioEffect_Statistic,
                                                      Common_Timer_Manager_t,
                                                      Test_U_AudioEffect_DirectShow_SessionData,
                                                      Test_U_AudioEffect_DirectShow_SessionData_t> Test_U_AudioEffect_DirectShow_Statistic_ReaderTask_t;
typedef Stream_Statistic_StatisticReport_WriterTask_T<ACE_MT_SYNCH,
                                                      Common_TimePolicy_t,
                                                      struct Test_U_AudioEffect_DirectShow_ModuleHandlerConfiguration,
                                                      Stream_ControlMessage_t,
                                                      Test_U_AudioEffect_DirectShow_Message,
                                                      Test_U_AudioEffect_DirectShow_SessionMessage,
                                                      Stream_CommandType_t,
                                                      struct Test_U_AudioEffect_Statistic,
                                                      Common_Timer_Manager_t,
                                                      Test_U_AudioEffect_DirectShow_SessionData,
                                                      Test_U_AudioEffect_DirectShow_SessionData_t> Test_U_AudioEffect_DirectShow_Statistic_WriterTask_t;
DATASTREAM_MODULE_DUPLEX (Test_U_AudioEffect_DirectShow_SessionData,                // session data type
                          enum Stream_SessionMessageType,                                  // session event type
                          struct Test_U_AudioEffect_DirectShow_ModuleHandlerConfiguration, // module handler configuration type
                          libacestream_default_stat_report_module_name_string,
                          Stream_INotify_t,                                                // stream notification interface type
                          Test_U_AudioEffect_DirectShow_Statistic_ReaderTask_t,            // reader type
                          Test_U_AudioEffect_DirectShow_Statistic_WriterTask_t,            // writer type
                          Test_U_AudioEffect_DirectShow_StatisticReport);                  // name

typedef Stream_Statistic_StatisticReport_ReaderTask_T<ACE_MT_SYNCH,
                                                      Common_TimePolicy_t,
                                                      struct Test_U_AudioEffect_MediaFoundation_ModuleHandlerConfiguration,
                                                      Stream_ControlMessage_t,
                                                      Test_U_AudioEffect_MediaFoundation_Message,
                                                      Test_U_AudioEffect_MediaFoundation_SessionMessage,
                                                      Stream_CommandType_t,
                                                      struct Test_U_AudioEffect_Statistic,
                                                      Common_Timer_Manager_t,
                                                      Test_U_AudioEffect_MediaFoundation_SessionData,
                                                      Test_U_AudioEffect_MediaFoundation_SessionData_t> Test_U_AudioEffect_MediaFoundation_Statistic_ReaderTask_t;
typedef Stream_Statistic_StatisticReport_WriterTask_T<ACE_MT_SYNCH,
                                                      Common_TimePolicy_t,
                                                      struct Test_U_AudioEffect_MediaFoundation_ModuleHandlerConfiguration,
                                                      Stream_ControlMessage_t,
                                                      Test_U_AudioEffect_MediaFoundation_Message,
                                                      Test_U_AudioEffect_MediaFoundation_SessionMessage,
                                                      Stream_CommandType_t,
                                                      struct Test_U_AudioEffect_Statistic,
                                                      Common_Timer_Manager_t,
                                                      Test_U_AudioEffect_MediaFoundation_SessionData,
                                                      Test_U_AudioEffect_MediaFoundation_SessionData_t> Test_U_AudioEffect_MediaFoundation_Statistic_WriterTask_t;
DATASTREAM_MODULE_DUPLEX (Test_U_AudioEffect_MediaFoundation_SessionData,                // session data type
                          enum Stream_SessionMessageType,                                       // session event type
                          struct Test_U_AudioEffect_MediaFoundation_ModuleHandlerConfiguration, // module handler configuration type
                          libacestream_default_stat_report_module_name_string,
                          Stream_INotify_t,                                                     // stream notification interface type
                          Test_U_AudioEffect_MediaFoundation_Statistic_ReaderTask_t,            // reader type
                          Test_U_AudioEffect_MediaFoundation_Statistic_WriterTask_t,            // writer type
                          Test_U_AudioEffect_MediaFoundation_StatisticReport);                  // name
#else
typedef Stream_Statistic_StatisticAnalysis_T<ACE_MT_SYNCH,
                                             Common_TimePolicy_t,
                                             struct Test_U_AudioEffect_ALSA_ModuleHandlerConfiguration,
                                             Stream_ControlMessage_t,
                                             Test_U_AudioEffect_Message,
                                             Test_U_AudioEffect_SessionMessage,
                                             struct Test_U_AudioEffect_Statistic,
                                             struct Test_U_AudioEffect_SessionData,
                                             Test_U_AudioEffect_SessionData_t,
                                             struct Stream_MediaFramework_ALSA_MediaType,
                                             double, 1> Test_U_AudioEffect_StatisticAnalysis;
DATASTREAM_MODULE_INPUT_ONLY (struct Test_U_AudioEffect_SessionData,                // session data type
                              enum Stream_SessionMessageType,                       // session event type
                              struct Test_U_AudioEffect_ALSA_ModuleHandlerConfiguration, // module handler configuration type
                              libacestream_default_stat_analysis_module_name_string,
                              Stream_INotify_t,                                     // stream notification interface type
                              Test_U_AudioEffect_StatisticAnalysis);                // name

typedef Stream_Statistic_StatisticReport_ReaderTask_T<ACE_MT_SYNCH,
                                                      Common_TimePolicy_t,
                                                      struct Test_U_AudioEffect_ALSA_ModuleHandlerConfiguration,
                                                      ACE_Message_Block,
                                                      Test_U_AudioEffect_Message,
                                                      Test_U_AudioEffect_SessionMessage,
                                                      Stream_CommandType_t,
                                                      struct Test_U_AudioEffect_Statistic,
                                                      Common_Timer_Manager_t,
                                                      struct Test_U_AudioEffect_SessionData,
                                                      Test_U_AudioEffect_SessionData_t> Test_U_AudioEffect_Module_Statistic_ReaderTask_t;
typedef Stream_Statistic_StatisticReport_WriterTask_T<ACE_MT_SYNCH,
                                                      Common_TimePolicy_t,
                                                      struct Test_U_AudioEffect_ALSA_ModuleHandlerConfiguration,
                                                      ACE_Message_Block,
                                                      Test_U_AudioEffect_Message,
                                                      Test_U_AudioEffect_SessionMessage,
                                                      Stream_CommandType_t,
                                                      struct Test_U_AudioEffect_Statistic,
                                                      Common_Timer_Manager_t,
                                                      struct Test_U_AudioEffect_SessionData,
                                                      Test_U_AudioEffect_SessionData_t> Test_U_AudioEffect_Module_Statistic_WriterTask_t;
DATASTREAM_MODULE_DUPLEX (struct Test_U_AudioEffect_SessionData,                // session data type
                          enum Stream_SessionMessageType,                       // session event type
                          struct Test_U_AudioEffect_ALSA_ModuleHandlerConfiguration, // module handler configuration type
                          libacestream_default_stat_report_module_name_string,
                          Stream_INotify_t,                                     // stream notification interface type
                          Test_U_AudioEffect_Module_Statistic_ReaderTask_t,     // reader type
                          Test_U_AudioEffect_Module_Statistic_WriterTask_t,     // writer type
                          Test_U_AudioEffect_StatisticReport);                  // name
#endif // ACE_WIN32 || ACE_WIN64

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#if defined (GUI_SUPPORT)
#if defined (GTK_USE)
typedef Stream_Visualization_GTK_Cairo_SpectrumAnalyzer_T<ACE_MT_SYNCH,
                                                          Common_TimePolicy_t,
                                                          struct Test_U_AudioEffect_DirectShow_ModuleHandlerConfiguration,
                                                          Stream_ControlMessage_t,
                                                          Test_U_AudioEffect_DirectShow_Message,
                                                          Test_U_AudioEffect_DirectShow_SessionMessage,
                                                          Test_U_AudioEffect_DirectShow_SessionData,
                                                          Test_U_AudioEffect_DirectShow_SessionData_t,
                                                          Common_Timer_Manager_t,
                                                          struct _AMMediaType> Test_U_AudioEffect_DirectShow_Vis_SpectrumAnalyzer;
DATASTREAM_MODULE_INPUT_ONLY (Test_U_AudioEffect_DirectShow_SessionData,                // session data type
                              enum Stream_SessionMessageType,                                  // session event type
                              struct Test_U_AudioEffect_DirectShow_ModuleHandlerConfiguration, // module handler configuration type
                              libacestream_default_vis_spectrum_analyzer_module_name_string,
                              Stream_INotify_t,                                                // stream notification interface type
                              Test_U_AudioEffect_DirectShow_Vis_SpectrumAnalyzer);             // writer type
typedef Stream_Visualization_GTK_Cairo_SpectrumAnalyzer_T<ACE_MT_SYNCH,
                                                          Common_TimePolicy_t,
                                                          struct Test_U_AudioEffect_MediaFoundation_ModuleHandlerConfiguration,
                                                          Stream_ControlMessage_t,
                                                          Test_U_AudioEffect_MediaFoundation_Message,
                                                          Test_U_AudioEffect_MediaFoundation_SessionMessage,
                                                          Test_U_AudioEffect_MediaFoundation_SessionData,
                                                          Test_U_AudioEffect_MediaFoundation_SessionData_t,
                                                          Common_Timer_Manager_t,
                                                          IMFMediaType*> Test_U_AudioEffect_MediaFoundation_Vis_SpectrumAnalyzer;
DATASTREAM_MODULE_INPUT_ONLY (Test_U_AudioEffect_MediaFoundation_SessionData,                // session data type
                              enum Stream_SessionMessageType,                                       // session event type
                              struct Test_U_AudioEffect_MediaFoundation_ModuleHandlerConfiguration, // module handler configuration type
                              libacestream_default_vis_spectrum_analyzer_module_name_string,
                              Stream_INotify_t,                                                     // stream notification interface type
                              Test_U_AudioEffect_MediaFoundation_Vis_SpectrumAnalyzer);             // writer type
#endif // GTK_USE
#endif // GUI_SUPPORT
typedef Stream_Decoder_WAVEncoder_T<ACE_MT_SYNCH,
                                    Common_TimePolicy_t,
                                    struct Test_U_AudioEffect_DirectShow_ModuleHandlerConfiguration,
                                    Stream_ControlMessage_t,
                                    Test_U_AudioEffect_DirectShow_Message,
                                    Test_U_AudioEffect_DirectShow_SessionMessage,
                                    Test_U_AudioEffect_DirectShow_SessionData_t,
                                    Test_U_AudioEffect_DirectShow_SessionData,
                                    struct _AMMediaType,
                                    struct Test_U_UserData> Test_U_AudioEffect_DirectShow_WAVEncoder;
DATASTREAM_MODULE_INPUT_ONLY (Test_U_AudioEffect_DirectShow_SessionData,                // session data type
                              enum Stream_SessionMessageType,                                  // session event type
                              struct Test_U_AudioEffect_DirectShow_ModuleHandlerConfiguration, // module handler configuration type
                              libacestream_default_dec_wav_encoder_module_name_string,
                              Stream_INotify_t,                                                // stream notification interface type
                              Test_U_AudioEffect_DirectShow_WAVEncoder);                       // writer type
typedef Stream_Decoder_WAVEncoder_T<ACE_MT_SYNCH,
                                    Common_TimePolicy_t,
                                    struct Test_U_AudioEffect_MediaFoundation_ModuleHandlerConfiguration,
                                    Stream_ControlMessage_t,
                                    Test_U_AudioEffect_MediaFoundation_Message,
                                    Test_U_AudioEffect_MediaFoundation_SessionMessage,
                                    Test_U_AudioEffect_MediaFoundation_SessionData_t,
                                    Test_U_AudioEffect_MediaFoundation_SessionData,
                                    IMFMediaType*,
                                    struct Test_U_UserData> Test_U_AudioEffect_MediaFoundation_WAVEncoder;
DATASTREAM_MODULE_INPUT_ONLY (Test_U_AudioEffect_MediaFoundation_SessionData,                // session data type
                              enum Stream_SessionMessageType,                                       // session event type
                              struct Test_U_AudioEffect_MediaFoundation_ModuleHandlerConfiguration, // module handler configuration type
                              libacestream_default_dec_wav_encoder_module_name_string,
                              Stream_INotify_t,                                                     // stream notification interface type
                              Test_U_AudioEffect_MediaFoundation_WAVEncoder);                       // writer type
#else
typedef Stream_Decoder_SoXEffect_T<ACE_MT_SYNCH,
                                   Common_TimePolicy_t,
                                   struct Test_U_AudioEffect_ALSA_ModuleHandlerConfiguration,
                                   Stream_ControlMessage_t,
                                   Test_U_AudioEffect_Message,
                                   Test_U_AudioEffect_SessionMessage,
                                   Test_U_AudioEffect_SessionData_t,
                                   struct Test_U_AudioEffect_SessionData> Test_U_AudioEffect_SoXEffect;
DATASTREAM_MODULE_INPUT_ONLY (struct Test_U_AudioEffect_SessionData,                  // session data type
                              enum Stream_SessionMessageType,                         // session event type
                              struct Test_U_AudioEffect_ALSA_ModuleHandlerConfiguration,   // module handler configuration type
                              libacestream_default_dec_sox_effect_module_name_string,
                              Stream_INotify_t,                                       // stream notification interface type
                              Test_U_AudioEffect_SoXEffect);                          // writer type

typedef Stream_Dev_Target_ALSA_T<ACE_MT_SYNCH,
                                 Common_TimePolicy_t,
                                 struct Test_U_AudioEffect_ALSA_ModuleHandlerConfiguration,
                                 Stream_ControlMessage_t,
                                 Test_U_AudioEffect_Message,
                                 Test_U_AudioEffect_SessionMessage,
                                 Stream_SessionId_t,
                                 struct Test_U_AudioEffect_SessionData> Test_U_AudioEffect_Target_ALSA;
DATASTREAM_MODULE_INPUT_ONLY (struct Test_U_AudioEffect_SessionData,                   // session data type
                              enum Stream_SessionMessageType,                          // session event type
                              struct Test_U_AudioEffect_ALSA_ModuleHandlerConfiguration,    // module handler configuration type
                              libacestream_default_dev_target_alsa_module_name_string,
                              Stream_INotify_t,                                        // stream notification interface type
                              Test_U_AudioEffect_Target_ALSA);                         // writer type
#if defined (GUI_SUPPORT)
#if defined (GTK_USE)
typedef Stream_Visualization_GTK_Cairo_SpectrumAnalyzer_T<ACE_MT_SYNCH,
                                                          Common_TimePolicy_t,
                                                          struct Test_U_AudioEffect_ALSA_ModuleHandlerConfiguration,
                                                          Stream_ControlMessage_t,
                                                          Test_U_AudioEffect_Message,
                                                          Test_U_AudioEffect_SessionMessage,
                                                          struct Test_U_AudioEffect_SessionData,
                                                          Test_U_AudioEffect_SessionData_t,
                                                          Common_Timer_Manager_t,
                                                          struct Stream_MediaFramework_ALSA_MediaType> Test_U_AudioEffect_Vis_SpectrumAnalyzer;
DATASTREAM_MODULE_INPUT_ONLY (struct Test_U_AudioEffect_SessionData,                         // session data type
                              enum Stream_SessionMessageType,                                // session event type
                              struct Test_U_AudioEffect_ALSA_ModuleHandlerConfiguration,          // module handler configuration type
                              libacestream_default_vis_spectrum_analyzer_module_name_string,
                              Stream_INotify_t,                                              // stream notification interface type
                              Test_U_AudioEffect_Vis_SpectrumAnalyzer);                      // writer type
#endif // GTK_USE
#endif // GUI_SUPPORT
typedef Stream_Decoder_WAVEncoder_T<ACE_MT_SYNCH,
                                    Common_TimePolicy_t,
                                    struct Test_U_AudioEffect_ALSA_ModuleHandlerConfiguration,
                                    Stream_ControlMessage_t,
                                    Test_U_AudioEffect_Message,
                                    Test_U_AudioEffect_SessionMessage,
                                    Test_U_AudioEffect_SessionData_t,
                                    struct Test_U_AudioEffect_SessionData,
                                    struct Stream_MediaFramework_ALSA_MediaType,
                                    struct Test_U_AudioEffect_Statistic> Test_U_AudioEffect_ALSA_WAVEncoder;
DATASTREAM_MODULE_INPUT_ONLY (struct Test_U_AudioEffect_SessionData,                   // session data type
                              enum Stream_SessionMessageType,                          // session event type
                              struct Test_U_AudioEffect_ALSA_ModuleHandlerConfiguration,    // module handler configuration type
                              libacestream_default_dec_wav_encoder_module_name_string,
                              Stream_INotify_t,                                        // stream notification interface type
                              Test_U_AudioEffect_ALSA_WAVEncoder);                     // writer type
#endif // ACE_WIN32 || ACE_WIN64

#if defined (ACE_WIN32) || defined (ACE_WIN64)
typedef Stream_Module_FileWriter_T<ACE_MT_SYNCH,
                                   Common_TimePolicy_t,
                                   struct Test_U_AudioEffect_DirectShow_ModuleHandlerConfiguration,
                                   Stream_ControlMessage_t,
                                   Test_U_AudioEffect_DirectShow_Message,
                                   Test_U_AudioEffect_DirectShow_SessionMessage> Test_U_AudioEffect_DirectShow_FileWriter;
DATASTREAM_MODULE_INPUT_ONLY (Test_U_AudioEffect_DirectShow_SessionData,                // session data type
                              enum Stream_SessionMessageType,                                  // session event type
                              struct Test_U_AudioEffect_DirectShow_ModuleHandlerConfiguration, // module handler configuration type
                              libacestream_default_file_sink_module_name_string,
                              Stream_INotify_t,                                                // stream notification interface type
                              Test_U_AudioEffect_DirectShow_FileWriter);                       // writer type
typedef Stream_Module_FileWriter_T<ACE_MT_SYNCH,
                                   Common_TimePolicy_t,
                                   struct Test_U_AudioEffect_MediaFoundation_ModuleHandlerConfiguration,
                                   Stream_ControlMessage_t,
                                   Test_U_AudioEffect_MediaFoundation_Message,
                                   Test_U_AudioEffect_MediaFoundation_SessionMessage> Test_U_AudioEffect_MediaFoundation_FileWriter;
DATASTREAM_MODULE_INPUT_ONLY (Test_U_AudioEffect_MediaFoundation_SessionData,                // session data type
                              enum Stream_SessionMessageType,                                       // session event type
                              struct Test_U_AudioEffect_MediaFoundation_ModuleHandlerConfiguration, // module handler configuration type
                              libacestream_default_file_sink_module_name_string,
                              Stream_INotify_t,                                                     // stream notification interface type
                              Test_U_AudioEffect_MediaFoundation_FileWriter);                       // writer type
#else
typedef Stream_Module_FileWriter_T<ACE_MT_SYNCH,
                                   Common_TimePolicy_t,
                                   struct Test_U_AudioEffect_ALSA_ModuleHandlerConfiguration,
                                   Stream_ControlMessage_t,
                                   Test_U_AudioEffect_Message,
                                   Test_U_AudioEffect_SessionMessage> Test_U_AudioEffect_Module_FileWriter;
DATASTREAM_MODULE_INPUT_ONLY (struct Test_U_AudioEffect_SessionData,                // session data type
                              enum Stream_SessionMessageType,                       // session event type
                              struct Test_U_AudioEffect_ALSA_ModuleHandlerConfiguration, // module handler configuration type
                              libacestream_default_file_sink_module_name_string,
                              Stream_INotify_t,                                     // stream notification interface type
                              Test_U_AudioEffect_Module_FileWriter);                // writer type
#endif // ACE_WIN32 || ACE_WIN64

#endif
