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

#include "../include/GL/freeglut.h"
#include "freeglut_internal.h"


/* -- GLOBAL EXPORTS ------------------------------------------------------- */

/*
 * The SFG_Structure container holds information about windows and menus
 * created between glutInit() and glutMainLoop() return.
 */

SFG_Structure fgStructure = { { NULL, NULL },  /* The list of windows       */
                              { NULL, NULL },  /* The list of menus         */
                              NULL,            /* The current window        */
                              NULL,            /* The current menu          */
                              NULL,            /* The menu OpenGL context   */
                              NULL,            /* The game mode window      */
                              0,               /* The current new window ID */
                              0 };             /* The current new menu ID   */


/* -- PRIVATE FUNCTIONS ---------------------------------------------------- */

void fgClearCallBacks( SFG_Window *window )
{
    if( window )
    {
        int i;
        for( i = 0; i < TOTAL_CALLBACKS; ++i )
            window->CallBacks[ i ] = NULL;
    }
}

/*
 * This private function creates, opens and adds to the hierarchy
 * a freeglut window complete with OpenGL context and stuff...
 *
 * If parent is set to NULL, the window created will be a topmost one.
 */
SFG_Window* fgCreateWindow( SFG_Window* parent, const char* title,
                            int x, int y, int w, int h,
                            GLboolean gameMode, GLboolean isMenu )
{
    /*
     * Have the window object created
     */
    SFG_Window *window = (SFG_Window *)calloc( sizeof(SFG_Window), 1 );
    int fakeArgc = 0;

    fgClearCallBacks( window );

    /*
     * If the freeglut internals haven't been initialized yet,
     * do it now. Hack's idea courtesy of Chris Purnell...
     */
    if( !fgState.Initialised )
        glutInit( &fakeArgc, NULL );

    /*
     * Initialize the object properties
     */
    window->ID = ++fgStructure.WindowID;

    fgListInit( &window->Children );
    if( parent )
    {
        fgListAppend( &parent->Children, &window->Node );
        window->Parent = parent;
    }
    else
        fgListAppend( &fgStructure.Windows, &window->Node );

    /*
     * Set the default mouse cursor and reset the modifiers value
     */
    window->State.Cursor    = GLUT_CURSOR_INHERIT;

    window->IsMenu = isMenu;

    /*
     * Open the window now. The fgOpenWindow() function is system
     * dependant, and resides in freeglut_window.c. Uses fgState.
     */
    fgOpenWindow( window, title, x, y, w, h, gameMode,
                  parent ? GL_TRUE : GL_FALSE );

    return window;
}

/*
 * This private function creates a menu and adds it to the menus list
 */
SFG_Menu* fgCreateMenu( FGCBMenu menuCallback )
{
    int x = 100, y = 100, w = 100, h = 100;
    SFG_Window *current_window = fgStructure.Window;

    /*
     * Have the menu object created
     */
    SFG_Menu* menu = (SFG_Menu *)calloc( sizeof(SFG_Menu), 1 );
    int fakeArgc = 0;

    /*
     * If the freeglut internals haven't been initialized yet,
     * do it now. Hack's idea courtesy of Chris Purnell...
     */
    if( !fgState.Initialised )
        glutInit( &fakeArgc, NULL );

    menu->ParentWindow = fgStructure.Window;

    /*
     * Create a window for the menu to reside in.
     */

    fgCreateWindow( NULL, NULL, x, y, w, h, GL_FALSE, GL_TRUE );
    menu->Window = fgStructure.Window;
    glutDisplayFunc( fgDisplayMenu );

    glutHideWindow( );  /* Hide the window for now */
    fgSetWindow( current_window );

    /*
     * Initialize the object properties:
     */
    menu->ID       = ++fgStructure.MenuID;
    menu->Callback = menuCallback;
    menu->ActiveEntry = NULL;

    fgListInit( &menu->Entries );
    fgListAppend( &fgStructure.Menus, &menu->Node );

    /*
     * Newly created menus implicitly become current ones
     */
    fgStructure.Menu = menu;

    return menu;
}

