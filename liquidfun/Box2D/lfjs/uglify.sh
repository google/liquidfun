#!/bin/bash
java -jar $CLOSURE_JAR --language_in ECMASCRIPT5 \
--compilation_level WHITESPACE_ONLY \
./lf_core.js \
./jsBindings/offsets.js \
./jsBindings/Common/b2Math.js \
./jsBindings/Collision/b2Collision.js \
./jsBindings/Collision/Shapes/b2EdgeShape.js \
./jsBindings/Collision/Shapes/b2PolygonShape.js \
./jsBindings/Collision/Shapes/b2Shape.js \
./jsBindings/Collision/Shapes/b2ChainShape.js \
./jsBindings/Collision/Shapes/b2CircleShape.js \
./jsBindings/Dynamics/b2Body.js \
./jsBindings/Dynamics/b2World.js \
./jsBindings/Dynamics/Joints/b2WheelJoint.js \
./jsBindings/Dynamics/Joints/b2WeldJoint.js \
./jsBindings/Dynamics/Joints/b2GearJoint.js \
./jsBindings/Dynamics/Joints/b2Joint.js \
./jsBindings/Dynamics/Joints/b2FrictionJoint.js \
./jsBindings/Dynamics/Joints/b2RevoluteJoint.js \
./jsBindings/Dynamics/Joints/b2MotorJoint.js \
./jsBindings/Dynamics/Joints/b2PulleyJoint.js \
./jsBindings/Dynamics/Joints/b2DistanceJoint.js \
./jsBindings/Dynamics/Joints/b2PrismaticJoint.js \
./jsBindings/Dynamics/Joints/b2RopeJoint.js \
./jsBindings/Dynamics/Joints/b2MouseJoint.js \
./jsBindings/Dynamics/Contacts/b2Contact.js \
./jsBindings/Dynamics/b2Fixture.js \
./jsBindings/Dynamics/b2WorldCallbacks.js \
./jsBindings/Particle/b2ParticleSystem.js \
./jsBindings/Particle/b2ParticleGroup.js \
./jsBindings/Particle/b2Particle.js \
--js_output_file liquidfun.js
