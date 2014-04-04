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

%module b2ParticleGroup

/// forward declarations
struct b2ParticleColor;
class b2Shape;

/// We are going to use b2ParticleFlag as b2ParticleGroupFlag for C++ and Java,
/// so it makes more sense to just expose them as final ints in Java
%include "enumtypeunsafe.swg"
%javaconst(1);

// Ignore internal enum flags
%ignore b2_particleGroupWillBeDestroyed;
%ignore b2_particleGroupNeedsUpdateDepth;
%ignore b2_particleGroupInternalMask;

// Userdata structure for b2ParticleGroupDef
// For safe access and memory cleanup.
%{
struct b2ParticleGroupDefUserData {
    b2ParticleGroupDefUserData() {
        circleShapeArray = NULL;
    }

    b2CircleShape* circleShapeArray;
};
%}

/// b2ParticleGroupDef extensions for easier Java access
/// Note: We use the userData pointer for setting b2ParticleGroupDef->shapes.
///       Please extend b2ParticleGroupDefUserData if you plan to use userData.
%extend b2ParticleGroupDef {
    // Clean up all pointers used for SetCircleShapes.
    // The userData is not freed as we might reuse it.
    void FreeShapesMemory() {
        b2ParticleGroupDefUserData* pgdUserData =
                (b2ParticleGroupDefUserData*) $self->userData;
        if (($self->userData != NULL)) {
            if (($self->shapes != NULL) &&
                (pgdUserData->circleShapeArray != NULL)) {
                delete[] $self->shapes;
                $self->shapes = NULL;
                delete[] pgdUserData->circleShapeArray;
                pgdUserData->circleShapeArray = NULL;
            }
        }
    }

    // Destructor. Free all memory used for SetCircleShapes and clean up
    // userData as needed.
    ~b2ParticleGroupDef() {
        b2ParticleGroupDef_FreeShapesMemory($self);
        if ($self->userData != NULL) {
            delete (b2ParticleGroupDefUserData*) $self->userData;
        }
    }

    // these won't override the default getters/setters
    // SWIG generates, but provides a more sane alternative
    // than to create (and have to destroy) throw away objects
    void SetPosition(float32 x, float32 y) {
        $self->position.Set(x, y);
    }

    void SetColor(int32 r, int32 g, int32 b, int32 a) {
        $self->color.Set(r, g, b, a);
    }

    // Typemaps for getting the raw address of output java.nio.ByteBuffer
    %typemap(jni) (void* inBuf) "jobject"
    %typemap(jtype) (void* inBuf) "java.nio.ByteBuffer"
    %typemap(jstype) (void* inBuf) "java.nio.ByteBuffer"
    %typemap(javain) (void* inBuf) "$javainput"
    %typemap(javaout) (void* inBuf) { return $jnicall; }

    // This extracts the Java ByteBuffer address from C++
    %typemap(in) (void* inBuf) {
        $1 = jenv->GetDirectBufferAddress($input);
        if ($1 == NULL) {
            SWIG_JavaThrowException(
                    jenv,
                    SWIG_JavaRuntimeException,
                    "Unable to get address of java.nio.ByteBuffer. "
                    "Is the ByteBuffer initialized?");
        }
    }

    // This uses the typemaps above to map a Java ByteBuffer into a C++
    // b2Shape buffer. We use ByteBuffers as they are more efficient.
    // The incoming Java ByteBuffer is assumed to contain a list of points,
    // and each point is 2 floats.
    // This function will allocate new C++ buffers to facilitate the
    // transformation from the list of points to actual CircleShapes.
    // Note: We use the userData pointer for this function.
    //       Please extend b2ParticleGroupDefUserData if you plan to use
    //       userData.
    void SetCircleShapes(void* inBuf, int bufStart, int numShapes,
                         float radius) {
        float* points = (float*) (((char*) inBuf) + bufStart);
        b2CircleShape* pCircleShapes= new b2CircleShape[numShapes];
        b2Shape** pShapes = new b2Shape*[numShapes];
        for (int i = 0; i < numShapes; ++i) {
            pCircleShapes[i].m_radius = radius;
            pCircleShapes[i].m_p = b2Vec2(points[i*2], points[i*2+1]);
            pShapes[i] = &pCircleShapes[i];
        }

        // Destroy existing buffers
        b2ParticleGroupDef_FreeShapesMemory($self);

        // Keep track of all the new arrays we allocated
        if ($self->userData == NULL) {
            $self->userData = (void*) new b2ParticleGroupDefUserData();
        }
        ((b2ParticleGroupDefUserData*) $self->userData)->circleShapeArray =
                pCircleShapes;
        $self->shapes = pShapes;
        $self->shapeCount = numShapes;
    }
}

/// b2ParticleGroup class
%nodefaultctor;      // don't create default constructors
%nodefaultdtor;      // don't create default destructors
class b2ParticleGroup {
public:
    const b2ParticleGroup* GetNext() const;
    int32 GetParticleCount() const;
    int32 GetBufferIndex() const;
    int32 GetGroupFlags() const;
};
%clearnodefaultdtor; // Re-enable default constructors
%clearnodefaultctor; // Re-enable default destructors

/// Ignore member variables so SWIG doesn't auto-generate get/setters
%ignore b2ParticleGroupDef::userData;
%ignore b2ParticleGroupDef::shapes;

/// Ignore b2ParticleGroup class in the .h file because it's too complex
/// A simple SWIG interface is provided before this
%ignore b2ParticleGroup;

/// Include original .h file for SWIG
/// The original file is simple enough we will just pull it in
%include "../../../Box2D/Box2D/Particle/b2ParticleGroup.h"

/// Clear the typemaps
%clear (void* inBuf);
