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

#include "libv4l2.h"
#include "linux/videodev2.h"

#include "stream_macros.h"

#include "stream_dev_tools.h"

template <typename LockType,
          typename SessionMessageType,
          typename ProtocolMessageType,
          typename ConfigurationType,
          typename StreamStateType,
          typename SessionDataType,
          typename SessionDataContainerType,
          typename StatisticContainerType>
Stream_Module_CamSource_V4L_T<LockType,
                              SessionMessageType,
                              ProtocolMessageType,
                              ConfigurationType,
                              StreamStateType,
                              SessionDataType,
                              SessionDataContainerType,
                              StatisticContainerType>::Stream_Module_CamSource_V4L_T (bool isActive_in,
                                                                                      bool autoStart_in)
 : inherited (NULL,         // lock handle
              isActive_in,  // active ?
              autoStart_in, // auto-start ?
              true,         // *NOTE*: when working in 'passive' mode, enabling
                            //         this utilizes the calling thread. Note
                            //         that this potentially renders state
                            //         transitions during processing a tricky
                            //         affair, as the calling thread may be
                            //         holding the lock --> check carefully
              true)         // generate sesssion messages ?
 , configuration_ (NULL)
 , captureFileDescriptor_ (-1)
 , overlayFileDescriptor_ (-1)
 , close_ (true)
 , isInitialized_ (false)
{
  STREAM_TRACE (ACE_TEXT ("Stream_Module_CamSource_V4L_T::Stream_Module_CamSource_V4L_T"));

}

template <typename LockType,
          typename SessionMessageType,
          typename ProtocolMessageType,
          typename ConfigurationType,
          typename StreamStateType,
          typename SessionDataType,
          typename SessionDataContainerType,
          typename StatisticContainerType>
Stream_Module_CamSource_V4L_T<LockType,
                              SessionMessageType,
                              ProtocolMessageType,
                              ConfigurationType,
                              StreamStateType,
                              SessionDataType,
                              SessionDataContainerType,
                              StatisticContainerType>::~Stream_Module_CamSource_V4L_T ()
{
  STREAM_TRACE (ACE_TEXT ("Stream_Module_CamSource_V4L_T::~Stream_Module_CamSource_V4L_T"));

  int result = -1;

  if (close_)
    if (captureFileDescriptor_ != -1)
    {
      result = v4l2_close (captureFileDescriptor_);
      if (result == -1)
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to v4l2_close(%d): \"%m\", continuing\n"),
                    captureFileDescriptor_));
    } // end IF
  if (overlayFileDescriptor_ != -1)
  {
    result = v4l2_close (overlayFileDescriptor_);
    if (result == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to v4l2_close(%d): \"%m\", continuing\n"),
                  overlayFileDescriptor_));
  } // end IF
}

//template <typename SessionMessageType,
//          typename MessageType,
//          typename ModuleHandlerConfigurationType,
//          typename SessionDataType>
//void
//Stream_Module_CamSource_V4L_T<SessionMessageType,
//                            MessageType,
//                            ModuleHandlerConfigurationType,
//                            SessionDataType>::handleDataMessage (MessageType*& message_inout,
//                                                                 bool& passMessageDownstream_out)
//{
//  STREAM_TRACE (ACE_TEXT ("Stream_Module_CamSource_V4L_T::handleDataMessage"));
//
//  ssize_t bytes_written = -1;
//
//  // don't care (implies yes per default, if part of a stream)
//  ACE_UNUSED_ARG (passMessageDownstream_out);
//
//  // sanity check(s)
//  if (!connection_)
//  {
////    ACE_DEBUG ((LM_ERROR,
////                ACE_TEXT ("failed to open db connection, returning\n")));
//    return;
//  } // end IF
//}

template <typename LockType,
          typename SessionMessageType,
          typename ProtocolMessageType,
          typename ConfigurationType,
          typename StreamStateType,
          typename SessionDataType,
          typename SessionDataContainerType,
          typename StatisticContainerType>
