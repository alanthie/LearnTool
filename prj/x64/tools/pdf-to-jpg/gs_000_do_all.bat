call gs_renameNoSpace.bat

FOR /F %%i IN ('dir /b *.pdf') DO echo absolute path: %%~fi
pause

FOR /F %%j IN ('dir /b *.pdf') DO call gs_all.bat %%~fj
pause

exit