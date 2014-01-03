/* TODO: implement me! */

#include <GL/freeglut.h>
#include "fg_internal.h"

void fgDeactivateMenu( SFG_Window *window ) {
  fprintf(stderr, "fgDeactivateMenu: STUB\n");
}
void fgDisplayMenu( void ) {
  fprintf(stderr, "fgDisplayMenu: STUB\n");
}
void fgUpdateMenuHighlight ( SFG_Menu *menu ) {
  fprintf(stderr, "fgUpdateMenuHighlight: STUB\n");
}
GLboolean fgCheckActiveMenu ( SFG_Window *window, int button, GLboolean pressed,
                              int mouse_x, int mouse_y )  {
  fprintf(stderr, "fgCheckActiveMenu: STUB\n");
  return GL_FALSE;
}

int  glutCreateMenu( void (* callback)( int menu ) ) { return 0; }
void glutDestroyMenu( int menu ) {}
int  glutGetMenu( void ) { return 0; }
void glutSetMenu( int menu ) {}
void glutAddMenuEntry( const char* label, int value ) {}
void glutAddSubMenu( const char* label, int subMenu ) {}
void glutChangeToMenuEntry( int item, const char* label, int value ) {}
void glutChangeToSubMenu( int item, const char* label, int value ) {}
void glutRemoveMenuItem( int item ) {}
void glutAttachMenu( int button ) {}
void glutDetachMenu( int button ) {}
void glutSetMenuFont( int menuID, void* font ) {}

void glutBitmapCharacter( void* font, int character ) {}
int  glutBitmapWidth( void* font, int character ) { return 0; }
int  glutBitmapLength( void* font, const unsigned char* string ) { return 0; }

void *glutGetMenuData( void ) { return NULL; }
void  glutSetMenuData(void* data) {}

int     glutBitmapHeight( void* font ) { return 0; }
void    glutBitmapString( void* font, const unsigned char *string ) {}
