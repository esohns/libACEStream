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
#include "ace/Message_Block.h"

#include "common_macros.h"

#include "stream_macros.h"

 // initialize statics
template <ACE_SYNCH_DECL,
          typename ConfigurationType>
typename Stream_DataBlockAllocatorHeap_T<ACE_SYNCH_USE,
                                         ConfigurationType>::DATABLOCK_LOCK_T
Stream_DataBlockAllocatorHeap_T<ACE_SYNCH_USE,
                                ConfigurationType>::referenceCountLock_;

template <ACE_SYNCH_DECL,
          typename ConfigurationType>
Stream_DataBlockAllocatorHeap_T<ACE_SYNCH_USE,
                                ConfigurationType>::Stream_DataBlockAllocatorHeap_T (HEAP_ALLOCATOR_T* allocator_in)
 : inherited ()
 , heapAllocator_ (allocator_in)
 , poolSize_ (0)
{
  STREAM_TRACE (ACE_TEXT ("Stream_DataBlockAllocatorHeap_T::Stream_DataBlockAllocatorHeap_T"));

  // *NOTE*: NULL --> use heap (== default allocator !)
  if (!heapAllocator_)
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("using default (== heap) message buffer allocation strategy\n")));
}

template <ACE_SYNCH_DECL,
          typename ConfigurationType>
void*
Stream_DataBlockAllocatorHeap_T<ACE_SYNCH_USE,
                                ConfigurationType>::calloc ()
{
  STREAM_TRACE (ACE_TEXT ("Stream_DataBlockAllocatorHeap_T::calloc"));

  ACE_Data_Block* data_block_p = NULL;
  try {
    ACE_NEW_MALLOC_NORETURN (data_block_p,
                             static_cast<ACE_Data_Block*> (inherited::malloc (sizeof (ACE_Data_Block))),
                             ACE_Data_Block (0,
                                             ACE_Message_Block::MB_NORMAL,
                                             NULL,
                                             NULL,
                                             NULL,
                                             0,
                                             this));
  } catch (...) {
    ACE_DEBUG ((LM_CRITICAL,
                ACE_TEXT ("caught exception in ACE_NEW_MALLOC_NORETURN(ACE_Data_Block(0)): \"%m\", continuing\n")));
  }
  if (unlikely (!data_block_p))
  {
    ACE_DEBUG ((LM_CRITICAL,
                ACE_TEXT ("failed to allocate ACE_Data_Block(0): \"%m\", aborting\n")));
    return NULL;
  } // end IF

  // increment running counter
  //   poolSize_ += data_block->capacity ();
  poolSize_++;

  return data_block_p;
}

template <ACE_SYNCH_DECL,
          typename ConfigurationType>
void*
Stream_DataBlockAllocatorHeap_T<ACE_SYNCH_USE,
                                ConfigurationType>::malloc (size_t bytes_in)
{
  STREAM_TRACE (ACE_TEXT ("Stream_DataBlockAllocatorHeap_T::malloc"));

  int result = -1;

  // sanity check(s)
  if (heapAllocator_)
  {
    ACE_ASSERT (heapAllocator_->configuration_);
  } // end IF

  ACE_Data_Block* data_block_p = NULL;
  try {
    // - delegate allocation to base class and...
    // - use placement new and...
    // - perform necessary initialization
    // *TODO*: use the heap allocator to allocate the instance
    ACE_NEW_MALLOC_NORETURN (data_block_p,
                             static_cast<ACE_Data_Block*> (inherited::malloc (sizeof (ACE_Data_Block))),
                             ACE_Data_Block ((bytes_in ? (heapAllocator_ ? bytes_in + heapAllocator_->configuration_->paddingBytes
                                                                         : bytes_in)
                                                       : 0),                          // size of data chunk
                                             (bytes_in ? ACE_Message_Block::MB_DATA : ACE_Message_Block::MB_USER),
                                             NULL,                                     // data --> use allocator !
                                             (bytes_in ? heapAllocator_ : NULL),       // heap allocator
                                             &OWN_TYPE_T::referenceCountLock_,         // reference count lock
                                             0,                                        // flags: release (heap) memory in dtor
                                             this));                                   // data block allocator
  } catch (...) {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("caught exception in ACE_NEW_MALLOC_NORETURN(ACE_Data_Block(%u)): \"%m\", continuing\n"),
                (bytes_in ? (heapAllocator_ ? bytes_in + heapAllocator_->configuration_->paddingBytes
                                            : bytes_in)
                          : 0)));
  }
  if (!data_block_p)
  {
    ACE_DEBUG ((LM_CRITICAL,
                ACE_TEXT ("failed to allocate ACE_Data_Block(%u): \"%m\", aborting\n"),
                (bytes_in ? (heapAllocator_ ? bytes_in + heapAllocator_->configuration_->paddingBytes
                                            : bytes_in)
                          : 0)));
    return NULL;
  } // end IF
  if (bytes_in)
  {
    result = data_block_p->size (bytes_in);
    if (result == -1)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_Data_Block::size(%u): \"%m\", aborting\n"),
                  bytes_in));

      // clean up
      data_block_p->release ();

      return NULL;
    } // end IF
  } // end IF

  // increment running counter
//   poolSize_ += data_block->capacity ();
  poolSize_++;

  return data_block_p;
}

template <ACE_SYNCH_DECL,
          typename ConfigurationType>
void
Stream_DataBlockAllocatorHeap_T<ACE_SYNCH_USE,
                                ConfigurationType>::free (void* handle_in)
{
  STREAM_TRACE (ACE_TEXT ("Stream_DataBlockAllocatorHeap_T::free"));

  // delegate to base class
  inherited::free (handle_in);

  // *NOTE*: handle_in really is a ACE_Data_Block*
//   ACE_Data_Block* data_block = NULL;
//   data_block = static_cast<ACE_Data_Block*> (handle_in);
//   ACE_ASSERT (data_block);

  // update allocation counter
//   poolSize_ -= data_block->capacity ();
  poolSize_--;
}

template <ACE_SYNCH_DECL,
          typename ConfigurationType>
size_t
Stream_DataBlockAllocatorHeap_T<ACE_SYNCH_USE,
                                ConfigurationType>::cache_depth () const
{
  STREAM_TRACE (ACE_TEXT ("Stream_DataBlockAllocatorHeap_T::cache_depth"));

  if (heapAllocator_)
    return heapAllocator_->cache_size (); // *TODO*: this doesn't look quite right

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  return std::numeric_limits<unsigned int>::max ();
#else
  return -1;
#endif
}

template <ACE_SYNCH_DECL,
          typename ConfigurationType>
void
Stream_DataBlockAllocatorHeap_T<ACE_SYNCH_USE,
                                ConfigurationType>::dump_state (void) const
{
  STREAM_TRACE (ACE_TEXT ("Stream_DataBlockAllocatorHeap_T::dump_state"));

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("# data fragments in flight: %u\n"),
              poolSize_.value ()));
}
