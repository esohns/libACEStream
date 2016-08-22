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

#ifndef TEST_I_TARGET_SIGNALHANDLER_H
#define TEST_I_TARGET_SIGNALHANDLER_H

#include "ace/Global_Macros.h"

#include "common_isignal.h"
#include "common_signalhandler.h"

template <typename ConfigurationType,
          typename ConnectionManagerType>
class Test_I_Target_SignalHandler_T
 : public Common_SignalHandler_T<ConfigurationType>
 , public Common_ISignal
{
 public:
  Test_I_Target_SignalHandler_T ();
  virtual ~Test_I_Target_SignalHandler_T ();

  // implement Common_ISignal
  virtual bool handleSignal (int); // signal

 private:
  typedef Common_SignalHandler_T<ConfigurationType> inherited;

  ACE_UNIMPLEMENTED_FUNC (Test_I_Target_SignalHandler_T (const Test_I_Target_SignalHandler_T&))
  ACE_UNIMPLEMENTED_FUNC (Test_I_Target_SignalHandler_T& operator= (const Test_I_Target_SignalHandler_T&))
};

// include template definition
#include "test_i_target_signalhandler.inl"

#endif
