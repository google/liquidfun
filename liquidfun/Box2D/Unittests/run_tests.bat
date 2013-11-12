@rem Copyright 2013 Google. All rights reserved.
@rem
@rem Use of this source code is governed by a BSD-style
@rem license that can be found in the LICENSE file or at
@rem https://developers.google.com/open-source/licenses/bsd
@echo off
rem Execute Splash2D unit tests on Windows.
rem
rem This assumes Splash2D test executables have been built for Windows and are
rem located in the following folders:
rem * splash2d\Box2D\Unittests\Debug
rem * splash2d\Box2D\Unittests\Release
rem * splash2d\Box2D\Unittests\MinSizeRel
rem * splash2d\Box2D\Unittests\RelWithDebInfo
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
