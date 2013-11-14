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
