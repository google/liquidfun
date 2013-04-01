/*! \file    shapes.c
    \ingroup demos

    This program is a test harness for the various shapes
    in OpenGLUT.  It may also be useful to see which
    parameters control what behavior in the OpenGLUT
    objects.
 
    Spinning wireframe and solid-shaded shapes are
    displayed.  Some parameters can be adjusted.
 
   Keys:
      -    <tt>Esc &nbsp;</tt> Quit
      -    <tt>q Q &nbsp;</tt> Quit
      -    <tt>i I &nbsp;</tt> Show info
      -    <tt>p P &nbsp;</tt> Toggle perspective or orthographic projection
      -    <tt>r R &nbsp;</tt> Toggle fixed or animated rotation around model X-axis
      -    <tt>s S &nbsp;</tt> Toggle toggle fixed function or shader render path
      -    <tt>n N &nbsp;</tt> Toggle visualization of object's normal vectors
      -    <tt>= + &nbsp;</tt> Increase \a slices
      -    <tt>- _ &nbsp;</tt> Decreate \a slices
      -    <tt>, < &nbsp;</tt> Decreate \a stacks
      -    <tt>. > &nbsp;</tt> Increase \a stacks
      -    <tt>9 ( &nbsp;</tt> Decreate \a depth  (Sierpinski Sponge)
      -    <tt>0 ) &nbsp;</tt> Increase \a depth  (Sierpinski Sponge)
      -    <tt>up&nbsp; &nbsp;</tt> Increase "outer radius"
      -    <tt>down&nbsp;</tt> Decrease "outer radius"
      -    <tt>left&nbsp;</tt> Decrease "inner radius"
      -    <tt>right</tt> Increase "inner radius"
      -    <tt>PgUp&nbsp;</tt> Next shape-drawing function
      -    <tt>PgDn&nbsp;</tt> Prev shape-drawing function

    \author  Written by Nigel Stewart November 2003

    \author  Portions Copyright (C) 2004, the OpenGLUT project contributors. <br>
             OpenGLUT branched from freeglut in February, 2004.
 
    \image   html openglut_shapes.png OpenGLUT Geometric Shapes Demonstration
    \include demos/shapes/shapes.c
*/

#include <GL/freeglut.h>

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "glmatrix.h"

#ifdef _MSC_VER
/* DUMP MEMORY LEAKS */
#include <crtdbg.h>
#endif

/* report GL errors, if any, to stderr */
void checkError(const char *functionName)
{
    GLenum error;
    while (( error = glGetError() ) != GL_NO_ERROR) {
        fprintf (stderr, "GL error 0x%X detected in %s\n", error, functionName);
    }
}

/*
 * OpenGL 2+ shader mode needs some function and macro definitions, 
 * avoiding a dependency on additional libraries like GLEW or the
 * GL/glext.h header
 */
#ifndef GL_FRAGMENT_SHADER
#define GL_FRAGMENT_SHADER 0x8B30
#endif

#ifndef GL_VERTEX_SHADER
#define GL_VERTEX_SHADER 0x8B31
#endif

#ifndef GL_COMPILE_STATUS
#define GL_COMPILE_STATUS 0x8B81
#endif

#ifndef GL_LINK_STATUS
#define GL_LINK_STATUS 0x8B82
#endif

#ifndef GL_INFO_LOG_LENGTH
#define GL_INFO_LOG_LENGTH 0x8B84
#endif

typedef ptrdiff_t ourGLsizeiptr;
typedef char ourGLchar;

#ifndef APIENTRY
#define APIENTRY
#endif

