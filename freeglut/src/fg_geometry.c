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

#include <GL/freeglut.h>
#include "fg_internal.h"

/*
 * TODO BEFORE THE STABLE RELEASE:
 *
 * See fghTetrahedron
 *
 * Following functions have been contributed by Andreas Umbach.
 *
 *      glutWireCube()          -- looks OK
 *      glutSolidCube()         -- OK
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
 *
 *  The Following functions have been updated by Nigel Stewart, based
 *  on FreeGLUT 2.0.0 implementations:
 *
 *      glutWireSphere()        -- looks OK
 *      glutSolidSphere()       -- looks OK
 *      glutWireCone()          -- looks OK
 *      glutSolidCone()         -- looks OK
 */


/* General function for drawing geometry. As for all geometry we have no 
 * redundancy (or hardly any in the case of cones and cylinders) in terms
 * of the vertex/normal combinations, we just use glDrawArrays.
 * useWireMode controls the drawing of solids (false) or wire frame
 * versions (TRUE) of the geometry you pass
 */
static void fghDrawGeometry(GLenum vertexMode, double* vertices, double* normals, GLsizei numVertices, GLboolean useWireMode)
{
    if (useWireMode)
    {
        glPushAttrib(GL_POLYGON_BIT);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);

    glVertexPointer(3, GL_DOUBLE, 0, vertices);
    glNormalPointer(GL_DOUBLE, 0, normals);
    glDrawArrays(vertexMode,0,numVertices);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);

    if (useWireMode)
    {
        glPopAttrib();
    }
}


/* -- INTERNAL SETUP OF GEOMETRY --------------------------------------- */
static unsigned int ipow (int x, unsigned int y)
{
    return y==0? 1: y==1? x: (y%2? x: 1) * ipow(x*x, y/2);
}

/* -- stuff that can be cached -- */
/*
 * In general, we build arrays with all vertices or normals.
 * We cant compress this and use glDrawElements as all combinations of
 * vertex and normals are unique.
 */

/* -- Cube -- */
#define CUBE_NUM_VERT           8
#define CUBE_NUM_FACES          6
#define CUBE_NUM_VERT_PER_FACE  4
#define CUBE_VERT_PER_TETR      CUBE_NUM_FACES*CUBE_NUM_VERT_PER_FACE
#define CUBE_VERT_ELEM_PER_TETR CUBE_VERT_PER_TETR*3
/* Vertex Coordinates */
static GLdouble cube_v[CUBE_NUM_VERT][3] =
{
    { .5, .5, .5},
    {-.5, .5, .5},
    {-.5,-.5, .5},
    { .5,-.5, .5},
    { .5,-.5,-.5},
    { .5, .5,-.5},
    {-.5, .5,-.5},
    {-.5,-.5,-.5}
};
/* Normal Vectors */
static GLdouble cube_n[CUBE_NUM_FACES][3] =
{
    { 1.0, 0.0, 0.0},
    { 0.0, 1.0, 0.0},
    { 0.0, 0.0, 1.0},
    {-1.0, 0.0, 0.0},
    { 0.0,-1.0, 0.0},
    { 0.0, 0.0,-1.0}
};

/* Vertex indices */
static GLubyte cube_vi[CUBE_NUM_FACES][CUBE_NUM_VERT_PER_FACE] =
{
    {0,1,2,3}, {0,3,4,5}, {0,5,6,1}, {1,6,7,2}, {7,4,3,2}, {4,7,6,5}
};

/* Cache of input to glDrawArrays */
static GLboolean cubeCached = FALSE;
static double cube_verts[CUBE_VERT_ELEM_PER_TETR];
static double cube_norms[CUBE_VERT_ELEM_PER_TETR];

static void fghCubeGenerate()
{   
    int i,j;
    for (i=0; i<CUBE_NUM_FACES; i++)
    {
        for (j=0; j<CUBE_NUM_VERT_PER_FACE; j++)
        {
            int idx = i*CUBE_NUM_VERT_PER_FACE*3+j*3;
            cube_verts[idx  ] = cube_v[cube_vi[i][j]][0];
            cube_verts[idx+1] = cube_v[cube_vi[i][j]][1];
            cube_verts[idx+2] = cube_v[cube_vi[i][j]][2];

            cube_norms[idx  ] = cube_n[i][0];
            cube_norms[idx+1] = cube_n[i][1];
            cube_norms[idx+2] = cube_n[i][2];
        }
    }
}

/* -- Tetrahedron -- */
/* Magic Numbers:  r0 = ( 1, 0, 0 )
 *                 r1 = ( -1/3, 2 sqrt(2) / 3, 0 )
 *                 r2 = ( -1/3, - sqrt(2) / 3,  sqrt(6) / 3 )
 *                 r3 = ( -1/3, - sqrt(2) / 3, -sqrt(6) / 3 )
 * |r0| = |r1| = |r2| = |r3| = 1
 * Distance between any two points is 2 sqrt(6) / 3
 *
 * Normals:  The unit normals are simply the negative of the coordinates of the point not on the surface.
 */
#define TETR_NUM_VERT           4
#define TETR_NUM_FACES          4
#define TETR_NUM_VERT_PER_FACE  3
#define TETR_VERT_PER_TETR      TETR_NUM_FACES*TETR_NUM_VERT_PER_FACE
#define TETR_VERT_ELEM_PER_TETR TETR_VERT_PER_TETR*3

/* Vertex Coordinates */
static GLdouble tet_r[TETR_NUM_VERT][TETR_NUM_VERT_PER_FACE] =
{
    {             1.0,             0.0,             0.0 },
    { -0.333333333333,  0.942809041582,             0.0 },
    { -0.333333333333, -0.471404520791,  0.816496580928 },
    { -0.333333333333, -0.471404520791, -0.816496580928 }
};
/* Normal Vectors */
static GLdouble tet_n[CUBE_NUM_FACES][3] =
{
    { -           1.0,             0.0,             0.0 },
    {  0.333333333333, -0.942809041582,             0.0 },
    {  0.333333333333,  0.471404520791, -0.816496580928 },
    {  0.333333333333,  0.471404520791,  0.816496580928 }
};

/* Vertex indices */
static GLubyte tet_i[TETR_NUM_FACES][TETR_NUM_VERT_PER_FACE] =
{
    { 1, 3, 2 }, { 0, 2, 3 }, { 0, 3, 1 }, { 0, 1, 2 }
};

/* Cache of input to glDrawArrays */
static GLboolean tetrCached = FALSE;
static double tetr_verts[TETR_VERT_ELEM_PER_TETR];
static double tetr_norms[TETR_VERT_ELEM_PER_TETR];

static void fghTetrahedronGenerate()
{
    int i,j;
    /*
     * Build array with vertices from vertex coordinates and vertex indices 
     * Do same for normals.
     * Need to do this because of different normals at shared vertices
     * (and because normals' coordinates need to be negated).
     */
    for (i=0; i<TETR_NUM_FACES; i++)
    {
        for (j=0; j<TETR_NUM_VERT_PER_FACE; j++)
        {
            int idx = i*TETR_NUM_VERT_PER_FACE*3+j*3;
            tetr_verts[idx  ] =  tet_r[tet_i[i][j]][0];
            tetr_verts[idx+1] =  tet_r[tet_i[i][j]][1];
            tetr_verts[idx+2] =  tet_r[tet_i[i][j]][2];

            tetr_norms[idx  ] =  tet_n[i][0];
            tetr_norms[idx+1] =  tet_n[i][1];
            tetr_norms[idx+2] =  tet_n[i][2];
        }
    }
}

