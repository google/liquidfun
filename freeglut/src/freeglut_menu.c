/*
 * freeglut_menu.c
 *
 * Pull-down menu creation and handling.
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

#define  G_LOG_DOMAIN  "freeglut-menu"

#include "../include/GL/freeglut.h"
#include "freeglut_internal.h"

/*
 * TODO BEFORE THE STABLE RELEASE:
 *
 * It would be cool if the submenu entries were somehow marked, for example with a dings
 * on the right menu border or something like that. Think about the possibility of doing
 * the menu on layers *or* using the native window system instead of OpenGL.
 */

/* -- DEFINITIONS ---------------------------------------------------------- */

/*
 * We'll be using freeglut fonts to draw the menu
 */
#define  FREEGLUT_MENU_FONT    GLUT_BITMAP_8_BY_13
#define  FREEGLUT_MENU_HEIGHT  15
#define  FREEGLUT_MENU_BORDER   8


/* -- PRIVATE FUNCTIONS ---------------------------------------------------- */

/*
 * Private static function to find a menu entry by index
 */
static SFG_MenuEntry *fghFindMenuEntry( SFG_Menu* menu, int index )
{
    SFG_MenuEntry *entry;
    int i = 1;

    for( entry = (SFG_MenuEntry *)menu->Entries.First; entry; entry = (SFG_MenuEntry *)entry->Node.Next)
    {
        if (i == index)
            break;
        ++i;
    }

    return entry;
}

/*
 * Private static function to check for the current menu/sub menu activity state
 */
static GLboolean fghCheckMenuStatus( SFG_Window* window, SFG_Menu* menu )
{
  SFG_MenuEntry* menuEntry;
  int x, y;

  /*
   * First of all check any of the active sub menus...
   */
  for( menuEntry = (SFG_MenuEntry *)menu->Entries.First; menuEntry;
       menuEntry = (SFG_MenuEntry *)menuEntry->Node.Next )
  {
    /*
     * Is that an active sub menu by any case?
     */
    if( menuEntry->SubMenu != NULL && menuEntry->IsActive == TRUE )
    {
      /*
       * OK, have the sub-menu checked, too. If it returns TRUE, it will mean
       * that it caught the mouse cursor and we do not need to regenerate
       * the activity list, and so our parents do...
       */
      GLboolean return_status = fghCheckMenuStatus( window, menuEntry->SubMenu ) ;

      /*
       * Reactivate the submenu as the checkMenuStatus may have turned it off if the mouse
       * is in its parent menu entry.
       */
      menuEntry->SubMenu->IsActive = TRUE ;
      if ( return_status == TRUE )
        return( TRUE );
    }
  }

  /*
   * That much about our sub menus, let's get to checking the current menu:
   */
  x = window->State.MouseX;
  y = window->State.MouseY;

  /*
   * Mark all menu entries inactive...
   */
  for( menuEntry = (SFG_MenuEntry *)menu->Entries.First; menuEntry;
       menuEntry = (SFG_MenuEntry *)menuEntry->Node.Next )
  {
    menuEntry->IsActive = FALSE;
  }


  menu->IsActive = FALSE;

  /*
   * Check if the mouse cursor is contained within the current menu box
   */
  if ( ( x >= 0 ) && ( x < menu->Width ) && ( y >= 0 ) && ( y < menu->Height ) && ( window == menu->Window ) )
  {
    /*
     * Calculation of the highlighted menu item is easy enough now:
     */
    int menuID = y / FREEGLUT_MENU_HEIGHT;

    /*
     * The mouse cursor is somewhere over our box, check it out.
     */
    menuEntry = fghFindMenuEntry( menu, menuID + 1 );
    assert( menuEntry != NULL );

    /*
     * Mark the menu as active...
     */
    menuEntry->IsActive = TRUE;
    menuEntry->Ordinal = menuID;

    /*
     * If this is not the same as the last active menu entry, deactivate the previous entry.
     * Specifically, if the previous active entry was a submenu then deactivate it.
     */
    if ( menu->ActiveEntry && ( menuEntry != menu->ActiveEntry ) )
    {
      if ( menu->ActiveEntry->SubMenu != NULL )
        fgDeactivateSubMenu ( menu->ActiveEntry ) ;
    }

    menu->ActiveEntry = menuEntry ;

    /*
     * Don't forget about marking the current menu as active, too:
     */

    menu->IsActive = TRUE;

    /*
     * OKi, we have marked that entry as active, but it would be also
     * nice to have its contents updated, in case it's a sub menu.
     * Also, ignore the return value of the check function:
     */
    if( menuEntry->SubMenu != NULL )
    {
      if ( ! menuEntry->SubMenu->IsActive )
      {
        SFG_Window *current_window = fgStructure.Window ;

        /*
         * Set up the initial menu position now...
         */

        /*
         * Mark the menu as active, so that it gets displayed:
         */
        menuEntry->SubMenu->IsActive = TRUE ;

        /*
         * Set up the initial submenu position now:
         */
        menuEntry->SubMenu->X = menu->X + menu->Width ;
        menuEntry->SubMenu->Y = menu->Y + menuEntry->Ordinal * FREEGLUT_MENU_HEIGHT ;

        fgSetWindow ( menuEntry->SubMenu->Window ) ;
        glutPositionWindow ( menuEntry->SubMenu->X, menuEntry->SubMenu->Y ) ;
        glutReshapeWindow ( menuEntry->SubMenu->Width, menuEntry->SubMenu->Height ) ;
        glutPopWindow () ;
        glutShowWindow () ;
        menuEntry->SubMenu->Window->ActiveMenu = menuEntry->SubMenu ;
        fgSetWindow ( current_window ) ;
      }

      /*
       * ...then check the submenu's state:
       */
      fghCheckMenuStatus( window, menuEntry->SubMenu );

      /*
       * Even if the submenu turned up inactive, activate it because its parent entry is active
       */
      menuEntry->SubMenu->IsActive = TRUE ;
    }

    /*
     * Report back that we have caught the menu cursor
     */
    return( TRUE );
  }

  /*
   * Looks like the menu cursor is somewhere else...
   */
  return( FALSE );
}

