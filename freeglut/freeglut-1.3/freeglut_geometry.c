/*
 * freeglut_geometry.c
 *
 * Freeglut geometry rendering methods.
 *
 * Copyright (c) 1999-2000 Pawel W. Olszta. All Rights Reserved.
 * Written by Pawel W. Olszta, <olszta@sourceforge.net>
 * Creation date: Fri Dec 3 1999
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define  G_LOG_DOMAIN  "freeglut-geometry"

#include "../include/GL/freeglut.h"
#include "../include/GL/freeglut_internal.h"

/*
 * TODO BEFORE THE STABLE RELEASE:
 *
 * Following functions have been contributed by Andreas Umbach.
 *
 *      glutWireCube()          -- looks OK
 *      glutSolidCube()         -- OK
 *      glutWireSphere()        -- OK
 *      glutSolidSphere()       -- OK
 *
 * Following functions have been implemented by Pawel and modified by John Fay:
 *
 *      glutWireCone()          -- looks OK
 *      glutSolidCone()         -- looks OK
 *
 * Those functions have been implemented by John Fay.
 *
 *      glutWireTorus()         -- looks OK
 *      glutSolidTorus()        -- looks OK
 *      glutWireDodecahedron()  -- looks OK
 *      glutSolidDodecahedron() -- looks OK
 *      glutWireOctahedron()    -- looks OK
 *      glutSolidOctahedron()   -- looks OK
 *      glutWireTetrahedron()   -- looks OK
 *      glutSolidTetrahedron()  -- looks OK
 *      glutWireIcosahedron()   -- looks OK
 *      glutSolidIcosahedron()  -- looks OK
 */


/* -- INTERFACE FUNCTIONS -------------------------------------------------- */

/*
 * Draws a wireframed cube. Code contributed by Andreas Umbach <marvin@dataway.ch>
 */
void FGAPIENTRY glutWireCube( GLdouble dSize )
{
    float size = (float) dSize * 0.5f;

#   define V(a,b,c) glVertex3f( a size, b size, c size );
#   define N(a,b,c) glNormal3f( a, b, c );

    /*
     * PWO: I dared to convert the code to use macros...
     */
    glBegin( GL_LINE_LOOP ); N( 1.0, 0.0, 0.0); V(+,-,+); V(+,-,-); V(+,+,-); V(+,+,+); glEnd();
    glBegin( GL_LINE_LOOP ); N( 0.0, 1.0, 0.0); V(+,+,+); V(+,+,-); V(-,+,-); V(-,+,+); glEnd();
    glBegin( GL_LINE_LOOP ); N( 0.0, 0.0, 1.0); V(+,+,+); V(-,+,+); V(-,-,+); V(+,-,+); glEnd();
    glBegin( GL_LINE_LOOP ); N(-1.0, 0.0, 0.0); V(-,-,+); V(-,+,+); V(-,+,-); V(-,-,-); glEnd();
    glBegin( GL_LINE_LOOP ); N( 0.0,-1.0, 0.0); V(-,-,+); V(-,-,-); V(+,-,-); V(+,-,+); glEnd();
    glBegin( GL_LINE_LOOP ); N( 0.0, 0.0,-1.0); V(-,-,-); V(-,+,-); V(+,+,-); V(+,-,-); glEnd();

#   undef V
#   undef N
}

/*
 * Draws a solid cube. Code contributed by Andreas Umbach <marvin@dataway.ch>
 */
void FGAPIENTRY glutSolidCube( GLdouble dSize )
{
    float size = (float) dSize * 0.5f;

#   define V(a,b,c) glVertex3f( a size, b size, c size );
#   define N(a,b,c) glNormal3f( a, b, c );

    /*
     * PWO: Again, I dared to convert the code to use macros...
     */
    glBegin( GL_QUADS );
        N( 1.0, 0.0, 0.0); V(+,-,+); V(+,-,-); V(+,+,-); V(+,+,+);
        N( 0.0, 1.0, 0.0); V(+,+,+); V(+,+,-); V(-,+,-); V(-,+,+);
        N( 0.0, 0.0, 1.0); V(+,+,+); V(-,+,+); V(-,-,+); V(+,-,+);
        N(-1.0, 0.0, 0.0); V(-,-,+); V(-,+,+); V(-,+,-); V(-,-,-);
        N( 0.0,-1.0, 0.0); V(-,-,+); V(-,-,-); V(+,-,-); V(+,-,+);
        N( 0.0, 0.0,-1.0); V(-,-,-); V(-,+,-); V(+,+,-); V(+,-,-);
    glEnd();

#   undef V
#   undef N
}

/*
 * Draws a wire sphere. Code contributed by Andreas Umbach <marvin@dataway.ch>
 */
void FGAPIENTRY glutWireSphere( GLdouble dRadius, GLint slices, GLint stacks )
{
    float  radius = (float) dRadius, phi, psi, dpsi, dphi;
    float* vertex;
    int    i, j;
    float cphi, sphi, cpsi, spsi ;

    /*
     * Allocate the vertices array
     */
    vertex = calloc( sizeof(float), 3 * slices * (stacks - 1) );

    glPushMatrix();
    glScalef( radius, radius, radius );

    dpsi = M_PI / (stacks + 1);
    dphi = 2 * M_PI / slices;
    psi  = dpsi;

    for( j=0; j<stacks-1; j++ )
    {
        cpsi = (float)cos ( psi ) ;
        spsi = (float)sin ( psi ) ;
        phi = 0;

        for( i=0; i<slices; i++ )
        {
          int offset = 3 * ( j * slices + i ) ;
          cphi = (float)cos ( phi ) ;
          sphi = (float)sin ( phi ) ;
            *(vertex + offset + 0) = sphi * spsi ;
            *(vertex + offset + 1) = cphi * spsi ;
            *(vertex + offset + 2) = cpsi ;
            phi += dphi;
        }

        psi += dpsi;
    }

    for( i=0; i<slices; i++ )
    {
        glBegin( GL_LINE_STRIP );
        glNormal3f( 0, 0, 1 );
        glVertex3f( 0, 0, 1 );

        for( j=0; j<stacks - 1; j++ )
        {
          int offset = 3 * ( j * slices + i ) ;
            glNormal3fv( vertex + offset );
            glVertex3fv( vertex + offset );
        }

        glNormal3f(0, 0, -1);
        glVertex3f(0, 0, -1);
        glEnd();
    }

    for( j=0; j<stacks-1; j++ )
    {
        glBegin(GL_LINE_LOOP);

        for( i=0; i<slices; i++ )
        {
          int offset = 3 * ( j * slices + i ) ;
            glNormal3fv( vertex + offset );
            glVertex3fv( vertex + offset );
        }

        glEnd();
    }

    free( vertex );
    glPopMatrix();
}

/*
 * Draws a solid sphere. Code contributed by Andreas Umbach <marvin@dataway.ch>
 */
