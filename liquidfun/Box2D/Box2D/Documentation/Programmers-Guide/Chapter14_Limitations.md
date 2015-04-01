## Limitations

LiquidFun uses several approximations to simulate rigid body physics
efficiently. This brings some limitations.

Here are the current limitations:

1. Stacking heavy bodies on top of much lighter bodies is not stable.
Stability degrades as the mass ratio passes 10:1.

2. Chains of bodies connected by joints may stretch if a lighter body is
supporting a heavier body. For example, a wrecking ball connect to a chain of
light weight bodies may not be stable. Stability degrades as the mass ratio
passes 10:1.

3. There is typically around 0.5cm of slop in shape versus shape collision.

4. Continuous collision does not handle joints. So you may see joint
stretching on fast moving objects.

5. LiquidFun uses the symplectic Euler integration scheme. It does not
reproduce parabolic motion of projectiles and has only first-order accuracy.
However it is fast and has good stability.

6. LiquidFun uses an iterative solver to provide real-time performance. You
will not get precisely rigid collisions or pixel perfect accuracy. Increasing
the iterations will improve accuracy.


*This content is licensed under
[Creative Commons Attribution 4.0](http://creativecommons.org/licenses/by/4.0/legalcode).
For details and restrictions, please see the
[Content License](md__content_license.html).*