/*
 * Displays a menu box and all of its submenus (if they are active)
 */
static void fghDisplayMenuBox( SFG_Menu* menu )
{
  SFG_MenuEntry *menuEntry;
  int i;

  /*
   * Have the menu box drawn first. The +- values are
   * here just to make it more nice-looking...
   */
  glColor4f( 0.1289f, 0.2257f, 0.28516f, 1.0f ); /* a non-black dark version of the below. */
  glBegin( GL_QUADS );
    glVertex2i( 0          , 0            );
    glVertex2i( menu->Width, 0            );
    glVertex2i( menu->Width, menu->Height );
    glVertex2i( 0          , menu->Height );
  glEnd();

  glColor4f( 0.3f, 0.4f, 0.5f, 1.0f );
  glBegin( GL_QUADS );
    glVertex2i(             1, 1             );
    glVertex2i( menu->Width-1, 1             );
    glVertex2i( menu->Width-1, menu->Height-1);
    glVertex2i(             1, menu->Height-1);
  glEnd();

  /*
   * Check if any of the submenus is currently active...
   */
  for( menuEntry = (SFG_MenuEntry *)menu->Entries.First; menuEntry;
       menuEntry = (SFG_MenuEntry *)menuEntry->Node.Next )
  {
    /*
     * Has the menu been marked as active, maybe?
     */
    if( menuEntry->IsActive == TRUE )
    {
      /*
       * That's truly right, and we need to have it highlighted.
       * There is an assumption that mouse cursor didn't move
       * since the last check of menu activity state:
       */
      int menuID = menuEntry->Ordinal;

      /*
       * So have the highlight drawn...
       */
      glColor4f( 0.2f, 0.3f, 0.4f, 1.0f );
      glBegin( GL_QUADS );
        glVertex2i( 2            , (menuID + 0)*FREEGLUT_MENU_HEIGHT + 1 );
        glVertex2i( menu->Width-2, (menuID + 0)*FREEGLUT_MENU_HEIGHT + 1 );
        glVertex2i( menu->Width-2, (menuID + 1)*FREEGLUT_MENU_HEIGHT + 2 );
        glVertex2i( 2            , (menuID + 1)*FREEGLUT_MENU_HEIGHT + 2 );
      glEnd();
    }
  }

  /*
   * Print the menu entries now...
   */
  glColor4f( 1, 1, 1, 1 );

  for( menuEntry = (SFG_MenuEntry *)menu->Entries.First, i=0; menuEntry;
       menuEntry = (SFG_MenuEntry *)menuEntry->Node.Next, ++i )
  {
    /*
     * Move the raster into position...
     */
    glRasterPos2i(
        FREEGLUT_MENU_BORDER,
        (i + 1)*FREEGLUT_MENU_HEIGHT-(int)(FREEGLUT_MENU_HEIGHT*0.3) /* Try to center the text - JCJ 31 July 2003*/
    );

    /*
     * Have the label drawn, character after character:
     */
    glutBitmapString( FREEGLUT_MENU_FONT, menuEntry->Text);

    /*
     * If it's a submenu, draw a right arrow
     */
    if ( menuEntry->SubMenu != NULL )
    {
      GLubyte arrow_char [] = { 0, 0, 32, 48, 56, 60, 62, 63, 62, 60, 56, 48, 32, 0, 0 } ;
      int width = glutBitmapWidth ( FREEGLUT_MENU_FONT, ' ' ) ;

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

      glRasterPos2i ( menu->Width - 2 - width,
                      (i + 1)*FREEGLUT_MENU_HEIGHT ) ;
      glBitmap ( width, FREEGLUT_MENU_HEIGHT, 0, 0, 0.0, 0.0, arrow_char ) ;
      glPopClientAttrib();
    }
  }

  /*
   * Now we are ready to check if any of our children needs to be redrawn:
   */
  for( menuEntry = (SFG_MenuEntry *)menu->Entries.First; menuEntry;
       menuEntry = (SFG_MenuEntry *)menuEntry->Node.Next )
  {
    /*
     * Is that an active sub menu by any case?
     */
    if( menuEntry->SubMenu != NULL && menuEntry->IsActive == TRUE )
    {
      /*
       * Yeah, indeed. Have it redrawn now:
       */
      fgSetWindow ( menuEntry->SubMenu->Window ) ;
      fghDisplayMenuBox( menuEntry->SubMenu );
      fgSetWindow ( menu->Window ) ;
    }
  }
}