/* -- Sierpinski Sponge -- */
static void fghSierpinskiSpongeGenerate ( int numLevels, GLdouble offset[3], GLdouble scale, double* vertices, double* normals )
{
    int i, j;
    if ( numLevels == 0 )
    {
        for ( i = 0 ; i < TETR_NUM_FACES ; i++ )
        {
            for ( j = 0; j < TETR_NUM_VERT_PER_FACE; j++ )
            {
                int idx = i*TETR_NUM_VERT_PER_FACE*3+j*3;
                vertices[idx  ] =  offset[0] + scale * tet_r[tet_i[i][j]][0];
                vertices[idx+1] =  offset[1] + scale * tet_r[tet_i[i][j]][1];
                vertices[idx+2] =  offset[2] + scale * tet_r[tet_i[i][j]][2];

                normals [idx  ] = -tet_r[i][0];
                normals [idx+1] = -tet_r[i][1];
                normals [idx+2] = -tet_r[i][2];
            }
        }
    }
    else if ( numLevels > 0 )
    {
        GLdouble local_offset[3] ;  /* Use a local variable to avoid buildup of roundoff errors */
        unsigned int stride = ipow(4,--numLevels)*TETR_VERT_ELEM_PER_TETR;
        scale /= 2.0 ;
        for ( i = 0 ; i < TETR_NUM_FACES ; i++ )
        {
            local_offset[0] = offset[0] + scale * tet_r[i][0];
            local_offset[1] = offset[1] + scale * tet_r[i][1];
            local_offset[2] = offset[2] + scale * tet_r[i][2];
            fghSierpinskiSpongeGenerate ( numLevels, local_offset, scale, vertices+i*stride, normals+i*stride );
        }
    }
}

/* -- Now the various shapes involving circles -- */
/*
 * Compute lookup table of cos and sin values forming a cirle
 *
 * Notes:
 *    It is the responsibility of the caller to free these tables
 *    The size of the table is (n+1) to form a connected loop
 *    The last entry is exactly the same as the first
 *    The sign of n can be flipped to get the reverse loop
 */
static void fghCircleTable(double **sint,double **cost,const int n)
{
    int i;

    /* Table size, the sign of n flips the circle direction */

    const int size = abs(n);

    /* Determine the angle between samples */

    const double angle = 2*M_PI/(double)( ( n == 0 ) ? 1 : n );

    /* Allocate memory for n samples, plus duplicate of first entry at the end */

    *sint = (double *) calloc(sizeof(double), size+1);
    *cost = (double *) calloc(sizeof(double), size+1);

    /* Bail out if memory allocation fails, fgError never returns */

    if (!(*sint) || !(*cost))
    {
        free(*sint);
        free(*cost);
        fgError("Failed to allocate memory in fghCircleTable");
    }

    /* Compute cos and sin around the circle */

    (*sint)[0] = 0.0;
    (*cost)[0] = 1.0;

    for (i=1; i<size; i++)
    {
        (*sint)[i] = sin(angle*i);
        (*cost)[i] = cos(angle*i);
    }

    /* Last sample is duplicate of the first */

    (*sint)[size] = (*sint)[0];
    (*cost)[size] = (*cost)[0];
}


/* -- INTERNAL DRAWING functions to avoid code duplication ------------- */

static void fghCube( GLdouble dSize, GLboolean useWireMode )
{
    if (!cubeCached)
        fghCubeGenerate();

    if (dSize!=1.)
    {
        /* Need to build new */
        fghDrawGeometry(GL_QUADS,cube_verts,cube_norms,CUBE_VERT_PER_TETR,useWireMode);
    }
    else
        fghDrawGeometry(GL_QUADS,cube_verts,cube_norms,CUBE_VERT_PER_TETR,useWireMode);
}

static void fghTetrahedron( GLboolean useWireMode )
{
    if (!tetrCached)
        fghTetrahedronGenerate();

    fghDrawGeometry(GL_TRIANGLES,tetr_verts,tetr_norms,TETR_VERT_PER_TETR,useWireMode);
}

static void fghSierpinskiSponge ( int numLevels, GLdouble offset[3], GLdouble scale, GLboolean useWireMode )
{
    double      *vertices;
    double      * normals;
    unsigned int  numTetr = numLevels<0? 0 : ipow(4,numLevels); /* No sponge for numLevels below 0 */
    unsigned int  numVert = numTetr*TETR_VERT_PER_TETR;

    if (numTetr)
    {
        /* Allocate memory */
        vertices = malloc(numVert*3 * sizeof(double));
        normals  = malloc(numVert*3 * sizeof(double));

        /* Generate elements */
        fghSierpinskiSpongeGenerate ( numLevels, offset, scale, vertices, normals );

        /* Draw and cleanup */
        fghDrawGeometry(GL_TRIANGLES,vertices,normals,numVert,useWireMode);
        free(vertices);
        free(normals );
    }
}


/* -- INTERFACE FUNCTIONS ---------------------------------------------- */


/*
 * Draws a solid sphere
 */
void FGAPIENTRY glutSolidSphere(GLdouble radius, GLint slices, GLint stacks)
{
    int i,j;

    /* Adjust z and radius as stacks are drawn. */

    double z0,z1;
    double r0,r1;

    /* Pre-computed circle */

    double *sint1,*cost1;
    double *sint2,*cost2;

    FREEGLUT_EXIT_IF_NOT_INITIALISED ( "glutSolidSphere" );

    fghCircleTable(&sint1,&cost1,-slices);
    fghCircleTable(&sint2,&cost2,stacks*2);

    /* The top stack is covered with a triangle fan */

    z0 = 1.0;
    z1 = cost2[(stacks>0)?1:0];
    r0 = 0.0;
    r1 = sint2[(stacks>0)?1:0];

    glBegin(GL_TRIANGLE_FAN);

        glNormal3d(0,0,1);
        glVertex3d(0,0,radius);

        for (j=slices; j>=0; j--)
        {
            glNormal3d(cost1[j]*r1,        sint1[j]*r1,        z1       );
            glVertex3d(cost1[j]*r1*radius, sint1[j]*r1*radius, z1*radius);
        }

    glEnd();

    /* Cover each stack with a quad strip, except the top and bottom stacks */

    for( i=1; i<stacks-1; i++ )
    {
        z0 = z1; z1 = cost2[i+1];
        r0 = r1; r1 = sint2[i+1];

        glBegin(GL_QUAD_STRIP);

            for(j=0; j<=slices; j++)
            {
                glNormal3d(cost1[j]*r1,        sint1[j]*r1,        z1       );
                glVertex3d(cost1[j]*r1*radius, sint1[j]*r1*radius, z1*radius);
                glNormal3d(cost1[j]*r0,        sint1[j]*r0,        z0       );
                glVertex3d(cost1[j]*r0*radius, sint1[j]*r0*radius, z0*radius);
            }

        glEnd();
    }

    /* The bottom stack is covered with a triangle fan */

    z0 = z1;
    r0 = r1;

    glBegin(GL_TRIANGLE_FAN);

        glNormal3d(0,0,-1);
        glVertex3d(0,0,-radius);

        for (j=0; j<=slices; j++)
        {
            glNormal3d(cost1[j]*r0,        sint1[j]*r0,        z0       );
            glVertex3d(cost1[j]*r0*radius, sint1[j]*r0*radius, z0*radius);
        }

    glEnd();

    /* Release sin and cos tables */

    free(sint1);
    free(cost1);
    free(sint2);
    free(cost2);
}

