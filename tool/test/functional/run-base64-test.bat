@echo off

set PREV_DIR=%cd%
set DIR=%~dp0
cd "%DIR%"

set VFC=%1
set VFC=%VFC:/=\%

"%VFC%" -o "%OUTPUT_DIR%" -i "%DIR%\input-base64.json" > "%DIR%\run-output-base64.json"
if %ERRORLEVEL% neq 0 (
	cd %PREV_DIR%
	exit /B %ERRORLEVEL%
)

fc /L "%DIR%\output-base64.json" "%DIR%\run-output-base64.json"
if %ERRORLEVEL% neq 0 (
	cd %PREV_DIR%
	exit /B %ERRORLEVEL%
)

cd %PREV_DIR%
exit /B %ERRORLEVEL%