/*
 * Private static function to set the parent window of a submenu and all of its submenus
 */
static void fghSetSubmenuParentWindow ( SFG_Window *window, SFG_Menu *menu )
{
  SFG_MenuEntry *menuEntry ;

  menu->ParentWindow = window ;

  for ( menuEntry = (SFG_MenuEntry *)menu->Entries.First; menuEntry; menuEntry = (SFG_MenuEntry *)menuEntry->Node.Next )
  {
    if ( menuEntry->SubMenu != NULL )
      fghSetSubmenuParentWindow ( window, menuEntry->SubMenu ) ;
  }
}


/*
 * Displays the currently active menu for the current window
 */
void fgDisplayMenu( void )
{
    SFG_Window* window = fgStructure.Window;
    SFG_Menu* menu = NULL;

    /*
     * Make sure there is a current window available
     */
    freeglut_assert_window;

    /*
     * Check if there is an active menu attached to this window...
     */
    menu = window->ActiveMenu;

    /*
     * Did we find an active menu?
     */
    freeglut_return_if_fail( menu != NULL );

    fgSetWindow ( menu->Window ) ;

    /*
     * Prepare the OpenGL state to do the rendering first:
     */
    glPushAttrib( GL_DEPTH_BUFFER_BIT | GL_TEXTURE_BIT | GL_LIGHTING_BIT | GL_POLYGON_BIT );

    glDisable( GL_DEPTH_TEST );
    glDisable( GL_TEXTURE_2D );
    glDisable( GL_LIGHTING   );
    glDisable( GL_CULL_FACE  );

    /*
     * We'll use an orthogonal projection matrix to draw the menu:
     */
    glMatrixMode( GL_PROJECTION );
    glPushMatrix();
    glLoadIdentity();
    glOrtho(
         0, glutGet( GLUT_WINDOW_WIDTH  ),
         glutGet( GLUT_WINDOW_HEIGHT ), 0,
        -1, 1
    );

    /*
     * Model-view matix gets reset to identity:
     */
    glMatrixMode( GL_MODELVIEW );
    glPushMatrix();
    glLoadIdentity();

    /*
     * First of all, have the exact menu status check:
     */
    fghCheckMenuStatus( window, menu );

    /*
     * The status has been updated and we're ready to have the menu drawn now:
     */
    fghDisplayMenuBox( menu );

    /*
     * Restore the old OpenGL settings now
     */
    glPopAttrib();

    glMatrixMode( GL_PROJECTION );
    glPopMatrix();
    glMatrixMode( GL_MODELVIEW );
    glPopMatrix();

    glutSwapBuffers () ;

    /*
     * Restore the current window
     */
    fgSetWindow ( window ) ;
}

