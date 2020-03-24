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

#ifndef TEST_I_MESSAGE_H
#define TEST_I_MESSAGE_H

#include "ace/Global_Macros.h"

#include "common_iget.h"

#include "stream_data_message_base.h"

#include "http_codes.h"
#include "http_defines.h"
#include "http_tools.h"

//#include "test_i_http_get_common.h"

// forward declaration(s)
class ACE_Allocator;
class ACE_Data_Block;
class ACE_Message_Block;
class Test_I_Stream_SessionMessage;
template <ACE_SYNCH_DECL,
          typename AllocatorConfigurationType,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType> class Stream_MessageAllocatorHeapBase_T;
typedef Stream_ControlMessage_T<enum Stream_ControlType,
                                enum Stream_ControlMessageType,
                                struct Common_Parser_FlexAllocatorConfiguration> Test_I_ControlMessage_t;

struct Test_I_MessageData
{
  Test_I_MessageData ()
    //: HTTPRecord (NULL)
    //, HTMLDocument (NULL)
  {}

  virtual ~Test_I_MessageData ()
  {
    //if (HTTPRecord)
    //  delete HTTPRecord;
    //if (HTMLDocument)
    //  xmlFreeDoc (HTMLDocument);
  }
  inline void operator+= (struct Test_I_MessageData rhs_in) { ACE_UNUSED_ARG (rhs_in); ACE_ASSERT (false); ACE_NOTSUP; ACE_NOTREACHED (return;) }
  //inline operator struct HTTP_Record&() const { ACE_ASSERT (HTTPRecord); return *HTTPRecord; }

   //struct HTTP_Record* HTTPRecord;
   //xmlDocPtr           HTMLDocument;
};
typedef Stream_DataBase_T<struct Test_I_MessageData> Test_I_MessageData_t;

//class Test_I_MessageData_t
// : public Stream_DataBase_T<struct Test_I_MessageData>
// //, public Common_ISetPR_T<struct HTTP_Record>
//{
//  typedef Stream_DataBase_T<struct Test_I_MessageData> inherited;
//
// public:
//  Test_I_MessageData_t ();
//  // *IMPORTANT NOTE*: fire-and-forget API
//  Test_I_MessageData_t (struct Test_I_MessageData*&, // data handle
//                             bool = true);                // delete in dtor ?
//  inline virtual ~Test_I_MessageData_t () {};
//
//  // implement Common_ISetPR_T
//  //virtual void setPR (struct HTTP_Record*&);
//
// private:
//  ACE_UNIMPLEMENTED_FUNC (Test_I_MessageData_t (const Test_I_MessageData_t&))
//  //ACE_UNIMPLEMENTED_FUNC (Test_I_MessageData_t& operator= (const Test_I_MessageData_t&))
//};

//////////////////////////////////////////

class Test_I_Stream_Message
 : public Stream_DataMessageBase_T<Test_I_MessageData_t,
                                   struct Common_Parser_FlexAllocatorConfiguration,
                                   enum Stream_MessageType,
                                   int>
{
  typedef Stream_DataMessageBase_T<Test_I_MessageData_t,
                                   struct Common_Parser_FlexAllocatorConfiguration,
                                   enum Stream_MessageType,
                                   int> inherited;

  // grant access to specific private ctors
  friend class Stream_MessageAllocatorHeapBase_T<ACE_MT_SYNCH,
                                                 struct Common_Parser_FlexAllocatorConfiguration,
                                                 Test_I_ControlMessage_t,
                                                 Test_I_Stream_Message,
                                                 Test_I_Stream_SessionMessage>;

 public:
  Test_I_Stream_Message (unsigned int); // size
  inline virtual ~Test_I_Stream_Message () {}

  // overrides from ACE_Message_Block
  // --> create a "shallow" copy of ourselves that references the same packet
  // *NOTE*: this uses our allocator (if any) to create a new message
  virtual ACE_Message_Block* duplicate (void) const;

  // implement Stream_MessageBase_T
  //virtual int command () const; // return value: message type
  //inline static std::string CommandTypeToString (HTTP_Method_t method_in) { return (method_in == HTTP_Codes::HTTP_METHOD_INVALID ? ACE_TEXT_ALWAYS_CHAR (HTTP_COMMAND_STRING_RESPONSE) : HTTP_Tools::MethodToString (method_in)); }

 protected:
  // copy ctor to be used by duplicate() and derived classes
  // --> uses an (incremented refcount of) the same datablock ("shallow copy")
  Test_I_Stream_Message (const Test_I_Stream_Message&);

 private:
  ACE_UNIMPLEMENTED_FUNC (Test_I_Stream_Message ())
  // *NOTE*: to be used by message allocators
  Test_I_Stream_Message (Stream_SessionId_t,
                         ACE_Data_Block*, // data block to use
                         ACE_Allocator*,  // message allocator
                         bool = true);    // increment running message counter ?
  Test_I_Stream_Message (Stream_SessionId_t,
                         ACE_Allocator*); // message allocator
  ACE_UNIMPLEMENTED_FUNC (Test_I_Stream_Message& operator= (const Test_I_Stream_Message&))
};

#endif
