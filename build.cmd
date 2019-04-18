@echo off
setlocal enableextensions 

echo ROOT: %ROOT%
echo PROJECT: %PROJECT%

set IP=192.168.1.50
set APP=lithium
set ACTIVITY=MainActivity
set PACKAGE=com.induction.%APP%

set ANT_ROOT=%ROOT%\ANT
set SDK_ROOT=%ROOT%\SDK
set NDK_ROOT=%ROOT%\NDK

call "%SDK_ROOT%\platform-tools\adb.exe" -a shell "am force-stop %PACKAGE%"
REM goto :log

if exist local.properties (
	for /f "tokens=1,2 delims==" %%a in (local.properties) do if %%a==sdk.dir set SDK_DIR=%%b
)
set SDK_DIR=%SDK_DIR:\\=\%
if exist "%SDK_DIR%" goto :compile

:prepare
if not exist AndroidManifest.xml (
    echo --^> Create Project...
    call "%SDK_ROOT%\tools\android.bat" create project -t 1 -n %APP% -p "%PROJECT%" -a %ACTIVITY% -k %PACKAGE%
) else (
    echo --^> Update Project...
    if exist "android\bin" rd "android\bin" /Q
    call "%SDK_ROOT%\tools\android.bat" update project -n %APP% -t 1 -p "%PROJECT%"
)

:clean
echo --^> Clean native parts...
call "%NDK_ROOT%\ndk-build.cmd" clean
if ERRORLEVEL 1 goto :build

:compile
echo --^> Compile native part...
if exist "obj\local\armeabi\objs\%APP%\Main.o" del /Q /F "obj\local\armeabi\objs\%APP%\Main.o"
if exist "jni\Android.mk" call "%NDK_ROOT%\ndk-build.cmd" -b
if ERRORLEVEL 1 goto :end

if not exist ".\%APP%.keystore" (
    echo --^> Make keystore...
    call "%JAVA_HOME%\bin\keytool.exe" -genkey -v -noprompt -keystore ".\%APP%.keystore" -dname "CN=, OU=, O=, L=, S=, C=" -alias %APP% -keyalg RSA -keysize 2048 -validity 10000 -keypass system -storepass system
)

:build
echo --^> Build apk...

call "%ANT_ROOT%\bin\ant" debug
REM call "%ANT_ROOT%\bin\ant" debug -S
if ERRORLEVEL 1 goto :end

:connect
echo --^> Connect to device...
REM call "%SDK_ROOT%\platform-tools\adb.exe" kill-server
ping -n 1 %IP% | find "TTL"
if not errorlevel 1 call "%SDK_ROOT%\platform-tools\adb.exe" connect %IP%:5555 > NUL

call "%SDK_ROOT%\platform-tools\adb.exe" -a shell "true"
if ERRORLEVEL 1 goto :end

:install
echo --^> Install...
for /f "tokens=2 delims==" %%a in ('call "%SDK_ROOT%\platform-tools\adb.exe" -a shell "dumpsys power | grep 'PowerManagerService.Display'"') do set POWER_ON=%%a
if "%POWER_ON%" == "0" (
    call "%SDK_ROOT%\platform-tools\adb.exe" -a shell "input keyevent 26"
    call "%SDK_ROOT%\platform-tools\adb.exe" -a shell "input touchscreen swipe 930 880 930 380"
)
call "%SDK_ROOT%\platform-tools\adb.exe" -a logcat -c
for /f "tokens=*" %%a in ('call "%SDK_ROOT%\platform-tools\adb.exe" -a shell "pm list packages | grep %PACKAGE%"') do set PACKAGE_EXIST=%%a
if not "%PACKAGE_EXIST%" == "" call "%SDK_ROOT%\platform-tools\adb.exe" uninstall %PACKAGE%
call "%SDK_ROOT%\platform-tools\adb.exe" install -r %PROJECT%\bin\%APP%-debug.apk

:run
echo --^> Run...
call "%SDK_ROOT%\platform-tools\adb.exe" -a shell "am start -n %PACKAGE%/%PACKAGE%.%ACTIVITY%"

:log
echo --^> Log...
call "%SDK_ROOT%\platform-tools\adb.exe" -a logcat %APP%:V *:S
REM call "%SDK_ROOT%\platform-tools\adb.exe" -a shell "am force-stop %PACKAGE%"
REM call "%SDK_ROOT%\platform-tools\adb.exe" -a logcat

:end
exit /b 0
