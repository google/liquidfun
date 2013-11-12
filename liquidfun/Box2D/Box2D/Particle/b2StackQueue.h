// Copyright 2013 Google. All rights reserved.
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd
#ifndef B2_STACK_QUEUE
#define B2_STACK_QUEUE

#include <Box2D/Common/b2StackAllocator.h>

template <typename T>
class b2StackQueue
{

public:

	b2StackQueue(b2StackAllocator *allocator, int32 capacity)
	{
		m_allocator = allocator;
		m_buffer = (T*) m_allocator->Allocate(sizeof(T) * capacity);
		m_front = m_buffer;
		m_back = m_buffer;
		m_end = m_buffer + capacity;
	}

	~b2StackQueue()
	{
		m_allocator->Free(m_buffer);
		m_buffer = NULL;
		m_front = NULL;
		m_back = NULL;
		m_end = NULL;
	}

	void Push(const T &item)
	{
		if (m_back >= m_end)
		{
			ptrdiff_t diff = m_front - m_buffer;
			for (T *it = m_front; it < m_back; ++it)
			{
				*(it - diff) = *it;
			}
			m_front -= diff;
			m_back -= diff;
			if (m_back >= m_end)
			{
				return;
			}
		}
		*m_back++ = item;
	}

	void Pop()
	{
		b2Assert(m_front < m_back);
		m_front++;
	}

	bool Empty() const
	{
		return m_front >= m_back;
	}

	const T &Front() const
	{
		return *m_front;
	}

private:

	b2StackAllocator *m_allocator;
	T* m_buffer;
	T* m_front;
	T* m_back;
	T* m_end;

};

#endif
