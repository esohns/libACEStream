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

#include "ace/Guard_T.h"
#include "ace/Log_Msg.h"

#include <mfapi.h>
#include <mferror.h>
#include <shlwapi.h>

#include "stream_macros.h"

template <typename TimePolicyType,
          typename SessionMessageType,
          typename ProtocolMessageType,
          typename ConfigurationType,
          typename MediaType>
HRESULT
Stream_MediaFramework_MediaFoundation_MediaSource_T<TimePolicyType,
                                                    SessionMessageType,
                                                    ProtocolMessageType,
                                                    ConfigurationType,
                                                    MediaType>::CreateInstance (IUnknown* parent_in,
                                                                                REFIID interfaceID_in,
                                                                                void** interface_out)
{
  STREAM_TRACE (ACE_TEXT ("Stream_MediaFramework_MediaFoundation_MediaSource_T::CreateInstance"));

  // sanity check(s)
  CheckPointer (interface_out, E_POINTER);
  // This object does not support aggregation.
  if (parent_in)
    return CLASS_E_NOAGGREGATION;

  HRESULT result = S_OK;
  OWN_TYPE_T* instance_p = NULL;
  ACE_NEW_NORETURN (instance_p,
                    OWN_TYPE_T (&result));
  if (!instance_p)
  {
    ACE_DEBUG ((LM_CRITICAL,
                ACE_TEXT ("failed to allocate memory: \"%m\", aborting\n")));
    return E_OUTOFMEMORY;
  } // end IF
  if (FAILED (result))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to instantiate Stream_MediaFramework_MediaFoundation_MediaSource_T: \"%s\", aborting\n"),
                ACE_TEXT (Common_Error_Tools::errorToString (result).c_str ())));
    instance_p->Release (); instance_p = NULL;
    return result;
  } // end IF
  result = instance_p->QueryInterface (interfaceID_in, interface_out);
  if (FAILED (result))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Stream_MediaFramework_MediaFoundation_MediaSource_T::QueryInterface(): \"%s\", aborting\n"),
                ACE_TEXT (Common_Error_Tools::errorToString (result).c_str ())));
    instance_p->Release (); instance_p = NULL;
    return result;
  } // end IF
  ACE_ASSERT (interface_out);
  instance_p->Release (); instance_p = NULL;

  return result;
}

template <typename TimePolicyType,
          typename SessionMessageType,
          typename ProtocolMessageType,
          typename ConfigurationType,
          typename MediaType>
Stream_MediaFramework_MediaFoundation_MediaSource_T<TimePolicyType,
                                                    SessionMessageType,
                                                    ProtocolMessageType,
                                                    ConfigurationType,
                                                    MediaType>::Stream_MediaFramework_MediaFoundation_MediaSource_T (HRESULT* result_out)
 : eventQueue_ (NULL)
//, hasCOMReference_ (false)
 , lock_ ()
 , referenceCount_ (1)
 , state_ (STATE_INVALID)
{
  STREAM_TRACE (ACE_TEXT ("Stream_MediaFramework_MediaFoundation_MediaSource_T::Stream_MediaFramework_MediaFoundation_MediaSource_T"));

  // sanity check(s)
  ACE_ASSERT (result_out);

  //{ ACE_GUARD (ACE_SYNCH_MUTEX, aGuard, lock_);
    *result_out = MFCreateEventQueue (&eventQueue_);
    if (FAILED (*result_out))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to MFCreateEventQueue(): \"%s\", returning\n"),
                  ACE_TEXT (Common_Error_Tools::errorToString (*result_out).c_str ())));
      return;
    } // end IF
  //} // end lock scope
}
template <typename TimePolicyType,
          typename SessionMessageType,
          typename ProtocolMessageType,
          typename ConfigurationType,
          typename MediaType>
Stream_MediaFramework_MediaFoundation_MediaSource_T<TimePolicyType,
                                                    SessionMessageType,
                                                    ProtocolMessageType,
                                                    ConfigurationType,
                                                    MediaType>::~Stream_MediaFramework_MediaFoundation_MediaSource_T ()
{
  STREAM_TRACE (ACE_TEXT ("Stream_MediaFramework_MediaFoundation_MediaSource_T::~Stream_MediaFramework_MediaFoundation_MediaSource_T"));

  HRESULT result = E_FAIL;

  { ACE_GUARD (ACE_SYNCH_MUTEX, aGuard, lock_);
    if (state_ != STATE_SHUTDOWN)
    {
      result = Shutdown ();
      if (FAILED (result))
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to Stream_MediaFramework_MediaFoundation_MediaSource_T::Shutdown(): \"%s\", continuing\n"),
                    ACE_TEXT (Common_Error_Tools::errorToString (result).c_str ())));
    } // end IF
    if (eventQueue_)
      eventQueue_->Release ();
  } // end lock scope
}

template <typename TimePolicyType,
          typename SessionMessageType,
          typename ProtocolMessageType,
          typename ConfigurationType,
          typename MediaType>
HRESULT
Stream_MediaFramework_MediaFoundation_MediaSource_T<TimePolicyType,
                                                    SessionMessageType,
                                                    ProtocolMessageType,
                                                    ConfigurationType,
                                                    MediaType>::BeginCreateObject (LPCWSTR pwszURL,
                                                                                   DWORD dwFlags,
                                                                                   IPropertyStore* pProps,
                                                                                   IUnknown** ppIUnknownCancelCookie,
                                                                                   IMFAsyncCallback* pCallback,
                                                                                   IUnknown* punkState)
{
  STREAM_TRACE (ACE_TEXT ("Stream_MediaFramework_MediaFoundation_MediaSource_T::BeginCreateObject"));

  ACE_ASSERT (false);
  ACE_NOTSUP_RETURN (E_FAIL);
  ACE_NOTREACHED (return E_FAIL);
}
template <typename TimePolicyType,
          typename SessionMessageType,
          typename ProtocolMessageType,
          typename ConfigurationType,
          typename MediaType>
