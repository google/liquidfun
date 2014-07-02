# Common
[About](#about)<br/>
[Settings](#settings)<br/>
[Memory Management](#mm)<br/>
[Math](#math)

<a name="about">
## About

The Common module contains settings, memory management, and vector math.

<a name="settings">
## Settings

The header b2Settings.h contains:

* Types such as int32 and float32

* Constants

* Allocation wrappers

* The version number

### Types

LiquidFun defines various types such as float32, int8, etc. to make it easy to
determine the size of structures.

### Constants

LiquidFun defines several constants. These are all documented in b2Settings.h.
Normally you do not need to adjust these constants.

LiquidFun uses floating point math for collision and simulation. Due to
round-off error some numerical tolerances are defined. Some tolerances are
absolute and some are relative.  Absolute tolerances use MKS units.

### Allocation wrappers

The settings file defines b2Alloc and b2Free for large allocations. You may
forward these calls to your own memory management system.

### Version

The b2Version structure holds the current version so you can query this at
run-time.

<a name="mm">
## Memory Management

A large number of the decisions about the design of LiquidFun were based on
the need for quick and efficient use of memory. In this section I will discuss
how and why LiquidFun allocates memory.

LiquidFun tends to allocate a large number of small objects (around 50-300
bytes). Using the system heap through malloc or new for small objects is
inefficient and can cause fragmentation. Many of these small objects may have
a short life span, such as contacts, but can persist for several time steps.
So we need an allocator that can efficiently provide heap memory for these
objects.

LiquidFun's solution is to use a small object allocator (SOA) called
b2BlockAllocator. The SOA keeps a number of growable pools of varying sizes.
When a request is made for memory, the SOA returns a block of memory that best
fits the requested size. When a block is freed, it is returned to the pool.
Both of these operations are fast and cause little heap traffic.

Since LiquidFun uses a SOA, you should never new or malloc a body, fixture, or
joint. However, you do have to allocate a b2World on your own. The b2World
class provides factories for you to create bodies, fixtures, and joints. This
allows LiquidFun to use the SOA and hide the gory details from you. Never,
call delete or free on a body, fixture, or joint.

While executing a time step, LiquidFun needs some temporary workspace memory.
For this, it uses a stack allocator called b2StackAllocator to avoid per-step
heap allocations. You don't need to interact with the stack allocator, but
it's good to know it's there.

<a name="math">
## Math

LiquidFun includes a simple small vector and matrix module. This has been
designed to suit the internal needs of LiquidFun and the API. All the members
are exposed, so you may use them freely in your application.

The math library is kept simple to make LiquidFun easy to port and maintain.


*This content is licensed under
[Creative Commons Attribution 4.0](http://creativecommons.org/licenses/by/4.0/legalcode).
For details and restrictions, please see the
[Content License](md__content_license.html).*
