@echo off
Title=DayZ Restarter
:loop
timeout /t 21600
taskkill /im DayZServer_x64.exe
cls
goto loop
