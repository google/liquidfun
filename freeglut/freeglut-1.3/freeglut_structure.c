/*
 * freeglut_structure.c
 *
 * Windows and menus need tree structure
 *
 * Copyright (c) 1999-2000 Pawel W. Olszta. All Rights Reserved.
 * Written by Pawel W. Olszta, <olszta@sourceforge.net>
 * Creation date: Sat Dec 18 1999
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

#define  G_LOG_DOMAIN  "freeglut-structure"

#include "../include/GL/freeglut.h"
#include "../include/GL/freeglut_internal.h"


/* -- GLOBAL EXPORTS ------------------------------------------------------- */

/*
 * The SFG_Structure container holds information about windows and menus
 * created between glutInit() and glutMainLoop() return.
 */
SFG_Structure fgStructure;


/* -- PRIVATE FUNCTIONS ---------------------------------------------------- */

/*
 * This private function creates, opens and adds to the hierarchy
 * a freeglut window complete with OpenGL context and stuff...
 *
 * If parent is set to NULL, the window created will be a topmost one.
 */
SFG_Window* fgCreateWindow( SFG_Window* parent, const gchar* title, gint x, gint y, gint w, gint h, gboolean gameMode )
{
    /*
     * Have the window object created
     */
    SFG_Window* window = g_new0( SFG_Window, 1 );
    gint fakeArgc = 0;

    /*
     * If the freeglut internals haven't been initialized yet,
     * do it now. Hack's idea courtesy of Chris Purnell...
     */
    if( fgState.Timer == NULL )
        glutInit( &fakeArgc, NULL );

    /*
     * Initialize the object properties
     */
    window->ID = ++fgStructure.WindowID;

    /*
     * Does this window have a parent?
     */
    if( parent != NULL )
    {
        /*
         * That's quite right, attach this windows as a child window
         */
        parent->Children = g_list_append( parent->Children, window );
        window->Parent = parent;
    }
    else
    {
        /*
         * Otherwise add the newly created window to the topmost windows list
         */
        fgStructure.Windows = g_list_append( fgStructure.Windows, window );
    }

    /*
     * Set the default mouse cursor and reset the modifiers value
     */
    window->State.Cursor    = GLUT_CURSOR_INHERIT;
    window->State.Modifiers = 0xffffffff;

    /*
     * Open the window now. The fgOpenWindow() function is system
     * dependant, and resides in freeglut_window.c. Uses fgState.
     */
    fgOpenWindow( window, title, x, y, w, h, gameMode );

    /*
     * Return a pointer to the newly created window
     */
    return( window );
}

/*
 * This private function creates a menu and adds it to the menus list
 */
SFG_Menu* fgCreateMenu( FGCBmenu menuCallback )
{
    /*
     * Have the menu object created
     */
    SFG_Menu* menu = g_new0( SFG_Menu, 1 );
    gint fakeArgc = 0;

    /*
     * If the freeglut internals haven't been initialized yet,
     * do it now. Hack's idea courtesy of Chris Purnell...
     */
    if( fgState.Timer == NULL )
        glutInit( &fakeArgc, NULL );

    /*
     * Initialize the object properties:
     */
    menu->ID       = ++fgStructure.MenuID;
    menu->Callback = menuCallback;

    /*
     * Add it to the menu structure hierarchy
     */
    fgStructure.Menus = g_list_append( fgStructure.Menus, menu );

    /*
     * Newly created menus implicitly become current ones
     */
    fgStructure.Menu = menu;

    /*
     * Return the result to the caller
     */
    return( menu );
}

/*
 * This function destroys a window and all of it's subwindows. Actually,
 * another function, defined in freeglut_window.c is called, but this is
 * a whole different story...
 */
