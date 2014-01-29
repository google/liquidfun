@rem Copyright (c) 2013 Google, Inc.
@rem
@rem This software is provided 'as-is', without any express or implied
@rem warranty.  In no event will the authors be held liable for any damages
@rem arising from the use of this software.
@rem Permission is granted to anyone to use this software for any purpose,
@rem including commercial applications, and to alter it and redistribute it
@rem freely, subject to the following restrictions:
@rem 1. The origin of this software must not be misrepresented; you must not
@rem claim that you wrote the original software. If you use this software
@rem in a product, an acknowledgment in the product documentation would be
@rem appreciated but is not required.
@rem 2. Altered source versions must be plainly marked as such, and must not be
@rem misrepresented as being the original software.
@rem 3. This notice may not be removed or altered from any source distribution.
@echo off
setlocal enabledelayedexpansion
rem Build all Android projects, see help text below for a description of this
rem script's arguments.

rem Project name.
set project_name=LiquidFun
rem Save this batch file's directory.
set script_directory=%~d0%~p0
rem Get the absolute path of the project root directory.
set project_root=%script_directory%..\..
for %%a in (%project_root%) do set project_root=%%~fa
rem Default build configuration
set build_configuration_default=debug release
set build_configuration=
set ndk_debug=
rem Enable clean step by default.
set clean=1
rem Enable dry run mode by setting this to "echo"
set dryrun=
rem Output directory for build artifacts.
set output_dir=
rem Output archive file.
set output_archive=
rem Location of the zip executable.
set zip=
rem Space separated list of directories to delete after the script is complete.
set delete_dirs=
rem Default build ABIs
set build_abi=all
rem List of projects to build relative to the root dir
set projects=
rem The directory this script was executed from.
set execution_dir=%cd%
rem Get the absolute path of the AOSP root directory.
set android_root=%script_directory%\..\..\..\..\..\..
for %%a in (%android_root%) do set android_root=%%~fa

rem Display help text and exit the script.
goto display_help_end
:display_help
  echo Build all %project_name% libraries and applications.
  echo.
  echo Usage: %~nx0 [-n] [-d] [-h] [-a abi] [-b build_configurations]
  echo          [-p projects] [-o output_dir] [-z archive]
  echo          [build_configurations]
  echo.
  echo -n: Builds the solution without cleaning first.
  echo.
  echo -a abi: ABIs to build.  Defaults to "%build_abi%".
  echo.
  echo -d: Display the build commands this script would run without building.
  echo.
  echo -b build_configurations: Is space separated list of build configurations
  echo that should be built by this script.  If this isn't specified it
  echo defaults to "%build_config%"
  echo.
  echo -p projects: Space separated list of projects to build.
  echo.
  echo -o output_dir: Output directory to copy build artifacts to.
  echo.
  echo -z archive: Create a zip archive of build artifacts.  This will
  echo implicitly create a temporary output directory.
  echo.
  echo -h: Display this help message.
  echo.
  echo build_configurations: Legacy form of '-b build_configurations'.
  echo.
  exit /B 1
:display_help_end

rem Disable the clean step.
goto disable_clean_end
:disable_clean
  set clean=0
  set msbuild_args=%msbuild_args:Rebuild=Build%
  goto:eof
:disable_clean_end

rem Set and try to create the output directory for build artifacts.
goto set_create_output_dir_end
:set_create_output_dir
  if "%1"=="" goto:eof
  set create_output_dir=%1
  set current_dir=%cd%
  cd %execution_dir%
  mkdir %create_output_dir% 2>NUL
  cd %create_output_dir%
  if not %ERRORLEVEL% EQU 0 (
    echo Unable to create output directory %create_output_dir% >&2
    goto:eof
  )
  set output_dir=%cd%
  cd %current_dir%
  goto:eof
:set_create_output_dir_end