void FGAPIENTRY glutSolidSphere( GLdouble dRadius, GLint slices, GLint stacks )
{
    float  radius = (float) dRadius, phi, psi, dpsi, dphi;
    float *next, *tmp, *row;
    int    i, j;
    float cphi, sphi, cpsi, spsi ;

    glPushMatrix();
    //glScalef( radius, radius, radius );

    row  = calloc( sizeof(float), slices * 3 );
    next = calloc( sizeof(float), slices * 3 );

    dpsi = M_PI / (stacks + 1);
    dphi = 2 * M_PI / slices;
    psi  = dpsi;
    phi  = 0;

    /* init first line + do polar cap */
    glBegin( GL_TRIANGLE_FAN );
    glNormal3f( 0, 0, 1 );
    glVertex3f( 0, 0, radius );

    for( i=0; i<slices; i++ )
    {
        row[ i * 3 + 0 ] = sin( phi ) * sin( psi );
        row[ i * 3 + 1 ] = cos( phi ) * sin( psi );
        row[ i * 3 + 2 ] = cos( psi );

        glNormal3fv( row + 3 * i );
        glVertex3f(
            radius * *(row + 3 * i + 0),
            radius * *(row + 3 * i + 1),
	        radius * *(row + 3 * i + 2)
	    );
	
        phi += dphi;
    }

    glNormal3fv( row );
    glVertex3f( radius * *(row + 0), radius * *(row + 1), radius * *(row + 2) );
    glEnd();

    for( j=0; j<stacks-1; j++ )
    {
        phi = 0;
        psi += dpsi;
        cpsi = (float)cos ( psi ) ;
        spsi = (float)sin ( psi ) ;

        /* get coords */
        glBegin( GL_QUAD_STRIP );

        /* glBegin(GL_LINE_LOOP); */
        for( i=0; i<slices; i++ )
        {
          cphi = (float)cos ( phi ) ;
          sphi = (float)sin ( phi ) ;
            next[ i * 3 + 0 ] = sphi * spsi ;
            next[ i * 3 + 1 ] = cphi * spsi ;
            next[ i * 3 + 2 ] = cpsi ;

            glNormal3fv( row + i * 3 );
            glVertex3f(
                radius * *(row + 3 * i + 0),
                radius * *(row + 3 * i + 1),
		        radius * *(row + 3 * i + 2)
		    );

            glNormal3fv( next + i * 3 );
            glVertex3f(
                radius * *(next + 3 * i + 0),
                radius * *(next + 3 * i + 1),
                radius * *(next + 3 * i + 2)
            );

            phi += dphi;
        }

        glNormal3fv( row );
        glVertex3f( radius * *(row + 0), radius * *(row + 1), radius * *(row + 2) );
        glNormal3fv( next );
        glVertex3f( radius * *(next + 0), radius * *(next + 1), radius * *(next + 2) );
        glEnd();

        tmp = row;
        row = next;
        next = tmp;
    }

    /* south pole */
    glBegin( GL_TRIANGLE_FAN );
    glNormal3f( 0, 0, -1 );
    glVertex3f( 0, 0, -radius );
    glNormal3fv( row );
    glVertex3f( radius * *(row + 0), radius * *(row + 1), radius * *(row + 2) );

    for( i=slices-1; i>=0; i-- )
    {
        glNormal3fv(row + 3 * i);
        glVertex3f(
            radius * *(row + 3 * i + 0),
            radius * *(row + 3 * i + 1),
	        radius * *(row + 3 * i + 2)
	   );
    }

    glEnd();

    free(row);
    free(next);
    glPopMatrix();
}

/*
 * Draws a wire cone
 */
void FGAPIENTRY glutWireCone( GLdouble base, GLdouble height, GLint slices, GLint stacks )
{
  float  alt   = (float) height / (float) (stacks + 1);
  float  angle = (float) M_PI / (float) slices * 2.0f;
  float  slope = (float) ( height / base );
  float  sBase = (float)base ;
  float  sinNormal = (float)( base   / sqrt ( height * height + base * base )) ;
  float  cosNormal = (float)( height / sqrt ( height * height + base * base )) ;

  float* vertices = NULL;
  int    i, j;

  /*
   * We need 'slices' points on a circle
   */
  vertices = calloc( sizeof(float), 2 * (slices + 1) );

  for( j=0; j<slices+1; j++ )
  {
    vertices[ j*2 + 0 ] = cos( angle * j );
    vertices[ j*2 + 1 ] = sin( angle * j );
  }

  /*
   * First the cone's bottom...
   */
  for( j=0; j<slices; j++ )
  {
    glBegin( GL_LINE_LOOP );
      glNormal3f( 0, 0, -1 );
      glVertex3f( vertices[ (j+0)*2+0 ] * sBase, vertices[ (j+0)*2+1 ] * sBase, 0 );
      glVertex3f( vertices[ (j+1)*2+0 ] * sBase, vertices[ (j+1)*2+1 ] * sBase, 0 );
      glVertex3f( 0, 0, 0 );
    glEnd();
  }

  /*
   * Then all the stacks between the bottom and the top
   */
  for( i=0; i<stacks; i++ )
  {
    float alt_a = i * alt, alt_b = (i + 1) * alt;
    float scl_a = (height - alt_a) / slope;
    float scl_b = (height - alt_b) / slope;

    for( j=0; j<slices; j++ )
    {
      glBegin( GL_LINE_LOOP );
        glNormal3f( sinNormal * vertices[(j+0)*2+0], sinNormal * vertices[(j+0)*2+1], cosNormal ) ;
        glVertex3f( vertices[(j+0)*2+0] * scl_a, vertices[(j+0)*2+1] * scl_a, alt_a );
        glNormal3f( sinNormal * vertices[(j+1)*2+0], sinNormal * vertices[(j+1)*2+1], cosNormal ) ;
        glVertex3f( vertices[(j+1)*2+0] * scl_a, vertices[(j+1)*2+1] * scl_a, alt_a );
        glNormal3f( sinNormal * vertices[(j+0)*2+0], sinNormal * vertices[(j+0)*2+1], cosNormal ) ;
        glVertex3f( vertices[(j+0)*2+0] * scl_b, vertices[(j+0)*2+1] * scl_b, alt_b );
      glEnd();

      glBegin( GL_LINE_LOOP );
        glNormal3f( sinNormal * vertices[(j+0)*2+0], sinNormal * vertices[(j+0)*2+1], cosNormal ) ;
        glVertex3f( vertices[(j+0)*2+0] * scl_b, vertices[(j+0)*2+1] * scl_b, alt_b );
        glNormal3f( sinNormal * vertices[(j+1)*2+0], sinNormal * vertices[(j+1)*2+1], cosNormal ) ;
        glVertex3f( vertices[(j+1)*2+0] * scl_b, vertices[(j+1)*2+1] * scl_b, alt_b );
        glVertex3f( vertices[(j+1)*2+0] * scl_a, vertices[(j+1)*2+1] * scl_a, alt_a );
      glEnd();
    }
  }

  /*
   * Finally have the top part drawn...
   */
  for( j=0; j<slices; j++ )
  {
    float scl = alt / slope;

    glBegin( GL_LINE_LOOP );
      glNormal3f( sinNormal * vertices[(j+0)*2+0], sinNormal * vertices[(j+0)*2+1], cosNormal ) ;
      glVertex3f( vertices[ (j+0)*2+0 ] * scl, vertices[ (j+0)*2+1 ] * scl, height - alt );
      glNormal3f( sinNormal * vertices[(j+1)*2+0], sinNormal * vertices[(j+1)*2+1], cosNormal ) ;
      glVertex3f( vertices[ (j+1)*2+0 ] * scl, vertices[ (j+1)*2+1 ] * scl, height - alt );
      glVertex3f( 0, 0, height );
    glEnd();
  }
}

/*
 * Draws a solid cone
 */
