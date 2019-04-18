@echo off
setlocal enabledelayedexpansion
set PATH=%PATH%;%ROOT%\GIT\bin

if "%1" == "" (
	copy %~f0 ..\ /y > nul 2>&1
	xcopy .credentials ..\ /h > nul 2>&1
	(goto) 2>nul & call ..\%~nx0 %~dp0
	exit /b 0
)
cd ..
del %1\* /a /s /q > nul 2>&1
for /f "delims=" %%d in ('dir %1 /s /b /ad ^| sort /r') do rd "%%d" /s /q

echo --^> Git Clone...

set count=1
for /f "tokens=* usebackq" %%f in (`type .credentials`) do (
  set line=%%f
  set credentials!count!=!line:@=%%40!
  set /a count=!count!+1
)

git config --global user.email "induction.lab@gmail.com"
git config --global user.name "induction.lab"
git config --global core.autocrlf false

git clone https://%credentials1%:%credentials2%@github.com/induction-lab/%APP%.git %1

copy %~f0 %1 /y > nul 2>&1
xcopy .credentials %1 /h > nul 2>&1
del /ah .credentials > nul 2>&1
echo Done.

(goto) 2>nul & del %~f0
