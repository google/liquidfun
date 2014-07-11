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
#include "fg_gl2.h"
#include <math.h>

/*
 * A note: We do not use the GLuint data type for vertex index arrays
 * in this code as Open GL ES1 only supports GLushort. This affects the
 * cylindrical objects only (Torus, Sphere, Cylinder and Cone) and limits
 * their number of vertices to 65535 (2^16-1). Thats about 256*256
 * subdivisions, which is sufficient for just about any usage case, so
 * I am not going to worry about it for now.
 * One could do compile time detection of the gluint type through CMake,
 * but it is likely that we'll eventually move to runtime selection
 * of OpenGL or GLES1/2, which would make that strategy useless...
 */

/* declare for drawing using the different OpenGL versions here so we can
   have a nice code order below */
#ifndef GL_ES_VERSION_2_0
static void fghDrawGeometryWire11(GLfloat *vertices, GLfloat *normals,
                                  GLushort *vertIdxs, GLsizei numParts, GLsizei numVertPerPart, GLenum vertexMode,
                                  GLushort *vertIdxs2, GLsizei numParts2, GLsizei numVertPerPart2
    );
static void fghDrawGeometrySolid11(GLfloat *vertices, GLfloat *normals, GLfloat *textcs, GLsizei numVertices,
                                   GLushort *vertIdxs, GLsizei numParts, GLsizei numVertIdxsPerPart);
#endif
static void fghDrawGeometryWire20(GLfloat *vertices, GLfloat *normals, GLsizei numVertices,
                                  GLushort *vertIdxs, GLsizei numParts, GLsizei numVertPerPart, GLenum vertexMode,
                                  GLushort *vertIdxs2, GLsizei numParts2, GLsizei numVertPerPart2,
                                  GLint attribute_v_coord, GLint attribute_v_normal
    );
static void fghDrawGeometrySolid20(GLfloat *vertices, GLfloat *normals, GLfloat *textcs, GLsizei numVertices,
                                   GLushort *vertIdxs, GLsizei numParts, GLsizei numVertIdxsPerPart,
                                   GLint attribute_v_coord, GLint attribute_v_normal, GLint attribute_v_texture);
/* declare function for generating visualization of normals */
static void fghGenerateNormalVisualization(GLfloat *vertices, GLfloat *normals, GLsizei numVertices);
#ifndef GL_ES_VERSION_2_0
static void fghDrawNormalVisualization11();
#endif
static void fghDrawNormalVisualization20(GLint attribute_v_coord);

/* Drawing geometry:
 * Explanation of the functions has to be separate for the polyhedra and
 * the non-polyhedra (objects with a circular cross-section).
 * Polyhedra:
 *   - We have only implemented the five platonic solids and the rhomboid
 *     dodecahedron. If you need more types of polyhedra, please see
 *     CPolyhedron in MRPT
 *   - Solids are drawn by glDrawArrays if composed of triangular faces
 *     (the tetrahedron, octahedron, and icosahedron), or are first
 *     decomposed into triangles and then drawn by glDrawElements if its
 *     faces are squares or pentagons (cube, dodecahedron and rhombic
 *     dodecahedron) as some vertices are repeated in that case.
 *   - WireFrame drawing is done using a GL_LINE_LOOP per face, and thus
 *     issuing one draw call per face. glDrawArrays is always used as no
 *     triangle decomposition is needed to draw faces. We use the "first"
 *     parameter in glDrawArrays to go from face to face.
 * 
 * Non-polyhedra:
 *   - We have implemented the sphere, cylinder, cone and torus.
 *   - All shapes are characterized by two parameters: the number of
 *     subdivisions along two axes used to construct the shape's vertices 
 *     (e.g. stacks and slices for the sphere).
 *     As different subdivisions are most suitable for different shapes,
 *     and are thus also named differently, I wont provide general comments
 *     on them here.
 *   - Solids are drawn using glDrawArrays and GL_TRIANGLE_STRIP. Each
 *     strip covers one revolution around one of the two subdivision axes
 *     of the shape.
 *   - WireFrame drawing is done for the subdivisions along the two axes 
 *     separately, usually using GL_LINE_LOOP. Vertex index arrays are
 *     built containing the vertices to be drawn for each loop, which are
 *     then drawn using multiple calls to glDrawElements. As the number of
 *     subdivisions along the two axes is not guaranteed to be equal, the
 *     vertex indices for e.g. stacks and slices are stored in separate
 *     arrays, which makes the input to the drawing function a bit clunky,
 *     but allows for the same drawing function to be used for all shapes.
 */


/**
 * Draw geometric shape in wire mode (only edges)
 *
 * Arguments:
 * GLfloat *vertices, GLfloat *normals, GLsizei numVertices
 *   The vertex coordinate and normal buffers, and the number of entries in
 *   those
 * GLushort *vertIdxs
 *   a vertex indices buffer, optional (never passed for the polyhedra)
 * GLsizei numParts, GLsizei numVertPerPart
 *   polyhedra: number of faces, and the number of vertices for drawing
 *     each face
 *   non-polyhedra: number of edges to draw for first subdivision (not
 *     necessarily equal to number of subdivisions requested by user, e.g.
 *     as each subdivision is enclosed by two edges), and number of
 *     vertices for drawing each
 *   numParts * numVertPerPart gives the number of entries in the vertex
 *     array vertIdxs
 * GLenum vertexMode
 *   vertex drawing mode (e.g. always GL_LINE_LOOP for polyhedra, varies
 *   for others)
 * GLushort *vertIdxs2, GLsizei numParts2, GLsizei numVertPerPart2
 *   non-polyhedra only: same as the above, but now for subdivisions along
 *   the other axis. Always drawn as GL_LINE_LOOP.
 *
 * Feel free to contribute better naming ;)
 */
void fghDrawGeometryWire(GLfloat *vertices, GLfloat *normals, GLsizei numVertices,
                                GLushort *vertIdxs, GLsizei numParts, GLsizei numVertPerPart, GLenum vertexMode,
                                GLushort *vertIdxs2, GLsizei numParts2, GLsizei numVertPerPart2
    )
{
    GLint attribute_v_coord  = fgStructure.CurrentWindow->Window.attribute_v_coord;
    GLint attribute_v_normal = fgStructure.CurrentWindow->Window.attribute_v_normal;

    if (fgState.HasOpenGL20 && (attribute_v_coord != -1 || attribute_v_normal != -1))
        /* User requested a 2.0 draw */
        fghDrawGeometryWire20(vertices, normals, numVertices,
                              vertIdxs, numParts, numVertPerPart, vertexMode,
                              vertIdxs2, numParts2, numVertPerPart2,
                              attribute_v_coord, attribute_v_normal);
#ifndef GL_ES_VERSION_2_0
    else
        fghDrawGeometryWire11(vertices, normals,
                              vertIdxs, numParts, numVertPerPart, vertexMode,
                              vertIdxs2, numParts2, numVertPerPart2);
#endif
}

/* Draw the geometric shape with filled triangles
 *
 * Arguments:
 * GLfloat *vertices, GLfloat *normals, GLfloat *textcs, GLsizei numVertices
 *   The vertex coordinate, normal and texture coordinate buffers, and the
 *   number of entries in those
 * GLushort *vertIdxs
 *   a vertex indices buffer, optional (not passed for the polyhedra with
 *   triangular faces)
 * GLsizei numParts, GLsizei numVertPerPart
 *   polyhedra: not used for polyhedra with triangular faces
       (numEdgePerFace==3), as each vertex+normal pair is drawn only once,
       so no vertex indices are used.
       Else, the shape was triangulated (DECOMPOSE_TO_TRIANGLE), leading to
       reuse of some vertex+normal pairs, and thus the need to draw with
       glDrawElements. numParts is always 1 in this case (we can draw the
       whole object with one call to glDrawElements as the vertex index
       array contains separate triangles), and numVertPerPart indicates
       the number of vertex indices in the vertex array.
 *   non-polyhedra: number of parts (GL_TRIANGLE_STRIPs) to be drawn
       separately (numParts calls to glDrawElements) to create the object.
       numVertPerPart indicates the number of vertex indices to be
       processed at each draw call.
 *   numParts * numVertPerPart gives the number of entries in the vertex
 *     array vertIdxs
 */
void fghDrawGeometrySolid(GLfloat *vertices, GLfloat *normals, GLfloat *textcs, GLsizei numVertices,
                          GLushort *vertIdxs, GLsizei numParts, GLsizei numVertIdxsPerPart)
{
    GLint attribute_v_coord   = fgStructure.CurrentWindow->Window.attribute_v_coord;
    GLint attribute_v_normal  = fgStructure.CurrentWindow->Window.attribute_v_normal;
    GLint attribute_v_texture = fgStructure.CurrentWindow->Window.attribute_v_texture;

    if (fgStructure.CurrentWindow->State.VisualizeNormals)
        /* generate normals for each vertex to be drawn as well */
        fghGenerateNormalVisualization(vertices, normals, numVertices);

    if (fgState.HasOpenGL20 && (attribute_v_coord != -1 || attribute_v_normal != -1))
    {
        /* User requested a 2.0 draw */
        fghDrawGeometrySolid20(vertices, normals, textcs, numVertices,
                               vertIdxs, numParts, numVertIdxsPerPart,
                               attribute_v_coord, attribute_v_normal, attribute_v_texture);

        if (fgStructure.CurrentWindow->State.VisualizeNormals)
            /* draw normals for each vertex as well */
            fghDrawNormalVisualization20(attribute_v_coord);
    }
#ifndef GL_ES_VERSION_2_0
    else
    {
        fghDrawGeometrySolid11(vertices, normals, textcs, numVertices,
                               vertIdxs, numParts, numVertIdxsPerPart);

        if (fgStructure.CurrentWindow->State.VisualizeNormals)
            /* draw normals for each vertex as well */
            fghDrawNormalVisualization11();
    }
#endif
}



/* Version for OpenGL (ES) 1.1 */
#ifndef GL_ES_VERSION_2_0
static void fghDrawGeometryWire11(GLfloat *vertices, GLfloat *normals,
                                  GLushort *vertIdxs, GLsizei numParts, GLsizei numVertPerPart, GLenum vertexMode,
                                  GLushort *vertIdxs2, GLsizei numParts2, GLsizei numVertPerPart2
    )
{
    int i;
    
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);

    glVertexPointer(3, GL_FLOAT, 0, vertices);
    glNormalPointer(GL_FLOAT, 0, normals);

    
    if (!vertIdxs)
        /* Draw per face (TODO: could use glMultiDrawArrays if available) */
        for (i=0; i<numParts; i++)
            glDrawArrays(vertexMode, i*numVertPerPart, numVertPerPart);
    else
        for (i=0; i<numParts; i++)
            glDrawElements(vertexMode,numVertPerPart,GL_UNSIGNED_SHORT,vertIdxs+i*numVertPerPart);

    if (vertIdxs2)
        for (i=0; i<numParts2; i++)
            glDrawElements(GL_LINE_LOOP,numVertPerPart2,GL_UNSIGNED_SHORT,vertIdxs2+i*numVertPerPart2);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
}


static void fghDrawGeometrySolid11(GLfloat *vertices, GLfloat *normals, GLfloat *textcs, GLsizei numVertices,
                                   GLushort *vertIdxs, GLsizei numParts, GLsizei numVertIdxsPerPart)
{
    int i;

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);

    glVertexPointer(3, GL_FLOAT, 0, vertices);
    glNormalPointer(GL_FLOAT, 0, normals);

    if (textcs)
    {
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glTexCoordPointer(2, GL_FLOAT, 0, textcs);
    }

    if (!vertIdxs)
        glDrawArrays(GL_TRIANGLES, 0, numVertices);
    else
        if (numParts>1)
            for (i=0; i<numParts; i++)
                glDrawElements(GL_TRIANGLE_STRIP, numVertIdxsPerPart, GL_UNSIGNED_SHORT, vertIdxs+i*numVertIdxsPerPart);
        else
            glDrawElements(GL_TRIANGLES, numVertIdxsPerPart, GL_UNSIGNED_SHORT, vertIdxs);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    if (textcs)
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}
#endif