void fgDestroyWindow( SFG_Window* window, gboolean needToClose )
{
    int i;

    g_assert( window != NULL );
    freeglut_assert_ready;

    /*
     * Does this window have any subwindows?
     */
    if( window->Children != NULL )
    {
        /*
         * OKi, while there are any subwindows left...
         */
        while( g_list_first( window->Children ) != NULL )
        {
            SFG_Window* subWindow = g_list_first( window->Children )->data;

            /*
             * Destroy the first window in the list (possibly destroying
             * it's subwindows too. This is not very effective, but works
             */
            fgDestroyWindow( subWindow, TRUE );

            /*
             * Remove the just destroyed node from the subwindows list
             */
            window->Children = g_list_remove( window->Children, subWindow );
        }

        /*
         * Have the list freed now (probably it already is, but you can
         * never be sure with no GLib documentation on your hdd...)
         */
        g_list_free( window->Children );
        window->Children = NULL;
    }

    /*
     * Now we should remove the reference to this window from it's parent
     */
    if( window->Parent != NULL )
        window->Parent->Children = g_list_remove( window->Parent->Children, window );

    /*
     * OK, this window seems disconnected from the structure enough
     * in order to be closed without any bigger risks...
     */
	if( needToClose == TRUE )
		fgCloseWindow( window );

    /*
     * Try removing the window from the parents list in fgStructure.
     * This might fail as the window is not guaranteed to be there:
     */
    fgStructure.Windows = g_list_remove( fgStructure.Windows, window );

    /*
     * Check if the window is the current one...
     */
    if( fgStructure.Window == window )
        fgStructure.Window = NULL;

    /*
     * Finally, we can delete the window's object. It hopefully does
     * have everything inside it freed and we do not have to care...
     */
    g_free( window );
}

/*
 * This is a helper static function that removes a menu (given it's pointer)
 * from any windows that can be accessed from a given parent...
 */
static void fghRemoveMenuFromWindow( SFG_Window* window, SFG_Menu* menu )
{
    gint i;

    /*
     * Check if the menu is attached to the current window,
     * if so, have it detached (by overwriting with a NULL):
     */
    for( i=0; i<3; i++ )
        if( window->Menu[ i ] == menu )
            window->Menu[ i ] = NULL;

    /*
     * Call this function for all of the window's children recursively:
     */
    for( i=0; i<(gint) g_list_length( window->Children ); i++ )
        fghRemoveMenuFromWindow(
            (SFG_Window *) g_list_nth( window->Children, i )->data,
            menu
        );
}

/*
 * This is a static helper function that removes menu references
 * from another menu, given two pointers to them...
 */
static void fghRemoveMenuFromMenu( SFG_Menu* from, SFG_Menu* menu )
{
    gboolean found = FALSE;

    /*
     * Do not allow removing a menu from itself...
     */
    if( from == menu )
        return;

    /*
     * Look up for the first entry that matches the given
     * menu and have it removed, then search again and again:
     */
    do
    {
        /*
         * Try searching for the incriminated menu entry
         */
        GList* where = g_list_find( from->Entries, menu );

        /*
         * Make sure we still have a list to be searched
         */
        if( where != NULL )
        {
            /*
             * Did we actually find the menu entry we want to remove?
             */
            found = ((SFG_Menu *) where->data == menu);

            /*
             * Need to check that, as the search might have failed
             */
            if( found )
                from->Entries = g_list_remove( from->Entries, menu );
        }
        else
        {
            /*
             * It would be nice if we had a stop rule ;-)
             */
            found = FALSE;
        }
    } while( found == TRUE );
}

/*
 * This function destroys a menu specified by the parameter. All menus
 * and windows are updated to make sure no ill pointers hang around.
 */
