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

#ifndef STREAM_MESSAGEQUEUE_H
#define STREAM_MESSAGEQUEUE_H

#include "ace/Global_Macros.h"
#include "ace/Synch_Traits.h"

#include "common_time_common.h"

#include "stream_messagequeue_base.h"

//class Stream_Export Stream_MessageQueue
template <typename SessionMessageType>
class Stream_MessageQueue_T
 : public Stream_MessageQueueBase_T<ACE_MT_SYNCH,
                                    Common_TimePolicy_t>
{
 public:
  Stream_MessageQueue_T (unsigned int); // maximum number of queued buffers
  virtual ~Stream_MessageQueue_T ();

  // implement Stream_IMessageQueue
  virtual unsigned int flush (bool = false);
  virtual void waitForIdleState () const;

 private:
  typedef Stream_MessageQueueBase_T<ACE_MT_SYNCH,
                                    Common_TimePolicy_t> inherited;

  ACE_UNIMPLEMENTED_FUNC (Stream_MessageQueue_T ())
  ACE_UNIMPLEMENTED_FUNC (Stream_MessageQueue_T (const Stream_MessageQueue_T&))
  ACE_UNIMPLEMENTED_FUNC (Stream_MessageQueue_T& operator= (const Stream_MessageQueue_T&))

  // convenient types
  typedef Stream_MessageQueue_T<SessionMessageType> OWN_TYPE_T;
};

// include template definition
#include "stream_messagequeue.inl"

#endif
