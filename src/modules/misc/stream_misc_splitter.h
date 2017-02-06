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

#ifndef STREAM_MODULE_SPLITTER_H
#define STREAM_MODULE_SPLITTER_H

#include <ace/config-lite.h>
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include <mfobjects.h>
#include <strmif.h>
#endif

#include <ace/Global_Macros.h>
#include <ace/Synch_Traits.h>

#include "common_time_common.h"

#include "stream_common.h"
#include "stream_headmoduletask_base.h"
#include "stream_imessage.h"
#include "stream_imodule.h"
#include "stream_task_base_synch.h"

// forward declarations
class ACE_Message_Block;
class Stream_IAllocator;

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          ////////////////////////////////
          typename ConfigurationType,
          ////////////////////////////////
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType,
          ////////////////////////////////
          typename SessionDataType>
class Stream_Module_Splitter_T
 : public Stream_TaskBaseSynch_T<ACE_SYNCH_USE,
                                 TimePolicyType,
                                 ConfigurationType,
                                 ControlMessageType,
                                 DataMessageType,
                                 SessionMessageType,
                                 Stream_SessionId_t,
                                 Stream_ControlType,
                                 Stream_SessionMessageType,
                                 Stream_UserData>
{
 public:
  Stream_Module_Splitter_T ();
  virtual ~Stream_Module_Splitter_T ();

  virtual bool initialize (const ConfigurationType&,
                           Stream_IAllocator*);

  // implement (part of) Stream_ITaskBase_T
  virtual void handleDataMessage (DataMessageType*&, // data message handle
                                  bool&);            // return value: pass message downstream ?
  //virtual void handleSessionMessage (SessionMessageType*&, // session message handle
  //                                   bool&);               // return value: pass message downstream ?

 protected:
  ACE_Message_Block* buffer_;
  bool               defragment_;
  unsigned int       PDUSize_;

 private:
  typedef Stream_TaskBaseSynch_T<ACE_SYNCH_USE,
                                 TimePolicyType,
                                 ConfigurationType,
                                 ControlMessageType,
                                 DataMessageType,
                                 SessionMessageType,
                                 Stream_SessionId_t,
                                 Stream_ControlType,
                                 Stream_SessionMessageType,
                                 Stream_UserData> inherited;

  ACE_UNIMPLEMENTED_FUNC (Stream_Module_Splitter_T (const Stream_Module_Splitter_T&))
  ACE_UNIMPLEMENTED_FUNC (Stream_Module_Splitter_T& operator= (const Stream_Module_Splitter_T&))

  // convenient types
  typedef Stream_IDataMessage_T<typename DataMessageType::MESSAGE_T,
                                typename DataMessageType::COMMAND_T> IDATA_MESSAGE_T;

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  // *NOTE*: callers must free the return value !
  template <typename FormatType> AM_MEDIA_TYPE* getFormat (const FormatType format_in) { return getFormat_impl (format_in); }
  AM_MEDIA_TYPE* getFormat_impl (const struct _AMMediaType*); // return value: media type handle
  AM_MEDIA_TYPE* getFormat_impl (const IMFMediaType*); // return value: media type handle
#else
  template <typename FormatType> struct v4l2_format* getFormat (const FormatType format_in) { return getFormat_impl (format_in); }
  struct v4l2_format* getFormat_impl (const struct Stream_Module_Device_ALSAConfiguration&); // return value: media type handle
  inline struct v4l2_format* getFormat_impl (const struct v4l2_format* format_in) { return const_cast<struct v4l2_format*> (format_in); } // return value: media type handle
#endif
};

//////////////////////////////////////////

template <ACE_SYNCH_DECL,
          ////////////////////////////////
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType,
          ////////////////////////////////
          typename ConfigurationType,
          ////////////////////////////////
          typename StreamControlType,
          typename StreamNotificationType,
          typename StreamStateType,
          ////////////////////////////////
          typename SessionDataType,          // session data
          typename SessionDataContainerType, // session message payload (reference counted)
          ////////////////////////////////
          typename StatisticContainerType>
