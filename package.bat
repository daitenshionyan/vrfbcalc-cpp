ECHO OFF
SET name=%1
SET binDir=%2
SET srcDir=%3
SET outDir=%4

SET binDir=%binDir:/=\%
SET srcDir=%srcDir:/=\%
SET outDir=%outDir:/=\%

SET zipper="C:\Program Files\7-Zip\7z.exe"

ECHO ---- [ PACKAGING ] -------------------------------------------------------------
IF EXIST "%outDir%\%name%.zip" (
    ECHO Deleting archive: %outDir%\%name%.zip
    DEL "%outDir%\%name%.zip"
)

IF EXIST %zipper% (
    CD "%binDir%"
    %zipper% a -tzip "%outDir%\%name%.zip" "%name%.exe" ^
        | findstr ^
            /c:"Creating archive" ^
            /c:"Add new data to archive:" ^
        || GOTO PACK_END
    %zipper% a -tzip "-x!*.exe" "-xr!output" "%outDir%\%name%.zip" * ^
        | findstr ^
            /c:"Add new data to archive:" ^
        || GOTO PACK_END
    CD "%srcDir%"
    %zipper% a -tzip "%outDir%\%name%.zip" COPYING COPYING.LESSER README.md ^
        | findstr ^
            /c:"Add new data to archive:" ^
            /c:"Archive size" ^
        || GOTO PACK_END
) ELSE (
    ECHO Failed to zip as %zipper% not found
)
:PACK_END