void FGAPIENTRY glutSolidCone( GLdouble base, GLdouble height, GLint slices, GLint stacks )
{
  float  alt   = (float) height / (float) (stacks + 1);
  float  angle = (float) M_PI / (float) slices * 2.0f;
  float  slope = (float) ( height / base );
  float  sBase = (float)base ;
  float  sinNormal = (float)( base   / sqrt ( height * height + base * base )) ;
  float  cosNormal = (float)( height / sqrt ( height * height + base * base )) ;

  float* vertices = NULL;
  int    i, j;

  /*
   * We need 'slices' points on a circle
   */
  vertices = calloc( sizeof(float), 2 * (slices + 1) );

  for( j=0; j<slices+1; j++ )
  {
    vertices[ j*2 + 0 ] = cos( angle * j );
    vertices[ j*2 + 1 ] = sin( angle * j );
  }

  /*
   * First the cone's bottom...
   */
  for( j=0; j<slices; j++ )
  {
    float scl = height / slope;

    glBegin( GL_TRIANGLES );
      glNormal3f( 0, 0, -1 );
      glVertex3f( vertices[ (j+0)*2+0 ] * sBase, vertices[ (j+0)*2+1 ] * sBase, 0 );
      glVertex3f( vertices[ (j+1)*2+0 ] * sBase, vertices[ (j+1)*2+1 ] * sBase, 0 );
      glVertex3f( 0, 0, 0 );
    glEnd();
  }

  /*
   * Then all the stacks between the bottom and the top
   */
  for( i=0; i<stacks; i++ )
  {
    float alt_a = i * alt, alt_b = (i + 1) * alt;
    float scl_a = (height - alt_a) / slope;
    float scl_b = (height - alt_b) / slope;

    for( j=0; j<slices; j++ )
    {
      glBegin( GL_TRIANGLES );
        glNormal3f( sinNormal * vertices[(j+0)*2+0], sinNormal * vertices[(j+0)*2+1], cosNormal ) ;
        glVertex3f( vertices[(j+0)*2+0] * scl_a, vertices[(j+0)*2+1] * scl_a, alt_a );
        glNormal3f( sinNormal * vertices[(j+1)*2+0], sinNormal * vertices[(j+1)*2+1], cosNormal ) ;
        glVertex3f( vertices[(j+1)*2+0] * scl_a, vertices[(j+1)*2+1] * scl_a, alt_a );
        glNormal3f( sinNormal * vertices[(j+0)*2+0], sinNormal * vertices[(j+0)*2+1], cosNormal ) ;
        glVertex3f( vertices[(j+0)*2+0] * scl_b, vertices[(j+0)*2+1] * scl_b, alt_b );
      glEnd();

      glBegin( GL_TRIANGLES );
        glNormal3f( sinNormal * vertices[(j+0)*2+0], sinNormal * vertices[(j+0)*2+1], cosNormal ) ;
        glVertex3f( vertices[(j+0)*2+0] * scl_b, vertices[(j+0)*2+1] * scl_b, alt_b );
        glNormal3f( sinNormal * vertices[(j+1)*2+0], sinNormal * vertices[(j+1)*2+1], cosNormal ) ;
        glVertex3f( vertices[(j+1)*2+0] * scl_b, vertices[(j+1)*2+1] * scl_b, alt_b );
        glVertex3f( vertices[(j+1)*2+0] * scl_a, vertices[(j+1)*2+1] * scl_a, alt_a );
      glEnd();
    }
  }

  /*
   * Finally have the top part drawn...
   */
  for( j=0; j<slices; j++ )
  {
    float scl = alt / slope;

    glBegin( GL_TRIANGLES );
      glNormal3f( sinNormal * vertices[(j+0)*2+0], sinNormal * vertices[(j+0)*2+1], cosNormal ) ;
      glVertex3f( vertices[ (j+0)*2+0 ] * scl, vertices[ (j+0)*2+1 ] * scl, height - alt );
      glNormal3f( sinNormal * vertices[(j+1)*2+0], sinNormal * vertices[(j+1)*2+1], cosNormal ) ;
      glVertex3f( vertices[ (j+1)*2+0 ] * scl, vertices[ (j+1)*2+1 ] * scl, height - alt );
      glVertex3f( 0, 0, height );
    glEnd();
  }
}

/*
 *
 */
void FGAPIENTRY glutWireTorus( GLdouble dInnerRadius, GLdouble dOuterRadius, GLint nSides, GLint nRings )
{
  float  iradius = (float) dInnerRadius, oradius = (float)dOuterRadius, phi, psi, dpsi, dphi;
  float* vertex, *normal;
  int    i, j;
  float spsi, cpsi, sphi, cphi ;

  /*
   * Allocate the vertices array
   */
  vertex = calloc( sizeof(float), 3 * nSides * nRings );
  normal = calloc( sizeof(float), 3 * nSides * nRings );

  glPushMatrix();

  dpsi = 2.0 * M_PI / (float)nRings ;
  dphi = 2.0 * M_PI / (float)nSides ;
  psi  = 0.0;

  for( j=0; j<nRings; j++ )
  {
    cpsi = (float)cos ( psi ) ;
    spsi = (float)sin ( psi ) ;
    phi = 0.0;

    for( i=0; i<nSides; i++ )
    {
      int offset = 3 * ( j * nSides + i ) ;
      cphi = (float)cos ( phi ) ;
      sphi = (float)sin ( phi ) ;
      *(vertex + offset + 0) = cpsi * ( oradius + cphi * iradius ) ;
      *(vertex + offset + 1) = spsi * ( oradius + cphi * iradius ) ;
      *(vertex + offset + 2) =                    sphi * iradius   ;
      *(normal + offset + 0) = cpsi * cphi ;
      *(normal + offset + 1) = spsi * cphi ;
      *(normal + offset + 2) =        sphi ;
      phi += dphi;
    }

    psi += dpsi;
  }

  for( i=0; i<nSides; i++ )
  {
    glBegin( GL_LINE_LOOP );

    for( j=0; j<nRings; j++ )
    {
      int offset = 3 * ( j * nSides + i ) ;
      glNormal3fv( normal + offset );
      glVertex3fv( vertex + offset );
    }

    glEnd();
  }

  for( j=0; j<nRings; j++ )
  {
    glBegin(GL_LINE_LOOP);

    for( i=0; i<nSides; i++ )
    {
      int offset = 3 * ( j * nSides + i ) ;
      glNormal3fv( normal + offset );
      glVertex3fv( vertex + offset );
    }

    glEnd();
  }

  free ( vertex ) ;
  free ( normal ) ;
  glPopMatrix();
}

/*
 *
 */
