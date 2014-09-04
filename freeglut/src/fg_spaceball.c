/* Spaceball support for Linux.
 * Written by John Tsiombikas <nuclear@member.fsf.org>
 *
 * This code supports 3Dconnexion's 6-dof space-whatever devices.
 * It can communicate with either the proprietary 3Dconnexion daemon (3dxsrv)
 * free spacenavd (http://spacenav.sourceforge.net), through the "standard"
 * magellan X-based protocol.
 */

#include <GL/freeglut.h>
#include "fg_internal.h"

/* -- PRIVATE FUNCTIONS --------------------------------------------------- */

extern void fgPlatformInitializeSpaceball(void);
extern void fgPlatformSpaceballClose(void);
extern int fgPlatformHasSpaceball(void);
extern int fgPlatformSpaceballNumButtons(void);
extern void fgPlatformSpaceballSetWindow(SFG_Window *window);


int sball_initialized = 0;

void fgInitialiseSpaceball(void)
{
    if(sball_initialized != 0) {
        return;
    }

    fgPlatformInitializeSpaceball();

    sball_initialized = 1;
}

void fgSpaceballClose(void)
{
	fgPlatformSpaceballClose();}

int fgHasSpaceball(void)
{
    if(sball_initialized == 0) {
        fgInitialiseSpaceball();
        if(sball_initialized != 1) {
            fgWarning("fgInitialiseSpaceball failed\n");
            return 0;
        }
    }

    return fgPlatformHasSpaceball();
}

int fgSpaceballNumButtons(void)
{
    if(sball_initialized == 0) {
        fgInitialiseSpaceball();
        if(sball_initialized != 1) {
            fgWarning("fgInitialiseSpaceball failed\n");
            return 0;
        }
    }

    return fgPlatformSpaceballNumButtons();
}

void fgSpaceballSetWindow(SFG_Window *window)
{
    if(sball_initialized == 0) {
        fgInitialiseSpaceball();
        if(sball_initialized != 1) {
            return;
        }
    }

    fgPlatformSpaceballSetWindow(window);
}

