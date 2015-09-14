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

#ifndef STREAM_BASE_H
#define STREAM_BASE_H

#include <deque>

//#include "ace/Containers_T.h"
#include "ace/Global_Macros.h"
#include "ace/Stream.h"
#include "ace/Synch_Traits.h"

#include "common_idumpstate.h"
//#include "common_iget.h"
#include "common_iinitialize.h"
#include "common_istatistic.h"

#include "stream_common.h"
#include "stream_istreamcontrol.h"
#include "stream_streammodule_base.h"

// forward declaration(s)
class Stream_IAllocator;

template <typename TaskSynchType,
          typename TimePolicyType,
          ///////////////////////////////
          typename StatusType,
          typename StateType,
          ///////////////////////////////
          typename ConfigurationType,
          ///////////////////////////////
          typename StatisticContainerType,
          ///////////////////////////////
          typename ModuleConfigurationType,
          typename HandlerConfigurationType,
          ///////////////////////////////
          typename SessionDataType,          // session data
          typename SessionDataContainerType, // (reference counted)
          typename SessionMessageType,
          typename ProtocolMessageType>
class Stream_Base_T
 : public ACE_Stream<TaskSynchType,
                     TimePolicyType>
 , public Stream_IStreamControl_T<StatusType,
                                  StateType>
 , public Common_IDumpState
