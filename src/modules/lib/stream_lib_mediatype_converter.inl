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

#include "stream_macros.h"

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
#include "stream_dev_tools.h"
#endif // ACE_WIN32 || ACE_WIN64

template <typename MediaType
#if defined (ACE_WIN32) || defined (ACE_WIN64)
         >
#else
          ,typename SessionDataType>
#endif // ACE_WIN32 || ACE_WIN64
Stream_MediaFramework_MediaTypeConverter_T<MediaType
#if defined (ACE_WIN32) || defined (ACE_WIN64)
                                          >::Stream_MediaFramework_MediaTypeConverter_T ()
#else
                                          ,SessionDataType>::Stream_MediaFramework_MediaTypeConverter_T ()
#endif // ACE_WIN32 || ACE_WIN64
{
  STREAM_TRACE (ACE_TEXT ("Stream_MediaFramework_MediaTypeConverter_T::Stream_MediaFramework_MediaTypeConverter_T"));

}

#if defined (ACE_WIN32) || defined (ACE_WIN64)
template <typename MediaType>
void
Stream_MediaFramework_MediaTypeConverter_T<MediaType>::getMediaType (const struct _AMMediaType& mediaType_in,
                                                                     struct Stream_MediaFramework_FFMPEG_VideoMediaType& mediaType_out)
{
  STREAM_TRACE (ACE_TEXT ("Stream_MediaFramework_MediaTypeConverter_T::getMediaType"));

  ACE_OS::memset (&mediaType_out, 0, sizeof (struct Stream_MediaFramework_FFMPEG_VideoMediaType));

  mediaType_out.format =
      Stream_Module_Decoder_Tools::mediaSubTypeToAVPixelFormat (mediaType_in.subtype,
                                                                STREAM_MEDIAFRAMEWORK_DIRECTSHOW);
  mediaType_out.frameRate.num =
      Stream_MediaFramework_DirectShow_Tools::toFramerate (mediaType_in);
  mediaType_out.resolution =
      Stream_MediaFramework_DirectShow_Tools::toResolution (mediaType_in);
}

template <typename MediaType>
void
Stream_MediaFramework_MediaTypeConverter_T<MediaType>::getMediaType (const struct _AMMediaType& mediaType_in,
                                                                     IMFMediaType*& mediaType_out)
{
  STREAM_TRACE (ACE_TEXT ("Stream_MediaFramework_MediaTypeConverter_T::getMediaType"));

  if (mediaType_out)
  {
    mediaType_out->Release (); mediaType_out = NULL;
  } // end IF

  HRESULT result =
    MFInitMediaTypeFromAMMediaType (mediaType_out,
                                    &mediaType_in);
  if (unlikely (FAILED (result)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to MFInitMediaTypeFromAMMediaType(): \"%s\", aborting\n"),
                ACE_TEXT (Common_Error_Tools::errorToString (result).c_str ())));
    return;
  } // end IF
  ACE_ASSERT (mediaType_out);
}

template <typename MediaType>
void
Stream_MediaFramework_MediaTypeConverter_T<MediaType>::getMediaType (const IMFMediaType* mediaType_in,
                                                                     IMFMediaType*& mediaType_out)
{
  STREAM_TRACE (ACE_TEXT ("Stream_MediaFramework_MediaTypeConverter_T::getMediaType"));

  // sanity check(s)
  ACE_ASSERT (mediaType_in);

  if (mediaType_out)
  {
    mediaType_out->Release (); mediaType_out = NULL;
  } // end IF
  
  HRESULT result = MFCreateMediaType (&mediaType_out);
  if (unlikely (FAILED (result)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to MFCreateMediaType(): \"%s\", aborting\n"),
                ACE_TEXT (Common_Error_Tools::errorToString (result).c_str ())));
    return;
  } // end IF
  ACE_ASSERT (mediaType_out);
  result =
    const_cast<IMFMediaType*> (mediaType_in)->CopyAllItems (mediaType_out);
  if (unlikely (FAILED (result)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to IMFAttributes::CopyAllItems(): \"%s\", aborting\n"),
                ACE_TEXT (Common_Error_Tools::errorToString (result).c_str ())));
    mediaType_out->Release (); mediaType_out = NULL;
    return;
  } // end IF
}

