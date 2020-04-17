# Deploying on Android devices

Starting from release 20180723 and engine 4.20 you can deploy the UnrealEnginePython plugin on android devices (both for launching and for packaging).

The python distribution is the one from the "Crystax NDK" project (https://www.crystax.net/android/ndk), but you do not need to download it as this repository
already contains the required shared libraries and headers as well as the the python standard lib compressed in a zip file:

https://github.com/20tab/UnrealEnginePython/tree/master/android/armeabi-v7a

https://github.com/20tab/UnrealEnginePython/tree/master/android/python35

Albeit the repository contains the files for python2.7, currently the only supported release is the 3.5.

Obviously you need UnrealEnginePython source distribution to rebuild the plugin for Android.

## The First Project

I am assuming your project is already deployable on Android and now you want to add a simple Python Component to one of its Character:

```python
import unreal_engine as ue

class AutoJump:

    def begin_play(self):
        self.timer = self.uobject.set_timer(3, self.jump, True)

    def jump(self):
        self.uobject.get_owner().Jump()
```

Once the script works in the editor you need to prepare your environment to be android-friendly.

The first step is configuring the packaging system to include the /Content/Scripts directory (where you have your python files, currently the one you created before for your Character):

![android_packaging](https://github.com/20tab/UnrealEnginePython/raw/master/docs/android_packaging.png)

just add the Scripts directory as a new item in the "Additional Non Assets Directories to Copy".

Finally get https://github.com/20tab/UnrealEnginePython/blob/master/android/python35/stdlib.zip and copy into the Scripts directory.

This is the python standard library required by libpython to correctly bootstrap itself. (DO NOT FORGET THIS STEP, WITHOUT IT YOUR ANDROID APP WILL RUN BUT CRASHES AFTER THE SPLASH SCREEN).

The stdlib.zip filename is hardcoded, so do not rename it (unless you want to modify UnrealEnginePython sources).

Now you can Launch your project over the Android device.

Before launching the UnrealEnginePython plugin will be built for the arm architecture, then an apk will be uploaded as well as your Content directory. If all goes wall you should see your Character auto-jumping in the Android device.

## Packaging

Packaging is a bit more complex as Unreal generates a fake .obb file along the .apk file.

This .obb file is a simple .zip file containing the .pak file as well as your Scripts directory. As this is not a true android .obb file, we cannot mount it as a filesystem (using the android StorageManager api) so we need to treat it as a simple zip file.

Lucky enough, python supports getting modules from a zip file, so just adding the .obb file to its modules search path will be more than enough (the UnrealEnginePython plugin does it automatically).

The problem is that python does not support zip files into zip files (stdlib.zip is into the .obb file) so to allow packaged games to run we need to decompress stdlib.zip in the Scripts directory. This will result in our Scripts directory being clobbered with additional .py files (feel free to propose better solutions).

Once the stdlib.zip file is decompressed you can package your project and upload it to the device.

## Note

Try to use python 3.5 even for the editor, otherwise the files extracted from stdlib.zip could cause problems (the SRE exception is the first you should get in such a case)

Python 3.6, albeit not tested, should work.

Any python 3 release built from the Crystax NDK should work too. Just compile it, copy the related files into the repository android/ directory and change https://github.com/20tab/UnrealEnginePython/blob/master/Source/UnrealEnginePython/UnrealEnginePython_APL.xml and 
https://github.com/20tab/UnrealEnginePython/blob/master/Source/UnrealEnginePython/UnrealEnginePython.Build.cs accordingly.
