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

#include "test_u_filecopy_eventhandler.h"

#include "ace/Guard_T.h"
#include "ace/Synch_Traits.h"

#include "common_ui_defines.h"

#include "stream_macros.h"

#include "test_u_filecopy_defines.h"

Stream_Filecopy_EventHandler::Stream_Filecopy_EventHandler (Stream_Filecopy_GTK_CBData* CBData_in)
 : CBData_ (CBData_in)
 , sessionData_ (NULL)
{
  STREAM_TRACE (ACE_TEXT ("Stream_Filecopy_EventHandler::Stream_Filecopy_EventHandler"));

}

Stream_Filecopy_EventHandler::~Stream_Filecopy_EventHandler ()
{
  STREAM_TRACE (ACE_TEXT ("Stream_Filecopy_EventHandler::~Stream_Filecopy_EventHandler"));

}

void
Stream_Filecopy_EventHandler::start (const Stream_Filecopy_SessionData& sessionData_in)
{
  STREAM_TRACE (ACE_TEXT ("Stream_Filecopy_EventHandler::start"));

  sessionData_ = &sessionData_in;

  // sanity check(s)
  ACE_ASSERT (CBData_);

  ACE_Guard<ACE_SYNCH_MUTEX> aGuard (CBData_->lock);

  CBData_->progressData.size = sessionData_->size;

  {
    ACE_Guard<ACE_SYNCH_RECURSIVE_MUTEX> aGuard (CBData_->stackLock);

    CBData_->eventStack.push_back (STREAM_GTKEVENT_START);
  } // end lock scope
}

void
Stream_Filecopy_EventHandler::notify (const Stream_Filecopy_Message& message_in)
{
  STREAM_TRACE (ACE_TEXT ("Stream_Filecopy_EventHandler::notify"));

  // sanity check(s)
  ACE_ASSERT (CBData_);
  ACE_ASSERT (sessionData_);

  ACE_Guard<ACE_SYNCH_MUTEX> aGuard (CBData_->lock);

  CBData_->progressData.copied += message_in.total_length ();

  {
    ACE_Guard<ACE_SYNCH_RECURSIVE_MUTEX> aGuard (CBData_->stackLock);

    CBData_->eventStack.push_back (STREAM_GTKEVENT_DATA);
  } // end lock scope
}
void
Stream_Filecopy_EventHandler::notify (const Stream_Filecopy_SessionMessage& sessionMessage_in)
{
  STREAM_TRACE (ACE_TEXT ("Stream_Filecopy_EventHandler::notify"));

  // sanity check(s)
  ACE_ASSERT (CBData_);

  Stream_GTK_Event event =
    ((sessionMessage_in.type () == STREAM_SESSION_STATISTIC) ? STREAM_GTKEVENT_STATISTIC
                                                             : STREAM_GKTEVENT_INVALID);

  {
    ACE_Guard<ACE_SYNCH_RECURSIVE_MUTEX> aGuard (CBData_->stackLock);

    CBData_->eventStack.push_back (event);
  } // end lock scope
}

void
Stream_Filecopy_EventHandler::end ()
{
  STREAM_TRACE (ACE_TEXT ("Stream_Filecopy_EventHandler::end"));

  // sanity check(s)
  ACE_ASSERT (CBData_);

  ACE_Guard<ACE_SYNCH_MUTEX> aGuard (CBData_->lock);

  //Common_UI_GladeXMLsIterator_t iterator =
  //  data_p->gladeXML.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_GTK_DEFINITION_DESCRIPTOR_MAIN));
  Common_UI_GTKBuildersIterator_t iterator =
    CBData_->builders.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_GTK_DEFINITION_DESCRIPTOR_MAIN));

  // sanity check(s)
  //ACE_ASSERT (iterator != CBData_->gladeXML.end ());
  ACE_ASSERT (iterator != CBData_->builders.end ());

  gdk_threads_enter ();
  GtkAction* action_p =
    GTK_ACTION (gtk_builder_get_object ((*iterator).second.second,
                                        ACE_TEXT_ALWAYS_CHAR (TEST_U_STREAM_UI_GTK_ACTION_STOP_NAME)));
  ACE_ASSERT (action_p);
  gtk_action_set_sensitive (action_p, FALSE);
  action_p =
    GTK_ACTION (gtk_builder_get_object ((*iterator).second.second,
                                        ACE_TEXT_ALWAYS_CHAR (TEST_U_STREAM_UI_GTK_ACTION_START_NAME)));
  ACE_ASSERT (action_p);
  gtk_action_set_stock_id (action_p, GTK_STOCK_MEDIA_PLAY);
  gdk_threads_leave ();

  {
    ACE_Guard<ACE_SYNCH_RECURSIVE_MUTEX> aGuard (CBData_->stackLock);

    CBData_->eventStack.push_back (STREAM_GTKEVENT_END);
  } // end lock scope
}
