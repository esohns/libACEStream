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

#ifndef TEST_U_CAMERASCREEN_STREAM_H
#define TEST_U_CAMERASCREEN_STREAM_H

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#if COMMON_OS_WIN32_TARGET_PLATFORM(0x0602) // _WIN32_WINNT_WIN8
#include <minwindef.h>
#else
#include <windef.h>
#endif // COMMON_OS_WIN32_TARGET_PLATFORM(0x0602)
#include <winnt.h>
#include <guiddef.h>
#include <mfidl.h>
#include <mfobjects.h>
#endif // ACE_WIN32 || ACE_WIN64

#include "ace/Global_Macros.h"
#include "ace/Synch_Traits.h"

#include "common_time_common.h"

#include "stream_base.h"
#include "stream_common.h"

#include "test_u_camerascreen_common.h"
#include "test_u_camerascreen_common_modules.h"
#include "test_u_camerascreen_message.h"
#include "test_u_camerascreen_session_message.h"

// forward declarations
class Stream_IAllocator;

extern const char stream_name_string_[];

#if defined (ACE_WIN32) || defined (ACE_WIN64)
class Stream_CameraScreen_DirectShow_Stream
 : public Stream_Base_T<ACE_MT_SYNCH,
                        Common_TimePolicy_t,
                        stream_name_string_,
                        enum Stream_ControlType,
                        enum Stream_SessionMessageType,
                        enum Stream_StateMachine_ControlState,
                        struct Stream_CameraScreen_DirectShow_StreamState,
                        struct Stream_CameraScreen_DirectShow_StreamConfiguration,
                        struct Stream_CameraScreen_StatisticData,
                        struct Stream_AllocatorConfiguration,
                        struct Stream_ModuleConfiguration,
                        struct Stream_CameraScreen_DirectShow_ModuleHandlerConfiguration,
                        Stream_CameraScreen_DirectShow_SessionData,
                        Stream_CameraScreen_DirectShow_SessionData_t,
                        Stream_ControlMessage_t,
                        Stream_CameraScreen_DirectShow_Message_t,
                        Stream_CameraScreen_DirectShow_SessionMessage_t>
{
  typedef Stream_Base_T<ACE_MT_SYNCH,
                        Common_TimePolicy_t,
                        stream_name_string_,
                        enum Stream_ControlType,
                        enum Stream_SessionMessageType,
                        enum Stream_StateMachine_ControlState,
                        struct Stream_CameraScreen_DirectShow_StreamState,
                        struct Stream_CameraScreen_DirectShow_StreamConfiguration,
                        struct Stream_CameraScreen_StatisticData,
                        struct Stream_AllocatorConfiguration,
                        struct Stream_ModuleConfiguration,
                        struct Stream_CameraScreen_DirectShow_ModuleHandlerConfiguration,
                        Stream_CameraScreen_DirectShow_SessionData,
                        Stream_CameraScreen_DirectShow_SessionData_t,
                        Stream_ControlMessage_t,
                        Stream_CameraScreen_DirectShow_Message_t,
                        Stream_CameraScreen_DirectShow_SessionMessage_t> inherited;

 public:
  Stream_CameraScreen_DirectShow_Stream ();
  virtual ~Stream_CameraScreen_DirectShow_Stream ();

  // implement (part of) Stream_IStreamControlBase
  virtual bool load (Stream_ModuleList_t&, // return value: module list
                     bool&);               // return value: delete modules ?

  // implement Common_IInitialize_T
  virtual bool initialize (const inherited::CONFIGURATION_T&); // configuration

 private:
  ACE_UNIMPLEMENTED_FUNC (Stream_CameraScreen_DirectShow_Stream (const Stream_CameraScreen_DirectShow_Stream&))
  ACE_UNIMPLEMENTED_FUNC (Stream_CameraScreen_DirectShow_Stream& operator= (const Stream_CameraScreen_DirectShow_Stream&))

  // modules
  Stream_CameraScreen_DirectShow_Source_Module            source_;
  Stream_CameraScreen_DirectShow_StatisticReport_Module   statisticReport_;
#if defined (GUI_SUPPORT)
  Stream_CameraScreen_DirectShow_Direct3DDisplay_Module   direct3DDisplay_;
  Stream_CameraScreen_DirectShow_DirectShowDisplay_Module directShowDisplay_;
#if defined (GTK_USE)
  Stream_CameraScreen_DirectShow_GTKCairoDisplay_Module   GTKCairoDisplay_;
#endif // GTK_USE
#endif // GUI_SUPPORT
  Stream_CameraScreen_DirectShow_AVIEncoder_Module        encoder_;
  Stream_CameraScreen_DirectShow_FileWriter_Module        fileWriter_;
};

