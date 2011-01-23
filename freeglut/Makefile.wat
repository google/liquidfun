#
# Makefile.wat
# Open Watcom makefile for Freeglut
# Usage:
#    wmake -f Makefile.wat {target}
# Where target is one of:
#    debug-static
#    debug-dynamic
#    release-static
#    release-dynamic
#    debug
#    release
#    all
#    clean
# Assumes:
#    WATCOM enviroment variable points to your Watcom installation directory
#    %WATCOM%\BINNT and %WATCOM%\BINW are in your path
#
RELEASE_STATIC_DIR = release-static
RELEASE_DYNAMIC_DIR = release-dynamic
DEBUG_STATIC_DIR = debug-static
DEBUG_DYNAMIC_DIR = debug-dynamic

INCLUDE_DIR = ..\include
SRC_DIR = ..\src
COMPILER = wcc386
COMPILE_COMMON_OPTIONS = -i$(INCLUDE_DIR);$(%WATCOM)\h;$(%WATCOM)\h\nt -fp6 -w4 -e25 -wcd202 -bt=nt -mf -zq
COMPILE_DEBUG_OPTIONS = -d3 -od -DDEBUG -D_DEBUG
COMPILE_RELEASE_OPTIONS = -d0 -otexanh
COMPILE_STATIC_OPTIONS = -DFREEGLUT_STATIC -6r
COMPILE_DYNAMIC_OPTIONS = -DFREEGLUT_EXPORTS -6s -bd -bm
COMPILE_OPTIONS = $(COMPILE_COMMON_OPTIONS) $(COMPILE_$(MODE)_OPTIONS) $(COMPILE_$(TYPE)_OPTIONS)

LINKER = wlink
LINK_OPTIONS = SYS nt_dll op m op maxe=25 op q op symf FIL

LIB_TOOL = wlib
LIB_COMMON_OPTIONS = -q -n -b
LIB_DYNAMIC_OPTIONS =
LIB_STATIC_OPTIONS = -c -p=512
LIB_OPTIONS = $(LIB_COMMON_OPTIONS) $(LIB_$(TYPE)_OPTIONS)

MAKE = wmake -h -f ..\Makefile.wat 

OBJ_FILES = &
	freeglut_callbacks.obj &
	freeglut_cursor.obj &
	freeglut_display.obj &
	freeglut_ext.obj &
	freeglut_font.obj &
	freeglut_font_data.obj &
	freeglut_gamemode.obj &
	freeglut_geometry.obj &
	freeglut_glutfont_definitions.obj &
	freeglut_init.obj &
	freeglut_input_devices.obj &
	freeglut_joystick.obj &
	freeglut_main.obj &
	freeglut_menu.obj &
	freeglut_misc.obj &
	freeglut_overlay.obj &
	freeglut_spaceball.obj &
	freeglut_state.obj &
	freeglut_stroke_mono_roman.obj &
	freeglut_stroke_roman.obj &
	freeglut_structure.obj &
	freeglut_teapot.obj &
	freeglut_videoresize.obj &
	freeglut_window.obj

default : .SYMBOLIC
	@echo Usage:
	@echo 	wmake -f Makefile.wat {target}
	@echo Where target is one of:
	@echo   debug-static
	@echo   debug-dynamic
	@echo   release-static
	@echo   release-dynamic
	@echo   debug
	@echo   release
	@echo   all
	@echo   clean

# static library
freeglut_static.lib : $(OBJ_FILES)
	$(LIB_TOOL) $(LIB_OPTIONS) $^* $(OBJ_FILES)

# dynamic link library
freeglut.dll : $(OBJ_FILES)
	$(LINKER) name $^* $(LINK_OPTIONS) $(OBJ_FILES: =,)