/*
 * Linked list of windows to destroy ... this is so we don't destroy a
 * window from the middle of its callback.  Some C compilers take an
 * extremely dim view of this.
 */

static SFG_WindowList* WindowsToDestroy = ( SFG_WindowList* )NULL;

/*
 * Function to add a window to the linked list of windows to destroy.
 * Subwindows are automatically added because they hang from the window
 * structure.
 */
void fgAddToWindowDestroyList( SFG_Window* window, GLboolean needToClose )
{
    SFG_WindowList *new_list_entry =
        ( SFG_WindowList* )malloc( sizeof(SFG_WindowList ) );
    new_list_entry->window = window;
    new_list_entry->needToClose = needToClose;
    new_list_entry->next = WindowsToDestroy;
    WindowsToDestroy = new_list_entry;

    /*
     * Check if the window is the current one...
     */
    if( fgStructure.Window == window )
        fgStructure.Window = NULL;

    /*
     * Clear all window callbacks except Destroy, which will
     * be invoked later.  Right now, we are potentially carrying
     * out a freeglut operation at the behest of a client callback,
     * so we are reluctant to re-enter the client with the Destroy
     * callback, right now.  The others are all wiped out, however,
     * to ensure that they are no longer called after this point.
     */
    {
        void *destroy = FETCH_WCB( *window, Destroy );
        fgClearCallBacks( window );
        FETCH_WCB( *window, Destroy ) = destroy;
    }
    

    /*
     * If the destroyed window has the highest window ID number, decrement
     * the window ID number.
     *
     * XXX Do we REALLY want to *ever* recycle window IDs?  Integers are
     * XXX plentiful, and clients may rely upon the implied promise in
     * XXX the GLUT docs to not recycle these.  (I can't remember if it
     * XXX is explicit.)
     *
     * XXX If we *do* want to do this, we should actually recompute the
     * XXX highest window-ID; the new highest may not in fact be one less
     * XXX than what we have just deleted.
     */
    if ( window->ID == fgStructure.WindowID )
        fgStructure.WindowID--;

    /*
     * Check the execution state.  If this has been called from
     * "glutDestroyWindow", a statement in that function will reset the
     * "ExecState" after this function returns.
     */
    if( fgState.ActionOnWindowClose != GLUT_ACTION_CONTINUE_EXECUTION )
        /*
         * Set the execution state flag to drop out of the main loop.
         */
        if( fgState.ActionOnWindowClose == GLUT_ACTION_EXIT )
            fgState.ExecState = GLUT_EXEC_STATE_STOP;
}

/*
 * Function to close down all the windows in the "WindowsToDestroy" list
 */
void fgCloseWindows( )
{
    SFG_WindowList *window_ptr = WindowsToDestroy;
    WindowsToDestroy = ( SFG_WindowList* )NULL;
    /* In case the destroy callbacks cause more windows to be closed */

    while( window_ptr )
    {
        SFG_WindowList *next = window_ptr->next;
        fgDestroyWindow( window_ptr->window, window_ptr->needToClose );
        free( window_ptr );
        window_ptr = next;

        if( !window_ptr )
        {
            window_ptr = WindowsToDestroy;
            WindowsToDestroy = ( SFG_WindowList* )NULL;
        }
    }
}

/*
 * This function destroys a window and all of its subwindows. Actually,
 * another function, defined in freeglut_window.c is called, but this is
 * a whole different story...
 */
void fgDestroyWindow( SFG_Window* window, GLboolean needToClose )
{
    SFG_Window* subWindow;
    int menu_index ;

    assert( window );
    freeglut_assert_ready;

    while( subWindow = ( SFG_Window * )window->Children.First )
        fgDestroyWindow( subWindow, needToClose );

    /*
     * XXX Since INVOKE_WCB() tests the function pointer, why not make
     * XXX this unconditional?  Overhead is close to nil, and it would
     * XXX clarify the code by omitting a conditional test.
     */
    if( FETCH_WCB( *window, Destroy ) )
    {
        SFG_Window *activeWindow = fgStructure.Window ;
        INVOKE_WCB( *window, Destroy, ( ) );
        fgSetWindow ( activeWindow );
    }

    if( window->Parent )
        fgListRemove( &window->Parent->Children, &window->Node );
    else
        fgListRemove( &fgStructure.Windows, &window->Node );

    if( window->ActiveMenu )
      fgDeactivateMenu( window );

    for ( menu_index = 0; menu_index < 3; menu_index ++ )
    {
      if ( window->Menu[menu_index] )
        window->Menu[menu_index]->ParentWindow = NULL ;
    }

    fgClearCallBacks( window );
    if( needToClose )
        fgCloseWindow( window );
    free( window );
    if( fgStructure.Window == window )
        fgStructure.Window = NULL;
}

