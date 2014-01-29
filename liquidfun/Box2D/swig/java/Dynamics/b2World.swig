%module b2World

/// Forward declarations
class b2Body;

/// b2World class
class b2World {
public:
    ~b2World();

    void SetDebugDraw(b2Draw* debugDraw);

    b2Body* CreateBody(const b2BodyDef* def);
    void DestroyBody(b2Body* body);

    void Step(
            float32 timeStep,
            int32 velocityIterations,
            int32 positionIterations,
            int32 particleIterations);
    void DrawDebugData();

    int32 GetBodyCount() const;

    b2ParticleSystem* CreateParticleSystem(const b2ParticleSystemDef* def);
};

/// b2World extensions for easier Java access
%extend b2World {
    // Constructor without the need for creating b2Vec2
    b2World(float32 gravityX, float32 gravityY) {
        b2World* pWorld = new b2World(b2Vec2(gravityX, gravityY));
        return pWorld;
    }

    void SetGravity(float32 gravityX, float32 gravityY) {
        $self->SetGravity(b2Vec2(gravityX, gravityY));
    }
};
