/* $XConsortium: wfont.h,v 5.2 94/04/17 20:10:09 rws Exp $ */

/*****************************************************************

Copyright (c) 1989,1990, 1991  X Consortium

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
X CONSORTIUM BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of the X Consortium shall not be
used in advertising or otherwise to promote the sale, use or other dealings
in this Software without prior written authorization from the X Consortium.

Copyright (c) 1989,1990, 1991 by Sun Microsystems, Inc.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the names of Sun Microsystems,
and the X Consortium, not be used in advertising or publicity 
pertaining to distribution of the software without specific, written 
prior permission.  

SUN MICROSYSTEMS DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, 
INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT 
SHALL SUN MICROSYSTEMS BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL 
DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/


#ifndef WFONT_INCLUDED
#define WFONT_INCLUDED

#define WFONT_MAGIC	0x813
#define WFONT_MAGIC_PLUS 0x715
#define WFONT_MAGIC_PEX 0x70686e74
#define START_PROPS 0x100
#define START_DISPATCH(_num_props)  (START_PROPS + 160 * _num_props)
#define START_PATH(_num_ch_, _num_props)  (START_DISPATCH(_num_props) + sizeof(Dispatch) * _num_ch_)
#define NUM_DISPATCH	128

typedef struct {
	unsigned short	x;
	unsigned short	y;
} Path_point2dpx;

typedef struct {
	float	x;
	float	y;
} Path_point2df;

typedef struct {
	int	x;
	int	y;
	int	z;
} Path_point3di;

typedef struct {
	float	x;
	float	y;
	float	z;
} Path_point3df;

typedef struct {
	float	x;
	float	y;
	float	z;
	float	w;
} Path_point4df;

typedef union {
	Path_point2dpx	*pt2dpx;
	Path_point2df	*pt2df;
	Path_point3di	*pt3di;
	Path_point3df	*pt3df;
	Path_point4df	*pt4df;
} Path_pt_ptr;

typedef enum {
	PATH_2DF, 
	PATH_2DPX, 
	PATH_3DF, 
	PATH_3DI, 
	PATH_4DF 
} Path_type;

typedef struct {
	int		n_pts;		/* number of points in the subpath */
	Path_pt_ptr	pts;		/* pointer to them */
	int		closed;		/* true if the subpath is closed */
	int		dcmp_flag;      /* flag for pgon dcmp, pgon type 
				       and dcmped triangle type */
} Path_subpath;

typedef struct {
	Path_type	type;		/* type of vertices in this path */
	int		n_subpaths;	/* number of subpaths */
	int		n_vertices;	/* total number of vertices */
	Path_subpath	*subpaths;	/* array of subpaths */
} Path;

typedef Path	*Path_handle;

typedef struct {
	char	propname[80];          /* font property name */
	char	propvalue[80];         /* font property value */
}Property;

typedef struct {
	int             magic;	       /* magic number */
	char            name[80];      /* name of this font */
	float           top,	       /* extreme values */
	                bottom,
			max_width;
	int		num_ch;        /* no. of fonts in the set */
	int             num_props;     /* no. of font properties */
	Property        *properties;   /* array of properties */
} Font_header;

typedef struct {
	float           center,	       /* center of the character */
	                right;	       /* right edge */
	long            offset;	       /* offset in the file of the character
					* description*/
} Dispatch;	


typedef struct {
	float           center,
	                right;
	Path            strokes;
} Ch_font;

typedef struct {
	char            name[80];
	float           top,
	                bottom,
			max_width;
	int		num_ch;		/* # characters in the font*/
	Ch_font         **ch_data;
} Phg_font;

#endif	/*WFONT_INCLUDED*/
