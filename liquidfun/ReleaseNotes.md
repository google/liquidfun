# Release Notes

This document describes the changes to [LiquidFun][] present in each release.
Releases are listed in reverse chronological order, with the most recent
release at the top of page and the oldest release at the bottom.

Each release comprises the following sections:
*   "Overview", which contains a high level description of the release.
*   "New Features", which describes new functionality added in the release.
*   "API Modifications", which details the set of API changes that could affect
    existing users' code.
*   "Other Changes", which lists all other changes present in the release.

View the entire set of changes in a release of LiquidFun by doing one of the
following:
*   Checkout the [LiquidFun source][] and run
    `git log --oneline --decorate=full` to view the list of changes.
*   Go to http://github.com/google/liquidfun/commits/vVERSION_NUMBER where
    *VERSION_NUMBER* is the version number of the release.
    For example, go to http://github.com/google/liquidfun/commits/v0.9.0 to
    view all changes present in the [0.9.0][] release.


<a name="1.0.0"></a>
### 1.0.0

#### Overview

The [1.0.0 release][] of LiquidFun introduces a number of new features to the
API described <a href="#newfeatures1.0.0">below</a>.  Some of the highlights
are:

*   Multiple particle systems.
*   New particle behaviors: barrier, static-pressure, and repulsive.
*   Particle lifetimes.
*   Detection of stuck particles.
*   The ability to apply forces and impulses to particles.
*   Java support via SWIG.
*   A host of new demos: inside the existing Testbed application, and gorgeous
    new "EyeCandy" demo for Android.

Users of [0.9.0][] should carefully read through the
<a href="#apimods1.0.0">API Modifications</a> section to understand what they
need to change to get their existing applications to work with the latest
release of LiquidFun.

<a name="newfeatures1.0.0"></a>
#### New Features

*   Added option to substep particle simulation independently of `b2World`.
*   Added barrier particles which can be used to prevent particle tunneling
    due to large particle velocities or simulation steps.
*   Added static pressure particles to reduce compression of fluids.
*   Added repulsive particles to separate particle groups from each other.
*   Added support for multiple independent particle systems.
*   Added option to create particles in an existing particle group.
*   Added option to detect stuck particles.
*   Added option to pause particle simulations. Paused simluations don't
    consume cycles in b2World::Step.
*   Added option to enable a more strict check for Particle/Body interactions
    in cases where complex world geometry may cause false positive contacts.
*   Added option to call a contact listener or filter on particle / fixture
    or particle / particle interactions.
*   Added the ability to apply forces and impulses to particles.
*   Added "EyeCandy" sample to demonstrate different particle rendering methods
    on Android.
*   Added "Sparky", "Sandbox" and "Soup Stirrer" tests to the Testbed example
    to demonstrate particle and rigid body interaction.
*   Added "Ramp", "Pointy", and "AntiPointy" tests to the Testbed example to
    illustrate the difference with strict contact checking.
*   Added "Maxwell" test to the Testbed to demonstrate the direct modification
    of particle velocities.
*   Added "Fracker" test to demonstrate particle viscous fluids and particle
    destruction callbacks.
*   Added "Multiple Systems" test to the Testbed to demonstrate multiple
    particle systems.
*   Added Java Native Interface support via SWIG. It covers a subset of the
    API.
*   Added the ability to pass a list of shapes to be passed in for
    particle-group creation.

<a name="apimods1.0.0"></a>
#### API Modifications

*   `b2Draw::e_particleBit` flag now enables/disables
    `b2World::DrawDebugData()` drawing the particle system.
    
    To render the particle system using `b2World::DrawDebugData()`, the
    `b2Draw::e_particleBit` flag must be set on the `b2Draw` object passed
    to `b2World::SetDebugDraw()`.  It is possible to set flags on `b2Draw`
    objects using `b2Draw::SetFlags()`.
    
    Release [0.9.0][] unconditionally called `b2Draw::DrawParticles()` from
    `b2World::DrawDebugData()`.
*   Building LiquidFun on Linux, using `cmake -G"Unix Makefiles"`, now places
    output files in a build type sub-directory.

    For example, the output executable of the Testbed example is now
    written to Box2D/Testbed/Release/Testbed when building using the Release
    configuration.
*   Multiple independent particle systems can now exist, each with its own
    physical coefficients.  Previously, only one particle system existed, and
    it always existed. Use b2World::CreateParticleSystem to create particle
    systems, and b2World::DestroyParticleSystem to destroy them.
*   b2DestructionListener::SayGoodbye, b2QueryCallback::ReportParticle, and
    b2RayCastCallback::ReportParticle each now takes a b2ParticleSystem pointer
    as its first parameter. This change is necessary since multiple particle
    systems can now exist.
*   The particle system API has moved from b2World to b2ParticleSystem.
    For example, b2ParticleSystem::DestroyParticlesInShape replaces
    b2World::DestroyParticlesInShape.  You can use
    b2World::CreateParticleSystem or b2World::GetParticleSystemList() to
    acquire references to b2ParticleSystem.
