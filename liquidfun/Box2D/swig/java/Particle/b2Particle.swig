%module b2Particle

/// We are going to use b2ParticleFlag as bitfields for C++ and Java, so it
/// makes more sense to just expose them as final ints in Java
%include "enumtypeunsafe.swg"
%javaconst(1);

/// b2ParticleDef extensions for easier Java access
%extend b2ParticleDef {
    // these won't override the default getters/setters
    // SWIG generates, but provides a more sane alternative
    // than to create (and have to destroy) throw away objects
    void setPosition(float32 x, float32 y) {
        $self->position.Set(x, y);
    }

    void setColor(int32 r, int32 g, int32 b, int32 a) {
        $self->color.Set(r, g, b, a);
    }
}

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
