
# UnrealEnginePython
Embed Python in Unreal Engine 4

Teaser (by Kite & Lightning): https://twitter.com/KNLstudio/status/932657812466843648

Fixing Mixamo RootMotion tuturial: https://github.com/20tab/UnrealEnginePython/blob/master/tutorials/FixingMixamoRootMotionWithPython.md

Funny snippets for working with StaticMesh and SkeletalMesh assets: https://github.com/20tab/UnrealEnginePython/blob/master/tutorials/SnippetsForStaticAndSkeletalMeshes.md

More tutorials: https://github.com/20tab/UnrealEnginePython/tree/master/tutorials

# How and Why ?

This is a plugin embedding a whole Python VM (versions 3.x [the default and suggested one] and 2.7) In Unreal Engine 4 (both the editor and runtime).

The Python VM tries to give easy access to all of the UE4 internal api + its reflection system. This means you can use the plugin to write other plugins, to automate tasks, to write unit tests and to implement gameplay elements.

It is not meant as a way to avoid blueprints or c++ but as a good companion to them (albeit reducing the amount of c++ required for coding a game could be an interesting thing ;). If your development pipeline is already python-based (Maya, Blender, ...), this plugin could easily help you in integrating unreal into it.

If you want to have an idea of what the plugin can do, jump here: https://github.com/20tab/UnrealEnginePython/blob/master/tutorials/YourFirstAutomatedPipeline.md

In addition to this, the plugin automatically adds an actor class (PyActor), a pawn class (PyPawn), a character class (PyCharacter) and a component class (PythonComponent) for "gentle" integration of python in your games.

In the spirit of automating tasks, even wrappers for third party libraries used by UE4 are exposed in a 'pythonic' way. As an example the FbxSdk is exposed to allow low-level interaction with Fbx files. This is an example extracting animation curves: https://github.com/20tab/UnrealEnginePython/blob/master/examples/fbx_curves_extractor.py

Another funny feature (well, a side effect ;) is that you can change your python code even after the project has been packaged. You can potentially build a completely new game from an already packaged one.

Once the plugin is installed and enabled, you get access to the 'PythonConsole' item in the 'Development Menu', you can use it to trigger python commands directly from the editor. There is even an experimental Editor/IDE included, you can run it from the Window/Layout/Python Editor menu item.

All of the exposed engine features are under the 'unreal_engine' virtual module (it is completely coded in c into the plugin, so do not expect to run 'import unreal_engine' from a standard python shell)

The minimal supported Unreal Engine version is 4.12, while the latest is 4.23

We support official python.org releases as well as IntelPython and Anaconda distributions.

