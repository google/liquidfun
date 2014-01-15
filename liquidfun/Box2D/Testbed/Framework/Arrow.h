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
#ifndef ARROW_H
#define ARROW_H
#include <Box2D/Box2D.h>


// Renders and performs hit tests for an arrow widget rendered in the
// viewport.
class Arrow
{
public:
	// Present angles used to rotate the arrow in different orientations.
	enum Angle {
		e_angleRight = 0,
		e_angleLeft = 180,
	};

	// Construct an arrow.
	// See m_angle, m_scale, m_position, m_identifier, m_viewCenter and
	// m_extents for a description of this function's arguments.
	// "viewCenter" and "extents" are directly referenced by this object so
	// must be present for the lifetime of this object.
	Arrow(const float32 angle, const float32 scale,
		  const b2Vec2 &position, const uint32 identifier,
		  const b2Vec2 *viewCenter, const b2Vec2 *extents);

	// Get the identifier of this arrow.
	uint32 GetIdentifier() const { return m_identifier; }

	// Determine whether the specified position is within the bounding
	// box that contains the arrow returning the identifier of this arrow
	// or notSelectedIdentifier if the position is outside of the bounding
	// box.
	uint32 Hit(const b2Vec2 &position, uint32 notSelectedIdentifier) const;

	// Draw this arrow using Arrow::k_activeColor if selectedIdentifier
	// matches this arrow's identifier.  If this arrow isn't selected
	// it's drawn using Arrow::k_passiveColor.
	void Draw(const uint32 selectedIdentifier) const;

	// Set the view center and extents.
	// "viewCenter" and "extents" are directly referenced by this object so
	// must be present for the lifetime of this object.
	void SetViewParameters(const b2Vec2 *viewCenter, const b2Vec2 *extents);

protected:
	// Calculate the scaling factor for the arrow given the current
	// viewport extents.
	float32 CalculateScale() const;

	// Calculate the viewport position of the arrow.
	b2Vec2* CalculateViewportPosition(
		b2Vec2 * const viewportPosition) const;

	// Draw a colored arrow rotated by angle, scaled by scale and at
	// the viewport relative position using DebugDraw.
	// With no transformation matrix applied, the arrow is drawn in box
	// area (3.5f, 3.5f) (see Arrow::k_size) and the overall bounding box
	// of the arrow is (-1.75f, -1.75f) to (1.75f, 1.75f).
	static void DrawArrow(
		const b2Color& color, const float32 angle, const float32 scale,
		const b2Vec2 &position);

private:
	// Angle of rotation of the arrow in degrees.
	float32 m_angle;
	// Scaling factor for the arrow rendered by DebugDraw::DrawArrow().
	// 0.0f is an infinitely small arrow and 1.0f is an arrow that fills
	// the screen.
	float32 m_scale;
	// Location of the arrow relative to the center of the view where
	// (0.0f, 0.0f) is the center of the view, (-1.0f, -1.0f) is the
	// bottom left corner of the view and (1.0f, 1.0f) is the top right
	// corner of the view.
	b2Vec2 m_position;
	// Identifier of this arrow.
	uint32 m_identifier;
	// Point which indicates the center of the view.
	const b2Vec2 *m_viewCenter;
	// Extents of the viewport which is the region
	// (-m_extents.x, -m_extents.y) to (m_extents.x, m_extents.y).
	const b2Vec2 *m_extents;

public:
	// Maximum size of the in world units rendered by
	// DebugDraw::DrawArrow().
	static const float32 k_size;

private:
	// Active (clicked / pressed) color of an arrow.
	static const b2Color k_activeColor;
	// Inactive color of an arrow.
	static const b2Color k_passiveColor;
};
#endif  // ARROW_H
