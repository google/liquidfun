%{
/* $XConsortium: lex.l,v 5.9 95/01/05 19:18:40 kaleb Exp $ */
/* $XFree86: xc/fonts/PEX/lex.l,v 3.9 1996/10/17 15:10:45 dawes Exp $ */

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

#include <ctype.h>
#include <math.h>
#include <stdlib.h>

int string(char *, int), res_words(char *);

%}
%%
\'[^']*\' |
\"[^"]*\"		return string(yytext, yyleng);
#.*			;
[ ,;\t\n\r]*              /* natural dilimters */ ;

[a-zA-Z][a-zA-Z0-9_.]*	{
				int	token;
				if ( (token = res_words(yytext)) )
					return token;
				return string(yytext, yyleng);
		 	}

[+-]?[0-9]+\.?[0-9]*[eE][+-]?[0-9]+ |
[+-]?[0-9]+\.[0-9]*     |
\.[0-9]+		{
                                yylval.dval = atof((char *)yytext);
				return REAL;
                        }
[+-]?[0-9]+#[0-9]+	{
				return INTEGER;
			}
[+-]?[0-9]+		{
				yylval.ival = atoi((char *)yytext);
				return INTEGER;
			}
[()]			;
%%

int res_words(char *str)
{
	static	struct	res_strct {
		char	*word;
		int	token;
	} res_table[] = {
		{"BOTTOM",		BOTTOM},
		{"CENTER",		CENTER},
                {"PROPERTIES",          PROPERTIES},
		{"CLOSE",		CLOSE},
		{"FONTNAME",		FONTNAME},
		{"INDEX",		INDEX},
		{"MAGIC",		MAGIC},
		{"OPEN",		OPEN},
		{"RIGHT",		RIGHT},
		{"STROKE",		STROKE},
		{"TOP",			TOP},
		{"VERTICES",		VERTICES},
		{"BEARING",		BEARING},
		{"L_SPACE",		L_SPACE},
		{"WIDTH",		WIDTH},
		{"R_SPACE",		R_SPACE},
		{"NUM_CH",		NUM_CH},
		{0, 0}
	};

	{
		register struct res_strct *reserved;

		reserved = res_table;

		do
			if (!strcmp(str, reserved->word))
				break;
		while ((++reserved)->word != 0);
		return reserved->token;
	}
}

int string(char *str, int n)
{
	if (*str == '\"' || *str == '\'')
	{
		str++;
		n -= 2;	/* one for EOL, one for end quote */
	}
	if ((yylval.cval = (char *)malloc(n + 1)) != NULL)
	{
		strncpy(yylval.cval, str, n);
		yylval.cval[n] = '\0';
		return STRING;
	}
	else
		return 0;
}
