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

#ifndef TEST_U_CAMSAVE_STREAM_H
#define TEST_U_CAMSAVE_STREAM_H

#include "ace/Global_Macros.h"
#include "ace/Synch_Traits.h"

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include <guiddef.h>
#include <initguid.h> // *NOTE*: this exports DEFINE_GUIDs
                      //         (see: stream_lib_common.h)
#include <minwindef.h>
#include <mfidl.h>
#include <mfobjects.h>
#include <winnt.h>
#endif // ACE_WIN32 || ACE_WIN64

#include "common_time_common.h"

#include "stream_base.h"
#include "stream_common.h"

#include "test_u_camsave_common.h"
#include "test_u_camsave_common_modules.h"
#include "test_u_camsave_message.h"
#include "test_u_camsave_session_message.h"

// forward declarations
class Stream_IAllocator;

extern const char stream_name_string_[];

#if defined (ACE_WIN32) || defined (ACE_WIN64)
class Stream_CamSave_DirectShow_Stream
 : public Stream_Base_T<ACE_MT_SYNCH,
                        Common_TimePolicy_t,
                        stream_name_string_,
                        enum Stream_ControlType,
                        enum Stream_SessionMessageType,
                        enum Stream_StateMachine_ControlState,
                        struct Stream_CamSave_StreamState,
                        struct Stream_CamSave_StreamConfiguration,
                        struct Stream_CamSave_StatisticData,
                        struct Stream_AllocatorConfiguration,
                        struct Stream_ModuleConfiguration,
                        struct Stream_CamSave_DirectShow_ModuleHandlerConfiguration,
                        struct Stream_CamSave_SessionData, // session data
                        Stream_CamSave_SessionData_t,      // session data container (reference counted)
                        Test_U_ControlMessage_t,
                        Stream_CamSave_DirectShow_Message_t,
                        Stream_CamSave_DirectShow_SessionMessage_t>
{
  typedef Stream_Base_T<ACE_MT_SYNCH,
                        Common_TimePolicy_t,
                        stream_name_string_,
                        enum Stream_ControlType,
                        enum Stream_SessionMessageType,
                        enum Stream_StateMachine_ControlState,
                        struct Stream_CamSave_StreamState,
                        struct Stream_CamSave_StreamConfiguration,
                        struct Stream_CamSave_StatisticData,
                        struct Stream_AllocatorConfiguration,
                        struct Stream_ModuleConfiguration,
                        struct Stream_CamSave_DirectShow_ModuleHandlerConfiguration,
                        struct Stream_CamSave_SessionData,
                        Stream_CamSave_SessionData_t,
                        Test_U_ControlMessage_t,
                        Stream_CamSave_DirectShow_Message_t,
                        Stream_CamSave_DirectShow_SessionMessage_t> inherited;

 public:
  Stream_CamSave_DirectShow_Stream ();
  virtual ~Stream_CamSave_DirectShow_Stream ();

  // implement (part of) Stream_IStreamControlBase
  virtual bool load (Stream_ModuleList_t&, // return value: module list
                     bool&);               // return value: delete modules ?

  // implement Common_IInitialize_T
  virtual bool initialize (const typename inherited::CONFIGURATION_T&); // configuration

 private:
  ACE_UNIMPLEMENTED_FUNC (Stream_CamSave_DirectShow_Stream (const Stream_CamSave_DirectShow_Stream&))
  ACE_UNIMPLEMENTED_FUNC (Stream_CamSave_DirectShow_Stream& operator= (const Stream_CamSave_DirectShow_Stream&))

  // modules
  Stream_CamSave_DirectShow_Source_Module          source_;
  Stream_CamSave_DirectShow_StatisticReport_Module statisticReport_;
  Stream_CamSave_DirectShow_Display_Module          display_;
  //Stream_CamSave_DirectShow_D3D_Display_Module     display_;
  Stream_CamSave_DirectShow_AVIEncoder_Module      encoder_;
  Stream_CamSave_DirectShow_FileWriter_Module      fileWriter_;
};

