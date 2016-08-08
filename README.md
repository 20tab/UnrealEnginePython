# UnrealEnginePython
Embed Python in Unreal Engine 4

# How and Why ?

This is a plugin allowing the mapping of Actor and Components to python classes (and a lot more). It is not meant as a way to
avoid blueprints or c++ but as a good companion to them (albeit reducing the amount of c++ required for coding a game could be an interesting thing ;)

Another funny feature is that you can change your python code even after the project has been packaged. You can potentially build a completely new game from an already packaged one.

The plugin embeds the python3 library and automatically add an actor class (PyActor) and a component class (PythonComponent).
Both are available in your editor once you load the plugin.

All of the exposed engine features are under the 'unreal_engine' virtual module (it is completely coded in c into the plugin, so do not expect to run 'import unreal_engine' from a standard python shell)

# Installation on Windows (64 bit)

The installation is pretty long (and boring) as you do not want the final users of the product to be required to install python, so we need to use the 'embedded python distribution' (available for windows in the official python site). Unfortunately the embedded distribution does not contain the python development headers so we need the official system-wide installation too.

* install Unreal Engine 4.12 and Python 3.5 as a system user
* run the unreal editor and create a new blank c++ project
* once the project is setup, close both unreal and visual studio
* move to the project directory (you can right click the problem from the epic launcher and choose 'show in explorer')
* create a 'Plugins' directory into the project directory
* move into the just created Plugins directory and clone the repository:

```sh
git clone https://github.com/20tab/UnrealEnginePython
```

* from the explorer right click on the project main file and choose 'generate visual studio project files'
* open again visual studio, you should now see Plugins/UnrealEnginePython in your solution explorer
* before running the plugin build process you need to copy development headers and libs in the plugin directory (Plugins/UnrealEnginePython).
* create the directory Plugins/UnrealEnginePython/python35
* copy "C:/Program Files/Python35/include" and "C:/Program Files/Python35/libs" into Plugins/UnrealEnginePython/python35
* run the compilation from visual studio
* once the compilation ends, copy the python35.dll (from "C:/Program Files/Python35" or from the embeded distribution) into the Binaries/Win64 directory of your project (you will find the plugin dll too in this directory)
* now you can re-run the unreal engine editor

If all goes well, open the output log and search for the string "Python". You should see the Python VM initialization message. It means your editor has now full python support.

# Installation on MacOSX

On the Mac the installation is easier, as the final user is currently forced to install python on its system (there are obviously dozens of workarounds but at this stage of the project we prefer focusing on the api).

* install the latest official python distribution from python.org (the installation will end in the "Library/Frameworks/Python.framework/Versions/3.5" directory).
* create a new unreal engine blank c++ project
* create a Plugins directory in the project directory
* move to the Projects directory and clone the plugin repository

```sh
git clone https://github.com/20tab/UnrealEnginePython
```

* restart the editor and a popup should appear asking your for confirmation of the build of the plugin.
* Once the plugin is built, go to the output log console and filter for 'Python'. You should see the Python VM banner.

# Installation on other platforms

Currently only Windows and MacOSX are supported, Linux support should be available soon. We are investigating Android support too via the kivy project.

# Using Python with Unreal Engine (finally)

Creating a new blueprint class managed by python
------------------------------------------------

We are going to create a new Actor based on python (instead of C++ or blueprints)

In the content browser click on 'add new' and choose 'blueprint class'

In the classes menu choose 'PyActor':

![Alt text](screenshots/unreal_screenshot1.png?raw=true "Screenshot 1")

You now have a new asset, give it a meaningful name, and double click on it to start configuring it in the blueprint editor

![Alt text](screenshots/unreal_screenshot2.png?raw=true "Screenshot 2")

On the right (in the 'Details' tab) you will find the Python section.

For now only 'Python Module' and 'Python Class' are meaningful.

Go to the Content directory of your project and create a directory named 'Scripts'. This is where all of your python modules will reside. With your favourite text editor create a new python module (like funnygameclasses.py), and define a new class into it:

