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