/*
 * This is a helper static function that removes a menu (given its pointer)
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
    for( i = 0; i < 3; i++ )
        if( window->Menu[ i ] == menu )
            window->Menu[ i ] = NULL;

    /*
     * Call this function for all of the window's children recursively:
     */
    for( subWindow = (SFG_Window *)window->Children.First;
         subWindow;
         subWindow = (SFG_Window *)subWindow->Node.Next)
        fghRemoveMenuFromWindow( subWindow, menu );
}

/*
 * This is a static helper function that removes menu references
 * from another menu, given two pointers to them...
 */
static void fghRemoveMenuFromMenu( SFG_Menu* from, SFG_Menu* menu )
{
    SFG_MenuEntry *entry;

    for( entry = (SFG_MenuEntry *)from->Entries.First;
         entry;
         entry = ( SFG_MenuEntry * )entry->Node.Next )
        if( entry->SubMenu == menu )
            entry->SubMenu = NULL;
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

    assert( menu );
    freeglut_assert_ready;

    /*
     * First of all, have all references to this menu removed from all windows:
     */
    for( window = (SFG_Window *)fgStructure.Windows.First;
         window;
         window = (SFG_Window *)window->Node.Next )
        fghRemoveMenuFromWindow( window, menu );

    /*
     * Now proceed with removing menu entries that lead to this menu
     */
    for( from = ( SFG_Menu * )fgStructure.Menus.First;
         from;
         from = ( SFG_Menu * )from->Node.Next )
        fghRemoveMenuFromMenu( from, menu );

    /*
     * If the programmer defined a destroy callback, call it
     * A. Donev: But first make this the active menu
     */
    if( menu->Destroy )
    {
        SFG_Menu *activeMenu=fgStructure.Menu;
        fgStructure.Menu = menu;
        menu->Destroy( );
        fgStructure.Menu = activeMenu;
    }

    /*
     * Now we are pretty sure the menu is not used anywhere
     * and that we can remove all of its entries
     */
    while( entry = ( SFG_MenuEntry * )menu->Entries.First )
    {
        fgListRemove( &menu->Entries, &entry->Node );

        if( entry->Text )
            free( entry->Text );
        entry->Text = NULL;

        free( entry );
        entry = NULL;
    }

    if( fgStructure.Window == menu->Window )
        fgSetWindow( menu->ParentWindow );
    fgDestroyWindow( menu->Window, GL_TRUE );
    fgListRemove( &fgStructure.Menus, &menu->Node );
    if( fgStructure.Menu == menu )
        fgStructure.Menu = NULL;

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
 * This function is automatically called on glutMainLoop() return.
 * It should deallocate and destroy all remnants of previous
 * glutInit()-enforced structure initialization...
 */
void fgDestroyStructure( void )
{
    SFG_Window *window;
    SFG_Menu *menu;

    freeglut_assert_ready;

    /*
     * Make sure all windows and menus have been deallocated
     */
    while( menu = ( SFG_Menu * )fgStructure.Menus.First )
        fgDestroyMenu( menu );

    while( window = ( SFG_Window * )fgStructure.Windows.First )
        fgDestroyWindow( window, GL_TRUE );
}

/*
 * Helper function to enumerate through all registered top-level windows
 */
void fgEnumWindows( FGCBenumerator enumCallback, SFG_Enumerator* enumerator )
{
    SFG_Window *window;

    assert( enumCallback && enumerator );
    freeglut_assert_ready;

    /*
     * Check every of the top-level windows
     */
    for( window = ( SFG_Window * )fgStructure.Windows.First;
         window;
         window = ( SFG_Window * )window->Node.Next )
    {
        enumCallback( window, enumerator );
        if( enumerator->found )
            return;
    }
}

/*
 * Helper function to enumerate through all a window's subwindows
 * (single level descent)
 */
void fgEnumSubWindows( SFG_Window* window, FGCBenumerator enumCallback,
                       SFG_Enumerator* enumerator )
{
    SFG_Window *child;

    assert( enumCallback && enumerator );
    freeglut_assert_ready;

    for( child = ( SFG_Window * )window->Children.First;
         child;
         child = ( SFG_Window * )child->Node.Next )
    {
        enumCallback( child, enumerator );
        if( enumerator->found )
            return;
    }
}

/*
 * A static helper function to look for a window given its handle
 */
static void fghcbWindowByHandle( SFG_Window *window,
                                 SFG_Enumerator *enumerator )
{
    if ( enumerator->found )
        return;

    /*
     * Check the window's handle. Hope this works. Looks ugly. That's for sure.
     */
    if( window->Window.Handle == (SFG_WindowHandleType) (enumerator->data) )
    {
        enumerator->found = GL_TRUE;
        enumerator->data = window;

        return;
    }

    /*
     * Otherwise, check this window's children
     */
    fgEnumSubWindows( window, fghcbWindowByHandle, enumerator );
}

/*
 * fgWindowByHandle returns a (SFG_Window *) value pointing to the
 * first window in the queue matching the specified window handle.
 * The function is defined in freeglut_structure.c file.
 */
SFG_Window* fgWindowByHandle ( SFG_WindowHandleType hWindow )
{
    SFG_Enumerator enumerator;

    /*
     * This is easy and makes use of the windows enumeration defined above
     */
    enumerator.found = GL_FALSE;
    enumerator.data = (void *)hWindow;
    fgEnumWindows( fghcbWindowByHandle, &enumerator );

    if( enumerator.found )
        return( SFG_Window *) enumerator.data;
    return NULL;
}

/*
 * A static helper function to look for a window given its ID
 */
static void fghcbWindowByID( SFG_Window *window, SFG_Enumerator *enumerator )
{
    /*
     * Make sure we do not overwrite our precious results...
     */
    if ( enumerator->found )
        return;

    /*
     * Check the window's handle. Hope this works. Looks ugly. That's for sure.
     */
    if( window->ID == (int) (enumerator->data) ) /* XXX int/ptr conversion! */
    {
        enumerator->found = GL_TRUE;
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
    enumerator.found = GL_FALSE;
    enumerator.data = (void *) windowID; /* XXX int/pointer conversion! */
    fgEnumWindows( fghcbWindowByID, &enumerator );
    if( enumerator.found )
        return( SFG_Window *) enumerator.data;
    return NULL;
}

/*
 * Looks up a menu given its ID. This is easier that fgWindowByXXX
 * as all menus are placed in a single doubly linked list...
 */
SFG_Menu* fgMenuByID( int menuID )
{
    SFG_Menu *menu = NULL;

    freeglut_assert_ready;

    /*
     * It's enough to check all entries in fgStructure.Menus...
     */
    for( menu = (SFG_Menu *)fgStructure.Menus.First;
         menu;
         menu = (SFG_Menu *)menu->Node.Next )
        if( menu->ID == menuID )
            return menu;
    return NULL;
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

    if ( ln = (SFG_Node *)list->Last )
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

    if( ln = (SFG_Node *)node->Next )
        ln->Prev = node->Prev;
    if( ln = (SFG_Node *)node->Prev )
        ln->Next = node->Next;
    if( (ln = (SFG_Node *)list->First) == node )
        list->First = node->Next;
    if( (ln = (SFG_Node *)list->Last) == node )
        list->Last = node->Prev;
}

int fgListLength(SFG_List *list)
{
    SFG_Node *node;
    int length = 0;

    for( node =( SFG_Node * )list->First;
         node;
         node = ( SFG_Node * )node->Next )
        ++length;

    return length;
}

/*** END OF FILE ***/
