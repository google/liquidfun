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

#if !defined(__ANDROID__) && !defined(__IOS__)
#define ENABLE_GLUI 1
#endif  // !defined(__ANDROID__) && !defined(__IOS__)

#include "Render.h"
#include "Test.h"
#include "Arrow.h"
#include "FullscreenUI.h"
#include "ParticleParameter.h"
#if ENABLE_GLUI
#include "glui/glui.h"
#else
#include "GL/freeglut.h"
#endif  // ENABLE_GLUI
#include <stdio.h>
#include "AndroidUtil/AndroidLogPrint.h"
#include <algorithm>
#include <string>
#include <sstream>

namespace TestMain
{

namespace
{
	int32 testIndex = 0;
	int32 testSelection = 0;
	int32 testCount = 0;
	TestEntry* entry;
	Test* test;
	Settings settings;
	int32 width = 640;
	int32 height = 540;
	int32 framePeriod = 16;
	int32 mainWindow;
	float settingsHz = 60.0;
#if ENABLE_GLUI
	GLUI *glui = NULL;
#endif  // ENABLE_GLUI
	float32 viewZoom = 1.0f;
	int tx, ty, tw, th;
	bool rMouseDown = false;
	// State of the mouse on the previous call of Mouse().
	int32 previousMouseState = -1;
	b2Vec2 lastp;
	b2Vec2 extents;

