@echo off
setlocal enabledelayedexpansion

echo --^> Run emulator...
echo.

if "%ROOT%" == "" (
    set ROOT=%~p0..\..
    pushd !ROOT! & set ROOT=!CD! & popd
    echo ROOT: !ROOT!
)

cd %ROOT%\SDK\emulator

:: get version
emulator.exe -version >nul
if /i "%errorlevel%"=="0" (
    emulator.exe -version
) else (
    echo Deprecated emulator.
    echo.
)

:: build "array" of avds
echo List of aviable avds:
set n=0
for /f "eol=: delims=" %%x in ('emulator.exe -list-avds') do (
  set /a n+=1
  set "avd!n!=%%x"
)

if [!avd1!]==[] (
    echo No avd found.
    goto :end
)

:: print menu
for /l %%x in (1 1 %n%) do echo %%x. !avd%%x!

:: get selection
echo.
choice /C 123456789 /D 1 /T 3 /N /M "Enter a avd number [default 1]:"
set selection=%errorlevel%
if [!avd%selection%!]==[] set selection=1
start emulator.exe -avd !avd%selection%!
exit /b 0

:: script end
:end