template <typename MediaType>
void
Stream_MediaFramework_MediaTypeConverter_T<MediaType>::getMediaType (const IMFMediaType* mediaType_in,
                                                                     struct _AMMediaType& mediaType_inout)
{
  STREAM_TRACE (ACE_TEXT ("Stream_MediaFramework_MediaTypeConverter_T::getMediaType"));

  // sanity check(s)
  ACE_ASSERT (mediaType_in);

  // initialize return value(s)
  Stream_MediaFramework_DirectShow_Tools::free (mediaType_inout);

  struct _AMMediaType* media_type_p = NULL;
  HRESULT result =
    MFCreateAMMediaTypeFromMFMediaType (const_cast<IMFMediaType*> (mediaType_in),
                                        GUID_NULL,
                                        &media_type_p);
  if (unlikely (FAILED (result)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to MFCreateAMMediaTypeFromMFMediaType(): \"%s\", aborting\n"),
                ACE_TEXT (Common_Error_Tools::errorToString (result).c_str ())));
    return;
  } // end IF
  ACE_ASSERT (media_type_p);
  mediaType_inout = *media_type_p;

  CoTaskMemFree (media_type_p); media_type_p = NULL;
}

template <typename MediaType>
void
Stream_MediaFramework_MediaTypeConverter_T<MediaType>::getMediaType (const IMFMediaType* mediaType_in,
                                                                     struct Stream_MediaFramework_FFMPEG_VideoMediaType& mediaType_out)
{
  STREAM_TRACE (ACE_TEXT ("Stream_MediaFramework_MediaTypeConverter_T::getMediaType"));

  ACE_OS::memset (&mediaType_out, 0, sizeof (struct Stream_MediaFramework_FFMPEG_VideoMediaType));

  // sanity check(s)
  ACE_ASSERT (mediaType_in);

  struct _AMMediaType media_type_s;
  ACE_OS::memset (&media_type_s, 0, sizeof (struct _AMMediaType));
  getMediaType (mediaType_in,
                media_type_s);
  getMediaType (media_type_s,
                mediaType_out);

  Stream_MediaFramework_DirectShow_Tools::free (media_type_s);
}

template <typename MediaType>
void
Stream_MediaFramework_MediaTypeConverter_T<MediaType>::getMediaType (const struct Stream_MediaFramework_FFMPEG_VideoMediaType& mediaType_in,
                                                                     struct _AMMediaType& mediaType_out)
{
  STREAM_TRACE (ACE_TEXT ("Stream_MediaFramework_MediaTypeConverter_T::getMediaType"));

  Stream_MediaFramework_DirectShow_Tools::free (mediaType_out);

  struct _AMMediaType* media_type_p =
    Stream_MediaFramework_DirectShow_Tools::to (mediaType_in);
  ACE_ASSERT (media_type_p);
  mediaType_out = *media_type_p;
}
#else
//template <typename MediaType,
//          typename SessionDataType>
//void
//Stream_MediaFramework_MediaTypeConverter_T<MediaType,
//                                           SessionDataType>::getMediaType_impl (const struct Stream_MediaFramework_V4L_MediaType& mediaType_in,
//                                                                                struct Stream_MediaFramework_FFMPEG_VideoMediaType& mediaType_out)
//{
//  STREAM_TRACE (ACE_TEXT ("Stream_MediaFramework_MediaTypeConverter_T::getMediaType_impl"));

//  ACE_OS::memset (&mediaType_out, 0, sizeof (struct Stream_MediaFramework_FFMPEG_VideoMediaType));

//  mediaType_out.format =
//      Stream_Device_Tools::v4l2FormatToffmpegFormat (mediaType_in.format.pixelformat);
//  mediaType_out.resolution.width = mediaType_in.format.width;
//  mediaType_out.resolution.height = mediaType_in.format.height;
//  mediaType_out.frameRate.den = mediaType_in.frameRate.denominator;
//  mediaType_out.frameRate.num = mediaType_in.frameRate.numerator;
//}

template <typename MediaType,
          typename SessionDataType>
