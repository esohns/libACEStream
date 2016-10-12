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

#ifndef TEST_U_AUDIOEFFECT_MESSAGE_H
#define TEST_U_AUDIOEFFECT_MESSAGE_H

#include <ace/Global_Macros.h>

#include "stream_data_message_base.h"

#include "test_u_audioeffect_common.h"

// forward declaration(s)
class ACE_Allocator;
class ACE_Data_Block;
class ACE_Message_Block;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
class Test_U_AudioEffect_DirectShow_SessionMessage;
class Test_U_AudioEffect_MediaFoundation_SessionMessage;
#else
class Test_U_AudioEffect_SessionMessage;
#endif
template <typename AllocatorConfigurationType,
          typename ControlMessageType,
          typename MessageType,
          typename SessionMessageType>
class Stream_MessageAllocatorHeapBase_T;

#if defined (ACE_WIN32) || defined (ACE_WIN64)
class Test_U_AudioEffect_DirectShow_Message
 : public Stream_DataMessageBase_T<Stream_AllocatorConfiguration,
                                   Test_U_AudioEffect_DirectShow_ControlMessage_t,
                                   Test_U_AudioEffect_DirectShow_SessionMessage,
                                   Test_U_AudioEffect_DirectShow_MessageData,
                                   Stream_CommandType_t>
{
  // grant access to specific private ctors
  friend class Stream_MessageAllocatorHeapBase_T<Stream_AllocatorConfiguration,
                                                 Test_U_AudioEffect_DirectShow_ControlMessage_t,
                                                 Test_U_AudioEffect_DirectShow_Message,
                                                 Test_U_AudioEffect_DirectShow_SessionMessage>;

 public:
  Test_U_AudioEffect_DirectShow_Message (unsigned int); // size
  virtual ~Test_U_AudioEffect_DirectShow_Message ();

  // overrides from ACE_Message_Block
  // --> create a "shallow" copy of ourselves that references the same packet
  // *NOTE*: this uses our allocator (if any) to create a new message
  virtual ACE_Message_Block* duplicate (void) const;

  virtual Stream_CommandType_t command () const; // return value: message type
  static std::string CommandType2String (Stream_CommandType_t);

 protected:
  // copy ctor to be used by duplicate() and child classes
  // --> uses an (incremented refcount of) the same datablock ("shallow copy")
  Test_U_AudioEffect_DirectShow_Message (const Test_U_AudioEffect_DirectShow_Message&);

 private:
  typedef Stream_DataMessageBase_T<Stream_AllocatorConfiguration,
                                   Test_U_AudioEffect_DirectShow_ControlMessage_t,
                                   Test_U_AudioEffect_DirectShow_SessionMessage,
                                   Test_U_AudioEffect_DirectShow_MessageData,
                                   Stream_CommandType_t> inherited;

  ACE_UNIMPLEMENTED_FUNC (Test_U_AudioEffect_DirectShow_Message ())
  // *NOTE*: to be used by message allocators
  Test_U_AudioEffect_DirectShow_Message (ACE_Data_Block*, // data block
                                         ACE_Allocator*,  // message allocator
                                         bool = true);    // increment running message counter ?
  Test_U_AudioEffect_DirectShow_Message (ACE_Allocator*); // message allocator
  ACE_UNIMPLEMENTED_FUNC (Test_U_AudioEffect_DirectShow_Message& operator= (const Test_U_AudioEffect_DirectShow_Message&))
};

//////////////////////////////////////////

