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

### 0.9.1 TODO update prior to release.

#### Overview

TODO describe the cool new stuff we have in this release TODO

#### New Features

*   Added option to substep particle simulation independently of `b2World`.
*   Added barrier particles which can be used to prevent particle tunneling
    due to large particle velocities or simulation steps.
*   Added "Sparky" and "Sandbox" tests to the Testbed example to demonstrate
    particle and rigid body interaction.

#### API Modifications

*   `b2Draw::e_particleBit` flag now enables/disables
    `b2World::DrawDebugData()` drawing the particle system.
    
    To render the particle system using `b2World::DrawDebugData()`, the
    `b2Draw::e_particleBit` flag must be set on the `b2Draw` object passed
    to `b2World::SetDebugDraw()`.  It is possible to set flags on `b2Draw`
    objects using `b2Draw::SetFlags()`.
    
    Release [0.9.0][] unconditionally called `b2Draw::DrawParticles()` from
    `b2World::DrawDebugData()`.

#### Other Changes

*   Integrated [Box2D][] 2.3.0 release from [Box2D revision 280][].
*   Improved damping force to prevent particle explosions.
*   Fixed `b2Timer` bug that occurs when the usec component wraps.
*   Added `b2Stat` to calculate min/max/mean of a set of samples, and
    integrated this functionality into Testbed to measure performance.
*   Added operators to `b2ParticleColor`.
*   Cleaned up statistics display in Testbed.
*   Added build step to unit-test execution scripts.
*   Added generation of code coverage report for unit-tests.
*   Added a script to clean up temporary cmake files.
*   Increased warning level and enabled conversion of warnings to errors.
*   Added unit-test for b2IsValid().

### 0.9.0 <a name="0.9.0"></a>

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
  [Box2D]: http://box2d.org
  [Box2D revision 280]: http://code.google.com/p/box2d/source/detail?r=280
  [Box2D revision 255]: http://code.google.com/p/box2d/source/detail?r=255