HRESULT
Stream_MediaFramework_MediaFoundation_MediaSource_T<TimePolicyType,
                                                    SessionMessageType,
                                                    ProtocolMessageType,
                                                    ConfigurationType,
                                                    MediaType>::CancelObjectCreation (IUnknown* pIUnknownCancelCookie)
{
  STREAM_TRACE (ACE_TEXT ("Stream_MediaFramework_MediaFoundation_MediaSource_T::CancelObjectCreation"));

  ACE_ASSERT (false);
  ACE_NOTSUP_RETURN (E_FAIL);
  ACE_NOTREACHED (return E_FAIL);
}
template <typename TimePolicyType,
          typename SessionMessageType,
          typename ProtocolMessageType,
          typename ConfigurationType,
          typename MediaType>
HRESULT
Stream_MediaFramework_MediaFoundation_MediaSource_T<TimePolicyType,
                                                    SessionMessageType,
                                                    ProtocolMessageType,
                                                    ConfigurationType,
                                                    MediaType>::EndCreateObject (IMFAsyncResult* pResult,
                                                                                 MF_OBJECT_TYPE* pObjectType,
                                                                                 IUnknown** ppObject)
{
  STREAM_TRACE (ACE_TEXT ("Stream_MediaFramework_MediaFoundation_MediaSource_T::EndCreateObject"));

  ACE_ASSERT (false);
  ACE_NOTSUP_RETURN (E_FAIL);
  ACE_NOTREACHED (return E_FAIL);
}

template <typename TimePolicyType,
          typename SessionMessageType,
          typename ProtocolMessageType,
          typename ConfigurationType,
          typename MediaType>
HRESULT
Stream_MediaFramework_MediaFoundation_MediaSource_T<TimePolicyType,
                                                    SessionMessageType,
                                                    ProtocolMessageType,
                                                    ConfigurationType,
                                                    MediaType>::QueryInterface (REFIID IID_in,
                                                                                void** interface_out)
{
  STREAM_TRACE (ACE_TEXT ("Stream_MediaFramework_MediaFoundation_MediaSource_T::QueryInterface"));

  static const QITAB query_interface_table[] =
  {
    QITABENT (OWN_TYPE_T, IMFSchemeHandler),
    QITABENT (OWN_TYPE_T, IMFMediaEventGenerator),
#if COMMON_OS_WIN32_TARGET_PLATFORM (0x0602) // _WIN32_WINNT_WIN8
    QITABENT (OWN_TYPE_T, IMFMediaSourceEx),
#else
    QITABENT (OWN_TYPE_T, IMFMediaSource),
#endif // _WIN32_WINNT_WIN8
    //QITABENT (OWN_TYPE_T, IMarshal),
    { 0 },
  };

  return QISearch (this,
                   query_interface_table,
                   IID_in,
                   interface_out);
}

template <typename TimePolicyType,
          typename SessionMessageType,
          typename ProtocolMessageType,
          typename ConfigurationType,
          typename MediaType>
ULONG
Stream_MediaFramework_MediaFoundation_MediaSource_T<TimePolicyType,
                                                    SessionMessageType,
                                                    ProtocolMessageType,
                                                    ConfigurationType,
                                                    MediaType>::Release ()
{
  STREAM_TRACE (ACE_TEXT ("Stream_MediaFramework_MediaFoundation_MediaSource_T::Release"));

  ULONG count = InterlockedDecrement (&referenceCount_);
  //if (count == 0)
  //delete this;

  return count;
}

template <typename TimePolicyType,
          typename SessionMessageType,
          typename ProtocolMessageType,
          typename ConfigurationType,
          typename MediaType>
HRESULT
Stream_MediaFramework_MediaFoundation_MediaSource_T<TimePolicyType,
                                                    SessionMessageType,
                                                    ProtocolMessageType,
                                                    ConfigurationType,
                                                    MediaType>::BeginGetEvent (IMFAsyncCallback* asynchCallback_in,
                                                                               IUnknown* state_in)
{
  STREAM_TRACE (ACE_TEXT ("Stream_MediaFramework_MediaFoundation_MediaSource_T::BeginGetEvent"));

  // sanity check(s)
  ACE_ASSERT (eventQueue_);

  HRESULT result = S_OK;

  { ACE_GUARD_RETURN (ACE_SYNCH_MUTEX, aGuard, lock_, E_FAIL);
    result = (state_ == STATE_SHUTDOWN ? MF_E_SHUTDOWN : S_OK);
    if (FAILED (result))
      goto done;
    result = eventQueue_->BeginGetEvent (asynchCallback_in, state_in);
    if (FAILED (result))
      goto done;
  } // end lock scope

done:
  return result;
}
template <typename TimePolicyType,
          typename SessionMessageType,
          typename ProtocolMessageType,
          typename ConfigurationType,
          typename MediaType>
HRESULT
Stream_MediaFramework_MediaFoundation_MediaSource_T<TimePolicyType,
                                                    SessionMessageType,
                                                    ProtocolMessageType,
                                                    ConfigurationType,
                                                    MediaType>::EndGetEvent (IMFAsyncResult* asynchResult_in,
                                                                             IMFMediaEvent** event_out)
{
  STREAM_TRACE (ACE_TEXT ("Stream_MediaFramework_MediaFoundation_MediaSource_T::EndGetEvent"));

  // sanity check(s)
  ACE_ASSERT (eventQueue_);

  HRESULT result = S_OK;

  { ACE_GUARD_RETURN (ACE_SYNCH_MUTEX, aGuard, lock_, E_FAIL);
    result = (state_ == STATE_SHUTDOWN ? MF_E_SHUTDOWN : S_OK);
    if (FAILED (result))
      goto done;
    result = eventQueue_->EndGetEvent (asynchResult_in, event_out);
    if (FAILED (result))
      goto done;
  } // end lock scope

done:
  return result;
}

template <typename TimePolicyType,
          typename SessionMessageType,
          typename ProtocolMessageType,
          typename ConfigurationType,
          typename MediaType>
