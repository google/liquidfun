# Debug Drawing

You can implement the b2DebugDraw class to get detailed drawing of the physics
world. Here are the available entities:

* shape outlines

* joint connectivity

* broad-phase axis-aligned bounding boxes (AABBs)

* center of mass

<img align="center" src="image_24.png" alt="Debug drawing" height="306"
width="431"><br/>
This is the preferred method of drawing these physics entities, rather than
accessing the data directly. The reason is that much of the necessary data is
internal and subject to change.

The testbed draws physics entities using the debug draw facility and the
contact listener, so it serves as the primary example of how to implement
debug drawing as well as how to draw contact points.


*This content is licensed under
[Creative Commons Attribution 4.0](http://creativecommons.org/licenses/by/4.0/legalcode).
For details and restrictions, please see the
[Content License](md__content_license.html).*
