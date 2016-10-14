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

#ifndef STREAM_MODULE_VIS_GTK_CAIRO_SPECTRUM_ANALYZER_H
#define STREAM_MODULE_VIS_GTK_CAIRO_SPECTRUM_ANALYZER_H

#include <functional>
#include <random>

#include <ace/config-lite.h>
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include <mfapi.h>
#include <mfobjects.h>
#endif

#include <ace/Global_Macros.h>
#include <ace/Synch_Traits.h>

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include <gl/GL.h>
#else
#include <GL/gl.h>
#endif
#include <gtk/gtk.h>
#if defined (GTKGL_SUPPORT)
#if GTK_CHECK_VERSION (3,0,0)
#if GTK_CHECK_VERSION (3,16,0)
#else
#include <gtkgl/gdkgl.h>
#endif
#else
#if defined (GTKGLAREA_SUPPORT)
#include <gtkgl/gdkgl.h>
#else
#include <gtk/gtkgl.h> // gtkglext
#endif
#endif
#endif

#include "common_icounter.h"
#include "common_inotify.h"
#include "common_time_common.h"

#include "common_math_fft.h"

#include "stream_resetcounterhandler.h"
#include "stream_task_base_synch.h"

#include "stream_misc_statistic_analysis.h"

enum Stream_Module_Visualization_SpectrumAnalyzer2DMode
{
  STREAM_MODULE_VIS_SPECTRUMANALYZER_2DMODE_OSCILLOSCOPE = 0,
  STREAM_MODULE_VIS_SPECTRUMANALYZER_2DMODE_SPECTRUM,
  ////////////////////////////////////////
  STREAM_MODULE_VIS_SPECTRUMANALYZER_2DMODE_MAX,
  STREAM_MODULE_VIS_SPECTRUMANALYZER_2DMODE_INVALID
};
enum Stream_Module_Visualization_SpectrumAnalyzer3DMode
{
  STREAM_MODULE_VIS_SPECTRUMANALYZER_3DMODE_DEFAULT = 0,
  ////////////////////////////////////////
  STREAM_MODULE_VIS_SPECTRUMANALYZER_3DMODE_MAX,
  STREAM_MODULE_VIS_SPECTRUMANALYZER_3DMODE_INVALID
};

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          ////////////////////////////////
          typename ConfigurationType,
          ////////////////////////////////
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType,
          ////////////////////////////////
          typename SessionDataType,
          typename SessionDataContainerType>
class Stream_Module_Vis_GTK_Cairo_SpectrumAnalyzer_T
 : public Stream_TaskBaseSynch_T<ACE_SYNCH_USE,
                                 TimePolicyType,
                                 ConfigurationType,
                                 ControlMessageType,
                                 DataMessageType,
                                 SessionMessageType,
                                 Stream_SessionId_t,
                                 Stream_SessionMessageType>
 , public Common_Math_FFT
 , public Common_IDispatch_T<Stream_Module_StatisticAnalysis_Event>
 , public Common_ICounter
{
 public:
  Stream_Module_Vis_GTK_Cairo_SpectrumAnalyzer_T ();
  virtual ~Stream_Module_Vis_GTK_Cairo_SpectrumAnalyzer_T ();

  virtual bool initialize (const ConfigurationType&);

  // implement (part of) Stream_ITaskBase_T
  virtual void handleDataMessage (DataMessageType*&, // data message handle
                                  bool&);            // return value: pass message downstream ?
  virtual void handleSessionMessage (SessionMessageType*&, // session message handle
                                     bool&);               // return value: pass message downstream ?

 private:
  typedef Stream_TaskBaseSynch_T<ACE_SYNCH_USE,
                                 TimePolicyType,
                                 ConfigurationType,
                                 ControlMessageType,
                                 DataMessageType,
                                 SessionMessageType,
                                 Stream_SessionId_t,
                                 Stream_SessionMessageType> inherited;
  typedef Common_Math_FFT inherited2;

  ACE_UNIMPLEMENTED_FUNC (Stream_Module_Vis_GTK_Cairo_SpectrumAnalyzer_T (const Stream_Module_Vis_GTK_Cairo_SpectrumAnalyzer_T&))
  ACE_UNIMPLEMENTED_FUNC (Stream_Module_Vis_GTK_Cairo_SpectrumAnalyzer_T& operator= (const Stream_Module_Vis_GTK_Cairo_SpectrumAnalyzer_T&))

  virtual int svc (void);

#if defined (GTK_MAJOR_VERSION) && (GTK_MAJOR_VERSION >= 3)
  bool initialize_Cairo (GdkWindow*,
                         cairo_t*&,
                         cairo_surface_t*&);
#else
  bool initialize_Cairo (GdkWindow*,
                         cairo_t*&,
                         GdkPixbuf*&);
#endif
  virtual void dispatch (const Stream_Module_StatisticAnalysis_Event&);
  // implement Common_ICounter (triggers frame rendering)
  virtual void reset ();

  void update ();

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  // *NOTE*: callers must free the return value !
  template <typename FormatType> AM_MEDIA_TYPE* getFormat (const FormatType format_in) { return getFormat_impl (format_in); }
  AM_MEDIA_TYPE* getFormat_impl (const struct _AMMediaType*); // return value: media type handle
  AM_MEDIA_TYPE* getFormat_impl (const IMFMediaType*); // return value: media type handle
#endif

  cairo_t*                                                 cairoContext2D_;
#if GTK_CHECK_VERSION (3,0,0)
  cairo_surface_t*                                         cairoSurface2D_;
#else
  ACE_SYNCH_MUTEX_T*                                       lock_;
  GdkPixbuf*                                               pixelBuffer2D_;
#endif
#if defined (GTKGL_SUPPORT)
#if GTK_CHECK_VERSION (3,0,0)
  GdkRGBA                                                  backgroundColor_;
#if GTK_CHECK_VERSION (3,16,0)
  GdkGLContext*                                            OpenGLContext_;
#else
  GglaContext*                                             OpenGLContext_;
  GdkWindow*                                               OpenGLWindow_;
#endif
#else
  GdkColor                                                 backgroundColor_;
#if defined (GTKGLAREA_SUPPORT)
  GdkGLContext*                                            OpenGLContext_;
  GdkWindow*                                               OpenGLWindow_;
#else
  GdkGLContext*                                            OpenGLContext_;
  GdkGLDrawable*                                           OpenGLWindow_;
#endif
#endif
  GLuint                                                   OpenGLTextureID_;
#endif
  double                                                   channelFactor_;
  double                                                   scaleFactorX_;
  double                                                   scaleFactorY_;
  int                                                      height_;
  int                                                      width_;

  enum Stream_Module_Visualization_SpectrumAnalyzer2DMode* mode2D_;
#if defined (GTKGL_SUPPORT)
  enum Stream_Module_Visualization_SpectrumAnalyzer3DMode* mode3D_;
#endif

  Stream_ResetCounterHandler                               renderHandler_;
  long                                                     renderHandlerTimerID_;

  SampleIterator                                           sampleIterator_;

  // random number generator
  std::uniform_int_distribution<int>                       randomDistribution_;
  std::default_random_engine                               randomEngine_;
  std::function<int ()>                                    randomGenerator_;
};

// include template definition
#include "stream_vis_gtk_cairo_spectrum_analyzer.inl"

#endif