void
Stream_Module_CamSource_V4L_T<LockType,
                              SessionMessageType,
                              ProtocolMessageType,
                              ConfigurationType,
                              StreamStateType,
                              SessionDataType,
                              SessionDataContainerType,
                              StatisticContainerType>::handleSessionMessage (SessionMessageType*& message_inout,
                                                                             bool& passMessageDownstream_out)
{
  STREAM_TRACE (ACE_TEXT ("Stream_Module_CamSource_V4L_T::handleSessionMessage"));

  int result = -1;

  // don't care (implies yes per default, if part of a stream)
  ACE_UNUSED_ARG (passMessageDownstream_out);

  // sanity check(s)
  ACE_ASSERT (isInitialized_);
  ACE_ASSERT (message_inout);

  const typename SessionMessageType::SESSION_DATA_T& session_data_container_r =
      message_inout->get ();
  SessionDataType& session_data_r =
      const_cast<SessionDataType&> (session_data_container_r.get ());
  switch (message_inout->type ())
  {
    case STREAM_SESSION_BEGIN:
    {
      // sanity check(s)
      ACE_ASSERT (configuration_->streamConfiguration);

      int toggle = 1;

      // step0: save current format as session data
      if (!Stream_Module_Device_Tools::getFormat (captureFileDescriptor_,
                                                  session_data_r.format))
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to Stream_Module_Device_Tools::getFormat(%d): \"%m\", aborting\n"),
                    captureFileDescriptor_));
        goto error;
      } // end IF

      // step1: fill buffer queue(s)
      if (captureFileDescriptor_ != -1)
        if (!Stream_Module_Device_Tools::initializeBuffers<ProtocolMessageType> (captureFileDescriptor_,
                                                                                 configuration_->method,
                                                                                 configuration_->buffers,
                                                                                 const_cast<ConfigurationType*> (configuration_)->bufferMap,
                                                                                 configuration_->streamConfiguration->messageAllocator))
        {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("failed to Stream_Module_Device_Tools::initializeBuffers(%d): \"%m\", aborting\n"),
                      captureFileDescriptor_));
          goto error;
        } // end IF

      // step2: start stream
      if (captureFileDescriptor_ != -1)
      {
        result = v4l2_ioctl (captureFileDescriptor_,
                             VIDIOC_STREAMON,
                             &toggle);
        if (result == -1)
        {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("failed to v4l2_ioctl(%d,%s): \"%m\", aborting\n"),
                      captureFileDescriptor_, ACE_TEXT ("VIDIOC_STREAMON")));
          goto error;
        } // end IF
      } // end IF
      if (overlayFileDescriptor_ != -1)
      {
        result = v4l2_ioctl (overlayFileDescriptor_,
                             VIDIOC_OVERLAY,
                             &toggle);
        if (result == -1)
        {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("failed to v4l2_ioctl(%d,%s): \"%m\", aborting\n"),
                      overlayFileDescriptor_, ACE_TEXT ("VIDIOC_OVERLAY")));
          goto error;
        } // end IF
      } // end IF

      break;

error:
      // *TODO*: remove type inference
      session_data_r.aborted = true;

      return;
    }
    case STREAM_SESSION_END:
    {
      // step1: empty buffer queue(s)
      if (captureFileDescriptor_ != -1)
        Stream_Module_Device_Tools::finalizeBuffers<ProtocolMessageType> (captureFileDescriptor_,
                                                                          configuration_->method,
                                                                          const_cast<ConfigurationType*> (configuration_)->bufferMap);

      // step2: stop stream
      int toggle = 0;
      if (captureFileDescriptor_ != -1)
      {
        toggle = 1; // ?
        result = v4l2_ioctl (captureFileDescriptor_,
                             VIDIOC_STREAMOFF,
                             &toggle);
        if (result == -1)
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("failed to v4l2_ioctl(%d,%u): \"%m\", continuing\n"),
                      captureFileDescriptor_, ACE_TEXT ("VIDIOC_STREAMOFF")));
      } // end IF
      if (overlayFileDescriptor_ != -1)
      {
        result = v4l2_ioctl (overlayFileDescriptor_,
                             VIDIOC_OVERLAY,
                             &toggle);
        if (result == -1)
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("failed to v4l2_ioctl(%d,%u): \"%m\", continuing\n"),
                      overlayFileDescriptor_, ACE_TEXT ("VIDIOC_OVERLAY")));
      } // end IF

      // step3: deallocate device buffer queue slots
      struct v4l2_requestbuffers request_buffers;
      ACE_OS::memset (&request_buffers, 0, sizeof (struct v4l2_requestbuffers));
      request_buffers.count = 0;
      request_buffers.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
      request_buffers.memory = configuration_->method;
      result = v4l2_ioctl (captureFileDescriptor_,
                           VIDIOC_REQBUFS,
                           &request_buffers);
      if (result == -1)
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to v4l2_ioctl(%d,%s): \"%m\", continuing\n"),
                    captureFileDescriptor_, ACE_TEXT ("VIDIOC_REQBUFS")));
      else
        ACE_DEBUG ((LM_DEBUG,
                    ACE_TEXT ("de-allocated %d device buffer slots...\n"),
                    configuration_->buffers));

