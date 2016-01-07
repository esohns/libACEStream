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

#ifndef TEST_U_CAMSAVE_COMMON_MODULES_H
#define TEST_U_CAMSAVE_COMMON_MODULES_H

#include "ace/Synch_Traits.h"

#include "common_time_common.h"

#include "stream_common.h"
#include "stream_streammodule_base.h"

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include "stream_dev_cam_source_directshow.h"
#include "stream_vis_target_directshow.h"
#else
#include "stream_dev_cam_source.h"
#endif
#include "stream_file_sink.h"
#include "stream_misc_runtimestatistic.h"

#include "test_u_camsave_common.h"
#include "test_u_camsave_message.h"
#include "test_u_camsave_session_message.h"

// declare module(s)
#if defined (ACE_WIN32) || defined (ACE_WIN64)
typedef Stream_Dev_Cam_Source_DirectShow_T<ACE_SYNCH_MUTEX,
                                           
                                           Stream_CamSave_SessionMessage,
                                           Stream_CamSave_Message,
                                           
                                           Stream_CamSave_ModuleHandlerConfiguration,
                                           
                                           Stream_State,
                                           
                                           Stream_CamSave_SessionData,
                                           Stream_CamSave_SessionData_t,
                                           
                                           Stream_Statistic> Stream_CamSave_Module_Source;
#else
typedef Stream_Dev_Cam_Source_T<ACE_SYNCH_MUTEX,
                                /////////
                                Stream_CamSave_SessionMessage,
                                Stream_CamSave_Message,
                                /////////
                                Stream_CamSave_ModuleHandlerConfiguration,
                                /////////
                                Stream_State,
                                /////////
                                Stream_CamSave_SessionData,
                                Stream_CamSave_SessionData_t,
                                /////////
                                Stream_Statistic> Stream_CamSave_Module_Source;
#endif
DATASTREAM_MODULE_INPUT_ONLY (ACE_MT_SYNCH,                              // task synch type
                              Common_TimePolicy_t,                       // time policy
                              Stream_ModuleConfiguration,                // module configuration type
                              Stream_CamSave_ModuleHandlerConfiguration, // module handler configuration type
                              Stream_CamSave_Module_Source);             // writer type

typedef Stream_Module_Statistic_ReaderTask_T<ACE_MT_SYNCH,
                                             Common_TimePolicy_t,
                                             Stream_CamSave_SessionMessage,
                                             Stream_CamSave_Message,
                                             Stream_CommandType_t,
                                             Stream_Statistic,
                                             Stream_CamSave_SessionData,
                                             Stream_CamSave_SessionData_t> Stream_CamSave_Module_Statistic_ReaderTask_t;
typedef Stream_Module_Statistic_WriterTask_T<ACE_MT_SYNCH,
                                             Common_TimePolicy_t,
                                             Stream_CamSave_SessionMessage,
                                             Stream_CamSave_Message,
                                             Stream_CommandType_t,
                                             Stream_Statistic,
                                             Stream_CamSave_SessionData,
                                             Stream_CamSave_SessionData_t> Stream_CamSave_Module_Statistic_WriterTask_t;
DATASTREAM_MODULE_DUPLEX (ACE_MT_SYNCH,                                 // task synch type
                          Common_TimePolicy_t,                          // time policy type
                          Stream_ModuleConfiguration,                   // module configuration type
                          Stream_CamSave_ModuleHandlerConfiguration,    // module handler configuration type
                          Stream_CamSave_Module_Statistic_ReaderTask_t, // reader type
                          Stream_CamSave_Module_Statistic_WriterTask_t, // writer type
                          Stream_CamSave_Module_RuntimeStatistic);      // name

#if defined (ACE_WIN32) || defined (ACE_WIN64)
typedef Stream_Vis_Target_DirectShow_T<Stream_CamSave_SessionMessage,
                                       Stream_CamSave_Message,
                                       ///
                                       Stream_CamSave_ModuleHandlerConfiguration,
                                       ///
                                       Stream_CamSave_SessionData,
                                       Stream_CamSave_SessionData_t> Stream_CamSave_Module_Display;
#else
#endif
DATASTREAM_MODULE_INPUT_ONLY (ACE_MT_SYNCH,                              // task synch type
                              Common_TimePolicy_t,                       // time policy
                              Stream_ModuleConfiguration,                // module configuration type
                              Stream_CamSave_ModuleHandlerConfiguration, // module handler configuration type
                              Stream_CamSave_Module_Display);            // writer type

typedef Stream_Module_FileWriter_T<Stream_CamSave_SessionMessage,
                                   Stream_CamSave_Message,
                                   //////
                                   Stream_CamSave_ModuleHandlerConfiguration,
                                   //////
                                   Stream_CamSave_SessionData> Stream_CamSave_Module_FileWriter;
DATASTREAM_MODULE_INPUT_ONLY (ACE_MT_SYNCH,                              // task synch type
                              Common_TimePolicy_t,                       // time policy
                              Stream_ModuleConfiguration,                // module configuration type
                              Stream_CamSave_ModuleHandlerConfiguration, // module handler configuration type
                              Stream_CamSave_Module_FileWriter);         // writer type

#endif