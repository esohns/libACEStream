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

#ifndef STREAM_DEC_AVI_ENCODER_H
#define STREAM_DEC_AVI_ENCODER_H

#include <string>

#include "ace/Global_Macros.h"

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
#ifdef __cplusplus
extern "C"
{
#include "libavformat/avformat.h"
}
#endif
#endif

#include "common_time_common.h"

#include "stream_task_base_synch.h"

#include "stream_dec_common.h"

// forward declaration(s)
class ACE_Message_Block;
class ACE_Time_Value;
class Stream_IAllocator;

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
int
stream_decoder_aviencoder_libav_write_cb (void*,    // act
                                          uint8_t*, // buffer address
                                          int);     // buffer size
#endif

template <typename SynchStrategyType,
          typename TimePolicyType,
          typename SessionDataContainerType,
          typename SessionDataType>
class Stream_Decoder_AVIEncoder_ReaderTask_T
 : public ACE_Thru_Task<SynchStrategyType,
                        TimePolicyType>
{
 public:
  Stream_Decoder_AVIEncoder_ReaderTask_T ();
  virtual ~Stream_Decoder_AVIEncoder_ReaderTask_T ();

  virtual int put (ACE_Message_Block*,      // message
                   ACE_Time_Value* = NULL); // time

 private:
  typedef ACE_Thru_Task<SynchStrategyType,
                        TimePolicyType> inherited;

  ACE_UNIMPLEMENTED_FUNC (Stream_Decoder_AVIEncoder_ReaderTask_T (const Stream_Decoder_AVIEncoder_ReaderTask_T&))
  ACE_UNIMPLEMENTED_FUNC (Stream_Decoder_AVIEncoder_ReaderTask_T& operator= (const Stream_Decoder_AVIEncoder_ReaderTask_T&))

  // helper function(s)
  bool postProcessHeader (const std::string&); // file name
};

//////////////////////////////////////////

template <typename SynchStrategyType,
          typename TimePolicyType,
          ////////////////////////////////
          typename ConfigurationType,
          ////////////////////////////////
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType,
          ////////////////////////////////
          typename SessionDataContainerType,
          typename SessionDataType>
class Stream_Decoder_AVIEncoder_WriterTask_T
 : public Stream_TaskBaseSynch_T<SynchStrategyType,
                                 TimePolicyType,
                                 /////////
                                 ConfigurationType,
                                 /////////
                                 ControlMessageType,
                                 DataMessageType,
                                 SessionMessageType>
 //, public Stream_IModuleHandler_T<ConfigurationType>
{
 public:
  Stream_Decoder_AVIEncoder_WriterTask_T ();
  virtual ~Stream_Decoder_AVIEncoder_WriterTask_T ();

  //// override (part of) Stream_IModuleHandler_T
  virtual bool initialize (const ConfigurationType&);
  //virtual const ConfigurationType& get () const;

  // implement (part of) Stream_ITaskBase
  virtual void handleDataMessage (DataMessageType*&, // data message handle
                                  bool&);            // return value: pass message downstream ?
  virtual void handleSessionMessage (SessionMessageType*&, // session message handle
                                     bool&);               // return value: pass message downstream ?

 protected:
  SessionDataContainerType* sessionData_;

 private:
  typedef Stream_TaskBaseSynch_T<SynchStrategyType,
                                 TimePolicyType,
                                 /////////
                                 ConfigurationType,
                                 /////////
                                 ControlMessageType,
                                 DataMessageType,
                                 SessionMessageType> inherited;

  ACE_UNIMPLEMENTED_FUNC (Stream_Decoder_AVIEncoder_WriterTask_T (const Stream_Decoder_AVIEncoder_WriterTask_T&))
  ACE_UNIMPLEMENTED_FUNC (Stream_Decoder_AVIEncoder_WriterTask_T& operator= (const Stream_Decoder_AVIEncoder_WriterTask_T&))

  // helper methods
  DataMessageType* allocateMessage (unsigned int); // requested size
  bool generateHeader (ACE_Message_Block*); // message buffer handle
  bool generateIndex (ACE_Message_Block*); // message buffer handle

  // *NOTE*: the RIFF-AVI (storage) format (like many others) foresees a
  //         header that contains size fields with information about
  //         the length of the consecutive linearly structured bulk data.
  //         Note how in a (streaming) scenario continuously generating data,
  //         this information often is not available during initial processing
  //         and may therefore have to be filled in in a post-processing step
  //         after the stream ends, potentially requiring reparsing of (written)
  //         data. This means that, unless configuration data (duration[,
  //         format]) is supplied externally - either through session
  //         data/and or module configuration, the encoding process must be
  //         split into two separate phases (or distinct processing modules -
  //         more adequate for pipelined processing), in order to generate
  //         standard-compliant files. This implementation fills in the size
  //         information upon reception of completion event messages sent
  //         upstream by trailing modules of the processing stream (i.e. reader-
  //         side processing)
  bool                      isFirst_;
  bool                      isInitialized_;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
  AVFormatContext*          formatContext_;
#endif
};

// include template implementation
#include "stream_dec_avi_encoder.inl"

#endif