/* Version for OpenGL (ES) >= 2.0 */
static void fghDrawGeometryWire20(GLfloat *vertices, GLfloat *normals, GLsizei numVertices,
                                  GLushort *vertIdxs, GLsizei numParts, GLsizei numVertPerPart, GLenum vertexMode,
                                  GLushort *vertIdxs2, GLsizei numParts2, GLsizei numVertPerPart2,
                                  GLint attribute_v_coord, GLint attribute_v_normal)
{
    GLuint vbo_coords = 0, vbo_normals = 0,
        ibo_elements = 0, ibo_elements2 = 0;
    GLsizei numVertIdxs = numParts * numVertPerPart;
    GLsizei numVertIdxs2 = numParts2 * numVertPerPart2;
    int i;

    if (numVertices > 0 && attribute_v_coord != -1) {
        fghGenBuffers(1, &vbo_coords);
        fghBindBuffer(FGH_ARRAY_BUFFER, vbo_coords);
        fghBufferData(FGH_ARRAY_BUFFER, numVertices * 3 * sizeof(vertices[0]),
                      vertices, FGH_STATIC_DRAW);
    }
    
    if (numVertices > 0 && attribute_v_normal != -1) {
        fghGenBuffers(1, &vbo_normals);
        fghBindBuffer(FGH_ARRAY_BUFFER, vbo_normals);
        fghBufferData(FGH_ARRAY_BUFFER, numVertices * 3 * sizeof(normals[0]),
                      normals, FGH_STATIC_DRAW);
    }
    
    if (vertIdxs != NULL) {
        fghGenBuffers(1, &ibo_elements);
        fghBindBuffer(FGH_ELEMENT_ARRAY_BUFFER, ibo_elements);
        fghBufferData(FGH_ELEMENT_ARRAY_BUFFER, numVertIdxs * sizeof(vertIdxs[0]),
                      vertIdxs, FGH_STATIC_DRAW);
        fghBindBuffer(FGH_ELEMENT_ARRAY_BUFFER, 0);
    }

    if (vertIdxs2 != NULL) {
        fghGenBuffers(1, &ibo_elements2);
        fghBindBuffer(FGH_ELEMENT_ARRAY_BUFFER, ibo_elements2);
        fghBufferData(FGH_ELEMENT_ARRAY_BUFFER, numVertIdxs2 * sizeof(vertIdxs2[0]),
                      vertIdxs2, FGH_STATIC_DRAW);
        fghBindBuffer(FGH_ELEMENT_ARRAY_BUFFER, 0);
    }

    if (vbo_coords) {
        fghEnableVertexAttribArray(attribute_v_coord);
        fghBindBuffer(FGH_ARRAY_BUFFER, vbo_coords);
        fghVertexAttribPointer(
            attribute_v_coord,  /* attribute */
            3,                  /* number of elements per vertex, here (x,y,z) */
            GL_FLOAT,           /* the type of each element */
            GL_FALSE,           /* take our values as-is */
            0,                  /* no extra data between each position */
            0                   /* offset of first element */
        );
        fghBindBuffer(FGH_ARRAY_BUFFER, 0);
    }

    if (vbo_normals) {
        fghEnableVertexAttribArray(attribute_v_normal);
        fghBindBuffer(FGH_ARRAY_BUFFER, vbo_normals);
        fghVertexAttribPointer(
            attribute_v_normal, /* attribute */
            3,                  /* number of elements per vertex, here (x,y,z) */
            GL_FLOAT,           /* the type of each element */
            GL_FALSE,           /* take our values as-is */
            0,                  /* no extra data between each position */
            0                   /* offset of first element */
        );
        fghBindBuffer(FGH_ARRAY_BUFFER, 0);
    }

    if (!vertIdxs) {
        /* Draw per face (TODO: could use glMultiDrawArrays if available) */
        for (i=0; i<numParts; i++)
            glDrawArrays(vertexMode, i*numVertPerPart, numVertPerPart);
    } else {
        fghBindBuffer(FGH_ELEMENT_ARRAY_BUFFER, ibo_elements);
        for (i=0; i<numParts; i++)
            glDrawElements(vertexMode, numVertPerPart,
                           GL_UNSIGNED_SHORT, (GLvoid*)(sizeof(vertIdxs[0])*i*numVertPerPart));
        /* Clean existing bindings before clean-up */
        /* Android showed instability otherwise */
        fghBindBuffer(FGH_ELEMENT_ARRAY_BUFFER, 0);
    }

    if (vertIdxs2) {
        fghBindBuffer(FGH_ELEMENT_ARRAY_BUFFER, ibo_elements2);
        for (i=0; i<numParts2; i++)
            glDrawElements(GL_LINE_LOOP, numVertPerPart2,
                           GL_UNSIGNED_SHORT, (GLvoid*)(sizeof(vertIdxs2[0])*i*numVertPerPart2));
        /* Clean existing bindings before clean-up */
        /* Android showed instability otherwise */
        fghBindBuffer(FGH_ELEMENT_ARRAY_BUFFER, 0);
    }
    
    if (vbo_coords != 0)
        fghDisableVertexAttribArray(attribute_v_coord);
    if (vbo_normals != 0)
        fghDisableVertexAttribArray(attribute_v_normal);
    
    if (vbo_coords != 0)
        fghDeleteBuffers(1, &vbo_coords);
    if (vbo_normals != 0)
        fghDeleteBuffers(1, &vbo_normals);
    if (ibo_elements != 0)
        fghDeleteBuffers(1, &ibo_elements);
    if (ibo_elements2 != 0)
        fghDeleteBuffers(1, &ibo_elements2);
}




/* Version for OpenGL (ES) >= 2.0 */
static void fghDrawGeometrySolid20(GLfloat *vertices, GLfloat *normals, GLfloat *textcs, GLsizei numVertices,
                                   GLushort *vertIdxs, GLsizei numParts, GLsizei numVertIdxsPerPart,
                                   GLint attribute_v_coord, GLint attribute_v_normal, GLint attribute_v_texture)
{
    GLuint vbo_coords = 0, vbo_normals = 0, vbo_textcs = 0, ibo_elements = 0;
    GLsizei numVertIdxs = numParts * numVertIdxsPerPart;
    int i;
  
    if (numVertices > 0 && attribute_v_coord != -1) {
        fghGenBuffers(1, &vbo_coords);
        fghBindBuffer(FGH_ARRAY_BUFFER, vbo_coords);
        fghBufferData(FGH_ARRAY_BUFFER, numVertices * 3 * sizeof(vertices[0]),
                      vertices, FGH_STATIC_DRAW);
        fghBindBuffer(FGH_ARRAY_BUFFER, 0);
    }
    
    if (numVertices > 0 && attribute_v_normal != -1) {
        fghGenBuffers(1, &vbo_normals);
        fghBindBuffer(FGH_ARRAY_BUFFER, vbo_normals);
        fghBufferData(FGH_ARRAY_BUFFER, numVertices * 3 * sizeof(normals[0]),
                      normals, FGH_STATIC_DRAW);
        fghBindBuffer(FGH_ARRAY_BUFFER, 0);
    }

    if (numVertices > 0 && attribute_v_texture != -1 && textcs) {
        fghGenBuffers(1, &vbo_textcs);
        fghBindBuffer(FGH_ARRAY_BUFFER, vbo_textcs);
        fghBufferData(FGH_ARRAY_BUFFER, numVertices * 2 * sizeof(textcs[0]),
                      textcs, FGH_STATIC_DRAW);
        fghBindBuffer(FGH_ARRAY_BUFFER, 0);
    }
    
    if (vertIdxs != NULL) {
        fghGenBuffers(1, &ibo_elements);
        fghBindBuffer(FGH_ELEMENT_ARRAY_BUFFER, ibo_elements);
        fghBufferData(FGH_ELEMENT_ARRAY_BUFFER, numVertIdxs * sizeof(vertIdxs[0]),
                      vertIdxs, FGH_STATIC_DRAW);
        fghBindBuffer(FGH_ELEMENT_ARRAY_BUFFER, 0);
    }
    
    if (vbo_coords) {
        fghEnableVertexAttribArray(attribute_v_coord);
        fghBindBuffer(FGH_ARRAY_BUFFER, vbo_coords);
        fghVertexAttribPointer(
            attribute_v_coord,  /* attribute */
            3,                  /* number of elements per vertex, here (x,y,z) */
            GL_FLOAT,           /* the type of each element */
            GL_FALSE,           /* take our values as-is */
            0,                  /* no extra data between each position */
            0                   /* offset of first element */
        );
        fghBindBuffer(FGH_ARRAY_BUFFER, 0);
    };
    
    if (vbo_normals) {
        fghEnableVertexAttribArray(attribute_v_normal);
        fghBindBuffer(FGH_ARRAY_BUFFER, vbo_normals);
        fghVertexAttribPointer(
            attribute_v_normal, /* attribute */
            3,                  /* number of elements per vertex, here (x,y,z) */
            GL_FLOAT,           /* the type of each element */
            GL_FALSE,           /* take our values as-is */
            0,                  /* no extra data between each position */
            0                   /* offset of first element */
        );
        fghBindBuffer(FGH_ARRAY_BUFFER, 0);
    };

    if (vbo_textcs) {
        fghEnableVertexAttribArray(attribute_v_texture);
        fghBindBuffer(FGH_ARRAY_BUFFER, vbo_textcs);
        fghVertexAttribPointer(
            attribute_v_texture,/* attribute */
            2,                  /* number of elements per vertex, here (s,t) */
            GL_FLOAT,           /* the type of each element */
            GL_FALSE,           /* take our values as-is */
            0,                  /* no extra data between each position */
            0                   /* offset of first element */
            );
        fghBindBuffer(FGH_ARRAY_BUFFER, 0);
    };
    
    if (vertIdxs == NULL) {
        glDrawArrays(GL_TRIANGLES, 0, numVertices);
    } else {
        fghBindBuffer(FGH_ELEMENT_ARRAY_BUFFER, ibo_elements);
        if (numParts>1) {
            for (i=0; i<numParts; i++) {
                glDrawElements(GL_TRIANGLE_STRIP, numVertIdxsPerPart, GL_UNSIGNED_SHORT, (GLvoid*)(sizeof(vertIdxs[0])*i*numVertIdxsPerPart));
            }
        } else {
            glDrawElements(GL_TRIANGLES, numVertIdxsPerPart, GL_UNSIGNED_SHORT, 0);
        }
        /* Clean existing bindings before clean-up */
        /* Android showed instability otherwise */
        fghBindBuffer(FGH_ELEMENT_ARRAY_BUFFER, 0);
    }
    
    if (vbo_coords != 0)
        fghDisableVertexAttribArray(attribute_v_coord);
    if (vbo_normals != 0)
        fghDisableVertexAttribArray(attribute_v_normal);
    if (vbo_textcs != 0)
        fghDisableVertexAttribArray(attribute_v_texture);
    
    if (vbo_coords != 0)
        fghDeleteBuffers(1, &vbo_coords);
    if (vbo_normals != 0)
        fghDeleteBuffers(1, &vbo_normals);
    if (vbo_textcs != 0)
        fghDeleteBuffers(1, &vbo_textcs);
    if (ibo_elements != 0)
        fghDeleteBuffers(1, &ibo_elements);
}



/**
 * Generate vertex indices for visualizing the normals.
 * vertices are written into verticesForNormalVisualization.
 * This must be freed by caller, we do the free at the
 * end of fghDrawNormalVisualization11/fghDrawNormalVisualization20
 */