void FGAPIENTRY glutSolidTorus( GLdouble dInnerRadius, GLdouble dOuterRadius, GLint nSides, GLint nRings )
{
  float  iradius = (float) dInnerRadius, oradius = (float)dOuterRadius, phi, psi, dpsi, dphi;
  float* vertex, *normal;
  int    i, j;
  float spsi, cpsi, sphi, cphi ;

  /*
   * Increment the number of sides and rings to allow for one more point than surface
   */
  nSides ++ ;
  nRings ++ ;

  /*
   * Allocate the vertices array
   */
  vertex = calloc( sizeof(float), 3 * nSides * nRings );
  normal = calloc( sizeof(float), 3 * nSides * nRings );

  glPushMatrix();

  dpsi = 2.0 * M_PI / (float)(nRings - 1) ;
  dphi = 2.0 * M_PI / (float)(nSides - 1) ;
  psi  = 0.0;

  for( j=0; j<nRings; j++ )
  {
    cpsi = (float)cos ( psi ) ;
    spsi = (float)sin ( psi ) ;
    phi = 0.0;

    for( i=0; i<nSides; i++ )
    {
      int offset = 3 * ( j * nSides + i ) ;
      cphi = (float)cos ( phi ) ;
      sphi = (float)sin ( phi ) ;
      *(vertex + offset + 0) = cpsi * ( oradius + cphi * iradius ) ;
      *(vertex + offset + 1) = spsi * ( oradius + cphi * iradius ) ;
      *(vertex + offset + 2) =                    sphi * iradius   ;
      *(normal + offset + 0) = cpsi * cphi ;
      *(normal + offset + 1) = spsi * cphi ;
      *(normal + offset + 2) =        sphi ;
      phi += dphi;
    }

    psi += dpsi;
  }

    glBegin( GL_QUADS );
  for( i=0; i<nSides-1; i++ )
  {
    for( j=0; j<nRings-1; j++ )
    {
      int offset = 3 * ( j * nSides + i ) ;
      glNormal3fv( normal + offset );
      glVertex3fv( vertex + offset );
      glNormal3fv( normal + offset + 3 );
      glVertex3fv( vertex + offset + 3 );
      glNormal3fv( normal + offset + 3 * nSides + 3 );
      glVertex3fv( vertex + offset + 3 * nSides + 3 );
      glNormal3fv( normal + offset + 3 * nSides );
      glVertex3fv( vertex + offset + 3 * nSides );
    }
  }

  glEnd();

  free ( vertex ) ;
  free ( normal ) ;
  glPopMatrix();
}

/*
 *
 */
void FGAPIENTRY glutWireDodecahedron( void )
{
  /* Magic Numbers:  It is possible to create a dodecahedron by attaching two pentagons to each face of
   * of a cube.  The coordinates of the points are:
   *   (+-x,0, z); (+-1, 1, 1); (0, z, x )
   * where x = 0.618033989 and z = 1.618033939.
   */
  glBegin ( GL_LINE_LOOP ) ;
  glNormal3f (  0.000000f,  0.525731f,  0.850651f ) ; glVertex3f (  0.000000f,  1.618034f,  0.618034f ) ; glVertex3f ( -1.0f,  1.0f,  1.0f ) ; glVertex3f ( -0.618034f, 0.000000f,  1.618034f ) ; glVertex3f (  0.618034f, 0.000000f,  1.618034f ) ; glVertex3f (  1.0f,  1.0f,  1.0f ) ;
  glEnd () ;
  glBegin ( GL_LINE_LOOP ) ;
  glNormal3f (  0.000000f,  0.525731f, -0.850651f ) ; glVertex3f (  0.000000f,  1.618034f, -0.618034f ) ; glVertex3f (  1.0f,  1.0f, -1.0f ) ; glVertex3f (  0.618034f, 0.000000f, -1.618034f ) ; glVertex3f ( -0.618034f, 0.000000f, -1.618034f ) ; glVertex3f ( -1.0f,  1.0f, -1.0f ) ;
  glEnd () ;
  glBegin ( GL_LINE_LOOP ) ;
  glNormal3f (  0.000000f, -0.525731f,  0.850651f ) ; glVertex3f (  0.000000f, -1.618034f,  0.618034f ) ; glVertex3f (  1.0f, -1.0f,  1.0f ) ; glVertex3f (  0.618034f, 0.000000f,  1.618034f ) ; glVertex3f ( -0.618034f, 0.000000f,  1.618034f ) ; glVertex3f ( -1.0f, -1.0f,  1.0f ) ;
  glEnd () ;
  glBegin ( GL_LINE_LOOP ) ;
  glNormal3f (  0.000000f, -0.525731f, -0.850651f ) ; glVertex3f (  0.000000f, -1.618034f, -0.618034f ) ; glVertex3f ( -1.0f, -1.0f, -1.0f ) ; glVertex3f ( -0.618034f, 0.000000f, -1.618034f ) ; glVertex3f (  0.618034f, 0.000000f, -1.618034f ) ; glVertex3f (  1.0f, -1.0f, -1.0f ) ;
  glEnd () ;

  glBegin ( GL_LINE_LOOP ) ;
  glNormal3f (  0.850651f,  0.000000f,  0.525731f ) ; glVertex3f (  0.618034f,  0.000000f,  1.618034f ) ; glVertex3f (  1.0f, -1.0f,  1.0f ) ; glVertex3f (  1.618034f, -0.618034f, 0.000000f ) ; glVertex3f (  1.618034f,  0.618034f, 0.000000f ) ; glVertex3f (  1.0f,  1.0f,  1.0f ) ;
  glEnd () ;
  glBegin ( GL_LINE_LOOP ) ;
  glNormal3f ( -0.850651f,  0.000000f,  0.525731f ) ; glVertex3f ( -0.618034f,  0.000000f,  1.618034f ) ; glVertex3f ( -1.0f,  1.0f,  1.0f ) ; glVertex3f ( -1.618034f,  0.618034f, 0.000000f ) ; glVertex3f ( -1.618034f, -0.618034f, 0.000000f ) ; glVertex3f ( -1.0f, -1.0f,  1.0f ) ;
  glEnd () ;
  glBegin ( GL_LINE_LOOP ) ;
  glNormal3f (  0.850651f,  0.000000f, -0.525731f ) ; glVertex3f (  0.618034f,  0.000000f, -1.618034f ) ; glVertex3f (  1.0f,  1.0f, -1.0f ) ; glVertex3f (  1.618034f,  0.618034f, 0.000000f ) ; glVertex3f (  1.618034f, -0.618034f, 0.000000f ) ; glVertex3f (  1.0f, -1.0f, -1.0f ) ;
  glEnd () ;
  glBegin ( GL_LINE_LOOP ) ;
  glNormal3f ( -0.850651f,  0.000000f, -0.525731f ) ; glVertex3f ( -0.618034f,  0.000000f, -1.618034f ) ; glVertex3f ( -1.0f, -1.0f, -1.0f ) ; glVertex3f ( -1.618034f, -0.618034f, 0.000000f ) ; glVertex3f ( -1.618034f,  0.618034f, 0.000000f ) ; glVertex3f ( -1.0f,  1.0f, -1.0f ) ;
  glEnd () ;

  glBegin ( GL_LINE_LOOP ) ;
  glNormal3f (  0.525731f,  0.850651f,  0.000000f ) ; glVertex3f (  1.618034f,  0.618034f,  0.000000f ) ; glVertex3f (  1.0f,  1.0f, -1.0f ) ; glVertex3f ( 0.000000f,  1.618034f, -0.618034f ) ; glVertex3f ( 0.000000f,  1.618034f,  0.618034f ) ; glVertex3f (  1.0f,  1.0f,  1.0f ) ;
  glEnd () ;
  glBegin ( GL_LINE_LOOP ) ;
  glNormal3f (  0.525731f, -0.850651f,  0.000000f ) ; glVertex3f (  1.618034f, -0.618034f,  0.000000f ) ; glVertex3f (  1.0f, -1.0f,  1.0f ) ; glVertex3f ( 0.000000f, -1.618034f,  0.618034f ) ; glVertex3f ( 0.000000f, -1.618034f, -0.618034f ) ; glVertex3f (  1.0f, -1.0f, -1.0f ) ;
  glEnd () ;
  glBegin ( GL_LINE_LOOP ) ;
  glNormal3f ( -0.525731f,  0.850651f,  0.000000f ) ; glVertex3f ( -1.618034f,  0.618034f,  0.000000f ) ; glVertex3f ( -1.0f,  1.0f,  1.0f ) ; glVertex3f ( 0.000000f,  1.618034f,  0.618034f ) ; glVertex3f ( 0.000000f,  1.618034f, -0.618034f ) ; glVertex3f ( -1.0f,  1.0f, -1.0f ) ;
  glEnd () ;
  glBegin ( GL_LINE_LOOP ) ;
  glNormal3f ( -0.525731f, -0.850651f,  0.000000f ) ; glVertex3f ( -1.618034f, -0.618034f,  0.000000f ) ; glVertex3f ( -1.0f, -1.0f, -1.0f ) ; glVertex3f ( 0.000000f, -1.618034f, -0.618034f ) ; glVertex3f ( 0.000000f, -1.618034f,  0.618034f ) ; glVertex3f ( -1.0f, -1.0f,  1.0f ) ;
  glEnd () ;
}

