@ECHO OFF

REM PUT THE COMPILER IN THE PATH IF IT ISN'T ALREADY.
WHERE cl.exe
REM IF %ERRORLEVEL% NEQ 0 CALL "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
CALL "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
WHERE cl.exe

SET TOOL_DIRECTORY_PATH="C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Tools\MSVC\14.22.27905\bin\Hostx64\x64"
SET COMPILER_PATH="%TOOL_DIRECTORY_PATH%\cl.exe"
SET LIB_TOOL_PATH="%TOOL_DIRECTORY_PATH%\lib.exe"

REM READ THE BUILD MODE COMMAND LINE ARGUMENT.
REM Either "debug" or "release" (no quotes).
REM If not specified, will default to debug.
SET build_mode=%1

REM DEFINE COMPILER OPTIONS.
SET COMMON_COMPILER_OPTIONS=/EHsc /WX /W4 /TP /std:c++latest
SET DEBUG_COMPILER_OPTIONS=%COMMON_COMPILER_OPTIONS% /Z7 /Od /MTd
SET RELEASE_COMPILER_OPTIONS=%COMMON_COMPILER_OPTIONS% /O2 /MT

REM DEFINE FILES TO COMPILE/LINK.
SET COMPILATION_FILE="..\SoftwareRendererLibrary.project"
SET MAIN_CODE_DIR="..\code"
SET LIBRARIES=user32.lib gdi32.lib

REM CREATE THE COMMAND LINE OPTIONS FOR THE FILES TO COMPILE/LINK.
SET INCLUDE_DIRS=/I %MAIN_CODE_DIR%
SET PROJECT_FILES_DIRS_AND_LIBS=%COMPILATION_FILE% %INCLUDE_DIRS% /link %LIBRARIES%

REM MOVE INTO THE BUILD DIRECTORY.
IF NOT EXIST "build" MKDIR "build"
PUSHD "build"

    REM BUILD THE PROGRAM BASED ON THE BUILD MODE.
    IF "%build_mode%"=="release" (
        "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Tools\MSVC\14.25.28610\bin\Hostx64\x64\cl.exe" %RELEASE_COMPILER_OPTIONS% %PROJECT_FILES_DIRS_AND_LIBS%
    ) ELSE (
        "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Tools\MSVC\14.25.28610\bin\Hostx64\x64\cl.exe" %DEBUG_COMPILER_OPTIONS% %PROJECT_FILES_DIRS_AND_LIBS%
    )

    "%LIB_TOOL_PATH%" "SoftwareRendererLibrary.obj"

POPD

ECHO Done

@ECHO ON
