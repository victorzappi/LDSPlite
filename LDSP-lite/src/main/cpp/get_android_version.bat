@echo off
rem Get the Android version of the connected device
for /f "tokens=*" %%a in ('adb shell getprop ro.build.version.sdk') do set ANDROID_VERSION=%%a
rem Print the Android version
echo %ANDROID_VERSION%

