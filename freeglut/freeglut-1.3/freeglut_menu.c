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
#include "../include/GL/freeglut_internal.h"

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


/* -- PRIVATE FUNCTIONS ---------------------------------------------------- */

/*
 * Private static function to check for the current menu/sub menu activity state
 */
static gboolean fghCheckMenuStatus( SFG_Menu* menu )
{
    SFG_Window* window = fgStructure.Window;
    gint i, x, y;

    /*
     * First of all check any of the active sub menus...
     */
    for( i=0; i<(gint) g_list_length( menu->Entries ); i++ )
    {
        SFG_MenuEntry* menuEntry = (SFG_MenuEntry *) g_list_nth( menu->Entries, i )->data;

        /*
         * Is that an active sub menu by any case?
         */
        if( menuEntry->SubMenu != NULL && menuEntry->IsActive == TRUE )
        {
            /*
             * OKi, have the sub-menu checked, too. If it returns TRUE, it will mean
             * that it caught the mouse cursor and we do not need to regenerate
             * the activity list, and so our parents do...
             */
            if( fghCheckMenuStatus( menuEntry->SubMenu ) == TRUE )
                return( TRUE );
        }
    }

    /*
     * That much about our sub menus, let's get to checking the current menu:
     */
    x = window->State.MouseX - menu->X;
    y = window->State.MouseY - menu->Y;

    /*
     * Mark all menu entries inactive...
     */
    for( i=0; i<(gint) g_list_length( menu->Entries ); i++ )
    {
        SFG_MenuEntry* menuEntry = (SFG_MenuEntry *) g_list_nth( menu->Entries, i )->data;

        menuEntry->IsActive = FALSE;
    }

    menu->IsActive = FALSE;

    /*
     * Check if the mouse cursor is contained within the current menu box
     */
    if( x >= 0 && x < menu->Width && y >= 0 && y < menu->Height )
    {
        /*
         * Calculation of the highlighted menu item is easy enough now:
         */
        gint menuID = y / FREEGLUT_MENU_HEIGHT;

        /*
         * The mouse cursor is somewhere over our box, check it out.
         */
        SFG_MenuEntry* menuEntry = (SFG_MenuEntry *) g_list_nth( menu->Entries, menuID )->data;
        g_assert( menuEntry != NULL );

        /*
         * Mark the menu as active...
         */
        menuEntry->IsActive = TRUE;
        menuEntry->Ordinal = menuID;

        /*
         * Don't forget about marking the current menu as active, too:
         */
        menu->IsActive = TRUE;

        /*
         * OKi, we have marked that entry as active, but it would be also
         * nice to have it's contents updated, in case it's a sub menu.
         * Also, ignore the return value of the check function:
         */
        if( menuEntry->SubMenu != NULL )
        {
            gint x = window->State.MouseX;
            gint y = window->State.MouseY;

            /*
             * Set up the initial menu position now...
             */

            if( x > 15 ) menuEntry->SubMenu->X = x - 15; else menuEntry->SubMenu->X = 15;
            if( y > 15 ) menuEntry->SubMenu->Y = y - 15; else menuEntry->SubMenu->Y = 15;

            if( x > (glutGet( GLUT_WINDOW_WIDTH  ) - menuEntry->SubMenu->Width  - 15) )
                menuEntry->SubMenu->X = glutGet( GLUT_WINDOW_WIDTH  ) - menuEntry->SubMenu->Width  - 15;
            if( y > (glutGet( GLUT_WINDOW_HEIGHT ) - menuEntry->SubMenu->Height - 15) )
                menuEntry->SubMenu->Y = glutGet( GLUT_WINDOW_HEIGHT ) - menuEntry->SubMenu->Height - 15;

            /*
             * ...then check the submenu's state:
             */
            fghCheckMenuStatus( menuEntry->SubMenu );
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
 * Displays a menu box and all of it's submenus (if they are active)
 */
static void fghDisplayMenuBox( SFG_Menu* menu )
{
    SFG_Window* window = fgStructure.Window;
    gint i, j, x, y;

    /*
     * Have the menu box drawn first. The +- values are
     * here just to make it more nice-looking...
     */
    glColor4f( 0.0, 0.0, 0.0, 1.0 );
    glBegin( GL_QUADS );
        glVertex2f( menu->X - 8              , menu->Y - 1                );
        glVertex2f( menu->X + 8 + menu->Width, menu->Y - 1                );
        glVertex2f( menu->X + 8 + menu->Width, menu->Y + 4 + menu->Height );
        glVertex2f( menu->X - 8              , menu->Y + 4 + menu->Height );
    glEnd();

    glColor4f( 0.3, 0.4, 0.5, 1.0 );
    glBegin( GL_QUADS );
        glVertex2f( menu->X - 6              , menu->Y + 1                );
        glVertex2f( menu->X + 6 + menu->Width, menu->Y + 1                );
        glVertex2f( menu->X + 6 + menu->Width, menu->Y + 2 + menu->Height );
        glVertex2f( menu->X - 6              , menu->Y + 2 + menu->Height );
    glEnd();

    /*
     * Check if any of the submenus is currently active...
     */
    for( i=0; i<(gint) g_list_length( menu->Entries ); i++ )
    {
        SFG_MenuEntry* menuEntry = (SFG_MenuEntry *) g_list_nth( menu->Entries, i )->data;

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
            gint menuID = menuEntry->Ordinal;

            /*
             * So have the highlight drawn...
             */
            glColor4f( 0.2, 0.3, 0.4, 1.0 );
            glBegin( GL_QUADS );
                glVertex2f( menu->X - 6              , menu->Y + (menuID + 0)*FREEGLUT_MENU_HEIGHT + 1 );
                glVertex2f( menu->X + 6 + menu->Width, menu->Y + (menuID + 0)*FREEGLUT_MENU_HEIGHT + 1 );
                glVertex2f( menu->X + 6 + menu->Width, menu->Y + (menuID + 1)*FREEGLUT_MENU_HEIGHT + 2 );
                glVertex2f( menu->X - 6              , menu->Y + (menuID + 1)*FREEGLUT_MENU_HEIGHT + 2 );
            glEnd();
        }
    }

    /*
     * Print the menu entries now...
     */
    glColor4f( 1, 1, 1, 1 );

    for( i=0; i<(gint) g_list_length( menu->Entries ); i++ )
    {
        SFG_MenuEntry* menuEntry = (SFG_MenuEntry *) g_list_nth( menu->Entries, i )->data;

        /*
         * Move the raster into position...
         */
        glRasterPos2i(
            menu->X,
            menu->Y + (i + 1)*FREEGLUT_MENU_HEIGHT
        );

        /*
         * Have the label drawn, character after character:
         */
        for( j=0; j<menuEntry->Text->len; j++ )
            glutBitmapCharacter( FREEGLUT_MENU_FONT, (gint) menuEntry->Text->str[ j ] );
    }

    /*
     * Now we are ready to check if any of our children needs to be redrawn:
     */
    for( i=0; i<(gint) g_list_length( menu->Entries ); i++ )
    {
        SFG_MenuEntry* menuEntry = (SFG_MenuEntry *) g_list_nth( menu->Entries, i )->data;

        /*
         * Is that an active sub menu by any case?
         */
        if( menuEntry->SubMenu != NULL && menuEntry->IsActive == TRUE )
        {
            /*
             * Yeah, indeed. Have it redrawn now:
             */
            fghDisplayMenuBox( menuEntry->SubMenu );
        }
    }
}

/*
 * Displays the currently active menu for the current window
 */
void fgDisplayMenu( void )
{
    SFG_Window* window = fgStructure.Window;
    SFG_Menu* menu = NULL;
    gint i;

    /*
     * Make sure there is a current window available
     */
    freeglut_assert_window;

    /*
     * Check if there is an active menu attached to this window...
     */
    for( i=0; i<FREEGLUT_MAX_MENUS; i++ )
    {
        if( window->Menu[ i ] != NULL && window->MenuActive[ i ] == TRUE )
            menu = window->Menu[ i ];
    };

    /*
     * Did we find an active window?
     */
    freeglut_return_if_fail( menu != NULL );

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
    fghCheckMenuStatus( menu );

    /*
     * The status has been updated and we're ready to have the menu drawn now:
     */
    fghDisplayMenuBox( menu );

    /*
     * Restore the old OpenGL settings now
     */
    glPopAttrib();

    glMatrixMode( GL_MODELVIEW );
    glPopMatrix();
    glMatrixMode( GL_PROJECTION );
    glPopMatrix();
}

/*
 * Activates a menu pointed by the function argument
 */
void fgActivateMenu( gint button )
{
    SFG_Window* window = fgStructure.Window;
    SFG_Menu* menu = NULL;
    gint x, y;

    freeglut_assert_window;

    /*
     * Mark the menu as active, so that it gets displayed:
     */
    window->MenuActive[ button ] = TRUE;

    /*
     * We'll be referencing this menu a lot, so remember it's address:
     */
    menu = window->Menu[ button ];

    /*
     * Grab the mouse cursor position respective to the current window
     */
    x = window->State.MouseX;
    y = window->State.MouseY;

    /*
     * Set up the initial menu position now:
     */
    if( x > 10 ) menu->X = x - 10; else menu->X = 5;
    if( y > 10 ) menu->Y = y - 10; else menu->Y = 5;

    if( x > (glutGet( GLUT_WINDOW_WIDTH  ) - menu->Width ) )
        menu->X = glutGet( GLUT_WINDOW_WIDTH  ) - menu->Width;
    if( y > (glutGet( GLUT_WINDOW_HEIGHT ) - menu->Height) )
        menu->Y = glutGet( GLUT_WINDOW_HEIGHT ) - menu->Height;
}

/*
 * Private static function to check for menu entry selection on menu deactivation
 */
static void fghCheckMenuSelect( SFG_Menu* menu )
{
    gint i;

    /*
     * First of all check any of the active sub menus...
     */
    for( i=0; i<(gint) g_list_length( menu->Entries ); i++ )
    {
        SFG_MenuEntry* menuEntry = (SFG_MenuEntry *) g_list_nth( menu->Entries, i )->data;

        /*
         * Is this menu entry active?
         */
        if( menuEntry->IsActive == TRUE )
        {
            /*
             * If this is not a sub menu, execute the menu callback and return...
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
            fghCheckMenuSelect( menuEntry->SubMenu );

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
void fgDeactivateMenu( gint button )
{
    SFG_Window* window = fgStructure.Window;
    SFG_Menu* menu = NULL;
    gint i, x, y;

    /*
     * Make sure there is a current window available...
     */
    freeglut_assert_window;

    /*
     * Check if there is an active menu attached to this window...
     */
    for( i=0; i<FREEGLUT_MAX_MENUS; i++ )
    {
        if( window->Menu[ i ] != NULL && window->MenuActive[ i ] == TRUE )
            menu = window->Menu[ i ];
    };

    /*
     * Did we find an active window?
     */
    freeglut_return_if_fail( menu != NULL );

    /*
     * Check if there was any menu entry active. This would
     * mean the user has selected a menu entry...
     */
    fghCheckMenuSelect( menu );

    /*
     * Forget about having that menu active anymore, now:
     */
    fgStructure.Window->MenuActive[ button ] = FALSE;
}

/*
 * Recalculates current menu's box size
 */
void fghCalculateMenuBoxSize( void )
{
    gint i, width;

    /*
     * Make sure there is a current menu set
     */
    freeglut_assert_ready; freeglut_return_if_fail( fgStructure.Menu != NULL );

    /*
     * The menu's box size depends on the menu entries:
     */
    for( i=0, width=0; i<(gint) g_list_length( fgStructure.Menu->Entries ); i++ )
    {
        SFG_MenuEntry* menuEntry = (SFG_MenuEntry *) g_list_nth( fgStructure.Menu->Entries, i )->data;

        /*
         * Update the menu entry's width value
         */
        menuEntry->Width = glutBitmapLength( FREEGLUT_MENU_FONT, menuEntry->Text->str );

        /*
         * Check if it's the biggest we've found
         */
        if( menuEntry->Width > width )
            width = menuEntry->Width;
    }

    /*
     * Store the menu's box size now:
     */
    fgStructure.Menu->Height = i * FREEGLUT_MENU_HEIGHT;
    fgStructure.Menu->Width  = width;
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
 * Sets the current menu given it's menu ID
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
    SFG_MenuEntry* menuEntry = g_new0( SFG_MenuEntry, 1 );

    /*
     * Make sure there is a current menu set
     */
    freeglut_assert_ready; freeglut_return_if_fail( fgStructure.Menu != NULL );

    /*
     * Fill in the appropriate values...
     */
    menuEntry->Text = g_string_new( label );
    menuEntry->ID   = value;

    /*
     * Have the new menu entry attached to the current menu
     */
    fgStructure.Menu->Entries = g_list_append( fgStructure.Menu->Entries, menuEntry );

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
    SFG_MenuEntry* menuEntry = g_new0( SFG_MenuEntry, 1 );
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
    menuEntry->Text    = g_string_new( label );
    menuEntry->SubMenu = subMenu;
    menuEntry->ID      = -1;

    /*
     * Have the new menu entry attached to the current menu
     */
    fgStructure.Menu->Entries = g_list_append( fgStructure.Menu->Entries, menuEntry );

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
     * Make sure the item counter seems valid
     */
    freeglut_return_if_fail( (item > 0) && (item <= (gint) g_list_length( fgStructure.Menu->Entries ) ) );

    /*
     * Get n-th menu entry in the current menu, starting from one:
     */
    menuEntry = (SFG_MenuEntry *) g_list_nth( fgStructure.Menu->Entries, item - 1 )->data;

    /*
     * We want it to become a normal menu entry, so:
     */
    if( menuEntry->Text != NULL )
        g_string_free( menuEntry->Text, TRUE );

    menuEntry->Text    = g_string_new( label );
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
     * Make sure the item counter seems valid
     */
    freeglut_return_if_fail( (item > 0) && (item <= (gint) g_list_length( fgStructure.Menu->Entries ) ) );

    /*
     * Get n-th menu entry in the current menu, starting from one:
     */
    menuEntry = (SFG_MenuEntry *) g_list_nth( fgStructure.Menu->Entries, item - 1 )->data;

    /*
     * We want it to become a sub menu entry, so:
     */
    if( menuEntry->Text != NULL )
        g_string_free( menuEntry->Text, TRUE );

    menuEntry->Text    = g_string_new( label );
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
     * Make sure the item counter seems valid
     */
    freeglut_return_if_fail( (item > 0) && (item <= (gint) g_list_length( fgStructure.Menu->Entries ) ) );

    /*
     * Removing a menu entry is quite simple...
     */
    menuEntry = (SFG_MenuEntry *) g_list_nth( fgStructure.Menu->Entries, item - 1 )->data;

    fgStructure.Menu->Entries = g_list_remove(
        fgStructure.Menu->Entries,
        menuEntry
    );

    /*
     * Free the entry label string, too
     */
    g_string_free( menuEntry->Text, TRUE );

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

/*** END OF FILE ***/