/*
 * Draws a wire sphere
 */
void FGAPIENTRY glutWireSphere(GLdouble radius, GLint slices, GLint stacks)
{
    int i,j;

    /* Adjust z and radius as stacks and slices are drawn. */

    double r;
    double x,y,z;

    /* Pre-computed circle */

    double *sint1,*cost1;
    double *sint2,*cost2;

    FREEGLUT_EXIT_IF_NOT_INITIALISED ( "glutWireSphere" );

    fghCircleTable(&sint1,&cost1,-slices  );
    fghCircleTable(&sint2,&cost2, stacks*2);

    /* Draw a line loop for each stack */

    for (i=1; i<stacks; i++)
    {
        z = cost2[i];
        r = sint2[i];

        glBegin(GL_LINE_LOOP);

            for(j=0; j<=slices; j++)
            {
                x = cost1[j];
                y = sint1[j];

                glNormal3d(x,y,z);
                glVertex3d(x*r*radius,y*r*radius,z*radius);
            }

        glEnd();
    }

    /* Draw a line loop for each slice */

    for (i=0; i<slices; i++)
    {
        glBegin(GL_LINE_STRIP);

            for(j=0; j<=stacks; j++)
            {
                x = cost1[i]*sint2[j];
                y = sint1[i]*sint2[j];
                z = cost2[j];

                glNormal3d(x,y,z);
                glVertex3d(x*radius,y*radius,z*radius);
            }

        glEnd();
    }

    /* Release sin and cos tables */

    free(sint1);
    free(cost1);
    free(sint2);
    free(cost2);
}

/*
 * Draws a solid cone
 */
void FGAPIENTRY glutSolidCone( GLdouble base, GLdouble height, GLint slices, GLint stacks )
{
    int i,j;

    /* Step in z and radius as stacks are drawn. */

    double z0,z1;
    double r0,r1;

    const double zStep = height / ( ( stacks > 0 ) ? stacks : 1 );
    const double rStep = base / ( ( stacks > 0 ) ? stacks : 1 );

    /* Scaling factors for vertex normals */

    const double cosn = ( height / sqrt ( height * height + base * base ));
    const double sinn = ( base   / sqrt ( height * height + base * base ));

    /* Pre-computed circle */

    double *sint,*cost;

    FREEGLUT_EXIT_IF_NOT_INITIALISED ( "glutSolidCone" );

    fghCircleTable(&sint,&cost,-slices);

    /* Cover the circular base with a triangle fan... */

    z0 = 0.0;
    z1 = zStep;

    r0 = base;
    r1 = r0 - rStep;

    glBegin(GL_TRIANGLE_FAN);

        glNormal3d(0.0,0.0,-1.0);
        glVertex3d(0.0,0.0, z0 );

        for (j=0; j<=slices; j++)
            glVertex3d(cost[j]*r0, sint[j]*r0, z0);

    glEnd();

    /* Cover each stack with a quad strip, except the top stack */

    for( i=0; i<stacks-1; i++ )
    {
        glBegin(GL_QUAD_STRIP);

            for(j=0; j<=slices; j++)
            {
                glNormal3d(cost[j]*cosn, sint[j]*cosn, sinn);
                glVertex3d(cost[j]*r0,   sint[j]*r0,   z0  );
                glVertex3d(cost[j]*r1,   sint[j]*r1,   z1  );
            }

            z0 = z1; z1 += zStep;
            r0 = r1; r1 -= rStep;

        glEnd();
    }

    /* The top stack is covered with individual triangles */

    glBegin(GL_TRIANGLES);

        glNormal3d(cost[0]*sinn, sint[0]*sinn, cosn);

        for (j=0; j<slices; j++)
        {
            glVertex3d(cost[j+0]*r0,   sint[j+0]*r0,   z0    );
            glVertex3d(0,              0,              height);
            glNormal3d(cost[j+1]*sinn, sint[j+1]*sinn, cosn  );
            glVertex3d(cost[j+1]*r0,   sint[j+1]*r0,   z0    );
        }

    glEnd();

    /* Release sin and cos tables */

    free(sint);
    free(cost);
}

/*
 * Draws a wire cone
 */
void FGAPIENTRY glutWireCone( GLdouble base, GLdouble height, GLint slices, GLint stacks)
{
    int i,j;

    /* Step in z and radius as stacks are drawn. */

    double z = 0.0;
    double r = base;

    const double zStep = height / ( ( stacks > 0 ) ? stacks : 1 );
    const double rStep = base / ( ( stacks > 0 ) ? stacks : 1 );

    /* Scaling factors for vertex normals */

    const double cosn = ( height / sqrt ( height * height + base * base ));
    const double sinn = ( base   / sqrt ( height * height + base * base ));

    /* Pre-computed circle */

    double *sint,*cost;

    FREEGLUT_EXIT_IF_NOT_INITIALISED ( "glutWireCone" );

    fghCircleTable(&sint,&cost,-slices);

    /* Draw the stacks... */

    for (i=0; i<stacks; i++)
    {
        glBegin(GL_LINE_LOOP);

            for( j=0; j<slices; j++ )
            {
                glNormal3d(cost[j]*sinn, sint[j]*sinn, cosn);
                glVertex3d(cost[j]*r,    sint[j]*r,    z   );
            }

        glEnd();

        z += zStep;
        r -= rStep;
    }

    /* Draw the slices */

    r = base;

    glBegin(GL_LINES);

        for (j=0; j<slices; j++)
        {
            glNormal3d(cost[j]*sinn, sint[j]*sinn, cosn  );
            glVertex3d(cost[j]*r,    sint[j]*r,    0.0   );
            glVertex3d(0.0,          0.0,          height);
        }

    glEnd();

    /* Release sin and cos tables */

    free(sint);
    free(cost);
}


/*
 * Draws a solid cylinder
 */