/*
 * Activates a menu pointed by the function argument
 */
void fgActivateMenu( SFG_Window* window, int button )
{
  /*
   * We'll be referencing this menu a lot, so remember its address:
   */
  SFG_Menu* menu = window->Menu[ button ];

  /*
   * Mark the menu as active, so that it gets displayed:
   */
  window->ActiveMenu = menu;
  menu->IsActive = TRUE ;
  fgState.ActiveMenus ++ ;

  /*
   * Set up the initial menu position now:
   */

  menu->X = window->State.MouseX + glutGet ( GLUT_WINDOW_X ) ;
  menu->Y = window->State.MouseY + glutGet ( GLUT_WINDOW_Y ) ;

  fgSetWindow ( menu->Window ) ;
  glutPositionWindow ( menu->X, menu->Y ) ;
  glutReshapeWindow ( menu->Width, menu->Height ) ;
  glutPopWindow () ;
  glutShowWindow () ;
  menu->Window->ActiveMenu = menu ;

/*  if( x > ( glutGet( GLUT_WINDOW_WIDTH ) - menu->Width ) )
    menu->X = glutGet( GLUT_WINDOW_WIDTH ) - menu->Width;
  if( y > ( glutGet( GLUT_WINDOW_HEIGHT ) - menu->Height) )
    menu->Y = glutGet( GLUT_WINDOW_HEIGHT ) - menu->Height; */
}

/*
 * Check whether an active menu absorbs a mouse click
 */
GLboolean fgCheckActiveMenu ( SFG_Window *window, SFG_Menu *menu )
{
  /*
   * Near as I can tell, this is the active menu behaviour:
   *  - Down-click any button outside the menu, menu active:  deactivate the menu
   *  - Down-click any button inside the menu, menu active:  select the menu entry and deactivate the menu
   *  - Up-click the menu button outside the menu, menu active:  nothing happens
   *  - Up-click the menu button inside the menu, menu active:  select the menu entry and deactivate the menu
   * Since menus can have submenus, we need to check this recursively.
   */
  return fghCheckMenuStatus ( window, menu ) ;
}

/*
 * Function to check for menu entry selection on menu deactivation
 */
void fgExecuteMenuCallback( SFG_Menu* menu )
{
  SFG_MenuEntry *menuEntry;

  /*
   * First of all check any of the active sub menus...
   */
  for( menuEntry = (SFG_MenuEntry *)menu->Entries.First; menuEntry; menuEntry = (SFG_MenuEntry *)menuEntry->Node.Next)
  {
    /*
     * Is this menu entry active?
     */
    if( menuEntry->IsActive == TRUE )
    {
      /*
       * If there is not a sub menu, execute the menu callback and return...
       */
      if( menuEntry->SubMenu == NULL )
      {
        /*
         * ...certainly given that there is one...
         */
        if( menu->Callback != NULL )
          menu->Callback( menuEntry->ID );

        return;
      }

      /*
       * Otherwise recurse into the submenu.
       */
      fgExecuteMenuCallback( menuEntry->SubMenu );

      /*
       * There is little sense in dwelling the search on
       */
      return;
    }
  }
}

/*
 * Deactivates a menu pointed by the function argument.
 */
