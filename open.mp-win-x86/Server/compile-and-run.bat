@echo off
cd /d %~dp0
qawno\pawncc.exe gamemodes\auction-containers\auction-containers.pwn -iqawno\include
if %errorlevel% == 0 (
    start omp-server.exe
)