class Test_U_AudioEffect_MediaFoundation_Message
 : public Stream_DataMessageBase_T<Stream_AllocatorConfiguration,
                                   Test_U_AudioEffect_MediaFoundation_ControlMessage_t,
                                   Test_U_AudioEffect_MediaFoundation_SessionMessage,
                                   Test_U_AudioEffect_MediaFoundation_MessageData,
                                   Stream_CommandType_t>
{
  // grant access to specific private ctors
  friend class Stream_MessageAllocatorHeapBase_T<Stream_AllocatorConfiguration,
                                                 Test_U_AudioEffect_MediaFoundation_ControlMessage_t,
                                                 Test_U_AudioEffect_MediaFoundation_Message,
                                                 Test_U_AudioEffect_MediaFoundation_SessionMessage>;

 public:
  Test_U_AudioEffect_MediaFoundation_Message (unsigned int); // size
  virtual ~Test_U_AudioEffect_MediaFoundation_Message ();

  // overrides from ACE_Message_Block
  // --> create a "shallow" copy of ourselves that references the same packet
  // *NOTE*: this uses our allocator (if any) to create a new message
  virtual ACE_Message_Block* duplicate (void) const;

  virtual Stream_CommandType_t command () const; // return value: message type
  static std::string CommandType2String (Stream_CommandType_t);

 protected:
  // copy ctor to be used by duplicate() and child classes
  // --> uses an (incremented refcount of) the same datablock ("shallow copy")
  Test_U_AudioEffect_MediaFoundation_Message (const Test_U_AudioEffect_MediaFoundation_Message&);

 private:
  typedef Stream_DataMessageBase_T<Stream_AllocatorConfiguration,
                                   Test_U_AudioEffect_MediaFoundation_ControlMessage_t,
                                   Test_U_AudioEffect_MediaFoundation_SessionMessage,
                                   Test_U_AudioEffect_MediaFoundation_MessageData,
                                   Stream_CommandType_t> inherited;

  ACE_UNIMPLEMENTED_FUNC (Test_U_AudioEffect_MediaFoundation_Message ())
  // *NOTE*: to be used by message allocators
  Test_U_AudioEffect_MediaFoundation_Message (ACE_Data_Block*, // data block
                                              ACE_Allocator*,  // message allocator
                                              bool = true);    // increment running message counter ?
  Test_U_AudioEffect_MediaFoundation_Message (ACE_Allocator*); // message allocator
  ACE_UNIMPLEMENTED_FUNC (Test_U_AudioEffect_MediaFoundation_Message& operator= (const Test_U_AudioEffect_MediaFoundation_Message&))
};
#else
class Test_U_AudioEffect_Message
 : public Stream_MessageBase_T<Stream_AllocatorConfiguration,
                               Test_U_AudioEffect_ControlMessage_t,
                               Test_U_AudioEffect_SessionMessage,
                               Stream_CommandType_t>
{
  // grant access to specific private ctors
  friend class Stream_MessageAllocatorHeapBase_T<Stream_AllocatorConfiguration,
                                                 Test_U_AudioEffect_ControlMessage_t,
                                                 Test_U_AudioEffect_Message,
                                                 Test_U_AudioEffect_SessionMessage>;

 public:
  Test_U_AudioEffect_Message (unsigned int); // size
  virtual ~Test_U_AudioEffect_Message ();

  // overrides from ACE_Message_Block
  // --> create a "shallow" copy of ourselves that references the same packet
  // *NOTE*: this uses our allocator (if any) to create a new message
  virtual ACE_Message_Block* duplicate (void) const;

  virtual Stream_CommandType_t command () const; // return value: message type
  static std::string CommandType2String (Stream_CommandType_t);

 protected:
  // copy ctor to be used by duplicate() and child classes
  // --> uses an (incremented refcount of) the same datablock ("shallow copy")
  Test_U_AudioEffect_Message (const Test_U_AudioEffect_Message&);

 private:
  typedef Stream_MessageBase_T<Stream_AllocatorConfiguration,
                               Test_U_AudioEffect_ControlMessage_t,
                               Test_U_AudioEffect_SessionMessage,
                               Stream_CommandType_t> inherited;

  ACE_UNIMPLEMENTED_FUNC (Test_U_AudioEffect_Message ())
  // *NOTE*: to be used by message allocators
  Test_U_AudioEffect_Message (ACE_Data_Block*, // data block
                              ACE_Allocator*,  // message allocator
                              bool = true);    // increment running message counter ?
  Test_U_AudioEffect_Message (ACE_Allocator*); // message allocator
  ACE_UNIMPLEMENTED_FUNC (Test_U_AudioEffect_Message& operator= (const Test_U_AudioEffect_Message&))
};
#endif

#endif
