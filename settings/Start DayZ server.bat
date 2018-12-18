@echo off
cls
set watch=DayZ Server
title %watch% Watchdog
cd D:\DayZServer
:watchdog
echo (%time%) %watch% started.
start "DayZ Server" /wait "DayZServer_x64.exe" -config=serverDZ.cfg -port=2302 -dologs -adminlog -netlog -freezecheck -BEpath=D:\DayZServer\battleye -profiles=D:\DayZServer\profile -scrAllowFileWrite "-mod=@RPCFramework;@Permissions Framework;@Community Online Tools;@KillFeed;"
echo (%time%) %watch% closed or crashed, restarting.
goto watchdog