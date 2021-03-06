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

#include <amvideo.h>
#include <vfwmsgs.h>

#include "ace/Log_Msg.h"
#include "ace/Message_Block.h"
#include "ace/OS_Memory.h"

#include "common_tools.h"

#include "stream_macros.h"

#include "stream_lib_common.h"
#include "stream_lib_defines.h"
#include "stream_lib_directshow_tools.h"
#include "stream_lib_guids.h"

template <typename TimePolicyType,
          typename SessionMessageType,
          typename ProtocolMessageType,
          typename ConfigurationType,
          typename PinConfigurationType,
          typename MediaType>
CUnknown* WINAPI
Stream_MediaFramework_DirectShow_Source_Filter_T<TimePolicyType,
                                                 SessionMessageType,
                                                 ProtocolMessageType,
                                                 ConfigurationType,
                                                 PinConfigurationType,
                                                 MediaType>::CreateInstance (LPUNKNOWN IUnknown_in,
                                                                             HRESULT* result_out)
{
  STREAM_TRACE (ACE_TEXT ("Stream_MediaFramework_DirectShow_Source_Filter_T::CreateInstance"));

  // sanity check(s)
  ACE_ASSERT (result_out);

  // initialize return value(s)
  *result_out = S_OK;

  CUnknown* unknown_p = NULL;
  ACE_NEW_NORETURN (unknown_p,
                    OWN_TYPE_T (NAME (STREAM_LIB_DIRECTSHOW_FILTER_NAME_SOURCE),
                                IUnknown_in,
                                CLSID_ACEStream_MediaFramework_Source_Filter,
                                result_out));
  if (!unknown_p)
  {
    ACE_DEBUG ((LM_CRITICAL,
                ACE_TEXT ("failed to allocate memory: \"%m\", aborting\n")));
    *result_out = E_OUTOFMEMORY;
  } // end IF

  return unknown_p;
} // CreateInstance
template <typename TimePolicyType,
          typename SessionMessageType,
          typename ProtocolMessageType,
          typename ConfigurationType,
          typename PinConfigurationType,
          typename MediaType>
void WINAPI
Stream_MediaFramework_DirectShow_Source_Filter_T<TimePolicyType,
                                                 SessionMessageType,
                                                 ProtocolMessageType,
                                                 ConfigurationType,
                                                 PinConfigurationType,
                                                 MediaType>::DeleteInstance (void* pointer_in)
{
  STREAM_TRACE (ACE_TEXT ("Stream_MediaFramework_DirectShow_Source_Filter_T::DeleteInstance"));

  // sanity check(s)
  ACE_ASSERT (pointer_in);

  //CUnknown* unknown_p = reinterpret_cast<CUnknown*> (pointer_in);
  //OWN_TYPE_T* instance_p = dynamic_cast<OWN_TYPE_T*> (unknown_p);
  //ACE_ASSERT (instance_p);
  OWN_TYPE_T* instance_p = static_cast<OWN_TYPE_T*> (pointer_in);

  ::delete instance_p;
} // DeleteInstance

template <typename TimePolicyType,
          typename SessionMessageType,
          typename ProtocolMessageType,
          typename ConfigurationType,
          typename PinConfigurationType,
          typename MediaType>
void
Stream_MediaFramework_DirectShow_Source_Filter_T<TimePolicyType,
                                                 SessionMessageType,
                                                 ProtocolMessageType,
                                                 ConfigurationType,
                                                 PinConfigurationType,
                                                 MediaType>::operator delete (void* pointer_in)
{
  STREAM_TRACE (ACE_TEXT ("Stream_MediaFramework_DirectShow_Source_Filter_T::operator delete"));

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
  OWN_TYPE_T::DeleteInstance (pointer_in);
}
//template <typename TimePolicyType,
//          typename SessionMessageType,
//          typename ProtocolMessageType,
//          typename MediaType,
//          typename ModuleType>
//void
//Stream_MediaFramework_DirectShow_Source_Filter_T<TimePolicyType,
//                                       SessionMessageType,
//                                       ProtocolMessageType,
//                                       MediaType,
//                                       ModuleType>::operator delete (void* pointer_in,
//                                                                     size_t bytes_in)
//{
//  STREAM_TRACE (ACE_TEXT ("Stream_MediaFramework_DirectShow_Source_Filter_T::operator delete"));
//
//  ACE_UNUSED_ARG (bytes_in);
//
//  // *NOTE*: see above
//  OWN_TYPE_T::DeleteInstance (pointer_in);
//}

template <typename TimePolicyType,
          typename SessionMessageType,
          typename ProtocolMessageType,
          typename ConfigurationType,
          typename PinConfigurationType,
          typename MediaType>
Stream_MediaFramework_DirectShow_Source_Filter_T<TimePolicyType,
                                                 SessionMessageType,
                                                 ProtocolMessageType,
                                                 ConfigurationType,
                                                 PinConfigurationType,
                                                 MediaType>::Stream_MediaFramework_DirectShow_Source_Filter_T ()
 : inherited (NAME (STREAM_LIB_DIRECTSHOW_FILTER_NAME_SOURCE), // name
              NULL,                                            // owner
              CLSID_ACEStream_MediaFramework_Source_Filter,    // CLSID
              NULL)                                            // result
 , filterConfiguration_ (NULL)
 , allocator_ (NULL)
 , allocatorProperties_ ()
//, hasCOMReference_ (false)
{
  STREAM_TRACE (ACE_TEXT ("Stream_MediaFramework_DirectShow_Source_Filter_T::Stream_MediaFramework_DirectShow_Source_Filter_T"));

  // initialize reference count
  inherited::m_cRef = 1;

  // *NOTE*: the pin will inherited::AddPin() itself to 'this'
  OUTPUT_PIN_T* pin_p = NULL;
  HRESULT result = E_FAIL;
  ACE_NEW_NORETURN (pin_p,
                    OUTPUT_PIN_T (&result,
                                  this,
                                  STREAM_LIB_DIRECTSHOW_FILTER_PIN_OUTPUT_NAME));
  if (!pin_p ||
      FAILED (result))
  {
    ACE_DEBUG ((LM_CRITICAL,
                ACE_TEXT ("%s: failed to allocate memory: \"%m\", aborting\n"),
                ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::name (this).c_str ())));
    return;
  } // end IF
  //ACE_DEBUG ((LM_DEBUG,
  //            ACE_TEXT ("%s: added output pin \"%s\"\n"),
  //            ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::name (this).c_str ()),
  //            ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::name (pin_p).c_str ())));

  // *TODO*: IMemAllocator::SetProperties returns VFW_E_BADALIGN (0x8004020e)
  //         if this is -1/0 (why ?)
  //allocatorProperties_.cbAlign = -1;  // <-- use default
  allocatorProperties_.cbAlign = 1;
  allocatorProperties_.cbBuffer = -1; // <-- use default
  allocatorProperties_.cbPrefix = -1; // <-- use default
  allocatorProperties_.cBuffers =
    STREAM_LIB_DIRECTSHOW_FILTER_SOURCE_BUFFERS;
  //allocatorProperties_.cBuffers = -1; // <-- use default
}
template <typename TimePolicyType,
          typename SessionMessageType,
          typename ProtocolMessageType,
          typename ConfigurationType,
          typename PinConfigurationType,
          typename MediaType>
Stream_MediaFramework_DirectShow_Source_Filter_T<TimePolicyType,
                                                 SessionMessageType,
                                                 ProtocolMessageType,
                                                 ConfigurationType,
                                                 PinConfigurationType,
                                                 MediaType>::Stream_MediaFramework_DirectShow_Source_Filter_T (LPTSTR name_in,
                                                                                                               LPUNKNOWN owner_in,
                                                                                                               REFGUID CLSID_in,
                                                                                                               HRESULT* result_out)
 : inherited (name_in,
              owner_in,
              CLSID_in,
              result_out)
 , filterConfiguration_ (NULL)
 , allocator_ (NULL)
 , allocatorProperties_ ()
//, hasCOMReference_ (false)
{
  STREAM_TRACE (ACE_TEXT ("Stream_MediaFramework_DirectShow_Source_Filter_T::Stream_MediaFramework_DirectShow_Source_Filter_T"));

  // initialize reference count
  inherited::m_cRef = 1;

  // *NOTE*: the pin will inherited::AddPin() itself to 'this'
  OUTPUT_PIN_T* pin_p = NULL;
  ACE_NEW_NORETURN (pin_p,
                    OUTPUT_PIN_T (result_out,
                                  this,
                                  STREAM_LIB_DIRECTSHOW_FILTER_PIN_OUTPUT_NAME));
  if (!pin_p ||
      (result_out && FAILED (*result_out)))
  {
    ACE_DEBUG ((LM_CRITICAL,
                ACE_TEXT ("%s: failed to allocate memory: \"%m\", aborting\n"),
                ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::name (this).c_str ())));
    if (result_out)
      *result_out = E_OUTOFMEMORY;
    return;
  } // end IF
  //ACE_DEBUG ((LM_DEBUG,
  //            ACE_TEXT ("%s: added output pin \"%s\"\n"),
  //            ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::name (this).c_str ()),
  //            ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::name (pin_p).c_str ())));

  // *TODO*: IMemAllocator::SetProperties returns VFW_E_BADALIGN (0x8004020e)
  //         if this is -1/0 (why ?)
  //allocatorProperties_.cbAlign = -1;  // <-- use default
  allocatorProperties_.cbAlign = 1;
  allocatorProperties_.cbBuffer = -1; // <-- use default
  allocatorProperties_.cbPrefix = -1; // <-- use default
  allocatorProperties_.cBuffers =
    STREAM_LIB_DIRECTSHOW_FILTER_SOURCE_BUFFERS;
  //allocatorProperties_.cBuffers = -1; // <-- use default
}
template <typename TimePolicyType,
          typename SessionMessageType,
          typename ProtocolMessageType,
          typename ConfigurationType,
          typename PinConfigurationType,
          typename MediaType>
Stream_MediaFramework_DirectShow_Source_Filter_T<TimePolicyType,
                                                 SessionMessageType,
                                                 ProtocolMessageType,
                                                 ConfigurationType,
                                                 PinConfigurationType,
                                                 MediaType>::~Stream_MediaFramework_DirectShow_Source_Filter_T ()
{
  STREAM_TRACE (ACE_TEXT ("Stream_MediaFramework_DirectShow_Source_Filter_T::~Stream_MediaFramework_DirectShow_Source_Filter_T"));

  struct _FilterInfo filter_info;
  ACE_OS::memset (&filter_info, 0, sizeof (struct _FilterInfo));
  HRESULT result = inherited::QueryFilterInfo (&filter_info);
  if (FAILED (result))
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: failed to IBaseFilter::QueryFilterInfo(): \"%s\", continuing\n"),
                ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::name (this).c_str ()),
                ACE_TEXT (Common_Error_Tools::errorToString (result, true).c_str ())));

  // step1: disconnect from graph
  if (!Stream_MediaFramework_DirectShow_Tools::disconnect (this))
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: failed to Stream_MediaFramework_DirectShow_Tools::disconnect(), continuing\n"),
                ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::name (this).c_str ())));

  // step2: remove from graph ?
  if (filter_info.pGraph)
  {
    result = filter_info.pGraph->RemoveFilter (this);
    if (FAILED (result))
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("%s: failed to IFilterGraph::RemoveFilter(%s): \"%s\", continuing\n"),
                  ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::name (this).c_str ()),
                  ACE_TEXT_WCHAR_TO_TCHAR (filter_info.achName),
                  ACE_TEXT (Common_Error_Tools::errorToString (result, true).c_str ())));
    filter_info.pGraph->Release ();
  } // end IF
}