#ifndef GL_VERSION_2_0
typedef GLuint (APIENTRY *PFNGLCREATESHADERPROC) (GLenum type);
typedef void (APIENTRY *PFNGLSHADERSOURCEPROC) (GLuint shader, GLsizei count, const ourGLchar **string, const GLint *length);
typedef void (APIENTRY *PFNGLCOMPILESHADERPROC) (GLuint shader);
typedef GLuint (APIENTRY *PFNGLCREATEPROGRAMPROC) (void);
typedef void (APIENTRY *PFNGLATTACHSHADERPROC) (GLuint program, GLuint shader);
typedef void (APIENTRY *PFNGLLINKPROGRAMPROC) (GLuint program);
typedef void (APIENTRY *PFNGLUSEPROGRAMPROC) (GLuint program);
typedef void (APIENTRY *PFNGLGETSHADERIVPROC) (GLuint shader, GLenum pname, GLint *params);
typedef void (APIENTRY *PFNGLGETSHADERINFOLOGPROC) (GLuint shader, GLsizei bufSize, GLsizei *length, ourGLchar *infoLog);
typedef void (APIENTRY *PFNGLGETPROGRAMIVPROC) (GLenum target, GLenum pname, GLint *params);
typedef void (APIENTRY *PFNGLGETPROGRAMINFOLOGPROC) (GLuint program, GLsizei bufSize, GLsizei *length, ourGLchar *infoLog);
typedef GLint (APIENTRY *PFNGLGETATTRIBLOCATIONPROC) (GLuint program, const ourGLchar *name);
typedef GLint (APIENTRY *PFNGLGETUNIFORMLOCATIONPROC) (GLuint program, const ourGLchar *name);
typedef void (APIENTRY *PFNGLUNIFORMMATRIX4FVPROC) (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
typedef void (APIENTRY *PFNGLUNIFORMMATRIX3FVPROC) (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
#endif

PFNGLCREATESHADERPROC gl_CreateShader;
PFNGLSHADERSOURCEPROC gl_ShaderSource;
PFNGLCOMPILESHADERPROC gl_CompileShader;
PFNGLCREATEPROGRAMPROC gl_CreateProgram;
PFNGLATTACHSHADERPROC gl_AttachShader;
PFNGLLINKPROGRAMPROC gl_LinkProgram;
PFNGLUSEPROGRAMPROC gl_UseProgram;
PFNGLGETSHADERIVPROC gl_GetShaderiv;
PFNGLGETSHADERINFOLOGPROC gl_GetShaderInfoLog;
PFNGLGETPROGRAMIVPROC gl_GetProgramiv;
PFNGLGETPROGRAMINFOLOGPROC gl_GetProgramInfoLog;
PFNGLGETATTRIBLOCATIONPROC gl_GetAttribLocation;
PFNGLGETUNIFORMLOCATIONPROC gl_GetUniformLocation;
PFNGLUNIFORMMATRIX4FVPROC gl_UniformMatrix4fv;
PFNGLUNIFORMMATRIX3FVPROC gl_UniformMatrix3fv;

void initExtensionEntries(void)
{
    gl_CreateShader = (PFNGLCREATESHADERPROC) glutGetProcAddress ("glCreateShader");
    gl_ShaderSource = (PFNGLSHADERSOURCEPROC) glutGetProcAddress ("glShaderSource");
    gl_CompileShader = (PFNGLCOMPILESHADERPROC) glutGetProcAddress ("glCompileShader");
    gl_CreateProgram = (PFNGLCREATEPROGRAMPROC) glutGetProcAddress ("glCreateProgram");
    gl_AttachShader = (PFNGLATTACHSHADERPROC) glutGetProcAddress ("glAttachShader");
    gl_LinkProgram = (PFNGLLINKPROGRAMPROC) glutGetProcAddress ("glLinkProgram");
    gl_UseProgram = (PFNGLUSEPROGRAMPROC) glutGetProcAddress ("glUseProgram");
    gl_GetShaderiv = (PFNGLGETSHADERIVPROC) glutGetProcAddress ("glGetShaderiv");
    gl_GetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC) glutGetProcAddress ("glGetShaderInfoLog");
    gl_GetProgramiv = (PFNGLGETPROGRAMIVPROC) glutGetProcAddress ("glGetProgramiv");
    gl_GetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC) glutGetProcAddress ("glGetProgramInfoLog");
    gl_GetAttribLocation = (PFNGLGETATTRIBLOCATIONPROC) glutGetProcAddress ("glGetAttribLocation");
    gl_GetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC) glutGetProcAddress ("glGetUniformLocation");
    gl_UniformMatrix4fv = (PFNGLUNIFORMMATRIX4FVPROC) glutGetProcAddress ("glUniformMatrix4fv");
    gl_UniformMatrix3fv = (PFNGLUNIFORMMATRIX3FVPROC) glutGetProcAddress ("glUniformMatrix3fv");
    if (!gl_CreateShader || !gl_ShaderSource || !gl_CompileShader || !gl_CreateProgram || !gl_AttachShader || !gl_LinkProgram || !gl_UseProgram || !gl_GetShaderiv || !gl_GetShaderInfoLog || !gl_GetProgramiv || !gl_GetProgramInfoLog || !gl_GetAttribLocation || !gl_GetUniformLocation || !gl_UniformMatrix4fv || !gl_UniformMatrix3fv)
    {
        fprintf (stderr, "glCreateShader, glShaderSource, glCompileShader, glCreateProgram, glAttachShader, glLinkProgram, glUseProgram, glGetShaderiv, glGetShaderInfoLog, glGetProgramiv, glGetProgramInfoLog, glGetAttribLocation, glGetUniformLocation, glUniformMatrix4fv or gl_UniformMatrix3fv not found");
        exit(1);
    }
}

const ourGLchar *vertexShaderSource[] = {
    "/**",
    " * From the OpenGL Programming wikibook: http://en.wikibooks.org/wiki/GLSL_Programming/GLUT/Smooth_Specular_Highlights",
    " * This file is in the public domain.",
    " * Contributors: Sylvain Beucler",
    " */",
    "attribute vec3 fg_coord;",
    "attribute vec3 fg_normal;",
    "varying vec4 position;  /* position of the vertex (and fragment) in world space */",
    "varying vec3 varyingNormalDirection;  /* surface normal vector in world space */",
    "uniform mat4 m, p;      /* don't need v, as always identity in our demo */",
    "uniform mat3 m_3x3_inv_transp;",
    " ",
    "void main()",
    "{",
    "  vec4 fg_coord4 = vec4(fg_coord, 1.0);",
    "  position = m * fg_coord4;",
    "  varyingNormalDirection = normalize(m_3x3_inv_transp * fg_normal);",
    " ",
    "  mat4 mvp = p*m;   /* normally p*v*m */",
    "  gl_Position = mvp * fg_coord4;",
    "}"
};

