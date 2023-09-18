@echo off
SETLOCAL DISABLEDELAYEDEXPANSION

set ERROR_LABEL=[91m[ERROR][0m
set WARN_LABEL=[93m[WARN][0m
set SUCC_LABEL=[92m[SUCCESS][0m

set SRC_PATH=%~dp0

rem ==== VCPKG MANIFEST CONFIGURATION ==========================================

set VCPKG_EXE_PATH=%~1
set VCPKG_EXE_FILE=%~nx1

set VCPKG_MANI_SRC=%SRC_PATH%dependency\dependency.json
set VCPKG_MANI_DES=%SRC_PATH%vcpkg.json

if exist "%VCPKG_MANI_DES%" (
    echo Deleting old vcpkg manifest file...
    del "%VCPKG_MANI_DES%"
)

if not exist "%VCPKG_MANI_SRC%" (
    echo %WARN_LABEL% Cannot find source vcpkg manifest, vcpkg manifest will not be configured
    goto VCPKG_CFG_END
)

if "%VCPKG_EXE_PATH%"=="" (
    where /q vcpkg
    if errorlevel 1 (
        echo %WARN_LABEL% Path to vcpkg.exe not specified and cannot be found, vcpkg manifest will not be configured
        goto VCPKG_CFG_END
    )
    echo Path to vcpkg.exe found
    set VCPKG_CMD=vcpkg
) else (
    if not "%VCPKG_EXE_FILE%"=="vcpkg.exe" (
        echo %WARN_LABEL% Path specifed does not end at vcpkg.exe, vcpkg manifest will not be configured
        goto VCPKG_CFG_END
    )
    set VCPKG_CMD="%VCPKG_EXE_PATH%"
)

copy /y "%VCPKG_MANI_SRC%" "%VCPKG_MANI_DES%" > NUL
%VCPKG_CMD% x-update-baseline --add-initial-baseline
echo %SUCC_LABEL% VCPKG manifest configured

:VCPKG_CFG_END


rem ==== OTHER DEPENDENCIES INSTALLATION =======================================

set ZIPPER=C:\Program Files\7-Zip\7z.exe
set QCP_DIR=%SRC_PATH%dependency\qcustomplot

if not exist "%ZIPPER%" (
    echo %ERROR_LABEL% Cannot find 7z executable - %ZIPPER%
    exit 1
)
if not exist "%QCP_DIR%" (
    echo %ERROR_LABEL% Cannot find qcustomplot output folder - %QCP_DIR%
    exit 1
)

echo Downloading qcustomplot archive...
bitsadmin /transfer "qcustomplot download" ^
    https://www.qcustomplot.com/release/2.1.1/QCustomPlot-source.tar.gz ^
    "%SRC_PATH%QCustomPlot-source.tar.gz"

echo Extracting qcustomplot...
"%ZIPPER%" e -aoa "%SRC_PATH%QCustomPlot-source.tar.gz" > NUL

cd "%QCP_DIR%"
"%ZIPPER%" e -aoa "%SRC_PATH%QCustomPlot-source.tar" ^
    qcustomplot-source\qcustomplot.cpp qcustomplot-source\qcustomplot.h ^
    > NUL

echo Cleaning up temporaries...
del "%SRC_PATH%QCustomPlot-source.tar.gz"
del "%SRC_PATH%QCustomPlot-source.tar"

set IS_SUCCESS=true
if not exist "%QCP_DIR%\qcustomplot.cpp" (
    echo %ERROR_LABEL% Failed to extract qcustomplot.cpp
    set IS_SUCCESS=false
)
if not exist "%QCP_DIR%\qcustomplot.h" (
    echo %ERROR_LABEL% Failed to extract qcustomplot.h
    set IS_SUCCESS=false
)

if %IS_SUCCESS%==true (
    echo %SUCC_LABEL% Dependencies installed
)
