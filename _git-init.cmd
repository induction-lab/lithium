@echo off
setlocal enabledelayedexpansion

if "%ROOT%" == "" set ROOT=..\..
set PATH=%PATH%;%ROOT%\GIT\bin

git init
git add .
git commit -m "initial commit"
git remote add origin https://github.com/induction-lab/lithium.git
git push -u origin master

pause