void fgDestroyMenu( SFG_Menu* menu )
{
    gint i;

    g_assert( menu != NULL );
    freeglut_assert_ready;

    /*
     * First of all, have all references to this menu removed from all windows:
     */
    for( i=0; i<(gint) g_list_length( fgStructure.Windows ); i++ )
        fghRemoveMenuFromWindow(
            (SFG_Window *) g_list_nth( fgStructure.Windows, i )->data,
            menu
        );

    /*
     * Now proceed with removing menu entries that lead to this menu
     */
    for( i=0; i<(gint) g_list_length( fgStructure.Menus ); i++ )
        fghRemoveMenuFromMenu(
            (SFG_Menu *) g_list_nth( fgStructure.Menus, i )->data,
            menu
        );

    /*
     * Now we are pretty sure the menu is not used anywhere
     * and that we can remove all of it's entries
     */
    for( i=0; i<(gint) g_list_length( menu->Entries ); i++ )
    {
        SFG_MenuEntry* entry = (SFG_MenuEntry *) g_list_nth( menu->Entries, i )->data;

        /*
         * There might be a string allocated, have it freed:
         */
        g_string_free( entry->Text, TRUE );

        /*
         * Deallocate the entry itself:
         */
        g_free( entry );
    }

    /*
     * Deallocate the entries list
     */
    g_list_free( menu->Entries );
    menu->Entries = NULL;

    /*
     * Remove the menu from the menus list
     */
    fgStructure.Menus = g_list_remove( fgStructure.Menus, menu );

    /*
     * If that menu was the current one...
     */
    if( fgStructure.Menu == menu )
        fgStructure.Menu = NULL;

    /*
     * Have the menu structure freed
     */
    g_free( menu );
}

/*
 * This function should be called on glutInit(). It will prepare the internal
 * structure of freeglut to be used in the application. The structure will be
 * destroyed using fgDestroyStructure() on glutMainLoop() return. In that
 * case further use of freeglut should be preceeded with a glutInit() call.
 */
void fgCreateStructure( void )
{
    /*
     * We will be needing two lists: the first containing windows, and the second
	 * containing the user-defined menus. However we do not need allocating anything, 
	 * as it is done automagically by GLib when appending new entries to both of them. 
	 * Also, no current window/menu is set, as none has been created yet.
     */
}

/*
 * This function is automatically called on glutMainLoop() return. It should deallocate 
 * and destroy all remnants of previous glutInit()-enforced structure initialization...
 */
void fgDestroyStructure( void )
{
	/*
	 * Just make sure we are not called in vain...
	 */
    freeglut_assert_ready;

    /*
     * Make sure all windows and menus have been deallocated
     */
    while( fgStructure.Windows != NULL )
        fgDestroyWindow( (SFG_Window *) g_list_first( fgStructure.Windows )->data, TRUE );

    while( fgStructure.Menus != NULL )
        fgDestroyMenu( (SFG_Menu *) g_list_first( fgStructure.Menus )->data );
}

/*
 * Helper function to enumerate through all registered top-level windows
 */
void fgEnumWindows( GFunc enumCallback, SFG_Enumerator* enumerator )
{
    gint i;

    g_assert( (enumCallback != NULL) && (enumerator != NULL) );
    freeglut_assert_ready;

    /*
     * Check every of the top-level windows
     */
    for( i=0; i<(gint) g_list_length( fgStructure.Windows ); i++ )
    {
        /*
         * Execute the callback...
         */
        enumCallback( (gpointer) g_list_nth( fgStructure.Windows, i )->data, (gpointer) enumerator );

        /*
         * If it has been marked as 'found', stop searching
         */
        if( enumerator->found == TRUE )
            return;
    }
}

/*
 * Helper function to enumerate through all a window's subwindows (single level descent)
 */
void fgEnumSubWindows( SFG_Window* window, GFunc enumCallback, SFG_Enumerator* enumerator )
{
    gint i;

    g_assert( (enumCallback != NULL) && (enumerator != NULL) );
    freeglut_assert_ready;

    /*
     * Check every of the window's children:
     */
    for( i=0; i<(gint) g_list_length( window->Children ); i++ )
    {
        /*
         * Execute the callback...
         */
        enumCallback( (gpointer) g_list_nth( window->Children, i )->data, (gpointer) enumerator );

        /*
         * If it has been marked as 'found', stop searching
         */
        if( enumerator->found == TRUE )
            return;
    }
}

/*
 * A static helper function to look for a window given it's handle
 */
