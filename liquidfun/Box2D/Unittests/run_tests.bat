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
rem Execute LiquidFun unit tests on Windows.
rem
rem This assumes LiquidFun test executables have been built for Windows and are
rem located in the following folders:
rem * Box2D\Unittests\Debug
rem * Box2D\Unittests\Release
rem * Box2D\Unittests\MinSizeRel
rem * Box2D\Unittests\RelWithDebInfo
rem
rem It's possible to build tests for one build configuration and execute
rem all tests under the output directory using this script.

set this_path=%~dp0
cd %this_path%
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
