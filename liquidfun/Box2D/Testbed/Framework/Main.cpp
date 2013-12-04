/*
* Copyright (c) 2006-2007 Erin Catto http://www.box2d.org
* Copyright (c) 2013 Google, Inc.
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

#include "Render.h"
#include "Test.h"
#include "glui/glui.h"

#include <cstdio>
#include <sstream>
using namespace std;

namespace
{
	int32 testIndex = 0;
	int32 testSelection = 0;
	int32 testCount = 0;
	TestEntry* entry;
	Test* test;
	Settings settings;
	int32 width = 640;
	int32 height = 480;
	int32 framePeriod = 16;
	int32 mainWindow;
	float settingsHz = 60.0;
	GLUI *glui = NULL;
	float32 viewZoom = 1.0f;
	int tx, ty, tw, th;
	bool rMouseDown = false;
	bool lMouseDown = false;
	b2Vec2 lastp;
	b2Vec2 extents;
#ifdef __ANDROID__
	const float arrowScale = 2.5;  // relative to worldspace
	const float arrowSize = 3.5; // see geometry in DrawArrow()
	const float arrowOffset = arrowScale * arrowSize; // defines hitbox & position
	const float smallerArrowFactor = 0.5f;
	const float arrowGap = 3;
	enum ArrowSelection
	{
		e_ArrowSelectionNone = 0,
		e_ArrowSelectionLeft,
		e_ArrowSelectionRight,
		e_ArrowParameterLeft,
		e_ArrowParameterRight,
	};
	ArrowSelection whichArrow = e_ArrowSelectionNone;
	int parameterIndex = 0;
	bool parameterChanged = false;
	bool extraArrows = false;
	const char *parameterName = "";
	const b2Color arrowActiveColor(0, 1, 0);
	const b2Color arrowPassiveColor(0.5f, 0.5f, 0.5f);
#endif // __ANDROID__
}

static void Resize(int32 w, int32 h)
{
	width = w;
	height = h;

	GLUI_Master.get_viewport_area(&tx, &ty, &tw, &th);
	glViewport(tx, ty, tw, th);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	float32 ratio = th ? float32(tw) / float32(th) : 1;

	extents = ratio >= 1 ? b2Vec2(ratio * 25.0f, 25.0f) : b2Vec2(25.0f, 25.0f / ratio);
	extents *= viewZoom;

	b2Vec2 lower = settings.viewCenter - extents;
	b2Vec2 upper = settings.viewCenter + extents;

	// L/R/B/T
	LoadOrtho2DMatrix(lower.x, upper.x, lower.y, upper.y);
}

static b2Vec2 ConvertScreenToWorld(int32 x, int32 y)
{
	float32 u = x / float32(tw);
	float32 v = (th - y) / float32(th);

	b2Vec2 lower = settings.viewCenter - extents;
	b2Vec2 upper = settings.viewCenter + extents;

	b2Vec2 p;
	p.x = (1.0f - u) * lower.x + u * upper.x;
	p.y = (1.0f - v) * lower.y + v * upper.y;
	return p;
}

// This is used to control the frame rate (60Hz).
static void Timer(int)
{
	glutSetWindow(mainWindow);
	glutPostRedisplay();
	glutTimerFunc(framePeriod, Timer, 0);
}

int TestParticleType()
{
#ifdef __ANDROID__
	extraArrows = true;
	static int flags[] = {
		b2_waterParticle,
		b2_elasticParticle,
		b2_powderParticle,
		b2_springParticle,
		b2_tensileParticle,
		b2_viscousParticle,
		b2_wallParticle,
	};
	static const char *flagNames[] = {
		"water",
		"elastic",
		"powder",
		"spring",
		"tensile",
		"viscous",
		"wall",
	};
	int whichFlag = parameterIndex % (sizeof(flags) / sizeof(int));
	parameterName = flagNames[whichFlag];
	return flags[whichFlag];
#endif // __ANDROID__
	return 0;
}

#ifdef __ANDROID__
void Arrow(ArrowSelection as, int dir, float angle, float scale, float offset2)
{
	DebugDraw dbgDraw;
	glPushMatrix();
	glTranslatef(settings.viewCenter.x + (extents.x - arrowOffset + arrowScale - offset2) * dir, settings.viewCenter.y, 0);
	glRotatef(angle, 0, 0, 1);
	glScalef(arrowScale * scale, arrowScale * scale, 1);
	dbgDraw.DrawArrow(lMouseDown && whichArrow == as ? arrowActiveColor : arrowPassiveColor);
	glPopMatrix();
}
#endif // __ANDROID__

static void SimulationLoop()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	settings.hz = settingsHz;

	// call this each frame, to function correctly with devices that may recreate the GL Context without us asking for it
	Resize(width, height);

	test->Step(&settings);

#ifdef __ANDROID__
	// special purpose code for Android: draw navigational arrows to browse test cases, since we don't have the full desktop UI
	Arrow(e_ArrowSelectionRight, 1, 0, 1, 0);
	Arrow(e_ArrowSelectionLeft, -1, 180, 1, 0);
	string msg = entry->name;
	if (extraArrows)
	{
		Arrow(e_ArrowParameterRight, 1, 0, smallerArrowFactor, arrowOffset * smallerArrowFactor + arrowGap);
		Arrow(e_ArrowParameterLeft, -1, 180, smallerArrowFactor, arrowOffset * smallerArrowFactor + arrowGap);
		msg += " : ";
		msg += parameterName;
	}

	glColor4f(1, 1, 1, 1);
	glPushMatrix();
	glTranslatef(-20, -4, 0);
	glScalef(0.015f, 0.015f, 1);
	float msec = ComputeFPS();
	std::stringstream ss;
	ss << int(1000 / msec);
	msg += " / " + ss.str() + " fps";
	glutStrokeString(GLUT_STROKE_MONO_ROMAN, (const unsigned char *)msg.c_str());
	glPopMatrix();

	if (parameterChanged) { parameterChanged = false; testIndex = -1; } // force test restart below
#endif // __ANDROID__

	test->DrawTitle(entry->name);

	glutSwapBuffers();

	if (testSelection != testIndex)
	{
		testIndex = testSelection;
		delete test;
		entry = g_testEntries + testIndex;
		test = entry->createFcn();
		viewZoom = 1.0f;
		settings.viewCenter.Set(0.0f, 20.0f);
		Resize(width, height);
	}
}

static void Keyboard(unsigned char key, int x, int y)
{
	B2_NOT_USED(x);
	B2_NOT_USED(y);

	switch (key)
	{
	case 27:
#ifndef __APPLE__
		// freeglut specific function
		glutLeaveMainLoop();
#endif
		exit(0);
		break;

		// Press 'z' to zoom out.
	case 'z':
		viewZoom = b2Min(1.1f * viewZoom, 20.0f);
		Resize(width, height);
		break;

		// Press 'x' to zoom in.
	case 'x':
		viewZoom = b2Max(0.9f * viewZoom, 0.02f);
		Resize(width, height);
		break;

		// Press 'r' to reset.
	case 'r':
		delete test;
		test = entry->createFcn();
		break;

		// Press space to launch a bomb.
	case ' ':
		if (test)
		{
			test->LaunchBomb();
		}
		break;

	case 'p':
		settings.pause = !settings.pause;
		break;

		// Press [ to prev test.
	case '[':
		--testSelection;
		if (testSelection < 0)
		{
			testSelection = testCount - 1;
		}
		if (glui) glui->sync_live();
		break;

		// Press ] to next test.
	case ']':
		++testSelection;
		if (testSelection == testCount)
		{
			testSelection = 0;
		}
		if (glui) glui->sync_live();
		break;

	default:
		if (test)
		{
			test->Keyboard(key);
		}
	}
}

static void KeyboardSpecial(int key, int x, int y)
{
	B2_NOT_USED(x);
	B2_NOT_USED(y);

	int mod = glutGetModifiers();

	switch (key)
	{
		// Press left to pan left.
	case GLUT_KEY_LEFT:
		if (mod == GLUT_ACTIVE_CTRL)
		{
			b2Vec2 newOrigin(2.0f, 0.0f);
			test->ShiftOrigin(newOrigin);
		}
		else
		{
			settings.viewCenter.x -= 0.5f;
			Resize(width, height);
		}
		break;

		// Press right to pan right.
	case GLUT_KEY_RIGHT:
		if (mod == GLUT_ACTIVE_CTRL)
		{
			b2Vec2 newOrigin(-2.0f, 0.0f);
			test->ShiftOrigin(newOrigin);
		}
		else
		{
			settings.viewCenter.x += 0.5f;
			Resize(width, height);
		}
		break;

		// Press down to pan down.
	case GLUT_KEY_DOWN:
		if (mod == GLUT_ACTIVE_CTRL)
		{
			b2Vec2 newOrigin(0.0f, 2.0f);
			test->ShiftOrigin(newOrigin);
		}
		else
		{
			settings.viewCenter.y -= 0.5f;
			Resize(width, height);
		}
		break;

		// Press up to pan up.
	case GLUT_KEY_UP:
		if (mod == GLUT_ACTIVE_CTRL)
		{
			b2Vec2 newOrigin(0.0f, -2.0f);
			test->ShiftOrigin(newOrigin);
		}
		else
		{
			settings.viewCenter.y += 0.5f;
			Resize(width, height);
		}
		break;

		// Press home to reset the view.
	case GLUT_KEY_HOME:
		viewZoom = 1.0f;
		settings.viewCenter.Set(0.0f, 20.0f);
		Resize(width, height);
		break;
	}
}

static void KeyboardUp(unsigned char key, int x, int y)
{
	B2_NOT_USED(x);
	B2_NOT_USED(y);

	if (test)
	{
		test->KeyboardUp(key);
	}
}

static void Mouse(int32 button, int32 state, int32 x, int32 y)
{
	// Use the mouse to move things around.
	if (button == GLUT_LEFT_BUTTON)
	{
		int mod = glutGetModifiers();
		b2Vec2 p = ConvertScreenToWorld(x, y);
		if (state == GLUT_DOWN)
		{
			lMouseDown = true;
			b2Vec2 p = ConvertScreenToWorld(x, y);
			if (mod == GLUT_ACTIVE_SHIFT)
			{
				test->ShiftMouseDown(p);
			}
			else
			{
				test->MouseDown(p);
			}
		}

		if (state == GLUT_UP)
		{
			lMouseDown = false;
			test->MouseUp(p);
		}

#ifdef __ANDROID__
		// Allow Android to advance to the next test.
		bool withinYrange = fabs(p.y - settings.viewCenter.y) < arrowScale * 2;
		if (withinYrange && p.x < -(extents.x - arrowOffset))
		{
			whichArrow = e_ArrowSelectionLeft;
			if (state == GLUT_UP)
			{
				testSelection = max(0, testSelection - 1);
				extraArrows = false;
				parameterIndex = 0;
			}
		}
		else if (withinYrange && p.x >  (extents.x - arrowOffset))
		{
			whichArrow = e_ArrowSelectionRight;
			if (state == GLUT_UP)
			{
				testSelection++;
				if (!g_testEntries[testSelection].name) testSelection--;
				extraArrows = false;
				parameterIndex = 0;
			}
		}
		else if (extraArrows && withinYrange && p.x < -(extents.x - arrowOffset - arrowOffset * smallerArrowFactor - arrowGap))
		{
			whichArrow = e_ArrowParameterLeft;
			if (state == GLUT_UP) { parameterIndex--; parameterChanged = true; }
		}
		else if (extraArrows && withinYrange && p.x >  (extents.x - arrowOffset - arrowOffset * smallerArrowFactor - arrowGap))
		{
			whichArrow = e_ArrowParameterRight;
			if (state == GLUT_UP) { parameterIndex++; parameterChanged = true; }
		}
		else
		{
			whichArrow = e_ArrowSelectionNone;
		}
#endif // __ANDROID__

	}
	else if (button == GLUT_RIGHT_BUTTON)
	{
		if (state == GLUT_DOWN)
		{
			lastp = ConvertScreenToWorld(x, y);
			rMouseDown = true;
		}

		if (state == GLUT_UP)
		{
			rMouseDown = false;
		}
	}
}

static void MouseMotion(int32 x, int32 y)
{
	b2Vec2 p = ConvertScreenToWorld(x, y);
	test->MouseMove(p);

	if (rMouseDown)
	{
		b2Vec2 diff = p - lastp;
		settings.viewCenter.x -= diff.x;
		settings.viewCenter.y -= diff.y;
		Resize(width, height);
		lastp = ConvertScreenToWorld(x, y);
	}
}

static void MouseWheel(int wheel, int direction, int x, int y)
{
	B2_NOT_USED(wheel);
	B2_NOT_USED(x);
	B2_NOT_USED(y);
	if (direction > 0)
	{
		viewZoom /= 1.1f;
	}
	else
	{
		viewZoom *= 1.1f;
	}
	Resize(width, height);
}

static void Restart(int)
{
	delete test;
	entry = g_testEntries + testIndex;
	test = entry->createFcn();
	Resize(width, height);
}

static void Pause(int)
{
	settings.pause = !settings.pause;
}

static void Exit(int code)
{
	// TODO: freeglut is not building on OSX
#ifdef FREEGLUT
	glutLeaveMainLoop();
#endif
	exit(code);
}

static void SingleStep(int)
{
	settings.pause = 1;
	settings.singleStep = 1;
}

int main(int argc, char** argv)
{
	testCount = 0;
	while (g_testEntries[testCount].createFcn != NULL)
	{
		++testCount;
	}

	testIndex = b2Clamp(testIndex, 0, testCount-1);
	testSelection = testIndex;

	entry = g_testEntries + testIndex;
	if (entry && entry->createFcn) {
		test = entry->createFcn();
	}

	glutInit(&argc, argv);
	glutInitContextVersion(2, 0);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(width, height);
	char title[32];
	sprintf(title, "Box2D Version %d.%d.%d", b2_version.major, b2_version.minor, b2_version.revision);
	mainWindow = glutCreateWindow(title);
	//glutSetOption (GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);

	glutDisplayFunc(SimulationLoop);

	GLUI_Master.set_glutReshapeFunc(Resize);
	GLUI_Master.set_glutKeyboardFunc(Keyboard);
	GLUI_Master.set_glutSpecialFunc(KeyboardSpecial);
	GLUI_Master.set_glutMouseFunc(Mouse);
#ifdef FREEGLUT
	glutMouseWheelFunc(MouseWheel);
#endif
	glutMotionFunc(MouseMotion);

	glutKeyboardUpFunc(KeyboardUp);

#ifndef __ANDROID__

	glui = GLUI_Master.create_glui_subwindow( mainWindow,
		GLUI_SUBWINDOW_RIGHT );

	glui->add_statictext("Tests");
	GLUI_Listbox* testList =
		glui->add_listbox("", &testSelection);

	glui->add_separator();

	GLUI_Spinner* velocityIterationSpinner =
		glui->add_spinner("Vel Iters", GLUI_SPINNER_INT, &settings.velocityIterations);
	velocityIterationSpinner->set_int_limits(1, 500);

	GLUI_Spinner* positionIterationSpinner =
		glui->add_spinner("Pos Iters", GLUI_SPINNER_INT, &settings.positionIterations);
	positionIterationSpinner->set_int_limits(0, 100);

	GLUI_Spinner* hertzSpinner =
		glui->add_spinner("Hertz", GLUI_SPINNER_FLOAT, &settingsHz);

	hertzSpinner->set_float_limits(5.0f, 200.0f);

	glui->add_checkbox("Sleep", &settings.enableSleep);
	glui->add_checkbox("Warm Starting", &settings.enableWarmStarting);
	glui->add_checkbox("Time of Impact", &settings.enableContinuous);
	glui->add_checkbox("Sub-Stepping", &settings.enableSubStepping);

	//glui->add_separator();

	GLUI_Panel* drawPanel =	glui->add_panel("Draw");
	glui->add_checkbox_to_panel(drawPanel, "Shapes", &settings.drawShapes);
	glui->add_checkbox_to_panel(drawPanel, "Joints", &settings.drawJoints);
	glui->add_checkbox_to_panel(drawPanel, "AABBs", &settings.drawAABBs);
	glui->add_checkbox_to_panel(drawPanel, "Contact Points", &settings.drawContactPoints);
	glui->add_checkbox_to_panel(drawPanel, "Contact Normals", &settings.drawContactNormals);
	glui->add_checkbox_to_panel(drawPanel, "Contact Impulses", &settings.drawContactImpulse);
	glui->add_checkbox_to_panel(drawPanel, "Friction Impulses", &settings.drawFrictionImpulse);
	glui->add_checkbox_to_panel(drawPanel, "Center of Masses", &settings.drawCOMs);
	glui->add_checkbox_to_panel(drawPanel, "Statistics", &settings.drawStats);
	glui->add_checkbox_to_panel(drawPanel, "Profile", &settings.drawProfile);

	int32 testCount = 0;
	TestEntry* e = g_testEntries;
	while (e->createFcn)
	{
		testList->add_item(testCount, e->name);
		++testCount;
		++e;
	}

	glui->add_button("Pause", 0, Pause);
	glui->add_button("Single Step", 0, SingleStep);
	glui->add_button("Restart", 0, Restart);

	glui->add_button("Quit", 0,(GLUI_Update_CB)Exit);

	glui->set_main_gfx_window( mainWindow );

#endif // __ANDROID__

	// Use a timer to control the frame rate.
	glutTimerFunc(framePeriod, Timer, 0);

	glutMainLoop();

	return 0;
}