const ourGLchar *fragmentShaderSource[] = {
    "/**",
    " * From the OpenGL Programming wikibook: http://en.wikibooks.org/wiki/GLSL_Programming/GLUT/Smooth_Specular_Highlights",
    " * This file is in the public domain.",
    " * Contributors: Martin Kraus, Sylvain Beucler",
    " */",
    "varying vec4 position;  /* position of the vertex (and fragment) in world space */",
    "varying vec3 varyingNormalDirection;  /* surface normal vector in world space */",
    "/* uniform mat4 v_inv;   // in this demo, the view matrix is always an identity matrix */",
    " ",
    "struct lightSource",
    "{",
    "  vec4 position;",
    "  vec4 diffuse;",
    "  vec4 specular;",
    "  float constantAttenuation, linearAttenuation, quadraticAttenuation;",
    "  float spotCutoff, spotExponent;",
    "  vec3 spotDirection;",
    "};",
    "lightSource light0 = lightSource(",
    "  vec4(2.0, 5.0, 5.0, 0.0),",
    "  vec4(1.0, 1.0, 1.0, 1.0),",
    "  vec4(1.0, 1.0, 1.0, 1.0),",
    "    0.0, 1.0, 0.0,",
    "  180.0, 0.0,",
    "  vec3(0.0, 0.0, 0.0)",
    ");",
    "vec4 scene_ambient = vec4(0.2, 0.2, 0.2, 1.0);",
    " ",
    "struct material",
    "{",
    "  vec4 ambient;",
    "  vec4 diffuse;",
    "  vec4 specular;",
    "  float shininess;",
    "};",
    "material frontMaterial = material(",
    "  vec4(1.0, 0.0, 0.0, 1.0),",
    "  vec4(1.0, 0.0, 0.0, 1.0),",
    "  vec4(1.0, 1.0, 1.0, 1.0),",
    "  100.0",
    ");",
    " ",
    "void main()",
    "{",
    "  vec3 normalDirection = normalize(varyingNormalDirection);",
    "  /* vec3 viewDirection = normalize(vec3(v_inv * vec4(0.0, 0.0, 0.0, 1.0) - position)); */",
    "  vec3 viewDirection = normalize(vec3(vec4(0.0, 0.0, 0.0, 1.0) - position));    /* in this demo, the view matrix is always an identity matrix */",
    "  vec3 lightDirection;",
    "  float attenuation;",
    " ",
    "  if (0.0 == light0.position.w) /* directional light? */",
    "    {",
    "      attenuation = 1.0; /* no attenuation */",
    "      lightDirection = normalize(vec3(light0.position));",
    "    } ",
    "  else /* point light or spotlight (or other kind of light) */",
    "    {",
    "      vec3 positionToLightSource = vec3(light0.position - position);",
    "      float distance = length(positionToLightSource);",
    "      lightDirection = normalize(positionToLightSource);",
    "      attenuation = 1.0 / (light0.constantAttenuation",
    "                           + light0.linearAttenuation * distance",
    "                           + light0.quadraticAttenuation * distance * distance);",
    " ",
    "      if (light0.spotCutoff <= 90.0) /* spotlight? */",
    "        {",
    "          float clampedCosine = max(0.0, dot(-lightDirection, light0.spotDirection));",
    "          if (clampedCosine < cos(radians(light0.spotCutoff))) /* outside of spotlight cone? */",
    "            {",
    "              attenuation = 0.0;",
    "            }",
    "          else",
    "            {",
    "              attenuation = attenuation * pow(clampedCosine, light0.spotExponent);   ",
    "            }",
    "        }",
    "    }",
    " ",
    "  vec3 ambientLighting = vec3(scene_ambient) * vec3(frontMaterial.ambient);",
    " ",
    "  vec3 diffuseReflection = attenuation ",
    "    * vec3(light0.diffuse) * vec3(frontMaterial.diffuse)",
    "    * max(0.0, dot(normalDirection, lightDirection));",
    " ",
    "  vec3 specularReflection;",
    "  if (dot(normalDirection, lightDirection) < 0.0) /* light source on the wrong side? */",
    "    {",
    "      specularReflection = vec3(0.0, 0.0, 0.0); /* no specular reflection */",
    "    }",
    "  else /* light source on the right side */",
    "    {",
    "      specularReflection = attenuation * vec3(light0.specular) * vec3(frontMaterial.specular) ",
    "        * pow(max(0.0, dot(reflect(-lightDirection, normalDirection), viewDirection)), frontMaterial.shininess);",
    "    }",
    " ",
    "  gl_FragColor = vec4(ambientLighting + diffuseReflection + specularReflection, 1.0);",
    "}"
};

GLint getAttribOrUniformLocation(const char* name, GLuint program, GLboolean isAttrib)
{
    if (isAttrib)
    {
        GLint attrib = gl_GetAttribLocation(program, name);
        if (attrib == -1)
        {
            fprintf(stderr, "Warning: Could not bind attrib %s\n", name);  
        }

        return attrib;
    }
    else
    {
        GLint uniform = gl_GetUniformLocation(program, name);
        if (uniform == -1)
        {
            fprintf(stderr, "Warning: Could not bind uniform %s\n", name);  
        }

        return uniform;
    }
    checkError ("getAttribOrUniformLocation");
}

GLuint program;
GLint attribute_fg_coord = -1, attribute_fg_normal = -1;  
GLint uniform_m = -1, uniform_p = -1, uniform_m_3x3_inv_transp = -1;
GLint shaderReady = 0;  /* Set to 1 when all initialization went well, to -1 when shader somehow unusable. */



