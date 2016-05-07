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

#ifndef STREAM_DEV_CAM_SOURCE_MEDIAFOUNDATION_H
#define STREAM_DEV_CAM_SOURCE_MEDIAFOUNDATION_H

#include <string>

#include "d3d9.h"
#include "dxva2api.h"
#include "mfidl.h"
#include "mfreadwrite.h"

#include "ace/Global_Macros.h"
#include "ace/Synch_Traits.h"

#include "common_time_common.h"

#include "stream_common.h"
#include "stream_headmoduletask_base.h"

template <typename LockType,
          ///////////////////////////////
          typename SessionMessageType,
          typename ProtocolMessageType,
          ///////////////////////////////
          typename ConfigurationType,
          ///////////////////////////////
          typename StreamStateType,
          ///////////////////////////////
          typename SessionDataType,          // session data
          typename SessionDataContainerType, // session message payload (reference counted)
          ///////////////////////////////
          typename StatisticContainerType>
class Stream_Dev_Cam_Source_MediaFoundation_T
 : public Stream_HeadModuleTaskBase_T<LockType,
                                      ///
                                      ACE_MT_SYNCH,
                                      Common_TimePolicy_t,
                                      SessionMessageType,
                                      ProtocolMessageType,
                                      ///
                                      ConfigurationType,
                                      ///
                                      StreamStateType,
                                      ///
                                      SessionDataType,
                                      SessionDataContainerType,
                                      ///
                                      StatisticContainerType>
 , public IMFSourceReaderCallback
{
 public:
  Stream_Dev_Cam_Source_MediaFoundation_T ();
  virtual ~Stream_Dev_Cam_Source_MediaFoundation_T ();

  // *PORTABILITY*: for some reason, this base class member is not exposed
  //                (MSVC/gcc)
  using Stream_HeadModuleTaskBase_T<LockType,
                                    ACE_MT_SYNCH,
                                    Common_TimePolicy_t,
                                    SessionMessageType,
                                    ProtocolMessageType,
                                    ConfigurationType,
                                    StreamStateType,
                                    SessionDataType,
                                    SessionDataContainerType,
                                    StatisticContainerType>::initialize;

  // override (part of) Stream_IModuleHandler_T
  virtual bool initialize (const ConfigurationType&);

  // implement Common_IStatistic
  // *NOTE*: implements regular (timer-based) statistic collection
  virtual bool collect (StatisticContainerType&); // return value: (currently unused !)
  //virtual void report () const;

  // info
  bool isInitialized () const;

//  // implement (part of) Stream_ITaskBase
//  virtual void handleDataMessage (ProtocolMessageType*&, // data message handle
//                                  bool&);                // return value: pass message downstream ?
  virtual void handleSessionMessage (SessionMessageType*&, // session message handle
                                     bool&);               // return value: pass message downstream ?

  // implement IMFSourceReaderCallback
  virtual HRESULT STDMETHODCALLTYPE QueryInterface (const IID&,
                                                    void**);
  virtual ULONG STDMETHODCALLTYPE AddRef ();
  virtual ULONG STDMETHODCALLTYPE Release ();
  STDMETHODIMP OnEvent (DWORD,           // stream index
                        IMFMediaEvent*); // event handle
  STDMETHODIMP OnFlush (DWORD); // stream index
  STDMETHODIMP OnReadSample (HRESULT,     // result
                             DWORD,       // stream index
                             DWORD,       // stream flags
                             LONGLONG,    // timestamp
                             IMFSample*); // sample handle

 private:
  typedef Stream_HeadModuleTaskBase_T<LockType,
                                      ///
                                      ACE_MT_SYNCH,
                                      Common_TimePolicy_t,
                                      SessionMessageType,
                                      ProtocolMessageType,
                                      ///
                                      ConfigurationType,
                                      ///
                                      StreamStateType,
                                      ///
                                      SessionDataType,
                                      SessionDataContainerType,
                                      ///
                                      StatisticContainerType> inherited;
  typedef IMFSourceReaderCallback inherited2;

  typedef Stream_Dev_Cam_Source_MediaFoundation_T<LockType,
                                                  
                                                  SessionMessageType,
                                                  ProtocolMessageType,
                                                  
                                                  ConfigurationType,
                                                  
                                                  StreamStateType,
                                                  
                                                  SessionDataType,          // session data
                                                  SessionDataContainerType, // session message payload (reference counted)
                                                  
                                                  StatisticContainerType> OWN_TYPE_T;

  ACE_UNIMPLEMENTED_FUNC (Stream_Dev_Cam_Source_MediaFoundation_T (const Stream_Dev_Cam_Source_MediaFoundation_T&))
  ACE_UNIMPLEMENTED_FUNC (Stream_Dev_Cam_Source_MediaFoundation_T& operator= (const Stream_Dev_Cam_Source_MediaFoundation_T&))

  // helper methods
  // *NOTE*: (if any,) fire-and-forget the media source handle (third argument)
  bool initialize_MediaFoundation (const std::string&,             // (source) device name (FriendlyName)
                                   const HWND,                     // (target) window handle [NULL: NullRenderer]
                                   const IDirect3DDeviceManager9*, // direct 3d manager handle
                                   IMFMediaSource*&,               // media source handle (in/out)
                                   IMFSourceReaderCallback*,       // source reader callback handle
                                   IMFSourceReader*&);             // return value: source reader handle

  bool             isFirst_;
  IMFMediaSource*  mediaSource_;
  long             referenceCount_;
  IMFSourceReader* sourceReader_;
  LONGLONG         baseTimeStamp_;
};

#include "stream_dev_cam_source_mediafoundation.inl"

#endif