freeglut.lib : freeglut.dll
	$(LIB_TOOL) $(LIB_OPTIONS) $^. +$[.


freeglut_callbacks.obj : $(SRC_DIR)\freeglut_callbacks.c
	$(COMPILER) $(SRC_DIR)\$[. $(COMPILE_OPTIONS)
	
freeglut_cursor.obj : $(SRC_DIR)\freeglut_cursor.c
	$(COMPILER) $(SRC_DIR)\$[. $(COMPILE_OPTIONS)

freeglut_display.obj : $(SRC_DIR)\freeglut_display.c
	$(COMPILER) $(SRC_DIR)\$[. $(COMPILE_OPTIONS)

freeglut_ext.obj : $(SRC_DIR)\freeglut_ext.c
	$(COMPILER) $(SRC_DIR)\$[. $(COMPILE_OPTIONS)

freeglut_font.obj : $(SRC_DIR)\freeglut_font.c
	$(COMPILER) $(SRC_DIR)\$[. $(COMPILE_OPTIONS)

freeglut_font_data.obj : $(SRC_DIR)\freeglut_font_data.c
	$(COMPILER) $(SRC_DIR)\$[. $(COMPILE_OPTIONS)

freeglut_gamemode.obj : $(SRC_DIR)\freeglut_gamemode.c
	$(COMPILER) $(SRC_DIR)\$[. $(COMPILE_OPTIONS)

freeglut_geometry.obj : $(SRC_DIR)\freeglut_geometry.c
	$(COMPILER) $(SRC_DIR)\$[. $(COMPILE_OPTIONS)

freeglut_glutfont_definitions.obj : $(SRC_DIR)\freeglut_glutfont_definitions.c
	$(COMPILER) $(SRC_DIR)\$[. $(COMPILE_OPTIONS)

freeglut_init.obj : $(SRC_DIR)\freeglut_init.c
	$(COMPILER) $(SRC_DIR)\$[. $(COMPILE_OPTIONS)

freeglut_input_devices.obj : $(SRC_DIR)\freeglut_input_devices.c
	$(COMPILER) $(SRC_DIR)\$[. $(COMPILE_OPTIONS)

freeglut_joystick.obj : $(SRC_DIR)\freeglut_joystick.c
	$(COMPILER) $(SRC_DIR)\$[. $(COMPILE_OPTIONS)

freeglut_main.obj : $(SRC_DIR)\freeglut_main.c
	$(COMPILER) $(SRC_DIR)\$[. $(COMPILE_OPTIONS)

freeglut_menu.obj : $(SRC_DIR)\freeglut_menu.c
	$(COMPILER) $(SRC_DIR)\$[. $(COMPILE_OPTIONS)

freeglut_misc.obj : $(SRC_DIR)\freeglut_misc.c
	$(COMPILER) $(SRC_DIR)\$[. $(COMPILE_OPTIONS)

freeglut_overlay.obj : $(SRC_DIR)\freeglut_overlay.c
	$(COMPILER) $(SRC_DIR)\$[. $(COMPILE_OPTIONS)

freeglut_spaceball.obj : $(SRC_DIR)\freeglut_spaceball.c
	$(COMPILER) $(SRC_DIR)\$[. $(COMPILE_OPTIONS)

freeglut_state.obj : $(SRC_DIR)\freeglut_state.c
	$(COMPILER) $(SRC_DIR)\$[. $(COMPILE_OPTIONS)

freeglut_stroke_mono_roman.obj : $(SRC_DIR)\freeglut_stroke_mono_roman.c
	$(COMPILER) $(SRC_DIR)\$[. $(COMPILE_OPTIONS)

freeglut_stroke_roman.obj : $(SRC_DIR)\freeglut_stroke_roman.c
	$(COMPILER) $(SRC_DIR)\$[. $(COMPILE_OPTIONS)

freeglut_structure.obj : $(SRC_DIR)\freeglut_structure.c
	$(COMPILER) $(SRC_DIR)\$[. $(COMPILE_OPTIONS)

freeglut_teapot.obj : $(SRC_DIR)\freeglut_teapot.c
	$(COMPILER) $(SRC_DIR)\$[. $(COMPILE_OPTIONS)

freeglut_videoresize.obj : $(SRC_DIR)\freeglut_videoresize.c
	$(COMPILER) $(SRC_DIR)\$[. $(COMPILE_OPTIONS)

freeglut_window.obj : $(SRC_DIR)\freeglut_window.c
	$(COMPILER) $(SRC_DIR)\$[. $(COMPILE_OPTIONS)

clean : .SYMBOLIC
	@-del $(DEBUG_STATIC_DIR)\*.obj
	@-del $(DEBUG_DYNAMIC_DIR)\*.obj
	@-del $(RELEASE_STATIC_DIR)\*.obj
	@-del $(RELEASE_DYNAMIC_DIR)\*.obj
	@echo All object files removed
	
debug-static : .SYMBOLIC
	@-if not exist $(DEBUG_STATIC_DIR)\. mkdir $(DEBUG_STATIC_DIR)
	@-cd $(DEBUG_STATIC_DIR)
	@$(MAKE) MODE=DEBUG TYPE=STATIC freeglut_static.lib
	@-cd ..
	@echo Debug static library up to date

debug-dynamic : .SYMBOLIC
	@-if not exist $(DEBUG_DYNAMIC_DIR)\. mkdir $(DEBUG_DYNAMIC_DIR)
	@-cd $(DEBUG_DYNAMIC_DIR)
	@$(MAKE) MODE=DEBUG TYPE=DYNAMIC freeglut.lib
	@-cd ..
	@echo Debug dynamic link library up to date

release-static : .SYMBOLIC
	@-if not exist $(RELEASE_STATIC_DIR)\. mkdir $(RELEASE_STATIC_DIR)
	@-cd $(RELEASE_STATIC_DIR)
	@$(MAKE) MODE=RELEASE TYPE=STATIC freeglut_static.lib
	@-cd ..
	@echo Release static library up to date

release-dynamic : .SYMBOLIC
	@-if not exist $(RELEASE_DYNAMIC_DIR)\. mkdir $(RELEASE_DYNAMIC_DIR)
	@-cd $(RELEASE_DYNAMIC_DIR)
	@$(MAKE) MODE=RELEASE TYPE=DYNAMIC freeglut.lib
	@-cd ..
	@echo Release dynamic link library up to date

debug : debug-static debug-dynamic .SYMBOLIC
	@echo Debug targets up to date

release : release-static release-dynamic .SYMBOLIC
	@echo Release targets up to date

all : debug release .SYMBOLIC
	@echo All targets up to date
