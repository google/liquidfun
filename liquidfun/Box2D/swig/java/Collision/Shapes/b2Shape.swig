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

%module b2Shape

%include "enums.swg"
%javaconst(1);

/// Rename the enums as SWIG doesn't allow multiple rename rules except with
/// a script that can slow SWIG down quite a bit.
%rename (CIRCLE) e_circle;
%rename (EDGE) e_edge;
%rename (POLYGON) e_polygon;
%rename (CHAIN) e_chain;

/// Ignore functions that return new objects.
%ignore b2Shape::Clone;

/// Ignore functions that have out params.
%ignore b2Shape::ComputeDistance;
%ignore b2Shape::RayCast;
%ignore b2Shape::ComputeAABB;

/// SWIG auto-generates get/setters for public member variables.
/// This function will be duplicated as there's an m_type variable, and SWIG
/// will auto-generate GetType() from it.
%ignore b2Shape::GetType;

/// C++ implementation does not have default public ctors. Don't allow them as
/// SWIG will add them otherwise and cause compile errors.
%nodefaultctor;      // don't create default constructors

/// Include original .h file for SWIG
/// The original file is simple enough we will just pull it in
%include "../../../Box2D/Box2D/Collision/Shapes/b2Shape.h"

%clearnodefaultctor; // Re-enable default constructors