void FGAPIENTRY glutSolidCylinder(GLdouble radius, GLdouble height, GLint slices, GLint stacks)
{
    int i,j;

    /* Step in z and radius as stacks are drawn. */

    double z0,z1;
    const double zStep = height / ( ( stacks > 0 ) ? stacks : 1 );

    /* Pre-computed circle */

    double *sint,*cost;

    FREEGLUT_EXIT_IF_NOT_INITIALISED ( "glutSolidCylinder" );

    fghCircleTable(&sint,&cost,-slices);

    /* Cover the base and top */

    glBegin(GL_TRIANGLE_FAN);
        glNormal3d(0.0, 0.0, -1.0 );
        glVertex3d(0.0, 0.0,  0.0 );
        for (j=0; j<=slices; j++)
          glVertex3d(cost[j]*radius, sint[j]*radius, 0.0);
    glEnd();

    glBegin(GL_TRIANGLE_FAN);
        glNormal3d(0.0, 0.0, 1.0   );
        glVertex3d(0.0, 0.0, height);
        for (j=slices; j>=0; j--)
          glVertex3d(cost[j]*radius, sint[j]*radius, height);
    glEnd();

    /* Do the stacks */

    z0 = 0.0;
    z1 = zStep;

    for (i=1; i<=stacks; i++)
    {
        if (i==stacks)
            z1 = height;

        glBegin(GL_QUAD_STRIP);
            for (j=0; j<=slices; j++ )
            {
                glNormal3d(cost[j],        sint[j],        0.0 );
                glVertex3d(cost[j]*radius, sint[j]*radius, z0  );
                glVertex3d(cost[j]*radius, sint[j]*radius, z1  );
            }
        glEnd();

        z0 = z1; z1 += zStep;
    }

    /* Release sin and cos tables */

    free(sint);
    free(cost);
}

/*
 * Draws a wire cylinder
 */
void FGAPIENTRY glutWireCylinder(GLdouble radius, GLdouble height, GLint slices, GLint stacks)
{
    int i,j;

    /* Step in z and radius as stacks are drawn. */

          double z = 0.0;
    const double zStep = height / ( ( stacks > 0 ) ? stacks : 1 );

    /* Pre-computed circle */

    double *sint,*cost;

    FREEGLUT_EXIT_IF_NOT_INITIALISED ( "glutWireCylinder" );

    fghCircleTable(&sint,&cost,-slices);

    /* Draw the stacks... */

    for (i=0; i<=stacks; i++)
    {
        if (i==stacks)
            z = height;

        glBegin(GL_LINE_LOOP);

            for( j=0; j<slices; j++ )
            {
                glNormal3d(cost[j],        sint[j],        0.0);
                glVertex3d(cost[j]*radius, sint[j]*radius, z  );
            }

        glEnd();

        z += zStep;
    }

    /* Draw the slices */

    glBegin(GL_LINES);

        for (j=0; j<slices; j++)
        {
            glNormal3d(cost[j],        sint[j],        0.0   );
            glVertex3d(cost[j]*radius, sint[j]*radius, 0.0   );
            glVertex3d(cost[j]*radius, sint[j]*radius, height);
        }

    glEnd();

    /* Release sin and cos tables */

    free(sint);
    free(cost);
}

/*
 * Draws a wire torus
 */
void FGAPIENTRY glutWireTorus( GLdouble dInnerRadius, GLdouble dOuterRadius, GLint nSides, GLint nRings )
{
  double  iradius = dInnerRadius, oradius = dOuterRadius, phi, psi, dpsi, dphi;
  double *vertex, *normal;
  int    i, j;
  double spsi, cpsi, sphi, cphi ;

  FREEGLUT_EXIT_IF_NOT_INITIALISED ( "glutWireTorus" );

  if ( nSides < 1 ) nSides = 1;
  if ( nRings < 1 ) nRings = 1;

  /* Allocate the vertices array */
  vertex = (double *)calloc( sizeof(double), 3 * nSides * nRings );
  normal = (double *)calloc( sizeof(double), 3 * nSides * nRings );

  glPushMatrix();

  dpsi =  2.0 * M_PI / (double)nRings ;
  dphi = -2.0 * M_PI / (double)nSides ;
  psi  = 0.0;

  for( j=0; j<nRings; j++ )
  {
    cpsi = cos ( psi ) ;
    spsi = sin ( psi ) ;
    phi = 0.0;

    for( i=0; i<nSides; i++ )
    {
      int offset = 3 * ( j * nSides + i ) ;
      cphi = cos ( phi ) ;
      sphi = sin ( phi ) ;
      *(vertex + offset + 0) = cpsi * ( oradius + cphi * iradius ) ;
      *(vertex + offset + 1) = spsi * ( oradius + cphi * iradius ) ;
      *(vertex + offset + 2) =                    sphi * iradius  ;
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
      glNormal3dv( normal + offset );
      glVertex3dv( vertex + offset );
    }

    glEnd();
  }

  for( j=0; j<nRings; j++ )
  {
    glBegin(GL_LINE_LOOP);

    for( i=0; i<nSides; i++ )
    {
      int offset = 3 * ( j * nSides + i ) ;
      glNormal3dv( normal + offset );
      glVertex3dv( vertex + offset );
    }

    glEnd();
  }

  free ( vertex ) ;
  free ( normal ) ;
  glPopMatrix();
}

/*
 * Draws a solid torus
 */
