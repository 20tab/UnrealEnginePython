# Deploying on Android devices

Starting from release 20180723 you can deploy the UnrealEnginePython plugin on android devices (both for launching and for packaging).

The python distribution is the one from the "Crystax NDK" project (https://www.crystax.net/android/ndk), but you do not need to download it as this repository
already contains the required shared libraries and headers as well as the the python standard lib compressed in a zip file:

https://github.com/20tab/UnrealEnginePython/tree/master/android/armeabi-v7a

https://github.com/20tab/UnrealEnginePython/tree/master/android/python35

Albeit the repository contains the files for python2.7, currently the only supported release is the 3.5.

Obviously you need UnrealEnginePython source distribution to rebuild the plugin for Android.
