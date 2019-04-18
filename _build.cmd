:: Main build toolchain
@echo off
setlocal enableextensions

echo --^> Start build toolchain...
echo ROOT: %ROOT%
echo PROJECT: %PROJECT%

set RELEASE=all
::set RELEASE=debug
::set RELEASE=release

set APP=lithium
set ACTIVITY=MainActivity
set PACKAGE=com.induction.%APP%
set TARGET=android-26

set ANT_ROOT=%ROOT%\ANT
set SDK_ROOT=%ROOT%\SDK
set NDK_ROOT=%SDK_ROOT%\ndk-bundle

if "%RELEASE%" == "all" call _clean.cmd

if exist local.properties (
    for /f "tokens=1,2 delims==" %%a in (local.properties) do if %%a==sdk.dir set SDK_DIR=%%b
)
set SDK_DIR=%SDK_DIR:\\=\%
if exist "%SDK_DIR%" goto :compile

:prepare
if not exist AndroidManifest*.xml (
    echo --^> Create Project...
    call "%SDK_ROOT%\tools\android.bat" create project ^
        --target %TARGET% ^
        --name %APP% ^
        --path "%PROJECT%" ^
        --activity %ACTIVITY% ^
        --package %PACKAGE%
) else (
    echo --^> Update Project...
    if exist "android\bin" rd "android\bin" /Q
    call "%SDK_ROOT%\tools\android.bat" update project ^
        --name %APP% ^
        --target %TARGET% ^
        --path "%PROJECT%"
)

if exist jni (
    echo --^> Clean native parts...
    call "%NDK_ROOT%\ndk-build.cmd" clean
    if ERRORLEVEL 1 goto :build
)

:compile
echo --^> Compile native part...
if exist "obj\local\armeabi\objs\%APP%\Main.o" del /Q /F "obj\local\armeabi\objs\%APP%\Main.o"
if exist "jni\Android.mk" call "%NDK_ROOT%\ndk-build.cmd" -b
if ERRORLEVEL 1 goto :end

:keystore
if not exist ".\%APP%.keystore" (
    echo --^> Make keystore...
    call "%JAVA_HOME%\bin\keytool.exe" -genkey -v -noprompt -keystore ".\%APP%.keystore" -dname "CN=induction, OU=Mobile, O=induction.lab, L=Omsk, S=Siberia, C=Russia" -alias %APP% -keyalg RSA -keysize 2048 -validity 5475 -keypass system -storepass system
)

:build
echo --^> Build apk...

call "%ANT_ROOT%\bin\ant" %RELEASE% -quiet
if ERRORLEVEL 1 goto :end
if "%RELEASE%" == "all" goto :end

:install
echo --^> Install...

call "%SDK_ROOT%\platform-tools\adb.exe" -a shell "true"
if ERRORLEVEL 1 goto :end

for /f "tokens=2 delims==" %%a in ('call "%SDK_ROOT%\platform-tools\adb.exe" -a shell "dumpsys power | grep 'PowerManagerService.Display'"') do set POWER_ON=%%a
if "%POWER_ON%" == "0" (
    call "%SDK_ROOT%\platform-tools\adb.exe" -a shell "input keyevent 26"
    call "%SDK_ROOT%\platform-tools\adb.exe" -a shell "input touchscreen swipe 930 880 930 380"
)
call "%SDK_ROOT%\platform-tools\adb.exe" -a logcat -c
for /f "tokens=*" %%a in ('call "%SDK_ROOT%\platform-tools\adb.exe" -a shell "pm list packages | grep %PACKAGE%"') do set PACKAGE_EXIST=%%a
if not "%PACKAGE_EXIST%" == "" call "%SDK_ROOT%\platform-tools\adb.exe" uninstall %PACKAGE%
call "%SDK_ROOT%\platform-tools\adb.exe" install -r %PROJECT%\bin\%APP%-%RELEASE%.apk

:run
echo --^> Run...
call "%SDK_ROOT%\platform-tools\adb.exe" -a shell "am start -n %PACKAGE%/%PACKAGE%.%ACTIVITY%"

:log
echo --^> Log...
call "%SDK_ROOT%\platform-tools\adb.exe" -a logcat %APP%:V *:S