HRESULT
Stream_MediaFramework_MediaFoundation_MediaSource_T<TimePolicyType,
                                                    SessionMessageType,
                                                    ProtocolMessageType,
                                                    ConfigurationType,
                                                    MediaType>::GetEvent (DWORD flags_in,
                                                                          IMFMediaEvent** event_out)
{
  STREAM_TRACE (ACE_TEXT ("Stream_MediaFramework_MediaFoundation_MediaSource_T::GetEvent"));

  HRESULT result = S_OK;

  IMFMediaEventQueue* event_queue_p = NULL;

  // *NOTE*: this could block indefinitely
  //         --> don't hold onto the critical section
  ULONG reference_count = 0;
  { ACE_GUARD_RETURN (ACE_SYNCH_MUTEX, aGuard, lock_, E_FAIL);
    result = (state_ == STATE_SHUTDOWN ? MF_E_SHUTDOWN : S_OK);
    if (FAILED (result))
      goto done;
    event_queue_p = eventQueue_;
    reference_count = event_queue_p->AddRef ();
  } // lock scope

  result = event_queue_p->GetEvent (flags_in, event_out);
  if (FAILED (result))
    goto done;

done:
  if (event_queue_p)
    event_queue_p->Release ();

  return result;
}

template <typename TimePolicyType,
          typename SessionMessageType,
          typename ProtocolMessageType,
          typename ConfigurationType,
          typename MediaType>
HRESULT
Stream_MediaFramework_MediaFoundation_MediaSource_T<TimePolicyType,
                                                    SessionMessageType,
                                                    ProtocolMessageType,
                                                    ConfigurationType,
                                                    MediaType>::QueueEvent (MediaEventType type_in,
                                                                            REFGUID extendedType_in,
                                                                            HRESULT status_in,
                                                                            const PROPVARIANT* value_in)
{
  STREAM_TRACE (ACE_TEXT ("Stream_MediaFramework_MediaFoundation_MediaSource_T::QueueEvent"));

  // sanity check(s)
  ACE_ASSERT (eventQueue_);

  HRESULT result = S_OK;

  { ACE_GUARD_RETURN (ACE_SYNCH_MUTEX, aGuard, lock_, E_FAIL);
    result = (state_ == STATE_SHUTDOWN ? MF_E_SHUTDOWN : S_OK);
    if (FAILED (result))
      goto done;
    result = eventQueue_->QueueEventParamVar (type_in,
                                              extendedType_in,
                                              status_in,
                                              value_in);
    if (FAILED (result))
      goto done;
  } // end lock scope

done:
  return result;
}

template <typename TimePolicyType,
          typename SessionMessageType,
          typename ProtocolMessageType,
          typename ConfigurationType,
          typename MediaType>
HRESULT
Stream_MediaFramework_MediaFoundation_MediaSource_T<TimePolicyType,
                                                    SessionMessageType,
                                                    ProtocolMessageType,
                                                    ConfigurationType,
                                                    MediaType>::GetCharacteristics (DWORD* characteristics_out)
{
  STREAM_TRACE (ACE_TEXT ("Stream_MediaFramework_MediaFoundation_MediaSource_T::GetCharacteristics"));

  ACE_UNUSED_ARG (characteristics_out);

  ACE_ASSERT (false);
  ACE_NOTSUP_RETURN (E_FAIL);

  ACE_NOTREACHED (return E_FAIL;)
}
template <typename TimePolicyType,
          typename SessionMessageType,
          typename ProtocolMessageType,
          typename ConfigurationType,
          typename MediaType>
HRESULT
Stream_MediaFramework_MediaFoundation_MediaSource_T<TimePolicyType,
                                                    SessionMessageType,
                                                    ProtocolMessageType,
                                                    ConfigurationType,
                                                    MediaType>::CreatePresentationDescriptor (IMFPresentationDescriptor** presentationDescriptor_out)
{
  STREAM_TRACE (ACE_TEXT ("Stream_MediaFramework_MediaFoundation_MediaSource_T::CreatePresentationDescriptor"));

  // sanity check(s)
  ACE_ASSERT (presentationDescriptor_out);

  *presentationDescriptor_out = this;

  return S_OK;
}
template <typename TimePolicyType,
          typename SessionMessageType,
          typename ProtocolMessageType,
          typename ConfigurationType,
          typename MediaType>
HRESULT
Stream_MediaFramework_MediaFoundation_MediaSource_T<TimePolicyType,
                                                    SessionMessageType,
                                                    ProtocolMessageType,
                                                    ConfigurationType,
                                                    MediaType>::Start (IMFPresentationDescriptor* presentationDescriptor_in,
                                                                       const GUID* timeFormat_in,
                                                                       const PROPVARIANT* startPosition_in)
{
  STREAM_TRACE (ACE_TEXT ("Stream_MediaFramework_MediaFoundation_MediaSource_T::Start"));

  ACE_UNUSED_ARG (presentationDescriptor_in);
  ACE_UNUSED_ARG (timeFormat_in);
  ACE_UNUSED_ARG (startPosition_in);

  ACE_ASSERT (false);
  ACE_NOTSUP_RETURN (E_FAIL);

  ACE_NOTREACHED (return E_FAIL;)
}
template <typename TimePolicyType,
          typename SessionMessageType,
          typename ProtocolMessageType,
          typename ConfigurationType,
          typename MediaType>
HRESULT
Stream_MediaFramework_MediaFoundation_MediaSource_T<TimePolicyType,
                                                    SessionMessageType,
                                                    ProtocolMessageType,
                                                    ConfigurationType,
                                                    MediaType>::Stop (void)
{
  STREAM_TRACE (ACE_TEXT ("Stream_MediaFramework_MediaFoundation_MediaSource_T::Stop"));

  ACE_ASSERT (false);
  ACE_NOTSUP_RETURN (E_FAIL);

  ACE_NOTREACHED (return E_FAIL;)
}
template <typename TimePolicyType,
          typename SessionMessageType,
          typename ProtocolMessageType,
          typename ConfigurationType,
          typename MediaType>
HRESULT
Stream_MediaFramework_MediaFoundation_MediaSource_T<TimePolicyType,
                                                    SessionMessageType,
                                                    ProtocolMessageType,
                                                    ConfigurationType,
                                                    MediaType>::Pause (void)
{
  STREAM_TRACE (ACE_TEXT ("Stream_MediaFramework_MediaFoundation_MediaSource_T::Pause"));

  ACE_ASSERT (false);
  ACE_NOTSUP_RETURN (E_FAIL);

  ACE_NOTREACHED (return E_FAIL;)
}
template <typename TimePolicyType,
          typename SessionMessageType,
          typename ProtocolMessageType,
          typename ConfigurationType,
          typename MediaType>