template <typename TimePolicyType,
          typename SessionMessageType,
          typename ProtocolMessageType,
          typename ConfigurationType,
          typename PinConfigurationType,
          typename MediaType>
HRESULT
Stream_MediaFramework_DirectShow_Source_Filter_T<TimePolicyType,
                                                 SessionMessageType,
                                                 ProtocolMessageType,
                                                 ConfigurationType,
                                                 PinConfigurationType,
                                                 MediaType>::NonDelegatingQueryInterface (REFIID riid_in,
                                                                                          void** interface_out)
{
  STREAM_TRACE (ACE_TEXT ("Stream_MediaFramework_DirectShow_Source_Filter_T::NonDelegatingQueryInterface"));

  // sanity check(s)
  CheckPointer (interface_out, E_POINTER);

  if (InlineIsEqualGUID (riid_in, IID_IMemAllocator))
    return GetInterface ((IMemAllocator*)this, interface_out);

  return inherited::NonDelegatingQueryInterface (riid_in, interface_out);
}

//template <typename TimePolicyType,
//          typename SessionMessageType,
//          typename ProtocolMessageType,
//          typename ConfigurationType,
//          typename PinConfigurationType,
//          typename MediaType>
//CBasePin*
//Stream_MediaFramework_DirectShow_Source_Filter_T<TimePolicyType,
//                                                 SessionMessageType,
//                                                 ProtocolMessageType,
//                                                 ConfigurationType,
//                                                 PinConfigurationType,
//                                                 MediaType>::GetPin (int pin_in)
//{
//  STREAM_TRACE (ACE_TEXT ("Stream_MediaFramework_DirectShow_Source_Filter_T::GetPin"));
//
//  ACE_UNUSED_ARG (pin_in);
//
//  CBasePin* result_p = NULL;
//
//  IPin* ipin_p = Stream_MediaFramework_DirectShow_Tools::pin (this,
//                                                              PINDIR_OUTPUT);
//  if (!ipin_p)
//  {
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("%s has no output pin, aborting\n"),
//                ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::name (this).c_str ())));
//    return NULL;
//  } // end IF
//  result_p = dynamic_cast<CBasePin*> (ipin_p);
//  ACE_ASSERT (result_p);
//
//  return result_p;
//}

//template <typename TimePolicyType,
//          typename SessionMessageType,
//          typename ProtocolMessageType,
//          typename ConfigurationType,
//          typename PinConfigurationType,
//          typename MediaType>
//HRESULT
//Stream_MediaFramework_DirectShow_Source_Filter_T<TimePolicyType,
//                                                 SessionMessageType,
//                                                 ProtocolMessageType,
//                                                 ConfigurationType,
//                                                 PinConfigurationType,
//                                                 MediaType>::AddPin (CBasePin* pin_in)
//{
//  STREAM_TRACE (ACE_TEXT ("Stream_MediaFramework_DirectShow_Source_Filter_T::AddPin"));
//
//  CAutoLock cAutoLock (&lock_);
//
//  /*  Allocate space for this pin and the old ones */
//  CBasePin** pins_p = NULL;
//  ACE_NEW_NORETURN (pins_p,
//                    CBasePin*[1]);
//  if (!pins_p)
//  {
//    ACE_DEBUG ((LM_CRITICAL,
//                ACE_TEXT ("%s: failed to allocate memory: \"%m\", returning\n"),
//                ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::name (this).c_str ())));
//    return E_OUTOFMEMORY;
//  } // end IF
//  if (pins_)
//  {
//    CopyMemory ((PVOID)pins_p, (PVOID)pins_,
//                1 * sizeof (pins_[0]));
//    pins_p[1] = pin_in;
//    delete [] pins_; pins_ = NULL;
//  } // end IF
//  pins_ = pins_p;
//  pins_[1] = pin_in;
//  numberOfPins_++;
//
//  return S_OK;
//}
//template <typename TimePolicyType,
//          typename SessionMessageType,
//          typename ProtocolMessageType,
//          typename ConfigurationType,
//          typename PinConfigurationType,
//          typename MediaType>
//HRESULT
//Stream_MediaFramework_DirectShow_Source_Filter_T<TimePolicyType,
//                                                 SessionMessageType,
//                                                 ProtocolMessageType,
//                                                 ConfigurationType,
//                                                 PinConfigurationType,
//                                                 MediaType>::RemovePin (CBasePin* pin_in)
//{
//  STREAM_TRACE (ACE_TEXT ("Stream_MediaFramework_DirectShow_Source_Filter_T::RemovePin"));
//
//  for (int i = 0; i < numberOfPins_; i++)
//  {
//    if (pins_[i] == pin_in)
//    {
//      if (numberOfPins_ == 1)
//      {
//        delete [] pins_; pins_ = NULL;
//      }
//      else
//      {
//        /*  no need to reallocate */
//        while (++i < numberOfPins_)
//          pins_[i - 1] = pins_[i];
//      } // end ELSE
//      numberOfPins_--;
//      return S_OK;
//    } // end IF
//  } // end FOR
//
//  return S_FALSE;
//}

template <typename TimePolicyType,
          typename SessionMessageType,
          typename ProtocolMessageType,
          typename ConfigurationType,
          typename PinConfigurationType,
          typename MediaType>
HRESULT
Stream_MediaFramework_DirectShow_Source_Filter_T<TimePolicyType,
                                                 SessionMessageType,
                                                 ProtocolMessageType,
                                                 ConfigurationType,
                                                 PinConfigurationType,
                                                 MediaType>::SetProperties (struct _AllocatorProperties* requestedProperties_in,
                                                                            struct _AllocatorProperties* properties_out)
{
  STREAM_TRACE (ACE_TEXT ("Stream_MediaFramework_DirectShow_Source_Filter_T::SetProperties"));

  // sanity check(s)
  CheckPointer (requestedProperties_in, E_POINTER);
  CheckPointer (properties_out, E_POINTER);

  // step1: find 'least common denominator'
  properties_out->cBuffers =
    std::min (requestedProperties_in->cBuffers, allocatorProperties_.cBuffers);
  properties_out->cbBuffer =
    std::min (requestedProperties_in->cbBuffer, allocatorProperties_.cbBuffer);
  // *TODO*: cannot align buffers at this time
  ACE_ASSERT (requestedProperties_in->cbAlign <= 1);
  properties_out->cbPrefix =
    std::min (requestedProperties_in->cbPrefix, allocatorProperties_.cbPrefix);

  // step2: validate against set properties
  // *TODO*

  return NOERROR;
}
template <typename TimePolicyType,
          typename SessionMessageType,
          typename ProtocolMessageType,
          typename ConfigurationType,
          typename PinConfigurationType,
          typename MediaType>
HRESULT
Stream_MediaFramework_DirectShow_Source_Filter_T<TimePolicyType,
                                                 SessionMessageType,
                                                 ProtocolMessageType,
                                                 ConfigurationType,
                                                 PinConfigurationType,
                                                 MediaType>::GetBuffer (IMediaSample** mediaSample_out,
                                                                        REFERENCE_TIME* startTime_out,
                                                                        REFERENCE_TIME* endTime_out,
                                                                        DWORD flags_in)
{
  STREAM_TRACE (ACE_TEXT ("Stream_MediaFramework_DirectShow_Source_Filter_T::GetBuffer"));

  ACE_UNUSED_ARG (flags_in);

  // sanity check(s)
  CheckPointer (mediaSample_out, E_POINTER);
  //CheckPointer (startTime_out, E_POINTER);
  //CheckPointer (endTime_out, E_POINTER);
  ACE_ASSERT (allocatorProperties_.cBuffers && allocatorProperties_.cbBuffer);

  // initialize return value(s)
  *mediaSample_out = NULL;
  //*startTime_out = 0;
  //*endTime_out = 0;

  // step1: allocate message
  unsigned int message_size = (allocatorProperties_.cbPrefix +
                               allocatorProperties_.cbBuffer);
  ProtocolMessageType* message_p = NULL;
  if (allocator_)
  {
allocate:
    try {
      message_p =
        static_cast<ProtocolMessageType*> (allocator_->malloc (message_size));
    } catch (...) {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("%s: caught exception in Stream_IAllocator::malloc(%u), aborting\n"),
                  ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::name (this).c_str ()),
                  message_size));
      return E_OUTOFMEMORY;
    }

    // keep retrying ?
    if (!message_p && !allocator_->block ())
      goto allocate;
  } // end IF
  else
    ACE_NEW_NORETURN (message_p,
                      ProtocolMessageType (message_size));
  if (!message_p)
  {
    if (allocator_)
    {
      if (allocator_->block ())
        ACE_DEBUG ((LM_CRITICAL,
                    ACE_TEXT ("%s: failed to allocate data message: \"%m\", aborting\n"),
                    ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::name (this).c_str ())));
    } // end IF
    else
      ACE_DEBUG ((LM_CRITICAL,
                  ACE_TEXT ("%s: failed to allocate data message: \"%m\", aborting\n"),
                  ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::name (this).c_str ())));
    return E_OUTOFMEMORY;
  } // end IF
  *mediaSample_out = dynamic_cast<IMediaSample*> (message_p);
  if (!*mediaSample_out)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: failed to dynamic_cast<IMediaSample*>(%@), aborting\n"),
                ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::name (this).c_str ()),
                message_p));
    message_p->release ();
    return E_FAIL;
  } // end IF

  // step2: set times
  // *TODO*: calculate times from the average sample rate and the reference
  //         start clock

  return NOERROR;
}
template <typename TimePolicyType,
          typename SessionMessageType,
          typename ProtocolMessageType,
          typename ConfigurationType,
          typename PinConfigurationType,
          typename MediaType>
HRESULT
Stream_MediaFramework_DirectShow_Source_Filter_T<TimePolicyType,
                                                 SessionMessageType,
                                                 ProtocolMessageType,
                                                 ConfigurationType,
                                                 PinConfigurationType,
                                                 MediaType>::ReleaseBuffer (IMediaSample* mediaSample_in)
{
  STREAM_TRACE (ACE_TEXT ("Stream_MediaFramework_DirectShow_Source_Filter_T::ReleaseBuffer"));

  // sanity check(s)
  CheckPointer (mediaSample_in, E_POINTER);

  ACE_Message_Block* message_p =
    dynamic_cast<ACE_Message_Block*> (mediaSample_in);
  if (message_p)
    message_p->release ();
  else
    mediaSample_in->Release ();

  return NOERROR;
}

template <typename TimePolicyType,
          typename SessionMessageType,
          typename ProtocolMessageType,
          typename ConfigurationType,
          typename PinConfigurationType,
          typename MediaType>