class Stream_CamSave_MediaFoundation_Stream
 : public Stream_Base_T<ACE_MT_SYNCH,
                        Common_TimePolicy_t,
                        stream_name_string_,
                        enum Stream_ControlType,
                        enum Stream_SessionMessageType,
                        enum Stream_StateMachine_ControlState,
                        struct Stream_CamSave_StreamState,
                        struct Stream_CamSave_StreamConfiguration,
                        struct Stream_CamSave_StatisticData,
                        struct Stream_AllocatorConfiguration,
                        struct Stream_ModuleConfiguration,
                        struct Stream_CamSave_MediaFoundation_ModuleHandlerConfiguration,
                        struct Stream_CamSave_SessionData, // session data
                        Stream_CamSave_SessionData_t,      // session data container (reference counted)
                        Test_U_ControlMessage_t,
                        Stream_CamSave_MediaFoundation_Message_t,
                        Stream_CamSave_MediaFoundation_SessionMessage_t>
 , public IMFAsyncCallback
{
  typedef Stream_Base_T<ACE_MT_SYNCH,
                        Common_TimePolicy_t,
                        stream_name_string_,
                        enum Stream_ControlType,
                        enum Stream_SessionMessageType,
                        enum Stream_StateMachine_ControlState,
                        struct Stream_CamSave_StreamState,
                        struct Stream_CamSave_StreamConfiguration,
                        struct Stream_CamSave_StatisticData,
                        struct Stream_AllocatorConfiguration,
                        struct Stream_ModuleConfiguration,
                        struct Stream_CamSave_MediaFoundation_ModuleHandlerConfiguration,
                        struct Stream_CamSave_SessionData,
                        Stream_CamSave_SessionData_t,
                        Test_U_ControlMessage_t,
                        Stream_CamSave_MediaFoundation_Message_t,
                        Stream_CamSave_MediaFoundation_SessionMessage_t> inherited;

 public:
  Stream_CamSave_MediaFoundation_Stream ();
  virtual ~Stream_CamSave_MediaFoundation_Stream ();

  // override (part of) Stream_IStreamControl_T
  virtual const Stream_Module_t* find (const std::string&) const; // module name
  virtual void start ();
  virtual void stop (bool = true,  // wait for completion ?
                     bool = true); // locked access ?

  // implement IMFAsyncCallback
  virtual STDMETHODIMP QueryInterface (const IID&,
                                       void**);
  virtual STDMETHODIMP_ (ULONG) AddRef ();
  virtual STDMETHODIMP_ (ULONG) Release ();
  virtual STDMETHODIMP GetParameters (DWORD*,  // return value: flags
                                      DWORD*); // return value: queue handle
  virtual STDMETHODIMP Invoke (IMFAsyncResult*); // asynchronous result handle

  // implement (part of) Stream_IStreamControlBase
  virtual bool load (Stream_ModuleList_t&, // return value: module list
                     bool&);               // return value: delete modules ?

  // implement Common_IInitialize_T
  virtual bool initialize (const typename inherited::CONFIGURATION_T&); // configuration

 private:
  ACE_UNIMPLEMENTED_FUNC (Stream_CamSave_MediaFoundation_Stream (const Stream_CamSave_MediaFoundation_Stream&))
  ACE_UNIMPLEMENTED_FUNC (Stream_CamSave_MediaFoundation_Stream& operator= (const Stream_CamSave_MediaFoundation_Stream&))

  // modules
  Stream_CamSave_MediaFoundation_Source_Module          source_;
  Stream_CamSave_MediaFoundation_StatisticReport_Module statisticReport_;
  //Stream_CamSave_MediaFoundation_D3D_Display_Module     display_;
  Stream_CamSave_MediaFoundation_Display_Module         display_;
  Stream_CamSave_MediaFoundation_DisplayNull_Module     displayNull_;
  Stream_CamSave_MediaFoundation_AVIEncoder_Module      encoder_;
  Stream_CamSave_MediaFoundation_FileWriter_Module      fileWriter_;

  // media session
  IMFMediaSession*                                  mediaSession_;
  ULONG                                             referenceCount_;
};
#else
class Stream_CamSave_Stream
 : public Stream_Base_T<ACE_MT_SYNCH,
                        Common_TimePolicy_t,
                        stream_name_string_,
                        enum Stream_ControlType,
                        enum Stream_SessionMessageType,
                        enum Stream_StateMachine_ControlState,
                        struct Stream_CamSave_StreamState,
                        struct Stream_CamSave_StreamConfiguration,
                        struct Stream_CamSave_StatisticData,
                        struct Stream_AllocatorConfiguration,
                        struct Stream_ModuleConfiguration,
                        struct Stream_CamSave_V4L_ModuleHandlerConfiguration,
                        struct Stream_CamSave_SessionData, // session data
                        Stream_CamSave_SessionData_t,      // session data container (reference counted)
                        Test_U_ControlMessage_t,
                        Stream_CamSave_Message_t,
                        Stream_CamSave_SessionMessage_t>
{
  typedef Stream_Base_T<ACE_MT_SYNCH,
                        Common_TimePolicy_t,
                        stream_name_string_,
                        enum Stream_ControlType,
                        enum Stream_SessionMessageType,
                        enum Stream_StateMachine_ControlState,
                        struct Stream_CamSave_StreamState,
                        struct Stream_CamSave_StreamConfiguration,
                        struct Stream_CamSave_StatisticData,
                        struct Stream_AllocatorConfiguration,
                        struct Stream_ModuleConfiguration,
                        struct Stream_CamSave_V4L_ModuleHandlerConfiguration,
                        struct Stream_CamSave_SessionData,
                        Stream_CamSave_SessionData_t,
                        Test_U_ControlMessage_t,
                        Stream_CamSave_Message_t,
                        Stream_CamSave_SessionMessage_t> inherited;

 public:
  Stream_CamSave_Stream ();
  virtual ~Stream_CamSave_Stream ();

  // implement (part of) Stream_IStreamControlBase
  virtual bool load (Stream_ModuleList_t&, // return value: module list
                     bool&);               // return value: delete modules ?

  // implement Common_IInitialize_T
  virtual bool initialize (const typename inherited::CONFIGURATION_T&); // configuration

 private:
  ACE_UNIMPLEMENTED_FUNC (Stream_CamSave_Stream (const Stream_CamSave_Stream&))
  ACE_UNIMPLEMENTED_FUNC (Stream_CamSave_Stream& operator= (const Stream_CamSave_Stream&))

  // modules
  Stream_CamSave_V4L_Source_Module      source_;
  Stream_CamSave_LibAVDecoder_Module    decoder_;
  Stream_CamSave_LibAVConverter_Module  converter_;
  Stream_CamSave_StatisticReport_Module statisticReport_;
  Stream_CamSave_Display_Module         display_;
  Stream_CamSave_V4L2_AVIEncoder_Module encoder_;
  Stream_CamSave_FileWriter_Module      fileWriter_;
};
#endif // ACE_WIN32 || ACE_WIN64

#endif
