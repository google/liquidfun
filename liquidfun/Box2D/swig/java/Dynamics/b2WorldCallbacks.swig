%module(directors="1") b2WorldCallbacks;

%feature("director") b2QueryCallback;

/// Forward declarations
class b2ParticleSystem;

%nodefaultctor b2QueryCallback;
class b2QueryCallback
{
public:
    b2QueryCallback() {}
    virtual ~b2QueryCallback();
    virtual bool ReportFixture(b2Fixture* fixture) = 0;
    virtual bool ReportParticle(const b2ParticleSystem* particleSystem,
                                int32 index);
};