bool
Stream_MediaFramework_DirectShow_Source_Filter_T<TimePolicyType,
                                                 SessionMessageType,
                                                 ProtocolMessageType,
                                                 ConfigurationType,
                                                 PinConfigurationType,
                                                 MediaType>::initialize (const ConfigurationType& configuration_in)
{
  STREAM_TRACE (ACE_TEXT ("Stream_MediaFramework_DirectShow_Source_Filter_T::initialize"));

  // sanity check(s)
  ACE_ASSERT (configuration_in.pinConfiguration);

  IPin* ipin_p = Stream_MediaFramework_DirectShow_Tools::pin (this,
                                                             PINDIR_OUTPUT);
  if (!ipin_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s has no output pin, aborting\n"),
                ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::name (this).c_str ())));
    goto error;
  } // end IF

  IPIN_INITIALIZE_T* iinitialize_p = dynamic_cast<IPIN_INITIALIZE_T*> (ipin_p);
  if (!iinitialize_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s/%s: failed to dynamic_cast<Common_IInitialize_T*> (0x%@), aborting\n"),
                ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::name (this).c_str ()),
                ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::name (ipin_p).c_str ()),
                ipin_p));
    goto error;
  } // end IF
  // *TODO*: remove type inference
  if (!iinitialize_p->initialize (*configuration_in.pinConfiguration))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: failed to Stream_MediaFramework_DirectShow_Source_Filter_OutputPin_T::initialize(), aborting\n"),
                ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::name (this).c_str ())));
    goto error;
  } // end IF

  IPIN_MEDIA_INITIALIZE_T* iinitialize_2 =
    dynamic_cast<IPIN_MEDIA_INITIALIZE_T*> (ipin_p);
  if (!iinitialize_2)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s/%s: failed to dynamic_cast<Common_IInitialize_T*> (0x%@), aborting\n"),
                ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::name (this).c_str ()),
                ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::name (ipin_p).c_str ()),
                ipin_p));
    goto error;
  } // end IF
  // *TODO*: remove type inference
  if (!iinitialize_2->initialize (configuration_in.pinConfiguration->format))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: failed to Stream_MediaFramework_DirectShow_Source_Filter_OutputPin_T::initialize(), aborting\n"),
                ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::name (this).c_str ())));
    goto error;
  } // end IF
  ipin_p->Release (); ipin_p = NULL;

  filterConfiguration_ = &const_cast<ConfigurationType&> (configuration_in);
  allocator_ = configuration_in.allocator;
  allocatorProperties_ = configuration_in.allocatorProperties;

  return true;

error:
  if (ipin_p)
    ipin_p->Release ();

  return false;
}
template <typename TimePolicyType,
          typename SessionMessageType,
          typename ProtocolMessageType,
          typename ConfigurationType,
          typename PinConfigurationType,
          typename MediaType>
bool
Stream_MediaFramework_DirectShow_Source_Filter_T<TimePolicyType,
                                                 SessionMessageType,
                                                 ProtocolMessageType,
                                                 ConfigurationType,
                                                 PinConfigurationType,
                                                 MediaType>::initialize (const MediaType* mediaType_in)
{
  STREAM_TRACE (ACE_TEXT ("Stream_MediaFramework_DirectShow_Source_Filter_T::initialize"));

  // sanity check(s)
  ACE_ASSERT (mediaType_in);

  // *NOTE*: the pin will inherited::AddPin() itself to 'this'
  IPin* ipin_p = Stream_MediaFramework_DirectShow_Tools::pin (this,
                                                              PINDIR_OUTPUT);
  if (!ipin_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s has no output pin, aborting\n"),
                ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::name (this).c_str ())));
    goto error;
  } // end IF

  IPIN_MEDIA_INITIALIZE_T* iinitialize_p =
    dynamic_cast<IPIN_MEDIA_INITIALIZE_T*> (ipin_p);
  if (!iinitialize_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s/%s: failed to dynamic_cast<Common_IInitialize_T*>(%@), aborting\n"),
                ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::name (this).c_str ()),
                ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::name (ipin_p).c_str ()),
                ipin_p));
    goto error;
  } // end IF
  ipin_p->Release (); ipin_p = NULL;

  return iinitialize_p->initialize (*mediaType_in);

error:
  if (ipin_p)
    ipin_p->Release ();

  return false;
}

////////////////////////////////////////////////////////////////////////////////

template <typename ConfigurationType,
          typename FilterType,
          typename MediaType>
Stream_MediaFramework_DirectShow_Source_Filter_OutputPin_T<ConfigurationType,
                                                           FilterType,
                                                           MediaType>::Stream_MediaFramework_DirectShow_Source_Filter_OutputPin_T (HRESULT* result_out,
                                                                                                                                   CSource* parentFilter_in,
                                                                                                                                   LPCWSTR pinName_in)
#if defined (UNICODE)
 : inherited (pinName_in,
#else
 : inherited (ACE_TEXT_ALWAYS_CHAR (ACE_TEXT_WCHAR_TO_TCHAR (pinName_in)),
#endif // UNICODE
              result_out,
              parentFilter_in,
              pinName_in)
 , configuration_ (NULL)
 , isInitialized_ (false)
 , mediaType_ (NULL)
 //, parentFilter_ (dynamic_cast<FilterType*> (parentFilter_in))
 , queue_ (NULL)
 /////////////////////////////////////////
 , defaultFrameInterval_ (STREAM_LIB_DIRECTSHOW_FILTER_SOURCE_FRAME_INTERVAL)
 , frameInterval_ (0)
 , hasMediaSampleBuffers_ (false)
 , isTopToBottom_ (false)
 , numberOfMediaTypes_ (1)
 , directShowHasEnded_ (false)
 , lock_ ()
 , sampleTime_ ()
{
  STREAM_TRACE (ACE_TEXT ("Stream_MediaFramework_DirectShow_Source_Filter_OutputPin_T::Stream_MediaFramework_DirectShow_Source_Filter_OutputPin_T"));

  // sanity check(s)
  ACE_ASSERT (inherited::m_pFilter);
  ACE_ASSERT (result_out);
} // (Constructor)

template <typename ConfigurationType,
          typename FilterType,
          typename MediaType>
Stream_MediaFramework_DirectShow_Source_Filter_OutputPin_T<ConfigurationType,
                                                           FilterType,
                                                           MediaType>::~Stream_MediaFramework_DirectShow_Source_Filter_OutputPin_T ()
{
  STREAM_TRACE (ACE_TEXT ("Stream_MediaFramework_DirectShow_Source_Filter_OutputPin_T::~Stream_MediaFramework_DirectShow_Source_Filter_OutputPin_T"));

  //HRESULT result = E_FAIL;
  //if (allocator_)
  //{
  //  result = allocator_->Decommit ();
  //  if (FAILED (result))
  //    ACE_DEBUG ((LM_ERROR,
  //                ACE_TEXT ("failed to IMemAllocator::Decommit(): \"%s\", continuing\n"),
  //                ACE_TEXT (Common_Error_Tools::errorToString (result).c_str ())));
  //  allocator_->Release ();
  //} // end IF

  if (mediaType_)
    Stream_MediaFramework_DirectShow_Tools::delete_ (mediaType_);
} // (Destructor)

template <typename ConfigurationType,
          typename FilterType,
          typename MediaType>
bool
Stream_MediaFramework_DirectShow_Source_Filter_OutputPin_T<ConfigurationType,
                                                           FilterType,
                                                           MediaType>::initialize (const ConfigurationType& configuration_in)
{
  STREAM_TRACE (ACE_TEXT ("Stream_MediaFramework_DirectShow_Source_Filter_OutputPin_T::initialize"));

  configuration_ = &const_cast<ConfigurationType&> (configuration_in);

  // *TODO*: remove type inferences
  queue_ = configuration_->queue;
  hasMediaSampleBuffers_ = configuration_->hasMediaSampleBuffers;
  isTopToBottom_ = configuration_->isTopToBottom;
  directShowHasEnded_ = false;

  isInitialized_ = true;

  return true;
}
template <typename ConfigurationType,
          typename FilterType,
          typename MediaType>
bool
Stream_MediaFramework_DirectShow_Source_Filter_OutputPin_T<ConfigurationType,
                                                           FilterType,
                                                           MediaType>::initialize (const MediaType& mediaType_in)
{
  STREAM_TRACE (ACE_TEXT ("Stream_MediaFramework_DirectShow_Source_Filter_OutputPin_T::initialize"));

  if (mediaType_)
    Stream_MediaFramework_DirectShow_Tools::delete_ (mediaType_);
  mediaType_ =
    Stream_MediaFramework_DirectShow_Tools::copy (mediaType_in);
  if (!mediaType_)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Stream_MediaFramework_DirectShow_Tools::copy(): \"%m\", aborting\n")));
    return false;
  } // end IF
#if defined (_DEBUG)
  ACE_ASSERT (inherited::m_pFilter);
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("%s/%s: set default output format: %s\n"),
              ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::name (inherited::m_pFilter).c_str ()),
              ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::name (this).c_str ()),
              ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::toString (*mediaType_, true).c_str ())));
#endif // _DEBUG

  return true;
}

// ------------------------------------

template <typename ConfigurationType,
          typename FilterType,
          typename MediaType>
HRESULT
Stream_MediaFramework_DirectShow_Source_Filter_OutputPin_T<ConfigurationType,
                                                           FilterType,
                                                           MediaType>::NonDelegatingQueryInterface (REFIID riid_in,
                                                                                                    void** interface_out)
{
  STREAM_TRACE (ACE_TEXT ("Stream_MediaFramework_DirectShow_Source_Filter_OutputPin_T::NonDelegatingQueryInterface"));

  // sanity check(s)
  CheckPointer (interface_out, E_POINTER);

  if (InlineIsEqualGUID (riid_in, IID_IKsPropertySet))
    return GetInterface ((IKsPropertySet*)this, interface_out);
  else if (InlineIsEqualGUID (riid_in, IID_IAMBufferNegotiation))
    return GetInterface ((IAMBufferNegotiation*)this, interface_out);
  else if (InlineIsEqualGUID (riid_in, IID_IAMStreamConfig))
    return GetInterface ((IAMStreamConfig*)this, interface_out);

  return inherited::NonDelegatingQueryInterface (riid_in, interface_out);
}

// ------------------------------------

//
// CheckMediaType
//
// accept the preconfigured media type, if any
// Returns E_INVALIDARG if the mediatype is not acceptable
//
template <typename ConfigurationType,
          typename FilterType,
          typename MediaType>
HRESULT
Stream_MediaFramework_DirectShow_Source_Filter_OutputPin_T<ConfigurationType,
                                                           FilterType,
                                                           MediaType>::CheckMediaType (const CMediaType *mediaType_in)
{
  STREAM_TRACE (ACE_TEXT ("Stream_MediaFramework_DirectShow_Source_Filter_OutputPin_T::CheckMediaType"));

  // sanity check(s)
  CheckPointer (mediaType_in, E_POINTER);
  //if ((*(mediaType_in->Type ()) != MEDIATYPE_Video) ||
  //    !mediaType_in->IsFixedSize ())
  //  return E_FAIL;
  ACE_ASSERT (mediaType_);
  ACE_ASSERT (inherited::m_pFilter);

  CAutoLock cAutoLock (inherited::m_pFilter->pStateLock ());

  CMediaType media_type;
  HRESULT result = media_type.Set (*mediaType_);
  ACE_ASSERT (SUCCEEDED (result));
  if (!media_type.MatchesPartial (mediaType_in))
  {
    //ACE_DEBUG ((LM_DEBUG,
    //            ACE_TEXT ("%s/%s: incompatible media types (\"%s\"\n\"%s\")\n"),
    //            ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::name (inherited::m_pFilter).c_str ()),
    //            ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::name (this).c_str ())));
    //            ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::toString (*configuration_->format).c_str ()),
    //            ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::toString (*mediaType_in).c_str ())));
    return S_FALSE;
  } // end IF

  return S_OK;
} // CheckMediaType

//
// GetMediaType
//
// Prefered types should be ordered by quality, zero as highest quality
// (iPosition > 4 is invalid)
//
template <typename ConfigurationType,
          typename FilterType,
          typename MediaType>
