# Microsoft Developer Studio Generated NMAKE File, Based on freeglut_static.dsp
!IF "$(CFG)" == ""
CFG=freeglut_static - Win32 Debug
!MESSAGE No configuration specified. Defaulting to freeglut_static - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "freeglut_static - Win32 Release" && "$(CFG)" != "freeglut_static - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "freeglut_static.mak" CFG="freeglut_static - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "freeglut_static - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "freeglut_static - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "freeglut_static - Win32 Release"

OUTDIR=.\ReleaseStatic
INTDIR=.\ReleaseStatic
# Begin Custom Macros
OutDir=.\ReleaseStatic
# End Custom Macros

ALL : "$(OUTDIR)\freeglut_static.lib"


CLEAN :
	-@erase "$(INTDIR)\freeglut_callbacks.obj"
	-@erase "$(INTDIR)\freeglut_cursor.obj"
	-@erase "$(INTDIR)\freeglut_display.obj"
	-@erase "$(INTDIR)\freeglut_ext.obj"
	-@erase "$(INTDIR)\freeglut_font.obj"
	-@erase "$(INTDIR)\freeglut_font_data.obj"
	-@erase "$(INTDIR)\freeglut_gamemode.obj"
	-@erase "$(INTDIR)\freeglut_geometry.obj"
	-@erase "$(INTDIR)\freeglut_glutfont_definitions.obj"
	-@erase "$(INTDIR)\freeglut_init.obj"
	-@erase "$(INTDIR)\freeglut_input_devices.obj"
	-@erase "$(INTDIR)\freeglut_joystick.obj"
	-@erase "$(INTDIR)\freeglut_main.obj"
	-@erase "$(INTDIR)\freeglut_menu.obj"
	-@erase "$(INTDIR)\freeglut_misc.obj"
	-@erase "$(INTDIR)\freeglut_overlay.obj"
	-@erase "$(INTDIR)\freeglut_state.obj"
	-@erase "$(INTDIR)\freeglut_stroke_mono_roman.obj"
	-@erase "$(INTDIR)\freeglut_stroke_roman.obj"
	-@erase "$(INTDIR)\freeglut_structure.obj"
	-@erase "$(INTDIR)\freeglut_teapot.obj"
	-@erase "$(INTDIR)\freeglut_videoresize.obj"
	-@erase "$(INTDIR)\freeglut_window.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\freeglut_static.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /ML /W3 /GX /O2 /I "include" /D "NDEBUG" /D "FREEGLUT_STATIC" /D "WIN32" /D "_MBCS" /D "_LIB" /Fp"$(INTDIR)\freeglut_static.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\freeglut_static.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\freeglut_static.lib" 
LIB32_OBJS= \
	"$(INTDIR)\freeglut_callbacks.obj" \
	"$(INTDIR)\freeglut_cursor.obj" \
	"$(INTDIR)\freeglut_display.obj" \
	"$(INTDIR)\freeglut_ext.obj" \
	"$(INTDIR)\freeglut_font.obj" \
	"$(INTDIR)\freeglut_font_data.obj" \
	"$(INTDIR)\freeglut_gamemode.obj" \
	"$(INTDIR)\freeglut_geometry.obj" \
	"$(INTDIR)\freeglut_glutfont_definitions.obj" \
	"$(INTDIR)\freeglut_init.obj" \
	"$(INTDIR)\freeglut_input_devices.obj" \
	"$(INTDIR)\freeglut_joystick.obj" \
	"$(INTDIR)\freeglut_main.obj" \
	"$(INTDIR)\freeglut_menu.obj" \
	"$(INTDIR)\freeglut_misc.obj" \
	"$(INTDIR)\freeglut_overlay.obj" \
	"$(INTDIR)\freeglut_state.obj" \
	"$(INTDIR)\freeglut_stroke_mono_roman.obj" \
	"$(INTDIR)\freeglut_stroke_roman.obj" \
	"$(INTDIR)\freeglut_structure.obj" \
	"$(INTDIR)\freeglut_teapot.obj" \
	"$(INTDIR)\freeglut_videoresize.obj" \
	"$(INTDIR)\freeglut_window.obj"

"$(OUTDIR)\freeglut_static.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "freeglut_static - Win32 Debug"

OUTDIR=.\DebugStatic
INTDIR=.\DebugStatic
# Begin Custom Macros
OutDir=.\DebugStatic
# End Custom Macros

ALL : "$(OUTDIR)\freeglut_static.lib" "$(OUTDIR)\freeglut_static.bsc"


