/*
 * freeglut_ext.c
 *
 * Functions related to OpenGL extensions.
 *
 * Copyright (c) 1999-2000 Pawel W. Olszta. All Rights Reserved.
 * Written by Pawel W. Olszta, <olszta@sourceforge.net>
 * Creation date: Thu Dec 9 1999
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * PAWEL W. OLSZTA BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#define GLX_GLXEXT_PROTOTYPES
#include <GL/freeglut.h>
#include "freeglut_internal.h"

struct name_address_pair
{
    const char *name;
    GLUTproc address;
};

static struct name_address_pair glut_functions[] =
{
   { "glutInit", (GLUTproc) glutInit },
   { "glutInitDisplayMode", (GLUTproc) glutInitDisplayMode },
   { "glutInitDisplayString", (GLUTproc) glutInitDisplayString },
   { "glutInitWindowPosition", (GLUTproc) glutInitWindowPosition },
   { "glutInitWindowSize", (GLUTproc) glutInitWindowSize },
   { "glutMainLoop", (GLUTproc) glutMainLoop },
   { "glutCreateWindow", (GLUTproc) glutCreateWindow },
   { "glutCreateSubWindow", (GLUTproc) glutCreateSubWindow },
   { "glutDestroyWindow", (GLUTproc) glutDestroyWindow },
   { "glutPostRedisplay", (GLUTproc) glutPostRedisplay },
   { "glutPostWindowRedisplay", (GLUTproc) glutPostWindowRedisplay },
   { "glutSwapBuffers", (GLUTproc) glutSwapBuffers },
   { "glutGetWindow", (GLUTproc) glutGetWindow },
   { "glutSetWindow", (GLUTproc) glutSetWindow },
   { "glutSetWindowTitle", (GLUTproc) glutSetWindowTitle },
   { "glutSetIconTitle", (GLUTproc) glutSetIconTitle },
   { "glutPositionWindow", (GLUTproc) glutPositionWindow },
   { "glutReshapeWindow", (GLUTproc) glutReshapeWindow },
   { "glutPopWindow", (GLUTproc) glutPopWindow },
   { "glutPushWindow", (GLUTproc) glutPushWindow },
   { "glutIconifyWindow", (GLUTproc) glutIconifyWindow },
   { "glutShowWindow", (GLUTproc) glutShowWindow },
   { "glutHideWindow", (GLUTproc) glutHideWindow },
   { "glutFullScreen", (GLUTproc) glutFullScreen },
   { "glutSetCursor", (GLUTproc) glutSetCursor },
   { "glutWarpPointer", (GLUTproc) glutWarpPointer },
   { "glutEstablishOverlay", (GLUTproc) glutEstablishOverlay },
   { "glutRemoveOverlay", (GLUTproc) glutRemoveOverlay },
   { "glutUseLayer", (GLUTproc) glutUseLayer },
   { "glutPostOverlayRedisplay", (GLUTproc) glutPostOverlayRedisplay },
   { "glutPostWindowOverlayRedisplay", (GLUTproc) glutPostWindowOverlayRedisplay },
   { "glutShowOverlay", (GLUTproc) glutShowOverlay },
   { "glutHideOverlay", (GLUTproc) glutHideOverlay },
   { "glutCreateMenu", (GLUTproc) glutCreateMenu },
   { "glutDestroyMenu", (GLUTproc) glutDestroyMenu },
   { "glutGetMenu", (GLUTproc) glutGetMenu },
   { "glutSetMenu", (GLUTproc) glutSetMenu },
   { "glutAddMenuEntry", (GLUTproc) glutAddMenuEntry },
   { "glutAddSubMenu", (GLUTproc) glutAddSubMenu },
   { "glutChangeToMenuEntry", (GLUTproc) glutChangeToMenuEntry },
   { "glutChangeToSubMenu", (GLUTproc) glutChangeToSubMenu },
   { "glutRemoveMenuItem", (GLUTproc) glutRemoveMenuItem },
   { "glutAttachMenu", (GLUTproc) glutAttachMenu },
   { "glutDetachMenu", (GLUTproc) glutDetachMenu },
   { "glutDisplayFunc", (GLUTproc) glutDisplayFunc },
   { "glutReshapeFunc", (GLUTproc) glutReshapeFunc },
   { "glutKeyboardFunc", (GLUTproc) glutKeyboardFunc },
   { "glutMouseFunc", (GLUTproc) glutMouseFunc },
   { "glutMotionFunc", (GLUTproc) glutMotionFunc },
   { "glutPassiveMotionFunc", (GLUTproc) glutPassiveMotionFunc },
   { "glutEntryFunc", (GLUTproc) glutEntryFunc },
   { "glutVisibilityFunc", (GLUTproc) glutVisibilityFunc },
   { "glutIdleFunc", (GLUTproc) glutIdleFunc },
   { "glutTimerFunc", (GLUTproc) glutTimerFunc },
   { "glutMenuStateFunc", (GLUTproc) glutMenuStateFunc },
   { "glutSpecialFunc", (GLUTproc) glutSpecialFunc },
   { "glutSpaceballMotionFunc", (GLUTproc) glutSpaceballMotionFunc },
   { "glutSpaceballRotateFunc", (GLUTproc) glutSpaceballRotateFunc },
   { "glutSpaceballButtonFunc", (GLUTproc) glutSpaceballButtonFunc },
   { "glutButtonBoxFunc", (GLUTproc) glutButtonBoxFunc },
   { "glutDialsFunc", (GLUTproc) glutDialsFunc },
   { "glutTabletMotionFunc", (GLUTproc) glutTabletMotionFunc },
   { "glutTabletButtonFunc", (GLUTproc) glutTabletButtonFunc },
   { "glutMenuStatusFunc", (GLUTproc) glutMenuStatusFunc },
   { "glutOverlayDisplayFunc", (GLUTproc) glutOverlayDisplayFunc },
   { "glutWindowStatusFunc", (GLUTproc) glutWindowStatusFunc },
   { "glutKeyboardUpFunc", (GLUTproc) glutKeyboardUpFunc },
   { "glutSpecialUpFunc", (GLUTproc) glutSpecialUpFunc },
#if !TARGET_HOST_WINCE
   { "glutJoystickFunc", (GLUTproc) glutJoystickFunc },
#endif /* !TARGET_HOST_WINCE */
   { "glutSetColor", (GLUTproc) glutSetColor },
   { "glutGetColor", (GLUTproc) glutGetColor },
   { "glutCopyColormap", (GLUTproc) glutCopyColormap },
   { "glutGet", (GLUTproc) glutGet },
   { "glutDeviceGet", (GLUTproc) glutDeviceGet },
   { "glutExtensionSupported", (GLUTproc) glutExtensionSupported },
   { "glutGetModifiers", (GLUTproc) glutGetModifiers },
   { "glutLayerGet", (GLUTproc) glutLayerGet },
   { "glutBitmapCharacter", (GLUTproc) glutBitmapCharacter },
   { "glutBitmapWidth", (GLUTproc) glutBitmapWidth },
   { "glutStrokeCharacter", (GLUTproc) glutStrokeCharacter },
   { "glutStrokeWidth", (GLUTproc) glutStrokeWidth },
   { "glutBitmapLength", (GLUTproc) glutBitmapLength },
   { "glutStrokeLength", (GLUTproc) glutStrokeLength },
   { "glutWireSphere", (GLUTproc) glutWireSphere },
   { "glutSolidSphere", (GLUTproc) glutSolidSphere },
   { "glutWireCone", (GLUTproc) glutWireCone },
   { "glutSolidCone", (GLUTproc) glutSolidCone },
   { "glutWireCube", (GLUTproc) glutWireCube },
   { "glutSolidCube", (GLUTproc) glutSolidCube },
   { "glutWireTorus", (GLUTproc) glutWireTorus },
   { "glutSolidTorus", (GLUTproc) glutSolidTorus },
   { "glutWireDodecahedron", (GLUTproc) glutWireDodecahedron },
   { "glutSolidDodecahedron", (GLUTproc) glutSolidDodecahedron },
   { "glutWireTeapot", (GLUTproc) glutWireTeapot },
   { "glutSolidTeapot", (GLUTproc) glutSolidTeapot },
   { "glutWireOctahedron", (GLUTproc) glutWireOctahedron },
   { "glutSolidOctahedron", (GLUTproc) glutSolidOctahedron },
   { "glutWireTetrahedron", (GLUTproc) glutWireTetrahedron },
   { "glutSolidTetrahedron", (GLUTproc) glutSolidTetrahedron },
   { "glutWireIcosahedron", (GLUTproc) glutWireIcosahedron },
   { "glutSolidIcosahedron", (GLUTproc) glutSolidIcosahedron },
   { "glutVideoResizeGet", (GLUTproc) glutVideoResizeGet },
   { "glutSetupVideoResizing", (GLUTproc) glutSetupVideoResizing },
   { "glutStopVideoResizing", (GLUTproc) glutStopVideoResizing },
   { "glutVideoResize", (GLUTproc) glutVideoResize },
   { "glutVideoPan", (GLUTproc) glutVideoPan },
   { "glutReportErrors", (GLUTproc) glutReportErrors },
   { "glutIgnoreKeyRepeat", (GLUTproc) glutIgnoreKeyRepeat },
   { "glutSetKeyRepeat", (GLUTproc) glutSetKeyRepeat },