void compileAndCheck(GLuint shader)
{
    GLint status;
    gl_CompileShader (shader);
    gl_GetShaderiv (shader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
        GLint infoLogLength;
        ourGLchar *infoLog;
        gl_GetShaderiv (shader, GL_INFO_LOG_LENGTH, &infoLogLength);
        infoLog = (ourGLchar*) malloc (infoLogLength);
        gl_GetShaderInfoLog (shader, infoLogLength, NULL, infoLog);
        fprintf (stderr, "compile log: %s\n", infoLog);
        free (infoLog);
    }
    checkError ("compileAndCheck");
}

GLuint compileShaderSource(GLenum type, GLsizei count, const ourGLchar **string)
{
    GLuint shader = gl_CreateShader (type);
    gl_ShaderSource (shader, count, string, NULL);

    checkError ("compileShaderSource");

    compileAndCheck (shader);
    return shader;
}

void linkAndCheck(GLuint program)
{
    GLint status;
    gl_LinkProgram (program);
    gl_GetProgramiv (program, GL_LINK_STATUS, &status);
    if (status == GL_FALSE) {
        GLint infoLogLength;
        ourGLchar *infoLog;
        gl_GetProgramiv (program, GL_INFO_LOG_LENGTH, &infoLogLength);
        infoLog = (ourGLchar*) malloc (infoLogLength);
        gl_GetProgramInfoLog (program, infoLogLength, NULL, infoLog);
        fprintf (stderr, "link log: %s\n", infoLog);
        free (infoLog);
    }
    checkError ("linkAndCheck");
}

void createProgram(GLuint vertexShader, GLuint fragmentShader)
{
    program = gl_CreateProgram ();
    if (vertexShader != 0) {
        gl_AttachShader (program, vertexShader);
    }
    if (fragmentShader != 0) {
        gl_AttachShader (program, fragmentShader);
    }

    checkError ("createProgram");

    linkAndCheck (program);
}

void initShader(void)
{
    const GLsizei vertexShaderLines = sizeof(vertexShaderSource) / sizeof(ourGLchar*);
    GLuint vertexShader =
        compileShaderSource (GL_VERTEX_SHADER, vertexShaderLines, vertexShaderSource);

    const GLsizei fragmentShaderLines = sizeof(fragmentShaderSource) / sizeof(ourGLchar*);
    GLuint fragmentShader =
        compileShaderSource (GL_FRAGMENT_SHADER, fragmentShaderLines, fragmentShaderSource);

    checkError ("initShader - 1");

    createProgram (vertexShader, fragmentShader);

    gl_UseProgram (program);

    attribute_fg_coord      = getAttribOrUniformLocation("fg_coord"         , program, GL_TRUE);
    attribute_fg_normal     = getAttribOrUniformLocation("fg_normal"        , program, GL_TRUE);
    uniform_m               = getAttribOrUniformLocation("m"                , program, GL_FALSE);
    uniform_p               = getAttribOrUniformLocation("p"                , program, GL_FALSE);
    uniform_m_3x3_inv_transp= getAttribOrUniformLocation("m_3x3_inv_transp" , program, GL_FALSE);

    gl_UseProgram (0);

    if (attribute_fg_coord==-1 || attribute_fg_normal==-1 ||
        uniform_m==-1 || uniform_p==-1 || uniform_m_3x3_inv_transp==-1)
        shaderReady = -1;
    else
        shaderReady = 1;

    checkError ("initShader - 2");
}

/*
 * This macro is only intended to be used on arrays, of course.
 */
#define NUMBEROF(x) ((sizeof(x))/(sizeof(x[0])))

/*
 * These global variables control which object is drawn,
 * and how it is drawn.  No object uses all of these
 * variables.
 */
static int function_index;
static int slices = 16;
static int stacks = 16;
static double irad = .25;
static double orad = 1.0;   /* doubles as size for objects other than Torus */
static int depth = 4;
static double offset[ 3 ] = { 0, 0, 0 };
static GLboolean show_info = GL_TRUE;
static float ar;
static GLboolean persProject = GL_TRUE;
static GLboolean animateXRot = GL_FALSE;
static GLboolean useShader   = GL_FALSE;
static GLboolean visNormals  = GL_FALSE;

/*
 * Enum to tell drawSizeInfo what to draw for each object
 */
#define GEO_NO_SIZE 0
#define GEO_SIZE 1
#define GEO_SCALE 2
#define GEO_INNER_OUTER_RAD 4
#define GEO_RAD 8
#define GEO_BASE_HEIGHT 16
#define GEO_RAD_HEIGHT 32

/*
 * These one-liners draw particular objects, fetching appropriate
 * information from the above globals.  They are just thin wrappers
 * for the FreeGLUT objects.
 */