HRESULT
Stream_MediaFramework_DirectShow_Source_Filter_OutputPin_T<ConfigurationType,
                                                           FilterType,
                                                           MediaType>::GetMediaType (int position_in,
                                                                                     CMediaType* mediaType_out)
{
  STREAM_TRACE (ACE_TEXT ("Stream_MediaFramework_DirectShow_Source_Filter_OutputPin_T::GetMediaType"));

  HRESULT result = E_FAIL;

  if (position_in < 0)
    return E_INVALIDARG;
  // *TODO*: implement a default set of supported media types
  if (static_cast<unsigned int> (position_in) > (numberOfMediaTypes_ - 1))
    return VFW_S_NO_MORE_ITEMS;

  // sanity check(s)
  CheckPointer (mediaType_out, E_POINTER);
  ACE_ASSERT (mediaType_);
  ACE_ASSERT (inherited::m_pFilter);

  CAutoLock cAutoLock (inherited::m_pFilter->pStateLock ());

  result = mediaType_out->Set (*mediaType_);
  if (FAILED (result))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s/%s: failed to CMediaType::Set(): \"%s\", aborting\n"),
                ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::name (inherited::m_pFilter).c_str ()),
                ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::name (this).c_str ()),
                ACE_TEXT (Common_Error_Tools::errorToString (result, true).c_str ())));
    return result;
  } // end IF

  // correct frame orientation ?
  if (!InlineIsEqualGUID (mediaType_out->majortype, MEDIATYPE_Video))
    goto continue_;
  struct tagVIDEOINFOHEADER* video_info_header_p = NULL;
  struct tagVIDEOINFOHEADER2* video_info_header2_p = NULL;
  if (InlineIsEqualGUID (mediaType_out->formattype, FORMAT_VideoInfo))
  { ACE_ASSERT (mediaType_out->cbFormat == sizeof (struct tagVIDEOINFOHEADER));
    video_info_header_p =
      reinterpret_cast<struct tagVIDEOINFOHEADER*> (mediaType_out->pbFormat);
    ACE_ASSERT (video_info_header_p);
    if (isTopToBottom_)
    {
      if (video_info_header_p->bmiHeader.biHeight > 0)
        video_info_header_p->bmiHeader.biHeight =
          -video_info_header_p->bmiHeader.biHeight;
    }
    else if (video_info_header_p->bmiHeader.biHeight < 0)
      video_info_header_p->bmiHeader.biHeight =
        -video_info_header_p->bmiHeader.biHeight;
  } // end IF
  else if (InlineIsEqualGUID (mediaType_out->formattype, FORMAT_VideoInfo2))
  { ACE_ASSERT (mediaType_out->cbFormat == sizeof (struct tagVIDEOINFOHEADER2));
    video_info_header2_p =
      reinterpret_cast<struct tagVIDEOINFOHEADER2*> (mediaType_out->pbFormat);
    ACE_ASSERT (video_info_header2_p);
    if (isTopToBottom_)
    {
      if (video_info_header2_p->bmiHeader.biHeight > 0)
        video_info_header2_p->bmiHeader.biHeight =
          -video_info_header2_p->bmiHeader.biHeight;
    }
    else if (video_info_header2_p->bmiHeader.biHeight < 0)
      video_info_header2_p->bmiHeader.biHeight =
        -video_info_header2_p->bmiHeader.biHeight;
  } // end ELSE IF
  else
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s/%s: invalid/unknown media type format type (was: \"%s\"), aborting\n"),
                ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::name (inherited::m_pFilter).c_str ()),
                ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::name (this).c_str ()),
                ACE_TEXT (Common_Tools::GUIDToString (mediaType_out->formattype).c_str ())));
    return E_FAIL;
  } // end ELSE

continue_:
  return S_OK;
} // GetMediaType

//
// SetMediaType
//
// Called when a media type is agreed between filters
//
template <typename ConfigurationType,
          typename FilterType,
          typename MediaType>
HRESULT
Stream_MediaFramework_DirectShow_Source_Filter_OutputPin_T<ConfigurationType,
                                                           FilterType,
                                                           MediaType>::SetMediaType (const CMediaType* mediaType_in)
{
  STREAM_TRACE (ACE_TEXT ("Stream_MediaFramework_DirectShow_Source_Filter_OutputPin_T::SetMediaType"));

  // sanity check(s)
  CheckPointer (mediaType_in, E_POINTER);
  ACE_ASSERT (inherited::m_pFilter);

  CAutoLock cAutoLock (inherited::m_pFilter->pStateLock ());

  HRESULT result = inherited::SetMediaType (mediaType_in);
  if (FAILED (result))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s/%s: failed to CSourceStream::SetMediaType(): \"%s\", aborting\n"),
                ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::name (inherited::m_pFilter).c_str ()),
                ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::name (this).c_str ()),
                ACE_TEXT (Common_Error_Tools::errorToString (result, true).c_str ())));
    return result;
  } // end IF

  // compute frame interval to correctly set the sample time
  REFERENCE_TIME avg_time_per_frame; // 100ns units
  if (InlineIsEqualGUID (inherited::m_mt.majortype, MEDIATYPE_Video))
  {
    struct tagVIDEOINFOHEADER* video_info_header_p = NULL;
    struct tagVIDEOINFOHEADER2* video_info_header2_p = NULL;
    if (InlineIsEqualGUID (inherited::m_mt.formattype, FORMAT_VideoInfo))
    { ACE_ASSERT (inherited::m_mt.cbFormat == sizeof (struct tagVIDEOINFOHEADER));
      video_info_header_p =
        reinterpret_cast<struct tagVIDEOINFOHEADER*> (inherited::m_mt.pbFormat);
      avg_time_per_frame = video_info_header_p->AvgTimePerFrame;
    } // end IF
    else if (InlineIsEqualGUID (inherited::m_mt.formattype, FORMAT_VideoInfo2))
    { ACE_ASSERT (inherited::m_mt.cbFormat == sizeof (struct tagVIDEOINFOHEADER2));
      video_info_header2_p =
        reinterpret_cast<struct tagVIDEOINFOHEADER2*> (inherited::m_mt.pbFormat);
      avg_time_per_frame = video_info_header2_p->AvgTimePerFrame;
    } // end ELSE IF
    else
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("%s/%s: invalid/unknown media type format type (was: \"%s\"), aborting\n"),
                  ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::name (inherited::m_pFilter).c_str ()),
                  ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::name (this).c_str ()),
                  ACE_TEXT (Common_Tools::GUIDToString (inherited::m_mt.formattype).c_str ())));
      return E_FAIL;
    } // end ELSE
    frameInterval_ = avg_time_per_frame / 10000; // 100ns --> ms
  } // end IF
  else if (InlineIsEqualGUID (inherited::m_mt.majortype, MEDIATYPE_Audio))
  { ACE_ASSERT (inherited::m_mt.cbFormat == sizeof (struct tWAVEFORMATEX));
    struct tWAVEFORMATEX* waveformatex_p =
      reinterpret_cast<struct tWAVEFORMATEX*> (inherited::m_mt.pbFormat);
    frameInterval_ = waveformatex_p->nSamplesPerSec / 1000;
  } // end ELSE IF
  else
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s/%s: invalid/unknown media type major type (was: \"%s\"), aborting\n"),
                ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::name (inherited::m_pFilter).c_str ()),
                ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::name (this).c_str ()),
                ACE_TEXT (Common_Tools::GUIDToString (inherited::m_mt.majortype).c_str ())));
    return E_FAIL;
  } // end ELSE

  // *NOTE*: the 'intelligent' DirectShow graph assembly implementation (i.e.
  //         IGraphBuilder::Connect()) involves media type resolution. In this
  //         case, the media type argument 'format type' field passed in here
  //         can be GUID_NULL, which means 'not specified'.
  //         Given the publicly available documentation, for video types, this
  //         essentially means that FORMAT_VideoInfo and FORMAT_VideoInfo2 must
  //         both be supported. However, support for this feature is 'brittle';
  //         many 'built-in' filter(-pin)s have not been updated and support
  //         only FORMAT_VideoInfo, while more recent filters only support
  //         FORMAT_VideoInfo2, leading to compatibility issues
  // *NOTE*: how this 'feature creep' effectively also breaks support for
  //         vendor-specific filter implementations
#if defined (_DEBUG)
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("%s/%s: set media type: %s\n"),
              ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::name (inherited::m_pFilter).c_str ()),
              ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::name (this).c_str ()),
              ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::toString (*mediaType_in, true).c_str ())));
#endif // _DEBUG

  return S_OK;
} // SetMediaType

template <typename ConfigurationType,
          typename FilterType,
          typename MediaType>
HRESULT
Stream_MediaFramework_DirectShow_Source_Filter_OutputPin_T<ConfigurationType,
                                                           FilterType,
                                                           MediaType>::QueryAccept (const struct _AMMediaType* mediaType_in)
{
  STREAM_TRACE (ACE_TEXT ("Stream_MediaFramework_DirectShow_Source_Filter_OutputPin_T::QueryAccept"));

  // sanity check(s)
  ACE_ASSERT (mediaType_in);

  struct _AMMediaType media_type_s;
  ACE_OS::memset (&media_type_s, 0, sizeof (struct _AMMediaType));
  HRESULT result = inherited::ConnectionMediaType (&media_type_s);
  if (FAILED (result))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s/%s: failed to ConnectionMediaType(): \"%s\", aborting\n"),
                ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::name (inherited::m_pFilter).c_str ()),
                ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::name (this).c_str ()),
                ACE_TEXT (Common_Error_Tools::errorToString (result, true).c_str ())));
    return E_FAIL;
  } // end IF
  if (Stream_MediaFramework_DirectShow_Tools::match (*mediaType_in,
                                                     media_type_s))
    return S_OK;

  ACE_DEBUG ((LM_WARNING,
              ACE_TEXT ("%s/%s: downstream requests a format change (was: %s), rejecting\n"),
              ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::name (inherited::m_pFilter).c_str ()),
              ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::name (this).c_str ()),
              ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::toString (*mediaType_in, true).c_str ())));

  // *NOTE*: "...The upstream filter can reject the format change by returning
  //         S_FALSE from QueryAccept. In that case, the Video Renderer
  //         continues to use GDI with the original format. ..."
  return S_FALSE;
}

template <typename ConfigurationType,
          typename FilterType,
          typename MediaType>
HRESULT
Stream_MediaFramework_DirectShow_Source_Filter_OutputPin_T<ConfigurationType,
                                                           FilterType,
                                                           MediaType>::DecideAllocator (IMemInputPin* inputPin_in,
                                                                                        IMemAllocator** allocator_out)
{
  STREAM_TRACE (ACE_TEXT ("Stream_MediaFramework_DirectShow_Source_Filter_OutputPin_T::DecideAllocator"));

  // sanity check(s)
  CheckPointer (inputPin_in, E_POINTER);
  CheckPointer (allocator_out, E_POINTER);
  ACE_ASSERT (!*allocator_out);
  ACE_ASSERT (!inherited::m_pAllocator);
  //ACE_ASSERT (!allocator_);
  ACE_ASSERT (inherited::m_pFilter);

  HRESULT result = E_FAIL;
  IPin* pin_p = NULL;
  IBaseFilter* filter_p = NULL;
  struct _GUID class_id = GUID_NULL;

  // debug info
  result = inputPin_in->QueryInterface (IID_PPV_ARGS (&pin_p));
  if (FAILED (result))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s/%s: failed to IMemInputPin::QueryInterface(IID_IPin) (handle was: 0x%@): \"%s\", aborting\n"),
                ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::name (inherited::m_pFilter).c_str ()),
                ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::name (this).c_str ()),
                inputPin_in,
                ACE_TEXT (Common_Error_Tools::errorToString (result, true).c_str ())));
    goto error;
  } // end IF
  ACE_ASSERT (pin_p);
  filter_p = Stream_MediaFramework_DirectShow_Tools::toFilter (pin_p);
  ACE_ASSERT (filter_p);

  struct _AllocatorProperties allocator_requirements;
  ACE_OS::memset (&allocator_requirements,
                  0,
                  sizeof (struct _AllocatorProperties));
  result = inputPin_in->GetAllocatorRequirements (&allocator_requirements);
  if (FAILED (result))
  {
    if (result != E_NOTIMPL) // E_NOTIMPL: 0x80004001
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("%s/%s: failed to IMemInputPin::GetAllocatorRequirements() (was: %s/%s): \"%s\", aborting\n"),
                  ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::name (inherited::m_pFilter).c_str ()),
                  ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::name (this).c_str ()),
                  ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::name (filter_p).c_str ()),
                  ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::name (pin_p).c_str ()),
                  ACE_TEXT (Common_Error_Tools::errorToString (result, true).c_str ())));
      goto error;
    } // end IF
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("%s/%s: failed to IMemInputPin::GetAllocatorRequirements() (was: %s/%s): \"%s\", continuing\n"),
                ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::name (inherited::m_pFilter).c_str ()),
                ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::name (this).c_str ()),
                ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::name (filter_p).c_str ()),
                ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::name (pin_p).c_str ()),
                ACE_TEXT (Common_Error_Tools::errorToString (result, true).c_str ())));
  } // end IF
