@rem Copyright 2013 Google. All rights reserved.
@rem
@rem Use of this source code is governed by a BSD-style
@rem license that can be found in the LICENSE file or at
@rem https://developers.google.com/open-source/licenses/bsd
@echo off
rem Search .NET framework install directories for the latest version of msbuild
rem and run it with this batch file's arguments if it's found.

set msbuild_exe=MSBuild.exe
set msbuild=
rem Find the newest version of the Microsoft.NET framework installed on the
rem machine.
set framework_root=%SYSTEMROOT%\Microsoft.NET\Framework
for /F %%D in ('dir /AD /B /O-N %framework_root%') do (
  set msbuild=%framework_root%\%%D\%msbuild_exe%
  if exist "%framework_root%\%%D\MSBuild.exe" (
    goto found_msbuild
  )
)
:found_msbuild
set msbuild_found=1
if "%msbuild%"=="" (
  set msbuild_found=0
)
if not exist "%msbuild%" (
  set msbuild_found=0
)
if %msbuild_found%==0 (
  echo Unable to find %msbuild_exe%
  exit /B 1
)

%msbuild% %*
exit /B %ERRORLEVEL%
