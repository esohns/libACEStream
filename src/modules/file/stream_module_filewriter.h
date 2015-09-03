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

#ifndef STREAM_MODULE_FILEWRITER_H
#define STREAM_MODULE_FILEWRITER_H

#include "ace/FILE_IO.h"
#include "ace/Global_Macros.h"

#include "common_time_common.h"

#include "stream_task_base_asynch.h"

#define STREAM_MODULE_FILE_DEFAULT_OUTPUT_FILE "output.tmp"

template <typename SessionMessageType,
          typename MessageType,
          ///////////////////////////////
          typename ModuleHandlerConfigurationType,
          ///////////////////////////////
          typename SessionDataType>
class Stream_Module_FileWriter_T
 : public Stream_TaskBaseAsynch_T<Common_TimePolicy_t,
                                  SessionMessageType,
                                  MessageType>
 , public Stream_IModuleHandler_T<ModuleHandlerConfigurationType>
{
 public:
  Stream_Module_FileWriter_T ();
  virtual ~Stream_Module_FileWriter_T ();

  // implement (part of) Stream_ITaskBase_T
  virtual void handleDataMessage (MessageType*&, // data message handle
                                  bool&);        // return value: pass message downstream ?
  virtual void handleSessionMessage (SessionMessageType*&, // session message handle
                                     bool&);               // return value: pass message downstream ?

  // implement Stream_IModuleHandler_T
  virtual bool initialize (const ModuleHandlerConfigurationType&);
  virtual const ModuleHandlerConfigurationType& get () const;

 protected:
  ModuleHandlerConfigurationType configuration_;

 private:
  typedef Stream_TaskBaseAsynch_T<Common_TimePolicy_t,
                                  SessionMessageType,
                                  MessageType> inherited;

  ACE_UNIMPLEMENTED_FUNC (Stream_Module_FileWriter_T (const Stream_Module_FileWriter_T&))
  ACE_UNIMPLEMENTED_FUNC (Stream_Module_FileWriter_T& operator= (const Stream_Module_FileWriter_T&))

  bool        isOpen_;
  ACE_FILE_IO stream_;
};

#include "stream_module_filewriter.inl"

#endif
