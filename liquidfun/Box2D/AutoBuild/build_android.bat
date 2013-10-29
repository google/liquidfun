@echo off
setlocal enabledelayedexpansion
rem Build all Android projects, see help text below for a description of this
rem script's arguments.

rem Project name.
set project_name=Splash2D
rem Save this batch file's directory.
set script_directory=%~d0%~p0
rem Get the absolute path of the project root directory.
set project_root=%script_directory%..\..
for %%a in (%project_root%) do set project_root=%%~fa
rem Default build configuration
set build_config=release
set ndk_debug=

set android_root=%script_directory%\..\..\..\..\..\..
for %%a in (%android_root%) do set android_root=%%~fa

rem Parse script arguments.
if "%1"=="-h" (
  echo Build all %project_name% libraries and applications.
  echo.
  echo Usage: %~nx0 [build_configuration]
  echo.
  echo build_configuration: Type of build to perform.  This can be either
  echo "debug" or "release".  If this isn't specified the build configuration
  echo defaults to "%build_config%".
  exit /B 1
)
if "%1"=="debug" set build_config=%1
if "%1"=="release" set build_config=%1

rem Configure ndk-build arguments.
if "%build_config%"=="debug" set ndk_debug=NDK_DEBUG=1
set ndk_build_args=%ndk_debug% APP_ABI=all

rem Find ndk-build.cmd.
for /F "delims=;" %%a in ('where ndk-build.cmd') do set ndk_build_path=%%a
for %%a in (%ndk_build_path% ^
            %NDK_HOME%\ndk-build.cmd ^
            %android_root%\prebuilts\ndk\current\ndk-build.cmd) do (
  if exist %%a (
    set ndk_build=%%a
    goto ndk_build_set
  )
)
echo Unable to find ndk-build.cmd. >&2
exit /B 1
:ndk_build_set

rem Find all projects in the tree.
set projects=
for /R . %%a in (jni) do (
  if exist "%%a" (
    for %%b in (%%~dpa) do (
      set project_dir=%%b
      set project_dir=!project_dir:~0,-1!
      if exist !project_dir!\AndroidManifest.xml (
        for %%c in (!project_dir!) do (
          if not "%%~nc"=="AndroidTemplate" (
            set projects=!projects! %%c
          )
        )
      )
    )
  )
)

rem Build all projects.
set failed_build=
for %%a in (%projects%) do (
  set project_name=%%a
  set project_name=!project_name:%project_root%\=!
  echo Building !project_name! >&2
  cd %%a
  call %ndk_build% clean %ndk_build_args%
  call %ndk_build% -j%NUMBER_OF_PROCESSORS% %ndk_build_args%
  if !ERRORLEVEL! NEQ 0 (
    set failed_build=!failed_build! !project_name!
  )
)

rem Display the list of failed builds.
if not "%failed_build%"=="" (
  echo ERROR: One or more projects failed to build: >&2
  for %%a in (%failed_build%) do (
    echo   %%a >&2
  )
)
