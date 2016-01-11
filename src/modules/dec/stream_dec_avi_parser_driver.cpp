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
#include "stdafx.h"

#include "stream_dec_avi_parser_driver.h"

#include "ace/Log_Msg.h"
#include "ace/Message_Block.h"
#include "ace/Message_Queue.h"

#include "stream_common.h"
#include "stream_macros.h"

#include "stream_dec_avi_parser.h"
#include "stream_dec_defines.h"
#include "stream_dec_riff_scanner.h"

Stream_Decoder_AVIParserDriver::Stream_Decoder_AVIParserDriver (bool traceScanning_in,
                                                                bool traceParsing_in)
 : chunks_ ()
 , finished_ (false)
 , fragment_ (NULL)
 , offset_ (0)
 , frameSize_ (NULL)
 , trace_ (traceParsing_in)
 , scannerState_ (NULL)
 , bufferState_ (NULL)
 , messageQueue_ (NULL)
 , useYYScanBuffer_ (STREAM_DECODER_DEFAULT_USE_YY_SCAN_BUFFER)
 , initialized_ (false)
{
  STREAM_TRACE (ACE_TEXT ("Stream_Decoder_AVIParserDriver::Stream_Decoder_AVIParserDriver"));

  if (RIFF_Scanner_lex_init_extra (this, &scannerState_))
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to yylex_init_extra: \"%m\", continuing\n")));
  ACE_ASSERT (scannerState_);
  //parser_.set (scannerState_);

  // trace ?
  RIFF_Scanner_set_debug ((traceScanning_in ? 1 : 0),
                          scannerState_);
#if YYDEBUG
  //parser_.set_debug_level (traceParsing_in ? 1
  //                                         : 0); // binary (see bison manual)
  yydebug = (traceParsing_in ? 1 : 0);
#endif
}

Stream_Decoder_AVIParserDriver::~Stream_Decoder_AVIParserDriver ()
{
  STREAM_TRACE (ACE_TEXT ("Stream_Decoder_AVIParserDriver::~Stream_Decoder_AVIParserDriver"));

  // finalize lex scanner
  if (RIFF_Scanner_lex_destroy (scannerState_))
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to yylex_destroy: \"%m\", continuing\n")));
}

void
Stream_Decoder_AVIParserDriver::initialize (unsigned int& frameSize_in,
                                            bool traceScanning_in,
                                            bool traceParsing_in,
                                            ACE_Message_Queue_Base* messageQueue_in,
                                            bool useYYScanBuffer_in)
{
  STREAM_TRACE (ACE_TEXT ("Stream_Decoder_AVIParserDriver::initialize"));

  if (initialized_)
  {
    finished_ = false;
    if (fragment_)
    {
      fragment_->release ();
      fragment_ = NULL;
    } // end IF
    offset_ = 0;

    frameSize_ = NULL;

    initialized_ = false;
  } // end IF

  // set parse target
  frameSize_ = &frameSize_in;

  // trace ?
  RIFF_Scanner_set_debug ((traceScanning_in ? 1 : 0),
                          scannerState_);
#if YYDEBUG
  //parser_.set_debug_level (traceParsing_in ? 1
  //                                         : 0); // binary (see bison manual)
  yydebug = (traceParsing_in ? 1 : 0);
#endif
  messageQueue_ = messageQueue_in;
  useYYScanBuffer_ = useYYScanBuffer_in;

  // OK
  initialized_ = true;
}

bool
Stream_Decoder_AVIParserDriver::parse (ACE_Message_Block* data_in)
{
  STREAM_TRACE (ACE_TEXT ("Stream_Decoder_AVIParserDriver::parse"));

  // sanity check(s)
  ACE_ASSERT (initialized_);
  ACE_ASSERT (data_in);

  // start with the first fragment...
  fragment_ = data_in;

  // *NOTE*: parse ALL available message fragments
  // *TODO*: yyrestart(), yy_create_buffer/yy_switch_to_buffer, YY_INPUT...
  int result = -1;
//   do
//   { // initialize scan buffer
    if (!scan_begin ())
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to Stream_Decoder_AVIParserDriver::scan_begin(), aborting\n")));

      // clean up
      fragment_ = NULL;

//       break;
      return false;
    } // end IF

    // initialize scanner
    RIFF_Scanner_set_column (1, scannerState_);
    RIFF_Scanner_set_lineno (1, scannerState_);
    int debug_level = 0;
#if YYDEBUG
    //debug_level = parser_.debug_level ();
    debug_level = yydebug;