void fgDeactivateMenu( SFG_Window *window )
{
  SFG_Window *current_window = fgStructure.Window ;

    /*
     * Check if there is an active menu attached to this window...
     */
    SFG_Menu* menu = window->ActiveMenu;
    SFG_MenuEntry *menuEntry ;

    /*
     * Did we find an active window?
     */
    freeglut_return_if_fail( menu != NULL );

    /*
     * Hide the present menu's window
     */
    fgSetWindow ( menu->Window ) ;
    glutHideWindow () ;

    /*
     * Forget about having that menu active anymore, now:
     */
    menu->Window->ActiveMenu = NULL ;
    menu->ParentWindow->ActiveMenu = NULL ;
    menu->IsActive = FALSE ;

    fgState.ActiveMenus -- ;

    /*
     * Hide all submenu windows, and the root menu's window.
     */
    for ( menuEntry = (SFG_MenuEntry *)menu->Entries.First; menuEntry;
          menuEntry = (SFG_MenuEntry *)menuEntry->Node.Next )
    {
      /*
       * Is that an active submenu by any case?
       */
      if ( ( menuEntry->SubMenu != NULL ) && menuEntry->SubMenu->IsActive )
        fgDeactivateSubMenu ( menuEntry ) ;
    }

    fgStructure.Window = current_window ;
}

/*
 * Deactivates a menu pointed by the function argument.
 */
void fgDeactivateSubMenu( SFG_MenuEntry *menuEntry )
{
  SFG_Window *current_window = fgStructure.Window ;
  SFG_MenuEntry *subMenuIter ;
    /*
     * Hide the present menu's window
     */
    fgSetWindow ( menuEntry->SubMenu->Window ) ;
    glutHideWindow () ;

    /*
     * Forget about having that menu active anymore, now:
     */
    menuEntry->SubMenu->Window->ActiveMenu = NULL ;
    menuEntry->SubMenu->IsActive = FALSE ;

    /*
     * Hide all submenu windows, and the root menu's window.
     */
    for ( subMenuIter = (SFG_MenuEntry *)menuEntry->SubMenu->Entries.First; subMenuIter;
          subMenuIter = (SFG_MenuEntry *)subMenuIter->Node.Next )
    {
      /*
       * Is that an active submenu by any case?
       */
      if ( ( subMenuIter->SubMenu != NULL ) && subMenuIter->SubMenu->IsActive )
        fgDeactivateSubMenu ( subMenuIter ) ;
    }

    fgStructure.Window = current_window ;
}

/*
 * Recalculates current menu's box size
 */
void fghCalculateMenuBoxSize( void )
{
  SFG_MenuEntry* menuEntry;
  int width = 0, height = 0;

  /*
   * Make sure there is a current menu set
   */
  freeglut_assert_ready; freeglut_return_if_fail( fgStructure.Menu != NULL );

  /*
   * The menu's box size depends on the menu entries:
   */
  for( menuEntry = (SFG_MenuEntry *)fgStructure.Menu->Entries.First; menuEntry;
       menuEntry = (SFG_MenuEntry *)menuEntry->Node.Next)
  {
    /*
     * Update the menu entry's width value
     */
    menuEntry->Width = glutBitmapLength( FREEGLUT_MENU_FONT, menuEntry->Text );

    /*
     * If the entry is a submenu, then it needs to be wider to accomodate the arrow. JCJ 31 July 2003
     */

    if (menuEntry->SubMenu != NULL)
       menuEntry->Width += glutBitmapLength( FREEGLUT_MENU_FONT, " " );

    /*
     * Check if it's the biggest we've found
     */
    if( menuEntry->Width > width )
      width = menuEntry->Width;

    height += FREEGLUT_MENU_HEIGHT;
  }

  /*
   * Store the menu's box size now:
   */
  fgStructure.Menu->Height = height; 
  fgStructure.Menu->Width  = width + 2 * FREEGLUT_MENU_BORDER ;
}


/* -- INTERFACE FUNCTIONS -------------------------------------------------- */

/*
 * Creates a new menu object, adding it to the freeglut structure
 */