/*
 *
 */
void FGAPIENTRY glutSolidDodecahedron( void )
{
  /* Magic Numbers:  It is possible to create a dodecahedron by attaching two pentagons to each face of
   * of a cube.  The coordinates of the points are:
   *   (+-x,0, z); (+-1, 1, 1); (0, z, x )
   * where x = 0.618033989 and z = 1.618033939.
   */
  glBegin ( GL_POLYGON ) ;
  glNormal3f (  0.000000f,  0.525731f,  0.850651f ) ; glVertex3f (  0.000000f,  1.618034f,  0.618034f ) ; glVertex3f ( -1.0f,  1.0f,  1.0f ) ; glVertex3f ( -0.618034f, 0.000000f,  1.618034f ) ; glVertex3f (  0.618034f, 0.000000f,  1.618034f ) ; glVertex3f (  1.0f,  1.0f,  1.0f ) ;
  glEnd () ;
  glBegin ( GL_POLYGON ) ;
  glNormal3f (  0.000000f,  0.525731f, -0.850651f ) ; glVertex3f (  0.000000f,  1.618034f, -0.618034f ) ; glVertex3f (  1.0f,  1.0f, -1.0f ) ; glVertex3f (  0.618034f, 0.000000f, -1.618034f ) ; glVertex3f ( -0.618034f, 0.000000f, -1.618034f ) ; glVertex3f ( -1.0f,  1.0f, -1.0f ) ;
  glEnd () ;
  glBegin ( GL_POLYGON ) ;
  glNormal3f (  0.000000f, -0.525731f,  0.850651f ) ; glVertex3f (  0.000000f, -1.618034f,  0.618034f ) ; glVertex3f (  1.0f, -1.0f,  1.0f ) ; glVertex3f (  0.618034f, 0.000000f,  1.618034f ) ; glVertex3f ( -0.618034f, 0.000000f,  1.618034f ) ; glVertex3f ( -1.0f, -1.0f,  1.0f ) ;
  glEnd () ;
  glBegin ( GL_POLYGON ) ;
  glNormal3f (  0.000000f, -0.525731f, -0.850651f ) ; glVertex3f (  0.000000f, -1.618034f, -0.618034f ) ; glVertex3f ( -1.0f, -1.0f, -1.0f ) ; glVertex3f ( -0.618034f, 0.000000f, -1.618034f ) ; glVertex3f (  0.618034f, 0.000000f, -1.618034f ) ; glVertex3f (  1.0f, -1.0f, -1.0f ) ;
  glEnd () ;

  glBegin ( GL_POLYGON ) ;
  glNormal3f (  0.850651f,  0.000000f,  0.525731f ) ; glVertex3f (  0.618034f,  0.000000f,  1.618034f ) ; glVertex3f (  1.0f, -1.0f,  1.0f ) ; glVertex3f (  1.618034f, -0.618034f, 0.000000f ) ; glVertex3f (  1.618034f,  0.618034f, 0.000000f ) ; glVertex3f (  1.0f,  1.0f,  1.0f ) ;
  glEnd () ;
  glBegin ( GL_POLYGON ) ;
  glNormal3f ( -0.850651f,  0.000000f,  0.525731f ) ; glVertex3f ( -0.618034f,  0.000000f,  1.618034f ) ; glVertex3f ( -1.0f,  1.0f,  1.0f ) ; glVertex3f ( -1.618034f,  0.618034f, 0.000000f ) ; glVertex3f ( -1.618034f, -0.618034f, 0.000000f ) ; glVertex3f ( -1.0f, -1.0f,  1.0f ) ;
  glEnd () ;
  glBegin ( GL_POLYGON ) ;
  glNormal3f (  0.850651f,  0.000000f, -0.525731f ) ; glVertex3f (  0.618034f,  0.000000f, -1.618034f ) ; glVertex3f (  1.0f,  1.0f, -1.0f ) ; glVertex3f (  1.618034f,  0.618034f, 0.000000f ) ; glVertex3f (  1.618034f, -0.618034f, 0.000000f ) ; glVertex3f (  1.0f, -1.0f, -1.0f ) ;
  glEnd () ;
  glBegin ( GL_POLYGON ) ;
  glNormal3f ( -0.850651f,  0.000000f, -0.525731f ) ; glVertex3f ( -0.618034f,  0.000000f, -1.618034f ) ; glVertex3f ( -1.0f, -1.0f, -1.0f ) ; glVertex3f ( -1.618034f, -0.618034f, 0.000000f ) ; glVertex3f ( -1.618034f,  0.618034f, 0.000000f ) ; glVertex3f ( -1.0f,  1.0f, -1.0f ) ;
  glEnd () ;

  glBegin ( GL_POLYGON ) ;
  glNormal3f (  0.525731f,  0.850651f,  0.000000f ) ; glVertex3f (  1.618034f,  0.618034f,  0.000000f ) ; glVertex3f (  1.0f,  1.0f, -1.0f ) ; glVertex3f ( 0.000000f,  1.618034f, -0.618034f ) ; glVertex3f ( 0.000000f,  1.618034f,  0.618034f ) ; glVertex3f (  1.0f,  1.0f,  1.0f ) ;
  glEnd () ;
  glBegin ( GL_POLYGON ) ;
  glNormal3f (  0.525731f, -0.850651f,  0.000000f ) ; glVertex3f (  1.618034f, -0.618034f,  0.000000f ) ; glVertex3f (  1.0f, -1.0f,  1.0f ) ; glVertex3f ( 0.000000f, -1.618034f,  0.618034f ) ; glVertex3f ( 0.000000f, -1.618034f, -0.618034f ) ; glVertex3f (  1.0f, -1.0f, -1.0f ) ;
  glEnd () ;
  glBegin ( GL_POLYGON ) ;
  glNormal3f ( -0.525731f,  0.850651f,  0.000000f ) ; glVertex3f ( -1.618034f,  0.618034f,  0.000000f ) ; glVertex3f ( -1.0f,  1.0f,  1.0f ) ; glVertex3f ( 0.000000f,  1.618034f,  0.618034f ) ; glVertex3f ( 0.000000f,  1.618034f, -0.618034f ) ; glVertex3f ( -1.0f,  1.0f, -1.0f ) ;
  glEnd () ;
  glBegin ( GL_POLYGON ) ;
  glNormal3f ( -0.525731f, -0.850651f,  0.000000f ) ; glVertex3f ( -1.618034f, -0.618034f,  0.000000f ) ; glVertex3f ( -1.0f, -1.0f, -1.0f ) ; glVertex3f ( 0.000000f, -1.618034f, -0.618034f ) ; glVertex3f ( 0.000000f, -1.618034f,  0.618034f ) ; glVertex3f ( -1.0f, -1.0f,  1.0f ) ;
  glEnd () ;
}