#endif
    ACE_UNUSED_ARG (debug_level);

    // parse data fragment
    try
    {
      //result = parser_.parse ();
      result = yyparse (this, scannerState_);
    }
    catch (...)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("caught exception in ::yyparse(), continuing\n")));
    }
    switch (result)
    {
      case 0:
        break; // done
      case 1:
      default:
      { // *NOTE*: need more data
//        ACE_DEBUG ((LM_DEBUG,
//                    ACE_TEXT ("failed to parse message fragment (result was: %d), aborting\n"),
//                    result));

//        // debug info
//        if (debug_level)
//        {
//          try
//          {
//            record_->dump_state ();
//          }
//          catch (...)
//          {
//            ACE_DEBUG ((LM_ERROR,
//                        ACE_TEXT ("caught exception in Common_IDumpState::dump_state(), continuing\n")));
//          }
//        } // end IF

        break;
      }
    } // end SWITCH

    // finalize buffer/scanner
    scan_end ();
//   } while (fragment_);

  return (result == 0);
}

bool
Stream_Decoder_AVIParserDriver::getDebugScanner () const
{
  STREAM_TRACE (ACE_TEXT ("Stream_Decoder_AVIParserDriver::getDebugScanner"));

  return (RIFF_Scanner_get_debug (scannerState_) != 0);
}

bool
Stream_Decoder_AVIParserDriver::switchBuffer ()
{
  STREAM_TRACE (ACE_TEXT ("Stream_Decoder_AVIParserDriver::switchBuffer"));

  // sanity check(s)
  ACE_ASSERT (scannerState_);

  if (fragment_->cont () == NULL)
    wait (); // <-- wait for data
  if (!fragment_->cont ())
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("no data after Stream_Decoder_AVIParserDriver::wait(), aborting\n")));
    return false;
  } // end IF
  fragment_ = fragment_->cont ();

  // switch to the next fragment

  // clean state
  scan_end ();

  // initialize next buffer

  // append the "\0\0"-sequence, as required by flex
  ACE_ASSERT (fragment_->capacity () - fragment_->length () >= STREAM_DECODER_FLEX_BUFFER_BOUNDARY_SIZE);
  *(fragment_->wr_ptr ()) = YY_END_OF_BUFFER_CHAR;
  *(fragment_->wr_ptr () + 1) = YY_END_OF_BUFFER_CHAR;
  // *NOTE*: DO NOT adjust the write pointer --> length() must stay as it was

  if (!scan_begin ())
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Stream_Decoder_AVIParserDriver::scan_begin(), aborting\n")));
    return false;
  } // end IF

//  ACE_DEBUG ((LM_DEBUG,
//              ACE_TEXT ("switched input buffers...\n")));

  return true;
}

void
Stream_Decoder_AVIParserDriver::wait ()
{
  STREAM_TRACE (ACE_TEXT ("Stream_Decoder_AVIParserDriver::wait"));

  int result = -1;
  ACE_Message_Block* message_block_p = NULL;

  // *IMPORTANT NOTE*: 'this' is the parser thread currently blocked in yylex()

  // sanity check(s)
  if (!messageQueue_)
    return;

  // 1. wait for data
  do
  {
    result = messageQueue_->dequeue_head (message_block_p,
                                          NULL);
    if (result == -1)
    {
      int error = ACE_OS::last_error ();
      if (error != ESHUTDOWN)
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ACE_Message_Queue::dequeue_head(): \"%m\", returning\n")));
      return;
    } // end IF
    ACE_ASSERT (message_block_p);

    if (message_block_p->msg_type () >= STREAM_MESSAGE_MAP_2)
      break;

    // session message --> put it back
    result = messageQueue_->enqueue_tail (message_block_p, NULL);
    if (result == -1)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_Message_Queue::enqueue_tail(): \"%m\", aborting\n")));
      return;
    } // end IF
  } while (true);

  // 2. enqueue data
  ACE_Message_Block* message_block_2 = fragment_;
  for (;
       message_block_2->cont ();
       message_block_2 = message_block_2->cont ());
  message_block_2->cont (message_block_p);
}

//void
//Stream_Decoder_AVIParserDriver::error (const yy::location& location_in,
//                                       const std::string& message_in)
//{
//  STREAM_TRACE (ACE_TEXT ("Stream_Decoder_AVIParserDriver::error"));
//
//  std::ostringstream converter;
//  converter << location_in;
//  // *NOTE*: the output format has been "adjusted" to fit in with bison error-reporting
//  ACE_DEBUG ((LM_ERROR,
//              ACE_TEXT ("(@%s): %s\n"),
//              ACE_TEXT (converter.str ().c_str ()),
//              ACE_TEXT (message_in.c_str ())));
////   ACE_DEBUG((LM_ERROR,
////              ACE_TEXT("failed to parse \"%s\" (@%s): \"%s\"\n"),
////              std::string(fragment_->rd_ptr(), fragment_->length()).c_str(),
////              converter.str().c_str(),
////              message_in.c_str()));
//
//  // dump message
//  ACE_Message_Block* head = fragment_;
//  while (head->prev ())
//    head = head->prev ();
//  ACE_ASSERT (head);
//  Common_IDumpState* idump_state_p = dynamic_cast<Common_IDumpState*> (head);
//  ACE_ASSERT (idump_state_p);
//  try
//  {
//    idump_state_p->dump_state ();
//  }
//  catch (...)
//  {
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("caught exception in Common_IDumpState::dump_state(), continuing\n")));
//  }
//
////   std::clog << location_in << ": " << message_in << std::endl;
//}

