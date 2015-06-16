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

#include "ace/Log_Msg.h"

#include "common_irefcount.h"

#include "stream_macros.h"

template <typename TaskSynchType,
          typename TimePolicyType,
          typename ConfigurationType,
          typename ReaderTaskType,
          typename WriterTaskType>
Stream_Module_Base_T<TaskSynchType,
                     TimePolicyType,
                     ConfigurationType,
                     ReaderTaskType,
                     WriterTaskType>::Stream_Module_Base_T (const std::string& name_in,
                                                            WriterTaskType* writerTask_in,
                                                            ReaderTaskType* readerTask_in,
                                                            Common_IRefCount* refCount_in)
 : inherited (ACE_TEXT_CHAR_TO_TCHAR (name_in.c_str ()), // name
              writerTask_in,                             // initialize writer side task
              readerTask_in,                             // initialize reader side task
              refCount_in,                               // argument passed to task open()
              inherited::M_DELETE_NONE)                  // don't "delete" ANYTHING during close()
 , configuration_ ()
 , reader_ (readerTask_in)
 , writer_ (writerTask_in)
{
  STREAM_TRACE (ACE_TEXT ("Stream_Module_Base_T::Stream_Module_Base_T"));

  // *WARNING*: apparently, we cannot use "this" at this stage
  // --> children must do this...
//   // set links to ourselves...
//   // *NOTE*: essential to enable dereferencing (name-lookups, controlled
  //shutdown, etc)
//   myWriter->mod_ = this;
//   myReader->mod_ = this;

  // *IMPORTANT NOTE*: when using this ACE_Module ctor, the next_ member isn't
  // initialized properly; this causes mayhem in a corner case so do it here...
  // *TODO*: notify the ACE people
  inherited::next(NULL);
}

template <typename TaskSynchType,
          typename TimePolicyType,
          typename ConfigurationType,
          typename ReaderTaskType,
          typename WriterTaskType>
Stream_Module_Base_T<TaskSynchType,
                     TimePolicyType,
                     ConfigurationType,
                     ReaderTaskType,
                     WriterTaskType>::~Stream_Module_Base_T ()
{
  STREAM_TRACE (ACE_TEXT ("Stream_Module_Base_T::~Stream_Module_Base_T"));

  // *NOTE*: the base class will invoke close() which will
  // invoke module_closed() and flush on every task...
  // *WARNING*: all member tasks will be destroyed by the time that happens...
  // --> close() all modules in advance so it doesn't happen here !!!
}

template <typename TaskSynchType,
          typename TimePolicyType,
          typename ConfigurationType,
          typename ReaderTaskType,
          typename WriterTaskType>
const ConfigurationType&
Stream_Module_Base_T<TaskSynchType,
                     TimePolicyType,
                     ConfigurationType,
                     ReaderTaskType,
                     WriterTaskType>::get () const
{
  STREAM_TRACE (ACE_TEXT ("Stream_Module_Base_T::get"));

  return configuration_;
}

template <typename TaskSynchType,
          typename TimePolicyType,
          typename ConfigurationType,
          typename ReaderTaskType,
          typename WriterTaskType>
bool
Stream_Module_Base_T<TaskSynchType,
                     TimePolicyType,
                     ConfigurationType,
                     ReaderTaskType,
                     WriterTaskType>::initialize (const ConfigurationType& configuration_in)
{
  STREAM_TRACE (ACE_TEXT ("Stream_Module_Base_T::initialize"));

  configuration_ = configuration_in;

  return true;
}

template <typename TaskSynchType,
          typename TimePolicyType,
          typename ConfigurationType,
          typename ReaderTaskType,
          typename WriterTaskType>
void
Stream_Module_Base_T<TaskSynchType,
                     TimePolicyType,
                     ConfigurationType,
                     ReaderTaskType,
                     WriterTaskType>::reset ()
{
  STREAM_TRACE (ACE_TEXT ("Stream_Module_Base_T::reset"));

  // OK: (re-)set our reader and writer tasks...
  inherited::writer(writer_,
                    inherited::M_DELETE_NONE);

  inherited::reader(reader_,
                    inherited::M_DELETE_NONE);
}

template <typename TaskSynchType,
          typename TimePolicyType,
          typename ConfigurationType,
          typename ReaderTaskType,
          typename WriterTaskType>
ACE_Module<TaskSynchType,
           TimePolicyType>*
Stream_Module_Base_T<TaskSynchType,
                     TimePolicyType,
                     ConfigurationType,
                     ReaderTaskType,
                     WriterTaskType>::clone ()
{
  STREAM_TRACE (ACE_TEXT ("Stream_Module_Base_T::clone"));

  // initialize return value(s)
  MODULE_T* module_p = NULL;

  // need a downcast...
  typename IMODULE_T::ICLONE_T* iclone_p =
      dynamic_cast<typename IMODULE_T::ICLONE_T*> (writer_);
  if (!iclone_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: dynamic_cast<Common_IClone_T> failed, aborting\n"),
                ACE_TEXT (inherited::name ())));
    return NULL;
  } // end IF

  try
  {
    module_p = iclone_p->clone ();
  }
  catch (...)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: caught exception in Common_IClone_T::clone(), continuing\n"),
                ACE_TEXT (inherited::name ())));
  }
  if (!module_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: failed to Common_IClone_T::clone(), aborting\n"),
                ACE_TEXT (inherited::name ())));
    return NULL;
  } // end IF

  return module_p;
}
