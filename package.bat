@echo off
SETLOCAL DISABLEDELAYEDEXPANSION

set ERROR_LABEL=[91m[ERROR][0m
set SUCC_LABEL=[92m[SUCCESS][0m

set SRC_PATH=%~dp0

set EXE_NAME_VAR_NAME=VRFBCALC_EXECUTABLE_NAME
set EXE_OUTP_VAR_NAME=EXECUTABLE_OUTPUT_PATH

set CMAKE_CACHE_PATH=%SRC_PATH%build\CMakeCache.txt
set ZIPPER=C:\Program Files\7-Zip\7z.exe

if not exist "%ZIPPER%" (
    echo %ERROR_LABEL% Cannot find 7z executable - %ZIPPER%
    exit 1
)
if not exist "%CMAKE_CACHE_PATH%" (
    echo %ERROR_LABEL% Cannot find CMakeCache.txt file - %CMAKE_CACHE_PATH%
    exit 1
)

set EXE_NAME_FOUND=false
set EXE_OUTP_FOUNT=false
for /f "usebackq tokens=1 eol=/ delims=" %%G in ("%CMAKE_CACHE_PATH%") do (
    for /f "usebackq tokens=1,2,* eol=# delims==:" %%H in ('%%~G') do (
        if %%H==%EXE_NAME_VAR_NAME% (
            set EXE_NAME_FOUND=true
            set EXE_NAME=%%~J
        )
        if %%H==%EXE_OUTP_VAR_NAME% (
            set EXE_OUTP_FOUND=true
            set OUT_PATH=%%~J
        )
    )
)

if %EXE_NAME_FOUND%==false (
    echo %ERROR_LABEL% %EXE_NAME_VAR_NAME% variable not found in CMakeCache.txt
    exit 1
)
if %EXE_OUTP_FOUND%==false (
    echo %ERROR_LABEL% %EXE_OUTP_VAR_NAME% variable not found in CMakeCache.txt
    exit 1
)

set PAK_DIR=%OUT_PATH%\package
set BIN_PATH=%OUT_PATH%\Release
set EXE_PATH=%OUT_PATH%\Release\%EXE_NAME%.exe
set PAK_PATH=%PAK_DIR%\%EXE_NAME%.zip

if not exist "%BIN_PATH%" (
    echo %ERROR_LABEL% Cannot find binary output folder - %BIN_PATH%
    exit 1
)
if not exist "%EXE_PATH%" (
    echo %ERROR_LABEL% Cannot find executable file - %EXE_PATH%
    exit 1
)
if exist "%PAK_PATH%" (
    del "%PAK_PATH%"
    echo Deleted old package
) else (
    if not exist "%PAK_DIR%" mkdir "%PAK_DIR%"
)

echo Packaging executable...
"%ZIPPER%" a -tzip "%PAK_PATH%" "%EXE_PATH%" ^
    | findstr ^
        /c:"Add new data to archive:"

echo Packaging dependencies...
"%ZIPPER%" a -tzip "%PAK_PATH%" "%BIN_PATH%\*.dll" ^
    | findstr ^
        /c:"Add new data to archive:"
"%ZIPPER%" a -tzip "%PAK_PATH%" "%BIN_PATH%\**\*.dll" ^
    | findstr ^
        /c:"Add new data to archive:"

echo Packaging readme and license...
"%ZIPPER%" a -tzip "%PAK_PATH%" "%SRC_PATH%\COPYING" "%SRC_PATH%\COPYING.LESSER" "%SRC_PATH%\README.md" ^
    | findstr ^
        /c:"Add new data to archive:" ^
        /c:"Archive size"

echo %SUCC_LABEL% Packaging completed - %PAK_PATH%