void
Stream_MediaFramework_MediaTypeConverter_T<MediaType,
                                           SessionDataType>::getMediaType (const struct Stream_MediaFramework_FFMPEG_VideoMediaType& mediaType_in,
                                                                           struct Stream_MediaFramework_LibCamera_MediaType& mediaType_out)
{
  STREAM_TRACE (ACE_TEXT ("Stream_MediaFramework_MediaTypeConverter_T::getMediaType"));

  mediaType_out.format =
      Stream_Device_Tools::ffmpegFormatToLibCameraFormat (mediaType_in.format);
  mediaType_out.resolution.width = mediaType_in.resolution.width;
  mediaType_out.resolution.height = mediaType_in.resolution.height;
  mediaType_out.frameRateNumerator = mediaType_in.frameRate.num;
  mediaType_out.frameRateDenominator = mediaType_in.frameRate.den;
}

template <typename MediaType,
          typename SessionDataType>
void
Stream_MediaFramework_MediaTypeConverter_T<MediaType,
                                           SessionDataType>::getMediaType (const struct Stream_MediaFramework_FFMPEG_VideoMediaType& mediaType_in,
                                                                           struct Stream_MediaFramework_V4L_MediaType& mediaType_out)
{
  STREAM_TRACE (ACE_TEXT ("Stream_MediaFramework_MediaTypeConverter_T::getMediaType"));

  ACE_OS::memset (&mediaType_out, 0, sizeof (struct Stream_MediaFramework_V4L_MediaType));

  mediaType_out.format.pixelformat =
      Stream_Device_Tools::ffmpegFormatToV4L2Format (mediaType_in.format);
  mediaType_out.format.width = mediaType_in.resolution.width;
  mediaType_out.format.height = mediaType_in.resolution.height;
  mediaType_out.frameRate.numerator = mediaType_in.frameRate.num;
  mediaType_out.frameRate.denominator = mediaType_in.frameRate.den;
}

template <typename MediaType,
          typename SessionDataType>
void
Stream_MediaFramework_MediaTypeConverter_T<MediaType,
                                           SessionDataType>::getMediaType (const struct Stream_MediaFramework_LibCamera_MediaType& mediaType_in,
                                                                           struct Stream_MediaFramework_FFMPEG_VideoMediaType& mediaType_out)
{
  STREAM_TRACE (ACE_TEXT ("Stream_MediaFramework_MediaTypeConverter_T::getMediaType"));

  ACE_OS::memset (&mediaType_out, 0, sizeof (struct Stream_MediaFramework_FFMPEG_VideoMediaType));

  mediaType_out.format =
      Stream_Device_Tools::libCameraFormatToffmpegFormat (mediaType_in.format);
  mediaType_out.resolution.width = mediaType_in.resolution.width;
  mediaType_out.resolution.height = mediaType_in.resolution.height;
  mediaType_out.frameRate.num = mediaType_in.frameRateNumerator;
  mediaType_out.frameRate.den = mediaType_in.frameRateDenominator;
}

template <typename MediaType,
          typename SessionDataType>
void
Stream_MediaFramework_MediaTypeConverter_T<MediaType,
                                           SessionDataType>::getMediaType (const struct Stream_MediaFramework_V4L_MediaType& mediaType_in,
                                                                           struct Stream_MediaFramework_FFMPEG_VideoMediaType& mediaType_out)
{
  STREAM_TRACE (ACE_TEXT ("Stream_MediaFramework_MediaTypeConverter_T::getMediaType"));

  ACE_OS::memset (&mediaType_out, 0, sizeof (struct Stream_MediaFramework_FFMPEG_VideoMediaType));

  mediaType_out.format =
      Stream_Device_Tools::v4l2FormatToffmpegFormat (mediaType_in.format.pixelformat);
  mediaType_out.resolution.width = mediaType_in.format.width;
  mediaType_out.resolution.height = mediaType_in.format.height;
  mediaType_out.frameRate.den = mediaType_in.frameRate.denominator;
  mediaType_out.frameRate.num = mediaType_in.frameRate.numerator;
}

#endif // ACE_WIN32 || ACE_WIN64
