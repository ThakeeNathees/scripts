
:: REPL C

@echo off
Pushd "%~dp0"

python ccli.py
if %errorlevel% neq 0 goto :l_error

gcc ccli_temp.c -o ccli_temp
if %errorlevel% neq 0 goto :l_error

ccli_temp
echo.
goto :l_end

:l_error
echo Error occured

:l_end

if exist ccli_temp.exe del ccli_temp.exe
if exist ccli_temp.c del ccli_temp.c

popd

