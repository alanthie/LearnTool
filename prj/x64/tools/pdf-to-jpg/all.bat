
set f1=%1

SETLOCAL
set file=%f1%
FOR %%i IN ("%file%") DO (
SET filedrive=%%~di
SET filepath=%%~pi
SET filename=%%~ni
SET fileextension=%%~xi
)

set d=%filedrive%%filepath%%filename%
ECHO %d%
mkdir %d%

copy %1 %d%\*
copy gsdll64.dll %d%\*
copy gsdll64.lib %d%\*
copy gswin64c.exe %d%\*
copy pdf-to-jpg.bat %d%\*
copy pdf-to-jpg-64.bat %d%\*

cd %d%

set ff=%filedrive%%filepath%%filename%\%filename%.pdf
echo %ff%

call pdf-to-jpg.bat %ff% ".\gswin64c.exe"
echo done
del gsdll64.dll
del gsdll64.lib
del gswin64c.exe
del pdf-to-jpg.bat
del pdf-to-jpg-64.bat

exit