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

/// b2ParticleGroupDef extensions for easier Java access
%extend b2ParticleGroupDef {
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

/// Ignore userdata pointers
%ignore b2ParticleGroupDef::userData;

/// Ignore b2ParticleGroup class in the .h file because it's too complex
/// A simple SWIG interface is provided before this
%ignore b2ParticleGroup;

/// Include original .h file for SWIG
/// The original file is simple enough we will just pull it in
%include "../../../Box2D/Box2D/Particle/b2ParticleGroup.h"