void FGAPIENTRY glutSolidTorus( GLdouble dInnerRadius, GLdouble dOuterRadius, GLint nSides, GLint nRings )
{
  double  iradius = dInnerRadius, oradius = dOuterRadius, phi, psi, dpsi, dphi;
  double *vertex, *normal;
  int    i, j;
  double spsi, cpsi, sphi, cphi ;

  FREEGLUT_EXIT_IF_NOT_INITIALISED ( "glutSolidTorus" );

  if ( nSides < 1 ) nSides = 1;
  if ( nRings < 1 ) nRings = 1;

  /* Increment the number of sides and rings to allow for one more point than surface */
  nSides ++ ;
  nRings ++ ;

  /* Allocate the vertices array */
  vertex = (double *)calloc( sizeof(double), 3 * nSides * nRings );
  normal = (double *)calloc( sizeof(double), 3 * nSides * nRings );

  glPushMatrix();

  dpsi =  2.0 * M_PI / (double)(nRings - 1) ;
  dphi = -2.0 * M_PI / (double)(nSides - 1) ;
  psi  = 0.0;

  for( j=0; j<nRings; j++ )
  {
    cpsi = cos ( psi ) ;
    spsi = sin ( psi ) ;
    phi = 0.0;

    for( i=0; i<nSides; i++ )
    {
      int offset = 3 * ( j * nSides + i ) ;
      cphi = cos ( phi ) ;
      sphi = sin ( phi ) ;
      *(vertex + offset + 0) = cpsi * ( oradius + cphi * iradius ) ;
      *(vertex + offset + 1) = spsi * ( oradius + cphi * iradius ) ;
      *(vertex + offset + 2) =                    sphi * iradius  ;
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
      glNormal3dv( normal + offset );
      glVertex3dv( vertex + offset );
      glNormal3dv( normal + offset + 3 );
      glVertex3dv( vertex + offset + 3 );
      glNormal3dv( normal + offset + 3 * nSides + 3 );
      glVertex3dv( vertex + offset + 3 * nSides + 3 );
      glNormal3dv( normal + offset + 3 * nSides );
      glVertex3dv( vertex + offset + 3 * nSides );
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
  FREEGLUT_EXIT_IF_NOT_INITIALISED ( "glutWireDodecahedron" );

  /* Magic Numbers:  It is possible to create a dodecahedron by attaching two pentagons to each face of
   * of a cube.  The coordinates of the points are:
   *   (+-x,0, z); (+-1, 1, 1); (0, z, x )
   * where x = (-1 + sqrt(5))/2, z = (1 + sqrt(5))/2  or
   *       x = 0.61803398875 and z = 1.61803398875.
   */
  glBegin ( GL_LINE_LOOP ) ;
  glNormal3d (  0.0,  0.525731112119,  0.850650808354 ) ; glVertex3d (  0.0,  1.61803398875,  0.61803398875 ) ; glVertex3d ( -1.0,  1.0,  1.0 ) ; glVertex3d ( -0.61803398875, 0.0,  1.61803398875 ) ; glVertex3d (  0.61803398875, 0.0,  1.61803398875 ) ; glVertex3d (  1.0,  1.0,  1.0 ) ;
  glEnd () ;
  glBegin ( GL_LINE_LOOP ) ;
  glNormal3d (  0.0,  0.525731112119, -0.850650808354 ) ; glVertex3d (  0.0,  1.61803398875, -0.61803398875 ) ; glVertex3d (  1.0,  1.0, -1.0 ) ; glVertex3d (  0.61803398875, 0.0, -1.61803398875 ) ; glVertex3d ( -0.61803398875, 0.0, -1.61803398875 ) ; glVertex3d ( -1.0,  1.0, -1.0 ) ;
  glEnd () ;
  glBegin ( GL_LINE_LOOP ) ;
  glNormal3d (  0.0, -0.525731112119,  0.850650808354 ) ; glVertex3d (  0.0, -1.61803398875,  0.61803398875 ) ; glVertex3d (  1.0, -1.0,  1.0 ) ; glVertex3d (  0.61803398875, 0.0,  1.61803398875 ) ; glVertex3d ( -0.61803398875, 0.0,  1.61803398875 ) ; glVertex3d ( -1.0, -1.0,  1.0 ) ;
  glEnd () ;
  glBegin ( GL_LINE_LOOP ) ;
  glNormal3d (  0.0, -0.525731112119, -0.850650808354 ) ; glVertex3d (  0.0, -1.61803398875, -0.61803398875 ) ; glVertex3d ( -1.0, -1.0, -1.0 ) ; glVertex3d ( -0.61803398875, 0.0, -1.61803398875 ) ; glVertex3d (  0.61803398875, 0.0, -1.61803398875 ) ; glVertex3d (  1.0, -1.0, -1.0 ) ;
  glEnd () ;

  glBegin ( GL_LINE_LOOP ) ;
  glNormal3d (  0.850650808354,  0.0,  0.525731112119 ) ; glVertex3d (  0.61803398875,  0.0,  1.61803398875 ) ; glVertex3d (  1.0, -1.0,  1.0 ) ; glVertex3d (  1.61803398875, -0.61803398875, 0.0 ) ; glVertex3d (  1.61803398875,  0.61803398875, 0.0 ) ; glVertex3d (  1.0,  1.0,  1.0 ) ;
  glEnd () ;
  glBegin ( GL_LINE_LOOP ) ;
  glNormal3d ( -0.850650808354,  0.0,  0.525731112119 ) ; glVertex3d ( -0.61803398875,  0.0,  1.61803398875 ) ; glVertex3d ( -1.0,  1.0,  1.0 ) ; glVertex3d ( -1.61803398875,  0.61803398875, 0.0 ) ; glVertex3d ( -1.61803398875, -0.61803398875, 0.0 ) ; glVertex3d ( -1.0, -1.0,  1.0 ) ;
  glEnd () ;
  glBegin ( GL_LINE_LOOP ) ;
  glNormal3d (  0.850650808354,  0.0, -0.525731112119 ) ; glVertex3d (  0.61803398875,  0.0, -1.61803398875 ) ; glVertex3d (  1.0,  1.0, -1.0 ) ; glVertex3d (  1.61803398875,  0.61803398875, 0.0 ) ; glVertex3d (  1.61803398875, -0.61803398875, 0.0 ) ; glVertex3d (  1.0, -1.0, -1.0 ) ;
  glEnd () ;
  glBegin ( GL_LINE_LOOP ) ;
  glNormal3d ( -0.850650808354,  0.0, -0.525731112119 ) ; glVertex3d ( -0.61803398875,  0.0, -1.61803398875 ) ; glVertex3d ( -1.0, -1.0, -1.0 ) ; glVertex3d ( -1.61803398875, -0.61803398875, 0.0 ) ; glVertex3d ( -1.61803398875,  0.61803398875, 0.0 ) ; glVertex3d ( -1.0,  1.0, -1.0 ) ;
  glEnd () ;

  glBegin ( GL_LINE_LOOP ) ;
  glNormal3d (  0.525731112119,  0.850650808354,  0.0 ) ; glVertex3d (  1.61803398875,  0.61803398875,  0.0 ) ; glVertex3d (  1.0,  1.0, -1.0 ) ; glVertex3d ( 0.0,  1.61803398875, -0.61803398875 ) ; glVertex3d ( 0.0,  1.61803398875,  0.61803398875 ) ; glVertex3d (  1.0,  1.0,  1.0 ) ;
  glEnd () ;
  glBegin ( GL_LINE_LOOP ) ;
  glNormal3d (  0.525731112119, -0.850650808354,  0.0 ) ; glVertex3d (  1.61803398875, -0.61803398875,  0.0 ) ; glVertex3d (  1.0, -1.0,  1.0 ) ; glVertex3d ( 0.0, -1.61803398875,  0.61803398875 ) ; glVertex3d ( 0.0, -1.61803398875, -0.61803398875 ) ; glVertex3d (  1.0, -1.0, -1.0 ) ;
  glEnd () ;
  glBegin ( GL_LINE_LOOP ) ;
  glNormal3d ( -0.525731112119,  0.850650808354,  0.0 ) ; glVertex3d ( -1.61803398875,  0.61803398875,  0.0 ) ; glVertex3d ( -1.0,  1.0,  1.0 ) ; glVertex3d ( 0.0,  1.61803398875,  0.61803398875 ) ; glVertex3d ( 0.0,  1.61803398875, -0.61803398875 ) ; glVertex3d ( -1.0,  1.0, -1.0 ) ;
  glEnd () ;
  glBegin ( GL_LINE_LOOP ) ;
  glNormal3d ( -0.525731112119, -0.850650808354,  0.0 ) ; glVertex3d ( -1.61803398875, -0.61803398875,  0.0 ) ; glVertex3d ( -1.0, -1.0, -1.0 ) ; glVertex3d ( 0.0, -1.61803398875, -0.61803398875 ) ; glVertex3d ( 0.0, -1.61803398875,  0.61803398875 ) ; glVertex3d ( -1.0, -1.0,  1.0 ) ;
  glEnd () ;
}

/*
 *
 */
void FGAPIENTRY glutSolidDodecahedron( void )
{
  FREEGLUT_EXIT_IF_NOT_INITIALISED ( "glutSolidDodecahedron" );

  /* Magic Numbers:  It is possible to create a dodecahedron by attaching two pentagons to each face of
   * of a cube.  The coordinates of the points are:
   *   (+-x,0, z); (+-1, 1, 1); (0, z, x )
   * where x = (-1 + sqrt(5))/2, z = (1 + sqrt(5))/2 or
   *       x = 0.61803398875 and z = 1.61803398875.
   */
  glBegin ( GL_POLYGON ) ;
  glNormal3d (  0.0,  0.525731112119,  0.850650808354 ) ; glVertex3d (  0.0,  1.61803398875,  0.61803398875 ) ; glVertex3d ( -1.0,  1.0,  1.0 ) ; glVertex3d ( -0.61803398875, 0.0,  1.61803398875 ) ; glVertex3d (  0.61803398875, 0.0,  1.61803398875 ) ; glVertex3d (  1.0,  1.0,  1.0 ) ;
  glEnd () ;
  glBegin ( GL_POLYGON ) ;
  glNormal3d (  0.0,  0.525731112119, -0.850650808354 ) ; glVertex3d (  0.0,  1.61803398875, -0.61803398875 ) ; glVertex3d (  1.0,  1.0, -1.0 ) ; glVertex3d (  0.61803398875, 0.0, -1.61803398875 ) ; glVertex3d ( -0.61803398875, 0.0, -1.61803398875 ) ; glVertex3d ( -1.0,  1.0, -1.0 ) ;
  glEnd () ;
  glBegin ( GL_POLYGON ) ;
  glNormal3d (  0.0, -0.525731112119,  0.850650808354 ) ; glVertex3d (  0.0, -1.61803398875,  0.61803398875 ) ; glVertex3d (  1.0, -1.0,  1.0 ) ; glVertex3d (  0.61803398875, 0.0,  1.61803398875 ) ; glVertex3d ( -0.61803398875, 0.0,  1.61803398875 ) ; glVertex3d ( -1.0, -1.0,  1.0 ) ;
  glEnd () ;
  glBegin ( GL_POLYGON ) ;
  glNormal3d (  0.0, -0.525731112119, -0.850650808354 ) ; glVertex3d (  0.0, -1.61803398875, -0.61803398875 ) ; glVertex3d ( -1.0, -1.0, -1.0 ) ; glVertex3d ( -0.61803398875, 0.0, -1.61803398875 ) ; glVertex3d (  0.61803398875, 0.0, -1.61803398875 ) ; glVertex3d (  1.0, -1.0, -1.0 ) ;
  glEnd () ;

  glBegin ( GL_POLYGON ) ;
  glNormal3d (  0.850650808354,  0.0,  0.525731112119 ) ; glVertex3d (  0.61803398875,  0.0,  1.61803398875 ) ; glVertex3d (  1.0, -1.0,  1.0 ) ; glVertex3d (  1.61803398875, -0.61803398875, 0.0 ) ; glVertex3d (  1.61803398875,  0.61803398875, 0.0 ) ; glVertex3d (  1.0,  1.0,  1.0 ) ;
  glEnd () ;
  glBegin ( GL_POLYGON ) ;
  glNormal3d ( -0.850650808354,  0.0,  0.525731112119 ) ; glVertex3d ( -0.61803398875,  0.0,  1.61803398875 ) ; glVertex3d ( -1.0,  1.0,  1.0 ) ; glVertex3d ( -1.61803398875,  0.61803398875, 0.0 ) ; glVertex3d ( -1.61803398875, -0.61803398875, 0.0 ) ; glVertex3d ( -1.0, -1.0,  1.0 ) ;
  glEnd () ;
  glBegin ( GL_POLYGON ) ;
  glNormal3d (  0.850650808354,  0.0, -0.525731112119 ) ; glVertex3d (  0.61803398875,  0.0, -1.61803398875 ) ; glVertex3d (  1.0,  1.0, -1.0 ) ; glVertex3d (  1.61803398875,  0.61803398875, 0.0 ) ; glVertex3d (  1.61803398875, -0.61803398875, 0.0 ) ; glVertex3d (  1.0, -1.0, -1.0 ) ;
  glEnd () ;
  glBegin ( GL_POLYGON ) ;
  glNormal3d ( -0.850650808354,  0.0, -0.525731112119 ) ; glVertex3d ( -0.61803398875,  0.0, -1.61803398875 ) ; glVertex3d ( -1.0, -1.0, -1.0 ) ; glVertex3d ( -1.61803398875, -0.61803398875, 0.0 ) ; glVertex3d ( -1.61803398875,  0.61803398875, 0.0 ) ; glVertex3d ( -1.0,  1.0, -1.0 ) ;
  glEnd () ;

  glBegin ( GL_POLYGON ) ;
  glNormal3d (  0.525731112119,  0.850650808354,  0.0 ) ; glVertex3d (  1.61803398875,  0.61803398875,  0.0 ) ; glVertex3d (  1.0,  1.0, -1.0 ) ; glVertex3d ( 0.0,  1.61803398875, -0.61803398875 ) ; glVertex3d ( 0.0,  1.61803398875,  0.61803398875 ) ; glVertex3d (  1.0,  1.0,  1.0 ) ;
  glEnd () ;
  glBegin ( GL_POLYGON ) ;
  glNormal3d (  0.525731112119, -0.850650808354,  0.0 ) ; glVertex3d (  1.61803398875, -0.61803398875,  0.0 ) ; glVertex3d (  1.0, -1.0,  1.0 ) ; glVertex3d ( 0.0, -1.61803398875,  0.61803398875 ) ; glVertex3d ( 0.0, -1.61803398875, -0.61803398875 ) ; glVertex3d (  1.0, -1.0, -1.0 ) ;
  glEnd () ;
  glBegin ( GL_POLYGON ) ;
  glNormal3d ( -0.525731112119,  0.850650808354,  0.0 ) ; glVertex3d ( -1.61803398875,  0.61803398875,  0.0 ) ; glVertex3d ( -1.0,  1.0,  1.0 ) ; glVertex3d ( 0.0,  1.61803398875,  0.61803398875 ) ; glVertex3d ( 0.0,  1.61803398875, -0.61803398875 ) ; glVertex3d ( -1.0,  1.0, -1.0 ) ;
  glEnd () ;
  glBegin ( GL_POLYGON ) ;
  glNormal3d ( -0.525731112119, -0.850650808354,  0.0 ) ; glVertex3d ( -1.61803398875, -0.61803398875,  0.0 ) ; glVertex3d ( -1.0, -1.0, -1.0 ) ; glVertex3d ( 0.0, -1.61803398875, -0.61803398875 ) ; glVertex3d ( 0.0, -1.61803398875,  0.61803398875 ) ; glVertex3d ( -1.0, -1.0,  1.0 ) ;
  glEnd () ;
}

/*
 *
 */
void FGAPIENTRY glutWireOctahedron( void )
{
  FREEGLUT_EXIT_IF_NOT_INITIALISED ( "glutWireOctahedron" );

#define RADIUS    1.0f
  glBegin( GL_LINE_LOOP );
    glNormal3d( 0.577350269189, 0.577350269189, 0.577350269189); glVertex3d( RADIUS, 0.0, 0.0 ); glVertex3d( 0.0, RADIUS, 0.0 ); glVertex3d( 0.0, 0.0, RADIUS );
    glNormal3d( 0.577350269189, 0.577350269189,-0.577350269189); glVertex3d( RADIUS, 0.0, 0.0 ); glVertex3d( 0.0, 0.0,-RADIUS ); glVertex3d( 0.0, RADIUS, 0.0 );
    glNormal3d( 0.577350269189,-0.577350269189, 0.577350269189); glVertex3d( RADIUS, 0.0, 0.0 ); glVertex3d( 0.0, 0.0, RADIUS ); glVertex3d( 0.0,-RADIUS, 0.0 );
    glNormal3d( 0.577350269189,-0.577350269189,-0.577350269189); glVertex3d( RADIUS, 0.0, 0.0 ); glVertex3d( 0.0,-RADIUS, 0.0 ); glVertex3d( 0.0, 0.0,-RADIUS );
    glNormal3d(-0.577350269189, 0.577350269189, 0.577350269189); glVertex3d(-RADIUS, 0.0, 0.0 ); glVertex3d( 0.0, 0.0, RADIUS ); glVertex3d( 0.0, RADIUS, 0.0 );
    glNormal3d(-0.577350269189, 0.577350269189,-0.577350269189); glVertex3d(-RADIUS, 0.0, 0.0 ); glVertex3d( 0.0, RADIUS, 0.0 ); glVertex3d( 0.0, 0.0,-RADIUS );
    glNormal3d(-0.577350269189,-0.577350269189, 0.577350269189); glVertex3d(-RADIUS, 0.0, 0.0 ); glVertex3d( 0.0,-RADIUS, 0.0 ); glVertex3d( 0.0, 0.0, RADIUS );
    glNormal3d(-0.577350269189,-0.577350269189,-0.577350269189); glVertex3d(-RADIUS, 0.0, 0.0 ); glVertex3d( 0.0, 0.0,-RADIUS ); glVertex3d( 0.0,-RADIUS, 0.0 );
  glEnd();
#undef RADIUS
}

/*
 *
 */
void FGAPIENTRY glutSolidOctahedron( void )
{
  FREEGLUT_EXIT_IF_NOT_INITIALISED ( "glutSolidOctahedron" );

#define RADIUS    1.0f
  glBegin( GL_TRIANGLES );
    glNormal3d( 0.577350269189, 0.577350269189, 0.577350269189); glVertex3d( RADIUS, 0.0, 0.0 ); glVertex3d( 0.0, RADIUS, 0.0 ); glVertex3d( 0.0, 0.0, RADIUS );
    glNormal3d( 0.577350269189, 0.577350269189,-0.577350269189); glVertex3d( RADIUS, 0.0, 0.0 ); glVertex3d( 0.0, 0.0,-RADIUS ); glVertex3d( 0.0, RADIUS, 0.0 );
    glNormal3d( 0.577350269189,-0.577350269189, 0.577350269189); glVertex3d( RADIUS, 0.0, 0.0 ); glVertex3d( 0.0, 0.0, RADIUS ); glVertex3d( 0.0,-RADIUS, 0.0 );
    glNormal3d( 0.577350269189,-0.577350269189,-0.577350269189); glVertex3d( RADIUS, 0.0, 0.0 ); glVertex3d( 0.0,-RADIUS, 0.0 ); glVertex3d( 0.0, 0.0,-RADIUS );
    glNormal3d(-0.577350269189, 0.577350269189, 0.577350269189); glVertex3d(-RADIUS, 0.0, 0.0 ); glVertex3d( 0.0, 0.0, RADIUS ); glVertex3d( 0.0, RADIUS, 0.0 );
    glNormal3d(-0.577350269189, 0.577350269189,-0.577350269189); glVertex3d(-RADIUS, 0.0, 0.0 ); glVertex3d( 0.0, RADIUS, 0.0 ); glVertex3d( 0.0, 0.0,-RADIUS );
    glNormal3d(-0.577350269189,-0.577350269189, 0.577350269189); glVertex3d(-RADIUS, 0.0, 0.0 ); glVertex3d( 0.0,-RADIUS, 0.0 ); glVertex3d( 0.0, 0.0, RADIUS );
    glNormal3d(-0.577350269189,-0.577350269189,-0.577350269189); glVertex3d(-RADIUS, 0.0, 0.0 ); glVertex3d( 0.0, 0.0,-RADIUS ); glVertex3d( 0.0,-RADIUS, 0.0 );
  glEnd();
#undef RADIUS
}

/*
 *
 */
static double icos_r[12][3] = {
    {  1.0,             0.0,             0.0            },
    {  0.447213595500,  0.894427191000,  0.0            },
    {  0.447213595500,  0.276393202252,  0.850650808354 },
    {  0.447213595500, -0.723606797748,  0.525731112119 },
    {  0.447213595500, -0.723606797748, -0.525731112119 },
    {  0.447213595500,  0.276393202252, -0.850650808354 },
    { -0.447213595500, -0.894427191000,  0.0 },
    { -0.447213595500, -0.276393202252,  0.850650808354 },
    { -0.447213595500,  0.723606797748,  0.525731112119 },
    { -0.447213595500,  0.723606797748, -0.525731112119 },
    { -0.447213595500, -0.276393202252, -0.850650808354 },
    { -1.0,             0.0,             0.0            }
};

static int icos_v [20][3] = {
    {  0,  1,  2 },
    {  0,  2,  3 },
    {  0,  3,  4 },
    {  0,  4,  5 },
    {  0,  5,  1 },
    {  1,  8,  2 },
    {  2,  7,  3 },
    {  3,  6,  4 },
    {  4, 10,  5 },
    {  5,  9,  1 },
    {  1,  9,  8 },
    {  2,  8,  7 },
    {  3,  7,  6 },
    {  4,  6, 10 },
    {  5, 10,  9 },
    { 11,  9, 10 },
    { 11,  8,  9 },
    { 11,  7,  8 },
    { 11,  6,  7 },
    { 11, 10,  6 }
};

void FGAPIENTRY glutWireIcosahedron( void )
{
  int i ;

  FREEGLUT_EXIT_IF_NOT_INITIALISED ( "glutWireIcosahedron" );

  for ( i = 0; i < 20; i++ )
  {
    double normal[3] ;
    normal[0] = ( icos_r[icos_v[i][1]][1] - icos_r[icos_v[i][0]][1] ) * ( icos_r[icos_v[i][2]][2] - icos_r[icos_v[i][0]][2] ) - ( icos_r[icos_v[i][1]][2] - icos_r[icos_v[i][0]][2] ) * ( icos_r[icos_v[i][2]][1] - icos_r[icos_v[i][0]][1] ) ;
    normal[1] = ( icos_r[icos_v[i][1]][2] - icos_r[icos_v[i][0]][2] ) * ( icos_r[icos_v[i][2]][0] - icos_r[icos_v[i][0]][0] ) - ( icos_r[icos_v[i][1]][0] - icos_r[icos_v[i][0]][0] ) * ( icos_r[icos_v[i][2]][2] - icos_r[icos_v[i][0]][2] ) ;
    normal[2] = ( icos_r[icos_v[i][1]][0] - icos_r[icos_v[i][0]][0] ) * ( icos_r[icos_v[i][2]][1] - icos_r[icos_v[i][0]][1] ) - ( icos_r[icos_v[i][1]][1] - icos_r[icos_v[i][0]][1] ) * ( icos_r[icos_v[i][2]][0] - icos_r[icos_v[i][0]][0] ) ;
    glBegin ( GL_LINE_LOOP ) ;
      glNormal3dv ( normal ) ;
      glVertex3dv ( icos_r[icos_v[i][0]] ) ;
      glVertex3dv ( icos_r[icos_v[i][1]] ) ;
      glVertex3dv ( icos_r[icos_v[i][2]] ) ;
    glEnd () ;
  }
}

/*
 *
 */
void FGAPIENTRY glutSolidIcosahedron( void )
{
  int i ;

  FREEGLUT_EXIT_IF_NOT_INITIALISED ( "glutSolidIcosahedron" );

  glBegin ( GL_TRIANGLES ) ;
  for ( i = 0; i < 20; i++ )
  {
    double normal[3] ;
    normal[0] = ( icos_r[icos_v[i][1]][1] - icos_r[icos_v[i][0]][1] ) * ( icos_r[icos_v[i][2]][2] - icos_r[icos_v[i][0]][2] ) - ( icos_r[icos_v[i][1]][2] - icos_r[icos_v[i][0]][2] ) * ( icos_r[icos_v[i][2]][1] - icos_r[icos_v[i][0]][1] ) ;
    normal[1] = ( icos_r[icos_v[i][1]][2] - icos_r[icos_v[i][0]][2] ) * ( icos_r[icos_v[i][2]][0] - icos_r[icos_v[i][0]][0] ) - ( icos_r[icos_v[i][1]][0] - icos_r[icos_v[i][0]][0] ) * ( icos_r[icos_v[i][2]][2] - icos_r[icos_v[i][0]][2] ) ;
    normal[2] = ( icos_r[icos_v[i][1]][0] - icos_r[icos_v[i][0]][0] ) * ( icos_r[icos_v[i][2]][1] - icos_r[icos_v[i][0]][1] ) - ( icos_r[icos_v[i][1]][1] - icos_r[icos_v[i][0]][1] ) * ( icos_r[icos_v[i][2]][0] - icos_r[icos_v[i][0]][0] ) ;
      glNormal3dv ( normal ) ;
      glVertex3dv ( icos_r[icos_v[i][0]] ) ;
      glVertex3dv ( icos_r[icos_v[i][1]] ) ;
      glVertex3dv ( icos_r[icos_v[i][2]] ) ;
  }

  glEnd () ;
}

/*
 *
 */
static double rdod_r[14][3] = {
    {  0.0,             0.0,             1.0 },
    {  0.707106781187,  0.000000000000,  0.5 },
    {  0.000000000000,  0.707106781187,  0.5 },
    { -0.707106781187,  0.000000000000,  0.5 },
    {  0.000000000000, -0.707106781187,  0.5 },
    {  0.707106781187,  0.707106781187,  0.0 },
    { -0.707106781187,  0.707106781187,  0.0 },
    { -0.707106781187, -0.707106781187,  0.0 },
    {  0.707106781187, -0.707106781187,  0.0 },
    {  0.707106781187,  0.000000000000, -0.5 },
    {  0.000000000000,  0.707106781187, -0.5 },
    { -0.707106781187,  0.000000000000, -0.5 },
    {  0.000000000000, -0.707106781187, -0.5 },
    {  0.0,             0.0,            -1.0 }
} ;

static int rdod_v [12][4] = {
    { 0,  1,  5,  2 },
    { 0,  2,  6,  3 },
    { 0,  3,  7,  4 },
    { 0,  4,  8,  1 },
    { 5, 10,  6,  2 },
    { 6, 11,  7,  3 },
    { 7, 12,  8,  4 },
    { 8,  9,  5,  1 },
    { 5,  9, 13, 10 },
    { 6, 10, 13, 11 },
    { 7, 11, 13, 12 },
    { 8, 12, 13,  9 }
};

static double rdod_n[12][3] = {
    {  0.353553390594,  0.353553390594,  0.5 },
    { -0.353553390594,  0.353553390594,  0.5 },
    { -0.353553390594, -0.353553390594,  0.5 },
    {  0.353553390594, -0.353553390594,  0.5 },
    {  0.000000000000,  1.000000000000,  0.0 },
    { -1.000000000000,  0.000000000000,  0.0 },
    {  0.000000000000, -1.000000000000,  0.0 },
    {  1.000000000000,  0.000000000000,  0.0 },
    {  0.353553390594,  0.353553390594, -0.5 },
    { -0.353553390594,  0.353553390594, -0.5 },
    { -0.353553390594, -0.353553390594, -0.5 },
    {  0.353553390594, -0.353553390594, -0.5 }
};

void FGAPIENTRY glutWireRhombicDodecahedron( void )
{
  int i ;

  FREEGLUT_EXIT_IF_NOT_INITIALISED ( "glutWireRhombicDodecahedron" );

  for ( i = 0; i < 12; i++ )
  {
    glBegin ( GL_LINE_LOOP ) ;
      glNormal3dv ( rdod_n[i] ) ;
      glVertex3dv ( rdod_r[rdod_v[i][0]] ) ;
      glVertex3dv ( rdod_r[rdod_v[i][1]] ) ;
      glVertex3dv ( rdod_r[rdod_v[i][2]] ) ;
      glVertex3dv ( rdod_r[rdod_v[i][3]] ) ;
    glEnd () ;
  }
}

/*
 *
 */
void FGAPIENTRY glutSolidRhombicDodecahedron( void )
{
  int i ;

  FREEGLUT_EXIT_IF_NOT_INITIALISED ( "glutSolidRhombicDodecahedron" );

  glBegin ( GL_QUADS ) ;
  for ( i = 0; i < 12; i++ )
  {
      glNormal3dv ( rdod_n[i] ) ;
      glVertex3dv ( rdod_r[rdod_v[i][0]] ) ;
      glVertex3dv ( rdod_r[rdod_v[i][1]] ) ;
      glVertex3dv ( rdod_r[rdod_v[i][2]] ) ;
      glVertex3dv ( rdod_r[rdod_v[i][3]] ) ;
  }

  glEnd () ;
}



/* -- INTERFACE FUNCTIONS -------------------------------------------------- */
/*
 * Draws a wireframed cube.
 */
void FGAPIENTRY glutWireCube( GLdouble dSize )
{
    FREEGLUT_EXIT_IF_NOT_INITIALISED ( "glutWireCube" );
    fghCube( dSize, TRUE );
}
void FGAPIENTRY glutSolidCube( GLdouble dSize )
{
    FREEGLUT_EXIT_IF_NOT_INITIALISED ( "glutSolidCube" );
    fghCube( dSize, FALSE );
}

void FGAPIENTRY glutWireTetrahedron( void )
{
    FREEGLUT_EXIT_IF_NOT_INITIALISED ( "glutWireTetrahedron" );
    fghTetrahedron( TRUE );
}
void FGAPIENTRY glutSolidTetrahedron( void )
{
    FREEGLUT_EXIT_IF_NOT_INITIALISED ( "glutSolidTetrahedron" );
    fghTetrahedron( FALSE );
}

void FGAPIENTRY glutWireSierpinskiSponge ( int num_levels, GLdouble offset[3], GLdouble scale )
{
    FREEGLUT_EXIT_IF_NOT_INITIALISED ( "glutWireSierpinskiSponge" );
    fghSierpinskiSponge ( num_levels, offset, scale, TRUE );
}
void FGAPIENTRY glutSolidSierpinskiSponge ( int num_levels, GLdouble offset[3], GLdouble scale )
{
    FREEGLUT_EXIT_IF_NOT_INITIALISED ( "glutSolidSierpinskiSponge" );
    fghSierpinskiSponge ( num_levels, offset, scale, FALSE );
}


/*** END OF FILE ***/