void
Stream_Decoder_AVIParserDriver::error (const std::string& message_in)
{
  STREAM_TRACE (ACE_TEXT ("Stream_Decoder_AVIParserDriver::error"));

  // *NOTE*: the output format has been "adjusted" to fit in with bison error-reporting
  ACE_DEBUG ((LM_ERROR,
              ACE_TEXT ("\": \"%s\"...\n"),
              ACE_TEXT (message_in.c_str ())));
//   ACE_DEBUG((LM_ERROR,
//              ACE_TEXT("failed to parse \"%s\": \"%s\"...\n"),
//              std::string(fragment_->rd_ptr(), fragment_->length()).c_str(),
//              message_in.c_str()));

//   std::clog << message_in << std::endl;
}

void
Stream_Decoder_AVIParserDriver::error (const YYLTYPE& location_in,
                                       const std::string& message_in)
{
  STREAM_TRACE (ACE_TEXT ("Stream_Decoder_AVIParserDriver::error"));

  // *NOTE*: the output format has been "adjusted" to fit in with bison error-reporting
  ACE_DEBUG ((LM_ERROR,
              ACE_TEXT ("(@%d.%d-%d.%d): %s\n"),
              location_in.first_line, location_in.first_column, location_in.last_line, location_in.last_column,
              ACE_TEXT (message_in.c_str ())));
  //   ACE_DEBUG((LM_ERROR,
  //              ACE_TEXT("failed to parse \"%s\" (@%s): \"%s\"\n"),
  //              std::string(fragment_->rd_ptr(), fragment_->length()).c_str(),
  //              converter.str().c_str(),
  //              message_in.c_str()));

  // dump message
  ACE_Message_Block* head = fragment_;
  while (head->prev ())
    head = head->prev ();
  ACE_ASSERT (head);
  Common_IDumpState* idump_state_p = dynamic_cast<Common_IDumpState*> (head);
  ACE_ASSERT (idump_state_p);
  try
  {
    idump_state_p->dump_state ();
  }
  catch (...)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("caught exception in Common_IDumpState::dump_state(), continuing\n")));
  }

  //   std::clog << location_in << ": " << message_in << std::endl;
}

bool
Stream_Decoder_AVIParserDriver::scan_begin ()
{
  STREAM_TRACE (ACE_TEXT ("Stream_Decoder_AVIParserDriver::scan_begin"));

//  static int counter = 1;

  // sanity check(s)
  ACE_ASSERT (bufferState_ == NULL);
  ACE_ASSERT (fragment_);

  // create/initialize a new buffer state
  if (useYYScanBuffer_)
  {
    bufferState_ =
      RIFF_Scanner__scan_buffer (fragment_->rd_ptr (),
                                 fragment_->length () + STREAM_DECODER_FLEX_BUFFER_BOUNDARY_SIZE,
                                 scannerState_);
  } // end IF
  else
  {
    bufferState_ =
      RIFF_Scanner__scan_bytes (fragment_->rd_ptr (),
                                fragment_->length (),
                                scannerState_);
  } // end ELSE
  if (!bufferState_)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to yy_scan_buffer/bytes(0x%@, %d), aborting\n"),
                fragment_->rd_ptr (),
                fragment_->length ()));
    return false;
  } // end IF
//  ACE_DEBUG ((LM_DEBUG,
//              ACE_TEXT ("parsing fragment #%d --> %d byte(s)...\n"),
//              counter++,
//              fragment_->length ()));

//  // *WARNING*: contrary (!) to the documentation, still need to switch_buffers()...
//  RIFF_Scanner__switch_to_buffer (bufferState_,
//                                  scannerState_);

  return true;
}

void
Stream_Decoder_AVIParserDriver::scan_end ()
{
  STREAM_TRACE (ACE_TEXT ("Stream_Decoder_AVIParserDriver::scan_end"));

  // sanity check(s)
  ACE_ASSERT (bufferState_);

  // clean state
  RIFF_Scanner__delete_buffer (bufferState_,
                               scannerState_);
  bufferState_ = NULL;
}