HRESULT
Stream_MediaFramework_MediaFoundation_MediaSource_T<TimePolicyType,
                                                    SessionMessageType,
                                                    ProtocolMessageType,
                                                    ConfigurationType,
                                                    MediaType>::Shutdown (void)
{
  STREAM_TRACE (ACE_TEXT ("Stream_MediaFramework_MediaFoundation_MediaSource_T::Shutdown"));

  ACE_ASSERT (false);
  ACE_NOTSUP_RETURN (E_FAIL);

  ACE_NOTREACHED (return E_FAIL;)
}

template <typename TimePolicyType,
          typename SessionMessageType,
          typename ProtocolMessageType,
          typename ConfigurationType,
          typename MediaType>
HRESULT
Stream_MediaFramework_MediaFoundation_MediaSource_T<TimePolicyType,
                                                    SessionMessageType,
                                                    ProtocolMessageType,
                                                    ConfigurationType,
                                                    MediaType>::GetSourceAttributes (IMFAttributes** attributes_out)
{
  STREAM_TRACE (ACE_TEXT ("Stream_MediaFramework_MediaFoundation_MediaSource_T::GetSourceAttributes"));

  ACE_UNUSED_ARG (attributes_out);

  ACE_ASSERT (false);
  ACE_NOTSUP_RETURN (E_FAIL);

  ACE_NOTREACHED (return E_FAIL;)
}
template <typename TimePolicyType,
          typename SessionMessageType,
          typename ProtocolMessageType,
          typename ConfigurationType,
          typename MediaType>
HRESULT
Stream_MediaFramework_MediaFoundation_MediaSource_T<TimePolicyType,
                                                    SessionMessageType,
                                                    ProtocolMessageType,
                                                    ConfigurationType,
                                                    MediaType>::GetStreamAttributes (DWORD streamIdentifier_in,
                                                                                     IMFAttributes** attributes_out)
{
  STREAM_TRACE (ACE_TEXT ("Stream_MediaFramework_MediaFoundation_MediaSource_T::GetStreamAttributes"));

  ACE_UNUSED_ARG (streamIdentifier_in);
  ACE_UNUSED_ARG (attributes_out);

  ACE_ASSERT (false);
  ACE_NOTSUP_RETURN (E_FAIL);

  ACE_NOTREACHED (return E_FAIL;)
}
template <typename TimePolicyType,
          typename SessionMessageType,
          typename ProtocolMessageType,
          typename ConfigurationType,
          typename MediaType>
HRESULT
Stream_MediaFramework_MediaFoundation_MediaSource_T<TimePolicyType,
                                                    SessionMessageType,
                                                    ProtocolMessageType,
                                                    ConfigurationType,
                                                    MediaType>::SetD3DManager (IUnknown* Direct3DManager_in)
{
  STREAM_TRACE (ACE_TEXT ("Stream_MediaFramework_MediaFoundation_MediaSource_T::SetD3DManager"));

  ACE_UNUSED_ARG (Direct3DManager_in);

  ACE_ASSERT (false);
  ACE_NOTSUP_RETURN (E_FAIL);

  ACE_NOTREACHED (return E_FAIL;)
}

template <typename TimePolicyType,
          typename SessionMessageType,
          typename ProtocolMessageType,
          typename ConfigurationType,
          typename MediaType>
HRESULT
Stream_MediaFramework_MediaFoundation_MediaSource_T<TimePolicyType,
                                                    SessionMessageType,
                                                    ProtocolMessageType,
                                                    ConfigurationType,
                                                    MediaType>::Clone (IMFPresentationDescriptor** ppPresentationDescriptor)
{
  STREAM_TRACE (ACE_TEXT ("Stream_MediaFramework_MediaFoundation_MediaSource_T::Clone"));

  ACE_UNUSED_ARG (ppPresentationDescriptor);

  ACE_ASSERT (false);
  ACE_NOTSUP_RETURN (E_FAIL);

  ACE_NOTREACHED (return E_FAIL;)
}
template <typename TimePolicyType,
          typename SessionMessageType,
          typename ProtocolMessageType,
          typename ConfigurationType,
          typename MediaType>
HRESULT
Stream_MediaFramework_MediaFoundation_MediaSource_T<TimePolicyType,
                                                    SessionMessageType,
                                                    ProtocolMessageType,
                                                    ConfigurationType,
                                                    MediaType>::DeselectStream (DWORD dwIndex)
{
  STREAM_TRACE (ACE_TEXT ("Stream_MediaFramework_MediaFoundation_MediaSource_T::DeselectStream"));

  ACE_UNUSED_ARG (dwIndex);

  ACE_ASSERT (false);
  ACE_NOTSUP_RETURN (E_FAIL);

  ACE_NOTREACHED (return E_FAIL;)
}
template <typename TimePolicyType,
          typename SessionMessageType,
          typename ProtocolMessageType,
          typename ConfigurationType,
          typename MediaType>
HRESULT
Stream_MediaFramework_MediaFoundation_MediaSource_T<TimePolicyType,
                                                    SessionMessageType,
                                                    ProtocolMessageType,
                                                    ConfigurationType,
                                                    MediaType>::GetStreamDescriptorByIndex (DWORD dwIndex,
                                                                                            BOOL* pfSelected,
                                                                                            IMFStreamDescriptor** ppStreamDescriptor)
{
  STREAM_TRACE (ACE_TEXT ("Stream_MediaFramework_MediaFoundation_MediaSource_T::GetStreamDescriptorByIndex"));

  ACE_UNUSED_ARG (dwIndex);

  // sanity check(s)
  ACE_ASSERT (pfSelected);
  ACE_ASSERT (ppStreamDescriptor);

  *pfSelected = TRUE;
  *ppStreamDescriptor = this;

  return S_OK;
}
template <typename TimePolicyType,
          typename SessionMessageType,
          typename ProtocolMessageType,
          typename ConfigurationType,
          typename MediaType>