:end
exit /b 0

:: Main build toolchain
echo --^> Start build toolchain...

@echo off
setlocal enableextensions 

echo ROOT: %ROOT%
echo PROJECT: %PROJECT%
set RELEASE=all
::set RELEASE=release
::set RELEASE=debug

set APP=lithium
set ACTIVITY=MainActivity
set PACKAGE=com.induction.%APP%
set TARGET=android-26

set ANT_ROOT=%ROOT%\ANT
set SDK_ROOT=%ROOT%\SDK
set NDK_ROOT=%SDK_ROOT%\ndk-bundle

if exist local.properties (
    for /f "tokens=1,2 delims==" %%a in (local.properties) do if %%a==sdk.dir set SDK_DIR=%%b
)
set SDK_DIR=%SDK_DIR:\\=\%
if exist "%SDK_DIR%" goto :compile

:prepare
if not exist AndroidManifest*.xml (
    echo --^> Create Project...
    call "%SDK_ROOT%\tools\android.bat" create project ^
        --target %TARGET% ^
        --name %APP% ^
        --path "%PROJECT%" ^
        --activity %ACTIVITY% ^
        --package %PACKAGE%
) else (
    echo --^> Update Project...
    if exist "android\bin" rd "android\bin" /Q
    call "%SDK_ROOT%\tools\android.bat" update project ^
        --name %APP% ^
        --target %TARGET% ^
        --path "%PROJECT%"
)

goto :build

:clean
echo --^> Clean native parts...
call "%NDK_ROOT%\ndk-build.cmd" clean
if ERRORLEVEL 1 goto :build

:compile
echo --^> Compile native part...
if exist "obj\local\armeabi\objs\%APP%\Main.o" del /Q /F "obj\local\armeabi\objs\%APP%\Main.o"
if exist "jni\Android.mk" call "%NDK_ROOT%\ndk-build.cmd" -b
if ERRORLEVEL 1 goto :end

:keystore
if not exist ".\%APP%.keystore" (
    echo --^> Make keystore...
    call "%JAVA_HOME%\bin\keytool.exe" -genkey -v -noprompt -keystore ".\%APP%.keystore" -dname "CN=induction, OU=Mobile, O=induction.lab, L=Omsk, S=Siberia, C=Russia" -alias %APP% -keyalg RSA -keysize 2048 -validity 5475 -keypass system -storepass system
)

:build
echo --^> Build apk...

call "%ANT_ROOT%\bin\ant" %RELEASE% -S
if ERRORLEVEL 1 goto :end
if "%RELEASE%" == "all" goto :end

:install
echo --^> Install...

call "%SDK_ROOT%\platform-tools\adb.exe" -a shell "true"
if ERRORLEVEL 1 goto :end

for /f "tokens=2 delims==" %%a in ('call "%SDK_ROOT%\platform-tools\adb.exe" -a shell "dumpsys power | grep 'PowerManagerService.Display'"') do set POWER_ON=%%a
if "%POWER_ON%" == "0" (
    call "%SDK_ROOT%\platform-tools\adb.exe" -a shell "input keyevent 26"
    call "%SDK_ROOT%\platform-tools\adb.exe" -a shell "input touchscreen swipe 930 880 930 380"
)
call "%SDK_ROOT%\platform-tools\adb.exe" -a logcat -c
for /f "tokens=*" %%a in ('call "%SDK_ROOT%\platform-tools\adb.exe" -a shell "pm list packages | grep %PACKAGE%"') do set PACKAGE_EXIST=%%a
if not "%PACKAGE_EXIST%" == "" call "%SDK_ROOT%\platform-tools\adb.exe" uninstall %PACKAGE%
call "%SDK_ROOT%\platform-tools\adb.exe" install -r %PROJECT%\bin\%APP%-%RELEASE%.apk

:run
echo --^> Run...
call "%SDK_ROOT%\platform-tools\adb.exe" -a shell "am start -n %PACKAGE%/%PACKAGE%.%ACTIVITY%"

:log
echo --^> Log...
call "%SDK_ROOT%\platform-tools\adb.exe" -a logcat %APP%:V *:S

:end
exit /b 0