#if defined (_DEBUG)
  else
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("%s/%s: allocator requirements (buffers/size/alignment/prefix) of %s/%s: %d/%d/%d/%d\n"),
                ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::name (inherited::m_pFilter).c_str ()),
                ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::name (this).c_str ()),
                ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::name (filter_p).c_str ()),
                ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::name (pin_p).c_str ()),
                allocator_requirements.cBuffers,
                allocator_requirements.cbBuffer,
                allocator_requirements.cbAlign,
                allocator_requirements.cbPrefix));
#endif // _DEBUG

  // *NOTE*: see also: https://msdn.microsoft.com/en-us/library/windows/desktop/dd319039(v=vs.85).aspx

  //// *TODO*: the Color Space Converter filters' allocator appears to be broken,
  ////         IMemAllocator::SetProperties() does not work and the filter hangs
  ////         in CBaseOutputPin::Deliver()
  ////         --> use our own
  //result = filter_p->GetClassID (&class_id);
  //ACE_ASSERT (SUCCEEDED (result));
  //if (InlineIsEqualGUID (class_id, CLSID_Colour))
  //{
  //  ACE_DEBUG ((LM_DEBUG,
  //              ACE_TEXT ("%s/%s: using own allocator...\n"),
  //              ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::name (inherited::m_pFilter).c_str ()),
  //              ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::name (this).c_str ())));
  //  goto continue_;
  //} // end IF

  // use input pins' allocator ?
  result = inputPin_in->GetAllocator (allocator_out);
  if (SUCCEEDED (result))
  { ACE_ASSERT (*allocator_out);
    //if (inherited::m_pAllocator)
    //  inherited::m_pAllocator->Release ();
    //inherited::m_pAllocator = *allocator_out;
#if defined (_DEBUG)
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("%s/%s: configuring allocator from %s/%s\n"),
                ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::name (inherited::m_pFilter).c_str ()),
                ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::name (this).c_str ()),
                ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::name (filter_p).c_str ()),
                ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::name (pin_p).c_str ())));
#endif // _DEBUG
    goto decide;
  } // end IF

//continue_:
  // the input pin does not supply an allocator
  // --> use our own
  ACE_ASSERT (!*allocator_out);

  // *NOTE*: how this really makes sense for asynchronous filters only
  if (hasMediaSampleBuffers_)
    *allocator_out = dynamic_cast<IMemAllocator*> (inherited::m_pFilter);
  else
  {
    result = inherited::InitAllocator (allocator_out);
    if (FAILED (result))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("%s/%s: failed to CBaseOutputPin::InitAllocator(): \"%s\", aborting\n"),
                  ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::name (inherited::m_pFilter).c_str ()),
                  ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::name (this).c_str ()),
                  ACE_TEXT (Common_Error_Tools::errorToString (result, true).c_str ())));
      goto error;
    } // end IF
  } // end ELSE
  ACE_ASSERT (*allocator_out);

decide:
  result = DecideBufferSize (*allocator_out,
                             &allocator_requirements);
  if (FAILED (result))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s/%s: failed to CBaseOutputPin::DecideBufferSize(): \"%s\", aborting\n"),
                ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::name (inherited::m_pFilter).c_str ()),
                ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::name (this).c_str ()),
                ACE_TEXT (Common_Error_Tools::errorToString (result, true).c_str ())));
    goto error;
  } // end IF

//notify:
  //ACE_ASSERT (inherited::m_pAllocator == *allocator_out);
  ACE_ASSERT (*allocator_out);

  result = inputPin_in->NotifyAllocator (*allocator_out,
                                         FALSE); // read-only buffers ?
  if (FAILED (result))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s/%s: failed to IMemInputPin::NotifyAllocator(): \"%s\", aborting\n"),
                ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::name (filter_p).c_str ()),
                ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::name (pin_p).c_str ()),
                ACE_TEXT (Common_Error_Tools::errorToString (result, true).c_str ())));
    goto error;
  } // end IF
#if defined (_DEBUG)
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("%s/%s: set allocator properties (buffers/size/alignment/prefix): %d/%d/%d/%d\n"),
              ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::name (inherited::m_pFilter).c_str ()),
              ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::name (this).c_str ()),
              allocator_requirements.cBuffers,
              allocator_requirements.cbBuffer,
              allocator_requirements.cbAlign,
              allocator_requirements.cbPrefix));
#endif // _DEBUG

  pin_p->Release (); pin_p = NULL;
  filter_p->Release (); filter_p = NULL;

  return S_OK;

error:
  //if (inherited::m_pAllocator)
  //{
  //  inherited::m_pAllocator->Release (); inherited::m_pAllocator = NULL;
  //} // end IF
  if (pin_p)
    pin_p->Release ();
  if (filter_p)
    filter_p->Release ();

  return result;
}

//
// DecideBufferSize
//
// This will always be called after the format has been sucessfully
// negotiated. So we have a look at m_mt to see what size image we agreed.
// Then we can ask for buffers of the correct size to contain them.
//
template <typename ConfigurationType,
          typename FilterType,
          typename MediaType>
HRESULT
Stream_MediaFramework_DirectShow_Source_Filter_OutputPin_T<ConfigurationType,
                                                           FilterType,
                                                           MediaType>::DecideBufferSize (IMemAllocator* allocator_in,
                                                                                         struct _AllocatorProperties* properties_inout)
{
  STREAM_TRACE (ACE_TEXT ("Stream_MediaFramework_DirectShow_Source_Filter_OutputPin_T::DecideBufferSize"));

  // sanity check(s)
  CheckPointer (allocator_in, E_POINTER);
  CheckPointer (properties_inout, E_POINTER);
  ACE_ASSERT (inherited::m_pFilter);

  CAutoLock cAutoLock (inherited::m_pFilter->pStateLock ());

  struct _AMMediaType media_type;
  ACE_OS::memset (&media_type, 0, sizeof (struct _AMMediaType));
  struct _AllocatorProperties allocator_properties_s;
  ACE_OS::memset (&allocator_properties_s, 0, sizeof (struct _AllocatorProperties));

  HRESULT result = GetAllocatorProperties (&allocator_properties_s);
  ACE_ASSERT (SUCCEEDED (result));

  //result = copy (&media_type, &(inherited::m_mt));
  result = inherited::ConnectionMediaType (&media_type);
  if (FAILED (result))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s/%s: failed to ConnectionMediaType(): \"%s\", aborting\n"),
                ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::name (inherited::m_pFilter).c_str ()),
                ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::name (this).c_str ()),
                ACE_TEXT (Common_Error_Tools::errorToString (result, true).c_str ())));
    return result;
  } // end IF
  ACE_ASSERT (InlineIsEqualGUID (media_type.formattype, FORMAT_VideoInfo) &&
              (media_type.cbFormat == sizeof (struct tagVIDEOINFOHEADER)));
  struct tagVIDEOINFOHEADER* video_info_p =
    reinterpret_cast<struct tagVIDEOINFOHEADER*> (media_type.pbFormat);
  ACE_ASSERT (video_info_p);

  // *TODO*: IMemAllocator::SetProperties returns VFW_E_BADALIGN (0x8004020e)
  //         if this is -1/0 (why ?)
  if (properties_inout->cbAlign <= 0)
    properties_inout->cbAlign = 1;
  // *NOTE*: the buffers must be large enough to support the 32-bit RGB (!) data
  //         format
  //video_info_p->bmiHeader.biBitCount = 32;
  //video_info_p->bmiHeader.biSizeImage = DIBSIZE (video_info_p->bmiHeader);
  //GetBitmapSize (&video_info_p->bmiHeader);
  properties_inout->cbBuffer =
    std::max (static_cast<long> (video_info_p->bmiHeader.biSizeImage),
              allocator_properties_s.cbBuffer);
  ACE_ASSERT (properties_inout->cbBuffer);
  //properties_inout->cbPrefix = 0;
  // *NOTE*: IMemAllocator::SetProperties returns E_INVALIDARG (0x80070057)
  //         if this is set (why ?)
  properties_inout->cBuffers = STREAM_LIB_DIRECTSHOW_FILTER_SOURCE_BUFFERS;

  FreeMediaType (media_type);

  // configure the allocator to reserve sample memory; remember to validate
  // the return value to confirm eligibility of buffer configuration
  // *NOTE*: this function does not actually allocate any memory (see
  //         IMemAllocator::Commit ())
  // *NOTE*: this may invoke QueryAccept() (see above)
  ACE_OS::memset (&allocator_properties_s, 0, sizeof (struct _AllocatorProperties));
  result = allocator_in->SetProperties (properties_inout,
                                        &allocator_properties_s);
  if (FAILED (result)) // VFW_E_TYPE_NOT_ACCEPTED: 0x8004022a
                       // E_INVALIDARG           : 0x80070057
                       
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s/%s: failed to IMemAllocator::SetProperties(): \"%s\" (0x%x), aborting\n"),
                ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::name (inherited::m_pFilter).c_str ()),
                ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::name (this).c_str ()),
                ACE_TEXT (Common_Error_Tools::errorToString (result, true).c_str ()),
                result));
    return result;
  } // end IF

  // --> is this allocator suitable ?
  // *TODO*: this needs more work
  if (allocator_properties_s.cbBuffer < properties_inout->cbBuffer)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s/%s: IMemAllocator::SetProperties() returned %d (expected: %d), aborting\n"),
                ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::name (inherited::m_pFilter).c_str ()),
                ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::name (this).c_str ()),
                allocator_properties_s.cbBuffer, properties_inout->cbBuffer));
    return E_FAIL;
  } // end IF
  ACE_ASSERT (allocator_properties_s.cBuffers >= 1);

  //// (try to) allocate required memory
  //result = allocator_in->Commit ();
  //if (FAILED (result))
  //{
  //  ACE_DEBUG ((LM_ERROR,
  //              ACE_TEXT ("failed to IMemAllocator::Commit(): \"%s\", aborting\n"),
  //              ACE_TEXT (Common_Error_Tools::errorToString (result, true).c_str ())));
  //  return result;
  //} // end IF

  return S_OK;
} // DecideBufferSize

template <typename ConfigurationType,
          typename FilterType,
          typename MediaType>
