/* The pure C function declarations need to be externed or they get
mangled by the compiler*/
extern "C" {
#include "Collision/b2CollisionJsBindings.h"
#include "Collision/Shapes/b2ChainShapeJsBindings.h"
#include "Collision/Shapes/b2CircleShapeJsBindings.h"
#include "Collision/Shapes/b2EdgeShapeJsBindings.h"
#include "Collision/Shapes/b2PolygonShapeJsBindings.h"
#include "Dynamics/b2BodyJsBindings.h"
#include "Dynamics/b2FixtureJsBindings.h"
#include "Dynamics/b2WorldJsBindings.h"
#include "Dynamics/Contacts/b2ContactJsBindings.h"
#include "Dynamics/Joints/b2DistanceJointJsBindings.h"
#include "Dynamics/Joints/b2FrictionJointJsBindings.h"
#include "Dynamics/Joints/b2GearJointJsBindings.h"
#include "Dynamics/Joints/b2JointJsBindings.h"
#include "Dynamics/Joints/b2MotorJointJsBindings.h"
#include "Dynamics/Joints/b2MouseJointJsBindings.h"
#include "Dynamics/Joints/b2PrismaticJointJsBindings.h"
#include "Dynamics/Joints/b2PulleyJointJsBindings.h"
#include "Dynamics/Joints/b2RevoluteJointJsBindings.h"
#include "Dynamics/Joints/b2RopeJointJsBindings.h"
#include "Dynamics/Joints/b2WeldJointJsBindings.h"
#include "Dynamics/Joints/b2WheelJointJsBindings.h"
#include "Particle/b2ParticleGroupJsBindings.h"
#include "Particle/b2ParticleSystemJsBindings.h"
void GenerateOffsets();
}
#include "Collision/b2CollisionJsBindings.cpp"
#include "Collision/Shapes/b2ChainShapeJsBindings.cpp"
#include "Collision/Shapes/b2CircleShapeJsBindings.cpp"
#include "Collision/Shapes/b2EdgeShapeJsBindings.cpp"
#include "Collision/Shapes/b2PolygonShapeJsBindings.cpp"
#include "Dynamics/b2BodyJsBindings.cpp"
#include "Dynamics/b2FixtureJsBindings.cpp"
#include "Dynamics/b2WorldJsBindings.cpp"
#include "Dynamics/Contacts/b2ContactJsBindings.cpp"
#include "Dynamics/Joints/b2DistanceJointJsBindings.cpp"
#include "Dynamics/Joints/b2FrictionJointJsBindings.cpp"
#include "Dynamics/Joints/b2GearJointJsBindings.cpp"
#include "Dynamics/Joints/b2JointJsBindings.cpp"
#include "Dynamics/Joints/b2MotorJointJsBindings.cpp"
#include "Dynamics/Joints/b2MouseJointJsBindings.cpp"
#include "Dynamics/Joints/b2PrismaticJointJsBindings.cpp"
#include "Dynamics/Joints/b2PulleyJointJsBindings.cpp"
#include "Dynamics/Joints/b2RevoluteJointJsBindings.cpp"
#include "Dynamics/Joints/b2RopeJointJsBindings.cpp"
#include "Dynamics/Joints/b2WeldJointJsBindings.cpp"
#include "Dynamics/Joints/b2WheelJointJsBindings.cpp"
#include "Particle/b2ParticleGroupJsBindings.cpp"
#include "Particle/b2ParticleSystemJsBindings.cpp"


