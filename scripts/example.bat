@echo off

REM # GADMIN-SAMBA example bat file for SAMBA netlogons.
REM # Rename this file to a corresponding UserName.bat or GroupName.bat.

echo Welcome to SAMBA24.

echo Setting current time...
net time \\samba24 /set /yes

REM # pause
