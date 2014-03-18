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

set this_path=%~dp0
cd %this_path%

rem Display help.
goto display_help_end
:display_help
  echo Execute LiquidFun unit tests on Windows.
  echo.
  echo Usage: %~n0 [-h] [-n]
  echo.
  echo -h: Display this help message.
  echo -n: Disable the unit test build step.
  echo.
  exit /B 1
:display_help_end

rem Whether to build the tests.
set build_tests=1

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
  if "%current_arg%"=="-h" goto display_help
  if "%current_arg%"=="-n" set build_tests=0
  goto parse_args
:parse_args_end

rem Build tests if they're not built already.
if "%build_tests%"=="1" (
  cmd /c %this_path%\..\AutoBuild\build.bat -n
  if not ERRORLEVEL 0 exit /B 1
)

set failed=
rem For all test directories.
for %%c in (Debug Release MinSizeRel RelWithDebInfo) do (
  rem Iterate through all test executables.
  for %%t in (%%c\*Tests.exe) do (
    rem Execute the test.
    echo %%t
    %%t
    rem If the test fails add it to the "failed" list.
    if not ERRORLEVEL 0 set failed=%failed% %%t
  )
)

rem Summarize the list of failing tests.
if not "%failed%"=="" (
  echo.
  echo ERROR: The following tests failed:
  for %%t in (%failed%) do (
    echo   %%t
  )
)