HRESULT
Stream_MediaFramework_MediaFoundation_MediaSource_T<TimePolicyType,
                                                    SessionMessageType,
                                                    ProtocolMessageType,
                                                    ConfigurationType,
                                                    MediaType>::GetStreamDescriptorCount (DWORD* pdwDescriptorCount)
{
  STREAM_TRACE (ACE_TEXT ("Stream_MediaFramework_MediaFoundation_MediaSource_T::DeselectStream"));

  // sanity check(s)
  ACE_ASSERT (pdwDescriptorCount);

  *pdwDescriptorCount = 1;

  return S_OK;
}
template <typename TimePolicyType,
          typename SessionMessageType,
          typename ProtocolMessageType,
          typename ConfigurationType,
          typename MediaType>
HRESULT
Stream_MediaFramework_MediaFoundation_MediaSource_T<TimePolicyType,
                                                    SessionMessageType,
                                                    ProtocolMessageType,
                                                    ConfigurationType,
                                                    MediaType>::SelectStream (DWORD dwIndex)
{
  STREAM_TRACE (ACE_TEXT ("Stream_MediaFramework_MediaFoundation_MediaSource_T::SelectStream"));

  ACE_UNUSED_ARG (dwIndex);

  return S_OK;
}

template <typename TimePolicyType,
          typename SessionMessageType,
          typename ProtocolMessageType,
          typename ConfigurationType,
          typename MediaType>
HRESULT
Stream_MediaFramework_MediaFoundation_MediaSource_T<TimePolicyType,
                                                    SessionMessageType,
                                                    ProtocolMessageType,
                                                    ConfigurationType,
                                                    MediaType>::GetMediaTypeHandler (IMFMediaTypeHandler** ppMediaTypeHandler)
{
  STREAM_TRACE (ACE_TEXT ("Stream_MediaFramework_MediaFoundation_MediaSource_T::GetMediaTypeHandler"));

  // sanity check(s)
  ACE_ASSERT (ppMediaTypeHandler);

  *ppMediaTypeHandler = this;

  return S_OK;
}
template <typename TimePolicyType,
          typename SessionMessageType,
          typename ProtocolMessageType,
          typename ConfigurationType,
          typename MediaType>
HRESULT
Stream_MediaFramework_MediaFoundation_MediaSource_T<TimePolicyType,
                                                    SessionMessageType,
                                                    ProtocolMessageType,
                                                    ConfigurationType,
                                                    MediaType>::GetStreamIdentifier (DWORD* pdwStreamIdentifier)
{
  STREAM_TRACE (ACE_TEXT ("Stream_MediaFramework_MediaFoundation_MediaSource_T::GetStreamIdentifier"));

  // sanity check(s)
  ACE_ASSERT (pdwStreamIdentifier);

  *pdwStreamIdentifier = 0;

  return S_OK;
}

template <typename TimePolicyType,
          typename SessionMessageType,
          typename ProtocolMessageType,
          typename ConfigurationType,
          typename MediaType>
HRESULT
Stream_MediaFramework_MediaFoundation_MediaSource_T<TimePolicyType,
                                                    SessionMessageType,
                                                    ProtocolMessageType,
                                                    ConfigurationType,
                                                    MediaType>::GetCurrentMediaType (IMFMediaType** ppMediaType)
{
  STREAM_TRACE (ACE_TEXT ("Stream_MediaFramework_MediaFoundation_MediaSource_T::GetCurrentMediaType"));

  // sanity check(s)
  ACE_ASSERT (ppMediaType && !*ppMediaType);

  HRESULT result = MFCreateMediaType (ppMediaType);
  ACE_ASSERT (SUCCEEDED (result)); ACE_ASSERT (*ppMediaType);

  result = (*ppMediaType)->SetGUID (MF_MT_MAJOR_TYPE,
                                    MFMediaType_Video);
  ACE_ASSERT (SUCCEEDED (result));
  result =
    (*ppMediaType)->SetUINT32 (MF_MT_INTERLACE_MODE,
                               MFVideoInterlace_Unknown);
  ACE_ASSERT (SUCCEEDED (result));
  result = MFSetAttributeRatio (*ppMediaType,
                                MF_MT_PIXEL_ASPECT_RATIO,
                                1, 1);
  ACE_ASSERT (SUCCEEDED (result));
  result = (*ppMediaType)->SetGUID (MF_MT_SUBTYPE,
                                    MFVideoFormat_RGB32);
  ACE_ASSERT (SUCCEEDED (result));
  result = MFSetAttributeSize (*ppMediaType,
                               MF_MT_FRAME_RATE,
                               STREAM_DEV_CAM_DEFAULT_CAPTURE_RATE, 1);
  ACE_ASSERT (SUCCEEDED (result));
  result = MFSetAttributeSize (*ppMediaType,
                               MF_MT_FRAME_SIZE,
                               STREAM_DEV_CAM_DEFAULT_CAPTURE_SIZE_WIDTH,
                               STREAM_DEV_CAM_DEFAULT_CAPTURE_SIZE_HEIGHT);
  ACE_ASSERT (SUCCEEDED (result));
  result = (*ppMediaType)->SetUINT32 (MF_MT_ALL_SAMPLES_INDEPENDENT,
                                      1);
  ACE_ASSERT (SUCCEEDED (result));
  result = (*ppMediaType)->SetUINT32 (MF_MT_FIXED_SIZE_SAMPLES,
                                      1);
  ACE_ASSERT (SUCCEEDED (result));
  UINT32 frame_size = 0;
  result = MFCalculateImageSize (MFVideoFormat_RGB32,
                                 STREAM_DEV_CAM_DEFAULT_CAPTURE_SIZE_WIDTH,
                                 STREAM_DEV_CAM_DEFAULT_CAPTURE_SIZE_HEIGHT,
                                 &frame_size);
  ACE_ASSERT (SUCCEEDED (result));
  result = (*ppMediaType)->SetUINT32 (MF_MT_SAMPLE_SIZE,
                                      frame_size);
  ACE_ASSERT (SUCCEEDED (result));

  return S_OK;
}
template <typename TimePolicyType,
          typename SessionMessageType,
          typename ProtocolMessageType,
          typename ConfigurationType,
          typename MediaType>
