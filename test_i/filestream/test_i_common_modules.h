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

#ifndef TEST_I_COMMON_MODULES_H
#define TEST_I_COMMON_MODULES_H

#include "ace/INET_Addr.h"
#include "ace/Message_Block.h"
#include "ace/Synch_Traits.h"

#include "common_time_common.h"

#include "stream_common.h"
#include "stream_streammodule_base.h"

#include "stream_file_sink.h"
#include "stream_file_source.h"
#include "stream_misc_runtimestatistic.h"
#include "stream_module_io.h"
//#include "stream_module_tcpsource.h"
//#include "stream_module_tcptarget.h"

#include "test_i_connection_common.h"
#include "test_i_message.h"
#include "test_i_module_eventhandler.h"
#include "test_i_session_message.h"
#include "test_i_source_common.h"
#include "test_i_target_common.h"

// outbound
typedef Stream_Module_FileReader_T<ACE_MT_SYNCH,
                                   Test_I_Source_ControlMessage_t,
                                   Test_I_Source_Message_t,
                                   Test_I_Source_SessionMessage,
                                   Test_I_Source_ModuleHandlerConfiguration,
                                   int,
                                   Stream_SessionMessageType,
                                   Test_I_Source_StreamState,
                                   Test_I_Source_SessionData,
                                   Test_I_Source_SessionData_t,
                                   Test_I_RuntimeStatistic_t> Test_I_Module_FileReader;
DATASTREAM_MODULE_INPUT_ONLY (Test_I_Source_SessionData,                // session data type
                              Stream_SessionMessageType,                // session event type
                              Test_I_Source_ModuleHandlerConfiguration, // module handler configuration type
                              Test_I_IStreamNotify_t,                   // stream notification interface type
                              Test_I_Module_FileReader);                // writer type

typedef Stream_Module_Statistic_ReaderTask_T<ACE_MT_SYNCH,
                                             Common_TimePolicy_t,
                                             Test_I_Source_ModuleHandlerConfiguration,
                                             Test_I_Source_ControlMessage_t,
                                             Test_I_Source_Message_t,
                                             Test_I_Source_SessionMessage,
                                             Stream_CommandType_t,
                                             Test_I_RuntimeStatistic_t,
                                             Test_I_Source_SessionData,
                                             Test_I_Source_SessionData_t> Test_I_Source_Module_Statistic_ReaderTask_t;
typedef Stream_Module_Statistic_WriterTask_T<ACE_MT_SYNCH,
                                             Common_TimePolicy_t,
                                             Test_I_Source_ModuleHandlerConfiguration,
                                             Test_I_Source_ControlMessage_t,
                                             Test_I_Source_Message_t,
                                             Test_I_Source_SessionMessage,
                                             Stream_CommandType_t,
                                             Test_I_RuntimeStatistic_t,
                                             Test_I_Source_SessionData,
                                             Test_I_Source_SessionData_t> Test_I_Source_Module_Statistic_WriterTask_t;
DATASTREAM_MODULE_DUPLEX (Test_I_Source_SessionData,                   // session data type
                          Stream_SessionMessageType,                   // session event type
                          Test_I_Source_ModuleHandlerConfiguration,    // module handler configuration type
                          Test_I_IStreamNotify_t,                      // stream notification interface type
                          Test_I_Source_Module_Statistic_ReaderTask_t, // reader type
                          Test_I_Source_Module_Statistic_WriterTask_t, // writer type
                          Test_I_Source_Module_RuntimeStatistic);      // name

// outbound
typedef Stream_Module_Net_IOWriter_T<ACE_MT_SYNCH,
                                     Test_I_Source_ControlMessage_t,
                                     Test_I_Source_Message_t,
                                     Test_I_Source_SessionMessage,
                                     Test_I_Source_ModuleHandlerConfiguration,
                                     int,
                                     Stream_SessionMessageType,
                                     Test_I_Source_StreamState,
                                     Test_I_Source_SessionData,
                                     Test_I_Source_SessionData_t,
                                     Test_I_RuntimeStatistic_t,
                                     ACE_INET_Addr,
                                     Test_I_Source_InetConnectionManager_t> Test_I_Source_Module_Net_Writer_t;
typedef Stream_Module_Net_IOReader_T<ACE_MT_SYNCH,
                                     Common_TimePolicy_t,
                                     Test_I_Source_ModuleHandlerConfiguration,
                                     Test_I_Source_ControlMessage_t,
                                     Test_I_Source_Message_t,
                                     Test_I_Source_SessionMessage,
                                     Test_I_Source_SessionData,
                                     Test_I_Source_SessionData_t,
                                     ACE_INET_Addr,
                                     Test_I_Source_InetConnectionManager_t> Test_I_Source_Module_Net_Reader_t;
DATASTREAM_MODULE_DUPLEX (Test_I_Source_SessionData,                // session data type
                          Stream_SessionMessageType,                // session event type
                          Test_I_Source_ModuleHandlerConfiguration, // module handler configuration type
                          Test_I_IStreamNotify_t,                   // stream notification interface type
                          Test_I_Source_Module_Net_Reader_t,        // reader type
                          Test_I_Source_Module_Net_Writer_t,        // writer type
                          Test_I_Source_Module_Net_IO);             // name
