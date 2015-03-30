/*
 * Copyright (c) 2014 Google, Inc.
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

%module b2Body

/// We are going to use b2ParticleFlag as bitfields for C++ and Java, so it
/// makes more sense to just expose them as final ints in Java
%include "enums.swg"
%javaconst(1);

/// b2BodyDef class
// Ignore userData
%ignore b2BodyDef::userData;

/// b2Body class
/// C++ implementation does not have default public ctors and dtors. Don't
/// allow them as SWIG will add them otherwise and cause compile errors.
%nodefaultctor b2Body;      // don't create default constructors
%nodefaultdtor b2Body;      // don't create default destructors

// Suppress warnings for overloaded functions, because SWIG will pick the
// non-const version automatically, which is what we want anyway.
// We could suppress the warning in all of b2Body, but it's clearer this way.
%warnfilter(516) b2Body::GetFixtureList;
%warnfilter(516) b2Body::GetJointList;
%warnfilter(516) b2Body::GetContactList;
%warnfilter(516) b2Body::GetNext;
%warnfilter(516) b2Body::GetWorld;

// Ignore userData
%ignore b2Body::m_userData;
%ignore b2Body::GetUserData;
%ignore b2Body::SetUserData;

/// Include original .h file for SWIG
/// The original file is simple enough we will just pull it in
%include "../../../Box2D/Box2D/Dynamics/b2Body.h"
