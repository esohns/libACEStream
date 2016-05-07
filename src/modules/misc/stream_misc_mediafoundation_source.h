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

#ifndef STREAM_MISC_MEDIAFOUNDATION_SOURCE_H
#define STREAM_MISC_MEDIAFOUNDATION_SOURCE_H

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

#include "stream_misc_directshow_asynch_source_filter.h"
#include "stream_misc_directshow_source_filter.h"

template <typename SessionMessageType,
          typename MessageType,
          ///////////////////////////////
          typename ConfigurationType,
          ///////////////////////////////
          typename SessionDataType,
          ///////////////////////////////
          typename MediaType>
class Stream_Misc_MediaFoundation_Source_T
 : public Stream_TaskBaseSynch_T<Common_TimePolicy_t,
                                 SessionMessageType,
                                 MessageType>
 , public Stream_IModuleHandler_T<ConfigurationType>
 , public IMFSourceReaderCallback
{
  //typedef Stream_Misc_DirectShow_Asynch_Source_Filter_T<Common_TimePolicy_t,
  //                                                      SessionMessageType,
  //                                                      MessageType,

  //                                                      PinConfigurationType,
  //                                                      MediaType> ASYNCH_FILTER_T;
  //typedef Stream_Misc_DirectShow_Source_Filter_T<Common_TimePolicy_t,
  //                                               SessionMessageType,
  //                                               MessageType,

  //                                               PinConfigurationType,
  //                                               MediaType> FILTER_T;
  //friend class ASYNCH_FILTER_T;
  //friend class FILTER_T;

 public:
  //// convenience types
  //typedef Common_IInitialize_T<ConfigurationType> IINITIALIZE_T;

  Stream_Misc_MediaFoundation_Source_T ();
  virtual ~Stream_Misc_MediaFoundation_Source_T ();

  // implement (part of) Stream_ITaskBase_T
  virtual void handleDataMessage (MessageType*&, // data message handle
                                  bool&);        // return value: pass message downstream ?
  virtual void handleSessionMessage (SessionMessageType*&, // session message handle
                                     bool&);               // return value: pass message downstream ?

  // implement Stream_IModuleHandler_T
  virtual bool initialize (const ConfigurationType&);
  virtual const ConfigurationType& get () const;

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

 protected:
  ConfigurationType* configuration_;
  SessionDataType*   sessionData_;

 private:
  typedef Stream_TaskBaseSynch_T<Common_TimePolicy_t,
                                 SessionMessageType,
                                 MessageType> inherited;

  //ACE_UNIMPLEMENTED_FUNC (Stream_Misc_MediaFoundation_Source_T ())
  ACE_UNIMPLEMENTED_FUNC (Stream_Misc_MediaFoundation_Source_T (const Stream_Misc_MediaFoundation_Source_T&))
  ACE_UNIMPLEMENTED_FUNC (Stream_Misc_MediaFoundation_Source_T& operator= (const Stream_Misc_MediaFoundation_Source_T&))

  // helper methods
  MessageType* allocateMessage (unsigned int); // (requested) size
  bool initialize_MediaFoundation (const std::string&,             // (source) device name (FriendlyName)
                                   const HWND,                     // (target) window handle [NULL: NullRenderer]
                                   IMFMediaSource*&,               // media source handle (in/out)
                                   const IDirect3DDeviceManager9*, // Direct3D device manager handle
                                   const IMFSourceReaderCallback*, // source reader callback handle
                                   IMFSourceReader*&);             // return value: source reader handle
  void finalize_MediaFoundation ();

  bool               isInitialized_;
  bool               isFirst_;
  
  IMFMediaSource*    mediaSource_;
  IMFSourceReader*   sourceReader_;
};

// include template definition
#include "stream_misc_mediafoundation_source.inl"

#endif
