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

#ifndef STREAM_LIB_FFMPEG_COMMON_H
#define STREAM_LIB_FFMPEG_COMMON_H

#include <deque>

extern "C" {
#include "libavcodec/avcodec.h"
#include "libavutil/pixfmt.h"
#include "libavutil/rational.h"
#include "libavutil/samplefmt.h"
} // extern "C"

#include "common_image_common.h"

struct Stream_MediaFramework_FFMPEG_VideoMediaType
{
  Stream_MediaFramework_FFMPEG_VideoMediaType ()
   : codec (AV_CODEC_ID_NONE)
   , format (AV_PIX_FMT_NONE)
   , frameRate ()
   , resolution ()
  {
    frameRate.den = 1;
//    frameRate.num = 0;
  }

  enum AVCodecID            codec;
  enum AVPixelFormat        format;
  struct AVRational         frameRate;
  Common_Image_Resolution_t resolution;
};
typedef std::deque<struct Stream_MediaFramework_FFMPEG_VideoMediaType> Stream_MediaFramework_FFMPEG_VideoFormats_t;
typedef Stream_MediaFramework_FFMPEG_VideoFormats_t::iterator Stream_MediaFramework_FFMPEG_VideoFormatsIterator_t;

struct Stream_MediaFramework_FFMPEG_AudioMediaType
{
  Stream_MediaFramework_FFMPEG_AudioMediaType ()
   : codec (AV_CODEC_ID_NONE)
   , format (AV_SAMPLE_FMT_NONE)
   , channels (0)
   , sampleRate (0)
  {}

  enum AVCodecID      codec;
  enum AVSampleFormat format;
  unsigned int        channels;
  unsigned int        sampleRate;
};
typedef std::deque<struct Stream_MediaFramework_FFMPEG_AudioMediaType> Stream_MediaFramework_FFMPEG_AudioFormats_t;
typedef Stream_MediaFramework_FFMPEG_AudioFormats_t::iterator Stream_MediaFramework_FFMPEG_AudioFormatsIterator_t;
#endif