#if !TARGET_HOST_WINCE
   { "glutForceJoystickFunc", (GLUTproc) glutForceJoystickFunc },
   { "glutGameModeString", (GLUTproc) glutGameModeString },
   { "glutEnterGameMode", (GLUTproc) glutEnterGameMode },
   { "glutLeaveGameMode", (GLUTproc) glutLeaveGameMode },
   { "glutGameModeGet", (GLUTproc) glutGameModeGet },
#endif /* !TARGET_HOST_WINCE */
   /* freeglut extensions */
   { "glutMainLoopEvent", (GLUTproc) glutMainLoopEvent },
   { "glutLeaveMainLoop", (GLUTproc) glutLeaveMainLoop },
   { "glutCloseFunc", (GLUTproc) glutCloseFunc },
   { "glutWMCloseFunc", (GLUTproc) glutWMCloseFunc },
   { "glutMenuDestroyFunc", (GLUTproc) glutMenuDestroyFunc },
   { "glutSetOption", (GLUTproc) glutSetOption },
   { "glutSetWindowData", (GLUTproc) glutSetWindowData },
   { "glutGetWindowData", (GLUTproc) glutGetWindowData },
   { "glutSetMenuData", (GLUTproc) glutSetMenuData },
   { "glutGetMenuData", (GLUTproc) glutGetMenuData },
   { "glutBitmapHeight", (GLUTproc) glutBitmapHeight },
   { "glutStrokeHeight", (GLUTproc) glutStrokeHeight },
   { "glutBitmapString", (GLUTproc) glutBitmapString },
   { "glutStrokeString", (GLUTproc) glutStrokeString },
   { "glutWireRhombicDodecahedron", (GLUTproc) glutWireRhombicDodecahedron },
   { "glutSolidRhombicDodecahedron", (GLUTproc) glutSolidRhombicDodecahedron },
   { "glutWireSierpinskiSponge", (GLUTproc) glutWireSierpinskiSponge },
   { "glutSolidSierpinskiSponge", (GLUTproc) glutSolidSierpinskiSponge },
   { "glutWireCylinder", (GLUTproc) glutWireCylinder },
   { "glutSolidCylinder", (GLUTproc) glutSolidCylinder },
   { "glutGetProcAddress", (GLUTproc) glutGetProcAddress },
   { "glutMouseWheelFunc", (GLUTproc) glutMouseWheelFunc },
   { NULL, NULL }
};


GLUTproc FGAPIENTRY
glutGetProcAddress( const char *procName )
{
    /* Try GLUT functions first */
    int i;
    FREEGLUT_EXIT_IF_NOT_INITIALISED ( "glutGetProcAddress" );
    for( i = 0; glut_functions[ i ].name; i++ )
        if( strcmp( glut_functions[ i ].name, procName ) == 0)
            return glut_functions[ i ].address;

    /* Try core GL functions */
#if TARGET_HOST_WIN32 || TARGET_HOST_WINCE
    return(GLUTproc)wglGetProcAddress( ( LPCSTR )procName );
#elif TARGET_HOST_UNIX_X11 && defined( GLX_ARB_get_proc_address )
    return(GLUTproc)glXGetProcAddressARB( ( const GLubyte * )procName );
#else
    return NULL;
#endif
}