rem Set the output archive filename.
goto set_output_archive_end
:set_output_archive
  if "%1"=="" goto:eof
  for /F "delims=;" %%a in ('where zip') do set zip=%%a
  if "%zip%"=="" (
    echo Unable to find "zip" executable, ignoring output archive %1
    goto:eof
  )
  set current_dir=%cd%
  cd %execution_dir%
  for /F %%a in ("%1") do set output_archive=%%~fa
  cd %current_dir%
  goto:eof
:set_output_archive_end

rem Set the build configuration to %1 or fallback to the default set if %1 is
rem an empty string.
goto set_build_config_end
:set_build_config
  if not "%1"=="" set build_configuration=%1
  if "%build_configuration%"=="" (
    set build_configuration=%build_configuration_default%
  )
  goto:eof
:set_build_config_end

rem Attempt to a project directory to the list of projects.
goto add_project_end
:add_project
  set new_project=%1
  set display_warnings=%2
  rem Remove double quotes (") from the argument.  This means that paths
  rem can't contain spaces as they can't be quoted.
  set new_project=!new_project:"=!
  set new_project_jni=%new_project%\jni
  if not exist "%new_project_jni%" (
    if %display_warnings% equ 1 (
      echo "jni directory not found under %new_project%, skipping" >&2
    )
    goto:eof
  )
  for %%a in (%new_project_jni%) do set project_dir=%%~dpa
  set project_dir=!project_dir:~0,-1!
  if not exist "!project_dir!\AndroidManifest.xml" (
    if %display_warnings% equ 1 (
      echo "AndroidManifest.xml not found under %project_dir%, skipping" >&2
      goto:eof
    )
  )
  set projects=!projects! !project_dir!
  goto:eof
:add_project_end

rem Validate and set the list of projects to %*
goto set_projects_end
:set_projects
  set current_dir=%cd%
  set new_projects=%1
  set display_warnings=%2
  rem Remove double quotes (") from the argument.  This means that paths
  rem can't contain spaces as they can't be quoted.
  set new_projects=!new_projects:"=!
  set projects=
  for %%a in (%new_projects%) do (
    call :add_project %%a %display_warnings%
  )
  cd %current_dir%
  goto:eof
:set_projects_end

rem Find all Android projects in the tree.
goto find_projects_end
:find_projects
  set projects=
  cd %project_root%
  for /R . %%a in (.) do (
    set sub_dir=%%a
    set sub_dir=!sub_dir:~0,-2!
    rem "call" is extremely slow so check for a jni folder before trying to
    rem add the directory as a project.
    if exist "!sub_dir!\jni" (
      call :add_project !sub_dir! 0
    )
  )
  goto:eof
:find_projects_end

rem Parse switches.
:parse_args
  set current_arg=%1
  rem Determine whether this is a switch (starts with "-").
  set arg_first_character=%current_arg:~0,1%
  if not "%arg_first_character%"=="-" (
    rem Not a switch, continue to positional argument parsing.
    goto parse_args_end
  )
  shift
  rem Interpret switches.
  if "%current_arg%"=="-h" goto display_help
  if "%current_arg%"=="-n" call :disable_clean
  if "%current_arg%"=="-d" set dryrun=echo
  if "%current_arg%"=="-a" (
    set build_abi=%1
    shift
  )
  if "%current_arg%"=="-b" (
    call :set_build_config %1
    shift
  )
  if "%current_arg%"=="-s" (
    call :set_vs_ver %1
    shift
  )
  if "%current_arg%"=="-o" (
    call :set_create_output_dir %1
    shift
  )
  if "%current_arg%"=="-z" (
    call :set_output_archive %1
    shift
  )
  if "%current_arg%"=="-p" (
    call :set_projects %1 1
    shift
  )
  goto parse_args
:parse_args_end
rem Parse positional arguments.
call :set_build_config %1

rem Change into this batch file's directory.
cd %script_directory%

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

rem If a zip archive is being created and an output directory isn't specified
rem use a temporary directory and mark it for deletion.
if "%output_archive%"=="" goto prepare_archive_end
if not "%output_dir%"=="" goto prepare_archive_end
call :set_create_output_dir %temp%\%random%
set delete_dirs=%delete_dirs% %output_dir%
:prepare_archive_end

rem If no projects were specified, find all projects in the tree.
if "%projects%"=="" call :find_projects

rem Initialize the list of failed builds.
set failed_build=

rem Search the path for swig.
set swig_prebuilt=%android_root%\prebuilts\misc\windows\swig\swig.exe
set swig_bin=
set swig_lib=
rem Look for a prebuilt swig in the tree.
if exist %swig_prebuilt% (
  set swig_bin=%swig_prebuilt%
  goto found_swig_prebuilt
)

echo Searching PATH for swig. >&2
for /F "delims=;" %%a in ('where swig') do set swig_bin=%%a
if not "%swig_bin%"=="" (
  goto found_swig_in_path
)

echo Unable to find swig on this machine.>&2
exit /B -1

:found_swig_prebuilt
set swig_version=
set swig_temp_file=swig_version_%RANDOM%
%swig_bin% -version > %swig_temp_file%
for /F "tokens=3" %%a in ('findstr /C:"SWIG Version" %swig_temp_file%') do (
  set swig_version=%%a
)
del %swig_temp_file%
set swig_lib=%android_root%\prebuilts\misc\common\swig\include\%swig_version%
goto find_swig_end

:found_swig_in_path
for /F %%a in ('%swig_bin% -swiglib') do set swig_lib=%%a
goto find_swig_end
:find_swig_end

set ndk_common_args=APP_ABI=%build_abi%
set ndk_common_args=%ndk_common_args% SWIG_BIN=%swig_bin% SWIG_LIB=%swig_lib%
for %%a in (%build_configuration%) do (
  echo Building %%a configuration >&2
  rem Configure ndk-build arguments.
  if "%%a"=="debug" set ndk_debug=NDK_DEBUG=1
  if "%%a"=="release" set ndk_debug=
  set ndk_build_args=!ndk_debug! %ndk_common_args%

  rem Build all projects.
  for %%b in (%projects%) do (
    set project_name=%%b
    set project_name=!project_name:%project_root%\=!
    echo Building !project_name! >&2
    cd %%b
    if %clean% equ 1 %dryrun% call %ndk_build% clean !ndk_build_args!
    %dryrun% call %ndk_build% -j%NUMBER_OF_PROCESSORS% !ndk_build_args!
    if !ERRORLEVEL! neq 0 (
      set failed_build=!failed_build! !project_name!:%%a
    )

    rem Get the path of each project directory relative to the root dir.
    set project_dir=%%b
    set project_dir=!project_dir:%project_root%=.!

    rem NOTE: This is not complete since this script only builds NDK
    rem components without packaging them like build_android.sh.  When
    rem build_apk.sh is working on Windows it will perform the job of copying
    rem build artifacts to an output directory.  For the moment this just
    rem copies shared and static libraries produced by the build process to the
    rem output directory.
    set artifact_dir=%output_dir%\!project_dir!\%%a
    if not "%output_dir%"=="" (
       mkdir !artifact_dir!
       %dryrun% xcopy /Q /K /Y obj\local\* !artifact_dir!
       %dryrun% xcopy /Q /K /Y /S libs !artifact_dir!
    )
  )
)


rem Create a zip archive.
if "%output_archive%"=="" goto archive_end
set current_dir=%cd%
cd %output_dir%
if exist "%output_archive%" del /q "%output_archive%"
%zip% -r "%output_archive%" .
cd %current_dir%
:archive_end

rem Clean up any directories that need to be deleted.
for %%a in (%delete_dirs%) do (
  rmdir %%a /S /Q >NUL
)

rem Display the list of failed builds.
if not "%failed_build%"=="" (
  echo ERROR: One or more projects failed to build: >&2
  for %%a in (%failed_build%) do (
    echo   %%a >&2
  )
  exit /B 1
)
exit /B 0
