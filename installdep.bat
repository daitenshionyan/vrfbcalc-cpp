@echo off
SETLOCAL DISABLEDELAYEDEXPANSION

set ERROR_LABEL=[91m[ERROR][0m
set SUCC_LABEL=[92m[SUCCESS][0m

set SRC_PATH=%~dp0

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
