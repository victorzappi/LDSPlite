#!/bin/bash
# Get the Android API version of the connected device
ANDROID_API=$(adb shell getprop ro.build.version.sdk)
# Print the Android API
echo $ANDROID_API

