@echo off
setlocal enabledelayedexpansion
set PATH=%PATH%;%ROOT%\GIT\bin

echo --^> Git Pull...

set count=1
for /f "tokens=* usebackq" %%f in (`type .credentials`) do (
    set line=%%f
    set credentials!count!=!line:@=%%40!
    set /a count=!count!+1
)

git config --global user.email "induction.lab@gmail.com"
git config --global user.name "induction.lab"
git config --global core.autocrlf false

git pull https://%credentials1%:%credentials2%@github.com/induction-lab/lithium.git --allow-unrelated-histories

:end