static GLfloat *verticesForNormalVisualization;
static GLsizei numNormalVertices = 0;
static void fghGenerateNormalVisualization(GLfloat *vertices, GLfloat *normals, GLsizei numVertices)
{
    int i,j;
    numNormalVertices = numVertices * 2;
    verticesForNormalVisualization = (GLfloat*)malloc(numNormalVertices*3 * sizeof(GLfloat));

    for (i=0,j=0; i<numNormalVertices*3/2; i+=3, j+=6)
    {
        verticesForNormalVisualization[j+0] = vertices[i+0];
        verticesForNormalVisualization[j+1] = vertices[i+1];
        verticesForNormalVisualization[j+2] = vertices[i+2];
        verticesForNormalVisualization[j+3] = vertices[i+0] + normals[i+0]/4.f;
        verticesForNormalVisualization[j+4] = vertices[i+1] + normals[i+1]/4.f;
        verticesForNormalVisualization[j+5] = vertices[i+2] + normals[i+2]/4.f;
    }
}

/* Version for OpenGL (ES) 1.1 */
#ifndef GL_ES_VERSION_2_0
static void fghDrawNormalVisualization11()
{
    GLfloat currentColor[4];
    /* Setup draw color: (1,1,1)-shape's color */
    glGetFloatv(GL_CURRENT_COLOR,currentColor);
    glColor4f(1-currentColor[0],1-currentColor[1],1-currentColor[2],currentColor[3]);

    glEnableClientState(GL_VERTEX_ARRAY);

    glVertexPointer(3, GL_FLOAT, 0, verticesForNormalVisualization);
    glDrawArrays(GL_LINES, 0, numNormalVertices);

    glDisableClientState(GL_VERTEX_ARRAY);

    /* Done, free memory, reset color */
    free(verticesForNormalVisualization);
    glColor4f(currentColor[0],currentColor[1],currentColor[2],currentColor[3]);
}
#endif

/* Version for OpenGL (ES) >= 2.0 */
static void fghDrawNormalVisualization20(GLint attribute_v_coord)
{
    GLuint vbo_coords = 0;

    if (attribute_v_coord != -1) {
        fghGenBuffers(1, &vbo_coords);
        fghBindBuffer(FGH_ARRAY_BUFFER, vbo_coords);
        fghBufferData(FGH_ARRAY_BUFFER, numNormalVertices * 3 * sizeof(verticesForNormalVisualization[0]),
            verticesForNormalVisualization, FGH_STATIC_DRAW);
    }


    if (vbo_coords) {
        fghEnableVertexAttribArray(attribute_v_coord);
        fghBindBuffer(FGH_ARRAY_BUFFER, vbo_coords);
        fghVertexAttribPointer(
            attribute_v_coord,  /* attribute */
            3,                  /* number of elements per vertex, here (x,y,z) */
            GL_FLOAT,           /* the type of each element */
            GL_FALSE,           /* take our values as-is */
            0,                  /* no extra data between each position */
            0                   /* offset of first element */
            );
        fghBindBuffer(FGH_ARRAY_BUFFER, 0);
    }

    glDrawArrays(GL_LINES, 0, numNormalVertices);

    if (vbo_coords != 0)
        fghDisableVertexAttribArray(attribute_v_coord);

    if (vbo_coords != 0)
        fghDeleteBuffers(1, &vbo_coords);

    /* Done, free memory */
    free(verticesForNormalVisualization);
}

/**
 * Generate all combinations of vertices and normals needed to draw object.
 * Optional shape decomposition to triangles:
 * We'll use glDrawElements to draw all shapes that are not naturally
 * composed of triangles, so generate an index vector here, using the
 * below sampling scheme.
 * Be careful to keep winding of all triangles counter-clockwise,
 * assuming that input has correct winding...
 */
static GLubyte   vert4Decomp[6] = {0,1,2, 0,2,3};             /* quad    : 4 input vertices, 6 output (2 triangles) */
static GLubyte   vert5Decomp[9] = {0,1,2, 0,2,4, 4,2,3};      /* pentagon: 5 input vertices, 9 output (3 triangles) */

static void fghGenerateGeometryWithIndexArray(int numFaces, int numEdgePerFace, GLfloat *vertices, GLubyte *vertIndices, GLfloat *normals, GLfloat *vertOut, GLfloat *normOut, GLushort *vertIdxOut)
{
    int i,j,numEdgeIdxPerFace = 0;
    GLubyte   *vertSamps = NULL;
    switch (numEdgePerFace)
    {
    case 3:
        /* nothing to do here, we'll draw with glDrawArrays */
        break;
    case 4:
        vertSamps = vert4Decomp;
        numEdgeIdxPerFace = 6;      /* 6 output vertices for each face */
        break;
    case 5:
        vertSamps = vert5Decomp;
        numEdgeIdxPerFace = 9;      /* 9 output vertices for each face */
        break;
    }
    /*
     * Build array with vertices using vertex coordinates and vertex indices
     * Do same for normals.
     * Need to do this because of different normals at shared vertices.
     */
    for (i=0; i<numFaces; i++)
    {
        int normIdx         = i*3;
        int faceIdxVertIdx  = i*numEdgePerFace; /* index to first element of "row" in vertex indices */
        for (j=0; j<numEdgePerFace; j++)
        {
            int outIdx  = i*numEdgePerFace*3+j*3;
            int vertIdx = vertIndices[faceIdxVertIdx+j]*3;

            vertOut[outIdx  ] = vertices[vertIdx  ];
            vertOut[outIdx+1] = vertices[vertIdx+1];
            vertOut[outIdx+2] = vertices[vertIdx+2];

            normOut[outIdx  ] = normals [normIdx  ];
            normOut[outIdx+1] = normals [normIdx+1];
            normOut[outIdx+2] = normals [normIdx+2];
        }

        /* generate vertex indices for each face */
        if (vertSamps)
            for (j=0; j<numEdgeIdxPerFace; j++)
                vertIdxOut[i*numEdgeIdxPerFace+j] = (GLushort)(faceIdxVertIdx + vertSamps[j]);
    }
}

static void fghGenerateGeometry(int numFaces, int numEdgePerFace, GLfloat *vertices, GLubyte *vertIndices, GLfloat *normals, GLfloat *vertOut, GLfloat *normOut)
{
    /* This function does the same as fghGenerateGeometryWithIndexArray, just skipping the index array generation... */
    fghGenerateGeometryWithIndexArray(numFaces, numEdgePerFace, vertices, vertIndices, normals, vertOut, normOut, NULL);
}


/* -- INTERNAL SETUP OF GEOMETRY --------------------------------------- */
/* -- stuff that can be cached -- */
/* Cache of input to glDrawArrays or glDrawElements
 * In general, we build arrays with all vertices or normals.
 * We cant compress this and use glDrawElements as all combinations of
 * vertices and normals are unique.
 */
