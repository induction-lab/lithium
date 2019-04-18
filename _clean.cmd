@echo off
echo --^> Clear project...
rd .\bin /S /Q 2> nul
rd .\gen /S /Q 2> nul
rd .\libs /S /Q 2> nul
rd .\obj /S /Q 2> nul
del .\*.keystore /F /Q 2> nul
del .\*.npp /F /Q 2> nul
echo Done.