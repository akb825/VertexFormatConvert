@echo off

set PREV_DIR=%cd%
set DIR=%~dp0
cd "%DIR%"

set VFC=%1
set VFC=%VFC:/=\%
set OUTPUT_DIR=%DIR%output

"%VFC%" -o "%OUTPUT_DIR%" -i "%DIR%\input.json" > "%DIR%\run-output.json"
if %ERRORLEVEL% neq 0 (
	cd %PREV_DIR%
	exit /B %ERRORLEVEL%
)

powershell -Command "(gc '%DIR%output.json') -replace '\$OUTPUT_DIR', '%OUTPUT_DIR%'" -replace '[/\\]', '\\' > "%OUTPUT_DIR%\expected-output.json"

fc /L "%OUTPUT_DIR%\expected-output.json" "%DIR%\run-output.json"
if %ERRORLEVEL% neq 0 (
	cd %PREV_DIR%
	exit /B %ERRORLEVEL%
)

fc /B "%DIR%\output.vertices.dat" "%OUTPUT_DIR%\vertices.dat"
if %ERRORLEVEL% neq 0 (
	cd %PREV_DIR%
	exit /B %ERRORLEVEL%
)

fc /B "%DIR%\output.indices.dat" "%OUTPUT_DIR%\indices.0.dat"
if %ERRORLEVEL% neq 0 (
	cd %PREV_DIR%
	exit /B %ERRORLEVEL%
)

cd %PREV_DIR%
exit /B %ERRORLEVEL%