*   Removed "Particle" from many of the functions in the particle system API.
    The word is redundant now that the API is under b2ParticleSystem. The API
    now has the following naming convention:
    *   Each function that takes a particle index or handle has a "Particle"
        component (e.g. "SetParticle").
    *   Each function that operates on the entire system has _no_ "Particle"
        component (e.g. "SetRadius").

            GetParticleMaxCount ==> GetMaxParticleCount
            SetParticleMaxCount ==> SetMaxParticleCount
            GetParticleDensity ==> GetDensity
            SetParticleDensity ==> SetDensity
            SetParticleGravityScale ==> SetGravityScale
            GetParticleGravityScale ==> GetGravityScale
            SetParticleDamping ==> SetDamping
            GetParticleDamping ==> GetDamping
            GetParticleStaticPressureIterations ==> GetStaticPressureIterations
            SetParticleStaticPressureIterations ==> SetStaticPressureIterations
            SetParticleRadius ==> SetRadius
            GetParticleRadius ==> GetRadius
            GetParticlePositionBuffer ==> GetPositionBuffer
            GetParticlePositionBuffer ==> GetPositionBuffer
            GetParticleVelocityBuffer ==> GetVelocityBuffer
            GetParticleVelocityBuffer ==> GetVelocityBuffer
            GetParticleColorBuffer ==> GetColorBuffer
            GetParticleColorBuffer ==> GetColorBuffer
            GetParticleGroupBuffer ==> GetGroupBuffer
            GetParticleGroupBuffer ==> GetGroupBuffer
            GetParticleUserDataBuffer ==> GetUserDataBuffer
            GetParticleUserDataBuffer ==> GetUserDataBuffer
            GetParticleFlagsBuffer ==> GetFlagsBuffer
            SetParticleFlagsBuffer ==> SetFlagsBuffer
            SetParticlePositionBuffer ==> SetPositionBuffer
            SetParticleVelocityBuffer ==> SetVelocityBuffer
            SetParticleColorBuffer ==> SetColorBuffer
            SetParticleUserDataBuffer ==> SetUserDataBuffer
            GetParticleContacts ==> GetContacts
            GetParticleContactCount ==> GetContactCount
            GetParticleBodyContacts ==> GetBodyContacts
            GetParticleBodyContactCount ==> GetBodyContactCount
            DestroyParticlesInGroup ==> DestroyParticles (moved to b2ParticleGroup)

*   Renamed `b2ParticleFlag` `b2DestructionListener` to
    `b2DestructionListenerParticle` to be consistent with other particle flag
    names.
*   Better particle-behavior stability.

#### Other Changes

*   Integrated [Box2D][] 2.3.0 release from [Box2D revision 280][].
*   Reduced the size of particles in Testbed particle tests to be consistent
    with the size of rigid bodies.
*   Improved damping force to prevent particle explosions.
*   Fixed `b2Timer` bug that occurs when the usec component wraps.
*   Added `b2Stat` to calculate min/max/mean of a set of samples, and
    integrated this functionality into Testbed to measure performance.
*   Fixed memory leaks of large blocks allocated by b2BlockAllocator.
*   Fixed incorrect stack-capacity calculation in b2VoronoiDiagram.
*   Added operators to `b2ParticleColor`.
*   Improved parameter and option selection in Testbed with the fullscreen UI.
*   Cleaned up statistics display in Testbed.
*   Fixed depth-buffer allocation when creating an empty particle group.
*   Added build step to unit-test execution scripts.
*   Added generation of code coverage report for unit-tests.
*   Added a script to clean up temporary cmake files.
*   Increased warning level and enabled conversion of warnings to errors.
*   Added unit-test for b2IsValid().
*   Added `b2World::QueryShapeAABB()` convenience function to query the world
    for objects in the AABB which encloses a shape.
*   Inlined some simple b2ParticleSystem and b2ParticleGroup functions.
*   Allowed particle systems to be culled from b2World::QueryAABB and
    b2World::RayCast.
*   Added callbacks so an application can override b2Alloc() and b2Free()
    without modifying the library.
*   Added valgrind support to unit-test runner on Linux / OSX.
*   Added cygwin wrapper script for build.bat and build_android.bat on Windows.
*   Added option to archive build artifacts from build scripts.
*   Changed Android C++ runtime library to gnustl_static.

<a name="0.9.0"></a>
### 0.9.0

#### Overview

The initial release of LiquidFun is available for download from the
[0.9.0 release][] release page.

#### New Features

*   Particle system added to [Box2D][].
*   Example applications ported to Android.

#### API Modifications

*   No changes to the [Box2D][] 2.3.0 pre-release API.

#### Other Changes

*   Integrated [Box2D][] 2.3.0 pre-release from [Box2D revision 255][].
*   Unit-tests implemented for the particle system.
*   Particle system tests added to the Testbed example application.
*   Build-system support for Android.
*   Moved documentation to Markdown/HTML.

  [LiquidFun]: ../../index.html
  [LiquidFun source]: http://github.com/google/liquidfun
  [0.9.0]: #0.9.0
  [0.9.0 release]: http://github.com/google/liquidfun/releases/tag/v0.9.0
  [1.0.0]: #1.0.0
  [1.0.0 release]: http://github.com/google/liquidfun/releases/tag/v1.0.0
  [apimods1.0.0]: #apimods1.0.0
  [newfeatures1.0.0]: #newfeatures1.0.0
  [Box2D]: http://box2d.org
  [Box2D revision 280]: http://code.google.com/p/box2d/source/detail?r=280
  [Box2D revision 255]: http://code.google.com/p/box2d/source/detail?r=255
