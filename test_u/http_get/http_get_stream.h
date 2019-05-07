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

#ifndef HTTP_GET_STREAM_T_H
#define HTTP_GET_STREAM_T_H

#include "ace/Global_Macros.h"
#include "ace/Synch_Traits.h"

#include "common_time_common.h"

#include "stream_base.h"
#include "stream_common.h"

#include "stream_net_source.h"

#include "http_get_common_modules.h"
#include "http_get_stream_common.h"
#include "http_get_network.h"

// forward declarations
class Stream_IAllocator;

template <typename ConnectorType>
class HTTPGet_Stream_T
 : public Stream_Base_T<ACE_MT_SYNCH,
                        Common_TimePolicy_t,
                        stream_name_string_,
                        enum Stream_ControlType,
                        enum Stream_SessionMessageType,
                        enum Stream_StateMachine_ControlState,
                        struct HTTPGet_StreamState,
                        struct Stream_Configuration,
                        struct Stream_Statistic,
                        struct Common_FlexParserAllocatorConfiguration,
                        struct Stream_ModuleConfiguration,
                        struct HTTPGet_ModuleHandlerConfiguration,
                        struct HTTPGet_SessionData,
                        HTTPGet_SessionData_t,
                        HTTPGet_ControlMessage_t,
                        HTTPGet_Message,
                        HTTPGet_SessionMessage>
{
  typedef Stream_Base_T<ACE_MT_SYNCH,
                        Common_TimePolicy_t,
                        stream_name_string_,
                        enum Stream_ControlType,
                        enum Stream_SessionMessageType,
                        enum Stream_StateMachine_ControlState,
                        struct HTTPGet_StreamState,
                        struct Stream_Configuration,
                        struct Stream_Statistic,
                        struct Common_FlexParserAllocatorConfiguration,
                        struct Stream_ModuleConfiguration,
                        struct HTTPGet_ModuleHandlerConfiguration,
                        struct HTTPGet_SessionData,
                        HTTPGet_SessionData_t,
                        HTTPGet_ControlMessage_t,
                        HTTPGet_Message,
                        HTTPGet_SessionMessage> inherited;

 public:
  HTTPGet_Stream_T ();
  virtual ~HTTPGet_Stream_T ();

  // implement (part of) Stream_IStreamControlBase
  virtual bool load (Stream_ModuleList_t&, // return value: module list
                     bool&);               // return value: delete modules ?

  // implement Common_IInitialize_T
  virtual bool initialize (const typename inherited::CONFIGURATION_T&); // configuration

  // implement Common_IStatistic_T
  // *NOTE*: these delegate to the statistic module (named: "StatisticReport")
  virtual bool collect (struct Stream_Statistic&); // return value: statistic data
  virtual void report () const;

 private:
  typedef Stream_Module_Net_Source_Writer_T<ACE_MT_SYNCH,
                                            Common_TimePolicy_t,
                                            struct HTTPGet_ModuleHandlerConfiguration,
                                            HTTPGet_ControlMessage_t,
                                            HTTPGet_Message,
                                            HTTPGet_SessionMessage,
                                            HTTPGet_ConnectionManager_t,
                                            ConnectorType> SOURCE_WRITER_T;
  typedef Stream_StreamModuleInputOnly_T<ACE_MT_SYNCH,                              // task synch type
                                         Common_TimePolicy_t,                       // time policy
                                         Stream_SessionId_t,                        // session id type
                                         struct HTTPGet_SessionData,                // session data type
                                         enum Stream_SessionMessageType,            // session event type
                                         struct Stream_ModuleConfiguration,         // module configuration type
                                         struct HTTPGet_ModuleHandlerConfiguration, // module handler configuration type
                                         libacestream_default_net_source_module_name_string,
                                         Stream_INotify_t,                          // stream notification interface type
                                         SOURCE_WRITER_T> SOURCE_MODULE_T;          // writer type

  ACE_UNIMPLEMENTED_FUNC (HTTPGet_Stream_T (const HTTPGet_Stream_T&))
  ACE_UNIMPLEMENTED_FUNC (HTTPGet_Stream_T& operator= (const HTTPGet_Stream_T&))

  // *TODO*: re-consider this API
  void ping ();
};

// include template definition
#include "http_get_stream.inl"

//////////////////////////////////////////

typedef HTTPGet_Stream_T<HTTPGet_TCPConnector_t> HTTPGet_Stream_t;
#if defined (SSL_SUPPORT)
typedef HTTPGet_Stream_T<HTTPGet_SSLTCPConnector_t> HTTPGet_SSL_Stream_t;
#endif // SSL_SUPPORT
typedef HTTPGet_Stream_T<HTTPGet_TCPAsynchConnector_t> HTTPGet_AsynchStream_t;

#endif