HRESULT
Stream_MediaFramework_DirectShow_Source_Filter_OutputPin_T<ConfigurationType,
                                                           FilterType,
                                                           MediaType>::FillBuffer (IMediaSample* mediaSample_in)
{
  STREAM_TRACE (ACE_TEXT ("Stream_MediaFramework_DirectShow_Source_Filter_OutputPin_T::FillBuffer"));

  // sanity check(s)
  CheckPointer (mediaSample_in, E_POINTER);
  ACE_ASSERT (inherited::m_pFilter);
  ACE_ASSERT (queue_);

  // done ?
  HRESULT result = E_FAIL;
  enum Command command_e = CMD_INIT;
  if (likely (!directShowHasEnded_))
  {
    if (likely (inherited::CheckRequest (&command_e)))
    {
      if (unlikely (command_e == CMD_STOP))
        directShowHasEnded_ = true; // --> wait for stream
      Reply ((command_e == CMD_STOP) ? S_FALSE : S_OK);
    } // end IF
  } // end IF

  BYTE* data_p = NULL;
  result = mediaSample_in->GetPointer (&data_p);
  if (unlikely (FAILED (result)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s/%s: failed to IMediaSample::GetPointer(): \"%s\", aborting\n"),
                ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::name (inherited::m_pFilter).c_str ()),
                ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::name (this).c_str ()),
                ACE_TEXT (Common_Error_Tools::errorToString (result, true).c_str ())));
    return S_FALSE; // --> stop
  } // end IF
  ACE_ASSERT (data_p);
  long data_length_l = 0;
  data_length_l = mediaSample_in->GetSize ();
  ACE_ASSERT (data_length_l);

  ACE_Message_Block* message_block_p = NULL;
  int result_2 = queue_->dequeue_head (message_block_p, NULL);
  if (unlikely (result_2 == -1))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s/%s: failed to ACE_Message_Queue_Base::dequeue_head(): \"%m\", aborting\n"),
                ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::name (inherited::m_pFilter).c_str ()),
                ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::name (this).c_str ())));
    return S_FALSE; // --> stop
  } // end IF
  ACE_ASSERT (message_block_p);
  if (unlikely (message_block_p->msg_type () == ACE_Message_Block::MB_STOP))
  {
    message_block_p->release (); message_block_p = NULL;

    // stream has ended --> wait for DirectShow ?
    if (!directShowHasEnded_)
    {
      while (command_e != CMD_STOP)
      {
        command_e = (enum Command)inherited::GetRequest ();
        inherited::Reply ((command_e == CMD_STOP) ? S_FALSE : S_OK);
      } // end WHILE
      directShowHasEnded_ = true;
    } // end IF

    return S_FALSE; // --> stop
  } // end IF

  size_t data_length_2 = message_block_p->length ();
  ACE_ASSERT (static_cast<size_t> (data_length_l) >= data_length_2);
  // *NOTE*: ideally, the message buffers should derive from IMediaSample to
  //         avoid this copy; this is how asynchronous 'push' source filters can
  //         be more efficient in 'capture' pipelines
  // *TODO*: for synchronous 'pull' filters, consider blocking in the
  //         GetBuffer() method (see above) and supplying the samples as soon as
  //         they arrive. Note that this should work as long as the downstream
  //         filter accepts 'external' allocators (apparently, some default
  //         filters, e.g. the Microsoft Color Space Converter, do not support
  //         this (IMemAllocator::SetProperties() fails with E_INVALIDARG, and
  //         the allocator handle passed into IMemInputPin::NotifyAllocator() is
  //         ignored; this requires investigation). Until further notice,
  //         memcpy() seems to be unavoidable to forward the data)
  ACE_OS::memcpy (data_p,
                  message_block_p->rd_ptr (),
                  data_length_2);
  message_block_p->release ();
  result = mediaSample_in->SetActualDataLength (data_length_2);
  if (unlikely (FAILED (result)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s/%s: failed to IMediaSample::SetActualDataLength(): \"%s\", aborting\n"),
                ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::name (inherited::m_pFilter).c_str ()),
                ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::name (this).c_str ()),
                ACE_TEXT (Common_Error_Tools::errorToString (result, true).c_str ())));
    return S_FALSE; // --> stop
  } // end IF

  // the current time is the samples' start
  CRefTime ref_time = sampleTime_;
  // increment to find the finish time
  sampleTime_ += (LONG)frameInterval_;
  result = mediaSample_in->SetTime ((REFERENCE_TIME*)&ref_time,
                                    (REFERENCE_TIME*)&sampleTime_);
  if (unlikely (FAILED (result)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s/%s: failed to IMediaSample::SetTime(): \"%s\", aborting\n"),
                ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::name (inherited::m_pFilter).c_str ()),
                ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::name (this).c_str ()),
                ACE_TEXT (Common_Error_Tools::errorToString (result, true).c_str ())));
    return S_FALSE; // --> stop
  } // end IF
  result = mediaSample_in->SetMediaTime ((REFERENCE_TIME*)&ref_time,
                                         (REFERENCE_TIME*)&sampleTime_);
  if (unlikely (FAILED (result)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s/%s: failed to IMediaSample::SetMediaTime(): \"%s\", aborting\n"),
                ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::name (inherited::m_pFilter).c_str ()),
                ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::name (this).c_str ()),
                ACE_TEXT (Common_Error_Tools::errorToString (result, true).c_str ())));
    return S_FALSE; // --> stop
  } // end IF

  // *TODO*: support delta frames
  result = mediaSample_in->SetSyncPoint (TRUE);
  if (unlikely (FAILED (result)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s/%s: failed to IMediaSample::SetSyncPoint(): \"%s\", aborting\n"),
                ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::name (inherited::m_pFilter).c_str ()),
                ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::name (this).c_str ()),
                ACE_TEXT (Common_Error_Tools::errorToString (result, true).c_str ())));
    return S_FALSE; // --> stop
  } // end IF

  //ULONG reference_count = mediaSample_in->AddRef ();
  //reference_count = mediaSample_in->Release ();

  return S_OK; // --> continue
} // FillBuffer

//
// OnThreadCreate
//
// As we go active reset the stream time to zero
//
template <typename ConfigurationType,
          typename FilterType,
          typename MediaType>
HRESULT
Stream_MediaFramework_DirectShow_Source_Filter_OutputPin_T<ConfigurationType,
                                                           FilterType,
                                                           MediaType>::OnThreadCreate ()
{
  STREAM_TRACE (ACE_TEXT ("Stream_MediaFramework_DirectShow_Source_Filter_OutputPin_T::OnThreadCreate"));

  // sanity check(s)
  ACE_ASSERT (inherited::m_pFilter);

#if defined (_DEBUG)
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("%s/%s: %t: spawned DirectShow processing thread\n"),
              ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::name (inherited::m_pFilter).c_str ()),
              ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::name (this).c_str ())));
#endif // _DEBUG

  // { CAutoLock cAutoLockShared (&lock_);
  // we need to also reset the repeat time in case the system
  // clock is turned off after m_iRepeatTime gets very big
  //frameInterval_ = defaultFrameInterval_; } // end lock scope

  return NOERROR;
} // OnThreadCreate
template <typename ConfigurationType,
          typename FilterType,
          typename MediaType>
HRESULT
Stream_MediaFramework_DirectShow_Source_Filter_OutputPin_T<ConfigurationType,
                                                           FilterType,
                                                           MediaType>::OnThreadDestroy ()
{
  STREAM_TRACE (ACE_TEXT ("Stream_MediaFramework_DirectShow_Source_Filter_OutputPin_T::OnThreadDestroy"));

  // sanity check(s)
  ACE_ASSERT (inherited::m_pFilter);

#if defined (_DEBUG)
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("%s/%s: %t: stopped DirectShow processing thread\n"),
              ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::name (inherited::m_pFilter).c_str ()),
              ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::name (this).c_str ())));
#endif // _DEBUG

  return NOERROR;
} // OnThreadDestroy
template <typename ConfigurationType,
          typename FilterType,
          typename MediaType>
HRESULT
Stream_MediaFramework_DirectShow_Source_Filter_OutputPin_T<ConfigurationType,
                                                           FilterType,
                                                           MediaType>::OnThreadStartPlay ()
{
  STREAM_TRACE (ACE_TEXT ("Stream_MediaFramework_DirectShow_Source_Filter_OutputPin_T::OnThreadStartPlay"));

  // sanity check(s)
  ACE_ASSERT (inherited::m_pFilter);

#if defined (_DEBUG)
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("%s/%s: %t: started DirectShow processing thread\n"),
              ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::name (inherited::m_pFilter).c_str ()),
              ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::name (this).c_str ())));
#endif // _DEBUG

  return NOERROR;
} // OnThreadStartPlay