// , public Common_IGet_T<SessionDataType>
// , public Common_IInitialize_T<ConfigurationType>
 , public Common_IStatistic_T<StatisticContainerType>
{
 public:
  // convenient types
  typedef ACE_Module<TaskSynchType,
                     TimePolicyType> MODULE_T;
  typedef Stream_IModule_T<TaskSynchType,
                           TimePolicyType,
                           ModuleConfigurationType,
                           HandlerConfigurationType> IMODULE_T;
  typedef ACE_Stream<TaskSynchType,
                     TimePolicyType> STREAM_T;
  typedef ACE_Stream_Iterator<TaskSynchType,
                              TimePolicyType> ITERATOR_T;
  typedef StateType STATE_T;
  typedef SessionDataType SESSION_DATA_T;

  // *NOTE*: this will try to sanely close down the stream:
  // 1: tell all worker threads to exit gracefully
  // 2: close() all modules which have not been enqueued onto the stream
  //    (next() == NULL)
  // 3: close() the stream (closes all enqueued modules: wait for queue to flush
  //    and threads, if any, to join)
  virtual ~Stream_Base_T ();

  // overload this from ACE_Stream to work as a hook to pass a messagecounter as
  // argument to the modules
  // open() method...
  //virtual int push(ACE_Module<ACE_MT_SYNCH>*); // handle to module

  // implement Stream_IStreamControl_T
  // *NOTE*: delegate these calls to the head module (which also implements this
  //         API)
  virtual void start ();
  virtual void stop (bool = true,  // wait for completion ?
                     bool = true); // locked access ?
  virtual bool isRunning () const;

  virtual void flush ();
  virtual void pause ();
  virtual void rewind ();
  virtual const StatusType& status () const;
  virtual void waitForCompletion ();
  virtual const StateType& state () const;

  // implement Common_IDumpState
  virtual void dump_state () const;

//  // implement Common_IGet_T
//  virtual const SessionDataType& get () const;
  const SessionDataType& sessionData () const;

// // implement Common_IInitialize_T
  virtual bool initialize (const ConfigurationType&) = 0;

  //// override ACE_Stream method(s)
  // *NOTE*: the default ACE_Stream impementation of link() joins writer A to
  //         reader B and writer B to reader A. Prefer 'concat' method: writer
  //         A to writer B(, reader A to reader B; see explanation)
  // *TODO*: the 'outbound' pipe of readers is not currently implemented, as it
  //         creates problems in conjunction with libACENetwork.
  //         libACENetwork connections use the connection streams'
  //         ACE_Stream_Head reader queue/notification to buffer outbound data
  //         while the reactor/connection thread dispatches the corresponding
  //         events.
  //         libACEStream modules encapsulating a network connection may be
  //         tempted to link the data processing stream to the connections'
  //         stream. For 'inbound' (i.e. reader-side oriented) modules, the
  //         connection stream is prepended, for 'outbound' modules appended.
  //         However, doing so requires consideration of the fact that the
  //         connection will still look for data on the connections' (!)
  //         ACE_Stream_Head, which is 'hidden' by the default linking
  //         procedure.
  //         --> avoid linking the outbound side of the stream for now
  // *WARNING*: this method is NOT (!) threadsafe in places
  //            --> handle with care !
  virtual int link (STREAM_T&);

  //// *NOTE*: the default implementation close(s) the removed module. This is not
  ////         the intended behavior when the module is being used by several
  ////         streams at once
  //// *TODO*: this needs more work...
  //virtual int remove (const ACE_TCHAR*, // name
  //                    int = M_DELETE);  // flags
  // *NOTE*: this gracefully (see above) removes the given module (and chain)
  bool remove (MODULE_T*); // module handle
  // *NOTE*: make sure the original API is not hidden
  using STREAM_T::remove;

  bool isInitialized () const;

 protected:
  // convenient types
  typedef ACE_Task<TaskSynchType,
                   TimePolicyType> TASK_T;
  typedef ACE_Message_Queue<TaskSynchType,
                            TimePolicyType> QUEUE_T;
  typedef Common_IInitialize_T<HandlerConfigurationType> IMODULEHANDLER_T;
  typedef std::deque<MODULE_T*> MODULE_CONTAINER_T;
  typedef typename MODULE_CONTAINER_T::const_iterator MODULE_CONTAINER_ITERATOR_T;
  typedef Stream_IStreamControl_T<StatusType,
                                  StateType> ISTREAM_CONTROL_T;

  // *NOTE*: need to subclass this !
  Stream_Base_T ();

  // implement (part of) Common_IControl
  virtual void initialize ();

  // *NOTE*: children need to call this PRIOR to module RE-initialization
  //         (i.e. in their own init()); this will:
  //         - pop/close push()ed modules, clean up default head/tail modules
  //         - reset reader/writer tasks for ALL modules
  //         - generate new default head/tail modules
  // *WARNING*: calling this while isRunning() == true blocks until the stream
  //            finishes (because close() of a module waits for its worker
  //            thread(s)...)
  bool reset ();

  // *NOTE*: children MUST call this in their dtor !
  void shutdown ();

  // *NOTE*: children need to add handles to ALL of their modules to this container !
  //ACE_DLList<MODULE_T> availableModules_;
  MODULE_CONTAINER_T availableModules_;

  // *NOTE*: children need to set this IF their initialization succeeded;
  //         otherwise, the dtor will NOT stop all worker threads before
  //         close()ing the modules...
  bool               isInitialized_;

  // *NOTE*: children need to initialize these !
  Stream_IAllocator* allocator_;
  SessionDataType*   sessionData_;
  StateType          state_;

 private:
  typedef ACE_Stream<TaskSynchType,
                     TimePolicyType> inherited;

  // convenient types
//  typedef Stream_HeadModuleTaskBase_T<TaskSynchType,
//                                      TimePolicyType,
//                                      StreamStateType,
//                                      SessionDataType,          // session data
//                                      SessionDataContainerType, // (reference counted)
//                                      SessionMessageType,
//                                      ProtocolMessageType> HEADMODULE_TASK_T;
  typedef Stream_Base_T<TaskSynchType,
                        TimePolicyType,
                        StatusType,
                        StateType,
                        ConfigurationType,
                        StatisticContainerType,
                        ModuleConfigurationType,
                        HandlerConfigurationType,
                        SessionDataType,
                        SessionDataContainerType,
                        SessionMessageType,
                        ProtocolMessageType> OWN_TYPE_T;

  ACE_UNIMPLEMENTED_FUNC (Stream_Base_T (const Stream_Base_T&))
  ACE_UNIMPLEMENTED_FUNC (Stream_Base_T& operator= (const Stream_Base_T&))

  // helper methods
  // wrap inherited::open/close() calls
  bool finalize ();
  void deactivateModules ();
};

// include template implementation
#include "stream_base.inl"

#endif
