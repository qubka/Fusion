@echo off
SETLOCAL ENABLEDELAYEDEXPANSION

SET "files="
FOR /f "delims=" %%i IN ('dir /b /s ".\src\Fusion\*.cs"') DO (
    SET files=!files! %%i
)
IF NOT EXIST "../assets/scripts" MKDIR "../assets/scripts"

csc -target:library -out:../assets/scripts/Fusion-ScriptCore.dll %files%