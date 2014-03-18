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
