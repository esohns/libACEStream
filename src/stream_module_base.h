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

#ifndef STREAM_MODULE_BASE_H
#define STREAM_MODULE_BASE_H

#include <string>

#include <ace/Global_Macros.h>

#include "stream_imodule.h"

// forward declaration(s)
template <ACE_SYNCH_DECL, class TIME_POLICY>
class ACE_Task;
template <ACE_SYNCH_DECL, class TIME_POLICY>
class ACE_Module;
class Common_IRefCount;

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          ////////////////////////////////
          typename SessionIdType,
          typename SessionDataType,
          typename SessionEventType,
          typename ConfigurationType,
          typename HandlerConfigurationType,
          ////////////////////////////////
          typename NotificationType, // *NOTE*: stream notification interface
          typename ReaderTaskType,
          typename WriterTaskType>
class Stream_Module_Base_T
 : public ACE_Module<ACE_SYNCH_USE,
                     TimePolicyType>
 , public Stream_IModule_T<SessionIdType,
                           SessionDataType,
                           SessionEventType,
                           ACE_SYNCH_USE,
                           TimePolicyType,
                           ConfigurationType,
                           HandlerConfigurationType>
{
 public:
  // convenient types
  typedef ConfigurationType CONFIGURATION_T;
  typedef Stream_IModule_T<SessionIdType,
                           SessionDataType,
                           SessionEventType,
                           ACE_SYNCH_USE,
                           TimePolicyType,
                           ConfigurationType,
                           HandlerConfigurationType> IMODULE_T;

  virtual ~Stream_Module_Base_T () throw ();

  // implement (part of) Stream_IModule_T
  // *IMPORTANT NOTE*: the default implementation simply forwards all module
  //                   events to the processing stream instance...
  virtual void start (SessionIdType,             // session id
                      const SessionDataType&);   // session data
  virtual void notify (SessionIdType,            // session id
                       const SessionEventType&); // event (state/status change, ...)
  virtual void end (SessionIdType);              // session id
  virtual const ConfigurationType& get () const;
  virtual bool initialize (const ConfigurationType&);
  virtual const HandlerConfigurationType& getHandlerConfiguration () const;
  virtual bool isFinal () const;
  virtual void reset ();

 protected:
  // convenient types
  typedef ACE_Task<ACE_SYNCH_USE,
                   TimePolicyType> TASK_T;

  Stream_Module_Base_T (const std::string&, // name
                        TASK_T*,            // handle to writer task
                        TASK_T*,            // handle to reader task
                        Common_IRefCount*,  // object counter
                        bool = false,       // delete tasks in dtor ?
                        bool = false);      // final module ?

  ConfigurationType* configuration_;
  NotificationType*  notify_;

 private:
  typedef ACE_Module<ACE_SYNCH_USE,
                     TimePolicyType> inherited;

  // convenient types
  typedef ACE_Module<ACE_SYNCH_USE,
                     TimePolicyType> MODULE_T;
  typedef Stream_Module_Base_T<ACE_SYNCH_USE,
                               TimePolicyType,
                               SessionIdType,
                               SessionDataType,
                               SessionEventType,
                               ConfigurationType,
                               HandlerConfigurationType,
                               NotificationType, // *NOTE*: stream notification interface
                               ReaderTaskType,
                               WriterTaskType> OWN_TYPE_T;
  typedef Stream_IModuleHandler_T<ACE_SYNCH_USE,
                                  TimePolicyType> IMODULE_HANDLER_T;
  typedef Common_IGet_T<HandlerConfigurationType> IGET_T;

  // implement (part of) Stream_IModule
  virtual MODULE_T* clone ();

  ACE_UNIMPLEMENTED_FUNC (Stream_Module_Base_T ())
  ACE_UNIMPLEMENTED_FUNC (Stream_Module_Base_T (const Stream_Module_Base_T&))
  ACE_UNIMPLEMENTED_FUNC (Stream_Module_Base_T& operator= (const Stream_Module_Base_T&))

  bool               delete_;
  bool               isFinal_;
  TASK_T*            reader_;
  TASK_T*            writer_;
};

// include template definition
#include "stream_module_base.inl"

#endif
