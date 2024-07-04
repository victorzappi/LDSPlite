@echo off
rem Get the Android API of the connected device
for /f "tokens=*" %%a in ('adb shell getprop ro.build.version.sdk') do set ANDROID_API=%%a
rem Print the Android API
echo %ANDROID_API

