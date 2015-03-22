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

#ifndef STREAM_SESSION_DATA_BASE_H
#define STREAM_SESSION_DATA_BASE_H

#include "ace/Global_Macros.h"
#include "ace/Time_Value.h"

#include "common_referencecounter_base.h"

template <typename DataType>
class Stream_SessionDataBase_T
 : public Common_ReferenceCounterBase
{
 public:
  Stream_SessionDataBase_T ();
  Stream_SessionDataBase_T (DataType*,     // (session) data
                            bool = false); // delete in dtor ?
  virtual ~Stream_SessionDataBase_T ();

  // info
  DataType* getData () const;

  // implement Common_IDumpState
  virtual void dump_state () const;

 private:
  typedef Common_ReferenceCounterBase inherited;

  ACE_UNIMPLEMENTED_FUNC (Stream_SessionDataBase_T (const Stream_SessionDataBase_T&));
//  ACE_UNIMPLEMENTED_FUNC (Stream_SessionDataBase_T& operator= (const Stream_SessionDataBase_T&));

  DataType* sessionData_;
  bool      deleteSessionData_;
};

// include template implementation
#include "stream_session_data_base.inl"

#endif