int FGAPIENTRY glutCreateMenu( void (* callback)( int ) )
{
    /*
     * The menu object creation code resides in freeglut_structure.c
     */
    return( fgCreateMenu( callback )->ID );
}

/*
 * Destroys a menu object, removing all references to it
 */
void FGAPIENTRY glutDestroyMenu( int menuID )
{
    SFG_Menu* menu = fgMenuByID( menuID );

    freeglut_assert_ready; freeglut_return_if_fail( menu != NULL );

    /*
     * The menu object destruction code resides in freeglut_structure.c
     */
    fgDestroyMenu( menu );
}

/*
 * Returns the ID number of the currently active menu
 */
int FGAPIENTRY glutGetMenu( void )
{
    freeglut_assert_ready;

    /*
     * Is there a current menu set?
     */
    if( fgStructure.Menu != NULL )
    {
        /*
         * Yes, there is indeed...
         */
        return( fgStructure.Menu->ID );
    }

    /*
     * No, there is no current menu at all
     */
    return( 0 );
}

/*
 * Sets the current menu given its menu ID
 */
void FGAPIENTRY glutSetMenu( int menuID )
{
    SFG_Menu* menu = fgMenuByID( menuID );

    freeglut_assert_ready; freeglut_return_if_fail( menu != NULL );

    /*
     * The current menu pointer is stored in fgStructure.Menu
     */
    fgStructure.Menu = menu;
}

/*
 * Adds a menu entry to the bottom of the current menu
 */
void FGAPIENTRY glutAddMenuEntry( const char* label, int value )
{
    SFG_MenuEntry* menuEntry = (SFG_MenuEntry *)calloc( sizeof(SFG_MenuEntry), 1 );

    /*
     * Make sure there is a current menu set
     */
    freeglut_assert_ready; freeglut_return_if_fail( fgStructure.Menu != NULL );

    /*
     * Fill in the appropriate values...
     */
    menuEntry->Text = strdup( label );
    menuEntry->ID   = value;

    /*
     * Have the new menu entry attached to the current menu
     */
    fgListAppend( &fgStructure.Menu->Entries, &menuEntry->Node );

    /*
     * Update the menu's dimensions now
     */
    fghCalculateMenuBoxSize();
}

/*
 * Add a sub menu to the bottom of the current menu
 */
void FGAPIENTRY glutAddSubMenu( const char* label, int subMenuID )
{
  SFG_MenuEntry* menuEntry = (SFG_MenuEntry *)calloc( sizeof(SFG_MenuEntry), 1 );
  SFG_Menu*      subMenu = fgMenuByID( subMenuID );

  /*
   * Make sure there is a current menu and the sub menu
   * we want to attach actually exists...
   */
  freeglut_assert_ready; freeglut_return_if_fail( fgStructure.Menu != NULL );
  freeglut_return_if_fail( subMenu != NULL );

  /*
   * Fill in the appropriate values
   */
  menuEntry->Text = strdup( label );
  menuEntry->SubMenu = subMenu;
  menuEntry->ID      = -1;

  /*
   * Make the submenu's parent window be the menu's parent window
   */
  fghSetSubmenuParentWindow ( fgStructure.Menu->ParentWindow, subMenu ) ;

  /*
   * Have the new menu entry attached to the current menu
   */
  fgListAppend( &fgStructure.Menu->Entries, &menuEntry->Node );

  /*
   * Update the menu's dimensions now
   */
  fghCalculateMenuBoxSize();
}

/*
 * Changes the specified menu item in the current menu into a menu entry
 */
void FGAPIENTRY glutChangeToMenuEntry( int item, const char* label, int value )
{
    SFG_MenuEntry* menuEntry = NULL;

    /*
     * Make sure there is a current menu set...
     */
    freeglut_assert_ready; freeglut_return_if_fail( fgStructure.Menu != NULL );

    /*
     * Get n-th menu entry in the current menu, starting from one:
     */
    menuEntry = fghFindMenuEntry( fgStructure.Menu, item );

    /*
     * Make sure the menu entry exists
     */
    freeglut_return_if_fail( menuEntry != NULL );

    /*
     * We want it to become a normal menu entry, so:
     */
    if( menuEntry->Text != NULL )
        free( menuEntry->Text );

    menuEntry->Text = strdup( label );
    menuEntry->ID      = value;
    menuEntry->SubMenu = NULL;

    /*
     * Update the menu's dimensions now
     */
    fghCalculateMenuBoxSize();
}

