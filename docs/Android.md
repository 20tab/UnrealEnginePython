# Deploying on Android devices

Starting from release 20180723 you can deploy the UnrealEnginePython plugin on android devices (both for launching and for packaging).

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