//      if (captureFileDescriptor_ != -1)
//      {
//        result = v4l2_close (captureFileDescriptor_);
//        if (result == -1)
//          ACE_DEBUG ((LM_ERROR,
//                      ACE_TEXT ("failed to v4l2_close(%d): \"%m\", continuing\n"),
//                      captureFileDescriptor_));
//      } // end IF
//      if (overlayFileDescriptor_ != -1)
//      {
//        result = v4l2_close (overlayFileDescriptor_);
//        if (result == -1)
//          ACE_DEBUG ((LM_ERROR,
//                      ACE_TEXT ("failed to v4l2_close(%d): \"%m\", continuing\n"),
//                      overlayFileDescriptor_));
//      } // end IF

      break;
    }
    default:
      break;
  } // end SWITCH
}

template <typename LockType,
          typename SessionMessageType,
          typename ProtocolMessageType,
          typename ConfigurationType,
          typename StreamStateType,
          typename SessionDataType,
          typename SessionDataContainerType,
          typename StatisticContainerType>
bool
Stream_Module_CamSource_V4L_T<LockType,
                              SessionMessageType,
                              ProtocolMessageType,
                              ConfigurationType,
                              StreamStateType,
                              SessionDataType,
                              SessionDataContainerType,
                              StatisticContainerType>::collect (StatisticContainerType& data_out)
{
  STREAM_TRACE (ACE_TEXT ("Stream_Module_CamSource_V4L_T::collect"));

  // sanity check(s)
  ACE_ASSERT (isInitialized_);

  // step0: initialize container
//  data_out.dataMessages = 0;
//  data_out.droppedMessages = 0;
//  data_out.bytes = 0.0;
  data_out.timeStamp = COMMON_TIME_NOW;

  // *TODO*: collect socket statistics information
  //         (and propagate it downstream ?)

  // step1: send the container downstream
  if (!putStatisticMessage (data_out)) // data container
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to putStatisticMessage(), aborting\n")));
    return false;
  } // end IF

  return true;
}

template <typename LockType,
          typename SessionMessageType,
          typename ProtocolMessageType,
          typename ConfigurationType,
          typename StreamStateType,
          typename SessionDataType,
          typename SessionDataContainerType,
          typename StatisticContainerType>
void
Stream_Module_CamSource_V4L_T<LockType,
                              SessionMessageType,
                              ProtocolMessageType,
                              ConfigurationType,
                              StreamStateType,
                              SessionDataType,
                              SessionDataContainerType,
                              StatisticContainerType>::report () const
{
  STREAM_TRACE (ACE_TEXT ("Stream_Module_CamSource_V4L_T::report"));

  ACE_ASSERT (false);
  ACE_NOTSUP;

  ACE_NOTREACHED (return;)
}

template <typename LockType,
          typename SessionMessageType,
          typename ProtocolMessageType,
          typename ConfigurationType,
          typename StreamStateType,
          typename SessionDataType,
          typename SessionDataContainerType,
          typename StatisticContainerType>