	// Fullscreen UI object.
	FullscreenUI fullscreenUI;
	// Used to control the behavior of particle tests.
	ParticleParameter particleParameter;
}

// Set whether to restart the test on particle parameter changes.
// This parameter is re-enabled when the test changes.
void SetRestartOnParticleParameterChange(bool enable)
{
	particleParameter.SetRestartOnChange(enable);
}

// Set the currently selected particle parameter value.  This value must
// match one of the values in TestMain::k_particleTypes or one of the values
// referenced by particleParameterDef passed to SetParticleParameters().
uint32 SetParticleParameterValue(uint32 value)
{
	const int32 index = particleParameter.FindIndexByValue(value);
	// If the particle type isn't found, so fallback to the first entry in the
	// parameter.
	particleParameter.Set(index >= 0 ? index : 0);
	return particleParameter.GetValue();
}

// Get the currently selected particle parameter value and enable particle
// parameter selection arrows on Android.
uint32 GetParticleParameterValue()
{
	// Enable display of particle type selection arrows.
	fullscreenUI.SetParticleParameterSelectionEnabled(true);
	return particleParameter.GetValue();
}

// Override the default particle parameters for the test.
void SetParticleParameters(
	const ParticleParameter::Definition * const particleParameterDef,
	const uint32 particleParameterDefCount)
{
	particleParameter.SetDefinition(particleParameterDef,
									particleParameterDefCount);
}

static void Resize(int32 w, int32 h)
{
	width = w;
	height = h;

#if ENABLE_GLUI
	GLUI_Master.get_viewport_area(&tx, &ty, &tw, &th);
#else
	tx = 0;
	ty = 0;
	tw = glutGet(GLUT_WINDOW_WIDTH);
	th = glutGet(GLUT_WINDOW_HEIGHT);
#endif  // ENABLE_GLUI
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

	if (fullscreenUI.GetEnabled())
	{
		fullscreenUI.SetViewParameters(&settings.viewCenter, &extents);
	}
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

static void SimulationLoop()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	settings.hz = settingsHz;

	// call this each frame, to function correctly with devices that may recreate
	// the GL Context without us asking for it
	Resize(width, height);

	test->Step(&settings);

	// Update the state of the particle parameter.
	bool restartTest;
	const bool changed = particleParameter.Changed(&restartTest);
	B2_NOT_USED(changed);


	if (fullscreenUI.GetEnabled())
	{
		// Set framework settings options based on parameters
		const uint32 options = particleParameter.GetOptions();

		settings.strictContacts 	= options &
			ParticleParameter::OptionStrictContacts;
		settings.drawContactPoints	= options &
			ParticleParameter::OptionDrawContactPoints;
		settings.drawContactNormals	= options &
			ParticleParameter::OptionDrawContactNormals;
		settings.drawContactImpulse	= options &
			ParticleParameter::OptionDrawContactImpulse;
		settings.drawFrictionImpulse = options &
			ParticleParameter::OptionDrawFrictionImpulse;
		settings.drawStats 			 = options &
			ParticleParameter::OptionDrawStats;
		settings.drawProfile		 = options &
			ParticleParameter::OptionDrawProfile;

		// The b2Draw based flags must be exactly 0 or 1 currently.
		settings.drawShapes 	= options &
			ParticleParameter::OptionDrawShapes ? 1 : 0;
		settings.drawParticles 	= options &
			ParticleParameter::OptionDrawParticles ? 1 : 0;
		settings.drawJoints		= options &
			ParticleParameter::OptionDrawJoints ? 1 : 0;
		settings.drawAABBs		= options &
			ParticleParameter::OptionDrawAABBs ? 1 : 0;
		settings.drawCOMs 		= options &
			ParticleParameter::OptionDrawCOMs ? 1 : 0;

		// Draw the full screen UI with
		// "test_name [: particle_parameter] / fps" at the bottom of the
		// screen.
		std::string msg = entry->name;
		if (fullscreenUI.GetParticleParameterSelectionEnabled())
		{
			msg += " : ";
			msg += particleParameter.GetName();
		}

		std::stringstream ss;
		ss << int(1000.0f / ComputeFPS());
		msg += " / " + ss.str() + " fps";
		fullscreenUI.Draw(msg);
	}

	test->DrawTitle(entry->name);

	glutSwapBuffers();

	if (testSelection != testIndex || restartTest)
	{
		fullscreenUI.Reset();
		if (!restartTest) particleParameter.Reset();

		testIndex = testSelection;
		delete test;
		entry = g_testEntries + testIndex;
		test = entry->createFcn();
		viewZoom = test->GetDefaultViewZoom();
		settings.viewCenter.Set(0.0f, 20.0f * viewZoom);
		Resize(width, height);
	}

	// print world step time stats every 600 frames
	static int s_printCount = 0;
	static b2Stat st;
	st.Record(settings.stepTimeOut);

	const int STAT_PRINT_INTERVAL = 600;
	if ( settings.printStepTimeStats && st.GetCount() == STAT_PRINT_INTERVAL )
	{
		printf("World Step Time samples %i-%i: %fmin %fmax %favg (ms)\n",
			s_printCount*STAT_PRINT_INTERVAL,
			(s_printCount+1)*STAT_PRINT_INTERVAL-1,
			st.GetMin(), st.GetMax(), st.GetMean());
		st.Clear();
		s_printCount++;
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
#if ENABLE_GLUI
		if (glui) glui->sync_live();
#endif  // ENABLE_GLUI
		break;

		// Press ] to next test.
	case ']':
		++testSelection;
		if (testSelection == testCount)
		{
			testSelection = 0;
		}
#if ENABLE_GLUI
		if (glui) glui->sync_live();
#endif  // ENABLE_GLUI
		break;

		// Press ~ to enable / disable the fullscreen UI.
	case '~':
		fullscreenUI.SetEnabled(!fullscreenUI.GetEnabled());
		break;

		// Press < to select the previous particle parameter setting.
	case '<':
		particleParameter.Decrement();
		break;

		// Press > to select the next particle parameter setting.
	case '>':
		particleParameter.Increment();
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

		switch (fullscreenUI.Mouse(button, state, previousMouseState, p))
		{
		case FullscreenUI::e_SelectionTestPrevious:
			testSelection = std::max(0, testSelection - 1);
			break;
		case FullscreenUI::e_SelectionTestNext:
			if (g_testEntries[testSelection + 1].name) testSelection++;
			break;
		case FullscreenUI::e_SelectionParameterPrevious:
			particleParameter.Decrement();
			break;
		case FullscreenUI::e_SelectionParameterNext:
			particleParameter.Increment();
			break;
		default:
			break;
		}

		if (state == GLUT_DOWN)
		{
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
			test->MouseUp(p);
		}
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
	previousMouseState = state;
}

static void MouseMotion(int32 x, int32 y)
{
	b2Vec2 p = ConvertScreenToWorld(x, y);

	if (fullscreenUI.GetSelection() == FullscreenUI::e_SelectionNone)
	{
		test->MouseMove(p);
	}

	if (rMouseDown)
	{
		b2Vec2 diff = p - lastp;
		settings.viewCenter.x -= diff.x;
		settings.viewCenter.y -= diff.y;
		Resize(width, height);
		lastp = ConvertScreenToWorld(x, y);
	}
}

#ifdef FREEGLUT
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
#endif

#if ENABLE_GLUI
static void Restart(int)
{
	delete test;
	entry = g_testEntries + testIndex;
	test = entry->createFcn();
	Resize(width, height);
}
#endif  // ENABLE_GLUI

#if ENABLE_GLUI
static void Pause(int)
{
	settings.pause = !settings.pause;
}
#endif  // ENABLE_GLUI

#if ENABLE_GLUI
static void Exit(int code)
{
	// TODO: freeglut is not building on OSX
#ifdef FREEGLUT
	glutLeaveMainLoop();
#endif
	exit(code);
}
#endif  // ENABLE_GLUI

#if ENABLE_GLUI
static void SingleStep(int)
{
	settings.pause = 1;
	settings.singleStep = 1;
}
#endif  // ENABLE_GLUI

}  // namespace TestMain

int main(int argc, char** argv)
{
	using namespace TestMain;

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
		testSelection = testIndex;
		testIndex = -1;
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

#if ENABLE_GLUI
	GLUI_Master.set_glutReshapeFunc(Resize);
	GLUI_Master.set_glutKeyboardFunc(Keyboard);
	GLUI_Master.set_glutSpecialFunc(KeyboardSpecial);
	GLUI_Master.set_glutMouseFunc(Mouse);
#else
	{
		glutReshapeFunc(Resize);
		glutKeyboardFunc(Keyboard);
		glutSpecialUpFunc(KeyboardSpecial);
		glutMouseFunc(Mouse);
	}
#endif  // ENABLE_GLUI

#ifdef FREEGLUT
	glutMouseWheelFunc(MouseWheel);
#endif
	glutMotionFunc(MouseMotion);

	glutKeyboardUpFunc(KeyboardUp);

#if ENABLE_GLUI
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

	GLUI_Spinner* particleIterationSpinner =
		glui->add_spinner("Pcl Iters", GLUI_SPINNER_INT, &settings.particleIterations);
	particleIterationSpinner->set_int_limits(1, 100);

	GLUI_Spinner* hertzSpinner =
		glui->add_spinner("Hertz", GLUI_SPINNER_FLOAT, &settingsHz);

	hertzSpinner->set_float_limits(5.0f, 200.0f);

	glui->add_checkbox("Sleep", &settings.enableSleep);
	glui->add_checkbox("Warm Starting", &settings.enableWarmStarting);
	glui->add_checkbox("Time of Impact", &settings.enableContinuous);
	glui->add_checkbox("Sub-Stepping", &settings.enableSubStepping);
	glui->add_checkbox("Strict Particle/Body Contacts", &settings.strictContacts);

	//glui->add_separator();

	GLUI_Panel* drawPanel =	glui->add_panel("Draw");
	glui->add_checkbox_to_panel(drawPanel, "Shapes", &settings.drawShapes);
	glui->add_checkbox_to_panel(drawPanel, "Particles", &settings.drawParticles);
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

#endif  // ENABLE_GLUI

	// Configure the fullscreen UI's viewport parameters.
	fullscreenUI.SetViewParameters(&settings.viewCenter, &extents);

	// Use a timer to control the frame rate.
	glutTimerFunc(framePeriod, Timer, 0);

	glutMainLoop();

	return 0;
}
