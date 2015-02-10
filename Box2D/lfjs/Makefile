# Makefile for generating a Box2D library using Emscripten.

# For placing path overrides.. this path is hidden from git
-include Makefile.local

PYTHON=$(ENV) python

O = ../Box2D
OBJECTS = \
$(O)/Rope/b2Rope.cpp \
$(O)/Collision/b2TimeOfImpact.cpp \
$(O)/Collision/b2Distance.cpp \
$(O)/Collision/Shapes/b2EdgeShape.cpp \
$(O)/Collision/Shapes/b2PolygonShape.cpp \
$(O)/Collision/Shapes/b2CircleShape.cpp \
$(O)/Collision/Shapes/b2ChainShape.cpp \
$(O)/Collision/b2BroadPhase.cpp \
$(O)/Collision/b2CollideCircle.cpp \
$(O)/Collision/b2DynamicTree.cpp \
$(O)/Collision/b2CollideEdge.cpp \
$(O)/Collision/b2CollidePolygon.cpp \
$(O)/Collision/b2Collision.cpp \
$(O)/Dynamics/b2Island.cpp \
$(O)/Dynamics/b2WorldCallbacks.cpp \
$(O)/Dynamics/Joints/b2MouseJoint.cpp \
$(O)/Dynamics/Joints/b2MotorJoint.cpp \
$(O)/Dynamics/Joints/b2DistanceJoint.cpp \
$(O)/Dynamics/Joints/b2FrictionJoint.cpp \
$(O)/Dynamics/Joints/b2WeldJoint.cpp \
$(O)/Dynamics/Joints/b2GearJoint.cpp \
$(O)/Dynamics/Joints/b2PrismaticJoint.cpp \
$(O)/Dynamics/Joints/b2RopeJoint.cpp \
$(O)/Dynamics/Joints/b2Joint.cpp \
$(O)/Dynamics/Joints/b2RevoluteJoint.cpp \
$(O)/Dynamics/Joints/b2WheelJoint.cpp \
$(O)/Dynamics/Joints/b2PulleyJoint.cpp \
$(O)/Dynamics/Contacts/b2ChainAndPolygonContact.cpp \
$(O)/Dynamics/Contacts/b2Contact.cpp \
$(O)/Dynamics/Contacts/b2PolygonAndCircleContact.cpp \
$(O)/Dynamics/Contacts/b2CircleContact.cpp \
$(O)/Dynamics/Contacts/b2ContactSolver.cpp \
$(O)/Dynamics/Contacts/b2EdgeAndCircleContact.cpp \
$(O)/Dynamics/Contacts/b2ChainAndCircleContact.cpp \
$(O)/Dynamics/Contacts/b2EdgeAndPolygonContact.cpp \
$(O)/Dynamics/Contacts/b2PolygonContact.cpp \
$(O)/Dynamics/b2Fixture.cpp \
$(O)/Dynamics/b2World.cpp \
$(O)/Dynamics/b2Body.cpp \
$(O)/Dynamics/b2ContactManager.cpp \
$(O)/Particle/b2VoronoiDiagram.cpp \
$(O)/Particle/b2ParticleGroup.cpp \
$(O)/Particle/b2ParticleSystem.cpp \
$(O)/Particle/b2Particle.cpp \
$(O)/Common/b2FreeList.cpp \
$(O)/Common/b2BlockAllocator.cpp \
$(O)/Common/b2Draw.cpp \
$(O)/Common/b2Math.cpp \
$(O)/Common/b2Stat.cpp \
$(O)/Common/b2Timer.cpp \
$(O)/Common/b2Settings.cpp \
$(O)/Common/b2TrackedBlock.cpp \
$(O)/Common/b2StackAllocator.cpp

B2BODY = \
	'_b2Body_ApplyAngularImpulse', \
	'_b2Body_ApplyForce', \
	'_b2Body_ApplyForceToCenter', \
	'_b2Body_ApplyTorque', \
	'_b2Body_DestroyFixture', \
	'_b2Body_GetAngle', \
	'_b2Body_GetAngularVelocity', \
	'_b2Body_GetInertia', \
	'_b2Body_GetLinearVelocity', \
	'_b2Body_GetLocalPoint', \
	'_b2Body_GetLocalVector', \
	'_b2Body_GetMass', \
	'_b2Body_GetPosition', \
	'_b2Body_GetTransform', \
	'_b2Body_GetType', \
	'_b2Body_GetWorldCenter', \
	'_b2Body_GetWorldPoint', \
	'_b2Body_GetWorldVector', \
	'_b2Body_SetAngularVelocity', \
	'_b2Body_SetAwake', \
	'_b2Body_SetLinearVelocity', \
	'_b2Body_SetMassData', \
	'_b2Body_SetTransform', \
	'_b2Body_SetType'

B2CHAINSHAPE = \
	'_b2ChainShape_CreateFixture'

B2CIRCLESHAPE = \
	'_b2CircleShape_CreateFixture', \
	'_b2CircleShape_CreateParticleGroup', \
	'_b2CircleShape_DestroyParticlesInShape'

B2COLLISION = \
	'_b2Manifold_GetPointCount'

B2CONTACT = \
	'_b2Contact_GetManifold', \
	'_b2Contact_GetWorldManifold'

B2DISTANCEJOINT = \
	'_b2DistanceJointDef_Create', \
	'_b2DistanceJointDef_InitializeAndCreate' \

B2EDGESHAPE = \
	'_b2EdgeShape_CreateFixture'

B2FIXTURE = \
	'_b2Fixture_TestPoint'

B2FRICTIONJOINT = \
	'_b2FrictionJointDef_Create', \
	'_b2FrictionJointDef_InitializeAndCreate' \

