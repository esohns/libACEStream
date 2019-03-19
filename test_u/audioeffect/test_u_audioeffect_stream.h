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

#ifndef TEST_U_AUDIOEFFECT_STREAM_H
#define TEST_U_AUDIOEFFECT_STREAM_H

#include "ace/config-lite.h"
#include "ace/Global_Macros.h"
#include "ace/Synch_Traits.h"

#include "common_time_common.h"

#include "stream_base.h"
#include "stream_common.h"

#include "test_u_audioeffect_common.h"
#include "test_u_audioeffect_message.h"
#include "test_u_audioeffect_session_message.h"

// forward declarations
class Stream_IAllocator;

extern const char stream_name_string_[];

#if defined (ACE_WIN32) || defined (ACE_WIN64)
class Test_U_AudioEffect_DirectShow_Stream
 : public Stream_Base_T<ACE_MT_SYNCH,
                        Common_TimePolicy_t,
                        stream_name_string_,
                        enum Stream_ControlType,
                        enum Stream_SessionMessageType,
                        enum Stream_StateMachine_ControlState,
                        struct Test_U_AudioEffect_DirectShow_StreamState,
                        struct Test_U_AudioEffect_DirectShow_StreamConfiguration,
                        struct Test_U_AudioEffect_Statistic,
                        struct Stream_AllocatorConfiguration,
                        struct Stream_ModuleConfiguration,
                        struct Test_U_AudioEffect_DirectShow_ModuleHandlerConfiguration,
                        Test_U_AudioEffect_DirectShow_SessionData,
                        Test_U_AudioEffect_DirectShow_SessionData_t,
                        Test_U_ControlMessage_t,
                        Test_U_AudioEffect_DirectShow_Message,
                        Test_U_AudioEffect_DirectShow_SessionMessage>
{
  typedef Stream_Base_T<ACE_MT_SYNCH,
                        Common_TimePolicy_t,
                        stream_name_string_,
                        enum Stream_ControlType,
                        enum Stream_SessionMessageType,
                        enum Stream_StateMachine_ControlState,
                        struct Test_U_AudioEffect_DirectShow_StreamState,
                        struct Test_U_AudioEffect_DirectShow_StreamConfiguration,
                        struct Test_U_AudioEffect_Statistic,
                        struct Stream_AllocatorConfiguration,
                        struct Stream_ModuleConfiguration,
                        struct Test_U_AudioEffect_DirectShow_ModuleHandlerConfiguration,
                        Test_U_AudioEffect_DirectShow_SessionData,
                        Test_U_AudioEffect_DirectShow_SessionData_t,
                        Test_U_ControlMessage_t,
                        Test_U_AudioEffect_DirectShow_Message,
                        Test_U_AudioEffect_DirectShow_SessionMessage> inherited;

 public:
  Test_U_AudioEffect_DirectShow_Stream ();
  virtual ~Test_U_AudioEffect_DirectShow_Stream ();

  // implement (part of) Stream_IStreamControlBase
  virtual bool load (Stream_ModuleList_t&, // return value: module list
                     bool&);               // return value: delete modules ?

  // implement Common_IInitialize_T
  virtual bool initialize (const inherited::CONFIGURATION_T&); // configuration

 private:
  ACE_UNIMPLEMENTED_FUNC (Test_U_AudioEffect_DirectShow_Stream (const Test_U_AudioEffect_DirectShow_Stream&))
  ACE_UNIMPLEMENTED_FUNC (Test_U_AudioEffect_DirectShow_Stream& operator= (const Test_U_AudioEffect_DirectShow_Stream&))

  IGraphBuilder* graphBuilder_;
};

//////////////////////////////////////////

