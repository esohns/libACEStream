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

#ifndef STREAM_MODULE_NET_IO_H
#define STREAM_MODULE_NET_IO_H

#include "ace/Global_Macros.h"
#include "ace/Synch_Traits.h"

#include "common_istatistic.h"
#include "common_time_common.h"

#include "stream_common.h"
#include "stream_headmoduletask_base.h"
#include "stream_task_base_synch.h"

template <typename SessionMessageType,
          typename ProtocolMessageType,
          ///////////////////////////////
          typename ConfigurationType,
          ///////////////////////////////
          typename StreamStateType,
          ///////////////////////////////
          typename SessionDataType,          // session data
          typename SessionDataContainerType, // session message payload (reference counted)
          ///////////////////////////////
          typename StatisticContainerType,
          ///////////////////////////////
          typename AddressType,
          typename ConnectionManagerType>
class Stream_Module_Net_IOWriter_T // --> output
 : public Stream_HeadModuleTaskBase_T<ACE_MT_SYNCH,
                                      Common_TimePolicy_t,
                                      SessionMessageType,
                                      ProtocolMessageType,
                                      ///
                                      ConfigurationType,
                                      ///
                                      StreamStateType,
                                      ///
                                      SessionDataType,
                                      SessionDataContainerType>
 , public Common_IStatistic_T<StatisticContainerType>
{
 public:
  Stream_Module_Net_IOWriter_T ();
  virtual ~Stream_Module_Net_IOWriter_T ();

#if defined (__GNUG__) || defined (_MSC_VER)
  // *PORTABILITY*: for some reason, this base class member is not exposed
  //                (MSVC/gcc)
  using Stream_HeadModuleTaskBase_T<ACE_MT_SYNCH,
                                    Common_TimePolicy_t,
                                    SessionMessageType,
                                    ProtocolMessageType,
                                    ConfigurationType,
                                    StreamStateType,
                                    SessionDataType,
                                    SessionDataContainerType>::initialize;
#endif

  // override (part of) Stream_IModuleHandler_T
  virtual bool initialize (const ConfigurationType&);

  // info
  bool isInitialized () const;

  // implement (part of) Stream_ITaskBase
  virtual void handleDataMessage (ProtocolMessageType*&, // data message handle
                                  bool&);                // return value: pass message downstream ?
  virtual void handleSessionMessage (SessionMessageType*&, // session message handle
                                     bool&);               // return value: pass message downstream ?

  // implement Common_IStatistic
  // *NOTE*: implements regular (timer-based) statistics collection
  virtual bool collect (StatisticContainerType&); // return value: (currently unused !)
  virtual void report () const;

  virtual void upStream (Stream_Base_t*);
  virtual Stream_Base_t* upStream () const;

 private:
  typedef Stream_HeadModuleTaskBase_T<ACE_MT_SYNCH,
                                      Common_TimePolicy_t,
                                      SessionMessageType,
                                      ProtocolMessageType,
                                      ///
                                      ConfigurationType,
                                      ///
                                      StreamStateType,
                                      ///
                                      SessionDataType,
                                      SessionDataContainerType> inherited;
  typedef ACE_Message_Queue<ACE_MT_SYNCH,
                            Common_TimePolicy_t> MESSAGEQUEUE_T;

  //  ACE_UNIMPLEMENTED_FUNC (Stream_Module_Net_IOWriter_T ())
  ACE_UNIMPLEMENTED_FUNC (Stream_Module_Net_IOWriter_T (const Stream_Module_Net_IOWriter_T&))
  ACE_UNIMPLEMENTED_FUNC (Stream_Module_Net_IOWriter_T& operator= (const Stream_Module_Net_IOWriter_T&))

  // helper methods
  //  virtual int svc (void);
  ProtocolMessageType* allocateMessage (unsigned int); // (requested) size
  bool putStatisticMessage (const StatisticContainerType&) const; // statistic info

  typename ConnectionManagerType::CONNECTION_T* connection_;
  bool                                          isInitialized_;

  // timer
  Stream_StatisticHandler_Reactor_t             statisticCollectionHandler_;
  long                                          timerID_;
};

/////////////////////////////////////////

template <typename SessionMessageType,
          typename MessageType,
          ///////////////////////////////
          typename ConfigurationType,
          ///////////////////////////////
          typename ModuleHandlerConfigurationType,
          ///////////////////////////////
          typename SessionDataType,          // session data
          typename SessionDataContainerType, // session message payload (reference counted)
          ///////////////////////////////
          typename AddressType,
          typename ConnectionManagerType>
class Stream_Module_Net_IOReader_T // --> input
 : public Stream_TaskBaseSynch_T<Common_TimePolicy_t,
                                 SessionMessageType,
                                 MessageType>
 , public Stream_IModuleHandler_T<ModuleHandlerConfigurationType>
{
 public:
   Stream_Module_Net_IOReader_T ();
   virtual ~Stream_Module_Net_IOReader_T ();

   // implement (part of) Stream_ITaskBase_T
   virtual void handleDataMessage (MessageType*&, // data message handle
                                   bool&);        // return value: pass message downstream ?
   virtual void handleSessionMessage (SessionMessageType*&, // session message handle
                                      bool&);               // return value: pass message downstream ?

   // implement Stream_IModuleHandler_T
   virtual bool initialize (const ModuleHandlerConfigurationType&);
   virtual const ModuleHandlerConfigurationType& get () const;

 protected:
  ModuleHandlerConfigurationType                configuration_;

 private:
  typedef Stream_TaskBaseSynch_T<Common_TimePolicy_t,
                                 SessionMessageType,
                                 MessageType> inherited;

  ACE_UNIMPLEMENTED_FUNC (Stream_Module_Net_IOReader_T (const Stream_Module_Net_IOReader_T&))
  ACE_UNIMPLEMENTED_FUNC (Stream_Module_Net_IOReader_T& operator= (const Stream_Module_Net_IOReader_T&))

  typename ConnectionManagerType::CONNECTION_T* connection_;
  bool                                          isInitialized_;
};

#include "stream_module_io.inl"

#endif