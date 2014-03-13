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

%module b2ParticleSystem

// error enums and strings for Java exceptions
%{
enum b2ExceptionType {
    BUFFER_TOO_SMALL,
    PARTICLE_INDEX_OUTOFBOUNDS,
    NUM_ERRORS,
    NO_EXCEPTIONS,
};

struct b2Exception {
    const char* jniType;
    const char* errorMsg;
} g_liquidfunExceptions[NUM_ERRORS] = {
    {"java/lang/IllegalArgumentException",
         "Supplied buffer is too small"},
    {"java/lang/ArrayIndexOutOfBoundsException",
        "Particle index is out of bounds. Check startIndex and numParticles."},
};
%}

// Disallow constructors and destructors; only made through b2World
%nodefaultctor b2ParticleSystem;
%nodefaultdtor b2ParticleSystem;
class b2ParticleSystem {
public:
    int32 CreateParticle(const b2ParticleDef& def);
    void JoinParticleGroups(b2ParticleGroup* groupA, b2ParticleGroup* groupB);
    const b2ParticleGroup* GetParticleGroupList() const;
    void DestroyParticlesInShape(const b2Shape& shape, const b2Transform& xf);
    b2ParticleGroup* CreateParticleGroup (const b2ParticleGroupDef &def);
    int32 GetParticleGroupCount() const;
    int32 GetParticleCount() const;
    void SetDamping(float32 damping);
    void SetRadius(float32 radius);

    void QueryShapeAABB(
            b2QueryCallback* callback,
            const b2Shape& shape,
            const b2Transform& xf) const;
};


/// b2ParticleSystem extensions for easier Java access
%extend b2ParticleSystem {
    void SetParticleVelocity(int32 index, float32 vx, float32 vy) {
        b2Vec2& v = $self->GetVelocityBuffer()[index];
        v.x = vx;
        v.y = vy;
    }

    float GetParticlePositionX(int32 index) {
        float x = $self->GetPositionBuffer()[index].x;
        return x;
    }

    float GetParticlePositionY(int32 index) {
        float y = $self->GetPositionBuffer()[index].y;
        return y;
    }

    // Typemaps for getting the raw address of output java.nio.ByteBuffer
    // This is mapping all function arguments with the signature
    //     (void* outBuf, int size)
    // to different Java intermediate ByteBuffer types.
    // See the Get*Buffer() below for examples.
    %typemap(jni) (void* outBuf, int size) "jobject"
    %typemap(jtype) (void* outBuf, int size) "java.nio.ByteBuffer"
    %typemap(jstype) (void* outBuf, int size) "java.nio.ByteBuffer"
    %typemap(javain) (void* outBuf, int size) "$javainput"
    %typemap(javaout) (void* outBuf, int size) {
        return $jnicall;
    }
    // This extracts the Java ByteBuffer address from C++
    %typemap(in) (void* outBuf, int size) {
        $1 = jenv->GetDirectBufferAddress($input);
        $2 = jenv->GetDirectBufferCapacity($input);
        if (($1 == NULL) && ($2 != 0)) {
            SWIG_JavaThrowException(
                    jenv,
                    SWIG_JavaRuntimeException,
                    "Unable to get address of java.nio.ByteBuffer. "
                    "Is the ByteBuffer initialized?");
        }
    }
    // This checks the output of the C++ functions for exceptions
    %typemap(argout) (void* outBuf, int size) {
        if($result != NO_EXCEPTIONS) {
            jclass clazz = jenv->FindClass(
                    g_liquidfunExceptions[$result].jniType);
            jenv->ThrowNew(clazz, g_liquidfunExceptions[$result].errorMsg);
        }
    }

    // Helper function for checking validity of buffer size
    b2ExceptionType IsBufCopyValid(
            int startIndex, int numParticles, int copySize, int bufSize) {
        const int maxNumParticles = $self->GetParticleCount();
        // are we actually copying?
        if (copySize == 0) {
            return NO_EXCEPTIONS;
        }
        // is the index out of bounds?
        if ((startIndex < 0) || (startIndex >= maxNumParticles) ||
                (numParticles < 0) ||
                (numParticles + startIndex > maxNumParticles)) {
            return PARTICLE_INDEX_OUTOFBOUNDS;
        }
        // are we copying within the boundaries?
        if (copySize > bufSize) {
            return BUFFER_TOO_SMALL;
        }
        return NO_EXCEPTIONS;
    }

    int GetPositionBuffer(
            int startIndex, int numParticles, void* outBuf, int size) {
        int copySize = numParticles * sizeof(b2Vec2);
        b2ExceptionType isValid = b2ParticleSystem_IsBufCopyValid(
                $self, startIndex, numParticles, copySize, size);
        if (isValid != NO_EXCEPTIONS) {
            return isValid;
        }
        memcpy(outBuf,
                $self->GetPositionBuffer() + startIndex, copySize);
        return NO_EXCEPTIONS;
    }

    int GetColorBuffer(
            int startIndex, int numParticles, void* outBuf, int size) {
        int copySize = numParticles * sizeof(b2ParticleColor);
        b2ExceptionType isValid = b2ParticleSystem_IsBufCopyValid(
                $self, startIndex, numParticles, copySize, size);
        if (isValid != NO_EXCEPTIONS) {
            return isValid;
        }
        memcpy(outBuf, $self->GetColorBuffer() + startIndex, copySize);
        return NO_EXCEPTIONS;
    }
};

/// Ignore other classes
%ignore b2ParticleContact;
%ignore b2ParticleBodyContact;

// Ignore the original version of b2ParticleSystem as it's too complex
%ignore b2ParticleSystem;

/// Include original .h file for SWIG
/// The original file is simple enough we will just pull it in
%include "../../../Box2D/Box2D/Particle/b2ParticleSystem.h"
