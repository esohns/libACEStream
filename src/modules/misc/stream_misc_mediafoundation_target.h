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

#ifndef STREAM_MISC_MEDIAFOUNDATION_TARGET_H
#define STREAM_MISC_MEDIAFOUNDATION_TARGET_H

#include "d3d9.h"
#include "dxva2api.h"
#include "mfidl.h"
#include "mfreadwrite.h"

#include "ace/Global_Macros.h"
#include "ace/Synch_Traits.h"

#include "common_iinitialize.h"
#include "common_time_common.h"

#include "stream_common.h"
#include "stream_task_base_synch.h"

template <typename SessionMessageType,
          typename MessageType,
          ///////////////////////////////
          typename ConfigurationType,
          ///////////////////////////////
          typename SessionDataType,
          typename SessionDataContainerType>
class Stream_Misc_MediaFoundation_Target_T
 : public Stream_TaskBaseSynch_T<Common_TimePolicy_t,
                                 SessionMessageType,
                                 MessageType>
 , public Stream_IModuleHandler_T<ConfigurationType>
 , public IMFSampleGrabberSinkCallback2
{
 public:
  //// convenience types
  //typedef Common_IInitialize_T<ConfigurationType> IINITIALIZE_T;

  Stream_Misc_MediaFoundation_Target_T ();
  virtual ~Stream_Misc_MediaFoundation_Target_T ();

  // implement (part of) Stream_ITaskBase_T
  virtual void handleDataMessage (MessageType*&, // data message handle
                                  bool&);        // return value: pass message downstream ?
  virtual void handleSessionMessage (SessionMessageType*&, // session message handle
                                     bool&);               // return value: pass message downstream ?

  // implement Stream_IModuleHandler_T
  virtual bool initialize (const ConfigurationType&);
  virtual const ConfigurationType& get () const;

  // implement IMFSampleGrabberSinkCallback2
  STDMETHODIMP QueryInterface (const IID&,
                               void**);
  virtual ULONG STDMETHODCALLTYPE AddRef ();
  virtual ULONG STDMETHODCALLTYPE Release ();
  //STDMETHODIMP OnEvent (DWORD,           // stream index
  //                      IMFMediaEvent*); // event handle
  //STDMETHODIMP OnFlush (DWORD); // stream index
  //STDMETHODIMP OnReadSample (HRESULT,     // result
  //                           DWORD,       // stream index
  //                           DWORD,       // stream flags
  //                           LONGLONG,    // timestamp
  //                           IMFSample*); // sample handle
  STDMETHODIMP OnClockStart (MFTIME,    // (system) clock start time
                             LONGLONG); // clock start offset
  STDMETHODIMP OnClockStop (MFTIME); // (system) clock start time
  STDMETHODIMP OnClockPause (MFTIME); // (system) clock pause time
  STDMETHODIMP OnClockRestart (MFTIME); // (system) clock restart time
  STDMETHODIMP OnClockSetRate (MFTIME, // (system) clock rate set time
                               float); // new playback rate
  STDMETHODIMP OnProcessSample (const struct _GUID&, // major media type
                                DWORD,               // flags
                                LONGLONG,            // timestamp
                                LONGLONG,            // duration
                                const BYTE*,         // buffer
                                DWORD);              // buffer size
  STDMETHODIMP OnProcessSampleEx (const struct _GUID&, // major media type
                                  DWORD,               // flags
                                  LONGLONG,            // timestamp
                                  LONGLONG,            // duration
                                  const BYTE*,         // buffer
                                  DWORD,               // buffer size
                                  IMFAttributes*);     // media sample attributes
  STDMETHODIMP OnSetPresentationClock (IMFPresentationClock*); // presentation clock handle
  STDMETHODIMP OnShutdown ();

 protected:
  ConfigurationType*        configuration_;
  SessionDataContainerType* sessionData_;

 private:
  typedef Stream_TaskBaseSynch_T<Common_TimePolicy_t,
                                 SessionMessageType,
                                 MessageType> inherited;

  // convenient types
  typedef Stream_Misc_MediaFoundation_Target_T<SessionMessageType,
                                               MessageType,

                                               ConfigurationType,

                                               SessionDataType,
                                               SessionDataContainerType> OWN_TYPE_T;

  //ACE_UNIMPLEMENTED_FUNC (Stream_Misc_MediaFoundation_Target_T ())
  ACE_UNIMPLEMENTED_FUNC (Stream_Misc_MediaFoundation_Target_T (const Stream_Misc_MediaFoundation_Target_T&))
  ACE_UNIMPLEMENTED_FUNC (Stream_Misc_MediaFoundation_Target_T& operator= (const Stream_Misc_MediaFoundation_Target_T&))

  // helper methods
  MessageType* allocateMessage (unsigned int); // (requested) size
  bool initialize_MediaFoundation (const IMFMediaType*,                  // sample grabber sink input media type handle
                                   const IMFSampleGrabberSinkCallback2*, // sample grabber sink callback handle
                                   TOPOID&,                              // return value: node id
                                   IMFMediaSession*&);                   // intput/return value: media session handle
  void finalize_MediaFoundation ();

  bool                      isFirst_;
  bool                      isInitialized_;

  LONGLONG                  baseTimeStamp_;
  IMFMediaSession*          mediaSession_;
  IMFPresentationClock*     presentationClock_;
  long                      referenceCount_;
  TOPOID                    sampleGrabberSinkNodeId_;
};

// include template definition
#include "stream_misc_mediafoundation_target.inl"

#endif