HRESULT
Stream_MediaFramework_MediaFoundation_MediaSource_T<TimePolicyType,
                                                    SessionMessageType,
                                                    ProtocolMessageType,
                                                    ConfigurationType,
                                                    MediaType>::GetMajorType (GUID* pguidMajorType)
{
  STREAM_TRACE (ACE_TEXT ("Stream_MediaFramework_MediaFoundation_MediaSource_T::GetMajorType"));

  // sanity check(s)
  ACE_ASSERT (pguidMajorType);

  *pguidMajorType = MFMediaType_Video;

  return S_OK;
}
template <typename TimePolicyType,
          typename SessionMessageType,
          typename ProtocolMessageType,
          typename ConfigurationType,
          typename MediaType>
HRESULT
Stream_MediaFramework_MediaFoundation_MediaSource_T<TimePolicyType,
                                                    SessionMessageType,
                                                    ProtocolMessageType,
                                                    ConfigurationType,
                                                    MediaType>::GetMediaTypeByIndex (DWORD dwIndex,
                                                                                     IMFMediaType** ppType)
{
  STREAM_TRACE (ACE_TEXT ("Stream_MediaFramework_MediaFoundation_MediaSource_T::GetMajorType"));

  // sanity check(s)
  ACE_ASSERT (!ppType);

  ACE_ASSERT (false);

  return S_OK;
}
template <typename TimePolicyType,
          typename SessionMessageType,
          typename ProtocolMessageType,
          typename ConfigurationType,
          typename MediaType>
HRESULT
Stream_MediaFramework_MediaFoundation_MediaSource_T<TimePolicyType,
                                                    SessionMessageType,
                                                    ProtocolMessageType,
                                                    ConfigurationType,
                                                    MediaType>::GetMediaTypeCount (DWORD* pdwTypeCount)
{
  STREAM_TRACE (ACE_TEXT ("Stream_MediaFramework_MediaFoundation_MediaSource_T::GetMediaTypeCount"));

  // sanity check(s)
  ACE_ASSERT (!pdwTypeCount);

  *pdwTypeCount = 1;

  return S_OK;
}
template <typename TimePolicyType,
          typename SessionMessageType,
          typename ProtocolMessageType,
          typename ConfigurationType,
          typename MediaType>
HRESULT
Stream_MediaFramework_MediaFoundation_MediaSource_T<TimePolicyType,
                                                    SessionMessageType,
                                                    ProtocolMessageType,
                                                    ConfigurationType,
                                                    MediaType>::IsMediaTypeSupported (IMFMediaType* pMediaType,
                                                                                      IMFMediaType** ppMediaType)
{
  STREAM_TRACE (ACE_TEXT ("Stream_MediaFramework_MediaFoundation_MediaSource_T::IsMediaTypeSupported"));

  // sanity check(s)
  ACE_ASSERT (pMediaType);

  ACE_ASSERT (false);

  return S_OK;
}
template <typename TimePolicyType,
          typename SessionMessageType,
          typename ProtocolMessageType,
          typename ConfigurationType,
          typename MediaType>
HRESULT
Stream_MediaFramework_MediaFoundation_MediaSource_T<TimePolicyType,
                                                    SessionMessageType,
                                                    ProtocolMessageType,
                                                    ConfigurationType,
                                                    MediaType>::SetCurrentMediaType (IMFMediaType* pMediaType)
{
  STREAM_TRACE (ACE_TEXT ("Stream_MediaFramework_MediaFoundation_MediaSource_T::IsMediaTypeSupported"));

  // sanity check(s)
  ACE_ASSERT (pMediaType);

  return S_OK;
}

