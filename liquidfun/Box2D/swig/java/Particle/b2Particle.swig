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

%module b2Particle

/// We are going to use b2ParticleFlag as bitfields for C++ and Java, so it
/// makes more sense to just expose them as final ints in Java
%include "enumtypeunsafe.swg"
%javaconst(1);

/// Ignore userdata pointers
%ignore b2ParticleDef::userData;

/// Ignore methods that generate new objects
%ignore b2ParticleColor::GetColor;

// SWIG will emit warning 401 about b2TypedIntrusiveListNode<b2ParticleHandle>
// even when we %ignore b2ParticleHandle. Filter the warning.
%warnfilter(401) b2ParticleHandle;
%ignore b2ParticleHandle;

/// Include original .h file for SWIG
/// The original file is simple enough we will just pull it in
%include "../../../Box2D/Box2D/Particle/b2Particle.h"
