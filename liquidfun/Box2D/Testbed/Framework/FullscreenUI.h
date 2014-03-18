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
#ifndef FULLSCREEN_UI_H
#define FULLSCREEN_UI_H
#include <Box2D/Box2D.h>
#include <string>

class Arrow;

// Handles drawing and selection of full screen UI.
class FullscreenUI
{
public:
	// Currently selected Arrow widget.
	enum Selection
	{
		e_SelectionTestPrevious = 0,
		e_SelectionTestNext,
		e_SelectionParameterPrevious,
		e_SelectionParameterNext,
		e_SelectionNone,
	};

	FullscreenUI();

	// Reset the UI to it's initial state.
	void Reset();

	// Get whether the fullscreen UI is enabled.
	bool GetEnabled() const { return m_enabled; }

	// Enable / disable the fullscreen UI.
	void SetEnabled(bool enable) { m_enabled = enable; }

	// Process mouse events and perform arrow selection returning the
	// ID of the selected arrow if it has been clicked.
	uint32 Mouse(const int32 button, const int32 state,
				 const int32 previousState, const b2Vec2 &mousePosition);

	// Get the currently selected arrow.
	uint32 GetSelection() const { return m_selection; }

	// Enable / disable particle parameter selection.
	void SetParticleParameterSelectionEnabled(const bool enable)
	{
		m_particleParameterSelectionEnabled = enable;
	}

	// Get whether particle parameter selection is enabled.
	bool GetParticleParameterSelectionEnabled() const
	{
		return m_particleParameterSelectionEnabled;
	}

	// Draw test + parameter selection arrows and the specified string
	// at the bottom of the screen.
	void Draw(const std::string &footer);

	// Set the view parameters for all widgets in the UI.
	void SetViewParameters(const b2Vec2 *viewCenter, const b2Vec2 *extents);

private:
	// Draw k_arrows.
	void DrawArrows();

	// Draw text at the bottom of the screen.
	void DrawFooterText(const std::string &text);

private:
	// Whether the UI is enabled.
	bool m_enabled;
	// Currently selected arrow.
	uint32 m_selection;
	// Whether particle parameters are enabled.
	bool m_particleParameterSelectionEnabled;
	// Ratio of width to height.
	float32 m_aspectRatio;

	// Whether the full screen UI is enabled by default.
	static const bool k_enabledDefault;
	// Default scaling factor for an arrow (see m_scale).
	static const float32 k_arrowScale;
	// Scaling factor for a small arrow (see m_scale).
	static const float32 k_arrowScaleSmall;
	// Spacing between arrows and the edge of the screen.  For the scale of
	// this coordinate system see Arrow::m_position.
	static const float32 k_arrowSpacing;
	// Location test change arrows are placed relative to the left / right
	// borders of the viewport.
	static const float32 k_arrowTestX;
	// Location particle parameter arrows are placed relative to the
	// left / right borders of the viewport.
	static const float32 k_arrowParticleParameterX;
	// Arrows to draw on the screen.
	static Arrow s_arrows[];
	// Number of arrows in s_arrows, since s_arrows is an incomplete type
	// when it is referenced in this class.
	static const uint32 k_numArrows;
};

#endif  // FULLSCREEN_UI_H