//template <typename TimePolicyType,
//          typename SessionMessageType,
//          typename ProtocolMessageType,
//          typename ConfigurationType,
//          typename MediaType>
//HRESULT
//Stream_MediaFramework_MediaFoundation_MediaSource_T<TimePolicyType,
//                                                    SessionMessageType,
//                                                    ProtocolMessageType,
//                                                    ConfigurationType,
//                                                    MediaType>::GetItem (REFGUID guidKey,
//                                                                         PROPVARIANT* pValue)
//{
//  ACE_ASSERT (false);
//}
//template <typename TimePolicyType,
//          typename SessionMessageType,
//          typename ProtocolMessageType,
//          typename ConfigurationType,
//          typename MediaType>
//HRESULT
//Stream_MediaFramework_MediaFoundation_MediaSource_T<TimePolicyType,
//                                                    SessionMessageType,
//                                                    ProtocolMessageType,
//                                                    ConfigurationType,
//                                                    MediaType>::GetItemType (REFGUID guidKey,
//                                                                             MF_ATTRIBUTE_TYPE* pType)
//{
//  ACE_ASSERT (false);
//}
//template <typename TimePolicyType,
//          typename SessionMessageType,
//          typename ProtocolMessageType,
//          typename ConfigurationType,
//          typename MediaType>
//HRESULT
//Stream_MediaFramework_MediaFoundation_MediaSource_T<TimePolicyType,
//                                                    SessionMessageType,
//                                                    ProtocolMessageType,
//                                                    ConfigurationType,
//                                                    MediaType>::CompareItem (REFGUID guidKey,
//                                                                             REFPROPVARIANT Value,
//                                                                             BOOL* pbResult)
//{
//  ACE_ASSERT (false);
//}
//template <typename TimePolicyType,
//          typename SessionMessageType,
//          typename ProtocolMessageType,
//          typename ConfigurationType,
//          typename MediaType>
//HRESULT
//Stream_MediaFramework_MediaFoundation_MediaSource_T<TimePolicyType,
//                                                    SessionMessageType,
//                                                    ProtocolMessageType,
//                                                    ConfigurationType,
//                                                    MediaType>::Compare (IMFAttributes* pTheirs,
//                                                                         MF_ATTRIBUTES_MATCH_TYPE MatchType,
//                                                                         BOOL* pbResult)
//{
//  ACE_ASSERT (false);
//}
//template <typename TimePolicyType,
//          typename SessionMessageType,
//          typename ProtocolMessageType,
//          typename ConfigurationType,
//          typename MediaType>
//HRESULT
//Stream_MediaFramework_MediaFoundation_MediaSource_T<TimePolicyType,
//                                                    SessionMessageType,
//                                                    ProtocolMessageType,
//                                                    ConfigurationType,
//                                                    MediaType>::GetUINT32 (REFGUID guidKey,
//                                                                           UINT32* punValue)
//{
//  ACE_ASSERT (false);
//}
  //virtual STDMETHODIMP GetUINT64 (REFGUID, // guidKey
  //                                UINT64); // punValue
  //virtual STDMETHODIMP GetDouble (REFGUID,  // guidKey
  //                                double*); // pfValue
  //virtual STDMETHODIMP GetGUID (REFGUID, // guidKey
  //                              GUID*);  // pguidValue
  //virtual STDMETHODIMP GetStringLength (REFGUID,  // guidKey
  //                                      UINT32*); // pcchLength
  //virtual STDMETHODIMP GetString (REFGUID,  // guidKey
  //                                LPWSTR,   // pwszValue
  //                                UINT32,   // cchBufSize
  //                                UINT32*); // pcchLength
  //virtual STDMETHODIMP GetAllocatedString (REFGUID,  // guidKey
  //                                         LPWSTR*,  // ppwszValue
  //                                         UINT32*); // pcchLength
  //virtual STDMETHODIMP GetBlobSize (REFGUID,  // guidKey
  //                                  UINT32*); // pcbBlobSize
  //virtual STDMETHODIMP GetBlob (REFGUID,  // guidKey
  //                              UINT8*,   // pBuf
  //                              UINT32,   // cbBufSize
  //                              UINT32*); // pcbBlobSize
  //virtual STDMETHODIMP GetAllocatedBlob (REFGUID,  // guidKey
  //                                       UINT8**,  // ppBuf
  //                                       UINT32*); // pcbSize
  //virtual STDMETHODIMP GetUnknown (REFGUID, // guidKey
  //                                 REFIID,  // riid
  //                                 LPVOID); // ppv
  //virtual STDMETHODIMP SetItem (REFGUID,         // guidKey
  //                              REFPROPVARIANT); // Value
  //virtual STDMETHODIMP DeleteItem (REFGUID); // guidKey
  //virtual STDMETHODIMP DeleteAllItems (void);
  //virtual STDMETHODIMP SetUINT32 (REFGUID, // guidKey
  //                                UINT32); // unValue
  //virtual STDMETHODIMP SetUINT64 (REFGUID, // guidKey
  //                                UINT64); // unValue
  //virtual STDMETHODIMP SetDouble (REFGUID, // guidKey
  //                                double); // fValue
  //virtual STDMETHODIMP SetGUID (REFGUID,  // guidKey
  //                              REFGUID); // guidValue
  //virtual STDMETHODIMP SetString (REFGUID,  // guidKey
  //                                LPCWSTR); // wszValue
  //virtual STDMETHODIMP SetBlob (REFGUID,      // guidKey
  //                              const UINT8*, // pBuf
  //                              UINT32);      // cbBufSize
  //virtual STDMETHODIMP SetUnknown (REFGUID,    // guidKey
  //                                 IUnknown*); // pUnknown
  //virtual STDMETHODIMP LockStore (void);
  //virtual STDMETHODIMP UnlockStore (void);
  //virtual STDMETHODIMP GetCount (UINT32*); // pcItems
  //virtual STDMETHODIMP GetItemByIndex (UINT32,        // unIndex
  //                                     GUID*,         // pguidKey
  //                                     PROPVARIANT*); // pValue
  //virtual STDMETHODIMP CopyAllItems (IMFAttributes); // pDest


