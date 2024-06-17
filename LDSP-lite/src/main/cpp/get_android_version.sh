#!/bin/bash
# Get the Android version of the connected device
ANDROID_VERSION=$(adb shell getprop ro.build.version.sdk)
# Print the Android version
echo $ANDROID_VERSION

