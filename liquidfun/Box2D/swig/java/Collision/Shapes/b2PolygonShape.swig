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

%module b2PolygonShape

/// Allow a constructor to be generated despite inheriting from an abstract
/// class with no default constructors
%feature("notabstract") b2PolygonShape;

class b2PolygonShape : public b2Shape {
public:
    b2PolygonShape();

    void SetAsBox(float32 hx, float32 hy);
    void SetCentroid(float32 x, float32 y);
    void SetAsBox(float32 hx,
                  float32 hy,
                  float32 centerX,
                  float32 centerY,
                  float32 angle);
};