```py
import unreal_engine as ue

ue.log('Hello i am a Python module')

class Hero:

    def begin_play(self):
        ue.log('Begin Play on Hero class')
        
    def tick(self, delta_time):
        # get current location
        x, y, z = self.uobject.get_actor_location()
        # increase Z honouring delta_time
        z += 100 * delta_time
        # set new location
        self.uobject.set_actor_location(x, y, z)

```

Now, go back to the blueprint editor and set 'funnygameclasses' in the 'Python Module' field, and 'Hero' in 'Python Class'

As you can see the actor will simply move over the z axis, but we need to give it some kind of visual representation to have a feedback in the scene. In the blueprint editor click on 'add component' and add some shape (a spehere, or a cube, or whatever you want). Save and Compile your blueprint.

Now you can drag the bluprint from the content browser to the scene and just click 'Play'.

You should see your actor moving along the 'z' axis at a speed of 1 meter per second

What is 'self.uobject' ?
------------------------

The python class is completely unrelated to Unreal Engine, but the PyActor class adds to it a 'uobject' field pointing to a special wrapper class (UPyObject). This single class manages all the UObject classes available in the engine, using (well, abusing) the c++ reflection system available in Unreal Engine. So whenever you want to access the engine you need to start from the 'uobject' field.

In the PyActor class, 'uobject' points the the related AActor, while when you use PythonComponent, uobject points to the related UActorComponent.

UPyObject are simple Unreal Engine objects dynamically attached to every engine object you need to interact with in your code.

So a call to:

```py
text_render_component = unreal_engine.find_class('TextRenderComponent')
```

will internally search for the 'TextRenderComponent' class (via c++ reflection) and if found, will create a new UPyObject holding a reference to a python class (ue_PyUObject internally) and will attach to the just found class using the __PyObject key.

Thanks to this approach, whenever we need to reference a UObject in python, the plugin will first search for the '__PyObject' subobject and if found will directly return it (without regenerating it')

Adding a python component to an Actor
-------------------------------------

The unreal_engine module
------------------------

```py
uclass = unreal_engine.find_class('name')
```

```py
unreal_engine.log('message')
```

```py
unreal_engine.log_warning('message')
```

```py
unreal_engine.log_error('message')
```

```py
unreal_engine.add_on_screen_debug_message(key, timeout, 'message')
```

```py
unreal_engine.print_string('message')
```

```py
unreal_engine.vector_add_vector(x, y, z[, x1, y1, z1, ...])
```

```py
unreal_engine.vector_add_float(x, y, z[, d1, d2, ...])
```

```py
unreal_engine.vector_mul_vector(x, y, z[, x1, y1, z1, ...])
```

```py
unreal_engine.vector_mul_float(x, y, z[, d1, d2, ...])
```


The uobject api
---------------

```py
x, y, z = uobject.get_actor_location()
```

```py
uobject.set_actor_location(x, y, z)
```

```py
pitch, yaw, roll = uobject.get_actor_rotation()
```

```py
uobject.set_actor_rotation(pitch, yaw, roll)
```

```py
x, y, z = uobject.get_actor_forward()
```

```py
x, y, z = uobject.get_actor_right()
```

```py
x, y, z = uobject.get_actor_up()
```

```py
x, y, z = uobject.get_actor_velocity()
```

```py
value = uobject.get_property('name')
```

```py
properties_list = uobject.properties()
```

```py
uobject.call('function')
```

```py
actor = uobject.get_owner()
```

```py
name = uobject.get_name()
```

```py
full_name = uobject.get_full_name()
```

```py
# really slow !
found_uobject = uobject.find_object('name')
```

```py
# really really slow !
found_uobjects = uobject.all_objects()
```

```py
found_actors = uobject.all_actors()
```

```py
uclass = uobject.get_class()
```

```py
uclass = uobject.actor_spawn(uclass[, x, y, z, pitch, yaw, roll])
```

```py
uobject.actor_destroy()
```

```py
yesno = uobject.actor_has_component_of_type(uclass)
```

```py
components = uobject.actor_components()
```

```py
uobject.enable_input()
```

```py
uobject.bind_input_axis('axis')
```

```py
uobject.quit_game()
```

```py
yesno = uobject.is_input_key_down('key')
```

```py
value = uobject.get_input_axis('axis')
```

Math functions
--------------

Memory management
-----------------