// inbound
typedef Stream_Module_Net_IOWriter_T<ACE_MT_SYNCH,
                                     Test_I_Target_ControlMessage_t,
                                     Test_I_Target_Message_t,
                                     Test_I_Target_SessionMessage,
                                     Test_I_Target_ModuleHandlerConfiguration,
                                     int,
                                     Stream_SessionMessageType,
                                     Test_I_Target_StreamState,
                                     Test_I_Target_SessionData,
                                     Test_I_Target_SessionData_t,
                                     Test_I_RuntimeStatistic_t,
                                     ACE_INET_Addr,
                                     Test_I_Target_InetConnectionManager_t> Test_I_Target_Module_Net_Writer_t;
typedef Stream_Module_Net_IOReader_T<ACE_MT_SYNCH,
                                     Common_TimePolicy_t,
                                     Test_I_Target_ModuleHandlerConfiguration,
                                     Test_I_Target_ControlMessage_t,
                                     Test_I_Target_Message_t,
                                     Test_I_Target_SessionMessage,
                                     Test_I_Target_SessionData,
                                     Test_I_Target_SessionData_t,
                                     ACE_INET_Addr,
                                     Test_I_Target_InetConnectionManager_t> Test_I_Target_Module_Net_Reader_t;
DATASTREAM_MODULE_DUPLEX (Test_I_Target_SessionData,                // session data type
                          Stream_SessionMessageType,                // session event type
                          Test_I_Target_ModuleHandlerConfiguration, // module handler configuration type
                          Test_I_IStreamNotify_t,                   // stream notification interface type
                          Test_I_Target_Module_Net_Reader_t,        // reader type
                          Test_I_Target_Module_Net_Writer_t,        // writer type
                          Test_I_Target_Module_Net_IO);             // name

typedef Stream_Module_Statistic_ReaderTask_T<ACE_MT_SYNCH,
                                             Common_TimePolicy_t,
                                             Test_I_Target_ModuleHandlerConfiguration,
                                             Test_I_Target_ControlMessage_t,
                                             Test_I_Target_Message_t,
                                             Test_I_Target_SessionMessage,
                                             Stream_CommandType_t,
                                             Test_I_RuntimeStatistic_t,
                                             Test_I_Target_SessionData,
                                             Test_I_Target_SessionData_t> Test_I_Target_Module_Statistic_ReaderTask_t;
typedef Stream_Module_Statistic_WriterTask_T<ACE_MT_SYNCH,
                                             Common_TimePolicy_t,
                                             Test_I_Target_ModuleHandlerConfiguration,
                                             Test_I_Target_ControlMessage_t,
                                             Test_I_Target_Message_t,
                                             Test_I_Target_SessionMessage,
                                             Stream_CommandType_t,
                                             Test_I_RuntimeStatistic_t,
                                             Test_I_Target_SessionData,
                                             Test_I_Target_SessionData_t> Test_I_Target_Module_Statistic_WriterTask_t;
DATASTREAM_MODULE_DUPLEX (Test_I_Target_SessionData,                   // session data type
                          Stream_SessionMessageType,                   // session event type
                          Test_I_Target_ModuleHandlerConfiguration,    // module handler configuration type
                          Test_I_IStreamNotify_t,                      // stream notification interface type
                          Test_I_Target_Module_Statistic_ReaderTask_t, // reader type
                          Test_I_Target_Module_Statistic_WriterTask_t, // writer type
                          Test_I_Target_Module_RuntimeStatistic);      // name

typedef Stream_Module_FileWriter_T<ACE_MT_SYNCH,
                                   Common_TimePolicy_t,
                                   Test_I_Target_ModuleHandlerConfiguration,
                                   Test_I_Target_ControlMessage_t,
                                   Test_I_Target_Message_t,
                                   Test_I_Target_SessionMessage,
                                   Test_I_Target_SessionData> Test_I_Module_FileWriter;
DATASTREAM_MODULE_INPUT_ONLY (Test_I_Target_SessionData,                // session data type
                              Stream_SessionMessageType,                // session event type
                              Test_I_Target_ModuleHandlerConfiguration, // module handler configuration type
                              Test_I_IStreamNotify_t,                   // stream notification interface type
                              Test_I_Module_FileWriter);                // writer type

typedef Test_I_Stream_Module_EventHandler_T<Stream_ModuleConfiguration,
                                            Test_I_Source_ModuleHandlerConfiguration,
                                            Test_I_Source_ControlMessage_t,
                                            Test_I_Source_Message_t,
                                            Test_I_Source_SessionMessage,
                                            Test_I_Source_SessionData,
                                            Test_I_Source_SessionData_t> Test_I_Source_Module_EventHandler;
typedef Test_I_Stream_Module_EventHandler_T<Stream_ModuleConfiguration,
                                            Test_I_Target_ModuleHandlerConfiguration,
                                            Test_I_Target_ControlMessage_t,
                                            Test_I_Target_Message_t,
                                            Test_I_Target_SessionMessage,
                                            Test_I_Target_SessionData,
                                            Test_I_Target_SessionData_t> Test_I_Target_Module_EventHandler;
DATASTREAM_MODULE_INPUT_ONLY (Test_I_Source_SessionData,                // session data type
                              Stream_SessionMessageType,                // session event type
                              Test_I_Source_ModuleHandlerConfiguration, // module handler configuration type
                              Test_I_IStreamNotify_t,                   // stream notification interface type
                              Test_I_Source_Module_EventHandler);       // writer type
DATASTREAM_MODULE_INPUT_ONLY (Test_I_Target_SessionData,                // session data type
                              Stream_SessionMessageType,                // session event type
                              Test_I_Target_ModuleHandlerConfiguration, // module handler configuration type
                              Test_I_IStreamNotify_t,                   // stream notification interface type
                              Test_I_Target_Module_EventHandler);       // writer type

#endif
