/*
 * freeglut_font.c
 *
 * Bitmap and stroke fonts displaying.
 *
 * Copyright (c) 1999-2000 Pawel W. Olszta. All Rights Reserved.
 * Written by Pawel W. Olszta, <olszta@sourceforge.net>
 * Creation date: Thu Dec 16 1999
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

#define  G_LOG_DOMAIN  "freeglut-font"

#include "../include/GL/freeglut.h"
#include "../include/GL/freeglut_internal.h"

/*
 * TODO BEFORE THE STABLE RELEASE:
 *
 *  glutStrokeCharacter()       -- stroke fonts not implemented, uses a bitmap font instead
 *  glutStrokeWidth()           -- stroke fonts not implemented, uses a bitmap font instead
 */

/* -- IMPORT DECLARATIONS -------------------------------------------------- */

/*
 * These are the font faces defined in freeglut_font_data.c file:
 */
extern SFG_Font fgFontFixed8x13;
extern SFG_Font fgFontFixed9x15;
extern SFG_Font fgFontHelvetica10;
extern SFG_Font fgFontHelvetica12;
extern SFG_Font fgFontHelvetica18;
extern SFG_Font fgFontTimesRoman10;
extern SFG_Font fgFontTimesRoman24;
extern SFG_StrokeFont fgStrokeRoman;
extern SFG_StrokeFont fgStrokeMonoRoman;

/*
 * This is for GLUT binary compatibility, as suggested by Steve Baker
 */
#if TARGET_HOST_UNIX_X11
    void* glutStrokeRoman;
    void* glutStrokeMonoRoman;
    void* glutBitmap9By15;
    void* glutBitmap8By13;
    void* glutBitmapTimesRoman10;
    void* glutBitmapTimesRoman24;
    void* glutBitmapHelvetica10;
    void* glutBitmapHelvetica12;
    void* glutBitmapHelvetica18;
#endif


/* -- PRIVATE FUNCTIONS ---------------------------------------------------- */

/*
 * Matches a font ID with a SFG_Font structure pointer.
 * This was changed to match the GLUT header style.
 */
static SFG_Font* fghFontByID( void* font )
{
    /*
     * Try matching the font ID and the font data structure
     */
    if( font == GLUT_BITMAP_8_BY_13        ) return( &fgFontFixed8x13    );
    if( font == GLUT_BITMAP_9_BY_15        ) return( &fgFontFixed9x15    );
    if( font == GLUT_BITMAP_HELVETICA_10   ) return( &fgFontHelvetica10  );
    if( font == GLUT_BITMAP_HELVETICA_12   ) return( &fgFontHelvetica12  );
    if( font == GLUT_BITMAP_HELVETICA_18   ) return( &fgFontHelvetica18  );
    if( font == GLUT_BITMAP_TIMES_ROMAN_10 ) return( &fgFontTimesRoman10 );
    if( font == GLUT_BITMAP_TIMES_ROMAN_24 ) return( &fgFontTimesRoman24 );

    /*
     * This probably is the library user's fault
     */
    fgError( "font 0x%08x not found", font );

    return 0;
}

/*
 * Matches a font ID with a SFG_StrokeFont structure pointer.
 * This was changed to match the GLUT header style.
 */
static SFG_StrokeFont* fghStrokeByID( void* font )
{
    /*
     * Try matching the font ID and the font data structure
     */
    if( font == GLUT_STROKE_ROMAN ) return( &fgStrokeRoman );
    if( font == GLUT_STROKE_MONO_ROMAN ) return( &fgStrokeMonoRoman );

    /*
     * This probably is the library user's fault
     */
    fgError( "stroke font 0x%08x not found", font );

    return 0;
}


/* -- INTERFACE FUNCTIONS -------------------------------------------------- */

/*
 * Draw a bitmap character
 */