bool
Stream_Module_CamSource_V4L_T<LockType,
                              SessionMessageType,
                              ProtocolMessageType,
                              ConfigurationType,
                              StreamStateType,
                              SessionDataType,
                              SessionDataContainerType,
                              StatisticContainerType>::initialize (const ConfigurationType& configuration_in)
{
  STREAM_TRACE (ACE_TEXT ("Stream_Module_CamSource_V4L_T::initialize"));

  int result = -1;

  configuration_ = &configuration_in;

  // sanity check(s)
  // *TODO*: remove type inference
  ACE_ASSERT (configuration_->streamConfiguration);

  if (isInitialized_)
  {
    if (close_)
      if (captureFileDescriptor_ != -1)
      {
        result = v4l2_close (captureFileDescriptor_);
        if (result == -1)
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("failed to v4l2_close(%d): \"%m\", continuing\n"),
                      captureFileDescriptor_));
        captureFileDescriptor_ = -1;
      } // end IF
    if (overlayFileDescriptor_ != -1)
    {
      result = v4l2_close (overlayFileDescriptor_);
      if (result == -1)
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to v4l2_close(%d): \"%m\", continuing\n"),
                    overlayFileDescriptor_));
      overlayFileDescriptor_ = -1;
    } // end IF

    close_ = true;
    isInitialized_ = false;
  } // end IF

  // *NOTE*: use O_NONBLOCK with a reactor (v4l2_select()) or proactor
  //         (v4l2_poll())
  // *TODO*: support O_NONBLOCK
  //  int open_mode = O_RDONLY;
  int open_mode =
      ((configuration_->method == V4L2_MEMORY_MMAP) ? O_RDWR : O_RDONLY);
  captureFileDescriptor_ = configuration_->fileDescriptor;
  if (captureFileDescriptor_ != -1)
    close_ = false;
  else
  {
    captureFileDescriptor_ = v4l2_open (configuration_->device.c_str (),
                                        open_mode);
    if (captureFileDescriptor_ == -1)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to v4l2_open(\"%s\",%u): \"%m\", aborting\n"),
                  ACE_TEXT (configuration_->device.c_str ()), open_mode));
      return false;
    } // end IF
  } // end ELSE
  ACE_ASSERT (captureFileDescriptor_ != -1);
  if (configuration_->window &&
      Stream_Module_Device_Tools::canOverlay (captureFileDescriptor_))
  {
    overlayFileDescriptor_ = v4l2_open (configuration_->device.c_str (),
                                        open_mode);
    if (overlayFileDescriptor_ == -1)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to v4l2_open(\"%s\",%u): \"%m\", aborting\n"),
                  ACE_TEXT (configuration_->device.c_str ()), open_mode));
      goto error;
    } // end IF
  } // end IF
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("opened v4l device \"%s\"...\n"),
              ACE_TEXT (configuration_->device.c_str ())));

  if (Stream_Module_Device_Tools::canStream (captureFileDescriptor_))
    if (!Stream_Module_Device_Tools::initializeCapture (captureFileDescriptor_,
                                                        configuration_->method,
                                                        const_cast<ConfigurationType*> (configuration_)->buffers))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to Stream_Module_Device_Tools::initializeCapture(%d): \"%m\", aborting\n"),
                  captureFileDescriptor_));
      goto error;
    } // end IF
  if (overlayFileDescriptor_ != -1)
  {
    if (!Stream_Module_Device_Tools::initializeOverlay (overlayFileDescriptor_,
                                                        *configuration_->window))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to Stream_Module_Device_Tools::initializeOverlay(%d): \"%m\", aborting\n"),
                  overlayFileDescriptor_));
      goto error;
    } // end IF
  } // end IF

  isInitialized_ = inherited::initialize (configuration_in);
  if (!isInitialized_)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Stream_HeadModuleTaskBase_T::initialize(): \"%m\", aborting\n")));
    goto error;
  } // end IF

  return isInitialized_;

error:
  if (captureFileDescriptor_ != -1)
  {
    result = v4l2_close (captureFileDescriptor_);
    if (result == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to v4l2_close(%d): \"%m\", continuing\n"),
                  captureFileDescriptor_));
    captureFileDescriptor_ = -1;
  } // end IF
  if (overlayFileDescriptor_ != -1)
  {
    result = v4l2_close (overlayFileDescriptor_);
    if (result == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to v4l2_close(%d): \"%m\", continuing\n"),
                  overlayFileDescriptor_));
    overlayFileDescriptor_ = -1;
  } // end IF

  return false;
}

template <typename LockType,
          typename SessionMessageType,
          typename ProtocolMessageType,
          typename ConfigurationType,
          typename StreamStateType,
          typename SessionDataType,
          typename SessionDataContainerType,
          typename StatisticContainerType>