//template <typename TimePolicyType,
//          typename SessionMessageType,
//          typename ProtocolMessageType,
//          typename ConfigurationType,
//          typename MediaType>
//HRESULT
//Stream_MediaFramework_MediaFoundation_MediaSource_T<TimePolicyType,
//                                                    SessionMessageType,
//                                                    ProtocolMessageType,
//                                                    ConfigurationType,
//                                                    MediaType>::GetUnmarshalClass (REFIID riid,
//                                                                                   void *pv,
//                                                                                   DWORD dwDestContext,
//                                                                                   void *pvDestContext,
//                                                                                   DWORD mshlflags,
//                                                                                   CLSID *pCid)
//{
//  STREAM_TRACE (ACE_TEXT ("Stream_MediaFramework_MediaFoundation_MediaSource_T::GetUnmarshalClass"));
//
//  ACE_ASSERT (false);
//  ACE_NOTSUP_RETURN (E_FAIL);
//
//  ACE_NOTREACHED (return E_FAIL;)
//}
//template <typename TimePolicyType,
//          typename SessionMessageType,
//          typename ProtocolMessageType,
//          typename ConfigurationType,
//          typename MediaType>
//HRESULT
//Stream_MediaFramework_MediaFoundation_MediaSource_T<TimePolicyType,
//                                                    SessionMessageType,
//                                                    ProtocolMessageType,
//                                                    ConfigurationType,
//                                                    MediaType>::GetMarshalSizeMax (REFIID riid,
//                                                                                   void *pv,
//                                                                                   DWORD dwDestContext,
//                                                                                   void *pvDestContext,
//                                                                                   DWORD mshlflags,
//                                                                                   DWORD *pSize)
//{
//  STREAM_TRACE (ACE_TEXT ("Stream_MediaFramework_MediaFoundation_MediaSource_T::GetMarshalSizeMax"));
//
//  ACE_ASSERT (false);
//  ACE_NOTSUP_RETURN (E_FAIL);
//
//  ACE_NOTREACHED (return E_FAIL;)
//}
//template <typename TimePolicyType,
//          typename SessionMessageType,
//          typename ProtocolMessageType,
//          typename ConfigurationType,
//          typename MediaType>
//HRESULT
//Stream_MediaFramework_MediaFoundation_MediaSource_T<TimePolicyType,
//                                                    SessionMessageType,
//                                                    ProtocolMessageType,
//                                                    ConfigurationType,
//                                                    MediaType>::MarshalInterface (IStream *pStm,
//                                                                                  REFIID riid,
//                                                                                  void *pv,
//                                                                                  DWORD dwDestContext,
//                                                                                  void *pvDestContext,
//                                                                                  DWORD mshlflags)
//{
//  STREAM_TRACE (ACE_TEXT ("Stream_MediaFramework_MediaFoundation_MediaSource_T::MarshalInterface"));
//
//  ACE_ASSERT (false);
//  ACE_NOTSUP_RETURN (E_FAIL);
//
//  ACE_NOTREACHED (return E_FAIL;)
//}
//template <typename TimePolicyType,
//          typename SessionMessageType,
//          typename ProtocolMessageType,
//          typename ConfigurationType,
//          typename MediaType>
//HRESULT
//Stream_MediaFramework_MediaFoundation_MediaSource_T<TimePolicyType,
//                                                    SessionMessageType,
//                                                    ProtocolMessageType,
//                                                    ConfigurationType,
//                                                    MediaType>::UnmarshalInterface (IStream *pStm,
//                                                                                    REFIID riid,
//                                                                                    void **ppv)
//{
//  STREAM_TRACE (ACE_TEXT ("Stream_MediaFramework_MediaFoundation_MediaSource_T::UnmarshalInterface"));
//
//  ACE_ASSERT (false);
//  ACE_NOTSUP_RETURN (E_FAIL);
//
//  ACE_NOTREACHED (return E_FAIL;)
//}
//template <typename TimePolicyType,
//          typename SessionMessageType,
//          typename ProtocolMessageType,
//          typename ConfigurationType,
//          typename MediaType>
//HRESULT
//Stream_MediaFramework_MediaFoundation_MediaSource_T<TimePolicyType,
//                                                    SessionMessageType,
//                                                    ProtocolMessageType,
//                                                    ConfigurationType,
//                                                    MediaType>::ReleaseMarshalData (IStream *pStm)
//{
//  STREAM_TRACE (ACE_TEXT ("Stream_MediaFramework_MediaFoundation_MediaSource_T::ReleaseMarshalData"));
//
//  ACE_ASSERT (false);
//  ACE_NOTSUP_RETURN (E_FAIL);
//
//  ACE_NOTREACHED (return E_FAIL;)
//}
//template <typename TimePolicyType,
//          typename SessionMessageType,
//          typename ProtocolMessageType,
//          typename ConfigurationType,
//          typename MediaType>
//HRESULT
//Stream_MediaFramework_MediaFoundation_MediaSource_T<TimePolicyType,
//                                                    SessionMessageType,
//                                                    ProtocolMessageType,
//                                                    ConfigurationType,
//                                                    MediaType>::DisconnectObject (DWORD dwReserved)
//{
//  STREAM_TRACE (ACE_TEXT ("Stream_MediaFramework_MediaFoundation_MediaSource_T::DisconnectObject"));
//
//  ACE_ASSERT (false);
//  ACE_NOTSUP_RETURN (E_FAIL);
//
//  ACE_NOTREACHED (return E_FAIL;)
//}

template <typename TimePolicyType,
          typename SessionMessageType,
          typename ProtocolMessageType,
          typename ConfigurationType,
          typename MediaType>
bool
Stream_MediaFramework_MediaFoundation_MediaSource_T<TimePolicyType,
                                                    SessionMessageType,
                                                    ProtocolMessageType,
                                                    ConfigurationType,
                                                    MediaType>::initialize (const ConfigurationType& configuration_in)
{
  STREAM_TRACE (ACE_TEXT ("Stream_MediaFramework_MediaFoundation_MediaSource_T::initialize"));

  configuration_ = &const_cast<ConfigurationType&> (configuration_in);

  return true;
}

template <typename TimePolicyType,
          typename SessionMessageType,
          typename ProtocolMessageType,
          typename ConfigurationType,
          typename MediaType>
void
Stream_MediaFramework_MediaFoundation_MediaSource_T<TimePolicyType,
                                                    SessionMessageType,
                                                    ProtocolMessageType,
                                                    ConfigurationType,
                                                    MediaType>::operator delete (void* pointer_in)
{
  STREAM_TRACE (ACE_TEXT ("Stream_MediaFramework_MediaFoundation_MediaSource_T::operator delete"));

  //// *NOTE*: when used as a regular C++ library (template) class, applications
  ////         instantiate filter objects through the default ctor. In this case,
  ////         class instances behave as standard COM objects, and must therefore
  ////         implement the IUnknown reference-counting mechanism to avoid memory
  ////         leakage.
  //if (hasCOMReference_)
  //{
  //  ULONG reference_count = Release ();
  //  return; // dtor has been invoked --> done
  //} // end IF

  // *NOTE*: when applications instantiate filter (COM) objects from DLLs, that
  //         filter instance may be allocated in a separate heap (this depends
  //         on the C runtime version (and, apparently, also type, i.e. static/
  //         dynamic) that was compiled into(/with ? ...) the DLL) and needs to
  //         be deallocated 'from' the same heap; i.e. the global 'delete'
  //         operator may (see above) fail in this particular scenario (
  //         _CrtIsValidHeapPointer() assertion), which is a known and long-
  //         standing issue. *TODO*: does this affect _DEBUG builds only ?
  //         --> overload the delete operator and forward the instance handle to
  //             a static function 'inside' (see 'translation/compilation units'
  //             and/or scope/namespace issues on how to address the 'global
  //             delete' operator) the DLL
  //         This implementation also handles the scenario where filter
  //         instances are allocated from 'plugin' DLLs that can be loaded/
  //         unloaded at runtime
  //OWN_TYPE_T::DeleteInstance (pointer_in);
}
//template <typename TimePolicyType,
//          typename SessionMessageType,
//          typename ProtocolMessageType,
//          typename MediaType,
//          typename ModuleType>
//void
//Stream_MediaFramework_MediaFoundation_MediaSource_T<TimePolicyType,
//                                       SessionMessageType,
//                                       ProtocolMessageType,
//                                       MediaType,
//                                       ModuleType>::operator delete (void* pointer_in,
//                                                                     size_t bytes_in)
//{
//  STREAM_TRACE (ACE_TEXT ("Stream_MediaFramework_MediaFoundation_MediaSource_T::operator delete"));
//
//  ACE_UNUSED_ARG (bytes_in);
//
//  // *NOTE*: see above
//  OWN_TYPE_T::DeleteInstance (pointer_in);
//}