void FGAPIENTRY glutBitmapCharacter( void* fontID, int character )
{
    const GLubyte* face;

    /*
     * First of all we'll need a font to use
     */
    SFG_Font* font = fghFontByID( fontID );

    /*
     * Make sure the character we want to output is valid
     */
    freeglut_return_if_fail( character >= 0 && character < font->Quantity );

    /*
     * Then find the character we want to draw
     */
    face = font->Characters[ character - 1 ];

    /*
     * Save the old pixel store settings
     */
    glPushClientAttrib( GL_CLIENT_PIXEL_STORE_BIT );

    /*
     * Set up the pixel unpacking ways
     */
    glPixelStorei( GL_UNPACK_SWAP_BYTES,  GL_FALSE );
    glPixelStorei( GL_UNPACK_LSB_FIRST,   GL_FALSE );
    glPixelStorei( GL_UNPACK_ROW_LENGTH,  0        );
    glPixelStorei( GL_UNPACK_SKIP_ROWS,   0        );
    glPixelStorei( GL_UNPACK_SKIP_PIXELS, 0        );
    glPixelStorei( GL_UNPACK_ALIGNMENT,   1        );

    /*
     * We'll use a glBitmap call to draw the font.
     */
    glBitmap(
        face[ 0 ], font->Height,    /* The bitmap's width and height */
        0, 0,                       /* The origin -- what the hell?  */
        face[ 0 ] + 1, 0,           /* The raster advance -- inc. x  */
        (face + 1)                  /* The packed bitmap data...     */
    );

    /*
     * Restore the old pixel store settings
     */
    glPopClientAttrib();
}

void FGAPIENTRY glutBitmapString( void* fontID, const char *string )
{
    int i;

    for( i=0; i<strlen( string ); i++ )
        glutBitmapCharacter( fontID, string[ i ] );
}

/*
 * Returns the width in pixels of a font's character
 */
int FGAPIENTRY glutBitmapWidth( void* fontID, int character )
{
    /*
     * First of all, grab the font we need
     */
    SFG_Font* font = fghFontByID( fontID );

    /*
     * Make sure the character we want to output is valid
     */
    freeglut_return_val_if_fail( character > 0 && character < font->Quantity, 0 );

    /*
         * Scan the font looking for the specified character
     */
    return( *(font->Characters[ character - 1 ]) + 1 );
}

/*
 * Draw a stroke character
 */
void FGAPIENTRY glutStrokeCharacter( void* fontID, int character )
{
    const SFG_StrokeChar *schar;
    const SFG_StrokeStrip *strip;
    int i, j;

    /*
     * First of all we'll need a font to use
     */
    SFG_StrokeFont* font = fghStrokeByID( fontID );

    /*
     * Make sure the character we want to output is valid
     */
    freeglut_return_if_fail( character >= 0 && character < font->Quantity );

    schar = font->Characters[character];

    strip = schar->Strips;

    for (i = 0; i < schar->Number; i++, strip++)
    {
        glBegin(GL_LINE_STRIP);
        for(j = 0; j < strip->Number; j++)
        {
            glVertex2f(strip->Vertices[j].X, strip->Vertices[j].Y);
        }
        glEnd();
    }
    glTranslatef(schar->Right, 0.0, 0.0);
}

/*
 * Return the width in pixels of a stroke character
 */
int FGAPIENTRY glutStrokeWidth( void* fontID, int character )
{
    /*
     * First of all we'll need a font to use
     */
    SFG_StrokeFont* font = fghStrokeByID( fontID );

    /*
     * Make sure the character we want to output is valid
     */
    freeglut_return_if_fail( character >= 0 && character < font->Quantity );

    return (font->Characters[character]->Right);
}

/*
 * Return the width of a string drawn using a bitmap font
 */
int FGAPIENTRY glutBitmapLength( void* fontID, const char* string )
{
    int i, length = 0;

    /*
     * Using glutBitmapWidth() function to calculate the result
     */
    for( i=0; i<strlen( string ); i++ )
        length += glutBitmapWidth( fontID, string[ i ] );

    /*
     * Return the result now
     */
    return( length );
}

/*
 * Return the width of a string drawn using a stroke font
 */
int FGAPIENTRY glutStrokeLength( void* fontID, const char* string )
{
    int i, length = 0;

    /*
     * Using glutStrokeWidth() function to calculate the result
     */
    for( i=0; i<strlen( string ); i++ )
        length += glutStrokeWidth( fontID, string[ i ] );

    /*
     * Return the result now
     */
    return( length );
}

/*
 * Returns the height of a bitmap font
 */
int FGAPIENTRY glutBitmapHeight( void* fontID )
{
    /*
     * See which font are we queried about
     */
    SFG_Font* font = fghFontByID( fontID );

    /*
     * Return the character set's height
     */
    return( font->Height );
}

/*
 * Returns the height of a stroke font
 */
GLfloat FGAPIENTRY glutStrokeHeight( void* fontID )
{
    /*
     * See which font are we queried about
     */
    SFG_StrokeFont* font = fghStrokeByID( fontID );

    /*
     * Return the character set's height
     */
    return( font->Height );
}

/*** END OF FILE ***/
