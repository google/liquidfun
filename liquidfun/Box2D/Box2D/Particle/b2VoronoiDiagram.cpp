// Copyright 2013 Google. All rights reserved.
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd
#include <Box2D/Particle/b2VoronoiDiagram.h>
#include <Box2D/Particle/b2StackQueue.h>
#include <Box2D/Collision/b2Collision.h>

struct b2VoronoiDiagramTask
{
	int32 m_x, m_y, m_i, m_k;
	b2VoronoiDiagramTask() {}
	b2VoronoiDiagramTask(int32 x, int32 y, int32 i, int32 k)
	{
		m_x = x;
		m_y = y;
		m_i = i;
		m_k = k;
	}
};

b2VoronoiDiagram::b2VoronoiDiagram(
	b2StackAllocator *allocator,
	const b2Vec2 *generators, int32 generatorCount,
	float32 radius)
{
	m_allocator = allocator;
	float32 inverseRadius = 1 / radius;
	b2Vec2 lower(+b2_maxFloat, +b2_maxFloat);
	b2Vec2 upper(-b2_maxFloat, -b2_maxFloat);
	for (int32 k = 0; k < generatorCount; k++)
	{
		b2Vec2 g = generators[k];
		lower = b2Min(lower, g);
		upper = b2Max(upper, g);
	}
	m_countX = 1 + (int32) (inverseRadius * (upper.x - lower.x));
	m_countY = 1 + (int32) (inverseRadius * (upper.y - lower.y));
	m_diagram = (int32*) m_allocator->Allocate(sizeof(int32) * m_countX * m_countY);
	for (int32 i = 0; i < m_countX * m_countY; i++)
	{
		m_diagram[i] = -1;
	}
	b2StackQueue<b2VoronoiDiagramTask> queue(m_allocator, 4 * m_countX * m_countX);
	for (int32 k = 0; k < generatorCount; k++)
	{
		b2Vec2 g = inverseRadius * (generators[k] - lower);
		int32 x = b2Max(0, b2Min((int32) g.x, m_countX - 1));
		int32 y = b2Max(0, b2Min((int32) g.y, m_countY - 1));
		queue.Push(b2VoronoiDiagramTask(x, y, x + y * m_countX, k));
	}
	while (!queue.Empty())
	{
		int32 x = queue.Front().m_x;
		int32 y = queue.Front().m_y;
		int32 i = queue.Front().m_i;
		int32 k = queue.Front().m_k;
		queue.Pop();
		if (m_diagram[i] < 0)
		{
			m_diagram[i] = k;
			if (x > 0)
			{
				queue.Push(b2VoronoiDiagramTask(x - 1, y, i - 1, k));
			}
			if (y > 0)
			{
				queue.Push(b2VoronoiDiagramTask(x, y - 1, i - m_countX, k));
			}
			if (x < m_countX - 1)
			{
				queue.Push(b2VoronoiDiagramTask(x + 1, y, i + 1, k));
			}
			if (y < m_countY - 1)
			{
				queue.Push(b2VoronoiDiagramTask(x, y + 1, i + m_countX, k));
			}
		}
	}
	int32 maxIteration = m_countX + m_countY;
	for (int32 iteration = 0; iteration < maxIteration; iteration++)
	{
		for (int32 y = 0; y < m_countY; y++)
		{
			for (int32 x = 0; x < m_countX - 1; x++)
			{
				int32 i = x + y * m_countX;
				int32 a = m_diagram[i];
				int32 b = m_diagram[i + 1];
				if (a != b)
				{
					queue.Push(b2VoronoiDiagramTask(x, y, i, b));
					queue.Push(b2VoronoiDiagramTask(x + 1, y, i + 1, a));
				}
			}
		}
		for (int32 y = 0; y < m_countY - 1; y++)
		{
			for (int32 x = 0; x < m_countX; x++)
			{
				int32 i = x + y * m_countX;
				int32 a = m_diagram[i];
				int32 b = m_diagram[i + m_countX];
				if (a != b)
				{
					queue.Push(b2VoronoiDiagramTask(x, y, i, b));
					queue.Push(b2VoronoiDiagramTask(x, y + 1, i + m_countX, a));
				}
			}
		}
		bool updated = false;
		while (!queue.Empty())
		{
			int32 x = queue.Front().m_x;
			int32 y = queue.Front().m_y;
			int32 i = queue.Front().m_i;
			int32 k = queue.Front().m_k;
			queue.Pop();
			int32 a = m_diagram[i];
			int32 b = k;
			if (a != b)
			{
				b2Vec2 va = inverseRadius * (generators[a] - lower);
				b2Vec2 vb = inverseRadius * (generators[b] - lower);
				float32 ax = va.x - x;
				float32 ay = va.y - y;
				float32 bx = vb.x - x;
				float32 by = vb.y - y;
				float32 a2 = ax * ax + ay * ay;
				float32 b2 = bx * bx + by * by;
				if (a2 > b2)
				{
					m_diagram[i] = b;
					if (x > 0)
					{
						queue.Push(b2VoronoiDiagramTask(x - 1, y, i - 1, b));
					}
					if (y > 0)
					{
						queue.Push(b2VoronoiDiagramTask(x, y - 1, i - m_countX, b));
					}
					if (x < m_countX - 1)
					{
						queue.Push(b2VoronoiDiagramTask(x + 1, y, i + 1, b));
					}
					if (y < m_countY - 1)
					{
						queue.Push(b2VoronoiDiagramTask(x, y + 1, i + m_countX, b));
					}
					updated = true;
				}
			}
		}
		if (!updated)
		{
			break;
		}
	}
}

b2VoronoiDiagram::~b2VoronoiDiagram()
{
	m_allocator->Free(m_diagram);
}
