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

%module liquidfun

%{
#include "Box2D/Box2D.h"
%}

// allow const reference typemaps
%naturalvar;

// typedef primitive types
typedef float float32;
typedef signed int int32;
typedef unsigned int uint32;
typedef unsigned char uint8;

// rename class names to Java convention
%rename("%(strip:[b2])s") "";

// rename functions to Java convention
%rename("%(lowercamelcase)s", %$isfunction) "";

// rename member varibles to Java convention
%rename("%(strip:[m_])s", %$isvariable) "";

// rename enums; not quite Java convention but close enough.
%rename("%(strip:[b2_])s", %$isenumitem) "";

/// Ignore most operators as they generate new Java or native objects.
/// Note that swig will return a new Java object when a reference is returned,
/// which could lead to multiple Java proxy objects pointing to the same C-
/// memory.
/// Alas, there doesn't seem to be any wildcard symbols other than *, which
/// doesn't work in this case (operator* maps to the * operator).
%ignore operator=;
%ignore operator*=;
%ignore operator*;
%ignore operator+=;
%ignore operator+;
%ignore operator-=;
%ignore operator-;

// rename the == operator
%rename(equals) operator==;

// We need to include b2Settings.h separately for swig to parse certain macros
%include "Common/b2Settings.swig"

// Include all the swig files
%include "Collision/Shapes/b2Shape.swig" // include before derived classes
%include "Collision/Shapes/b2CircleShape.swig"
%include "Collision/Shapes/b2EdgeShape.swig"
%include "Collision/Shapes/b2PolygonShape.swig"

%include "Common/b2Draw.swig"
%include "Common/b2Math.swig"

%include "Dynamics/b2Body.swig"
%include "Dynamics/b2Fixture.swig"
%include "Dynamics/b2World.swig"
%include "Dynamics/b2WorldCallbacks.swig"

%include "Particle/b2Particle.swig"
%include "Particle/b2ParticleGroup.swig"
%include "Particle/b2ParticleSystem.swig"
