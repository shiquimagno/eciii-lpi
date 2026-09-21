@echo off
setlocal

set "ORIGINAL_PATH=%Path%"
set PATH=
set "Path=%ORIGINAL_PATH%"

set "VS_ROOT=C:\Program Files\Microsoft Visual Studio\18\Community"
set "VS_CMAKE=%VS_ROOT%\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe"
set "VS_NINJA=%VS_ROOT%\Common7\IDE\CommonExtensions\Microsoft\CMake\Ninja\ninja.exe"

if not exist "%VS_CMAKE%" (
    echo No se encontro CMake dentro de Visual Studio.
    exit /b 1
)

call "%VS_ROOT%\VC\Auxiliary\Build\vcvars64.bat"
if errorlevel 1 exit /b 1

"%VS_CMAKE%" --fresh -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_MAKE_PROGRAM="%VS_NINJA%"
if errorlevel 1 exit /b 1

"%VS_CMAKE%" --build build
if errorlevel 1 exit /b 1

echo Compilacion completada. Ejecutables en build\Release\
endlocal
