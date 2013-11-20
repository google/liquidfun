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
#include <assert.h>
#include <Box2D/Box2D.h>

#define EPSILON 0.001f

// Create a 10x10 box shaped particle group.
b2ParticleGroup* CreateBoxShapedParticleGroup(b2World * const world)
{
	assert(world);
	b2ParticleGroupDef def;
	b2PolygonShape shape;
	shape.SetAsBox(10, 10);
	def.shape = &shape;
	return world->CreateParticleGroup(def);
}

// Create and destroy a particle, returning an index to the particle that
// was destroyed.
int32 CreateAndDestroyParticle(b2World *const world, uint32 additionalFlags,
                               bool callDestructionListener)
{
	assert(world);
	b2ParticleDef def;
	def.flags |= additionalFlags;
	int32 index = world->CreateParticle(def);
	world->DestroyParticle(index, callDestructionListener);
	world->Step(0.001f, 1, 1);
	return index;
}