/*
 *
 */
void FGAPIENTRY glutWireOctahedron( void )
{
#define RADIUS    1.0f
  glBegin( GL_LINE_LOOP );
    glNormal3f( 0.577350f, 0.577350f, 0.577350f); glVertex3f( RADIUS, 0.0f, 0.0f ); glVertex3f( 0.0f, RADIUS, 0.0f ); glVertex3f( 0.0f, 0.0f, RADIUS );
    glNormal3f( 0.577350f, 0.577350f,-0.577350f); glVertex3f( RADIUS, 0.0f, 0.0f ); glVertex3f( 0.0f, RADIUS, 0.0f ); glVertex3f( 0.0f, 0.0f,-RADIUS );
    glNormal3f( 0.577350f,-0.577350f, 0.577350f); glVertex3f( RADIUS, 0.0f, 0.0f ); glVertex3f( 0.0f,-RADIUS, 0.0f ); glVertex3f( 0.0f, 0.0f, RADIUS );
    glNormal3f( 0.577350f,-0.577350f,-0.577350f); glVertex3f( RADIUS, 0.0f, 0.0f ); glVertex3f( 0.0f,-RADIUS, 0.0f ); glVertex3f( 0.0f, 0.0f,-RADIUS );
    glNormal3f(-0.577350f, 0.577350f, 0.577350f); glVertex3f(-RADIUS, 0.0f, 0.0f ); glVertex3f( 0.0f, RADIUS, 0.0f ); glVertex3f( 0.0f, 0.0f, RADIUS );
    glNormal3f(-0.577350f, 0.577350f,-0.577350f); glVertex3f(-RADIUS, 0.0f, 0.0f ); glVertex3f( 0.0f, RADIUS, 0.0f ); glVertex3f( 0.0f, 0.0f,-RADIUS );
    glNormal3f(-0.577350f,-0.577350f, 0.577350f); glVertex3f(-RADIUS, 0.0f, 0.0f ); glVertex3f( 0.0f,-RADIUS, 0.0f ); glVertex3f( 0.0f, 0.0f, RADIUS );
    glNormal3f(-0.577350f,-0.577350f,-0.577350f); glVertex3f(-RADIUS, 0.0f, 0.0f ); glVertex3f( 0.0f,-RADIUS, 0.0f ); glVertex3f( 0.0f, 0.0f,-RADIUS );
  glEnd();
#undef RADIUS
}

/*
 *
 */
void FGAPIENTRY glutSolidOctahedron( void )
{
#define RADIUS    1.0f
  glBegin( GL_TRIANGLES );
    glNormal3f( 0.577350f, 0.577350f, 0.577350f); glVertex3f( RADIUS, 0.0f, 0.0f ); glVertex3f( 0.0f, RADIUS, 0.0f ); glVertex3f( 0.0f, 0.0f, RADIUS );
    glNormal3f( 0.577350f, 0.577350f,-0.577350f); glVertex3f( RADIUS, 0.0f, 0.0f ); glVertex3f( 0.0f, RADIUS, 0.0f ); glVertex3f( 0.0f, 0.0f,-RADIUS );
    glNormal3f( 0.577350f,-0.577350f, 0.577350f); glVertex3f( RADIUS, 0.0f, 0.0f ); glVertex3f( 0.0f,-RADIUS, 0.0f ); glVertex3f( 0.0f, 0.0f, RADIUS );
    glNormal3f( 0.577350f,-0.577350f,-0.577350f); glVertex3f( RADIUS, 0.0f, 0.0f ); glVertex3f( 0.0f,-RADIUS, 0.0f ); glVertex3f( 0.0f, 0.0f,-RADIUS );
    glNormal3f(-0.577350f, 0.577350f, 0.577350f); glVertex3f(-RADIUS, 0.0f, 0.0f ); glVertex3f( 0.0f, RADIUS, 0.0f ); glVertex3f( 0.0f, 0.0f, RADIUS );
    glNormal3f(-0.577350f, 0.577350f,-0.577350f); glVertex3f(-RADIUS, 0.0f, 0.0f ); glVertex3f( 0.0f, RADIUS, 0.0f ); glVertex3f( 0.0f, 0.0f,-RADIUS );
    glNormal3f(-0.577350f,-0.577350f, 0.577350f); glVertex3f(-RADIUS, 0.0f, 0.0f ); glVertex3f( 0.0f,-RADIUS, 0.0f ); glVertex3f( 0.0f, 0.0f, RADIUS );
    glNormal3f(-0.577350f,-0.577350f,-0.577350f); glVertex3f(-RADIUS, 0.0f, 0.0f ); glVertex3f( 0.0f,-RADIUS, 0.0f ); glVertex3f( 0.0f, 0.0f,-RADIUS );
  glEnd();
#undef RADIUS
}

/*
 *
 */
void FGAPIENTRY glutWireTetrahedron( void )
{
  /* Magic Numbers:  r0 = ( 1, 0, 0 )
   *                 r1 = ( -1/3, 2 sqrt(2) / 3, 0 )
   *                 r2 = ( -1/3, -sqrt(2) / 3, sqrt(6) / 3 )
   *                 r3 = ( -1/3, -sqrt(2) / 3, -sqrt(6) / 3 )
   * |r0| = |r1| = |r2| = |r3| = 1
   * Distance between any two points is 2 sqrt(6) / 3
   *
   * Normals:  The unit normals are simply the negative of the coordinates of the point not on the surface.
   */

  float r0[3] = {       1.0f,       0.0f,       0.0f } ;
  float r1[3] = { -0.333333f,  0.942809f,       0.0f } ;
  float r2[3] = { -0.333333f, -0.471405f,  0.816497f } ;
  float r3[3] = { -0.333333f, -0.471405f, -0.816497f } ;

  glBegin( GL_LINE_LOOP ) ;
    glNormal3f (     -1.0f,       0.0f,       0.0f ) ; glVertex3fv ( r1 ) ; glVertex3fv ( r3 ) ; glVertex3fv ( r2 ) ;
    glNormal3f ( 0.333333f, -0.942809f,       0.0f ) ; glVertex3fv ( r0 ) ; glVertex3fv ( r2 ) ; glVertex3fv ( r3 ) ;
    glNormal3f ( 0.333333f,  0.471405f, -0.816497f ) ; glVertex3fv ( r0 ) ; glVertex3fv ( r3 ) ; glVertex3fv ( r1 ) ;
    glNormal3f ( 0.333333f,  0.471405f,  0.816497f ) ; glVertex3fv ( r0 ) ; glVertex3fv ( r1 ) ; glVertex3fv ( r2 ) ;
  glEnd() ;
}

/*
 *
 */