/*
// TODO clean all of this up, and/or make it auto generated from the
// header files
void PrintOffsets(b2Body* b) {
  printf("\tb2Body: {\n");
  printf("\t\ttype: %u,\n", (unsigned int)&b->m_type - (unsigned int)b);
  printf("\t\tislandIndex: %u,\n", (unsigned int)&b->m_islandIndex - (unsigned int)b);
  printf("\t\txf: %u,\n", (unsigned int)&b->m_xf - (unsigned int)b);
  printf("\t\txf0: %u,\n", (unsigned int)&b->m_xf0 - (unsigned int)b);
  printf("\t\tsweep: %u,\n", (unsigned int)&b->m_sweep - (unsigned int)b);
  printf("\t\tlinearVelocity: %u,\n", (unsigned int)&b->m_linearVelocity - (unsigned int)b);
  printf("\t\tangularVelocity: %u,\n", (unsigned int)&b->m_angularVelocity - (unsigned int)b);
  printf("\t\tforce: %u,\n", (unsigned int)&b->m_force - (unsigned int)b);
  printf("\t\ttorque: %u,\n", (unsigned int)&b->m_torque - (unsigned int)b);
  printf("\t\tworld: %u,\n", (unsigned int)&b->m_world - (unsigned int)b);
  printf("\t\tprev: %u,\n", (unsigned int)&b->m_prev - (unsigned int)b);
  printf("\t\tnext: %u,\n", (unsigned int)&b->m_next - (unsigned int)b);
  printf("\t\tfixtureList: %u,\n", (unsigned int)&b->m_fixtureList - (unsigned int)b);
  printf("\t\tfixtureCount: %u,\n", (unsigned int)&b->m_fixtureCount - (unsigned int)b);
  printf("\t\tjointList: %u,\n", (unsigned int)&b->m_jointList - (unsigned int)b);
  printf("\t\tcontactList: %u,\n", (unsigned int)&b->m_contactList - (unsigned int)b);
  printf("\t\tmass: %u,\n", (unsigned int)&b->m_mass - (unsigned int)b);
  printf("\t\tinvMass: %u,\n", (unsigned int)&b->m_invMass - (unsigned int)b);
  printf("\t\tI: %u,\n", (unsigned int)&b->m_I - (unsigned int)b);
  printf("\t\tinvI: %u,\n", (unsigned int)&b->m_invI - (unsigned int)b);
  printf("\t\tlinearDamping: %u,\n", (unsigned int)&b->m_linearDamping - (unsigned int)b);
  printf("\t\tangularDamping: %u,\n", (unsigned int)&b->m_angularDamping - (unsigned int)b);
  printf("\t\tgravityScale: %u,\n", (unsigned int)&b->m_gravityScale - (unsigned int)b);
  printf("\t\tsleepTime: %u,\n", (unsigned int)&b->m_sleepTime - (unsigned int)b);
  printf("\t\tuserData: %u\n", (unsigned int)&b->m_userData - (unsigned int)b);
  printf("\t}\n");
}

void PrintOffsets(b2Contact* c) {
  printf("\tb2Contact: {\n");
  printf("\t\tflags: %u,\n", (unsigned int)&c->m_flags - (unsigned int)c);
  printf("\t\tprev: %u,\n", (unsigned int)&c->m_prev - (unsigned int)c);
  printf("\t\tnext: %u,\n", (unsigned int)&c->m_next - (unsigned int)c);
  printf("\t\tnodeA: %u,\n", (unsigned int)&c->m_nodeA - (unsigned int)c);
  printf("\t\tnodeB: %u,\n", (unsigned int)&c->m_nodeB - (unsigned int)c);
  printf("\t\tfixtureA: %u,\n", (unsigned int)&c->m_fixtureA - (unsigned int)c);
  printf("\t\tfixtureB: %u,\n", (unsigned int)&c->m_fixtureB - (unsigned int)c);
  printf("\t\tindexA: %u,\n", (unsigned int)&c->m_indexA - (unsigned int)c);
  printf("\t\tindexB: %u,\n", (unsigned int)&c->m_indexB - (unsigned int)c);
  printf("\t\tmanifold: %u,\n", (unsigned int)&c->m_manifold - (unsigned int)c);
  printf("\t\ttoiCount: %u,\n", (unsigned int)&c->m_toiCount - (unsigned int)c);
  printf("\t\ttoi: %u,\n", (unsigned int)&c->m_toi - (unsigned int)c);
  printf("\t\tfriction: %u,\n", (unsigned int)&c->m_friction - (unsigned int)c);
  printf("\t\trestitution: %u,\n", (unsigned int)&c->m_restitution - (unsigned int)c);
  printf("\t\ttangentSpeed: %u\n", (unsigned int)&c->m_tangentSpeed - (unsigned int)c);
  printf("\t},\n");
}

void PrintOffsets(b2Fixture* f) {
  printf("\tb2Fixture: {\n");
  printf("\t\tdensity: %u,\n", (unsigned int)&f->m_density - (unsigned int)f);
  printf("\t\tnext: %u,\n", (unsigned int)&f->m_next - (unsigned int)f);
  printf("\t\tbody: %u,\n", (unsigned int)&f->m_body - (unsigned int)f);
  printf("\t\tshape: %u,\n", (unsigned int)&f->m_shape - (unsigned int)f);
  printf("\t\tfriction: %u,\n", (unsigned int)&f->m_friction - (unsigned int)f);
  printf("\t\trestitution: %u,\n", (unsigned int)&f->m_restitution - (unsigned int)f);
  printf("\t\tproxies: %u,\n", (unsigned int)&f->m_proxies - (unsigned int)f);
  printf("\t\tproxyCount: %u,\n", (unsigned int)&f->m_proxyCount - (unsigned int)f);
  printf("\t\tfilter: %u,\n", (unsigned int)&f->m_filter - (unsigned int)f);
  printf("\t\tisSensor: %u,\n", (unsigned int)&f->m_isSensor - (unsigned int)f);
  printf("\t\tuserData: %u\n", (unsigned int)&f->m_userData - (unsigned int)f);
  printf("\t},\n");
}

void PrintOffsets(b2ParticleGroup* p) {
  printf("\tb2ParticleGroup: {\n");
  printf("\t\tsystem: %u,\n", (unsigned int)&p->m_system - (unsigned int)p);
  printf("\t\tfirstIndex: %u,\n", (unsigned int)&p->m_firstIndex - (unsigned int)p);
  printf("\t\tlastIndex: %u,\n", (unsigned int)&p->m_lastIndex - (unsigned int)p);
  printf("\t\tgroupFlags: %u,\n", (unsigned int)&p->m_groupFlags - (unsigned int)p);
  printf("\t\tstrength: %u,\n", (unsigned int)&p->m_strength - (unsigned int)p);
  printf("\t\tprev: %u,\n", (unsigned int)&p->m_prev - (unsigned int)p);
  printf("\t\tnext: %u,\n", (unsigned int)&p->m_next - (unsigned int)p);
  printf("\t\ttimestamp: %u,\n", (unsigned int)&p->m_timestamp - (unsigned int)p);
  printf("\t\tmass: %u,\n", (unsigned int)&p->m_mass - (unsigned int)p);
  printf("\t\tinertia: %u,\n", (unsigned int)&p->m_inertia - (unsigned int)p);
  printf("\t\tcenter: %u,\n", (unsigned int)&p->m_center - (unsigned int)p);
  printf("\t\tlinearVelocity: %u,\n", (unsigned int)&p->m_linearVelocity - (unsigned int)p);
  printf("\t\tangularVelocity: %u,\n", (unsigned int)&p->m_angularVelocity - (unsigned int)p);
  printf("\t\ttransform: %u,\n", (unsigned int)&p->m_transform - (unsigned int)p);
  printf("\t\tuserData: %u\n", (unsigned int)&p->m_userData - (unsigned int)p);
  printf("\t},\n");
}

void PrintOffsets(b2World* w) {
  printf("\tb2World: {\n");
  printf("\t\tbodyList: %u\n", (unsigned int)&w->m_bodyList - (unsigned int)w);
  printf("\t},\n");
}

void PrintOffsets(b2WorldManifold* wm) {
  printf("\tb2WorldManifold: {\n");
  printf("\t\tnormal: %u,\n", (unsigned int)&wm->normal - (unsigned int)wm);
  printf("\t\tpoints: %u,\n", (unsigned int)&wm->points - (unsigned int)wm);
  printf("\t\tseparations: %u\n", (unsigned int)&wm->separations - (unsigned int)wm);
  printf("\t},\n");
}


#include <stdio.h>
extern "C" {
void GenerateOffsets() {
  printf("{\n");
  //create dummy body to generate offsets
  b2World world(b2Vec2(0, 0));
  b2BodyDef def;
  b2Body* body1 = world.CreateBody(&def);

  b2CircleShape s;
  b2FixtureDef d;
  d.shape = &s;
  b2Fixture* f =body1->CreateFixture(&d);

  b2ParticleSystemDef psd;
  b2ParticleSystem* ps = world.CreateParticleSystem(&psd);

  b2ParticleGroupDef pgd;
  b2ParticleGroup* pg = ps->CreateParticleGroup(pgd);

  b2WorldManifold worldManifold;
  PrintOffsets(body1);
  PrintOffsets(f);
  PrintOffsets(pg);
  PrintOffsets(&world);
  PrintOffsets(&worldManifold);

  // need to instantiate contact differently
  //b2Contact contact;
  //PrintOffsets(&contact);

  printf("};\n");
}
}*/
