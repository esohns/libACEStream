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

#include "stream_module_tcpio_stream.h"

// *TODO*: is there a way to initialize (/export) static template members within
//         (/from) a dynamic library ?
//// initialize statics
//template <typename TaskSynchType,
//          typename TimePolicyType,
//          typename StatusType,
//          typename StateType,
//          typename ConfigurationType,
//          typename StatisticContainerType,
//          typename ModuleConfigurationType,
//          typename HandlerConfigurationType,
//          typename SessionDataType,
//          typename SessionDataContainerType,
//          typename SessionMessageType,
//          typename ProtocolMessageType,
//          typename ConnectionManagerType>
//ACE_Atomic_Op<ACE_Thread_Mutex, unsigned long>
//Stream_Module_TCPIO_Stream_T<TaskSynchType,
//                             TimePolicyType,
//                             StatusType,
//                             StateType,
//                             ConfigurationType,
//                             StatisticContainerType,
//                             ModuleConfigurationType,
//                             HandlerConfigurationType,
//                             SessionDataType,
//                             SessionDataContainerType,
//                             SessionMessageType,
//                             ProtocolMessageType,
//                             ConnectionManagerType>::currentSessionID = 0;