CLEAN :
	-@erase "$(INTDIR)\freeglut_callbacks.obj"
	-@erase "$(INTDIR)\freeglut_callbacks.sbr"
	-@erase "$(INTDIR)\freeglut_cursor.obj"
	-@erase "$(INTDIR)\freeglut_cursor.sbr"
	-@erase "$(INTDIR)\freeglut_display.obj"
	-@erase "$(INTDIR)\freeglut_display.sbr"
	-@erase "$(INTDIR)\freeglut_ext.obj"
	-@erase "$(INTDIR)\freeglut_ext.sbr"
	-@erase "$(INTDIR)\freeglut_font.obj"
	-@erase "$(INTDIR)\freeglut_font.sbr"
	-@erase "$(INTDIR)\freeglut_font_data.obj"
	-@erase "$(INTDIR)\freeglut_font_data.sbr"
	-@erase "$(INTDIR)\freeglut_gamemode.obj"
	-@erase "$(INTDIR)\freeglut_gamemode.sbr"
	-@erase "$(INTDIR)\freeglut_geometry.obj"
	-@erase "$(INTDIR)\freeglut_geometry.sbr"
	-@erase "$(INTDIR)\freeglut_glutfont_definitions.obj"
	-@erase "$(INTDIR)\freeglut_glutfont_definitions.sbr"
	-@erase "$(INTDIR)\freeglut_init.obj"
	-@erase "$(INTDIR)\freeglut_init.sbr"
	-@erase "$(INTDIR)\freeglut_input_devices.obj"
	-@erase "$(INTDIR)\freeglut_input_devices.sbr"
	-@erase "$(INTDIR)\freeglut_joystick.obj"
	-@erase "$(INTDIR)\freeglut_joystick.sbr"
	-@erase "$(INTDIR)\freeglut_main.obj"
	-@erase "$(INTDIR)\freeglut_main.sbr"
	-@erase "$(INTDIR)\freeglut_menu.obj"
	-@erase "$(INTDIR)\freeglut_menu.sbr"
	-@erase "$(INTDIR)\freeglut_misc.obj"
	-@erase "$(INTDIR)\freeglut_misc.sbr"
	-@erase "$(INTDIR)\freeglut_overlay.obj"
	-@erase "$(INTDIR)\freeglut_overlay.sbr"
	-@erase "$(INTDIR)\freeglut_state.obj"
	-@erase "$(INTDIR)\freeglut_state.sbr"
	-@erase "$(INTDIR)\freeglut_stroke_mono_roman.obj"
	-@erase "$(INTDIR)\freeglut_stroke_mono_roman.sbr"
	-@erase "$(INTDIR)\freeglut_stroke_roman.obj"
	-@erase "$(INTDIR)\freeglut_stroke_roman.sbr"
	-@erase "$(INTDIR)\freeglut_structure.obj"
	-@erase "$(INTDIR)\freeglut_structure.sbr"
	-@erase "$(INTDIR)\freeglut_teapot.obj"
	-@erase "$(INTDIR)\freeglut_teapot.sbr"
	-@erase "$(INTDIR)\freeglut_videoresize.obj"
	-@erase "$(INTDIR)\freeglut_videoresize.sbr"
	-@erase "$(INTDIR)\freeglut_window.obj"
	-@erase "$(INTDIR)\freeglut_window.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\freeglut_static.bsc"
	-@erase "$(OUTDIR)\freeglut_static.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /ZI /Od /I "include" /D "_DEBUG" /D "FREEGLUT_STATIC" /D "WIN32" /D "_MBCS" /D "_LIB" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\freeglut_static.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\freeglut_static.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\freeglut_callbacks.sbr" \
	"$(INTDIR)\freeglut_cursor.sbr" \
	"$(INTDIR)\freeglut_display.sbr" \
	"$(INTDIR)\freeglut_ext.sbr" \
	"$(INTDIR)\freeglut_font.sbr" \
	"$(INTDIR)\freeglut_font_data.sbr" \
	"$(INTDIR)\freeglut_gamemode.sbr" \
	"$(INTDIR)\freeglut_geometry.sbr" \
	"$(INTDIR)\freeglut_glutfont_definitions.sbr" \
	"$(INTDIR)\freeglut_init.sbr" \
	"$(INTDIR)\freeglut_input_devices.sbr" \
	"$(INTDIR)\freeglut_joystick.sbr" \
	"$(INTDIR)\freeglut_main.sbr" \
	"$(INTDIR)\freeglut_menu.sbr" \
	"$(INTDIR)\freeglut_misc.sbr" \
	"$(INTDIR)\freeglut_overlay.sbr" \
	"$(INTDIR)\freeglut_state.sbr" \
	"$(INTDIR)\freeglut_stroke_mono_roman.sbr" \
	"$(INTDIR)\freeglut_stroke_roman.sbr" \
	"$(INTDIR)\freeglut_structure.sbr" \
	"$(INTDIR)\freeglut_teapot.sbr" \
	"$(INTDIR)\freeglut_videoresize.sbr" \
	"$(INTDIR)\freeglut_window.sbr"