int
Stream_Module_CamSource_V4L_T<LockType,
                              SessionMessageType,
                              ProtocolMessageType,
                              ConfigurationType,
                              StreamStateType,
                              SessionDataType,
                              SessionDataContainerType,
                              StatisticContainerType>::svc (void)
{
  STREAM_TRACE (ACE_TEXT ("Stream_Dev_Cam_Source_V4L_T::svc"));

  // sanity check(s)
  ACE_ASSERT (isInitialized_);

  int result = -1;
  int result_2 = -1;
  ACE_Message_Block* message_block_p = NULL;
  ACE_Time_Value no_wait = COMMON_TIME_NOW;
  int message_type = -1;
  bool finished = false;
  bool stop_processing = false;
  struct v4l2_buffer buffer;
  ACE_OS::memset (&buffer, 0, sizeof (struct v4l2_buffer));
  buffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  buffer.memory = V4L2_MEMORY_USERPTR;
  INDEX2BUFFER_MAP_ITERATOR_T iterator;

  // step1: start processing data...
//   ACE_DEBUG ((LM_DEBUG,
//               ACE_TEXT ("entering processing loop...\n")));
  do
  {
    message_block_p = NULL;
    result = inherited::getq (message_block_p,
                              &no_wait);
    if (result == 0)
    {
      ACE_ASSERT (message_block_p);
      message_type = message_block_p->msg_type ();
      switch (message_type)
      {
        case ACE_Message_Block::MB_STOP:
        {
          // clean up
          message_block_p->release ();
          message_block_p = NULL;

          // *NOTE*: when close()d manually (i.e. user abort), 'finished' will not
          //         have been set at this stage

          // signal the controller ?
          if (!finished)
          {
            ACE_DEBUG ((LM_DEBUG,
                        ACE_TEXT ("session aborted...\n")));

            finished = true;
            inherited::finished (); // *NOTE*: enqueues STREAM_SESSION_END
          } // end IF
          continue;
        }
        default:
          break;
      } // end SWITCH

      // process
      inherited::handleMessage (message_block_p,
                                stop_processing);
      if (stop_processing)
      {
//        SessionMessageType* session_message_p = NULL;
//        // downcast message
//        session_message_p = dynamic_cast<SessionMessageType*> (message_block_p);
//        if (!session_message_p)
//        {
//          if (inherited::module ())
//            ACE_DEBUG ((LM_ERROR,
//                        ACE_TEXT ("%s: dynamic_cast<SessionMessageType*>(0x%@) failed (type was: %d), aborting\n"),
//                        inherited::name (),
//                        message_block_p,
//                        message_type));
//          else
//            ACE_DEBUG ((LM_ERROR,
//                        ACE_TEXT ("dynamic_cast<SessionMessageType*>(0x%@) failed (type was: %d), aborting\n"),
//                        message_block_p,
//                        message_type));
//          break;
//        } // end IF
//        if (session_message_p->type () == STREAM_SESSION_END)
          result_2 = 0; // success
        goto done; // finished processing
      } // end IF
    } // end IF
    else if (result == -1)
    {
      int error = ACE_OS::last_error ();
      if (error != EWOULDBLOCK) // Win32: 10035
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ACE_Task::getq(): \"%m\", aborting\n")));

        // signal the controller ?
        if (!finished)
        {
          finished = true;
          inherited::finished ();
        } // end IF

        break;
      } // end IF
    } // end IF

    // *NOTE*: blocks until a frame has been written by the device
    result = v4l2_ioctl (captureFileDescriptor_,
                         VIDIOC_DQBUF,
                         &buffer);
    if (result == -1)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to v4l2_ioctl(%d,%s): \"%m\", aborting\n"),
                  captureFileDescriptor_, ACE_TEXT ("VIDIOC_DQBUF")));
      break;
    } // end IF
    if (buffer.flags & V4L2_BUF_FLAG_ERROR)
      ACE_DEBUG ((LM_WARNING,
                  ACE_TEXT ("%s: streaming error (fd: %d, index: %d), continuing\n"),
                  inherited::mod_->name (),
                  captureFileDescriptor_, buffer.index));