static void drawSolidTetrahedron(void)         { glutSolidTetrahedron ();                        }
static void drawWireTetrahedron(void)          { glutWireTetrahedron ();                         }
static void drawSolidCube(void)                { glutSolidCube(orad);                            }  /* orad doubles as size input */
static void drawWireCube(void)                 { glutWireCube(orad);                             }  /* orad doubles as size input */
static void drawSolidOctahedron(void)          { glutSolidOctahedron ();                         }
static void drawWireOctahedron(void)           { glutWireOctahedron ();                          }
static void drawSolidDodecahedron(void)        { glutSolidDodecahedron ();                       }
static void drawWireDodecahedron(void)         { glutWireDodecahedron ();                        }
static void drawSolidRhombicDodecahedron(void) { glutSolidRhombicDodecahedron ();                }
static void drawWireRhombicDodecahedron(void)  { glutWireRhombicDodecahedron ();                 }
static void drawSolidIcosahedron(void)         { glutSolidIcosahedron ();                        }
static void drawWireIcosahedron(void)          { glutWireIcosahedron ();                         }
static void drawSolidSierpinskiSponge(void)    { glutSolidSierpinskiSponge (depth, offset, orad);}  /* orad doubles as size input */
static void drawWireSierpinskiSponge(void)     { glutWireSierpinskiSponge (depth, offset, orad); }  /* orad doubles as size input */
static void drawSolidTorus(void)               { glutSolidTorus(irad,orad,slices,stacks);        }
static void drawWireTorus(void)                { glutWireTorus (irad,orad,slices,stacks);        }
static void drawSolidSphere(void)              { glutSolidSphere(orad,slices,stacks);            }  /* orad doubles as size input */
static void drawWireSphere(void)               { glutWireSphere(orad,slices,stacks);             }  /* orad doubles as size input */
static void drawSolidCone(void)                { glutSolidCone(irad,orad,slices,stacks);         }  /* irad doubles as base input, and orad as height input */
static void drawWireCone(void)                 { glutWireCone(irad,orad,slices,stacks);          }  /* irad doubles as base input, and orad as height input */
static void drawSolidCylinder(void)            { glutSolidCylinder(irad,orad,slices,stacks);     }  /* irad doubles as radius input, and orad as height input */
static void drawWireCylinder(void)             { glutWireCylinder(irad,orad,slices,stacks);      }  /* irad doubles as radius input, and orad as height input */
/* per Glut manpage, it should be noted that the teapot is rendered
 * with clockwise winding for front facing polygons...
 * Same for the teacup and teaspoon
 */
static void drawSolidTeapot(void)
{   glFrontFace(GL_CW);    glutSolidTeapot(orad);   glFrontFace(GL_CCW);    /* orad doubles as size input */}
static void drawWireTeapot(void)
{   glFrontFace(GL_CW);    glutWireTeapot(orad);    glFrontFace(GL_CCW);    /* orad doubles as size input */}
static void drawSolidTeacup(void)
{   glFrontFace(GL_CW);    glutSolidTeacup(orad);   glFrontFace(GL_CCW);    /* orad doubles as size input */}
static void drawWireTeacup(void)
{   glFrontFace(GL_CW);    glutWireTeacup(orad);    glFrontFace(GL_CCW);    /* orad doubles as size input */}
static void drawSolidTeaspoon(void)
{   glFrontFace(GL_CW);    glutSolidTeaspoon(orad); glFrontFace(GL_CCW);    /* orad doubles as size input */}
static void drawWireTeaspoon(void)
{   glFrontFace(GL_CW);    glutWireTeaspoon(orad);  glFrontFace(GL_CCW);    /* orad doubles as size input */}

#define RADIUSFAC    0.70710678118654752440084436210485f

static void drawSolidCuboctahedron(void)
{
  GLfloat radius = RADIUSFAC*(GLfloat)orad; /* orad doubles as size */
  glBegin( GL_TRIANGLES );
    glNormal3d( 0.577350269189, 0.577350269189, 0.577350269189); glVertex3d( radius, radius, 0.0 ); glVertex3d( 0.0, radius, radius ); glVertex3d( radius, 0.0, radius );
    glNormal3d( 0.577350269189, 0.577350269189,-0.577350269189); glVertex3d( radius, radius, 0.0 ); glVertex3d( radius, 0.0,-radius ); glVertex3d( 0.0, radius,-radius );
    glNormal3d( 0.577350269189,-0.577350269189, 0.577350269189); glVertex3d( radius,-radius, 0.0 ); glVertex3d( radius, 0.0, radius ); glVertex3d( 0.0,-radius, radius );
    glNormal3d( 0.577350269189,-0.577350269189,-0.577350269189); glVertex3d( radius,-radius, 0.0 ); glVertex3d( 0.0,-radius,-radius ); glVertex3d( radius, 0.0,-radius );
    glNormal3d(-0.577350269189, 0.577350269189, 0.577350269189); glVertex3d(-radius, radius, 0.0 ); glVertex3d(-radius, 0.0, radius ); glVertex3d( 0.0, radius, radius );
    glNormal3d(-0.577350269189, 0.577350269189,-0.577350269189); glVertex3d(-radius, radius, 0.0 ); glVertex3d( 0.0, radius,-radius ); glVertex3d(-radius, 0.0,-radius );
    glNormal3d(-0.577350269189,-0.577350269189, 0.577350269189); glVertex3d(-radius,-radius, 0.0 ); glVertex3d( 0.0,-radius, radius ); glVertex3d(-radius, 0.0, radius );
    glNormal3d(-0.577350269189,-0.577350269189,-0.577350269189); glVertex3d(-radius,-radius, 0.0 ); glVertex3d(-radius, 0.0,-radius ); glVertex3d( 0.0,-radius,-radius );
  glEnd();

  glBegin( GL_QUADS );
    glNormal3d( 1.0, 0.0, 0.0 ); glVertex3d( radius, radius, 0.0 ); glVertex3d( radius, 0.0, radius ); glVertex3d( radius,-radius, 0.0 ); glVertex3d( radius, 0.0,-radius );
    glNormal3d(-1.0, 0.0, 0.0 ); glVertex3d(-radius, radius, 0.0 ); glVertex3d(-radius, 0.0,-radius ); glVertex3d(-radius,-radius, 0.0 ); glVertex3d(-radius, 0.0, radius );
    glNormal3d( 0.0, 1.0, 0.0 ); glVertex3d( radius, radius, 0.0 ); glVertex3d( 0.0, radius,-radius ); glVertex3d(-radius, radius, 0.0 ); glVertex3d( 0.0, radius, radius );
    glNormal3d( 0.0,-1.0, 0.0 ); glVertex3d( radius,-radius, 0.0 ); glVertex3d( 0.0,-radius, radius ); glVertex3d(-radius,-radius, 0.0 ); glVertex3d( 0.0,-radius,-radius );
    glNormal3d( 0.0, 0.0, 1.0 ); glVertex3d( radius, 0.0, radius ); glVertex3d( 0.0, radius, radius ); glVertex3d(-radius, 0.0, radius ); glVertex3d( 0.0,-radius, radius );
    glNormal3d( 0.0, 0.0,-1.0 ); glVertex3d( radius, 0.0,-radius ); glVertex3d( 0.0,-radius,-radius ); glVertex3d(-radius, 0.0,-radius ); glVertex3d( 0.0, radius,-radius );
  glEnd();
}