B2GEARJOINT = \
	'_b2GearJoint_GetRatio', \
	'_b2GearJointDef_Create', \
	'_b2GearJointDef_InitializeAndCreate'

B2JOINT = \
	'_b2Joint_GetBodyA', \
	'_b2Joint_GetBodyB'

B2MOTORJOINT = \
	'_b2MotorJoint_SetAngularOffset', \
	'_b2MotorJoint_SetLinearOffset', \
	'_b2MotorJointDef_Create', \
	'_b2MotorJointDef_InitializeAndCreate'

B2MOUSEJOINT = \
	'_b2MouseJoint_SetTarget', \
	'_b2MouseJointDef_Create'

B2PARTICLEGROUP = \
	'_b2ParticleGroup_ApplyForce', \
	'_b2ParticleGroup_ApplyLinearImpulse', \
	'_b2ParticleGroup_DestroyParticles', \
	'_b2ParticleGroup_GetParticleCount', \
	'_b2ParticleGroup_GetBufferIndex'

B2PARTICLESYSTEM = \
	'_b2ParticleSystem_CreateParticle', \
	'_b2ParticleSystem_GetColorBuffer', \
	'_b2ParticleSystem_GetParticleCount', \
	'_b2ParticleSystem_GetPositionBuffer', \
	'_b2ParticleSystem_GetVelocityBuffer', \
	'_b2ParticleSystem_SetDamping', \
	'_b2ParticleSystem_SetDensity', \
	'_b2ParticleSystem_SetRadius'

B2POLYGONSHAPE = \
	'_b2PolygonShape_CreateFixture_3', \
	'_b2PolygonShape_CreateFixture_4', \
	'_b2PolygonShape_CreateFixture_5', \
	'_b2PolygonShape_CreateFixture_6', \
	'_b2PolygonShape_CreateFixture_7', \
  '_b2PolygonShape_CreateFixture_8', \
	'_b2PolygonShape_CreateParticleGroup_4', \
	'_b2PolygonShape_DestroyParticlesInShape_4'

B2PRISMATICJOINT = \
	'_b2PrismaticJoint_EnableLimit', \
	'_b2PrismaticJoint_EnableMotor', \
	'_b2PrismaticJoint_GetJointTranslation', \
	'_b2PrismaticJoint_GetMotorSpeed', \
	'_b2PrismaticJoint_GetMotorForce', \
	'_b2PrismaticJoint_IsLimitEnabled', \
	'_b2PrismaticJoint_IsMotorEnabled', \
	'_b2PrismaticJoint_SetMotorSpeed', \
	'_b2PrismaticJointDef_Create', \
	'_b2PrismaticJointDef_InitializeAndCreate' \

B2PULLEYJOINT = \
	'_b2PulleyJointDef_Create', \
	'_b2PulleyJointDef_InitializeAndCreate' \

B2REVOLUTEJOINT = \
	'_b2RevoluteJoint_EnableLimit', \
	'_b2RevoluteJoint_EnableMotor', \
	'_b2RevoluteJoint_GetJointAngle', \
	'_b2RevoluteJoint_IsLimitEnabled', \
	'_b2RevoluteJoint_IsMotorEnabled', \
	'_b2RevoluteJoint_SetMotorSpeed', \
	'_b2RevoluteJointDef_Create', \
	'_b2RevoluteJointDef_InitializeAndCreate'

B2ROPEJOINT = \
	'_b2RopeJointDef_Create'

B2WELDJOINT = \
	'_b2WeldJointDef_Create', \
	'_b2WeldJointDef_InitializeAndCreate' \

B2WHEELJOINT = \
	'_b2WheelJoint_SetMotorSpeed', \
	'_b2WheelJoint_SetSpringFrequencyHz', \
	'_b2WheelJointDef_Create', \
	'_b2WheelJointDef_InitializeAndCreate' \

B2WORLD = \
	'_b2World_Create', \
	'_b2World_CreateBody', \
	'_b2World_CreateParticleSystem',\
	'_b2World_Delete', \
	'_b2World_DestroyBody', \
	'_b2World_DestroyJoint', \
	'_b2World_DestroyParticleSystem', \
	'_b2World_QueryAABB', \
	'_b2World_RayCast', \
	'_b2World_SetContactListener', \
	'_b2World_SetGravity', \
	'_b2World_Step'

EXPORTS = EXPORTED_FUNCTIONS="[ \
	'_GenerateOffsets', \
	$(B2BODY), \
	$(B2CHAINSHAPE), \
	$(B2CIRCLESHAPE), \
	$(B2COLLISION), \
	$(B2CONTACT), \
	$(B2EDGESHAPE), \
	$(B2DISTANCEJOINT), \
	$(B2FIXTURE), \
	$(B2FRICTIONJOINT), \
	$(B2GEARJOINT), \
	$(B2JOINT), \
	$(B2MOTORJOINT), \
	$(B2MOUSEJOINT), \
	$(B2PARTICLEGROUP), \
	$(B2PARTICLESYSTEM), \
	$(B2POLYGONSHAPE), \
	$(B2PRISMATICJOINT), \
	$(B2PULLEYJOINT), \
	$(B2REVOLUTEJOINT), \
	$(B2ROPEJOINT), \
	$(B2WELDJOINT), \
	$(B2WHEELJOINT), \
	$(B2WORLD) \
	]"

bindings.js:
	$(EMSCRIPTEN)/emcc -I../ -o lf_core.js jsBindings/jsBindings.cpp $(OBJECTS) -s $(EXPORTS) -s TOTAL_MEMORY=33554432 -O2 --js-library callbacks.js

