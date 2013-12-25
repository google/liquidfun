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
#ifndef B2_STATLOG
#define B2_STATLOG

#include <Box2D/Common/b2Settings.h>

/// Calculates min/max/mean of a set of samples
class b2Stat
{
public:
	b2Stat();

	/// Record a sample
	void Record( float32 t );

	int GetCount();
	float32 GetMean();
	float32 GetMin();
	float32 GetMax();

	/// Clear stat buffer
	void Clear();
private:

	int m_count;
	float64 m_total;
	float32 m_min;
	float32 m_max;
};

#endif
