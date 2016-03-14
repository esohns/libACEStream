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

#include "ace/Global_Macros.h"
#include "ace/Synch_Traits.h"

#include "common_time_common.h"

#include "stream_common.h"
#include "stream_headmoduletask_base.h"
#include "stream_imodule.h"
#include "stream_task_base_synch.h"

// forward declarations
class ACE_Message_Block;

template <typename SessionMessageType,
          typename MessageType,
          ///////////////////////////////
          typename ConfigurationType,
          ///////////////////////////////
          typename SessionDataType>
class Stream_Module_Splitter_T
 : public Stream_TaskBaseSynch_T<Common_TimePolicy_t,
                                 SessionMessageType,
                                 MessageType>
 , public Stream_IModuleHandler_T<ConfigurationType>
{
 public:
  Stream_Module_Splitter_T ();
  virtual ~Stream_Module_Splitter_T ();

  // implement (part of) Stream_ITaskBase_T
  virtual void handleDataMessage (MessageType*&, // data message handle
                                  bool&);        // return value: pass message downstream ?
  virtual void handleSessionMessage (SessionMessageType*&, // session message handle
                                     bool&);               // return value: pass message downstream ?

  // implement Stream_IModuleHandler_T
  virtual bool initialize (const ConfigurationType&);
  virtual const ConfigurationType& get () const;

 protected:
  ConfigurationType* configuration_;

 private:
  typedef Stream_TaskBaseSynch_T<Common_TimePolicy_t,
                                 SessionMessageType,
                                 MessageType> inherited;

  //typedef Stream_Module_Splitter_T<SessionMessageType,
  //                             MessageType,
  //                             ///////////
  //                             ConfigurationType,
  //                             ///////////
  //                             SessionDataType> OWN_TYPE_T;

  ACE_UNIMPLEMENTED_FUNC (Stream_Module_Splitter_T (const Stream_Module_Splitter_T&))
  ACE_UNIMPLEMENTED_FUNC (Stream_Module_Splitter_T& operator= (const Stream_Module_Splitter_T&))

  ACE_Message_Block* currentBuffer_;
};

/////////////////////////////////////////

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
class Stream_Module_SplitterH_T
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
{
 public:
  Stream_Module_SplitterH_T (bool = false,  // active object ?
                             bool = false); // auto-start ?
  virtual ~Stream_Module_SplitterH_T ();

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

  // implement (part of) Stream_ITaskBase_T
  virtual void handleDataMessage (ProtocolMessageType*&, // data message handle
                                  bool&);                // return value: pass message downstream ?
  virtual void handleSessionMessage (SessionMessageType*&, // session message handle
                                     bool&);               // return value: pass message downstream ?

  // implement Common_IStatistic
  // *NOTE*: implements regular (timer-based) statistic collection
  virtual bool collect (StatisticContainerType&); // return value: (currently unused !)
  //virtual void report () const;

 protected:
  ConfigurationType* configuration_;

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

  ACE_UNIMPLEMENTED_FUNC (Stream_Module_SplitterH_T (const Stream_Module_SplitterH_T&))
  ACE_UNIMPLEMENTED_FUNC (Stream_Module_SplitterH_T& operator= (const Stream_Module_SplitterH_T&))

//  virtual int svc (void);

  // helper methods
  //bool putStatisticMessage (const StatisticContainerType&) const; // statistics info

  ACE_Message_Block* currentBuffer_;
  //bool               isInitialized_;
};

// include template defintion
#include "stream_misc_splitter.inl"

#endif