"$(OUTDIR)\freeglut_static.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\freeglut_static.lib" 
LIB32_OBJS= \
	"$(INTDIR)\freeglut_callbacks.obj" \
	"$(INTDIR)\freeglut_cursor.obj" \
	"$(INTDIR)\freeglut_display.obj" \
	"$(INTDIR)\freeglut_ext.obj" \
	"$(INTDIR)\freeglut_font.obj" \
	"$(INTDIR)\freeglut_font_data.obj" \
	"$(INTDIR)\freeglut_gamemode.obj" \
	"$(INTDIR)\freeglut_geometry.obj" \
	"$(INTDIR)\freeglut_glutfont_definitions.obj" \
	"$(INTDIR)\freeglut_init.obj" \
	"$(INTDIR)\freeglut_input_devices.obj" \
	"$(INTDIR)\freeglut_joystick.obj" \
	"$(INTDIR)\freeglut_main.obj" \
	"$(INTDIR)\freeglut_menu.obj" \
	"$(INTDIR)\freeglut_misc.obj" \
	"$(INTDIR)\freeglut_overlay.obj" \
	"$(INTDIR)\freeglut_state.obj" \
	"$(INTDIR)\freeglut_stroke_mono_roman.obj" \
	"$(INTDIR)\freeglut_stroke_roman.obj" \
	"$(INTDIR)\freeglut_structure.obj" \
	"$(INTDIR)\freeglut_teapot.obj" \
	"$(INTDIR)\freeglut_videoresize.obj" \
	"$(INTDIR)\freeglut_window.obj"

"$(OUTDIR)\freeglut_static.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("freeglut_static.dep")
!INCLUDE "freeglut_static.dep"
!ELSE 
!MESSAGE Warning: cannot find "freeglut_static.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "freeglut_static - Win32 Release" || "$(CFG)" == "freeglut_static - Win32 Debug"
SOURCE=.\src\freeglut_callbacks.c

!IF  "$(CFG)" == "freeglut_static - Win32 Release"


"$(INTDIR)\freeglut_callbacks.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "freeglut_static - Win32 Debug"


"$(INTDIR)\freeglut_callbacks.obj"	"$(INTDIR)\freeglut_callbacks.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\freeglut_cursor.c

!IF  "$(CFG)" == "freeglut_static - Win32 Release"


"$(INTDIR)\freeglut_cursor.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "freeglut_static - Win32 Debug"


"$(INTDIR)\freeglut_cursor.obj"	"$(INTDIR)\freeglut_cursor.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\freeglut_display.c

!IF  "$(CFG)" == "freeglut_static - Win32 Release"


"$(INTDIR)\freeglut_display.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "freeglut_static - Win32 Debug"


"$(INTDIR)\freeglut_display.obj"	"$(INTDIR)\freeglut_display.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\freeglut_ext.c

!IF  "$(CFG)" == "freeglut_static - Win32 Release"


"$(INTDIR)\freeglut_ext.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "freeglut_static - Win32 Debug"


"$(INTDIR)\freeglut_ext.obj"	"$(INTDIR)\freeglut_ext.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\freeglut_font.c

!IF  "$(CFG)" == "freeglut_static - Win32 Release"


"$(INTDIR)\freeglut_font.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "freeglut_static - Win32 Debug"


"$(INTDIR)\freeglut_font.obj"	"$(INTDIR)\freeglut_font.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\freeglut_font_data.c

!IF  "$(CFG)" == "freeglut_static - Win32 Release"


"$(INTDIR)\freeglut_font_data.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "freeglut_static - Win32 Debug"


"$(INTDIR)\freeglut_font_data.obj"	"$(INTDIR)\freeglut_font_data.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\freeglut_gamemode.c

!IF  "$(CFG)" == "freeglut_static - Win32 Release"


"$(INTDIR)\freeglut_gamemode.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "freeglut_static - Win32 Debug"


"$(INTDIR)\freeglut_gamemode.obj"	"$(INTDIR)\freeglut_gamemode.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\freeglut_geometry.c

!IF  "$(CFG)" == "freeglut_static - Win32 Release"


"$(INTDIR)\freeglut_geometry.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "freeglut_static - Win32 Debug"


"$(INTDIR)\freeglut_geometry.obj"	"$(INTDIR)\freeglut_geometry.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\freeglut_glutfont_definitions.c