Note: this plugin has nothing to do with the experimental 'PythonScriptPlugin' included in Unreal Engine >= 4.19. We aim at full integration with engine and editor (included the Slate api, check here: https://github.com/20tab/UnrealEnginePython/blob/master/docs/Slate_API.md), as well as support for the vast majority of python features like asyncio, coroutines, generators, threads and third party modules.

# Binary installation on Windows (64 bit)

Check in the releases page (https://github.com/20tab/UnrealEnginePython/releases) if there is a binary version that matches your configuration (otherwise open an issue asking us for it [please specify the python version too]) and download it.

Binary releases are in two forms: standard and embedded. Standard uses the python installation of your system, so ensure the python installation directory is in your system PATH environment variable (otherwise you will get an error while loading your project). Embedded releases include an embedded python installation so you do not need to have python in your system.

Create (if it does not already exist) a Plugins directory in your project root directory (at the same level of Content/ and the .uproject file) and unzip the plugin into it. If your project is named FooBar you will end with FooBar/Plugins/UnrealEnginePython.

Open your project and go to the Edit/Plugins menu. Go to the bottom and under "Project/Scripting Languages" enable UnrealEnginePython.

Restart your project and you should see the PythonConsole under the "Window/Developer Tools" menu

Binary releases are mainly useful for editor scripting, if you want to package your project for distribution and you need the python runtime, you need a source release (see below).

If instead, you want to package your project without python, just remember to change the UnrealEnginePython.uplugin to have this line: https://github.com/20tab/UnrealEnginePython/blob/master/UnrealEnginePython.uplugin#L20 set as "Editor" instead of "Runtime"

# Installation from sources on Windows (64 bit)

Currently python3.6, python3.5 and python2.7 are supported. It is highly suggested to have a python system wide installation (by default the official python distributions are installed in user's home directory) with the PATH environment variable including it (if you change the PATH variable remember to reboot the system before running the build procedure, this is not strictly required but will ensure the PATH is updated). If the PATH variable does not contain the path of your python installation you will see a warning in the build log/output.

Download a source official release or simply clone the repository for latest updates:

```sh
git clone https://github.com/20tab/UnrealEnginePython
```

By default the build procedure will try to discover your python installation looking at hardcoded known paths. If you want to specify a custom python installation (or the autodetection simply fails) you can change it in the Source/UnrealEnginePython/UnrealEnginePython.Build.cs file at this line: https://github.com/20tab/UnrealEnginePython/blob/master/Source/UnrealEnginePython/UnrealEnginePython.Build.cs#L10

Note: ensure you have a 64bit python installation


choose a project you want to install the plugin into, open the file explorer (you can do it from the epic launcher too) and:

* create a Plugins/ directory (if it does not exist) in your project and copy the directory UnrealEnginePython into it
* from the file explorer right click on the project main file and choose 'generate visual studio project files'
* open visual studio, you should now see Plugins/UnrealEnginePython in your solution explorer
* run the compilation from visual studio
* once the compilation ends, double check the python libraries can be found by the plugin (they must be in the system PATH like previously described, or brutally copy them in the Binaries/Win64 directory of the just built plugin)
* now you can re-run the unreal engine editor

If all goes well, you will see 'Python Console' in the "Window/Developer Tools" menu

If you want to package your project (it is required only if you need to have a python VM at runtime, read: your game logic is programmed in python) ensure the Content/Scripts/ue_site.py file is in your project (it can be empty). At the end of the build procedure ensure to copy all of your required python scripts in the final directory. Remember that unless you add an embedded python in your final build, the final users of your project will require python installed in his/her system.

If you want to package without python, just remember to change the UnrealEnginePython.uplugin to have this line: https://github.com/20tab/UnrealEnginePython/blob/master/UnrealEnginePython.uplugin#L20 set as "Editor" instead of "Runtime"

# Binaries installation on MaxOSX

Check in the releases page (https://github.com/20tab/UnrealEnginePython/releases) if there is a binary version that matches your configuration (otherwise open an issue asking us for it [please specify the python version too]) and download it.

Binary releases for MacOSX expects an official python installation (the packages you get from python.org).

Create (if it does not already exist) a Plugins directory in your project root directory (at the same level of Content/ and the .uproject file) and unzip the plugin into it. If your project is named FooBar you will end with FooBar/Plugins/UnrealEnginePython.

Open your project and go to the Edit/Plugins menu. Go to the bottom and under "Project/Scripting Languages" enable UnrealEnginePython.

Restart your project and you should see the PythonConsole under the "Window/Developer Tools" menu

Binary releases are mainly useful for editor scripting, if you want to package your project for distribution and you need the python runtime, you need a source release (see below).

If instead, you want to package your project without python, just remember to change the UnrealEnginePython.uplugin to have this line: https://github.com/20tab/UnrealEnginePython/blob/master/UnrealEnginePython.uplugin#L20 set as "Editor" instead of "Runtime"

# Installation from sources on MacOSX

* install the latest official python distribution from python.org (the installation will end in the "/Library/Frameworks/Python.framework/Versions/X.Y" directory).
* create a new unreal engine blank c++ project (NOT a blueprint one, otherwise XCode will not be initialized)
* create a Plugins directory in the project directory
* move to the Plugins directory and clone the plugin repository


```sh
git clone https://github.com/20tab/UnrealEnginePython
```

* restart the editor and a popup should appear asking your for confirmation of the build of the plugin.
* Once the plugin is built, go to the output log console and filter for 'Python'. You should see the Python VM banner.

The build procedure will try to automatically discover python installations. If you need custom paths, just edit here:

https://github.com/20tab/UnrealEnginePython/blob/master/Source/UnrealEnginePython/UnrealEnginePython.Build.cs#L10

Upgrading on MacOSX
-------------------

To upgrade to the latest development version of UnrealEnginePython:
* move to the Plugins directory in the project directory and use git pull

```sh
git pull
```

* move to UnrealEnginePython/Binaries/Mac from the Plugin directory
* remove the plugin libraries to warn UnrealEngine to recompile the plugin

```sh
rm *.dylib
```

* restart the editor and a popup should appear asking your for confirmation of the build of the plugin.
* Once the plugin is built, go to the output log console and filter for 'Python'. You should see the Python VM banner.

Installation from sources On Linux (64 bit)
-------------------------------------------

Currently the suggested distribution is Ubuntu Xenial (LTS 16.04) 64bit. Obviously you need to already have an Unreal Engine build (note that on ubuntu xenial you need to install the clang-3.5 package to build the editor). Both python2.7 and python3.5 are supported and the default configuration assumes python3 (so ensure to install the python3-dev package).

* Create a new C++ project and close the editor once the project is fully started
* go to the just created project directory and create the Plugins folder
* move to the Plugins folder and clone the plugin repository:


```sh
git clone https://github.com/20tab/UnrealEnginePython
```

* re-open your project, this time you will get a popup asking you for re-building the python plugin. Choose yes and wait.

NOTE: always run your project from a terminal so you can see startup logs (they are really useful when building the plugin the first time, if you cannot build the plugin, open an issue on github pasting the related log lines).

If you want to use python2 (or another specific version) just edit the Source/UnrealEnginePython/UnrealEnginePython.Build.cs file and change the pythonHome string accordingly (ensure to have the python2.7-dev package installed).


Upgrade the plugin on Linux
---------------------------

Just remove the .so files in Plugins/UnrealEnginePython/Binaries/Linux and pull the latest code.

At the next run the build procedure wil be started again.

Android Deployment
------------------

Check https://github.com/20tab/UnrealEnginePython/blob/master/docs/Android.md

# Installation on other platforms

Currently only Windows, MacOSX, Linux and Android are supported.

# Using Python with Unreal Engine (finally)

If your objective is to script the editor, you can directly jump to

https://github.com/20tab/UnrealEnginePython/tree/master/docs

and

https://github.com/20tab/UnrealEnginePython/tree/master/examples

The first directory contains the official documentation for specific areas, while the second one is a collection of python scripts doing any sort of 'magic' with your project ;)

Creating a new blueprint class managed by python
------------------------------------------------

We are going to create a new Actor based on python (instead of C++ or blueprints)

This is the "gentle" approach, using a 'proxy' python class to speak with the UE4 api. Once you get familiar with the system, you can
go further and start working withe native subclassing api (https://github.com/20tab/UnrealEnginePython/blob/master/docs/Subclassing_API.md) 

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

    # this is called on game start
    def begin_play(self):
        ue.log('Begin Play on Hero class')
        
    # this is called at every 'tick'    
    def tick(self, delta_time):
        # get current location
        location = self.uobject.get_actor_location()
        # increase Z honouring delta_time
        location.z += 100 * delta_time
        # set new location
        self.uobject.set_actor_location(location)

```

Now, go back to the blueprint editor and set 'funnygameclasses' in the 'Python Module' field, and 'Hero' in 'Python Class'

As you can see the actor will simply move over the z axis, but we need to give it some kind of visual representation to have a feedback in the scene. In the blueprint editor click on 'add component' and add some shape (a sphere, or a cube, or whatever you want). Save and Compile your blueprint.

Now you can drag the bluprint from the content browser to the scene and just click 'Play'.

You should see your actor moving along the 'z' axis at a speed of 1 meter per second

By default a 'begin_play' and a 'tick' method are expected (they will be automatically taken into account if found). In addition to them an 'automagic' system for defining event is available:

```py
def on_actor_begin_overlap(self, me, other_actor):
    pass

def on_actor_end_overlap(self, me, other_actor):
    pass
    
def on_actor_hit(self, me, other_actor, normal_impulse, hit_result):
    pass

...
```

Basically for each method startwing with 'on_' the related delegate/event is automatically configured (if available).

If you instead prefer to manually setup events, the following functions are exposed:

```py

class Ball:

    def begin_play(self):
        self.uobject.bind_event('OnActorBeginOverlap', self.manage_overlap)
        self.uobject.bind_action('Jump', ue.IE_PRESSED, self.uobject.jump)
        self.uobject.bind_key('K', ue.IE_PRESSED, self.you_pressed_K)
        self.uobject.bind_axis('MoveForward', self.move_forward)
        
    def manage_overlap(self, me, other):
        ue.print_string('overlapping ' + other.get_name())
        
    def you_pressed_K(self):
        ue.log_warning('you pressed K')
        
     def move_forward(self, amount):
        ue.print_string('axis value: ' + str(amount))
        

```


What is 'self.uobject' ?
------------------------

To allow seamless Python integration, each UObject of the engine is automatically mapped to a special Python Object (ue_PyUObject).

Whenever you want to access a UObject from python, you effectively get a reference to a ue_PyUObject exposing (via its methods) the features of the UObject (properties, functions, ....)

This special python object is cached into a c++ map in memory. (The key is the UObject pointer, the value is the ue_PyUObject pointer)

To be more clear, a call to:

```py
text_render_component = unreal_engine.find_class('TextRenderComponent')
```

will internally search for the 'TextRenderComponent' class (via unreal c++ reflection) and when found will check if it is available in the cache, otherwise it will create a new ue_PyUObject object that will be placed in the cache.

From the previous example the 'text_render_component' maintains a mapping to the UObject (well a UClass in this example).

Pay attention: the python class you map to the PyActor (or PyPawn, PyCharacter or PyComponent), is not a ue_PyUObject. It is a classic python class that holds a reference (via the 'uobject' field) to the related ue_PyUObject mapped object. The best technical term to describe those classes is 'proxy'.

Note about 'uobject' from now on
---------------------------------

In the following lines, whenever you find a reference to 'uobject' it is meant as a ue_PyUObject object.

Adding a python component to an Actor
-------------------------------------

This works in the same way as the PyActor class, but it is, well, a component. You can attach it (search for the 'Python' component) to any actor.

Remember that for components, the self.uobject field point to the component itself, not the actor.

To access the actor you can use:

```py
actor = self.uobject.get_owner()
```

The following example implements the third person official blueprint as a python component:

```py
class Player:
    
    def begin_play(self):
        # get a reference to the owing pawn (a character)
        self.pawn = self.uobject.get_owner()

        # the following two values were originally implemented as blueprint variable
        self.base_turn_rate = 45.0
        self.base_look_up_rate = 45.0

        # bind axis events
        self.pawn.bind_axis('TurnRate', self.turn)
        self.pawn.bind_axis('LookUpRate', self.look_up)
        self.pawn.bind_axis('Turn', self.pawn.add_controller_yaw_input)
        self.pawn.bind_axis('LookUp', self.pawn.add_controller_pitch_input)

        self.pawn.bind_axis('MoveForward', self.move_forward)
        self.pawn.bind_axis('MoveRight', self.move_right)

        # bind actions
        self.pawn.bind_action('Jump', ue.IE_PRESSED, self.pawn.jump)
        self.pawn.bind_action('Jump', ue.IE_RELEASED, self.pawn.stop_jumping)

    def turn(self, axis_value):
        turn_rate = axis_value * self.base_turn_rate * self.uobject.get_world_delta_seconds()
        self.pawn.add_controller_yaw_input(turn_rate)

    def look_up(self, axis_value):
        look_up_rate = axis_value * self.base_look_up_rate * self.uobject.get_world_delta_seconds()
        self.pawn.add_controller_pitch_input(look_up_rate)

    def move_forward(self, axis_value):
        rot = self.pawn.get_control_rotation()
        fwd = ue.get_forward_vector(0, 0, rot[2])
        self.pawn.add_movement_input(fwd, axis_value)

    def move_right(self, axis_value):
        rot = self.pawn.get_control_rotation()
        right = ue.get_right_vector(0, 0, rot[2])
        self.pawn.add_movement_input(right, axis_value)
```

Native methods VS reflection
----------------------------

By default the UObject class defines __getattr__ and __setattr__ as wrappers for unreal properties and functions.

This means that calling:

```py
self.uobject.bCanBeDamaged = True
```

it is the same as

```py
self.uobject.set_property('bCanBeDamaged', True)
```

As well as function calls:

```py
vec = self.uobject.GetActorRightForward()
```

means

```py
vec = self.uobject.call_function('GetActorRightForward')
```

And more important (and handy) K2_ functions are automagically exposed too:

```py
vec = self.uobject.GetActorLocation()
```

is equal to:

```py
vec = self.uobject.call_function('K2_GetActorLocation')
```

Obviously you can combine methods/properties:

```py
self.uobject.CharacterMovement.MaxWalkSpeed = 600.0
```

Albeit the system allows for full unreal api usage, reflection is slower than native methods.

Try to use native methods whenever possible, and open pull request whenever you think a function should be exposed as native methods.

So

```py
vec = self.uobject.get_actor_location()
```

is way faster than

```py
vec = self.uobject.GetActorLocation()
```

Reflection based functions are those in camelcase (or with the first capital letter). Native functions instead follow the python style, with lower case, underscore-as-separator function names.

Note that, in editor builds, when you change the property of an archetype (included ClassDefaultObject) via __setattr__ all of the archtype instances will be updated too.

To be more clear:

```python
your_blueprint.GeneratedClass.get_cdo().CharacterMovement.MaxWalkSpeed = 600.0
```

is a super shortcut for:

```python
your_blueprint.GeneratedClass.get_cdo().CharacterMovement.pre_edit_change('MaxWalkSpeed')
your_blueprint.GeneratedClass.get_cdo().CharacterMovement.set_property('MaxWalkSpeed', 600.0)
your_blueprint.GeneratedClass.get_cdo().CharacterMovement.post_edit_change_property('MaxWalkSpeed')
for instance in your_blueprint.GeneratedClass.get_cdo().CharacterMovement.get_archetype_instances():
    instance.pre_edit_change('MaxWalkSpeed')
    instance.set_property('MaxWalkSpeed', 600.0)
    instance.post_edit_change_property('MaxWalkSpeed')
```


The automagic UClass, UStruct and UEnums mappers
------------------------------------------------

Instead of doing a gazilion of unreal_engine.find_class(name) calls, the plugin adds three 'magic' modules called unreal_engine.classes, unreal_engine.structs and unreal_engine.enums. They allows to import unreal classes/structs/enums like python classes:

```py
from unreal_engine.classes import ActorComponent, ForceFeedbackEffect, KismetSystemLibrary

...
components = self.uobject.get_owner().GetComponentsByClass(ActorComponent)

...
self.force_feedback = ue.load_object(ForceFeedbackEffect, '/Game/vibrate')
self.uobject.get_player_controller().ClientPlayForceFeedback(self.force_feedback)

...
name = KismetSystemLibrary.GetObjectName(self.actor)
```

the last example, shows another magic feature: static classes function calls. Obviously in this specific case using self.actor.get_name() would have been the best approach, but this feature allows you to access your blueprint function libraries too.

Another example for adding a widget:

```py
from unreal_engine.classes import WidgetBlueprintLibrary

class PythonFunnyActor:
    def begin_play(self):
        WidgetBlueprintLibrary.Create(self.uobject, ue.find_class('velocity_C'))
```

And another complex example using enums, keyword arguments and output values (output values are appended after the return value):

```py

import unreal_engine as ue
from unreal_engine import FVector, FRotator, FTransform, FHitResult
from unreal_engine.classes import ActorComponent, ForceFeedbackEffect, KismetSystemLibrary, WidgetBlueprintLibrary
from unreal_engine.enums import EInputEvent, ETraceTypeQuery, EDrawDebugTrace

...

is_hitting_something, hit_result = KismetSystemLibrary.LineTraceSingle_NEW(self.actor, self.actor.get_actor_location(), FVector(300, 300, 300), ETraceTypeQuery.TraceTypeQuery1, DrawDebugType=EDrawDebugTrace.ForOneFrame)
if is_hitting_something:
    ue.log(hit_result)
```

To create a new struct instance you can do:

```python
from unreal_engine.structs import TerrificStruct

ts = TerrificStruct()
```

or (to initialize some of its fields)

```python
from unreal_engine.structs import TerrificStruct

ts = TerrificStruct(Foo='Bar', Test=17.22)
```

To access the fields of a struct just call the fields() method.

A good example of struct usage is available here: https://github.com/20tab/UnrealEnginePython/blob/master/docs/Settings.md


More details here: https://github.com/20tab/UnrealEnginePython/blob/master/docs/MemoryManagement.md

The ue_site.py file
-------------------

On Editor/Engine start, the ue_site module is tried for import. You should place initialization code there. If the module cannot be imported, you will get a (harmful) message in the logs.

PyPawn
------

This works like PyActor, but this time you generate a new Pawn class (that you can posses with a controller)


The 'World' concept
-------------------

Every actor is mapped to a world (UWorld in c++). Generally when you play on a Level your objects all live in the same world, but at the same time there could be multiple worlds (for example while testing in the editor there is a world for the editor and one for the simulation)

While it is pretty rare to reference other worlds, you may need to compare the world of two uobject's (for example you may have a reference in your python module to a uobject of a hidden world and you want to check if you need to use it).

The uobject.get_world() function returns a uobject representing the world (the C++ UWorld class)

The uobject api
---------------

Each uobject represent a UObject class of the Engine. This C++ class is basically the root of all the other classes (Actors, Pawns, components, properties ...). Thanks to Unreal Engine reflection system we do not need to implement a python class for each unreal engine class, but for performance reason we expose the most common methods. The uobject system checks for the type of the mapped C++ UObject and will call the method only if it is safe to call it.

Sometime methods are implemented for automatically getting the right object. As an example get_actor_location() when called over a component will automatically retrieve the related actor and will call C++ AActor::GetActorLocation() method over it.

When this automagic approach is too risky, the method will check for the uobject type and will raise an exception in the case of inconsistencies.

Remember, there is no need to implement every single engine class method, the reflection system is powerful enough to be governed only via properties and function calls (check the uobject call() method)

Most-used methods are implemented directly as uobject methods for performance reasons.

You can get the the list of uobject api methods here: https://github.com/20tab/UnrealEnginePython/blob/master/docs/uobject_API.md

Automatic module reloading (Editor only)
----------------------------------------

When in the editor, you can change the code of your modules mapped to proxies without restarting the project. The editor will reload the module every time a PyActor, PyPawn or PythonComponent is instantiated. This is obviously not the best approach. In the future we would like to implement timestamp monitoring on the file to reload only when needed.

Primitives and Math functions
-----------------------------

The plugin exposes FVector, FRotator, FQuat, FColor, FHitResult and a bunch of the internal handles.

Where meaningful, math operations are exposed:


```py
import unreal_engine

class ActorGoingUp:
    def begin_play(self):
        # 1 meter by second
        self.speed = 100
    
    def tick(self, delta_time):
        # get the up vector
        up = self.uobject.get_up_vector()
        # get current position
        position = self.uobject.get_actor_location()
        # build a direction vector based on speed
        up_amount = up * self.speed * delta_time)
        # sum the direction to the position
        position += up_amount
        # set the new position
        self.uobject.set_actor_location(new_position)
```

Referencing objects
-------------------

You can use find_class(), find_struct() and find_object() functions to reference already loaded classes/objects.

If you need to reference assets (still) not loaded in the engine you can use load_struct(), load_class() or load_object():

```py
a_struct_data = ue.load_struct('/Game/Data')
ue.log(a_struct_data.as_dict())
```

or to find a specific asset:

```py
texture_class = ue.find_class('Texture2D')
a_specific_texture = ue.load_object(texture_class, '/Game/Textures/logo2')
```

More infos about dealing with assets are available here: https://github.com/20tab/UnrealEnginePython/blob/master/docs/ManagingAssets.md

The as_dict() method
--------------------

This special method can be called on any uobject: it will attempt to serialize it to a python dictionary



Blueprints integration
----------------------

You can call blueprints functions (or custom events) via the .call() and .call_function() methods:

```py
your_funny_blueprint_object.call('AFunctionOrACustomEvent with_a_arg')
```

Whenever you need to reference external object, avoid using find_object() and similar. Instead add a public variable in your blueprint
pointing to the specific object. You can then reference this object easily getting the property value:

```py
the_other_object = self.uobject.get_property('target')
the_other_object.set_actor_location(0, 0, 0)
```

.call_function() is more advanced, as it allows for return values and python args:

```py
# an example of moving an object z with curves:
class Curver:
    def begin_play(self):
        self.curve = self.uobject.get_owner().get_property('curve')
        self.accumulator = 0.0
    def tick(self, delta_time):
        location = self.uobject.get_actor_location()
        z = self.curve.call_function('GetFloatValue', self.accumulator) * 100
        self.uobject.set_actor_location(location.x, location.y, z)
        self.accumulator += delta_time

```

Events
------

You can easily bind events (as seen before) with the bind_event function

```py
self.uobject.bind_event('OnActorBeginOverlap', a_funny_callback)
```

You can obviously bind to Event Dispatchers too.

Triggering events is basically like calling functions, self.uobject.call('OnActorBeginOverlap') will be more than enough.

If you want to map events from a blueprint to a python function, the best thing to do is using the 'python call' blueprint functions exposed by the various plugin classes:

![Alt text](screenshots/unreal_screenshot3.png?raw=true "Screenshot 3")

Plugin Configuration
--------------------

You can tune your python environment adding a [Python] stanza to the Config/DefaultEngine.ini file.

The following parameters are supported:

* `Home`: set the path of the python installation, useful for forcing the python vm to search modules in a specific directory (like old-style virtualenvs)
* `RelativeHome`: like Home but relative to the /Content directory
* `ProgramName`: set the python program name path
* `RelativeProgramName`: like ProgramName, but the path is relative to the /Content directory
* `ScriptsPath`: change the default path on where Unreal Engine searches for python scripts
* `RelativeScriptsPath`: like ScriptsPath, but relative to the /Content directory
* `AdditionalModulesPath`: add the specified directory to sys.path
* `RelativeAdditionalModulesPath`: like AdditionalModulesPath, but the path is relative to the /Content directory
* `ZipPath`: allow to specify a .zip file that is added to sys.path
* `RelativeZipPath`: like ZipPath, but the path is relative to the /Content directory
* `ImportModules: comma/space/semicolon separated list of modules to import on startup (after ue_site)

Example:

```ini
[Python]
Home = C:/FooBar/Python36
```

Packaging
---------

When you package your projects, remember to include the libpython (dll or dylib or .so based on your operating system) in the binaries folder and the Scripts directory (if you do not want to force the user to have python installed in its system). For Windows system you can use the embedded distributions available in the official python.org site. Just uncompress the zip in the plugin binary folder (at the same level of UnrealEnginePython.dll)

If you do not want to distribute python sources, you can include only the ```__pycache__``` directory with the bytecode.

Do not forget to include python third party modules (if you use any of them in your project)

Quick Examples
--------------

This is a PyActor destroying itself whenever another actor overlap it. Remember to add a mesh component to it (like a sphere) and set its collision behaviour as 'OverlapAll'. This could be tested with the third person official template.

```py
class Ball:
    def begin_play(self):
        ue.print_string('Hello')

    def on_actor_begin_overlap(self, other_actor):
        ue.print_string('Collided with ' + other_actor.get_name())
        self.uobject.actor_destroy()
```

Now we create (at runtime !!!) a whole new PyActor:

```python
class SuperHero:
    def begin_play(self):
        # spawn a new PyActor
        new_actor = self.uobject.actor_spawn(ue.find_class('PyActor'), Fvector(0, 0, 0),FRotator(0, 0, 90))
        # add a sphere component as the root one
        static_mesh = new_actor.add_actor_root_component(ue.find_class('StaticMeshComponent'), 'SphereMesh')
        # set the mesh as the Sphere asset
        static_mesh.call('SetStaticMesh /Engine/EngineMeshes/Sphere.Sphere')
        # set the python module
        new_actor.set_property('PythonModule', 'gameclasses')
        # set the python class
        new_actor.set_property('PythonClass', 'Vertical')
```

For more examples: https://github.com/20tab/UnrealEnginePython/tree/master/examples

Spawning Notes
--------------

Remember that only Actors can be spawned in a world, and that even the editor is a valid world:

```python
import unreal_engine as ue
from unreal_engine.classes import Actor, Character
from unreal_engine import FVector, FRotator

world = ue.get_editor_world()
actor000 = world.actor_spawn(Actor, FVector(0, 0, 0), FRotator(0, 0, 0))
character000 = world.actor_spawn(Character, FVector(100, 100, 100), FRotator(0, 0, 0))
```

Remember that the Blueprint asset is not a valid actor by itself, you need to get the class generated by the blueprint:

```python
import unreal_engine as ue
from unreal_engine.classes import Blueprint
from unreal_engine import FVector, FRotator

world = ue.get_editor_world()

blueprint = ue.load_object(Blueprint, '/Game/TestBall.TestBall')
actor000 = world.actor_spawn(blueprint.GeneratedClass, FVector(0, 0, 0), FRotator(0, 0, 0))
```

otherwise you can directly reference the BlueprintGeneratedClass

```python
import unreal_engine as ue
from unreal_engine.classes import BlueprintGeneratedClass
from unreal_engine import FVector, FRotator

world = ue.get_editor_world()

blueprint_actor = ue.load_object(BlueprintGeneratedClass, '/Game/TestBall.TestBall_C')
actor000 = world.actor_spawn(blueprint_actor, FVector(0, 0, 0), FRotator(0, 0, 0))
```

The Python Editor
-----------------

Starting from version 20170301 a handy editor has been added to the plugin:

![Alt text](screenshots/python_editor_screenshot001.png?raw=true "Python Editor Screenshot")

It allows you to run, create, modify and delete scripts directly from the UE editor

The first pull request for the editor has been issued by https://github.com/sun5471 so many thanks to him ;)

Integration with Qt4/Qt5/PySide2
--------------------------------

Thanks to solid GIL management, you can integrate Qt python apps in Unreal Engine 4.

Pay attention to not call app.exec_() as it will result in Qt taking control of the UE loop. Instead use a ticker to integrate the Qt loop in the editor loop:

```python

# save it as ueqt.py
import sys
import unreal_engine as ue
import PySide2
from PySide2 import QtWidgets

app = QtWidgets.QApplication(sys.argv)

def ticker_loop(delta_time):
    app.processEvents()
    return True

ticker = ue.add_ticker(ticker_loop)
```
now you can start writing your gui (this is a simple example loading asset thumbnail):

```python
import ueqt
from PySide2 import QtCore, QtWidgets, QtGui
import unreal_engine as ue

from unreal_engine import FARFilter

_filter = FARFilter()
_filter.class_names = ['SkeletalMesh', 'Material']

class MyWidget(QtWidgets.QWidget):
    def __init__(self):
        super().__init__()
        self.vertical = QtWidgets.QVBoxLayout()
        self.scroll = QtWidgets.QScrollArea()
        self.content = QtWidgets.QWidget()
        self.scroll.setWidget(self.content)
        self.scroll.setWidgetResizable(True)
        self.layout = QtWidgets.QVBoxLayout()
	
        for asset_data in ue.get_assets_by_filter(_filter, True):
            try:
                thumbnail = asset_data.get_thumbnail()
            except:
                continue

            label = QtWidgets.QLabel()
            data = thumbnail.get_uncompressed_image_data()
            image = QtGui.QImage(data, 256, 256, QtGui.QImage.Format_RGB32)
            label.setPixmap(QtGui.QPixmap.fromImage(image).scaled(256, 256))
            self.layout.addWidget(label)

        self.content.setLayout(self.layout)
        self.vertical.addWidget(self.scroll)
        self.setLayout(self.vertical)



widget = MyWidget()
widget.resize(800, 600)
widget.show()

root_window = ue.get_editor_window()
root_window.set_as_owner(widget.winId())
```

(no need to allocate a new Qt app, or start it, as the UE4 Editor, thanks to to ueqt module is now the Qt app itself)

Note the 2 final lines: they 'attach' the Qt window as a 'child' of the editor root window. Note that on windows platform this is not simple parenting but 'ownership'.

Memory management
-----------------

Dealing with 2 different GC's is really challenging.

Starting from release 20180226 a new memory management system has been added (FUnrealEnginePythonHouseKeeper, available here https://github.com/20tab/UnrealEnginePython/blob/master/Source/UnrealEnginePython/Public/PythonHouseKeeper.h). This new system is completely integrated with the Unreal Engine reflection-based GC and will hold track of each ue_PyUObject abd the related UObject to understand when a python object can be safely destroyed.

The same system works for delegates, as well as Slate.

More details here: https://github.com/20tab/UnrealEnginePython/blob/master/docs/MemoryManagement.md

Unit Testing
------------

The repository includes the tests/ directory from which unit tests will be run.

To run the unit tests (ensure to run them on an empty/useless project to avoid messing with assets) run the following commands from the ue4 python console:

```python
import unreal_engine as ue
ue.py_exec(ue.find_plugin('UnrealEnginePython').get_base_dir() + '/run_tests.py')
```
if you plan to add new features to the plugin, including a test suite in your pull request will be really appreciated ;)

Threading
------------------------

Since release 20180624 threading is fully supported.

As with native threads, do not modify (included deletion) UObjects from non-main threads.

Accessing Python Proxy From UObject
-----------------------------------

Sometimes you may have a UObject and know that it is backed by a python object. To get the python object from the UObject, use the `get_py_proxy` method. For example, imagine you have the following situation:

   1. There is a `PyActor` sub-class called `PyExplosiveActor` which has `Explosive` as its python class.
   2. The `Explosive` has a `go_boom` python method.
   3. There is a `PyActor` sub-class called `PyBadGuyActor` which has a Blueprint property called `MyBomb` and a python class called `BadGuy`.
   4. The `BadGuy` instance in python knows that its UObject has its `MyBomb` as an instance of `PyExplosiveActor` and wants to call the `go_boom` python method.
   
This would be resolved as shown below:

```python
import unreal_engine as ue

class Explosive:
    'Python representation for PyExplosiveActor in UE4'

    def go_boom(self):
        # do python stuff to explode
        ...
        self.uobject.destory()

class BadGuy:
    'Python reprsentation for PyBadGuyActor in UE4'
   
    def ignite_bomb(self, delay):
        bomb = self.uobject.MyBomb
        py_bomb = bomb.get_py_proxy()
        py_bomb.go_boom()
	
```

What is going on here in `BadGuy` is that self.uobject is a reference to the PyActor UObject and `self.uobject.MyBomb` is a reference to the `PyExplosive` uobject. But instead you want to access its proxy class (`Explosive`). The `get_py_proxy()` method returns the python custom class, `Explosive` that the `PyExplosiveActor` object is mapped to.

Status and Known issues
-----------------------

Exposing the full ue4 api is a huge amount of work, feel free to make pull requests for your specific needs.

We still do not have a plugin icon ;)

We try to do our best to "protect" the user, but you can effectively crash UE from python as you are effectively calling the C/C++ api

Contacts and Commercial Support
-------------------------------

If you need commercial support for UnrealEnginePython just drop a mail to info at 20tab.com

Follow @unbit on twitter for news about the project

Special Thanks
--------------

Such a big project requires constant sponsorship, special thanks go to:

* Kite & Lighting http://kiteandlightning.la/ (they are sponsoring various areas of the project, expecially the slate api)

* GoodTH.INC https://www.goodthinc.com/ (they are sponsoring the sequencer api)

* Quixel AB https://megascans.se/ (built their integration tool over UnrealEnginePython giving us tons of useful feedbacks and ideas)
