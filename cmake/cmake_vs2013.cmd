cls
set CRTDIR=%CD%
REM go up to the repo root
cd ..
set BUILD_PATH=%CD%/build_vs2013
set ROOT_PATH=%CD%

if "%VS120COMNTOOLS%"=="" goto VSNotInstalledError
call "%VS120COMNTOOLS%\vsvars32.bat"

%CRTDIR:~0,2%
cd "%CRTDIR%"
mkdir "%BUILD_PATH%"
cd "%BUILD_PATH%"

cmake -G "Visual Studio 12" "../"
goto end

:VSNotInstalledError
echo ERROR: Visual Studio 12 (2013) is not installed properly!
goto end

:end
%CRTDIR:~0,2%
cd %CRTDIR%

REM make the bin folder and copy libs
md ..\bin32
xcopy /H /R /Y ..\src\3rdparty\sdl2\lib\x86\*.dll ..\bin32