class Test_U_AudioEffect_MediaFoundation_Stream
 : public Stream_Base_T<ACE_MT_SYNCH,
                        Common_TimePolicy_t,
                        stream_name_string_,
                        enum Stream_ControlType,
                        enum Stream_SessionMessageType,
                        enum Stream_StateMachine_ControlState,
                        struct Test_U_AudioEffect_MediaFoundation_StreamState,
                        struct Test_U_AudioEffect_MediaFoundation_StreamConfiguration,
                        struct Test_U_AudioEffect_Statistic,
                        struct Stream_AllocatorConfiguration,
                        struct Stream_ModuleConfiguration,
                        struct Test_U_AudioEffect_MediaFoundation_ModuleHandlerConfiguration,
                        Test_U_AudioEffect_MediaFoundation_SessionData,
                        Test_U_AudioEffect_MediaFoundation_SessionData_t,
                        Test_U_ControlMessage_t,
                        Test_U_AudioEffect_MediaFoundation_Message,
                        Test_U_AudioEffect_MediaFoundation_SessionMessage>
 , public IMFAsyncCallback
{
  typedef Stream_Base_T<ACE_MT_SYNCH,
                        Common_TimePolicy_t,
                        stream_name_string_,
                        enum Stream_ControlType,
                        enum Stream_SessionMessageType,
                        enum Stream_StateMachine_ControlState,
                        struct Test_U_AudioEffect_MediaFoundation_StreamState,
                        struct Test_U_AudioEffect_MediaFoundation_StreamConfiguration,
                        struct Test_U_AudioEffect_Statistic,
                        struct Stream_AllocatorConfiguration,
                        struct Stream_ModuleConfiguration,
                        struct Test_U_AudioEffect_MediaFoundation_ModuleHandlerConfiguration,
                        Test_U_AudioEffect_MediaFoundation_SessionData,
                        Test_U_AudioEffect_MediaFoundation_SessionData_t,
                        Test_U_ControlMessage_t,
                        Test_U_AudioEffect_MediaFoundation_Message,
                        Test_U_AudioEffect_MediaFoundation_SessionMessage> inherited;

 public:
  Test_U_AudioEffect_MediaFoundation_Stream ();
  virtual ~Test_U_AudioEffect_MediaFoundation_Stream ();

  // override (part of) Stream_IStreamControl_T
  virtual const Stream_Module_t* find (const std::string&) const; // module name
  virtual void start ();
  virtual void stop (bool = true,  // wait for completion ?
                     bool = true); // locked access ?

  // implement (part of) Stream_IStreamControlBase
  virtual bool load (Stream_ModuleList_t&, // return value: module list
                     bool&);               // return value: delete modules ?

  // implement Common_IInitialize_T
  virtual bool initialize (const inherited::CONFIGURATION_T&); // configuration

  // implement IMFAsyncCallback
  virtual STDMETHODIMP QueryInterface (const IID&,
                                       void**);
  inline virtual STDMETHODIMP_ (ULONG) AddRef () { return InterlockedIncrement (&referenceCount_); }
  inline virtual STDMETHODIMP_ (ULONG) Release () { ULONG count = InterlockedDecrement (&referenceCount_); return count; }
  // *NOTE*: "...If you want default values for both parameters, return
  //         E_NOTIMPL. ..."
  inline virtual STDMETHODIMP GetParameters (DWORD* flags_out, DWORD* queue_out) { ACE_UNUSED_ARG (flags_out); ACE_UNUSED_ARG (queue_out); return E_NOTIMPL; }
  virtual STDMETHODIMP Invoke (IMFAsyncResult*); // asynchronous result handle

 private:
  ACE_UNIMPLEMENTED_FUNC (Test_U_AudioEffect_MediaFoundation_Stream (const Test_U_AudioEffect_MediaFoundation_Stream&))
  ACE_UNIMPLEMENTED_FUNC (Test_U_AudioEffect_MediaFoundation_Stream& operator= (const Test_U_AudioEffect_MediaFoundation_Stream&))

#if COMMON_OS_WIN32_TARGET_PLATFORM(0x0600) // _WIN32_WINNT_VISTA
  IMFMediaSession* mediaSession_;
#endif // COMMON_OS_WIN32_TARGET_PLATFORM(0x0600)
  ULONG            referenceCount_;
};
#else
class Test_U_AudioEffect_ALSA_Stream
 : public Stream_Base_T<ACE_MT_SYNCH,
                        Common_TimePolicy_t,
                        stream_name_string_,
                        enum Stream_ControlType,
                        enum Stream_SessionMessageType,
                        enum Stream_StateMachine_ControlState,
                        struct Test_U_AudioEffect_StreamState,
                        struct Test_U_AudioEffect_ALSA_StreamConfiguration,
                        struct Test_U_AudioEffect_Statistic,
                        struct Stream_AllocatorConfiguration,
                        struct Stream_ModuleConfiguration,
                        struct Test_U_AudioEffect_ALSA_ModuleHandlerConfiguration,
                        struct Test_U_AudioEffect_SessionData,
                        Test_U_AudioEffect_SessionData_t,
                        Test_U_ControlMessage_t,
                        Test_U_AudioEffect_Message,
                        Test_U_AudioEffect_SessionMessage>
{
  typedef Stream_Base_T<ACE_MT_SYNCH,
                        Common_TimePolicy_t,
                        stream_name_string_,
                        enum Stream_ControlType,
                        enum Stream_SessionMessageType,
                        enum Stream_StateMachine_ControlState,
                        struct Test_U_AudioEffect_StreamState,
                        struct Test_U_AudioEffect_ALSA_StreamConfiguration,
                        struct Test_U_AudioEffect_Statistic,
                        struct Stream_AllocatorConfiguration,
                        struct Stream_ModuleConfiguration,
                        struct Test_U_AudioEffect_ALSA_ModuleHandlerConfiguration,
                        struct Test_U_AudioEffect_SessionData,
                        Test_U_AudioEffect_SessionData_t,
                        Test_U_ControlMessage_t,
                        Test_U_AudioEffect_Message,
                        Test_U_AudioEffect_SessionMessage> inherited;

 public:
  Test_U_AudioEffect_ALSA_Stream ();
  virtual ~Test_U_AudioEffect_ALSA_Stream ();

  // implement (part of) Stream_IStreamControlBase
  virtual bool load (Stream_ILayout*, // return value: module list
                     bool&);          // return value: delete modules ?

  // implement Common_IInitialize_T
  virtual bool initialize (const typename inherited::CONFIGURATION_T&); // configuration

 private:
  ACE_UNIMPLEMENTED_FUNC (Test_U_AudioEffect_ALSA_Stream (const Test_U_AudioEffect_ALSA_Stream&))
  ACE_UNIMPLEMENTED_FUNC (Test_U_AudioEffect_ALSA_Stream& operator= (const Test_U_AudioEffect_ALSA_Stream&))
};
#endif

#endif
