@echo off
SETLOCAL ENABLEDELAYEDEXPANSION

SET "files="
FOR /f "delims=" %%i IN ('dir /b /s ".\Fusion\Fusion\src\*.cs"') DO (
    SET files=!files! %%i
)
IF NOT EXIST "../assets/scripts" MKDIR "../assets/scripts"

csc -target:library -reference:System.Numerics.dll -out:../assets/scripts/Fusion-ScriptCore.dll %files%
pause