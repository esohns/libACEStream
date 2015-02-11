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

#ifndef STREAM_SESSION_MESSAGE_H
#define STREAM_SESSION_MESSAGE_H

#include <string>

#include "ace/Global_Macros.h"
#include "ace/Message_Block.h"

#include "common_idumpstate.h"

#include "stream_exports.h"
#include "stream_messageallocatorheap.h"
#include "stream_message_base.h"

// forward declaratation(s)
class Stream_SessionConfiguration;
class ACE_Allocator;

class Stream_Export Stream_SessionMessage
 : public ACE_Message_Block,
   public Common_IDumpState
{
  // need access to specific ctors
  friend class Stream_MessageAllocatorHeap;
//   friend class Stream_MessageAllocatorHeapBase<arg1, arg2>;

 public:
  enum SessionMessageType
  {
    // *NOTE*: see <stream_message_base.h> for details...
    MB_BEGIN_STREAM_SESSION_MAP = Stream_MessageBase::MB_STREAM_SESSION,
    // *** STREAM CONTROL ***
    MB_STREAM_SESSION_BEGIN,
    MB_STREAM_SESSION_STEP,
    MB_STREAM_SESSION_END,
    MB_STREAM_SESSION_STATISTICS
    // *** STREAM CONTROL - END ***
  };

  // *NOTE*: assume lifetime responsibility for the second argument !
  Stream_SessionMessage(unsigned int,                   // session ID
                        const SessionMessageType&,      // session message type
                        Stream_SessionConfiguration*&); // config handle
  virtual ~Stream_SessionMessage ();

  // initialization-after-construction
  // *NOTE*: assume lifetime responsibility for the second argument !
  void init (unsigned int,                   // session ID
             const SessionMessageType&,      // session message type
             Stream_SessionConfiguration*&); // config handle

  // info
  unsigned int getID () const;
  SessionMessageType getType () const;
  // *TODO*: clean this up !
  const Stream_SessionConfiguration* const getConfiguration () const;

  // implement Common_IDumpState
  virtual void dump_state () const;

  // overloaded from ACE_Message_Block
  virtual ACE_Message_Block* duplicate (void) const;

  // debug tools
  static void SessionMessageType2String (SessionMessageType, // message type
                                         std::string&);      // corresp. string

 private:
  typedef ACE_Message_Block inherited;

  // copy ctor to be used by duplicate()
  Stream_SessionMessage (const Stream_SessionMessage&);

  // *NOTE*: these may be used by message allocators...
  // *WARNING*: these ctors are NOT threadsafe...
  Stream_SessionMessage (ACE_Allocator*); // message allocator
  Stream_SessionMessage (ACE_Data_Block*, // data block
                         ACE_Allocator*); // message allocator

  ACE_UNIMPLEMENTED_FUNC (Stream_SessionMessage ());
  ACE_UNIMPLEMENTED_FUNC (Stream_SessionMessage& operator= (const Stream_SessionMessage&));

  unsigned int                 ID_;
  SessionMessageType           messageType_;
  Stream_SessionConfiguration* configuration_;
  bool                         isInitialized_;
};

// convenient types
typedef Stream_SessionMessage::SessionMessageType Stream_SessionMessageType;

#endif