static void drawWireCuboctahedron(void)
{
  GLfloat radius = RADIUSFAC*(GLfloat)orad; /* orad doubles as size */
  glBegin( GL_LINE_LOOP );
    glNormal3d( 1.0, 0.0, 0.0 ); glVertex3d( radius, radius, 0.0 ); glVertex3d( radius, 0.0, radius ); glVertex3d( radius,-radius, 0.0 ); glVertex3d( radius, 0.0,-radius );
  glEnd();
  glBegin( GL_LINE_LOOP );
    glNormal3d(-1.0, 0.0, 0.0 ); glVertex3d(-radius, radius, 0.0 ); glVertex3d(-radius, 0.0,-radius ); glVertex3d(-radius,-radius, 0.0 ); glVertex3d(-radius, 0.0, radius );
  glEnd();
  glBegin( GL_LINE_LOOP );
    glNormal3d( 0.0, 1.0, 0.0 ); glVertex3d( radius, radius, 0.0 ); glVertex3d( 0.0, radius,-radius ); glVertex3d(-radius, radius, 0.0 ); glVertex3d( 0.0, radius, radius );
  glEnd();
  glBegin( GL_LINE_LOOP );
    glNormal3d( 0.0,-1.0, 0.0 ); glVertex3d( radius,-radius, 0.0 ); glVertex3d( 0.0,-radius, radius ); glVertex3d(-radius,-radius, 0.0 ); glVertex3d( 0.0,-radius,-radius );
  glEnd();
  glBegin( GL_LINE_LOOP );
    glNormal3d( 0.0, 0.0, 1.0 ); glVertex3d( radius, 0.0, radius ); glVertex3d( 0.0, radius, radius ); glVertex3d(-radius, 0.0, radius ); glVertex3d( 0.0,-radius, radius );
  glEnd();
  glBegin( GL_LINE_LOOP );
    glNormal3d( 0.0, 0.0,-1.0 ); glVertex3d( radius, 0.0,-radius ); glVertex3d( 0.0,-radius,-radius ); glVertex3d(-radius, 0.0,-radius ); glVertex3d( 0.0, radius,-radius );
  glEnd();
}

#undef RADIUSFAC

/*
 * This structure defines an entry in our function-table.
 */
typedef struct
{
    const char * const name;
    void (*solid) (void);
    void (*wire)  (void);
    int drawSizeInfoFlag;
} entry;

#define ENTRY(e,f) {#e, drawSolid##e, drawWire##e,f}
static const entry table [] =
{
    ENTRY (Tetrahedron,GEO_NO_SIZE),
    ENTRY (Cube,GEO_SIZE),
    ENTRY (Octahedron,GEO_NO_SIZE),
    ENTRY (Dodecahedron,GEO_NO_SIZE),
    ENTRY (RhombicDodecahedron,GEO_NO_SIZE),
    ENTRY (Icosahedron,GEO_NO_SIZE),
    ENTRY (SierpinskiSponge,GEO_SCALE),
    ENTRY (Teapot,GEO_SIZE),
    ENTRY (Teacup,GEO_SIZE),
    ENTRY (Teaspoon,GEO_SIZE),
    ENTRY (Torus,GEO_INNER_OUTER_RAD),
    ENTRY (Sphere,GEO_RAD),
    ENTRY (Cone,GEO_BASE_HEIGHT),
    ENTRY (Cylinder,GEO_RAD_HEIGHT),
    ENTRY (Cuboctahedron,GEO_SIZE)   /* This one doesn't work when in shader mode and is then skipped */
};
#undef ENTRY

