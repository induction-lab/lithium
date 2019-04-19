@echo off
setlocal enabledelayedexpansion

if "%ROOT%" == "" set ROOT=%~p0..\..
pushd !ROOT!
set ROOT=!CD!
popd
if "%APP%" == "" (
    for %%i in ("%~p0") do set parent=%%~i
    for %%i in ("!parent:~0,-1!") do set APP=%%~nxi
)
echo ROOT: !ROOT!
echo APP: !APP!

set PATH=%PATH%;%ROOT%\GIT\bin
cd %~p0

git init
git add .
git commit -m "initial commit"
git remote add origin https://github.com/induction-lab/%APP%.git
git push -u origin master

pause
