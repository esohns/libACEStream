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

#ifndef STREAM_DEC_LIBAV_DECODER_T_H
#define STREAM_DEC_LIBAV_DECODER_T_H

#ifdef __cplusplus
extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavutil/pixfmt.h"
}
#endif /* __cplusplus */

#include "ace/Global_Macros.h"

#include "common_time_common.h"

#include "common_ui_common.h"

#include "stream_task_base_synch.h"

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
#include "stream_dev_tools.h"
#endif // ACE_WIN32 || ACE_WIN64

// forward declaration(s)
struct AVFrame;
struct SwsContext;
class ACE_Message_Block;
class Stream_IAllocator;

extern const char libacestream_default_dec_libav_decoder_module_name_string[];

enum AVPixelFormat stream_decoder_libav_getformat_cb (struct AVCodecContext*, const enum AVPixelFormat*);

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
class Stream_Decoder_LibAVDecoder_T
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
  Stream_Decoder_LibAVDecoder_T (ISTREAM_T*); // stream handle
#else
  Stream_Decoder_LibAVDecoder_T (typename inherited::ISTREAM_T*); // stream handle
#endif // ACE_WIN32 || ACE_WIN64
  virtual ~Stream_Decoder_LibAVDecoder_T ();

  // override (part of) Stream_IModuleHandler_T
  virtual bool initialize (const ConfigurationType&,
                           Stream_IAllocator*);

  // implement (part of) Stream_ITaskBase
  virtual void handleDataMessage (DataMessageType*&, // data message handle
                                  bool&);            // return value: pass message downstream ?
  virtual void handleSessionMessage (SessionMessageType*&, // session message handle
                                     bool&);               // return value: pass message downstream ?

 private:
  // convenient types
  typedef Stream_Decoder_LibAVDecoder_T<ACE_SYNCH_USE,
                                        TimePolicyType,
                                        ConfigurationType,
                                        ControlMessageType,
                                        DataMessageType,
                                        SessionMessageType,
                                        SessionDataContainerType> OWN_TYPE_T;

  ACE_UNIMPLEMENTED_FUNC (Stream_Decoder_LibAVDecoder_T ())
  ACE_UNIMPLEMENTED_FUNC (Stream_Decoder_LibAVDecoder_T (const Stream_Decoder_LibAVDecoder_T&))
  ACE_UNIMPLEMENTED_FUNC (Stream_Decoder_LibAVDecoder_T& operator= (const Stream_Decoder_LibAVDecoder_T&))

  // helper methods
  DataMessageType* allocateMessage (typename DataMessageType::MESSAGE_T, // message type
                                    unsigned int);                       // requested size
  template <typename MediaType> enum AVPixelFormat getFormat (const MediaType& mediaType_in) { return getFormat_impl (mediaType_in); }
//  inline enum AVPixelFormat getFormat_impl (enum AVPixelFormat format_in) { return format_in; }
  template <typename MediaType> Common_UI_Resolution_t getResolution (const MediaType& mediaType_in) { return getResolution_impl (mediaType_in); }
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  inline enum AVPixelFormat getFormat_impl (const struct _AMMediaType*& mediaType_in) { ACE_ASSERT (mediaType_in); return Stream_Module_Decoder_Tools::mediaSubTypeToAVPixelFormat (mediaType_in->subtype, STREAM_MEDIAFRAMEWORK_DIRECTSHOW); }
  inline enum AVPixelFormat getFormat_impl (const IMFMediaType*& mediaType_in) { ACE_ASSERT (mediaType_in); struct _GUID subtype_s = GUID_NULL; HRESULT result = const_cast<IMFMediaType*> (mediaType_in)->GetGUID (MF_MT_SUBTYPE, &subtype_s); ACE_ASSERT (SUCCEEDED (result) && !InlineIsEqualGUID (subtype_s, GUID_NULL)); return Stream_Module_Decoder_Tools::mediaSubTypeToAVPixelFormat (subtype_s, STREAM_MEDIAFRAMEWORK_MEDIAFOUNDATION); }
  inline Common_UI_Resolution_t getResolution_impl (const struct _AMMediaType*& mediaType_in) { ACE_ASSERT (mediaType_in); return Stream_MediaFramework_DirectShow_Tools::toResolution (*mediaType_in); }
  inline Common_UI_Resolution_t getResolution_impl (const IMFMediaType*& mediaType_in) { ACE_ASSERT (mediaType_in); return Stream_MediaFramework_MediaFoundation_Tools::toResolution (mediaType_in); }
#else
  inline enum AVPixelFormat getFormat_impl (const struct v4l2_pix_format& mediaType_in) { return Stream_Device_Tools::v4l2FormatToffmpegFormat (mediaType_in.pixelformat); }
  inline enum AVPixelFormat getFormat_impl (const struct Stream_MediaFramework_V4L_MediaType& mediaType_in) { return Stream_Device_Tools::v4l2FormatToffmpegFormat (mediaType_in.format.pixelformat); }
  inline Common_UI_Resolution_t getResolution_impl (const struct v4l2_pix_format& mediaType_in) { Common_UI_Resolution_t return_value; return_value.width = mediaType_in.width; return_value.height = mediaType_in.height; return return_value; }
  inline Common_UI_Resolution_t getResolution_impl (const struct Stream_MediaFramework_V4L_MediaType& mediaType_in) { Common_UI_Resolution_t return_value; return_value.width = mediaType_in.format.width; return_value.height = mediaType_in.format.height; return return_value; }
#endif // ACE_WIN32 || ACE_WIN64

  DataMessageType*       buffer_;
//  struct AVBuffer        buffer_;
//  struct AVBufferRef     bufferRef_;
  enum AVCodecID         codecId_;
  struct AVCodecContext* context_;
  enum AVPixelFormat     format_; // codec output-
  unsigned int           formatHeight_; // codec output-
  struct AVFrame*        frame_;
  unsigned int           frameSize_; // codec output-
  enum AVPixelFormat     outputFormat_; // output-
  unsigned int           outputFrameSize_; // output-
  int                    profile_; // codec-
  struct SwsContext*     transformContext_;

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  static char            paddingBuffer[AV_INPUT_BUFFER_PADDING_SIZE];
#else
  static char            paddingBuffer[AV_INPUT_BUFFER_PADDING_SIZE];
//  static char            paddingBuffer[FF_INPUT_BUFFER_PADDING_SIZE];
#endif // ACE_WIN32 || ACE_WIN64
};

// include template definition
#include "stream_dec_libav_decoder.inl"

#endif