//template <typename ConfigurationType,
//          typename FilterType,
//          typename MediaType>
//DWORD
//Stream_MediaFramework_DirectShow_Source_Filter_OutputPin_T<ConfigurationType,
//                                                           FilterType,
//                                                           MediaType>::ThreadProc ()
//{
//  STREAM_TRACE (ACE_TEXT ("Stream_MediaFramework_DirectShow_Source_Filter_OutputPin_T::ThreadProc"));
//
//  enum CommandType command_e;
//  do
//  {
//    command_e = (enum CommandType)GetRequest ();
//    if (unlikely (command_e != CMD_INIT))
//    {
//      ACE_DEBUG ((LM_ERROR,
//                  ACE_TEXT ("%s: invalid state change (expected %d, was: %d), aborting\n"),
//                  ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::name (inherited::m_pFilter).c_str ()),
//                  CMD_INIT, command_e));
//      Reply ((DWORD)E_UNEXPECTED);
//    } // end IF
//  } while (command_e != CMD_INIT);
//#if defined (_DEBUG)
//  ACE_DEBUG ((LM_DEBUG,
//              ACE_TEXT ("%s: initialized DirectShow processing thread (id: %t)\n"),
//              ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::name (inherited::m_pFilter).c_str ())));
//#endif // _DEBUG
//
//  HRESULT result = OnThreadCreate ();
//  if (FAILED (result))
//  {
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("%s: failed to OnThreadCreate(): \"%s\", aborting\n"),
//                ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::name (inherited::m_pFilter).c_str ()),
//                ACE_TEXT (Common_Error_Tools::errorToString (result).c_str ())));
//    HRESULT result_2 = OnThreadDestroy ();
//    ACE_UNUSED_ARG (result_2);
//    Reply (result); // send failed return code from OnThreadCreate
//    return -1;
//  } // end IF
//
//  // Initialisation suceeded
//  Reply (NOERROR);
//
//  do
//  {
//    command_e = (enum CommandType)GetRequest ();
//    switch (command_e)
//    {
//      case CMD_EXIT:
//        Reply (NOERROR);
//        break;
//      case CMD_RUN:
//        ACE_DEBUG ((LM_WARNING,
//                    ACE_TEXT ("%s: CMD_RUN received before CMD_PAUSE, continuing\n"),
//                    ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::name (inherited::m_pFilter).c_str ())));
//        // !!! fall through ???
//      case CMD_PAUSE:
//        Reply (NOERROR);
//        result = DoBufferProcessingLoop ();
//        break;
//      case CMD_STOP:
//        Reply (NOERROR);
//        break;
//      default:
//        ACE_DEBUG ((LM_ERROR,
//                    ACE_TEXT ("%s: invalid/unknown command (was: %d), continuing\n"),
//                    ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::name (inherited::m_pFilter).c_str ()),
//                    command_e));
//        Reply ((DWORD)E_NOTIMPL);
//        break;
//    } // end SWITCH
//  } while (command_e != CMD_EXIT);
//
//  result = OnThreadDestroy ();
//  if (FAILED (result))
//  {
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("%s: failed to OnThreadDestroy(): \"%s\", aborting\n"),
//                ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::name (inherited::m_pFilter).c_str ()),
//                ACE_TEXT (Common_Error_Tools::errorToString (result).c_str ())));
//    return -1;
//  } // end IF
//
//#if defined (_DEBUG)
//  ACE_DEBUG ((LM_DEBUG,
//              ACE_TEXT ("%s: finalized DirectShow processing thread (id: %t)\n"),
//              ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::name (inherited::m_pFilter).c_str ()),
//              ACE_TEXT (Common_Error_Tools::errorToString (result).c_str ())));
//#endif // _DEBUG
//  return 0;
//}
//
//template <typename ConfigurationType,
//          typename FilterType,
//          typename MediaType>
//HRESULT
//Stream_MediaFramework_DirectShow_Source_Filter_OutputPin_T<ConfigurationType,
//                                                           FilterType,
//                                                           MediaType>::DoBufferProcessingLoop ()
//{
//  STREAM_TRACE (ACE_TEXT ("Stream_MediaFramework_DirectShow_Source_Filter_OutputPin_T::DoBufferProcessingLoop"));
//
//  HRESULT result = OnThreadStartPlay ();
//  if (FAILED (result))
//  {
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("%s: failed to OnThreadStartPlay(): \"%s\", aborting\n"),
//                ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::name (inherited::m_pFilter).c_str ()),
//                ACE_TEXT (Common_Error_Tools::errorToString (result).c_str ())));
//    return result;
//  } // end IF
//
//  enum CommandType command_e;
//  IMediaSample* media_sample_p = NULL;
//  do
//  {
//    while (!CheckRequest ((DWORD*)&command_e))
//    { ACE_ASSERT (!media_sample_p);
//      result = GetDeliveryBuffer (&media_sample_p,
//                                  NULL,
//                                  NULL,
//                                  0);
//      if (FAILED (result))
//      {
//        Sleep (1);
//        continue; // go round again. Perhaps the error will go away
//                  // or the allocator is decommited & we will be asked to
//                  // exit soon
//      } // end IF
//      ACE_ASSERT (media_sample_p);
//
//      result = FillBuffer (media_sample_p);
//      if (result == S_OK)
//      {
//        result = Deliver (media_sample_p);
//        // downstream filter returns S_FALSE if it wants us to
//        // stop or an error if it's reporting an error.
//        if (FAILED (result))
//        {
//          ACE_DEBUG ((LM_ERROR,
//                      ACE_TEXT ("%s: failed to Deliver(): \"%s\", aborting\n"),
//                      ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::name (inherited::m_pFilter).c_str ()),
//                      ACE_TEXT (Common_Error_Tools::errorToString (result).c_str ())));
//          media_sample_p->Release(); media_sample_p = NULL;
//          return result;
//        } // end IF
//      } // end IF
//      else if (result == S_FALSE)
//      {
//        // derived class wants us to stop pushing data
//        media_sample_p->Release(); media_sample_p = NULL;
//        HRESULT result_2 = DeliverEndOfStream ();
//        ACE_UNUSED_ARG (result_2);
//        return S_OK;
//      } // end ELSE IF
//      else
//      {
//        ACE_DEBUG ((LM_ERROR,
//                    ACE_TEXT ("%s: failed to FillBuffer(): \"%s\", aborting\n"),
//                    ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::name (inherited::m_pFilter).c_str ()),
//                    ACE_TEXT (Common_Error_Tools::errorToString (result).c_str ())));
//        media_sample_p->Release(); media_sample_p = NULL;
//        HRESULT result_2 = DeliverEndOfStream ();
//        ACE_UNUSED_ARG (result_2);
//        inherited::m_pFilter->NotifyEvent (EC_ERRORABORT, result, 0);
//        return result;
//      } // end ELSE
//      media_sample_p->Release(); media_sample_p = NULL;
//    } // end WHILE
//
//    // reply to all received commands
//    if ((command_e == CMD_RUN) ||
//        (command_e == CMD_PAUSE))
//      Reply (NOERROR);
//    else if (command_e != CMD_STOP)
//    {
//      ACE_DEBUG ((LM_WARNING,
//                  ACE_TEXT ("%s: invalid/unknown command (was: %d): \"%s\", continuing\n"),
//                  ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::name (inherited::m_pFilter).c_str ()),
//                  command_e));
//      Reply ((DWORD)E_UNEXPECTED);
//    } // end ELSE IF
//  } while (command_e != CMD_STOP);
//
//  return S_FALSE;
//}
//
//template <typename ConfigurationType,
//          typename FilterType,
//          typename MediaType>
//HRESULT
//Stream_MediaFramework_DirectShow_Source_Filter_OutputPin_T<ConfigurationType,
//                                                           FilterType,
//                                                           MediaType>::Active (void)
//{
//  STREAM_TRACE (ACE_TEXT ("Stream_MediaFramework_DirectShow_Source_Filter_OutputPin_T::Active"));
//
//  // sanity check(s)
//  if (parentFilter_->IsActive ())
//    return S_FALSE;
//
//  ACE_ASSERT (parentFilter_);
//  CAutoLock lock (&(parentFilter_->lock_));
//
//  // do nothing if not connected - its ok not to connect to
//  // all pins of a source filter
//  if (!IsConnected ())
//    return NOERROR;
//
//  // commit the allocator
//  HRESULT result = inherited::Active ();
//  if (FAILED (result))
//    return result;
//
//  ACE_ASSERT (!ThreadExists ());
//
//  if (!Create ())
//    return E_FAIL;
//
//  // Tell thread to initialize. If OnThreadCreate Fails, so does this.
//  result = Init ();
//  if (FAILED (result))
//    return result;
//
//  return Pause ();
//}
//template <typename ConfigurationType,
//          typename FilterType,
//          typename MediaType>
//HRESULT
//Stream_MediaFramework_DirectShow_Source_Filter_OutputPin_T<ConfigurationType,
//                                                           FilterType,
//                                                           MediaType>::Inactive (void)
//{
//  STREAM_TRACE (ACE_TEXT ("Stream_MediaFramework_DirectShow_Source_Filter_OutputPin_T::Inactive"));
//
//  // sanity check(s)
//  ACE_ASSERT (parentFilter_);
//  CAutoLock lock (&(parentFilter_->lock_));
//
//  // do nothing if not connected - its ok not to connect to
//  // all pins of a source filter
//  if (!IsConnected ())
//    return NOERROR;
//
//  // *NOTE*: decommit the allocator first to avoid a potential deadlock
//  HRESULT result = CBaseOutputPin::Inactive ();
//  if (FAILED (result))
//    return result;
//
//  if (ThreadExists ())
//  {
//    result = Stop ();
//    if (FAILED (result))
//      return result;
//    result = Exit ();
//    if (FAILED (result))
//      return result;
//  } // end IF
//
//  Close (); // wait for thread
//
//  return NOERROR;
//}

//
// Notify
//
// Alter the repeat rate according to quality management messages sent from
// the downstream filter (often the renderer).  Wind it up or down according
// to the flooding level - also skip forward if we are notified of Late-ness
//
template <typename ConfigurationType,
          typename FilterType,
          typename MediaType>
STDMETHODIMP
Stream_MediaFramework_DirectShow_Source_Filter_OutputPin_T<ConfigurationType,
                                                           FilterType,
                                                           MediaType>::Notify (IBaseFilter* filter_in,
                                                                               Quality quality_in)
{
  STREAM_TRACE (ACE_TEXT ("Stream_MediaFramework_DirectShow_Source_Filter_OutputPin_T::Notify"));

  // Adjust the repeat rate.
  if (quality_in.Proportion <= 0)
      frameInterval_ = 1000;        // We don't go slower than 1 per second
  else
  {
    frameInterval_ = frameInterval_ * 1000 / quality_in.Proportion;
    if (frameInterval_ > 1000)
      frameInterval_ = 1000;    // We don't go slower than 1 per second
    else if (frameInterval_ < 10)
      frameInterval_ = 10;      // We don't go faster than 100/sec
  } // end ELSE

  // skip forwards
  if (quality_in.Late > 0)
    sampleTime_ = sampleTime_ + quality_in.Late;

  return NOERROR;
} // Notify

// ---------------------------------------

template <typename ConfigurationType,
          typename FilterType,
          typename MediaType>
STDMETHODIMP
Stream_MediaFramework_DirectShow_Source_Filter_OutputPin_T<ConfigurationType,
                                                           FilterType,
                                                           MediaType>::Get (REFGUID guidPropSet_in,
                                                                            DWORD dwPropID_in,
                                                                            LPVOID pInstanceData_in,
                                                                            DWORD cbInstanceData_in,
                                                                            LPVOID pPropData_in,
                                                                            DWORD cbPropData_in,
                                                                            DWORD* pcbReturned_in)
{
  STREAM_TRACE (ACE_TEXT ("Stream_MediaFramework_DirectShow_Source_Filter_OutputPin_T::Get"));

  // sanity check(s)
  if (!InlineIsEqualGUID (guidPropSet_in, AMPROPSETID_Pin))
    return E_PROP_SET_UNSUPPORTED;
  if (dwPropID_in != AMPROPERTY_PIN_CATEGORY)
    return E_PROP_ID_UNSUPPORTED;
  if (pPropData_in == NULL && pcbReturned_in == NULL)
    return E_POINTER;

  if (pcbReturned_in)
    *pcbReturned_in = sizeof (struct _GUID);
  if (pPropData_in == NULL)  // Caller just wants to know the size.
    return S_OK;

  // sanity check(s)
  if (cbPropData_in < sizeof (struct _GUID)) // The buffer is too small.
    return E_UNEXPECTED;

  *reinterpret_cast<struct _GUID*> (pPropData_in) = PIN_CATEGORY_CAPTURE;

  return S_OK;
}
template <typename ConfigurationType,
          typename FilterType,
          typename MediaType>
STDMETHODIMP
Stream_MediaFramework_DirectShow_Source_Filter_OutputPin_T<ConfigurationType,
                                                           FilterType,
                                                           MediaType>::QuerySupported (REFGUID guidPropSet_in,
                                                                                       DWORD dwPropID_in,
                                                                                       DWORD* pTypeSupport_in)
{
  STREAM_TRACE (ACE_TEXT ("Stream_MediaFramework_DirectShow_Source_Filter_OutputPin_T::QuerySupported"));

  // sanity check(s)
  if (!InlineIsEqualGUID (guidPropSet_in, AMPROPSETID_Pin))
    return E_PROP_SET_UNSUPPORTED;
  if (dwPropID_in != AMPROPERTY_PIN_CATEGORY)
    return E_PROP_ID_UNSUPPORTED;

  if (pTypeSupport_in)
    // We support getting this property, but not setting it.
    *pTypeSupport_in = KSPROPERTY_SUPPORT_GET;

  return S_OK;
}

//template <typename ConfigurationType,
//          typename FilterType,
//          typename MediaType>
//STDMETHODIMP
//Stream_MediaFramework_DirectShow_Source_Filter_OutputPin_T<ConfigurationType,
//                                                           FilterType,
//                                                           MediaType>::Get (REFGUID rguidPropSet_in,
//                                                                            ULONG ulId_in,
//                                                                            LPVOID pInstanceData_in,
//                                                                            ULONG ulInstanceLength_in,
//                                                                            LPVOID pPropertyData_in,
//                                                                            ULONG ulDataLength_in,
//                                                                            PULONG pulBytesReturned_in)
//{
//  return Get (rguidPropSet_in,
//              static_cast<DWORD> (ulId_in),
//              pInstanceData_in,
//              static_cast<DWORD> (ulInstanceLength_in),
//              pPropertyData_in,
//              static_cast<DWORD> (ulDataLength_in),
//              static_cast<DWORD*> (pulBytesReturned_in));
//}
template <typename ConfigurationType,
          typename FilterType,
          typename MediaType>
STDMETHODIMP
Stream_MediaFramework_DirectShow_Source_Filter_OutputPin_T<ConfigurationType,
                                                           FilterType,
                                                           MediaType>::QuerySupport (REFGUID rguidPropSet_in,
                                                                                     ULONG ulId_in,
                                                                                     PULONG pulTypeSupport_in)
{
  return QuerySupported (rguidPropSet_in,
                         static_cast<DWORD> (ulId_in),
                         static_cast<DWORD*> (pulTypeSupport_in));
}

