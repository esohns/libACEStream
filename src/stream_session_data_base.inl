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

#include "stream_macros.h"
#include "stream_tools.h"

template <typename DataType>
Stream_SessionDataBase_T<DataType>::Stream_SessionDataBase_T ()
 : inherited (1,    // initial count
              true) // delete on zero ?
 , data_ (NULL)
 , delete_ (false)
{
  STREAM_TRACE (ACE_TEXT ("Stream_SessionDataBase_T::Stream_SessionDataBase_T"));

}

template <typename DataType>
Stream_SessionDataBase_T<DataType>::Stream_SessionDataBase_T (DataType* data_in,
                                                              bool delete_in)
 : inherited (1,    // initial count
              true) // delete on zero ?
 , data_ (data_in)
 , delete_ (delete_in)
{
  STREAM_TRACE (ACE_TEXT ("Stream_SessionDataBase_T::Stream_SessionDataBase_T"));

}

template <typename DataType>
Stream_SessionDataBase_T<DataType>::~Stream_SessionDataBase_T ()
{
  STREAM_TRACE (ACE_TEXT ("Stream_SessionDataBase_T::~Stream_SessionDataBase_T"));

  // clean up ?
  if (delete_)
    delete data_;
}

template <typename DataType>
void
Stream_SessionDataBase_T<DataType>::dump_state () const
{
  STREAM_TRACE (ACE_TEXT ("Stream_SessionDataBase_T::dump_state"));

  // sanity check(s)
  ACE_ASSERT (data_);

  // *TODO*: remove type inferences
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("user data: %@, start of session: %s%s\n"),
              data_->userData,
              ACE_TEXT (Stream_Tools::timeStamp2LocalString (data_->startOfSession).c_str ()),
              (data_->aborted ? ACE_TEXT(" [user abort !]")
                              : ACE_TEXT(""))));
}

template <typename DataType>
const DataType&
Stream_SessionDataBase_T<DataType>::get () const
{
  STREAM_TRACE (ACE_TEXT ("Stream_SessionDataBase_T::get"));

  // sanity check(s)
  ACE_ASSERT (data_);

  return *data_;
}
template <typename DataType>
void
Stream_SessionDataBase_T<DataType>::set (const DataType& data_in)
{
  STREAM_TRACE (ACE_TEXT ("Stream_SessionDataBase_T::set"));

  // merge ?
  // *TODO*: enforce merge
  DataType& data_r = const_cast<DataType&> (data_in);
  if (data_)
    data_r = *data_; // *WARNING*: this SHOULD be a merge operation !

  // clean up ?
  if (data_ && delete_)
  {
    delete data_;
    data_ = NULL;

    delete_ = false;
  } // end IF

  data_ = &const_cast<DataType&> (data_in);
  delete_ = false; // never delete
}

template <typename DataType>
Stream_SessionDataBase_T<DataType>&
Stream_SessionDataBase_T<DataType>::operator= (const Stream_SessionDataBase_T& rhs_in)
{
  STREAM_TRACE (ACE_TEXT ("Stream_SessionDataBase_T::set"));

  // merge ?
  // *TODO*: enforce merge
  DataType* data_p = const_cast<DataType*> (rhs_in.data_);
  if (data_ && data_p)
    *data_p = *data_; // *WARNING*: this SHOULD be a merge operation !

  // clean up ?
  if (data_ && delete_)
  {
    delete data_;
    data_ = NULL;
  } // end IF

  data_ = rhs_in.data_;
  delete_ = false; // never delete

  return *this;
}