!IF  "$(CFG)" == "freeglut_static - Win32 Release"


"$(INTDIR)\freeglut_glutfont_definitions.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "freeglut_static - Win32 Debug"


"$(INTDIR)\freeglut_glutfont_definitions.obj"	"$(INTDIR)\freeglut_glutfont_definitions.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\freeglut_init.c

!IF  "$(CFG)" == "freeglut_static - Win32 Release"


"$(INTDIR)\freeglut_init.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "freeglut_static - Win32 Debug"


"$(INTDIR)\freeglut_init.obj"	"$(INTDIR)\freeglut_init.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\freeglut_input_devices.c

!IF  "$(CFG)" == "freeglut_static - Win32 Release"


"$(INTDIR)\freeglut_input_devices.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "freeglut_static - Win32 Debug"


"$(INTDIR)\freeglut_input_devices.obj"	"$(INTDIR)\freeglut_input_devices.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\freeglut_joystick.c

!IF  "$(CFG)" == "freeglut_static - Win32 Release"


"$(INTDIR)\freeglut_joystick.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "freeglut_static - Win32 Debug"


"$(INTDIR)\freeglut_joystick.obj"	"$(INTDIR)\freeglut_joystick.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\freeglut_main.c

!IF  "$(CFG)" == "freeglut_static - Win32 Release"


"$(INTDIR)\freeglut_main.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "freeglut_static - Win32 Debug"


"$(INTDIR)\freeglut_main.obj"	"$(INTDIR)\freeglut_main.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\freeglut_menu.c

!IF  "$(CFG)" == "freeglut_static - Win32 Release"


"$(INTDIR)\freeglut_menu.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "freeglut_static - Win32 Debug"


"$(INTDIR)\freeglut_menu.obj"	"$(INTDIR)\freeglut_menu.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\freeglut_misc.c

!IF  "$(CFG)" == "freeglut_static - Win32 Release"


"$(INTDIR)\freeglut_misc.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "freeglut_static - Win32 Debug"


"$(INTDIR)\freeglut_misc.obj"	"$(INTDIR)\freeglut_misc.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\freeglut_overlay.c

!IF  "$(CFG)" == "freeglut_static - Win32 Release"


"$(INTDIR)\freeglut_overlay.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "freeglut_static - Win32 Debug"


"$(INTDIR)\freeglut_overlay.obj"	"$(INTDIR)\freeglut_overlay.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\freeglut_state.c

!IF  "$(CFG)" == "freeglut_static - Win32 Release"


"$(INTDIR)\freeglut_state.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "freeglut_static - Win32 Debug"


"$(INTDIR)\freeglut_state.obj"	"$(INTDIR)\freeglut_state.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\freeglut_stroke_mono_roman.c

!IF  "$(CFG)" == "freeglut_static - Win32 Release"


"$(INTDIR)\freeglut_stroke_mono_roman.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "freeglut_static - Win32 Debug"


"$(INTDIR)\freeglut_stroke_mono_roman.obj"	"$(INTDIR)\freeglut_stroke_mono_roman.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\freeglut_stroke_roman.c

!IF  "$(CFG)" == "freeglut_static - Win32 Release"


"$(INTDIR)\freeglut_stroke_roman.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "freeglut_static - Win32 Debug"


"$(INTDIR)\freeglut_stroke_roman.obj"	"$(INTDIR)\freeglut_stroke_roman.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\freeglut_structure.c

!IF  "$(CFG)" == "freeglut_static - Win32 Release"


"$(INTDIR)\freeglut_structure.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "freeglut_static - Win32 Debug"


"$(INTDIR)\freeglut_structure.obj"	"$(INTDIR)\freeglut_structure.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\freeglut_teapot.c

!IF  "$(CFG)" == "freeglut_static - Win32 Release"


"$(INTDIR)\freeglut_teapot.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "freeglut_static - Win32 Debug"


"$(INTDIR)\freeglut_teapot.obj"	"$(INTDIR)\freeglut_teapot.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\freeglut_videoresize.c

!IF  "$(CFG)" == "freeglut_static - Win32 Release"


"$(INTDIR)\freeglut_videoresize.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "freeglut_static - Win32 Debug"


"$(INTDIR)\freeglut_videoresize.obj"	"$(INTDIR)\freeglut_videoresize.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\freeglut_window.c

!IF  "$(CFG)" == "freeglut_static - Win32 Release"


"$(INTDIR)\freeglut_window.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "freeglut_static - Win32 Debug"


"$(INTDIR)\freeglut_window.obj"	"$(INTDIR)\freeglut_window.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 


!ENDIF 