// ------------------------------------

template <typename ConfigurationType,
          typename FilterType,
          typename MediaType>
STDMETHODIMP
Stream_MediaFramework_DirectShow_Source_Filter_OutputPin_T<ConfigurationType,
                                                           FilterType,
                                                           MediaType>::SuggestAllocatorProperties (const struct _AllocatorProperties* properties_in)
{
  STREAM_TRACE (ACE_TEXT ("Stream_MediaFramework_DirectShow_Source_Filter_OutputPin_T::SuggestAllocatorProperties"));

  // sanity check(s)
  CheckPointer (properties_in, E_POINTER);

  FilterType* filter_p = dynamic_cast<FilterType*> (inherited::m_pFilter);
  ACE_ASSERT (filter_p);
  filter_p->allocatorProperties_ = *properties_in;

  return S_OK;
}
template <typename ConfigurationType,
          typename FilterType,
          typename MediaType>
STDMETHODIMP
Stream_MediaFramework_DirectShow_Source_Filter_OutputPin_T<ConfigurationType,
                                                           FilterType,
                                                           MediaType>::GetAllocatorProperties (struct _AllocatorProperties* properties_out)
{
  STREAM_TRACE (ACE_TEXT ("Stream_MediaFramework_DirectShow_Source_Filter_OutputPin_T::GetAllocatorProperties"));

  // sanity check(s)
  CheckPointer (properties_out, E_POINTER);

  FilterType* filter_p = dynamic_cast<FilterType*> (inherited::m_pFilter);
  ACE_ASSERT (filter_p);

  *properties_out = filter_p->allocatorProperties_;

  return NOERROR;
}

// ------------------------------------

template <typename ConfigurationType,
          typename FilterType,
          typename MediaType>
STDMETHODIMP
Stream_MediaFramework_DirectShow_Source_Filter_OutputPin_T<ConfigurationType,
                                                           FilterType,
                                                           MediaType>::SetFormat (struct _AMMediaType* pmt_in)
{
  STREAM_TRACE (ACE_TEXT ("Stream_MediaFramework_DirectShow_Source_Filter_OutputPin_T::SetFormat"));

  // sanity check(s)
  CheckPointer (pmt_in, E_POINTER);
  ACE_ASSERT (mediaType_);

  // check compatibility
  CMediaType media_type, media_type_2;
  HRESULT result = media_type.Set (*pmt_in);
  ACE_ASSERT (SUCCEEDED (result));
  result = media_type_2.Set (*mediaType_);
  ACE_ASSERT (SUCCEEDED (result));
  if (!media_type.MatchesPartial (&media_type_2))
  {
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("%s/%s: incompatible media types (\"%s\"\n\"%s\")\n"),
                ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::name (inherited::m_pFilter).c_str ()),
                ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::name (this).c_str ()),
                ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::toString (*pmt_in).c_str ()),
                ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::toString (*mediaType_).c_str ())));
    return VFW_E_INVALIDMEDIATYPE;
  } // end IF

  if (pmt_in == mediaType_)
    goto continue_;
  if (mediaType_)
    Stream_MediaFramework_DirectShow_Tools::delete_ (mediaType_);
  mediaType_ =
    Stream_MediaFramework_DirectShow_Tools::copy (*pmt_in);
  if (!mediaType_)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s/%s: failed to Stream_MediaFramework_DirectShow_Tools::copy(): \"%m\", aborting\n"),
                ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::name (inherited::m_pFilter).c_str ()),
                ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::name (this).c_str ())));
    return E_OUTOFMEMORY;
  } // end IF

continue_:
  // *TODO*: verify connectivity and state
  //         see also: https://msdn.microsoft.com/en-us/library/windows/desktop/dd319788(v=vs.85).aspx
  // VFW_E_NOT_CONNECTED
  // VFW_E_NOT_STOPPED
  // VFW_E_WRONG_STATE

#if defined (_DEBUG)
  if (pmt_in == mediaType_)
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("%s/%s: reset preferred format\n"),
                ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::name (inherited::m_pFilter).c_str ()),
                ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::name (this).c_str ())));
  else
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("%s/%s: set preferred format: %s\n"),
                ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::name (inherited::m_pFilter).c_str ()),
                ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::name (this).c_str ()),
                ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::toString (*mediaType_, true).c_str ())));
#endif // _DEBUG

  return S_OK;
}
template <typename ConfigurationType,
          typename FilterType,
          typename MediaType>
STDMETHODIMP
Stream_MediaFramework_DirectShow_Source_Filter_OutputPin_T<ConfigurationType,
                                                           FilterType,
                                                           MediaType>::GetFormat (struct _AMMediaType** ppmt_inout)
{
  STREAM_TRACE (ACE_TEXT ("Stream_MediaFramework_DirectShow_Source_Filter_OutputPin_T::GetFormat"));

  // sanity check(s)
  CheckPointer (ppmt_inout, E_POINTER);
  IPin* pin_p = NULL;
  //HRESULT result = inherited::ConnectedTo (&pin_p);
  //if (FAILED (result)) return result;
  //pin_p->Release ();
  ACE_ASSERT (!*ppmt_inout);
  ACE_ASSERT (mediaType_);

  *ppmt_inout = Stream_MediaFramework_DirectShow_Tools::copy (*mediaType_);
  if (!*ppmt_inout)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s/%s: failed to Stream_MediaFramework_DirectShow_Tools::copy(): \"%m\", aborting\n"),
                ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::name (inherited::m_pFilter).c_str ()),
                ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::name (this).c_str ())));
    return E_OUTOFMEMORY;
  } // end IF
  ACE_ASSERT (*ppmt_inout);

  return S_OK;
}
template <typename ConfigurationType,
          typename FilterType,
          typename MediaType>
STDMETHODIMP
Stream_MediaFramework_DirectShow_Source_Filter_OutputPin_T<ConfigurationType,
                                                           FilterType,
                                                           MediaType>::GetNumberOfCapabilities (int* piCount_inout,
                                                                                                int* piSize_inout)
{
  STREAM_TRACE (ACE_TEXT ("Stream_MediaFramework_DirectShow_Source_Filter_OutputPin_T::GetNumberOfCapabilities"));

  // sanity check(s)
  CheckPointer (piCount_inout, E_POINTER);
  CheckPointer (piSize_inout, E_POINTER);

  *piCount_inout = 1;
  *piSize_inout = sizeof (struct _VIDEO_STREAM_CONFIG_CAPS);

  return S_OK;
}
template <typename ConfigurationType,
          typename FilterType,
          typename MediaType>
STDMETHODIMP
Stream_MediaFramework_DirectShow_Source_Filter_OutputPin_T<ConfigurationType,
                                                           FilterType,
                                                           MediaType>::GetStreamCaps (int iIndex_in,
                                                                                      struct _AMMediaType** ppmt_inout,
                                                                                      BYTE* pSCC_in)
{
  STREAM_TRACE (ACE_TEXT ("Stream_MediaFramework_DirectShow_Source_Filter_OutputPin_T::GetStreamCaps"));

  // sanity check(s)
  if (iIndex_in > 0)
    return S_FALSE; // E_INVALIDARG
  ACE_ASSERT (ppmt_inout);
  ACE_ASSERT (!*ppmt_inout);
  ACE_ASSERT (pSCC_in);
  ACE_ASSERT (mediaType_);

  *ppmt_inout =
    Stream_MediaFramework_DirectShow_Tools::copy (*mediaType_);
  if (!*ppmt_inout)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s/%s: failed to Stream_MediaFramework_DirectShow_Tools::copy(): \"%m\", aborting\n"),
                ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::name (inherited::m_pFilter).c_str ()),
                ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::name (this).c_str ())));
    return E_OUTOFMEMORY;
  } // end IF
  ACE_ASSERT (*ppmt_inout);

  struct _VIDEO_STREAM_CONFIG_CAPS* capabilities_p =
    reinterpret_cast<struct _VIDEO_STREAM_CONFIG_CAPS*> (pSCC_in);
  ACE_OS::memset (capabilities_p,
                  0,
                  sizeof (struct _VIDEO_STREAM_CONFIG_CAPS));
  capabilities_p->guid = mediaType_->formattype;
  capabilities_p->VideoStandard = 0;
  struct tagVIDEOINFOHEADER* video_info_p = NULL;
  struct tagVIDEOINFOHEADER2* video_info_2 = NULL;
  if (InlineIsEqualGUID (mediaType_->formattype, FORMAT_VideoInfo))
  {
    video_info_p =
      reinterpret_cast<struct tagVIDEOINFOHEADER*> (mediaType_->pbFormat);
    capabilities_p->InputSize.cx = video_info_p->bmiHeader.biWidth;
    capabilities_p->InputSize.cy = video_info_p->bmiHeader.biHeight;
    capabilities_p->MinCroppingSize.cx =
      (video_info_p->rcSource.right - video_info_p->rcSource.left);
    capabilities_p->MinCroppingSize.cy =
      (video_info_p->rcSource.bottom - video_info_p->rcSource.top);
    capabilities_p->MinOutputSize.cx = video_info_p->bmiHeader.biWidth;
    capabilities_p->MinOutputSize.cy = video_info_p->bmiHeader.biHeight;
    capabilities_p->MaxOutputSize.cx = video_info_p->bmiHeader.biWidth;
    capabilities_p->MaxOutputSize.cx = video_info_p->bmiHeader.biHeight;
    capabilities_p->MinFrameInterval = video_info_p->AvgTimePerFrame;
    capabilities_p->MaxFrameInterval = video_info_p->AvgTimePerFrame;
    capabilities_p->MinBitsPerSecond = video_info_p->dwBitRate;
    capabilities_p->MaxBitsPerSecond = video_info_p->dwBitRate;
  } // end IF
  else if (InlineIsEqualGUID (mediaType_->formattype, FORMAT_VideoInfo2))
  {
    video_info_2 =
      reinterpret_cast<struct tagVIDEOINFOHEADER2*> (mediaType_->pbFormat);
    capabilities_p->InputSize.cx = video_info_2->bmiHeader.biWidth;
    capabilities_p->InputSize.cy = video_info_2->bmiHeader.biHeight;
    capabilities_p->MinCroppingSize.cx =
      (video_info_2->rcSource.right - video_info_2->rcSource.left);
    capabilities_p->MinCroppingSize.cy =
      (video_info_2->rcSource.bottom - video_info_2->rcSource.top);
    capabilities_p->MinOutputSize.cx = video_info_2->bmiHeader.biWidth;
    capabilities_p->MinOutputSize.cy = video_info_2->bmiHeader.biHeight;
    capabilities_p->MaxOutputSize.cx = video_info_2->bmiHeader.biWidth;
    capabilities_p->MaxOutputSize.cx = video_info_2->bmiHeader.biHeight;
    capabilities_p->MinFrameInterval = video_info_2->AvgTimePerFrame;
    capabilities_p->MaxFrameInterval = video_info_2->AvgTimePerFrame;
    capabilities_p->MinBitsPerSecond = video_info_2->dwBitRate;
    capabilities_p->MaxBitsPerSecond = video_info_2->dwBitRate;
  } // end ELSE IF
  else
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s/%s: invalid/unknnown media type format (was: \"%s\"), aborting\n"),
                ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::name (inherited::m_pFilter).c_str ()),
                ACE_TEXT (Stream_MediaFramework_DirectShow_Tools::name (this).c_str ()),
                ACE_TEXT (Common_Tools::GUIDToString (mediaType_->formattype).c_str ())));
    return E_OUTOFMEMORY;
  } // end ELSE

  return S_OK;
}