/*!
    Does printf()-like work using freeglut
    glutBitmapString().  Uses a fixed font.  Prints
    at the indicated row/column position.

    Limitation: Cannot address pixels.
    Limitation: Renders in screen coords, not model coords.
*/
static void shapesPrintf (int row, int col, const char *fmt, ...)
{
    static char buf[256];
    int viewport[4];
    void *font = GLUT_BITMAP_9_BY_15;
    va_list args;

    va_start(args, fmt);
#if defined(WIN32) && !defined(__CYGWIN__)
    (void) _vsnprintf (buf, sizeof(buf), fmt, args);
#else
    (void) vsnprintf (buf, sizeof(buf), fmt, args);
#endif
    va_end(args);

    glGetIntegerv(GL_VIEWPORT,viewport);

    glPushMatrix();
    glLoadIdentity();

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();

        glOrtho(0,viewport[2],0,viewport[3],-1,1);

        glRasterPos2i
        (
              glutBitmapWidth(font, ' ') * col,
            - glutBitmapHeight(font) * row + viewport[3]
        );
        glutBitmapString (font, (unsigned char*)buf);

    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

/* Print info about the about the current shape and render state on the screen */
static void DrawSizeInfo(int *row)
{
    switch (table [function_index].drawSizeInfoFlag)
    {
    case GEO_NO_SIZE:
        break;
    case GEO_SIZE:
        shapesPrintf ((*row)++, 1, "Size  Up  Down : %f", orad);
        break;
    case GEO_SCALE:
        shapesPrintf ((*row)++, 1, "Scale  Up  Down : %f", orad);
        break;
    case GEO_INNER_OUTER_RAD:
        shapesPrintf ((*row)++, 1, "Inner radius Left Right: %f", irad);
        shapesPrintf ((*row)++, 1, "Outer radius  Up  Down : %f", orad);
        break;
    case GEO_RAD:
        shapesPrintf ((*row)++, 1, "Radius  Up  Down : %f", orad);
        break;
    case GEO_BASE_HEIGHT:
        shapesPrintf ((*row)++, 1, "Base   Left Right: %f", irad);
        shapesPrintf ((*row)++, 1, "Height  Up  Down : %f", orad);
        break;
    case GEO_RAD_HEIGHT:
        shapesPrintf ((*row)++, 1, "Radius Left Right: %f", irad);
        shapesPrintf ((*row)++, 1, "Height  Up  Down : %f", orad);
        break;
    }
}

static void drawInfo()
{
    int row = 1;
    shapesPrintf (row++, 1, "Shape PgUp PgDn: %s", table [function_index].name);
    shapesPrintf (row++, 1, "Slices +-: %d   Stacks <>: %d", slices, stacks);
    shapesPrintf (row++, 1, "nSides +-: %d   nRings <>: %d", slices, stacks);
    shapesPrintf (row++, 1, "Depth  (): %d", depth);
    DrawSizeInfo(&row);
    if (persProject)
        shapesPrintf (row++, 1, "Perspective projection (p)");
    else
        shapesPrintf (row++, 1, "Orthographic projection (p)");
    if (useShader)
        shapesPrintf (row++, 1, "Using shader (s)");
    else
        shapesPrintf (row++, 1, "Using fixed function pipeline (s)");
    if (animateXRot)
        shapesPrintf (row++, 1, "2D rotation (r)");
    else
        shapesPrintf (row++, 1, "1D rotation (r)");
    shapesPrintf (row++, 1, "visualizing normals: %i (n)",visNormals);
}

/* GLUT callback Handlers */
static void
resize(int width, int height)
{
    ar = (float) width / (float) height;

    glViewport(0, 0, width, height);
}

static void display(void)
{
    const double t = glutGet(GLUT_ELAPSED_TIME) / 1000.0;
    const double a = t*89.0;
    const double b = (animateXRot?t:1)*67.0;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glutSetOption(GLUT_GEOMETRY_VISUALIZE_NORMALS,visNormals);  /* Normals visualized or not? */

    if (useShader && !shaderReady)
        initShader();

    if (useShader && shaderReady)
    {
        /* setup use of shader (and vertex buffer by FreeGLUT) */
        gl_UseProgram (program);
        glutSetVertexAttribCoord3(attribute_fg_coord);
        glutSetVertexAttribNormal(attribute_fg_normal);
        /* There is also a glutSetVertexAttribTexCoord2, which is used only when drawing the teapot, teacup or teaspoon */

        gl_matrix_mode(GL_PROJECTION);
        gl_load_identity();
        if (persProject)
            gl_frustum(-ar, ar, -1.f, 1.f, 2.f, 100.f);
        else
            gl_ortho(-ar*3, ar*3, -3.f, 3.f, 2.f, 100.f);
        gl_UniformMatrix4fv (uniform_p, 1, GL_FALSE, get_matrix(GL_PROJECTION));


        gl_matrix_mode(GL_MODELVIEW);
        gl_load_identity();

        gl_push_matrix();
            /* Not in reverse order like normal OpenGL, our util library multiplies the matrices in the order they are specified in */
            gl_rotatef((float)a,0,0,1);
            gl_rotatef((float)b,1,0,0);
            gl_translatef(0,1.2f,-6);
            gl_UniformMatrix4fv (uniform_m               , 1, GL_FALSE, get_matrix(GL_MODELVIEW));
            gl_UniformMatrix3fv (uniform_m_3x3_inv_transp, 1, GL_FALSE, get_inv_transpose_3x3(GL_MODELVIEW));
            table [function_index].solid ();
        gl_pop_matrix();

        gl_push_matrix();
            gl_rotatef((float)a,0,0,1);
            gl_rotatef((float)b,1,0,0);
            gl_translatef(0,-1.2f,-6);
            gl_UniformMatrix4fv (uniform_m               , 1, GL_FALSE, get_matrix(GL_MODELVIEW));
            gl_UniformMatrix3fv (uniform_m_3x3_inv_transp, 1, GL_FALSE, get_inv_transpose_3x3(GL_MODELVIEW));
            table [function_index].wire ();
        gl_pop_matrix();

        gl_UseProgram (0);
        glutSetVertexAttribCoord3(-1);
        glutSetVertexAttribNormal(-1);

        checkError ("display");
    }
    else
    {
        /* fixed function pipeline */
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        if (persProject)
            glFrustum(-ar, ar, -1.0, 1.0, 2.0, 100.0);
        else
            glOrtho(-ar*3, ar*3, -3.0, 3.0, 2.0, 100.0);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glEnable(GL_LIGHTING);

        glColor3d(1,0,0);

        glPushMatrix();
            glTranslated(0,1.2,-6);
            glRotated(b,1,0,0);
            glRotated(a,0,0,1);
            table [function_index].solid ();
        glPopMatrix();

        glPushMatrix();
            glTranslated(0,-1.2,-6);
            glRotated(b,1,0,0);
            glRotated(a,0,0,1);
            table [function_index].wire ();
        glPopMatrix();

        glDisable(GL_LIGHTING);
        glColor3d(0.1,0.1,0.4);
    }

    if( show_info )
        /* print info to screen */
        drawInfo();
    else
        /* print to command line instead */
        printf ( "Shape %d slides %d stacks %d\n", function_index, slices, stacks ) ;

    glutSwapBuffers();
}


static void
key(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 27 :
    case 'Q':
    case 'q': glutLeaveMainLoop () ;      break;

    case 'I':
    case 'i': show_info=!show_info;       break;

    case '=':
    case '+': slices++;                   break;

    case '-':
    case '_': if( slices > -1 ) slices--; break;

    case ',':
    case '<': if( stacks > -1 ) stacks--; break;

    case '.':
    case '>': stacks++;                   break;

    case '9': 
    case '(': if( depth > -1 ) depth--;   break;

    case '0': 
    case ')': ++depth;                    break;

    case 'P':
    case 'p': persProject=!persProject;   break;

    case 'R':
    case 'r': animateXRot=!animateXRot;   break;

    case 'S':
    case 's':
        useShader=!useShader;
        /* Cuboctahedron can't be shown when in shader mode, move to next */
        if (useShader && NUMBEROF (table)-1 == ( unsigned )function_index)
                function_index = 0;
        break;

    case 'N':
    case 'n': visNormals=!visNormals;     break;

    default:
        break;
    }

    glutPostRedisplay();
}

