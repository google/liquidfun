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
#include "Arrow.h"
#include "Render.h"

#include "GL/freeglut.h"

const float32 Arrow::k_size = 3.5f;
const b2Color Arrow::k_activeColor(0.0f, 1.0f, 0.0f);
const b2Color Arrow::k_passiveColor(0.5f, 0.5f, 0.5f);

// Construct an arrow.
// See m_angle, m_scale, m_position, m_identifier, m_viewCenter and
// m_extents for a description of this function's arguments.
Arrow::Arrow(const float32 angle, const float32 scale,
			 const b2Vec2 &position, const uint32 identifier,
			 const b2Vec2 *viewCenter, const b2Vec2 *extents)
{
	m_angle = angle;
	m_scale = scale;
	m_position = position;
	m_identifier = identifier;
	SetViewParameters(viewCenter, extents);
}

// Determine whether the specified position is within the bounding
// box that contains the arrow returning the identifier of this arrow
// or notSelectedIdentifier if the position is outside of the bounding
// box.
uint32 Arrow::Hit(const b2Vec2 &position, uint32 notSelectedIdentifier) const
{
	// Calculate the extent of the arrow's bounding box in viewport
	// coordinates.
	const float32 arrowExtent =
		CalculateScale() * Arrow::k_size * 0.5f;
	b2Vec2 arrowPosition;
	// Translate position into this arrow's space.
	const b2Vec2 relativePosition =
		position - *CalculateViewportPosition(&arrowPosition);
	// Determine whether position is within the arrow's bounding box.
	if (fabs(relativePosition.x) < arrowExtent &&
		fabs(relativePosition.y) < arrowExtent)
	{
		return m_identifier;
	}
	return notSelectedIdentifier;
}

// Draw this arrow using Arrow::k_activeColor if selectedIdentifier
// matches this arrow's identifier.  If this arrow isn't selected
// it's drawn using Arrow::k_passiveColor.
void Arrow::Draw(const uint32 selectedIdentifier) const
{
	b2Vec2 position;
	DrawArrow(selectedIdentifier == m_identifier ?
			  Arrow::k_activeColor : Arrow::k_passiveColor,
			  m_angle, CalculateScale(),
			  *CalculateViewportPosition(&position));
}

// Set the view center and extents.
void Arrow::SetViewParameters(const b2Vec2 *viewCenter, const b2Vec2 *extents)
{
	b2Assert(viewCenter);
	m_viewCenter = viewCenter;
	b2Assert(extents);
	m_extents = extents;
}

// Calculate the scaling factor for the arrow given the current
// viewport extents.
float32 Arrow::CalculateScale() const
{
	// Determine the minimum of a viewport dimension.  Extents are
	// defined as half the viewport width / height so multiply by
	// 2.0f to get the complete dimension.
	const float32 minDimension =
		std::min(m_extents->x, m_extents->y) * 2.0f;
	// Calculate the scaling factor for the arrow in world space.
	return (minDimension / Arrow::k_size) * m_scale;
}

// Calculate the viewport position of the arrow.
b2Vec2* Arrow::CalculateViewportPosition(
	b2Vec2 * const viewportPosition) const
{
	b2Assert(viewportPosition);
	viewportPosition->x =
		m_viewCenter->x + (m_extents->x * m_position.x);
	viewportPosition->y =
		m_viewCenter->y + (m_extents->y * m_position.y);
	return viewportPosition;
}

// Draw a colored arrow rotated by angle, scaled by scale and at
// the viewport relative position using DebugDraw.
// With no transformation matrix applied, the arrow is drawn in box
// area (3.5f, 3.5f) (see Arrow::k_size) and the overall bounding box
// of the arrow is (-1.75f, -1.75f) to (1.75f, 1.75f).
void Arrow::DrawArrow(const b2Color &color, const float32 angle,
					  const float32 scale, const b2Vec2 &position)
{
	static const b2Vec2 square[] =
	{
		b2Vec2(0.25f, 1.0f), b2Vec2(0.25f, -1.0f),
		b2Vec2(-1.75f, -1.0f), b2Vec2(-1.75f, 1.0f)
	};
	static const b2Vec2 triangle[] =
	{
		b2Vec2(0.25f, 1.75f), b2Vec2(1.75f, 0.0f),
		b2Vec2(0.25f, -1.75f)
	};
	// Build the transformation matrix.
	glPushMatrix();
	glTranslatef(position.x, position.y, 0.0f);
	glRotatef(angle, 0.0f, 0.0f, 1.0f);
	glScalef(scale, scale, 1.0f);
	// Draw the arrow.
	DebugDraw dbgDraw;
	dbgDraw.DrawFlatPolygon(square, B2_ARRAY_SIZE(square), color);
	dbgDraw.DrawFlatPolygon(triangle, B2_ARRAY_SIZE(triangle), color);
	glPopMatrix();
}