#define DECLARE_SHAPE_CACHE(name,nameICaps,nameCaps)\
    static GLboolean name##Cached = GL_FALSE;\
    static GLfloat   name##_verts[nameCaps##_VERT_ELEM_PER_OBJ];\
    static GLfloat   name##_norms[nameCaps##_VERT_ELEM_PER_OBJ];\
    static void fgh##nameICaps##Generate()\
    {\
        fghGenerateGeometry(nameCaps##_NUM_FACES, nameCaps##_NUM_EDGE_PER_FACE,\
                            name##_v, name##_vi, name##_n,\
                            name##_verts, name##_norms);\
    }
#define DECLARE_SHAPE_CACHE_DECOMPOSE_TO_TRIANGLE(name,nameICaps,nameCaps)\
    static GLboolean name##Cached = GL_FALSE;\
    static GLfloat   name##_verts[nameCaps##_VERT_ELEM_PER_OBJ];\
    static GLfloat   name##_norms[nameCaps##_VERT_ELEM_PER_OBJ];\
    static GLushort  name##_vertIdxs[nameCaps##_VERT_PER_OBJ_TRI];\
    static void fgh##nameICaps##Generate()\
    {\
        fghGenerateGeometryWithIndexArray(nameCaps##_NUM_FACES, nameCaps##_NUM_EDGE_PER_FACE,\
                                          name##_v, name##_vi, name##_n,\
                                          name##_verts, name##_norms, name##_vertIdxs);\
    }

/* -- Cube -- */
#define CUBE_NUM_VERT           8
#define CUBE_NUM_FACES          6
#define CUBE_NUM_EDGE_PER_FACE  4
#define CUBE_VERT_PER_OBJ       (CUBE_NUM_FACES*CUBE_NUM_EDGE_PER_FACE)
#define CUBE_VERT_ELEM_PER_OBJ  (CUBE_VERT_PER_OBJ*3)
#define CUBE_VERT_PER_OBJ_TRI   (CUBE_VERT_PER_OBJ+CUBE_NUM_FACES*2)    /* 2 extra edges per face when drawing quads as triangles */
/* Vertex Coordinates */
static GLfloat cube_v[CUBE_NUM_VERT*3] =
{
     .5f, .5f, .5f,
    -.5f, .5f, .5f,
    -.5f,-.5f, .5f,
     .5f,-.5f, .5f,
     .5f,-.5f,-.5f,
     .5f, .5f,-.5f,
    -.5f, .5f,-.5f,
    -.5f,-.5f,-.5f
};
/* Normal Vectors */
static GLfloat cube_n[CUBE_NUM_FACES*3] =
{
     0.0f, 0.0f, 1.0f,
     1.0f, 0.0f, 0.0f,
     0.0f, 1.0f, 0.0f,
    -1.0f, 0.0f, 0.0f,
     0.0f,-1.0f, 0.0f,
     0.0f, 0.0f,-1.0f
};

/* Vertex indices, as quads, before triangulation */
static GLubyte cube_vi[CUBE_VERT_PER_OBJ] =
{
    0,1,2,3,
    0,3,4,5,
    0,5,6,1,
    1,6,7,2,
    7,4,3,2,
    4,7,6,5
};
DECLARE_SHAPE_CACHE_DECOMPOSE_TO_TRIANGLE(cube,Cube,CUBE)

/* -- Dodecahedron -- */
/* Magic Numbers:  It is possible to create a dodecahedron by attaching two
 * pentagons to each face of of a cube. The coordinates of the points are:
 *   (+-x,0, z); (+-1, 1, 1); (0, z, x )
 * where x = (-1 + sqrt(5))/2, z = (1 + sqrt(5))/2 or
 *       x = 0.61803398875 and z = 1.61803398875.
 */
#define DODECAHEDRON_NUM_VERT           20
#define DODECAHEDRON_NUM_FACES          12
#define DODECAHEDRON_NUM_EDGE_PER_FACE  5
#define DODECAHEDRON_VERT_PER_OBJ       (DODECAHEDRON_NUM_FACES*DODECAHEDRON_NUM_EDGE_PER_FACE)
#define DODECAHEDRON_VERT_ELEM_PER_OBJ  (DODECAHEDRON_VERT_PER_OBJ*3)
#define DODECAHEDRON_VERT_PER_OBJ_TRI   (DODECAHEDRON_VERT_PER_OBJ+DODECAHEDRON_NUM_FACES*4)    /* 4 extra edges per face when drawing pentagons as triangles */
/* Vertex Coordinates */
static GLfloat dodecahedron_v[DODECAHEDRON_NUM_VERT*3] =
{
               0.0f,  1.61803398875f,  0.61803398875f,
    -          1.0f,            1.0f,            1.0f,
    -0.61803398875f,            0.0f,  1.61803398875f,
     0.61803398875f,            0.0f,  1.61803398875f,
               1.0f,            1.0f,            1.0f,
               0.0f,  1.61803398875f, -0.61803398875f,
               1.0f,            1.0f, -          1.0f,
     0.61803398875f,            0.0f, -1.61803398875f,
    -0.61803398875f,            0.0f, -1.61803398875f,
    -          1.0f,            1.0f, -          1.0f,
               0.0f, -1.61803398875f,  0.61803398875f,
               1.0f, -          1.0f,            1.0f,
    -          1.0f, -          1.0f,            1.0f,
               0.0f, -1.61803398875f, -0.61803398875f,
    -          1.0f, -          1.0f, -          1.0f,
               1.0f, -          1.0f, -          1.0f,
     1.61803398875f, -0.61803398875f,            0.0f,
     1.61803398875f,  0.61803398875f,            0.0f,
    -1.61803398875f,  0.61803398875f,            0.0f,
    -1.61803398875f, -0.61803398875f,            0.0f
};
/* Normal Vectors */
static GLfloat dodecahedron_n[DODECAHEDRON_NUM_FACES*3] =
{
                0.0f,  0.525731112119f,  0.850650808354f,
                0.0f,  0.525731112119f, -0.850650808354f,
                0.0f, -0.525731112119f,  0.850650808354f,
                0.0f, -0.525731112119f, -0.850650808354f,

     0.850650808354f,             0.0f,  0.525731112119f,
    -0.850650808354f,             0.0f,  0.525731112119f,
     0.850650808354f,             0.0f, -0.525731112119f,
    -0.850650808354f,             0.0f, -0.525731112119f,

     0.525731112119f,  0.850650808354f,             0.0f,
     0.525731112119f, -0.850650808354f,             0.0f,
    -0.525731112119f,  0.850650808354f,             0.0f, 
    -0.525731112119f, -0.850650808354f,             0.0f,
};

/* Vertex indices */
static GLubyte dodecahedron_vi[DODECAHEDRON_VERT_PER_OBJ] =
{
     0,  1,  2,  3,  4, 
     5,  6,  7,  8,  9, 
    10, 11,  3,  2, 12, 
    13, 14,  8,  7, 15, 

     3, 11, 16, 17,  4, 
     2,  1, 18, 19, 12, 
     7,  6, 17, 16, 15, 
     8, 14, 19, 18,  9, 

    17,  6,  5,  0,  4, 
    16, 11, 10, 13, 15, 
    18,  1,  0,  5,  9, 
    19, 14, 13, 10, 12
};
DECLARE_SHAPE_CACHE_DECOMPOSE_TO_TRIANGLE(dodecahedron,Dodecahedron,DODECAHEDRON)


/* -- Icosahedron -- */
#define ICOSAHEDRON_NUM_VERT           12
#define ICOSAHEDRON_NUM_FACES          20
#define ICOSAHEDRON_NUM_EDGE_PER_FACE  3
#define ICOSAHEDRON_VERT_PER_OBJ       (ICOSAHEDRON_NUM_FACES*ICOSAHEDRON_NUM_EDGE_PER_FACE)
#define ICOSAHEDRON_VERT_ELEM_PER_OBJ  (ICOSAHEDRON_VERT_PER_OBJ*3)
#define ICOSAHEDRON_VERT_PER_OBJ_TRI   ICOSAHEDRON_VERT_PER_OBJ
/* Vertex Coordinates */
static GLfloat icosahedron_v[ICOSAHEDRON_NUM_VERT*3] =
{
                1.0f,             0.0f,             0.0f,
     0.447213595500f,  0.894427191000f,             0.0f,
     0.447213595500f,  0.276393202252f,  0.850650808354f,
     0.447213595500f, -0.723606797748f,  0.525731112119f,
     0.447213595500f, -0.723606797748f, -0.525731112119f,
     0.447213595500f,  0.276393202252f, -0.850650808354f,
    -0.447213595500f, -0.894427191000f,             0.0f,
    -0.447213595500f, -0.276393202252f,  0.850650808354f,
    -0.447213595500f,  0.723606797748f,  0.525731112119f,
    -0.447213595500f,  0.723606797748f, -0.525731112119f,
    -0.447213595500f, -0.276393202252f, -0.850650808354f,
    -           1.0f,             0.0f,             0.0f
};
/* Normal Vectors:
 * icosahedron_n[i][0] = ( icosahedron_v[icosahedron_vi[i][1]][1] - icosahedron_v[icosahedron_vi[i][0]][1] ) * ( icosahedron_v[icosahedron_vi[i][2]][2] - icosahedron_v[icosahedron_vi[i][0]][2] ) - ( icosahedron_v[icosahedron_vi[i][1]][2] - icosahedron_v[icosahedron_vi[i][0]][2] ) * ( icosahedron_v[icosahedron_vi[i][2]][1] - icosahedron_v[icosahedron_vi[i][0]][1] ) ;
 * icosahedron_n[i][1] = ( icosahedron_v[icosahedron_vi[i][1]][2] - icosahedron_v[icosahedron_vi[i][0]][2] ) * ( icosahedron_v[icosahedron_vi[i][2]][0] - icosahedron_v[icosahedron_vi[i][0]][0] ) - ( icosahedron_v[icosahedron_vi[i][1]][0] - icosahedron_v[icosahedron_vi[i][0]][0] ) * ( icosahedron_v[icosahedron_vi[i][2]][2] - icosahedron_v[icosahedron_vi[i][0]][2] ) ;
 * icosahedron_n[i][2] = ( icosahedron_v[icosahedron_vi[i][1]][0] - icosahedron_v[icosahedron_vi[i][0]][0] ) * ( icosahedron_v[icosahedron_vi[i][2]][1] - icosahedron_v[icosahedron_vi[i][0]][1] ) - ( icosahedron_v[icosahedron_vi[i][1]][1] - icosahedron_v[icosahedron_vi[i][0]][1] ) * ( icosahedron_v[icosahedron_vi[i][2]][0] - icosahedron_v[icosahedron_vi[i][0]][0] ) ;
*/
static GLfloat icosahedron_n[ICOSAHEDRON_NUM_FACES*3] =
{
     0.760845213037948f,  0.470228201835026f,  0.341640786498800f,
     0.760845213036861f, -0.179611190632978f,  0.552786404500000f,
     0.760845213033849f, -0.581234022404097f,                0.0f,
     0.760845213036861f, -0.179611190632978f, -0.552786404500000f,
     0.760845213037948f,  0.470228201835026f, -0.341640786498800f,
     0.179611190628666f,  0.760845213037948f,  0.552786404498399f,
     0.179611190634277f, -0.290617011204044f,  0.894427191000000f,
     0.179611190633958f, -0.940456403667806f,                0.0f,
     0.179611190634278f, -0.290617011204044f, -0.894427191000000f,
     0.179611190628666f,  0.760845213037948f, -0.552786404498399f,
    -0.179611190633958f,  0.940456403667806f,                0.0f,
    -0.179611190634277f,  0.290617011204044f,  0.894427191000000f,
    -0.179611190628666f, -0.760845213037948f,  0.552786404498399f,
    -0.179611190628666f, -0.760845213037948f, -0.552786404498399f,
    -0.179611190634277f,  0.290617011204044f, -0.894427191000000f,
    -0.760845213036861f,  0.179611190632978f, -0.552786404500000f,
    -0.760845213033849f,  0.581234022404097f,                0.0f,
    -0.760845213036861f,  0.179611190632978f,  0.552786404500000f,
    -0.760845213037948f, -0.470228201835026f,  0.341640786498800f,
    -0.760845213037948f, -0.470228201835026f, -0.341640786498800f,
};

/* Vertex indices */
static GLubyte icosahedron_vi[ICOSAHEDRON_VERT_PER_OBJ] =
{
    0,   1,  2 ,
    0,   2,  3 ,
    0,   3,  4 ,
    0,   4,  5 ,
    0,   5,  1 ,
    1,   8,  2 ,
    2,   7,  3 ,
    3,   6,  4 ,
    4,  10,  5 ,
    5,   9,  1 ,
    1,   9,  8 ,
    2,   8,  7 ,
    3,   7,  6 ,
    4,   6, 10 ,
    5,  10,  9 ,
    11,  9, 10 ,
    11,  8,  9 ,
    11,  7,  8 ,
    11,  6,  7 ,
    11, 10,  6 
};
DECLARE_SHAPE_CACHE(icosahedron,Icosahedron,ICOSAHEDRON)

/* -- Octahedron -- */
#define OCTAHEDRON_NUM_VERT           6
#define OCTAHEDRON_NUM_FACES          8
#define OCTAHEDRON_NUM_EDGE_PER_FACE  3
#define OCTAHEDRON_VERT_PER_OBJ       (OCTAHEDRON_NUM_FACES*OCTAHEDRON_NUM_EDGE_PER_FACE)
#define OCTAHEDRON_VERT_ELEM_PER_OBJ  (OCTAHEDRON_VERT_PER_OBJ*3)
#define OCTAHEDRON_VERT_PER_OBJ_TRI   OCTAHEDRON_VERT_PER_OBJ

/* Vertex Coordinates */
static GLfloat octahedron_v[OCTAHEDRON_NUM_VERT*3] =
{
     1.f,  0.f,  0.f,
     0.f,  1.f,  0.f,
     0.f,  0.f,  1.f,
    -1.f,  0.f,  0.f,
     0.f, -1.f,  0.f,
     0.f,  0.f, -1.f,

};
/* Normal Vectors */
static GLfloat octahedron_n[OCTAHEDRON_NUM_FACES*3] =
{
     0.577350269189f, 0.577350269189f, 0.577350269189f,    /* sqrt(1/3) */
     0.577350269189f, 0.577350269189f,-0.577350269189f,
     0.577350269189f,-0.577350269189f, 0.577350269189f,
     0.577350269189f,-0.577350269189f,-0.577350269189f,
    -0.577350269189f, 0.577350269189f, 0.577350269189f,
    -0.577350269189f, 0.577350269189f,-0.577350269189f,
    -0.577350269189f,-0.577350269189f, 0.577350269189f,
    -0.577350269189f,-0.577350269189f,-0.577350269189f

};

/* Vertex indices */
static GLubyte octahedron_vi[OCTAHEDRON_VERT_PER_OBJ] =
{
    0, 1, 2,
    0, 5, 1,
    0, 2, 4,
    0, 4, 5,
    3, 2, 1,
    3, 1, 5,
    3, 4, 2,
    3, 5, 4
};
DECLARE_SHAPE_CACHE(octahedron,Octahedron,OCTAHEDRON)

/* -- RhombicDodecahedron -- */
#define RHOMBICDODECAHEDRON_NUM_VERT            14
#define RHOMBICDODECAHEDRON_NUM_FACES           12
#define RHOMBICDODECAHEDRON_NUM_EDGE_PER_FACE   4
#define RHOMBICDODECAHEDRON_VERT_PER_OBJ       (RHOMBICDODECAHEDRON_NUM_FACES*RHOMBICDODECAHEDRON_NUM_EDGE_PER_FACE)
#define RHOMBICDODECAHEDRON_VERT_ELEM_PER_OBJ  (RHOMBICDODECAHEDRON_VERT_PER_OBJ*3)
#define RHOMBICDODECAHEDRON_VERT_PER_OBJ_TRI   (RHOMBICDODECAHEDRON_VERT_PER_OBJ+RHOMBICDODECAHEDRON_NUM_FACES*2)    /* 2 extra edges per face when drawing quads as triangles */

/* Vertex Coordinates */
static GLfloat rhombicdodecahedron_v[RHOMBICDODECAHEDRON_NUM_VERT*3] =
{
                0.0f,             0.0f,  1.0f,
     0.707106781187f,             0.0f,  0.5f,
                0.0f,  0.707106781187f,  0.5f,
    -0.707106781187f,             0.0f,  0.5f,
                0.0f, -0.707106781187f,  0.5f,
     0.707106781187f,  0.707106781187f,  0.0f,
    -0.707106781187f,  0.707106781187f,  0.0f,
    -0.707106781187f, -0.707106781187f,  0.0f,
     0.707106781187f, -0.707106781187f,  0.0f,
     0.707106781187f,             0.0f, -0.5f,
                0.0f,  0.707106781187f, -0.5f,
    -0.707106781187f,             0.0f, -0.5f,
                0.0f, -0.707106781187f, -0.5f,
                0.0f,             0.0f, -1.0f
};
/* Normal Vectors */
static GLfloat rhombicdodecahedron_n[RHOMBICDODECAHEDRON_NUM_FACES*3] =
{
     0.353553390594f,  0.353553390594f,  0.5f,
    -0.353553390594f,  0.353553390594f,  0.5f,
    -0.353553390594f, -0.353553390594f,  0.5f,
     0.353553390594f, -0.353553390594f,  0.5f,
                0.0f,             1.0f,  0.0f,
    -           1.0f,             0.0f,  0.0f,
                0.0f, -           1.0f,  0.0f,
                1.0f,             0.0f,  0.0f,
     0.353553390594f,  0.353553390594f, -0.5f,
    -0.353553390594f,  0.353553390594f, -0.5f,
    -0.353553390594f, -0.353553390594f, -0.5f,
     0.353553390594f, -0.353553390594f, -0.5f
};

/* Vertex indices */
static GLubyte rhombicdodecahedron_vi[RHOMBICDODECAHEDRON_VERT_PER_OBJ] =
{
    0,  1,  5,  2,
    0,  2,  6,  3,
    0,  3,  7,  4,
    0,  4,  8,  1,
    5, 10,  6,  2,
    6, 11,  7,  3,
    7, 12,  8,  4,
    8,  9,  5,  1,
    5,  9, 13, 10,
    6, 10, 13, 11,
    7, 11, 13, 12,
    8, 12, 13,  9
};
DECLARE_SHAPE_CACHE_DECOMPOSE_TO_TRIANGLE(rhombicdodecahedron,RhombicDodecahedron,RHOMBICDODECAHEDRON)

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
#define TETRAHEDRON_NUM_VERT            4
#define TETRAHEDRON_NUM_FACES           4
#define TETRAHEDRON_NUM_EDGE_PER_FACE   3
#define TETRAHEDRON_VERT_PER_OBJ        (TETRAHEDRON_NUM_FACES*TETRAHEDRON_NUM_EDGE_PER_FACE)
#define TETRAHEDRON_VERT_ELEM_PER_OBJ   (TETRAHEDRON_VERT_PER_OBJ*3)
#define TETRAHEDRON_VERT_PER_OBJ_TRI    TETRAHEDRON_VERT_PER_OBJ

/* Vertex Coordinates */
static GLfloat tetrahedron_v[TETRAHEDRON_NUM_VERT*3] =
{
                1.0f,             0.0f,             0.0f,
    -0.333333333333f,  0.942809041582f,             0.0f,
    -0.333333333333f, -0.471404520791f,  0.816496580928f,
    -0.333333333333f, -0.471404520791f, -0.816496580928f
};
/* Normal Vectors */
static GLfloat tetrahedron_n[TETRAHEDRON_NUM_FACES*3] =
{
    -           1.0f,             0.0f,             0.0f,
     0.333333333333f, -0.942809041582f,             0.0f,
     0.333333333333f,  0.471404520791f, -0.816496580928f,
     0.333333333333f,  0.471404520791f,  0.816496580928f
};

/* Vertex indices */
static GLubyte tetrahedron_vi[TETRAHEDRON_VERT_PER_OBJ] =
{
    1, 3, 2,
    0, 2, 3,
    0, 3, 1,
    0, 1, 2
};
DECLARE_SHAPE_CACHE(tetrahedron,Tetrahedron,TETRAHEDRON)

/* -- Sierpinski Sponge -- */
static unsigned int ipow (int x, unsigned int y)
{
    /* return y==0? 1: y==1? x: (y%2? x: 1) * ipow(x*x, y/2); */
    if (y==0)
        return 1;
    else
    {
        if (y==1)
            return x;
        else
        {
            return (y%2? x: 1) * ipow(x*x, y/2);
        }
    }
}

static void fghSierpinskiSpongeGenerate ( int numLevels, double offset[3], GLfloat scale, GLfloat* vertices, GLfloat* normals )
{
    int i, j;
    if ( numLevels == 0 )
    {
        for (i=0; i<TETRAHEDRON_NUM_FACES; i++)
        {
            int normIdx         = i*3;
            int faceIdxVertIdx  = i*TETRAHEDRON_NUM_EDGE_PER_FACE;
            for (j=0; j<TETRAHEDRON_NUM_EDGE_PER_FACE; j++)
            {
                int outIdx  = i*TETRAHEDRON_NUM_EDGE_PER_FACE*3+j*3;
                int vertIdx = tetrahedron_vi[faceIdxVertIdx+j]*3;

                vertices[outIdx  ] = (GLfloat)offset[0] + scale * tetrahedron_v[vertIdx  ];
                vertices[outIdx+1] = (GLfloat)offset[1] + scale * tetrahedron_v[vertIdx+1];
                vertices[outIdx+2] = (GLfloat)offset[2] + scale * tetrahedron_v[vertIdx+2];

                normals [outIdx  ] = tetrahedron_n[normIdx  ];
                normals [outIdx+1] = tetrahedron_n[normIdx+1];
                normals [outIdx+2] = tetrahedron_n[normIdx+2];
            }
        }
    }
    else if ( numLevels > 0 )
    {
        double local_offset[3] ;    /* Use a local variable to avoid buildup of roundoff errors */
        unsigned int stride = ipow(4,--numLevels)*TETRAHEDRON_VERT_ELEM_PER_OBJ;
        scale /= 2.0 ;
        for ( i = 0 ; i < TETRAHEDRON_NUM_FACES ; i++ )
        {
            int idx         = i*3;
            local_offset[0] = offset[0] + scale * tetrahedron_v[idx  ];
            local_offset[1] = offset[1] + scale * tetrahedron_v[idx+1];
            local_offset[2] = offset[2] + scale * tetrahedron_v[idx+2];
            fghSierpinskiSpongeGenerate ( numLevels, local_offset, scale, vertices+i*stride, normals+i*stride );
        }
    }
}

/* -- Now the various non-polyhedra (shapes involving circles) -- */
/*
 * Compute lookup table of cos and sin values forming a circle
 * (or half circle if halfCircle==TRUE)
 *
 * Notes:
 *    It is the responsibility of the caller to free these tables
 *    The size of the table is (n+1) to form a connected loop
 *    The last entry is exactly the same as the first
 *    The sign of n can be flipped to get the reverse loop
 */
static void fghCircleTable(GLfloat **sint, GLfloat **cost, const int n, const GLboolean halfCircle)
{
    int i;
    
    /* Table size, the sign of n flips the circle direction */
    const int size = abs(n);

    /* Determine the angle between samples */
    const GLfloat angle = (halfCircle?1:2)*(GLfloat)M_PI/(GLfloat)( ( n == 0 ) ? 1 : n );

    /* Allocate memory for n samples, plus duplicate of first entry at the end */
    *sint = (GLfloat*)calloc(1, sizeof(GLfloat) * (size+1));
    *cost = (GLfloat*)calloc(1, sizeof(GLfloat) * (size+1));

    /* Bail out if memory allocation fails, fgError never returns */
    if (!(*sint) || !(*cost))
    {
        free(*sint);
        free(*cost);
        *sint=NULL;
        *cost=NULL;
        fgError("Failed to allocate memory in fghCircleTable");
        return;
    }

    /* Compute cos and sin around the circle */
    (*sint)[0] = 0.0;
    (*cost)[0] = 1.0;

    for (i=1; i<size; i++)
    {
        (*sint)[i] = (GLfloat)sin(angle*i);
        (*cost)[i] = (GLfloat)cos(angle*i);
    }

    
    if (halfCircle)
    {
        (*sint)[size] =  0.0f;  /* sin PI */
        (*cost)[size] = -1.0f;  /* cos PI */
    }
    else
    {
        /* Last sample is duplicate of the first (sin or cos of 2 PI) */
        (*sint)[size] = (*sint)[0];
        (*cost)[size] = (*cost)[0];
    }
}

static void fghGenerateSphere(GLfloat radius, GLint slices, GLint stacks, GLfloat **vertices, GLfloat **normals, int* nVert)
{
    int i,j;
    int idx = 0;    /* idx into vertex/normal buffer */
    GLfloat x,y,z;

    /* Pre-computed circle */
    GLfloat *sint1,*cost1;
    GLfloat *sint2,*cost2;

    /* number of unique vertices */
    if (slices==0 || stacks<2)
    {
        /* nothing to generate */
        *nVert = 0;
        return;
    }
    *nVert = slices*(stacks-1)+2;
    if ((*nVert) > 65535)
        /*
         * limit of glushort, thats 256*256 subdivisions, should be enough in practice. See note above
         */
        fgWarning("fghGenerateSphere: too many slices or stacks requested, indices will wrap");

    /* precompute values on unit circle */
    fghCircleTable(&sint1,&cost1,-slices,GL_FALSE);
    fghCircleTable(&sint2,&cost2, stacks,GL_TRUE);

    /* Allocate vertex and normal buffers, bail out if memory allocation fails */
    *vertices = (GLfloat*)malloc((*nVert)*3*sizeof(GLfloat));
    *normals  = (GLfloat*)malloc((*nVert)*3*sizeof(GLfloat));
    if (!(*vertices) || !(*normals))
    {
        free(*vertices);
        free(*normals);
        *vertices = NULL;
        *normals = NULL;
        fgError("Failed to allocate memory in fghGenerateSphere");
        return;
    }

    /* top */
    (*vertices)[0] = 0.f;
    (*vertices)[1] = 0.f;
    (*vertices)[2] = radius;
    (*normals )[0] = 0.f;
    (*normals )[1] = 0.f;
    (*normals )[2] = 1.f;
    idx = 3;

    /* each stack */
    for( i=1; i<stacks; i++ )
    {
        for(j=0; j<slices; j++, idx+=3)
        {
            x = cost1[j]*sint2[i];
            y = sint1[j]*sint2[i];
            z = cost2[i];

            (*vertices)[idx  ] = x*radius;
            (*vertices)[idx+1] = y*radius;
            (*vertices)[idx+2] = z*radius;
            (*normals )[idx  ] = x;
            (*normals )[idx+1] = y;
            (*normals )[idx+2] = z;
        }
    }

    /* bottom */
    (*vertices)[idx  ] =  0.f;
    (*vertices)[idx+1] =  0.f;
    (*vertices)[idx+2] = -radius;
    (*normals )[idx  ] =  0.f;
    (*normals )[idx+1] =  0.f;
    (*normals )[idx+2] = -1.f;

    /* Done creating vertices, release sin and cos tables */
    free(sint1);
    free(cost1);
    free(sint2);
    free(cost2);
}

void fghGenerateCone(
    GLfloat base, GLfloat height, GLint slices, GLint stacks,   /*  input */
    GLfloat **vertices, GLfloat **normals, int* nVert           /* output */
    )
{
    int i,j;
    int idx = 0;    /* idx into vertex/normal buffer */

    /* Pre-computed circle */
    GLfloat *sint,*cost;

    /* Step in z and radius as stacks are drawn. */
    GLfloat z = 0;
    GLfloat r = (GLfloat)base;

    const GLfloat zStep = (GLfloat)height / ( ( stacks > 0 ) ? stacks : 1 );
    const GLfloat rStep = (GLfloat)base / ( ( stacks > 0 ) ? stacks : 1 );

    /* Scaling factors for vertex normals */
    const GLfloat cosn = (GLfloat) (height / sqrt( height * height + base * base ));
    const GLfloat sinn = (GLfloat) (base   / sqrt( height * height + base * base ));



    /* number of unique vertices */
    if (slices==0 || stacks<1)
    {
        /* nothing to generate */
        *nVert = 0;
        *vertices = NULL;
        *normals = NULL;
        return;
    }
    *nVert = slices*(stacks+2)+1;   /* need an extra stack for closing off bottom with correct normals */

    if ((*nVert) > 65535)
        /*
         * limit of glushort, thats 256*256 subdivisions, should be enough in practice. See note above
         */
        fgWarning("fghGenerateCone: too many slices or stacks requested, indices will wrap");

    /* Pre-computed circle */
    fghCircleTable(&sint,&cost,-slices,GL_FALSE);

    /* Allocate vertex and normal buffers, bail out if memory allocation fails */
    *vertices = (GLfloat*)malloc((*nVert)*3*sizeof(GLfloat));
    *normals  = (GLfloat*)malloc((*nVert)*3*sizeof(GLfloat));
    if (!(*vertices) || !(*normals))
    {
        free(*vertices);
        free(*normals);
        *vertices = NULL;
        *normals = NULL;
        fgError("Failed to allocate memory in fghGenerateCone");
        return;
    }

    /* bottom */
    (*vertices)[0] =  0.f;
    (*vertices)[1] =  0.f;
    (*vertices)[2] =  z;
    (*normals )[0] =  0.f;
    (*normals )[1] =  0.f;
    (*normals )[2] = -1.f;
    idx = 3;
    /* other on bottom (get normals right) */
    for (j=0; j<slices; j++, idx+=3)
    {
        (*vertices)[idx  ] = cost[j]*r;
        (*vertices)[idx+1] = sint[j]*r;
        (*vertices)[idx+2] = z;
        (*normals )[idx  ] =  0.f;
        (*normals )[idx+1] =  0.f;
        (*normals )[idx+2] = -1.f;
    }

    /* each stack */
    for (i=0; i<stacks+1; i++ )
    {
        for (j=0; j<slices; j++, idx+=3)
        {
            (*vertices)[idx  ] = cost[j]*r;
            (*vertices)[idx+1] = sint[j]*r;
            (*vertices)[idx+2] = z;
            (*normals )[idx  ] = cost[j]*cosn;
            (*normals )[idx+1] = sint[j]*cosn;
            (*normals )[idx+2] = sinn;
        }

        z += zStep;
        r -= rStep;
    }

    /* Release sin and cos tables */
    free(sint);
    free(cost);
}

void fghGenerateCylinder(
    GLfloat radius, GLfloat height, GLint slices, GLint stacks, /*  input */
    GLfloat **vertices, GLfloat **normals, int* nVert           /* output */
    )
{
    int i,j;
    int idx = 0;    /* idx into vertex/normal buffer */

    /* Step in z as stacks are drawn. */
    GLfloat radf = (GLfloat)radius;
    GLfloat z;
    const GLfloat zStep = (GLfloat)height / ( ( stacks > 0 ) ? stacks : 1 );

    /* Pre-computed circle */
    GLfloat *sint,*cost;

    /* number of unique vertices */
    if (slices==0 || stacks<1)
    {
        /* nothing to generate */
        *nVert = 0;
        *vertices = NULL;
        *normals = NULL;
        return;
    }
    *nVert = slices*(stacks+3)+2;   /* need two extra stacks for closing off top and bottom with correct normals */

    if ((*nVert) > 65535)
        /*
         * limit of glushort, thats 256*256 subdivisions, should be enough in practice. See note above
         */
        fgWarning("fghGenerateCylinder: too many slices or stacks requested, indices will wrap");

    /* Pre-computed circle */
    fghCircleTable(&sint,&cost,-slices,GL_FALSE);

    /* Allocate vertex and normal buffers, bail out if memory allocation fails */
    *vertices = (GLfloat*)malloc((*nVert)*3*sizeof(GLfloat));
    *normals  = (GLfloat*)malloc((*nVert)*3*sizeof(GLfloat));
    if (!(*vertices) || !(*normals))
    {
        free(*vertices);
        free(*normals);
        *vertices = NULL;
        *normals = NULL;
        fgError("Failed to allocate memory in fghGenerateCylinder");
        return;
    }

    z=0;
    /* top on Z-axis */
    (*vertices)[0] =  0.f;
    (*vertices)[1] =  0.f;
    (*vertices)[2] =  0.f;
    (*normals )[0] =  0.f;
    (*normals )[1] =  0.f;
    (*normals )[2] = -1.f;
    idx = 3;
    /* other on top (get normals right) */
    for (j=0; j<slices; j++, idx+=3)
    {
        (*vertices)[idx  ] = cost[j]*radf;
        (*vertices)[idx+1] = sint[j]*radf;
        (*vertices)[idx+2] = z;
        (*normals )[idx  ] = 0.f;
        (*normals )[idx+1] = 0.f;
        (*normals )[idx+2] = -1.f;
    }

    /* each stack */
    for (i=0; i<stacks+1; i++ )
    {
        for (j=0; j<slices; j++, idx+=3)
        {
            (*vertices)[idx  ] = cost[j]*radf;
            (*vertices)[idx+1] = sint[j]*radf;
            (*vertices)[idx+2] = z;
            (*normals )[idx  ] = cost[j];
            (*normals )[idx+1] = sint[j];
            (*normals )[idx+2] = 0.f;
        }

        z += zStep;
    }

    /* other on bottom (get normals right) */
    z -= zStep;
    for (j=0; j<slices; j++, idx+=3)
    {
        (*vertices)[idx  ] = cost[j]*radf;
        (*vertices)[idx+1] = sint[j]*radf;
        (*vertices)[idx+2] = z;
        (*normals )[idx  ] = 0.f;
        (*normals )[idx+1] = 0.f;
        (*normals )[idx+2] = 1.f;
    }

    /* bottom */
    (*vertices)[idx  ] =  0.f;
    (*vertices)[idx+1] =  0.f;
    (*vertices)[idx+2] =  height;
    (*normals )[idx  ] =  0.f;
    (*normals )[idx+1] =  0.f;
    (*normals )[idx+2] =  1.f;

    /* Release sin and cos tables */
    free(sint);
    free(cost);
}

void fghGenerateTorus(
    double dInnerRadius, double dOuterRadius, GLint nSides, GLint nRings, /*  input */
    GLfloat **vertices, GLfloat **normals, int* nVert                     /* output */
    )
{
    GLfloat  iradius = (float)dInnerRadius;
    GLfloat  oradius = (float)dOuterRadius;
    int    i, j;

    /* Pre-computed circle */
    GLfloat *spsi, *cpsi;
    GLfloat *sphi, *cphi;

    /* number of unique vertices */
    if (nSides<2 || nRings<2)
    {
        /* nothing to generate */
        *nVert = 0;
        return;
    }
    *nVert = nSides * nRings;

    if ((*nVert) > 65535)
        /*
         * limit of glushort, thats 256*256 subdivisions, should be enough in practice. See note above
         */
        fgWarning("fghGenerateTorus: too many slices or stacks requested, indices will wrap");

    /* precompute values on unit circle */
    fghCircleTable(&spsi,&cpsi, nRings,GL_FALSE);
    fghCircleTable(&sphi,&cphi,-nSides,GL_FALSE);

    /* Allocate vertex and normal buffers, bail out if memory allocation fails */
    *vertices = (GLfloat*)malloc((*nVert)*3*sizeof(GLfloat));
    *normals  = (GLfloat*)malloc((*nVert)*3*sizeof(GLfloat));
    if (!(*vertices) || !(*normals) || !(spsi && cpsi && sphi && cphi))
    {
        free(spsi);
        free(cpsi);
        free(sphi);
        free(cphi);
        free(*vertices);
        free(*normals);
        fgError("Failed to allocate memory in fghGenerateTorus");
        return;
    }

    for( j=0; j<nRings; j++ )
    {
        for( i=0; i<nSides; i++ )
        {
            int offset = 3 * ( j * nSides + i ) ;

            (*vertices)[offset  ] = cpsi[j] * ( oradius + cphi[i] * iradius ) ;
            (*vertices)[offset+1] = spsi[j] * ( oradius + cphi[i] * iradius ) ;
            (*vertices)[offset+2] =                       sphi[i] * iradius  ;
            (*normals )[offset  ] = cpsi[j] * cphi[i] ;
            (*normals )[offset+1] = spsi[j] * cphi[i] ;
            (*normals )[offset+2] =           sphi[i] ;
        }
    }

    /* Release sin and cos tables */
    free(spsi);
    free(cpsi);
    free(sphi);
    free(cphi);
}

/* -- INTERNAL DRAWING functions --------------------------------------- */
#define _DECLARE_INTERNAL_DRAW_DO_DECLARE(name,nameICaps,nameCaps,vertIdxs)\
    static void fgh##nameICaps( GLboolean useWireMode )\
    {\
        if (!name##Cached)\
        {\
            fgh##nameICaps##Generate();\
            name##Cached = GL_TRUE;\
        }\
        \
        if (useWireMode)\
        {\
            fghDrawGeometryWire (name##_verts,name##_norms,nameCaps##_VERT_PER_OBJ, \
                                 NULL,nameCaps##_NUM_FACES,nameCaps##_NUM_EDGE_PER_FACE,GL_LINE_LOOP,\
                                 NULL,0,0);\
        }\
        else\
        {\
            fghDrawGeometrySolid(name##_verts,name##_norms,NULL,nameCaps##_VERT_PER_OBJ,\
                                 vertIdxs, 1, nameCaps##_VERT_PER_OBJ_TRI); \
        }\
    }
#define DECLARE_INTERNAL_DRAW(name,nameICaps,nameCaps)                        _DECLARE_INTERNAL_DRAW_DO_DECLARE(name,nameICaps,nameCaps,NULL)
#define DECLARE_INTERNAL_DRAW_DECOMPOSED_TO_TRIANGLE(name,nameICaps,nameCaps) _DECLARE_INTERNAL_DRAW_DO_DECLARE(name,nameICaps,nameCaps,name##_vertIdxs)

static void fghCube( GLfloat dSize, GLboolean useWireMode )
{
    GLfloat *vertices;

    if (!cubeCached)
    {
        fghCubeGenerate();
        cubeCached = GL_TRUE;
    }

    if (dSize!=1.f)
    {
        /* Need to build new vertex list containing vertices for cube of different size */
        int i;

        vertices = (GLfloat*)malloc(CUBE_VERT_ELEM_PER_OBJ * sizeof(GLfloat));

        /* Bail out if memory allocation fails, fgError never returns */
        if (!vertices)
        {
            free(vertices);
            fgError("Failed to allocate memory in fghCube");
            return;
        }

        for (i=0; i<CUBE_VERT_ELEM_PER_OBJ; i++)
            vertices[i] = dSize*cube_verts[i];
    }
    else
        vertices = cube_verts;

    if (useWireMode)
        fghDrawGeometryWire(vertices, cube_norms, CUBE_VERT_PER_OBJ,
                            NULL,CUBE_NUM_FACES, CUBE_NUM_EDGE_PER_FACE,GL_LINE_LOOP,
                            NULL,0,0);
    else
        fghDrawGeometrySolid(vertices, cube_norms, NULL, CUBE_VERT_PER_OBJ,
                             cube_vertIdxs, 1, CUBE_VERT_PER_OBJ_TRI);

    if (dSize!=1.f && vertices != cube_verts)
        /* cleanup allocated memory */
        free(vertices);
}

DECLARE_INTERNAL_DRAW_DECOMPOSED_TO_TRIANGLE(dodecahedron,Dodecahedron,DODECAHEDRON)
DECLARE_INTERNAL_DRAW(icosahedron,Icosahedron,ICOSAHEDRON)
DECLARE_INTERNAL_DRAW(octahedron,Octahedron,OCTAHEDRON)
DECLARE_INTERNAL_DRAW_DECOMPOSED_TO_TRIANGLE(rhombicdodecahedron,RhombicDodecahedron,RHOMBICDODECAHEDRON)
DECLARE_INTERNAL_DRAW(tetrahedron,Tetrahedron,TETRAHEDRON)

static void fghSierpinskiSponge ( int numLevels, double offset[3], GLfloat scale, GLboolean useWireMode )
{
    GLfloat *vertices;
    GLfloat * normals;
    GLsizei    numTetr = numLevels<0? 0 : ipow(4,numLevels); /* No sponge for numLevels below 0 */
    GLsizei    numVert = numTetr*TETRAHEDRON_VERT_PER_OBJ;
    GLsizei    numFace = numTetr*TETRAHEDRON_NUM_FACES;

    if (numTetr)
    {
        /* Allocate memory */
        vertices = (GLfloat*)malloc(numVert*3 * sizeof(GLfloat));
        normals  = (GLfloat*)malloc(numVert*3 * sizeof(GLfloat));
        /* Bail out if memory allocation fails, fgError never returns */
        if (!vertices || !normals)
        {
            free(vertices);
            free(normals);
            fgError("Failed to allocate memory in fghSierpinskiSponge");
            return;
        }

        /* Generate elements */
        fghSierpinskiSpongeGenerate ( numLevels, offset, scale, vertices, normals );

        /* Draw and cleanup */
        if (useWireMode)
            fghDrawGeometryWire (vertices,normals,numVert,
                                 NULL,numFace,TETRAHEDRON_NUM_EDGE_PER_FACE,GL_LINE_LOOP,
                                 NULL,0,0);
        else
            fghDrawGeometrySolid(vertices,normals,NULL,numVert,NULL,1,0);

        free(vertices);
        free(normals );
    }
}


static void fghSphere( GLfloat radius, GLint slices, GLint stacks, GLboolean useWireMode )
{
    int i,j,idx, nVert;
    GLfloat *vertices = NULL, *normals = NULL;

    /* Generate vertices and normals */
    fghGenerateSphere(radius,slices,stacks,&vertices,&normals,&nVert);
    
    if (nVert==0 || !normals || !vertices) {
        if (normals) free(normals);
        if (vertices) free(vertices);
        /* nothing to draw */
        return;
    }

    if (useWireMode)
    {
        GLushort  *sliceIdx, *stackIdx;
        /* First, generate vertex index arrays for drawing with glDrawElements
         * We have a bunch of line_loops to draw for each stack, and a
         * bunch for each slice.
         */

        sliceIdx = (GLushort*)malloc(slices*(stacks+1)*sizeof(GLushort));
        stackIdx = (GLushort*)malloc(slices*(stacks-1)*sizeof(GLushort));
        if (!(stackIdx) || !(sliceIdx))
        {
            free(normals);
            free(vertices);
            free(stackIdx);
            free(sliceIdx);
            fgError("Failed to allocate memory in fghSphere");
            return;
        }

        /* generate for each stack */
        for (i=0,idx=0; i<stacks-1; i++)
        {
            GLushort offset = (GLushort)(1+i*slices);           /* start at 1 (0 is top vertex), and we advance one stack down as we go along */
            for (j=0; j<slices; j++, idx++)
            {
                stackIdx[idx] = (GLushort)(offset+j);
            }
        }

        /* generate for each slice */
        for (i=0,idx=0; i<slices; i++)
        {
            GLushort offset = (GLushort)(1+i);                  /* start at 1 (0 is top vertex), and we advance one slice as we go along */
            sliceIdx[idx++] = 0;                    /* vertex on top */
            for (j=0; j<stacks-1; j++, idx++)
            {
                sliceIdx[idx] = (GLushort)(offset+j*slices);
            }
            sliceIdx[idx++] = (GLushort)(nVert-1);              /* zero based index, last element in array... */
        }

        /* draw */
        fghDrawGeometryWire(vertices,normals,nVert,
            sliceIdx,slices,stacks+1,GL_LINE_STRIP,
            stackIdx,stacks-1,slices);
        
        /* cleanup allocated memory */
        free(sliceIdx);
        free(stackIdx);
    }
    else
    {
        /* First, generate vertex index arrays for drawing with glDrawElements
         * All stacks, including top and bottom are covered with a triangle
         * strip.
         */
        GLushort  *stripIdx;
        /* Create index vector */
        GLushort offset;

        /* Allocate buffers for indices, bail out if memory allocation fails */
        stripIdx = (GLushort*)malloc((slices+1)*2*(stacks)*sizeof(GLushort));
        if (!(stripIdx))
        {
            free(stripIdx);
            fgError("Failed to allocate memory in fghSphere");
        }

        /* top stack */
        for (j=0, idx=0;  j<slices;  j++, idx+=2)
        {
            stripIdx[idx  ] = (GLushort)(j+1);              /* 0 is top vertex, 1 is first for first stack */
            stripIdx[idx+1] = 0;
        }
        stripIdx[idx  ] = 1;                    /* repeat first slice's idx for closing off shape */
        stripIdx[idx+1] = 0;
        idx+=2;

        /* middle stacks: */
        /* Strip indices are relative to first index belonging to strip, NOT relative to first vertex/normal pair in array */
        for (i=0; i<stacks-2; i++, idx+=2)
        {
            offset = (GLushort)(1+i*slices);                    /* triangle_strip indices start at 1 (0 is top vertex), and we advance one stack down as we go along */
            for (j=0; j<slices; j++, idx+=2)
            {
                stripIdx[idx  ] = (GLushort)(offset+j+slices);
                stripIdx[idx+1] = (GLushort)(offset+j);
            }
            stripIdx[idx  ] = (GLushort)(offset+slices);        /* repeat first slice's idx for closing off shape */
            stripIdx[idx+1] = offset;
        }

        /* bottom stack */
        offset = (GLushort)(1+(stacks-2)*slices);               /* triangle_strip indices start at 1 (0 is top vertex), and we advance one stack down as we go along */
        for (j=0; j<slices; j++, idx+=2)
        {
            stripIdx[idx  ] = (GLushort)(nVert-1);              /* zero based index, last element in array (bottom vertex)... */
            stripIdx[idx+1] = (GLushort)(offset+j);
        }
        stripIdx[idx  ] = (GLushort)(nVert-1);                  /* repeat first slice's idx for closing off shape */
        stripIdx[idx+1] = offset;


        /* draw */
        fghDrawGeometrySolid(vertices,normals,NULL,nVert,stripIdx,stacks,(slices+1)*2);

        /* cleanup allocated memory */
        free(stripIdx);
    }
    
    /* cleanup allocated memory */
    free(vertices);
    free(normals);
}

static void fghCone( GLfloat base, GLfloat height, GLint slices, GLint stacks, GLboolean useWireMode )
{
    int i,j,idx, nVert;
    GLfloat *vertices, *normals;

    /* Generate vertices and normals */
    /* Note, (stacks+1)*slices vertices for side of object, slices+1 for top and bottom closures */
    fghGenerateCone(base,height,slices,stacks,&vertices,&normals,&nVert);

    if (nVert==0) {
        free(normals);
        free(vertices);
        /* nothing to draw */
        return;
    }

    if (useWireMode)
    {
        GLushort  *sliceIdx, *stackIdx;
        /* First, generate vertex index arrays for drawing with glDrawElements
         * We have a bunch of line_loops to draw for each stack, and a
         * bunch for each slice.
         */

        stackIdx = (GLushort*)malloc(slices*stacks*sizeof(GLushort));
        sliceIdx = (GLushort*)malloc(slices*2     *sizeof(GLushort));
        if (!(stackIdx) || !(sliceIdx))
        {
            free(normals);
            free(vertices);
            free(stackIdx);
            free(sliceIdx);
            fgError("Failed to allocate memory in fghCone");
            return;
        }

        /* generate for each stack */
        for (i=0,idx=0; i<stacks; i++)
        {
            GLushort offset = (GLushort)(1+(i+1)*slices);       /* start at 1 (0 is top vertex), and we advance one stack down as we go along */
            for (j=0; j<slices; j++, idx++)
            {
                stackIdx[idx] = (GLushort)(offset+j);
            }
        }

        /* generate for each slice */
        for (i=0,idx=0; i<slices; i++)
        {
            GLushort offset = (GLushort)(1+i);                  /* start at 1 (0 is top vertex), and we advance one slice as we go along */
            sliceIdx[idx++] = (GLushort)(offset+slices);
            sliceIdx[idx++] = (GLushort)(offset+(stacks+1)*slices);
        }

        /* draw */
        fghDrawGeometryWire(vertices,normals,nVert,
            sliceIdx,1,slices*2,GL_LINES,
            stackIdx,stacks,slices);

        /* cleanup allocated memory */
        free(sliceIdx);
        free(stackIdx);
    }
    else
    {
        /* First, generate vertex index arrays for drawing with glDrawElements
         * All stacks, including top and bottom are covered with a triangle
         * strip.
         */
        GLushort  *stripIdx;
        /* Create index vector */
        GLushort offset;

        /* Allocate buffers for indices, bail out if memory allocation fails */
        stripIdx = (GLushort*)malloc((slices+1)*2*(stacks+1)*sizeof(GLushort));    /*stacks +1 because of closing off bottom */
        if (!(stripIdx))
        {
            free(stripIdx);
            fgError("Failed to allocate memory in fghCone");
        }

        /* top stack */
        for (j=0, idx=0;  j<slices;  j++, idx+=2)
        {
            stripIdx[idx  ] = 0;
            stripIdx[idx+1] = (GLushort)(j+1);              /* 0 is top vertex, 1 is first for first stack */
        }
        stripIdx[idx  ] = 0;                    /* repeat first slice's idx for closing off shape */
        stripIdx[idx+1] = 1;
        idx+=2;

        /* middle stacks: */
        /* Strip indices are relative to first index belonging to strip, NOT relative to first vertex/normal pair in array */
        for (i=0; i<stacks; i++, idx+=2)
        {
            offset = (GLushort)(1+(i+1)*slices);                /* triangle_strip indices start at 1 (0 is top vertex), and we advance one stack down as we go along */
            for (j=0; j<slices; j++, idx+=2)
            {
                stripIdx[idx  ] = (GLushort)(offset+j);
                stripIdx[idx+1] = (GLushort)(offset+j+slices);
            }
            stripIdx[idx  ] = offset;               /* repeat first slice's idx for closing off shape */
            stripIdx[idx+1] = (GLushort)(offset+slices);
        }

        /* draw */
        fghDrawGeometrySolid(vertices,normals,NULL,nVert,stripIdx,stacks+1,(slices+1)*2);

        /* cleanup allocated memory */
        free(stripIdx);
    }

    /* cleanup allocated memory */
    free(vertices);
    free(normals);
}

static void fghCylinder( GLfloat radius, GLfloat height, GLint slices, GLint stacks, GLboolean useWireMode )
{
    int i,j,idx, nVert;
    GLfloat *vertices, *normals;

    /* Generate vertices and normals */
    /* Note, (stacks+1)*slices vertices for side of object, 2*slices+2 for top and bottom closures */
    fghGenerateCylinder(radius,height,slices,stacks,&vertices,&normals,&nVert);

    if (nVert==0)
    {
        /* nothing to draw */
        free(vertices);
        free(normals);
        return;
    }

    if (useWireMode)
    {
        GLushort  *sliceIdx, *stackIdx;
        /* First, generate vertex index arrays for drawing with glDrawElements
         * We have a bunch of line_loops to draw for each stack, and a
         * bunch for each slice.
         */
        GLint allocSize = slices*(stacks+1)*sizeof(GLushort);
        stackIdx = (GLushort*)(allocSize > 0 ? malloc(allocSize) : NULL);
        sliceIdx = (GLushort*)(slices > 0 ? malloc(slices*2*sizeof(GLushort)) : NULL);
        if (!(stackIdx) || !(sliceIdx))
        {
            free(vertices);
            free(normals);
            free(stackIdx);
            free(sliceIdx);
            fgError("Failed to allocate memory in fghCylinder");
            return;
        }

        /* generate for each stack */
        for (i=0,idx=0; i<stacks+1; i++)
        {
            GLushort offset = (GLushort)(1+(i+1)*slices);       /* start at 1 (0 is top vertex), and we advance one stack down as we go along */
            for (j=0; j<slices; j++, idx++)
            {
                stackIdx[idx] = (GLushort)(offset+j);
            }
        }

        /* generate for each slice */
        for (i=0,idx=0; i<slices; i++)
        {
            GLushort offset = (GLushort)(1+i);                  /* start at 1 (0 is top vertex), and we advance one slice as we go along */
            sliceIdx[idx++] = (GLushort)(offset+slices);
            sliceIdx[idx++] = (GLushort)(offset+(stacks+1)*slices);
        }

        /* draw */
        fghDrawGeometryWire(vertices,normals,nVert,
            sliceIdx,1,slices*2,GL_LINES,
            stackIdx,stacks+1,slices);

        /* cleanup allocated memory */
        free(sliceIdx);
        free(stackIdx);
    }
    else
    {
        /* First, generate vertex index arrays for drawing with glDrawElements
         * All stacks, including top and bottom are covered with a triangle
         * strip.
         */
        GLushort  *stripIdx;
        /* Create index vector */
        GLushort offset;

        /* Allocate buffers for indices, bail out if memory allocation fails */
        stripIdx = (GLushort*)malloc((slices+1)*2*(stacks+2)*sizeof(GLushort));    /*stacks +2 because of closing off bottom and top */
        if (!(stripIdx))
        {
            free(stripIdx);
            fgError("Failed to allocate memory in fghCylinder");
        }

        /* top stack */
        for (j=0, idx=0;  j<slices;  j++, idx+=2)
        {
            stripIdx[idx  ] = 0;
            stripIdx[idx+1] = (GLushort)(j+1);              /* 0 is top vertex, 1 is first for first stack */
        }
        stripIdx[idx  ] = 0;                    /* repeat first slice's idx for closing off shape */
        stripIdx[idx+1] = 1;
        idx+=2;

        /* middle stacks: */
        /* Strip indices are relative to first index belonging to strip, NOT relative to first vertex/normal pair in array */
        for (i=0; i<stacks; i++, idx+=2)
        {
            offset = (GLushort)(1+(i+1)*slices);                /* triangle_strip indices start at 1 (0 is top vertex), and we advance one stack down as we go along */
            for (j=0; j<slices; j++, idx+=2)
            {
                stripIdx[idx  ] = (GLushort)(offset+j);
                stripIdx[idx+1] = (GLushort)(offset+j+slices);
            }
            stripIdx[idx  ] = offset;               /* repeat first slice's idx for closing off shape */
            stripIdx[idx+1] = (GLushort)(offset+slices);
        }

        /* top stack */
        offset = (GLushort)(1+(stacks+2)*slices);
        for (j=0; j<slices; j++, idx+=2)
        {
            stripIdx[idx  ] = (GLushort)(offset+j);
            stripIdx[idx+1] = (GLushort)(nVert-1);              /* zero based index, last element in array (bottom vertex)... */
        }
        stripIdx[idx  ] = offset;
        stripIdx[idx+1] = (GLushort)(nVert-1);                  /* repeat first slice's idx for closing off shape */

        /* draw */
        fghDrawGeometrySolid(vertices,normals,NULL,nVert,stripIdx,stacks+2,(slices+1)*2);

        /* cleanup allocated memory */
        free(stripIdx);
    }

    /* cleanup allocated memory */
    free(vertices);
    free(normals);
}

static void fghTorus( GLfloat dInnerRadius, GLfloat dOuterRadius, GLint nSides, GLint nRings, GLboolean useWireMode )
{
    int i,j,idx, nVert;
    GLfloat *vertices, *normals;

    /* Generate vertices and normals */
    fghGenerateTorus(dInnerRadius,dOuterRadius,nSides,nRings, &vertices,&normals,&nVert);

    if (nVert==0)
        /* nothing to draw */
        return;

    if (useWireMode)
    {
        GLushort  *sideIdx, *ringIdx;
        /* First, generate vertex index arrays for drawing with glDrawElements
         * We have a bunch of line_loops to draw each side, and a
         * bunch for each ring.
         */

        ringIdx = (GLushort*)malloc(nRings*nSides*sizeof(GLushort));
        sideIdx = (GLushort*)malloc(nSides*nRings*sizeof(GLushort));
        if (!(ringIdx) || !(sideIdx))
        {
            free(ringIdx);
            free(sideIdx);
            fgError("Failed to allocate memory in fghTorus");
        }

        /* generate for each ring */
        for( j=0,idx=0; j<nRings; j++ )
            for( i=0; i<nSides; i++, idx++ )
                ringIdx[idx] = (GLushort)(j * nSides + i);

        /* generate for each side */
        for( i=0,idx=0; i<nSides; i++ )
            for( j=0; j<nRings; j++, idx++ )
                sideIdx[idx] = (GLushort)(j * nSides + i);

        /* draw */
        fghDrawGeometryWire(vertices,normals,nVert,
            ringIdx,nRings,nSides,GL_LINE_LOOP,
            sideIdx,nSides,nRings);
        
        /* cleanup allocated memory */
        free(sideIdx);
        free(ringIdx);
    }
    else
    {
        /* First, generate vertex index arrays for drawing with glDrawElements
         * All stacks, including top and bottom are covered with a triangle
         * strip.
         */
        GLushort  *stripIdx;

        /* Allocate buffers for indices, bail out if memory allocation fails */
        stripIdx = (GLushort*)malloc((nRings+1)*2*nSides*sizeof(GLushort));
        if (!(stripIdx))
        {
            free(stripIdx);
            fgError("Failed to allocate memory in fghTorus");
            return;
        }

        for( i=0, idx=0; i<nSides; i++ )
        {
            int ioff = 1;
            if (i==nSides-1)
                ioff = -i;

            for( j=0; j<nRings; j++, idx+=2 )
            {
                int offset = j * nSides + i;
                stripIdx[idx  ] = (GLushort)offset;
                stripIdx[idx+1] = (GLushort)(offset + ioff);
            }
            /* repeat first to close off shape */
            stripIdx[idx  ] = (GLushort)i;
            stripIdx[idx+1] = (GLushort)(i + ioff);
            idx +=2;
        }

        /* draw */
        fghDrawGeometrySolid(vertices,normals,NULL,nVert,stripIdx,nSides,(nRings+1)*2);

        /* cleanup allocated memory */
        free(stripIdx);
    }

    /* cleanup allocated memory */
    free(vertices);
    free(normals);
}


/* -- INTERFACE FUNCTIONS ---------------------------------------------- */


/*
 * Draws a solid sphere
 */
void FGAPIENTRY glutSolidSphere(double radius, GLint slices, GLint stacks)
{
    FREEGLUT_EXIT_IF_NOT_INITIALISED ( "glutSolidSphere" );
    fghSphere((GLfloat)radius, slices, stacks, GL_FALSE );
}

/*
 * Draws a wire sphere
 */
void FGAPIENTRY glutWireSphere(double radius, GLint slices, GLint stacks)
{
    FREEGLUT_EXIT_IF_NOT_INITIALISED ( "glutWireSphere" );
    fghSphere((GLfloat)radius, slices, stacks, GL_TRUE );
    
}

/*
 * Draws a solid cone
 */
void FGAPIENTRY glutSolidCone( double base, double height, GLint slices, GLint stacks )
{
    FREEGLUT_EXIT_IF_NOT_INITIALISED ( "glutSolidCone" );
    fghCone((GLfloat)base, (GLfloat)height, slices, stacks, GL_FALSE );
}

/*
 * Draws a wire cone
 */
void FGAPIENTRY glutWireCone( double base, double height, GLint slices, GLint stacks)
{
    FREEGLUT_EXIT_IF_NOT_INITIALISED ( "glutWireCone" );
    fghCone((GLfloat)base, (GLfloat)height, slices, stacks, GL_TRUE );
}


/*
 * Draws a solid cylinder
 */
void FGAPIENTRY glutSolidCylinder(double radius, double height, GLint slices, GLint stacks)
{
    FREEGLUT_EXIT_IF_NOT_INITIALISED ( "glutSolidCylinder" );
    fghCylinder((GLfloat)radius, (GLfloat)height, slices, stacks, GL_FALSE );
}

/*
 * Draws a wire cylinder
 */
void FGAPIENTRY glutWireCylinder(double radius, double height, GLint slices, GLint stacks)
{
    FREEGLUT_EXIT_IF_NOT_INITIALISED ( "glutWireCylinder" );
    fghCylinder((GLfloat)radius, (GLfloat)height, slices, stacks, GL_TRUE );
}

/*
 * Draws a wire torus
 */
void FGAPIENTRY glutWireTorus( double dInnerRadius, double dOuterRadius, GLint nSides, GLint nRings )
{
    FREEGLUT_EXIT_IF_NOT_INITIALISED ( "glutWireTorus" );
    fghTorus((GLfloat)dInnerRadius, (GLfloat)dOuterRadius, nSides, nRings, GL_TRUE);
}

/*
 * Draws a solid torus
 */
void FGAPIENTRY glutSolidTorus( double dInnerRadius, double dOuterRadius, GLint nSides, GLint nRings )
{
    FREEGLUT_EXIT_IF_NOT_INITIALISED ( "glutSolidTorus" );
    fghTorus((GLfloat)dInnerRadius, (GLfloat)dOuterRadius, nSides, nRings, GL_FALSE);
}



/* -- INTERFACE FUNCTIONS -------------------------------------------------- */
/* Macro to generate interface functions */
#define DECLARE_SHAPE_INTERFACE(nameICaps)\
    void FGAPIENTRY glutWire##nameICaps( void )\
    {\
        FREEGLUT_EXIT_IF_NOT_INITIALISED ( "glutWire"#nameICaps );\
        fgh##nameICaps( GL_TRUE );\
    }\
    void FGAPIENTRY glutSolid##nameICaps( void )\
    {\
        FREEGLUT_EXIT_IF_NOT_INITIALISED ( "glutSolid"#nameICaps );\
        fgh##nameICaps( GL_FALSE );\
    }

void FGAPIENTRY glutWireCube( double dSize )
{
    FREEGLUT_EXIT_IF_NOT_INITIALISED ( "glutWireCube" );
    fghCube( (GLfloat)dSize, GL_TRUE );
}
void FGAPIENTRY glutSolidCube( double dSize )
{
    FREEGLUT_EXIT_IF_NOT_INITIALISED ( "glutSolidCube" );
    fghCube( (GLfloat)dSize, GL_FALSE );
}

DECLARE_SHAPE_INTERFACE(Dodecahedron)
DECLARE_SHAPE_INTERFACE(Icosahedron)
DECLARE_SHAPE_INTERFACE(Octahedron)
DECLARE_SHAPE_INTERFACE(RhombicDodecahedron)

void FGAPIENTRY glutWireSierpinskiSponge ( int num_levels, double offset[3], double scale )
{
    FREEGLUT_EXIT_IF_NOT_INITIALISED ( "glutWireSierpinskiSponge" );
    fghSierpinskiSponge ( num_levels, offset, (GLfloat)scale, GL_TRUE );
}
void FGAPIENTRY glutSolidSierpinskiSponge ( int num_levels, double offset[3], double scale )
{
    FREEGLUT_EXIT_IF_NOT_INITIALISED ( "glutSolidSierpinskiSponge" );
    fghSierpinskiSponge ( num_levels, offset, (GLfloat)scale, GL_FALSE );
}

DECLARE_SHAPE_INTERFACE(Tetrahedron)


/*** END OF FILE ***/
