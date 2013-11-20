/*
* Copyright (c) 2013 Google, Inc.
*
* This software is provided 'as-is', without any express or implied
* warranty.  In no event will the authors be held liable for any damages
* arising from the use of this software.
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
* 1. The origin of this software must not be misrepresented; you must not
* claim that you wrote the original software. If you use this software
* in a product, an acknowledgment in the product documentation would be
* appreciated but is not required.
* 2. Altered source versions must be plainly marked as such, and must not be
* misrepresented as being the original software.
* 3. This notice may not be removed or altered from any source distribution.
*/
#ifndef B2_VORONOI_DIAGRAM
#define B2_VORONOI_DIAGRAM

#include <Box2D/Common/b2Math.h>

class b2StackAllocator;
struct b2AABB;

/// A field representing the nearest generator from each point.
class b2VoronoiDiagram
{

public:

	b2VoronoiDiagram(
		b2StackAllocator *allocator,
		const b2Vec2 *generatorData, int32 generatorCount,
		float32 radius);
	~b2VoronoiDiagram();

	template<class Callback>
	void GetNodes(const Callback &callback) const
	{
		for (int32 y = 0; y < m_countY - 1; y++)
		{
			for (int32 x = 0; x < m_countX - 1; x++)
			{
				int32 i = x + y * m_countX;
				int32 a = m_diagram[i];
				int32 b = m_diagram[i + 1];
				int32 c = m_diagram[i + m_countX];
				int32 d = m_diagram[i + 1 + m_countX];
				if (b != c)
				{
					if (a != b && a != c)
					{
						callback(a, b, c);
					}
					if (d != b && d != c)
					{
						callback(b, d, c);
					}
				}
			}
		}
	}

private:

	b2StackAllocator *m_allocator;
	int32 m_countX, m_countY;
	int32 *m_diagram;

};

#endif