class Stream_CameraScreen_MediaFoundation_Stream
 : public Stream_Base_T<ACE_MT_SYNCH,
                        Common_TimePolicy_t,
                        stream_name_string_,
                        enum Stream_ControlType,
                        enum Stream_SessionMessageType,
                        enum Stream_StateMachine_ControlState,
                        struct Stream_CameraScreen_MediaFoundation_StreamState,
                        struct Stream_CameraScreen_MediaFoundation_StreamConfiguration,
                        struct Stream_CameraScreen_StatisticData,
                        struct Stream_AllocatorConfiguration,
                        struct Stream_ModuleConfiguration,
                        struct Stream_CameraScreen_MediaFoundation_ModuleHandlerConfiguration,
                        Stream_CameraScreen_MediaFoundation_SessionData,
                        Stream_CameraScreen_MediaFoundation_SessionData_t,
                        Stream_ControlMessage_t,
                        Stream_CameraScreen_MediaFoundation_Message_t,
                        Stream_CameraScreen_MediaFoundation_SessionMessage_t>
 , public IMFAsyncCallback
{
  typedef Stream_Base_T<ACE_MT_SYNCH,
                        Common_TimePolicy_t,
                        stream_name_string_,
                        enum Stream_ControlType,
                        enum Stream_SessionMessageType,
                        enum Stream_StateMachine_ControlState,
                        struct Stream_CameraScreen_MediaFoundation_StreamState,
                        struct Stream_CameraScreen_MediaFoundation_StreamConfiguration,
                        struct Stream_CameraScreen_StatisticData,
                        struct Stream_AllocatorConfiguration,
                        struct Stream_ModuleConfiguration,
                        struct Stream_CameraScreen_MediaFoundation_ModuleHandlerConfiguration,
                        Stream_CameraScreen_MediaFoundation_SessionData,
                        Stream_CameraScreen_MediaFoundation_SessionData_t,
                        Stream_ControlMessage_t,
                        Stream_CameraScreen_MediaFoundation_Message_t,
                        Stream_CameraScreen_MediaFoundation_SessionMessage_t> inherited;

 public:
  Stream_CameraScreen_MediaFoundation_Stream ();
  virtual ~Stream_CameraScreen_MediaFoundation_Stream ();

  // override (part of) Stream_IStreamControl_T
  virtual const Stream_Module_t* find (const std::string&) const; // module name
  virtual void start ();
  virtual void stop (bool = true,  // wait for completion ?
                     bool = true,  // recurse upstream (if any) ?
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
  virtual bool initialize (const inherited::CONFIGURATION_T&); // configuration

 private:
  ACE_UNIMPLEMENTED_FUNC (Stream_CameraScreen_MediaFoundation_Stream (const Stream_CameraScreen_MediaFoundation_Stream&))
  ACE_UNIMPLEMENTED_FUNC (Stream_CameraScreen_MediaFoundation_Stream& operator= (const Stream_CameraScreen_MediaFoundation_Stream&))

  // modules
  Stream_CameraScreen_MediaFoundation_Source_Module                     source_;
  Stream_CameraScreen_MediaFoundation_StatisticReport_Module            statisticReport_;
  Stream_CameraScreen_MediaFoundation_MediaFoundationDisplay_Module     mediaFoundationDisplay_;
  Stream_CameraScreen_MediaFoundation_MediaFoundationDisplayNull_Module mediaFoundationDisplayNull_;
  Stream_CameraScreen_MediaFoundation_Direct3DDisplay_Module            direct3DDisplay_;
#if defined (GUI_SUPPORT)
#if defined (GTK_USE)
  Stream_CameraScreen_MediaFoundation_GTKCairoDisplay_Module            GTKCairoDisplay_;
#endif // GTK_USE
#endif // GUI_SUPPORT
  Stream_CameraScreen_MediaFoundation_AVIEncoder_Module                 encoder_;
  Stream_CameraScreen_MediaFoundation_FileWriter_Module                 fileWriter_;

#if COMMON_OS_WIN32_TARGET_PLATFORM(0x0600) // _WIN32_WINNT_VISTA
  // media session
  IMFMediaSession*                                                 mediaSession_;
#endif // COMMON_OS_WIN32_TARGET_PLATFORM(0x0600)
  ULONG                                                            referenceCount_;
};
#else
class Stream_CameraScreen_Stream
 : public Stream_Base_T<ACE_MT_SYNCH,
                        Common_TimePolicy_t,
                        stream_name_string_,
                        enum Stream_ControlType,
                        enum Stream_SessionMessageType,
                        enum Stream_StateMachine_ControlState,
                        struct Stream_CameraScreen_StreamState,
                        struct Stream_CameraScreen_StreamConfiguration,
                        struct Stream_CameraScreen_StatisticData,
                        struct Stream_AllocatorConfiguration,
                        struct Stream_ModuleConfiguration,
                        struct Stream_CameraScreen_V4L_ModuleHandlerConfiguration,
                        Stream_CameraScreen_V4L_SessionData,
                        Stream_CameraScreen_V4L_SessionData_t,
                        Stream_ControlMessage_t,
                        Stream_CameraScreen_Message_t,
                        Stream_CameraScreen_SessionMessage_t>
{
  typedef Stream_Base_T<ACE_MT_SYNCH,
                        Common_TimePolicy_t,
                        stream_name_string_,
                        enum Stream_ControlType,
                        enum Stream_SessionMessageType,
                        enum Stream_StateMachine_ControlState,
                        struct Stream_CameraScreen_StreamState,
                        struct Stream_CameraScreen_StreamConfiguration,
                        struct Stream_CameraScreen_StatisticData,
                        struct Stream_AllocatorConfiguration,
                        struct Stream_ModuleConfiguration,
                        struct Stream_CameraScreen_V4L_ModuleHandlerConfiguration,
                        Stream_CameraScreen_V4L_SessionData,
                        Stream_CameraScreen_V4L_SessionData_t,
                        Stream_ControlMessage_t,
                        Stream_CameraScreen_Message_t,
                        Stream_CameraScreen_SessionMessage_t> inherited;

 public:
  Stream_CameraScreen_Stream ();
  virtual ~Stream_CameraScreen_Stream ();

  // implement (part of) Stream_IStreamControlBase
  virtual bool load (typename inherited::LAYOUT_T&, // return value: layout
                     bool&);                        // return value: delete modules ?

  // implement Common_IInitialize_T
  virtual bool initialize (const typename inherited::CONFIGURATION_T&); // configuration

 private:
  ACE_UNIMPLEMENTED_FUNC (Stream_CameraScreen_Stream (const Stream_CameraScreen_Stream&))
  ACE_UNIMPLEMENTED_FUNC (Stream_CameraScreen_Stream& operator= (const Stream_CameraScreen_Stream&))

  // modules
  Stream_CameraScreen_V4L_Source_Module      source_;
  Stream_CameraScreen_StatisticReport_Module statisticReport_;
  Stream_CameraScreen_LibAVResize_Module     resizer_; // --> window size/fullscreen
  Stream_CameraScreen_Display_Module         display_;
  Stream_CameraScreen_Display_2_Module       display_2_;
};
#endif // ACE_WIN32 || ACE_WIN64

#endif