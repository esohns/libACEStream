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

#ifndef STREAM_MODULE_PARSER_H
#define STREAM_MODULE_PARSER_H

#include <iostream>
#include <string>

#include "ace/Global_Macros.h"
#include "ace/Synch_Traits.h"

#include "common_time_common.h"

#include "stream_common.h"
#include "stream_imodule.h"
#include "stream_iparser.h"
#include "stream_task_base_synch.h"

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          ////////////////////////////////
          typename ConfigurationType,
          ////////////////////////////////
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType,
          ////////////////////////////////
          typename ScannerType, // (f/)lex-
          typename ParserType, // yacc/bison-
          typename ParserConfigurationType,
          typename ParserInterfaceType, // derived from Stream_IParser_T
          typename ArgumentType, // yacc/bison-
          ////////////////////////////////
          typename UserDataType>
class Stream_Module_Parser_T
 : public Stream_TaskBaseSynch_T<ACE_SYNCH_USE,
                                 TimePolicyType,
                                 ConfigurationType,
                                 ControlMessageType,
                                 DataMessageType,
                                 SessionMessageType,
                                 Stream_SessionId_t,
                                 enum Stream_ControlType,
                                 enum Stream_SessionMessageType,
                                 UserDataType>
 , public ParserInterfaceType
 , virtual public Stream_IScanner_T<ParserInterfaceType>
{
  typedef Stream_TaskBaseSynch_T<ACE_SYNCH_USE,
                                 TimePolicyType,
                                 ConfigurationType,
                                 ControlMessageType,
                                 DataMessageType,
                                 SessionMessageType,
                                 Stream_SessionId_t,
                                 enum Stream_ControlType,
                                 enum Stream_SessionMessageType,
                                 UserDataType> inherited;

 public:
  // *TODO*: on MSVC 2015u3 the accurate declaration does not compile
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  Stream_Module_Parser_T (ISTREAM_T*,                     // stream handle
#else
  Stream_Module_Parser_T (typename inherited::ISTREAM_T*, // stream handle
#endif
                          bool,  // debug scanning ?
                          bool); // debug parsing ?
  virtual ~Stream_Module_Parser_T ();

  virtual bool initialize (const ConfigurationType&,
                           Stream_IAllocator* = NULL);

  // implement (part of) Stream_ITaskBase_T
  virtual void handleDataMessage (DataMessageType*&, // data message handle
                                  bool&);            // return value: pass message downstream ?
  virtual void handleSessionMessage (SessionMessageType*&, // session message handle
                                     bool&);               // return value: pass message downstream ?

  // implement (part of) ParserInterfaceType
  inline virtual ACE_Message_Block* buffer () { return fragment_; };
//  inline virtual bool debugScanner () const { return bittorrent_get_debug (scannerState_); };
  inline virtual bool debugScanner () const { ACE_ASSERT (false); ACE_NOTSUP_RETURN (false); ACE_NOTREACHED (return false;) };
  inline virtual bool isBlocking () const { return blockInParse_; };
//  virtual void error (const YYLTYPE&,      // location
//                      const std::string&); // message
  virtual void error (const std::string&); // message
  inline virtual void offset (unsigned int offset_in) { offset_ += offset_in; }; // offset (increment)
  inline virtual unsigned int offset () const { return offset_; };
  virtual bool parse (ACE_Message_Block*); // data buffer handle
  virtual bool switchBuffer (bool = false); // unlink current fragment ?
  // *NOTE*: (waits for and) appends the next data chunk to fragment_;
  virtual void waitBuffer ();

  inline virtual void debug (yyscan_t state_in, bool toggle_in) { scanner_ .debug (state_in, toggle_in); };

  inline virtual bool initialize (yyscan_t& state_in) { return scanner_.initialize (state_in); };
  virtual void finalize (yyscan_t& state_in) { scanner_.finalize (state_in); };

  inline virtual struct yy_buffer_state* create (yyscan_t state_in, char* buffer_in, size_t size_in) { return scanner_.create (state_in, buffer_in, size_in); };
  inline virtual void destroy (yyscan_t state_in, struct yy_buffer_state*& buffer_inout) { scanner_.destroy (state_in, buffer_inout); };
  inline virtual void set (ParserInterfaceType* interfaceHandle_in) { scanner_.set (interfaceHandle_in); };

  inline virtual void dump_state () const { ACE_ASSERT (false); ACE_NOTSUP; };

 protected:
  ParserConfigurationType* configuration_;

  ACE_Message_Block*       fragment_;
  unsigned int             offset_; // parsed fragment bytes
  bool                     trace_;

  // parser
  ParserType               parser_;
//  ArgumentType            argument_;

  // scanner
  ScannerType              scanner_;

 private:
  ACE_UNIMPLEMENTED_FUNC (Stream_Module_Parser_T ())
  ACE_UNIMPLEMENTED_FUNC (Stream_Module_Parser_T (const Stream_Module_Parser_T&))
  ACE_UNIMPLEMENTED_FUNC (Stream_Module_Parser_T& operator= (const Stream_Module_Parser_T&))

  // helper methods
  bool scan_begin ();
  void scan_end ();

  // helper types
  struct MEMORY_BUFFER_T
   : std::streambuf
  {
    void set (char* buffer_in, unsigned int size_in) {
      this->setg (buffer_in, buffer_in, buffer_in + size_in);
    }
  };

  bool                     blockInParse_;
  bool                     isFirst_;

  struct yy_buffer_state*  buffer_;
  MEMORY_BUFFER_T          streamBuffer_;
  std::istream             stream_;
};

// include template definition
#include "stream_misc_parser.inl"

#endif