static void special (int key, int x, int y)
{
    switch (key)
    {
    case GLUT_KEY_PAGE_UP:    ++function_index; break;
    case GLUT_KEY_PAGE_DOWN:  --function_index; break;
    case GLUT_KEY_UP:         orad *= 2;        break;
    case GLUT_KEY_DOWN:       orad /= 2;        break;

    case GLUT_KEY_RIGHT:      irad *= 2;        break;
    case GLUT_KEY_LEFT:       irad /= 2;        break;

    default:
        break;
    }

    if (0 > function_index)
        function_index = NUMBEROF (table) - 1;

    if (NUMBEROF (table) <= ( unsigned )function_index)
        function_index = 0;

    /* Cuboctahedron can't be shown when in shader mode, skip it */
    if (useShader && NUMBEROF (table)-1 == ( unsigned )function_index)
    {
        if (key==GLUT_KEY_PAGE_UP)
            function_index = 0;
        else
            function_index -= 1;
    }
}


static void
idle(void)
{
    glutPostRedisplay();
}

const GLfloat light_ambient[]  = { 0.0f, 0.0f, 0.0f, 1.0f };
const GLfloat light_diffuse[]  = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat light_position[] = { 2.0f, 5.0f, 5.0f, 0.0f };

const GLfloat mat_ambient[]    = { 0.7f, 0.7f, 0.7f, 1.0f };
const GLfloat mat_diffuse[]    = { 0.8f, 0.8f, 0.8f, 1.0f };
const GLfloat mat_specular[]   = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat high_shininess[] = { 100.0f };

/* Program entry point */

int
main(int argc, char *argv[])
{
    glutInitWindowSize(800,600);
    glutInitWindowPosition(40,40);
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE);

    glutCreateWindow("FreeGLUT Shapes");

    glutReshapeFunc(resize);
    glutDisplayFunc(display);
    glutKeyboardFunc(key);
    glutSpecialFunc(special);
    glutIdleFunc(idle);

    glutSetOption ( GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION ) ;

    glClearColor(1,1,1,1);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);

    glLightfv(GL_LIGHT0, GL_AMBIENT,  light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    glMaterialfv(GL_FRONT, GL_AMBIENT,   mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE,   mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR,  mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess);

    initExtensionEntries();

    glutMainLoop();

#ifdef _MSC_VER
    /* DUMP MEMORY LEAK INFORMATION */
    _CrtDumpMemoryLeaks () ;
#endif

    return EXIT_SUCCESS;
}