//    // sanity check(s)
//    ACE_ASSERT (buffer.reserved);
//    message_block_p = reinterpret_cast<ACE_Message_Block*> (buffer.reserved);
    iterator = configuration_->bufferMap.find (buffer.index);
    ACE_ASSERT (iterator != configuration_->bufferMap.end ());
    message_block_p = (*iterator).second;

    result = inherited::put_next (message_block_p, NULL);
    if (result == -1)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_Task::put_next(): \"%m\", aborting\n")));

      // clean up
      message_block_p->release ();

      break;
    } // end IF

    buffer.reserved = 0;
  } while (true);

done:
  return result_2;
}

template <typename LockType,
          typename SessionMessageType,
          typename ProtocolMessageType,
          typename ConfigurationType,
          typename StreamStateType,
          typename SessionDataType,
          typename SessionDataContainerType,
          typename StatisticContainerType>
ProtocolMessageType*
Stream_Module_CamSource_V4L_T<LockType,
                              SessionMessageType,
                              ProtocolMessageType,
                              ConfigurationType,
                              StreamStateType,
                              SessionDataType,
                              SessionDataContainerType,
                              StatisticContainerType>::allocateMessage (unsigned int requestedSize_in)
{
  STREAM_TRACE (ACE_TEXT ("Stream_Module_CamSource_V4L_T::allocateMessage"));

  // sanity check(s)
  ACE_ASSERT (inherited::configuration_);
  // *TODO*: remove type inference
  ACE_ASSERT (inherited::configuration_->streamConfiguration);

  // initialize return value(s)
  ProtocolMessageType* message_out = NULL;

  if (inherited::configuration_->streamConfiguration->messageAllocator)
  {
    try
    {
      // *TODO*: remove type inference
      message_out =
          static_cast<ProtocolMessageType*> (inherited::configuration_->streamConfiguration->messageAllocator->malloc (requestedSize_in));
    }
    catch (...)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("caught exception in Stream_IAllocator::malloc(%u), continuing\n"),
                  requestedSize_in));
      message_out = NULL;
    }
  } // end IF
  else
  {
    ACE_NEW_NORETURN (message_out,
                      ProtocolMessageType (requestedSize_in));
  } // end ELSE
  if (!message_out)
  {
    ACE_DEBUG ((LM_CRITICAL,
                ACE_TEXT ("failed to Stream_IAllocator::malloc(%u), aborting\n"),
                requestedSize_in));
  } // end IF

  return message_out;
}

template <typename LockType,
          typename SessionMessageType,
          typename ProtocolMessageType,
          typename ConfigurationType,
          typename StreamStateType,
          typename SessionDataType,
          typename SessionDataContainerType,
          typename StatisticContainerType>
bool
Stream_Module_CamSource_V4L_T<LockType,
                              SessionMessageType,
                              ProtocolMessageType,
                              ConfigurationType,
                              StreamStateType,
                              SessionDataType,
                              SessionDataContainerType,
                              StatisticContainerType>::putStatisticMessage (const StatisticContainerType& statisticData_in) const
{
  STREAM_TRACE (ACE_TEXT ("Stream_Module_CamSource_V4L_T::putStatisticMessage"));

  // sanity check(s)
  ACE_ASSERT (inherited::configuration_);
  ACE_ASSERT (inherited::sessionData_);
  // *TODO*: remove type inference
  ACE_ASSERT (inherited::configuration_->streamConfiguration);

  // step1: update session state
  SessionDataType& session_data_r =
      const_cast<SessionDataType&> (inherited::sessionData_->get ());
  // *TODO*: remove type inferences
  session_data_r.currentStatistic = statisticData_in;

  // *TODO*: attach stream state information to the session data

//  // step2: create session data object container
//  SessionDataContainerType* session_data_p = NULL;
//  ACE_NEW_NORETURN (session_data_p,
//                    SessionDataContainerType (inherited::sessionData_,
//                                              false));
//  if (!session_data_p)
//  {
//    ACE_DEBUG ((LM_CRITICAL,
//                ACE_TEXT ("failed to allocate SessionDataContainerType: \"%m\", aborting\n")));
//    return false;
//  } // end IF

  // step3: send the statistic data downstream
//  // *NOTE*: fire-and-forget session_data_p here
  // *TODO*: remove type inference
  return inherited::putSessionMessage (STREAM_SESSION_STATISTIC,
                                       *inherited::sessionData_,
                                       inherited::configuration_->streamConfiguration->messageAllocator);
}