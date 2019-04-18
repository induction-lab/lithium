@echo off
setlocal enabledelayedexpansion
set PATH=%PATH%;%ROOT%\GIT\bin

echo --^> Git Export...
echo.

set count=1
for /f "tokens=* usebackq" %%f in (`type .credentials`) do (
    set line=%%f
    set credentials!count!=!line:@=%%40!
    set /a count=!count!+1
)

git config --global user.email "induction.lab@gmail.com"
git config --global user.name "induction.lab"
git config --global core.autocrlf false

for /d %%a in (..\git\*) do (
    set filename=%%~nxa
    echo !filename!
    PowerShell.exe -Command "Get-ChildItem -Path '.' -Exclude ".git", '.gitignore', '_*.cmd', '.credentials' | foreach ($_) { Remove-Item $_.fullname -Force -Recurse -Confirm:$false }"
    xcopy "..\git\!filename!\*" ".\" /a /s /y /q
    git add .
    git status
    git commit -m "!filename!"
    git pull https://%credentials1%:%credentials2%@github.com/induction-lab/%APP%.git --allow-unrelated-histories
    git push --force https://%credentials1%:%credentials2%@github.com/induction-lab/%APP%.git
    echo.
)
