@echo off&setlocal enabledelayedexpansion

if not exist scripts (msg %username% E001: no scripts folder found! && goto :eof)

:: ca/dut config
for /F "tokens=1,2 delims==" %%i IN (scripts/config.txt) do set %%i=%%j
for /F "tokens=1,2 delims==" %%i IN (%PROGRAM_DIR%/config.txt) do set %%i=%%j

echo ##################################
echo ##                              ##
echo ##   Sigma Certification tool   ##
echo ##                              ##
echo ##################################
echo.
echo Testing device: %DEVICE%

set ORIGIN_DIR=%~dp0
:: list all program
CD %PROGRAM_DIR%
set "i=0"
for /f "tokens=*" %%f in ('dir /b/A:D') do (
    set arr[!i!]=%%f & set /a "i+=1"
)
set "len=!i!"
set "i=0"
:loop
echo %i%: !arr[%i%]!
set /a "i+=1"
if %i% neq %len% goto:loop
CD %ORIGIN_DIR%

set /p program= Enter program number:
if %program% GTR %len% echo Invalid number&goto :end
set testMode=!arr[%program%]!
:: trim string
for /F "tokens=* delims= " %%s in ('echo %testMode%') do set _trimmed=%%s
set testMode=%_trimmed:~0,-1%
echo.
echo Install for ^<^<%testMode%^>^>
echo %testMode% > scripts/testMode.txt
echo.

if not exist log (mkdir log)
echo wait for device ready
adb wait-for-device
adb root
adb wait-for-device

:disable_verity_done
echo root device
adb root
echo wait for device ready
adb wait-for-device

echo force to stop wifi
call stop_sigma.bat force

echo delete wifi.cfg/wifi_fw.cfg
adb shell rm data/misc/wifi/wifi.cfg
adb shell rm data/misc/wifi/wifi_fw.cfg


set CALLBACK=%PROGRAM_DIR%\%testMode%\callback_for_install.bat
if exist %CALLBACK% (call %CALLBACK% %testMode%)

adb shell rm -rf %BIN_DIR%
adb shell mkdir %BIN_DIR%
for /f %%f in (scripts/install_list.txt) do (adb push "scripts/%%f" %BIN_DIR%
adb shell chmod 755 %BIN_DIR%/%%~nxf)
adb shell "echo %WPA_CLI_CMD% > %BIN_DIR%/wpa_cli_cmd.txt"
adb shell ln -s %SH_PATH% %BIN_DIR%/sh

echo.
echo Software installation complete!!
color 0A

echo.
del /Q /F DUTInfo\Please_use*
set list=6631 6635
for %%a in (%list%) do (
    if exist DUTInfo\DUTInfo_%%a_gen4m (
        adb shell getprop | findstr vendor.connsys.adie.chipid | findstr %%a > nul && echo ************************************ && echo *** Please_use_DUTInfo_%%a_gen4m *** && echo ************************************ && echo. 2>DUTInfo\Please_use_DUTInfo_%%a_gen4m
    )
)

:end
pause
