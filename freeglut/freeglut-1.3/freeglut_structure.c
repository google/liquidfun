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
SFG_Window* fgCreateWindow( SFG_Window* parent, const char* title, int x, int y, int w, int h, GLboolean gameMode )
{
    /*
     * Have the window object created
     */
    SFG_Window* window = calloc( sizeof(SFG_Window), 1 );
    int fakeArgc = 0;

    /*
     * If the freeglut internals haven't been initialized yet,
     * do it now. Hack's idea courtesy of Chris Purnell...
     */
    if( !fgState.Time.Set )
        glutInit( &fakeArgc, NULL );

    /*
     * Initialize the object properties
     */
    window->ID = ++fgStructure.WindowID;

    /*
     * Initialize the children list
     */
    fgListInit( &window->Children );

    /*
     * Does this window have a parent?
     */
    if( parent != NULL )
    {
        /*
         * That's quite right, attach this windows as a child window
         */
	fgListAppend( &parent->Children, &window->Node );
        window->Parent = parent;
    }
    else
    {
        /*
         * Otherwise add the newly created window to the topmost windows list
         */
	fgListAppend( &fgStructure.Windows, &window->Node );
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
    SFG_Menu* menu = calloc( sizeof(SFG_Menu), 1 );
    int fakeArgc = 0;

    /*
     * If the freeglut internals haven't been initialized yet,
     * do it now. Hack's idea courtesy of Chris Purnell...
     */
    if( !fgState.Time.Set )
        glutInit( &fakeArgc, NULL );

    /*
     * Initialize the object properties:
     */
    menu->ID       = ++fgStructure.MenuID;
    menu->Callback = menuCallback;

    /*
     * Initialize the entries list
     */
    fgListInit( &menu->Entries );

    /*
     * Add it to the menu structure hierarchy
     */
    fgListAppend( &fgStructure.Menus, &menu->Node );

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
void fgDestroyWindow( SFG_Window* window, GLboolean needToClose )
{
    SFG_Window* subWindow;

    assert( window != NULL );
    freeglut_assert_ready;

    /*
     * Does this window have any subwindows?
     */
    if( (subWindow = window->Children.First) != NULL )
    {
        /*
         * Destroy the first window in the list (possibly destroying
         * it's subwindows too). This is not very effective, but works
         */
        fgDestroyWindow( subWindow, TRUE );
    }

    /*
     * Now we should remove the reference to this window from it's parent
     */
    if( window->Parent != NULL )
        fgListRemove( &window->Parent->Children, &window->Node );
    else
        fgListRemove( &fgStructure.Windows, &window->Node );

    /*
     * OK, this window seems disconnected from the structure enough
     * in order to be closed without any bigger risks...
     */
    if( needToClose == TRUE )
        fgCloseWindow( window );

    /*
     * Check if the window is the current one...
     */
    if( fgStructure.Window == window )
        fgStructure.Window = NULL;

    /*
     * Finally, we can delete the window's object. It hopefully does
     * have everything inside it freed and we do not have to care...
     */
    free( window );
}

/*
 * This is a helper static function that removes a menu (given it's pointer)
 * from any windows that can be accessed from a given parent...
 */
static void fghRemoveMenuFromWindow( SFG_Window* window, SFG_Menu* menu )
{
    SFG_Window *subWindow;
    int i;

    /*
     * Check if the menu is attached to the current window,
     * if so, have it detached (by overwriting with a NULL):
     */
    for( i=0; i<3; i++ )
    {
        if( window->Menu[ i ] == menu )
            window->Menu[ i ] = NULL;
    }

    /*
     * Call this function for all of the window's children recursively:
     */
    for( subWindow = window->Children.First; subWindow;
         subWindow = subWindow->Node.Next)
    {
        fghRemoveMenuFromWindow( subWindow, menu );
    }
}

/*
 * This is a static helper function that removes menu references
 * from another menu, given two pointers to them...
 */
static void fghRemoveMenuFromMenu( SFG_Menu* from, SFG_Menu* menu )
{
    SFG_MenuEntry *entry;

    for( entry = from->Entries.First; entry; entry = entry->Node.Next )
    {
        if (entry->SubMenu == menu)
        {
                entry->SubMenu = NULL;
        }
    }
}

/*
 * This function destroys a menu specified by the parameter. All menus
 * and windows are updated to make sure no ill pointers hang around.
 */
void fgDestroyMenu( SFG_Menu* menu )
{
    SFG_Window *window;
    SFG_Menu *from;
    SFG_MenuEntry *entry;

    assert( menu != NULL );
    freeglut_assert_ready;

    /*
     * First of all, have all references to this menu removed from all windows:
     */
    for( window = fgStructure.Windows.First; window;
	 window = window->Node.Next )
    {
        fghRemoveMenuFromWindow( window, menu );
    }

    /*
     * Now proceed with removing menu entries that lead to this menu
     */
    for( from = fgStructure.Menus.First; from; from = from->Node.Next )
    {
        fghRemoveMenuFromMenu( from, menu );
    }

    /*
     * Now we are pretty sure the menu is not used anywhere
     * and that we can remove all of it's entries
     */
    while( (entry = menu->Entries.First) != NULL )
    {
	fgListRemove(&menu->Entries, &entry->Node);

        /*
         * There might be a string allocated, have it freed:
         */
        free( entry->Text );

        /*
         * Deallocate the entry itself:
         */
        free( entry );
    }

    /*
     * Remove the menu from the menus list
     */
    fgListRemove( &fgStructure.Menus, &menu->Node );

    /*
     * If that menu was the current one...
     */
    if( fgStructure.Menu == menu )
        fgStructure.Menu = NULL;

    /*
     * Have the menu structure freed
     */
    free( menu );
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
     * We will be needing two lists: the first containing windows,
     * and the second containing the user-defined menus.
     * Also, no current window/menu is set, as none has been created yet.
     */

   fgListInit(&fgStructure.Windows);
   fgListInit(&fgStructure.Menus);
}

/*
 * This function is automatically called on glutMainLoop() return. It should deallocate 
 * and destroy all remnants of previous glutInit()-enforced structure initialization...
 */
void fgDestroyStructure( void )
{
    SFG_Window *window;
    SFG_Menu *menu;

    /*
     * Just make sure we are not called in vain...
     */
    freeglut_assert_ready;

    /*
     * Make sure all windows and menus have been deallocated
     */
    while( (window = fgStructure.Windows.First) != NULL )
        fgDestroyWindow( window, TRUE );

    while( (menu = fgStructure.Menus.First) != NULL )
        fgDestroyMenu( menu );
}

/*
 * Helper function to enumerate through all registered top-level windows
 */
void fgEnumWindows( FGCBenumerator enumCallback, SFG_Enumerator* enumerator )
{
    SFG_Window *window;

    assert( (enumCallback != NULL) && (enumerator != NULL) );
    freeglut_assert_ready;

    /*
     * Check every of the top-level windows
     */
    for( window = fgStructure.Windows.First; window;
         window = window->Node.Next )
    {
        /*
         * Execute the callback...
         */
        enumCallback( window, enumerator );

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
void fgEnumSubWindows( SFG_Window* window, FGCBenumerator enumCallback, SFG_Enumerator* enumerator )
{
    SFG_Window *child;

    assert( (enumCallback != NULL) && (enumerator != NULL) );
    freeglut_assert_ready;

    /*
     * Check every of the window's children:
     */
    for( child = window->Children.First; child; child = child->Node.Next )
    {
        /*
         * Execute the callback...
         */
        enumCallback( child, enumerator );

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
static void fghcbWindowByHandle( SFG_Window *window, SFG_Enumerator *enumerator )
{
    /*
     * Make sure we do not overwrite our precious results...
     */
    freeglut_return_if_fail( enumerator->found == FALSE );

#if TARGET_HOST_UNIX_X11
    #define WBHANDLE (Window)
#elif TARGET_HOST_WIN32
    #define WBHANDLE (HWND)
#endif

    /*
     * Check the window's handle. Hope this works. Looks ugly. That's for sure.
     */
    if( window->Window.Handle == WBHANDLE (enumerator->data) )
    {
        enumerator->found = TRUE;
        enumerator->data = window;

        return;
    }

    /*
     * Otherwise, check this window's children
     */
    fgEnumSubWindows( window, fghcbWindowByHandle, enumerator );

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
    enumerator.data = (void *)hWindow;

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
static void fghcbWindowByID( SFG_Window *window, SFG_Enumerator *enumerator )
{
    /*
     * Make sure we do not overwrite our precious results...
     */
    freeglut_return_if_fail( enumerator->found == FALSE );

    /*
     * Check the window's handle. Hope this works. Looks ugly. That's for sure.
     */
    if( window->ID == (int) (enumerator->data) )
    {
        enumerator->found = TRUE;
        enumerator->data = window;

        return;
    }

    /*
     * Otherwise, check this window's children
     */
    fgEnumSubWindows( window, fghcbWindowByID, enumerator );
}

/*
 * This function is similiar to the previous one, except it is
 * looking for a specified (sub)window identifier. The function
 * is defined in freeglut_structure.c file.
 */
SFG_Window* fgWindowByID( int windowID )
{
    SFG_Enumerator enumerator;

    /*
     * Uses a method very similiar for fgWindowByHandle...
     */
    enumerator.found = FALSE;
    enumerator.data = (void *) windowID;

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
SFG_Menu* fgMenuByID( int menuID )
{
    SFG_Menu *menu = NULL;

    freeglut_assert_ready;

    /*
     * It's enough to check all entries in fgStructure.Menus...
     */
    for( menu = fgStructure.Menus.First; menu; menu = menu->Node.Next )
    {
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

/*
 * List functions...
 */
void fgListInit(SFG_List *list)
{
    list->First = NULL;
    list->Last = NULL;
}

void fgListAppend(SFG_List *list, SFG_Node *node)
{
    SFG_Node *ln;

    if ( (ln = list->Last) != NULL )
    {
        ln->Next = node;
        node->Prev = ln;
    }
    else
    {
        node->Prev = NULL;
        list->First = node;
    }

    node->Next = NULL;
    list->Last = node;
}

void fgListRemove(SFG_List *list, SFG_Node *node)
{
    SFG_Node *ln;

    if ( (ln = node->Next) != NULL )
        ln->Prev = node->Prev;
    if ( (ln = node->Prev) != NULL )
        ln->Next = node->Next;
    if ( (ln = list->First) == node )
        list->First = node->Next;
    if ( (ln = list->Last) == node )
        list->Last = node->Prev;
}

int fgListLength(SFG_List *list)
{
    SFG_Node *node;
    int length = 0;

    for( node = list->First; node; node = node->Next )
        ++length;

    return( length );
}

/*** END OF FILE ***/