/*
 * Changes the specified menu item in the current menu into a sub-menu trigger.
 */
void FGAPIENTRY glutChangeToSubMenu( int item, const char* label, int subMenuID )
{
    SFG_Menu*      subMenu = fgMenuByID( subMenuID );
    SFG_MenuEntry* menuEntry = NULL;

    /*
     * Make sure there is a current menu set and the sub menu exists...
     */
    freeglut_assert_ready; freeglut_return_if_fail( fgStructure.Menu != NULL );
    freeglut_return_if_fail( subMenu != NULL );

    /*
     * Get n-th menu entry in the current menu, starting from one:
     */
    menuEntry = fghFindMenuEntry( fgStructure.Menu, item );

    /*
     * Make sure the menu entry exists
     */
    freeglut_return_if_fail( menuEntry != NULL );

    /*
     * We want it to become a sub menu entry, so:
     */
    if( menuEntry->Text != NULL )
        free( menuEntry->Text );

    menuEntry->Text = strdup( label );
    menuEntry->SubMenu = subMenu;
    menuEntry->ID      = -1;

    /*
     * Update the menu's dimensions now
     */
    fghCalculateMenuBoxSize();
}

/*
 * Removes the specified menu item from the current menu
 */
void FGAPIENTRY glutRemoveMenuItem( int item )
{
    SFG_MenuEntry* menuEntry;

    /*
     * Make sure there is a current menu set
     */
    freeglut_assert_ready; freeglut_return_if_fail( fgStructure.Menu != NULL );

    /*
     * Get n-th menu entry in the current menu, starting from one:
     */
    menuEntry = fghFindMenuEntry( fgStructure.Menu, item );

    /*
     * Make sure the menu entry exists
     */
    freeglut_return_if_fail( menuEntry != NULL );

    /*
     * Removing a menu entry is quite simple...
     */
    fgListRemove( &fgStructure.Menu->Entries, &menuEntry->Node );

    /*
     * Free the entry label string, too
     */
    free( menuEntry->Text );

    free( menuEntry );

    /*
     * Update the menu's dimensions now
     */
    fghCalculateMenuBoxSize();
}

/*
 * Attaches a menu to the current window
 */
void FGAPIENTRY glutAttachMenu( int button )
{
    freeglut_assert_ready;

    /*
     * There must be a current window and a current menu set:
     */
    freeglut_return_if_fail( fgStructure.Window != NULL || fgStructure.Menu != NULL );

    /*
     * Make sure the button value is valid (0, 1 or 2, see freeglut.h)
     */
    freeglut_return_if_fail( button == GLUT_LEFT_BUTTON || button == GLUT_MIDDLE_BUTTON || button == GLUT_RIGHT_BUTTON );

    /*
     * It is safe now to attach the menu
     */
    fgStructure.Window->Menu[ button ] = fgStructure.Menu;

    /*
     * Make the parent window of the menu (and all submenus) the current window
     */
    fghSetSubmenuParentWindow ( fgStructure.Window, fgStructure.Menu ) ;
}

/*
 * Detaches a menu from the current window
 */
void FGAPIENTRY glutDetachMenu( int button )
{
    freeglut_assert_ready;

    /*
     * There must be a current window set:
     */
    freeglut_return_if_fail( fgStructure.Window != NULL );

    /*
     * Make sure the button value is valid (0, 1 or 2, see freeglut.h)
     */
    freeglut_return_if_fail( button != 0 && button != 1 && button != 2 );

    /*
     * It is safe now to detach the menu
     */
    fgStructure.Window->Menu[ button ] = NULL;
}

/*
 * A.Donev: Set and retrieve the menu's user data
 */
void* FGAPIENTRY glutGetMenuData( void )
{
   return(fgStructure.Menu->UserData);
}

void FGAPIENTRY glutSetMenuData(void* data)
{
  fgStructure.Menu->UserData=data;
}

/*** END OF FILE ***/