void FGAPIENTRY glutSolidTetrahedron( void )
{
  /* Magic Numbers:  r0 = ( 1, 0, 0 )
   *                 r1 = ( -1/3, 2 sqrt(2) / 3, 0 )
   *                 r2 = ( -1/3, -sqrt(2) / 3, sqrt(6) / 3 )
   *                 r3 = ( -1/3, -sqrt(2) / 3, -sqrt(6) / 3 )
   * |r0| = |r1| = |r2| = |r3| = 1
   * Distance between any two points is 2 sqrt(6) / 3
   *
   * Normals:  The unit normals are simply the negative of the coordinates of the point not on the surface.
   */

  float r0[3] = {       1.0f,       0.0f,       0.0f } ;
  float r1[3] = { -0.333333f,  0.942809f,       0.0f } ;
  float r2[3] = { -0.333333f, -0.471405f,  0.816497f } ;
  float r3[3] = { -0.333333f, -0.471405f, -0.816497f } ;

  glBegin( GL_TRIANGLES ) ;
    glNormal3f (     -1.0f,       0.0f,       0.0f ) ; glVertex3fv ( r1 ) ; glVertex3fv ( r3 ) ; glVertex3fv ( r2 ) ;
    glNormal3f ( 0.333333f, -0.942809f,       0.0f ) ; glVertex3fv ( r0 ) ; glVertex3fv ( r2 ) ; glVertex3fv ( r3 ) ;
    glNormal3f ( 0.333333f,  0.471405f, -0.816497f ) ; glVertex3fv ( r0 ) ; glVertex3fv ( r3 ) ; glVertex3fv ( r1 ) ;
    glNormal3f ( 0.333333f,  0.471405f,  0.816497f ) ; glVertex3fv ( r0 ) ; glVertex3fv ( r1 ) ; glVertex3fv ( r2 ) ;
  glEnd() ;
}

/*
 *
 */
float icos_r[12][3] = { { 1.0f, 0.0f, 0.0f },
  {  0.447214f,  0.894427f, 0.0f }, {  0.447214f,  0.276393f, 0.850651f }, {  0.447214f, -0.723607f, 0.525731f }, {  0.447214f, -0.723607f, -0.525731f }, {  0.447214f,  0.276393f, -0.850651f },
  { -0.447214f, -0.894427f, 0.0f }, { -0.447214f, -0.276393f, 0.850651f }, { -0.447214f,  0.723607f, 0.525731f }, { -0.447214f,  0.723607f, -0.525731f }, { -0.447214f, -0.276393f, -0.850651f },
  { -1.0f, 0.0f, 0.0f } } ;
int icos_v [20][3] = { { 0, 1, 2 }, { 0, 2, 3 }, { 0, 3, 4 }, { 0, 4, 5 }, { 0, 5, 1 },
                       { 1, 8, 2 }, { 2, 7, 3 }, { 3, 6, 4 }, { 4, 10, 5 }, { 5, 9, 1 },
                       { 1, 9, 8 }, { 2, 8, 7 }, { 3, 7, 6 }, { 4, 6, 10 }, { 5, 10, 9 },
                       { 11, 9, 10 }, { 11, 8, 9 }, { 11, 7, 8 }, { 11, 6, 7 }, { 11, 10, 6 } } ;

void FGAPIENTRY glutWireIcosahedron( void )
{
  int i ;
  for ( i = 0; i < 20; i++ )
  {
    float normal[3] ;
    normal[0] = ( icos_r[icos_v[i][1]][1] - icos_r[icos_v[i][0]][1] ) * ( icos_r[icos_v[i][2]][2] - icos_r[icos_v[i][0]][2] ) - ( icos_r[icos_v[i][1]][2] - icos_r[icos_v[i][0]][2] ) * ( icos_r[icos_v[i][2]][1] - icos_r[icos_v[i][0]][1] ) ;
    normal[1] = ( icos_r[icos_v[i][1]][2] - icos_r[icos_v[i][0]][2] ) * ( icos_r[icos_v[i][2]][0] - icos_r[icos_v[i][0]][0] ) - ( icos_r[icos_v[i][1]][0] - icos_r[icos_v[i][0]][0] ) * ( icos_r[icos_v[i][2]][2] - icos_r[icos_v[i][0]][2] ) ;
    normal[2] = ( icos_r[icos_v[i][1]][0] - icos_r[icos_v[i][0]][0] ) * ( icos_r[icos_v[i][2]][1] - icos_r[icos_v[i][0]][1] ) - ( icos_r[icos_v[i][1]][1] - icos_r[icos_v[i][0]][1] ) * ( icos_r[icos_v[i][2]][0] - icos_r[icos_v[i][0]][0] ) ;
    glBegin ( GL_LINE_LOOP ) ;
      glNormal3fv ( normal ) ;
      glVertex3fv ( icos_r[icos_v[i][0]] ) ;
      glVertex3fv ( icos_r[icos_v[i][1]] ) ;
      glVertex3fv ( icos_r[icos_v[i][2]] ) ;
    glEnd () ;
  }
}

/*
 *
 */
void FGAPIENTRY glutSolidIcosahedron( void )
{
  int i ;

  glBegin ( GL_TRIANGLES ) ;
  for ( i = 0; i < 20; i++ )
  {
    float normal[3] ;
    normal[0] = ( icos_r[icos_v[i][1]][1] - icos_r[icos_v[i][0]][1] ) * ( icos_r[icos_v[i][2]][2] - icos_r[icos_v[i][0]][2] ) - ( icos_r[icos_v[i][1]][2] - icos_r[icos_v[i][0]][2] ) * ( icos_r[icos_v[i][2]][1] - icos_r[icos_v[i][0]][1] ) ;
    normal[1] = ( icos_r[icos_v[i][1]][2] - icos_r[icos_v[i][0]][2] ) * ( icos_r[icos_v[i][2]][0] - icos_r[icos_v[i][0]][0] ) - ( icos_r[icos_v[i][1]][0] - icos_r[icos_v[i][0]][0] ) * ( icos_r[icos_v[i][2]][2] - icos_r[icos_v[i][0]][2] ) ;
    normal[2] = ( icos_r[icos_v[i][1]][0] - icos_r[icos_v[i][0]][0] ) * ( icos_r[icos_v[i][2]][1] - icos_r[icos_v[i][0]][1] ) - ( icos_r[icos_v[i][1]][1] - icos_r[icos_v[i][0]][1] ) * ( icos_r[icos_v[i][2]][0] - icos_r[icos_v[i][0]][0] ) ;
      glNormal3fv ( normal ) ;
      glVertex3fv ( icos_r[icos_v[i][0]] ) ;
      glVertex3fv ( icos_r[icos_v[i][1]] ) ;
      glVertex3fv ( icos_r[icos_v[i][2]] ) ;
  }

  glEnd () ;
}

/*
 *
 */
float rdod_r[14][3] = { { 0.0f, 0.0f, 1.0f },
  {  0.707107f,  0.000000f,  0.5f }, {  0.000000f,  0.707107f,  0.5f }, { -0.707107f,  0.000000f,  0.5f }, {  0.000000f, -0.707107f,  0.5f },
  {  0.707107f,  0.707107f,  0.0f }, { -0.707107f,  0.707107f,  0.0f }, { -0.707107f, -0.707107f,  0.0f }, {  0.707107f, -0.707107f,  0.0f },
  {  0.707107f,  0.000000f, -0.5f }, {  0.000000f,  0.707107f, -0.5f }, { -0.707107f,  0.000000f, -0.5f }, {  0.000000f, -0.707107f, -0.5f },
  {  0.0f, 0.0f, -1.0f } } ;