static void fghcbWindowByHandle( gpointer window, gpointer enumerator )
{
    /*
     * Make sure we do not overwrite our precious results...
     */
    freeglut_return_if_fail( ((SFG_Enumerator *) enumerator)->found == FALSE );

#if TARGET_HOST_UNIX_X11
    #define WBHANDLE (Window)
#elif TARGET_HOST_WIN32
    #define WBHANDLE (HWND)
#endif

    /*
     * Check the window's handle. Hope this works. Looks ugly. That's for sure.
     */
    if( ((SFG_Window *) window)->Window.Handle == (WBHANDLE ((SFG_Enumerator *) enumerator)->data) )
    {
        ((SFG_Enumerator *) enumerator)->found = TRUE;
        ((SFG_Enumerator *) enumerator)->data = (gpointer) window;

        return;
    }

    /*
     * Otherwise, check this window's children
     */
    fgEnumSubWindows( (SFG_Window *) window, fghcbWindowByHandle, enumerator );

#undef WBHANDLE
}

/*
 * fgWindowByHandle returns a (SFG_Window *) value pointing to the
 * first window in the queue matching the specified window handle.
 * The function is defined in freeglut_structure.c file.
 */
SFG_Window* fgWindowByHandle
#if TARGET_HOST_UNIX_X11
( Window hWindow )
#elif TARGET_HOST_WIN32
( HWND hWindow )
#endif
{
    SFG_Enumerator enumerator;

    /*
     * This is easy and makes use of the windows enumeration defined above
     */
    enumerator.found = FALSE;
    enumerator.data = (gpointer) hWindow;

    /*
     * Start the enumeration now:
     */
    fgEnumWindows( fghcbWindowByHandle, &enumerator );

    /*
     * Check if the window has been found or not:
     */
    if( enumerator.found == TRUE )
        return( (SFG_Window *) enumerator.data );

    /*
     * Otherwise return NULL to mark the failure
     */
    return( NULL );
}

/*
 * A static helper function to look for a window given it's ID
 */
static void fghcbWindowByID( gpointer window, gpointer enumerator )
{
    /*
     * Make sure we do not overwrite our precious results...
     */
    g_return_if_fail( ((SFG_Enumerator *) enumerator)->found == FALSE );

    /*
     * Check the window's handle. Hope this works. Looks ugly. That's for sure.
     */
    if( ((SFG_Window *) window)->ID == (gint) (((SFG_Enumerator *) enumerator)->data) )
    {
        ((SFG_Enumerator *) enumerator)->found = TRUE;
        ((SFG_Enumerator *) enumerator)->data = (gpointer) window;

        return;
    }

    /*
     * Otherwise, check this window's children
     */
    fgEnumSubWindows( (SFG_Window *) window, fghcbWindowByID, enumerator );
}

/*
 * This function is similiar to the previous one, except it is
 * looking for a specified (sub)window identifier. The function
 * is defined in freeglut_structure.c file.
 */
SFG_Window* fgWindowByID( gint windowID )
{
    SFG_Enumerator enumerator;

    /*
     * Uses a method very similiar for fgWindowByHandle...
     */
    enumerator.found = FALSE;
    enumerator.data = (gpointer) windowID;

    /*
     * Start the enumeration now:
     */
    fgEnumWindows( fghcbWindowByID, &enumerator );

    /*
     * Check if the window has been found or not:
     */
    if( enumerator.found == TRUE )
        return( (SFG_Window *) enumerator.data );

    /*
     * Otherwise return NULL to mark the failure
     */
    return( NULL );
}

/*
 * Looks up a menu given it's ID. This is easier that fgWindowByXXX
 * as all menus are placed in a single doubly linked list...
 */
SFG_Menu* fgMenuByID( gint menuID )
{
    SFG_Menu *menu = NULL;
    gint i;

    freeglut_assert_ready;

    /*
     * It's enough to check all entries in fgStructure.Menus...
     */
    for( i=0; i<(gint) g_list_length( fgStructure.Menus ); i++ )
    {
        /*
         * Grab the n-th element of the menu objects list...
         */
        menu = (SFG_Menu *) g_list_nth( fgStructure.Menus, i )->data;

        /*
         * Does the ID number match?
         */
        if( menu->ID == menuID )
            return( menu );
    }

    /*
     * We have not found the requested menu ID
     */
    return( NULL );
}

/*** END OF FILE ***/









