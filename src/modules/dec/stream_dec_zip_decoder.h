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

#ifndef STREAM_DEC_ZIP_DECODER_H
#define STREAM_DEC_ZIP_DECODER_H

#include "ace/Global_Macros.h"

#include "zlib.h"

#include "common_time_common.h"

#include "stream_task_base_synch.h"

#include "stream_dec_common.h"
//#include "stream_dec_exports.h"

//extern Stream_Dec_Export const char libacestream_default_dec_zip_decoder_module_name_string[];
extern const char libacestream_default_dec_zip_decoder_module_name_string[];

// forward declaration(s)
class ACE_Message_Block;
class Stream_IAllocator;

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          ////////////////////////////////
          typename ConfigurationType,
          ////////////////////////////////
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType,
          ////////////////////////////////
          typename SessionDataContainerType>
class Stream_Decoder_ZIPDecoder_T
 : public Stream_TaskBaseSynch_T<ACE_SYNCH_USE,
                                 TimePolicyType,
                                 ConfigurationType,
                                 ControlMessageType,
                                 DataMessageType,
                                 SessionMessageType,
                                 Stream_SessionId_t,
                                 enum Stream_ControlType,
                                 enum Stream_SessionMessageType,
                                 struct Stream_UserData>
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
                                 struct Stream_UserData> inherited;

 public:
  // *TODO*: on MSVC 2015u3 the accurate declaration does not compile
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  Stream_Decoder_ZIPDecoder_T (ISTREAM_T*); // stream handle
#else
  Stream_Decoder_ZIPDecoder_T (typename inherited::ISTREAM_T*); // stream handle
#endif // ACE_WIN32 || ACE_WIN64
  virtual ~Stream_Decoder_ZIPDecoder_T ();

  // override (part of) Stream_IModuleHandler_T
  virtual bool initialize (const ConfigurationType&,
                           Stream_IAllocator* = NULL);
  //virtual const ConfigurationType& get () const;

  // implement (part of) Stream_ITaskBase
  virtual void handleDataMessage (DataMessageType*&, // data message handle
                                  bool&);            // return value: pass message downstream ?
  virtual void handleSessionMessage (SessionMessageType*&, // session message handle
                                     bool&);               // return value: pass message downstream ?

 private:
  ACE_UNIMPLEMENTED_FUNC (Stream_Decoder_ZIPDecoder_T ())
  ACE_UNIMPLEMENTED_FUNC (Stream_Decoder_ZIPDecoder_T (const Stream_Decoder_ZIPDecoder_T&))
  ACE_UNIMPLEMENTED_FUNC (Stream_Decoder_ZIPDecoder_T& operator= (const Stream_Decoder_ZIPDecoder_T&))

  ACE_Message_Block*                        buffer_; // <-- continuation chain
  bool                                      crunchMessages_;
  enum Stream_Decoder_CompressionFormatType format_;
  struct z_stream_s                         stream_;
};

// include template definition
#include "stream_dec_zip_decoder.inl"

#endif