int rdod_v [12][4] = { { 0,  1,  5,  2 }, { 0,  2,  6,  3 }, { 0,  3,  7,  4 }, { 0,  4,  8, 1 },
                       { 5, 10,  6,  2 }, { 6, 11,  7,  3 }, { 7, 12,  8,  4 }, { 8,  9,  5, 1 },
                       { 5,  9, 13, 10 }, { 6, 10, 13, 11 }, { 7, 11, 13, 12 }, { 8, 12, 13, 9 } } ;
float rdod_n[12][3] = {
  {  0.353553f,  0.353553f,  0.5f }, { -0.353553f,  0.353553f,  0.5f }, { -0.353553f, -0.353553f,  0.5f }, {  0.353553f, -0.353553f,  0.5f },
  {  0.000000f,  1.000000f,  0.0f }, { -1.000000f,  0.000000f,  0.0f }, {  0.000000f, -1.000000f,  0.0f }, {  1.000000f,  0.000000f,  0.0f },
  {  0.353553f,  0.353553f, -0.5f }, { -0.353553f,  0.353553f, -0.5f }, { -0.353553f, -0.353553f, -0.5f }, {  0.353553f, -0.353553f, -0.5f }
  } ;

void FGAPIENTRY glutWireRhombicDodecahedron( void )
{
  int i ;
  for ( i = 0; i < 12; i++ )
  {
    glBegin ( GL_LINE_LOOP ) ;
      glNormal3fv ( rdod_n[i] ) ;
      glVertex3fv ( rdod_r[rdod_v[i][0]] ) ;
      glVertex3fv ( rdod_r[rdod_v[i][1]] ) ;
      glVertex3fv ( rdod_r[rdod_v[i][2]] ) ;
      glVertex3fv ( rdod_r[rdod_v[i][3]] ) ;
    glEnd () ;
  }
}

/*
 *
 */
void FGAPIENTRY glutSolidRhombicDodecahedron( void )
{
  int i ;

  glBegin ( GL_QUADS ) ;
  for ( i = 0; i < 12; i++ )
  {
      glNormal3fv ( rdod_n[i] ) ;
      glVertex3fv ( rdod_r[rdod_v[i][0]] ) ;
      glVertex3fv ( rdod_r[rdod_v[i][1]] ) ;
      glVertex3fv ( rdod_r[rdod_v[i][2]] ) ;
      glVertex3fv ( rdod_r[rdod_v[i][3]] ) ;
  }

  glEnd () ;
}

#define NUM_FACES     4

static GLfloat tetrahedron_v[4][3] =  /* Vertices */
{
  { -0.5, -0.288675134, -0.144337567 },
  {  0.5, -0.288675134, -0.144337567 },
  {  0.0,  0.577350269, -0.144337567 },
  {  0.0,  0.0,          0.672159013 }
} ;

static GLint tetrahedron_i[4][3] =  /* Vertex indices */
{
  { 0, 1, 2 }, { 0, 2, 3 }, { 0, 3, 1 }, { 1, 3, 2 }
} ;

static GLfloat tetrahedron_n[4][3] =  /* Normals */
{
  {  0.0,          0.0,         -1.0 },
  { -0.816496581,  0.471404521,  0.333333333 },
  {  0.0,         -0.942809042,  0.333333333 },
  {  0.816496581,  0.471404521,  0.333333333 }
} ;

void FGAPIENTRY glutWireSierpinskiSponge ( int num_levels, GLfloat offset[3], GLfloat scale )
{
  int i, j ;

  if ( num_levels == 0 )
  {

    for ( i = 0 ; i < NUM_FACES ; i++ )
    {
      glBegin ( GL_LINE_LOOP ) ;
      glNormal3fv ( tetrahedron_n[i] ) ;
      for ( j = 0; j < 3; j++ )
      {
        float x = offset[0] + scale * tetrahedron_v[tetrahedron_i[i][j]][0] ;
        float y = offset[1] + scale * tetrahedron_v[tetrahedron_i[i][j]][1] ;
        float z = offset[2] + scale * tetrahedron_v[tetrahedron_i[i][j]][2] ;
        glVertex3f ( x, y, z ) ;
      }

      glEnd () ;
    }
  }
  else
  {
    GLfloat local_offset[3] ;  // Use a local variable to avoid buildup of roundoff errors
    num_levels -- ;
    scale /= 2.0 ;
    local_offset[0] = offset[0] + scale * tetrahedron_v[0][0] ;
    local_offset[1] = offset[1] + scale * tetrahedron_v[0][1] ;
    local_offset[2] = offset[2] + scale * tetrahedron_v[0][2] ;
    glutWireSierpinskiSponge ( num_levels, local_offset, scale ) ;
    local_offset[0] += scale ;
    glutWireSierpinskiSponge ( num_levels, local_offset, scale ) ;
    local_offset[0] -= 0.5         * scale ;
    local_offset[1] += 0.866025403 * scale ;
    glutWireSierpinskiSponge ( num_levels, local_offset, scale ) ;
    local_offset[1] -= 0.577350269 * scale ;
    local_offset[2] += 0.816496581 * scale ;
    glutWireSierpinskiSponge ( num_levels, local_offset, scale ) ;
  }
}

void FGAPIENTRY glutSolidSierpinskiSponge ( int num_levels, GLfloat offset[3], GLfloat scale )
{
  int i, j ;

  if ( num_levels == 0 )
  {
    glBegin ( GL_TRIANGLES ) ;

    for ( i = 0 ; i < NUM_FACES ; i++ )
    {
      glNormal3fv ( tetrahedron_n[i] ) ;
      for ( j = 0; j < 3; j++ )
      {
        float x = offset[0] + scale * tetrahedron_v[tetrahedron_i[i][j]][0] ;
        float y = offset[1] + scale * tetrahedron_v[tetrahedron_i[i][j]][1] ;
        float z = offset[2] + scale * tetrahedron_v[tetrahedron_i[i][j]][2] ;
        glVertex3f ( x, y, z ) ;
      }
    }

    glEnd () ;
  }
  else
  {
    GLfloat local_offset[3] ;  // Use a local variable to avoid buildup of roundoff errors
    num_levels -- ;
    scale /= 2.0 ;
    local_offset[0] = offset[0] + scale * tetrahedron_v[0][0] ;
    local_offset[1] = offset[1] + scale * tetrahedron_v[0][1] ;
    local_offset[2] = offset[2] + scale * tetrahedron_v[0][2] ;
    glutSolidSierpinskiSponge ( num_levels, local_offset, scale ) ;
    local_offset[0] += scale ;
    glutSolidSierpinskiSponge ( num_levels, local_offset, scale ) ;
    local_offset[0] -= 0.5         * scale ;
    local_offset[1] += 0.866025403 * scale ;
    glutSolidSierpinskiSponge ( num_levels, local_offset, scale ) ;
    local_offset[1] -= 0.577350269 * scale ;
    local_offset[2] += 0.816496581 * scale ;
    glutSolidSierpinskiSponge ( num_levels, local_offset, scale ) ;
  }
}

#undef NUM_FACES

/*** END OF FILE ***/