class Stream_Module_SplitterH_T
 : public Stream_HeadModuleTaskBase_T<ACE_SYNCH_USE,
                                      Common_TimePolicy_t,
                                      ControlMessageType,
                                      DataMessageType,
                                      SessionMessageType,
                                      ConfigurationType,
                                      StreamControlType,
                                      StreamNotificationType,
                                      StreamStateType,
                                      SessionDataType,
                                      SessionDataContainerType,
                                      StatisticContainerType,
                                      Stream_UserData>
{
 public:
  Stream_Module_SplitterH_T (ACE_SYNCH_MUTEX_T* = NULL,                                                // lock handle (state machine)
                             bool = false,                                                             // auto-start ?
                             enum Stream_HeadModuleConcurrency = STREAM_HEADMODULECONCURRENCY_PASSIVE, // concurrency mode
                             bool = true);                                                             // generate session messages ?
  virtual ~Stream_Module_SplitterH_T ();

  // *PORTABILITY*: for some reason, this base class member is not exposed
  //                (MSVC/gcc)
  using Stream_HeadModuleTaskBase_T<ACE_SYNCH_USE,
                                    Common_TimePolicy_t,
                                    ControlMessageType,
                                    DataMessageType,
                                    SessionMessageType,
                                    ConfigurationType,
                                    StreamControlType,
                                    StreamNotificationType,
                                    StreamStateType,
                                    SessionDataType,
                                    SessionDataContainerType,
                                    StatisticContainerType,
                                    Stream_UserData>::initialize;

  // override (part of) Stream_IModuleHandler_T
  virtual bool initialize (const ConfigurationType&,
                           Stream_IAllocator*);

  // implement (part of) Stream_ITaskBase_T
  virtual void handleDataMessage (DataMessageType*&, // data message handle
                                  bool&);            // return value: pass message downstream ?
  virtual void handleSessionMessage (SessionMessageType*&, // session message handle
                                     bool&);               // return value: pass message downstream ?

  // implement Common_IStatistic
  // *NOTE*: implements regular (timer-based) statistic collection
  virtual bool collect (StatisticContainerType&); // return value: (currently unused !)
  //virtual void report () const;

 private:
  typedef Stream_HeadModuleTaskBase_T<ACE_SYNCH_USE,
                                      Common_TimePolicy_t,
                                      ControlMessageType,
                                      DataMessageType,
                                      SessionMessageType,
                                      ConfigurationType,
                                      StreamControlType,
                                      StreamNotificationType,
                                      StreamStateType,
                                      SessionDataType,
                                      SessionDataContainerType,
                                      StatisticContainerType,
                                      Stream_UserData> inherited;

  ACE_UNIMPLEMENTED_FUNC (Stream_Module_SplitterH_T (const Stream_Module_SplitterH_T&))
  ACE_UNIMPLEMENTED_FUNC (Stream_Module_SplitterH_T& operator= (const Stream_Module_SplitterH_T&))

//  virtual int svc (void);

  // helper methods
  //bool putStatisticMessage (const StatisticContainerType&) const; // statistics info
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  // *NOTE*: callers must free the return value !
  template <typename FormatType> AM_MEDIA_TYPE* getFormat (const FormatType format_in) { return getFormat_impl (format_in); }
  AM_MEDIA_TYPE* getFormat_impl (const struct _AMMediaType*); // return value: media type handle
  AM_MEDIA_TYPE* getFormat_impl (const IMFMediaType*); // return value: media type handle
#else
  template <typename FormatType> struct v4l2_format* getFormat (const FormatType format_in) { return getFormat_impl (format_in); }
  struct v4l2_format* getFormat_impl (const struct Stream_Module_Device_ALSAConfiguration&); // return value: media type handle
  inline struct v4l2_format* getFormat_impl (const struct v4l2_format* format_in) { return const_cast<struct v4l2_format*> (format_in); } // return value: media type handle
#endif

  ACE_Message_Block* buffer_;
  unsigned int       PDUSize_;
};

// include template definition
#include "stream_misc_splitter.inl"

#endif
