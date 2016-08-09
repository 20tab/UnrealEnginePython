# UnrealEnginePython
Embed Python in Unreal Engine 4

# How and Why ?

This is a plugin allowing the mapping of Actors, Pawns and Components to python classes (and a lot more). It is not meant as a way to
avoid blueprints or c++ but as a good companion to them (albeit reducing the amount of c++ required for coding a game could be an interesting thing ;)

Another funny feature is that you can change your python code even after the project has been packaged. You can potentially build a completely new game from an already packaged one.

Mid term goal is allowing scripting of the editor too.

The plugin embeds the python3 library and automatically adds an actor class (PyActor), a pawn class (PyPawn) and a component class (PythonComponent).
They are available in your editor once you load the plugin.

All of the exposed engine features are under the 'unreal_engine' virtual module (it is completely coded in c into the plugin, so do not expect to run 'import unreal_engine' from a standard python shell)

# Installation on Windows (64 bit)

The installation is pretty long (and boring) as you do not want the final users of the product to be required to install python, so we need to use the 'embedded python distribution' (available for windows in the official python site). Unfortunately the embedded distribution does not contain the python development headers so we need the official system-wide installation too.

* install Unreal Engine 4.12 and Python 3.5 as a system user
* run the unreal editor and create a new blank c++ project (NOT a blueprint one, otherwise visual studio will not be initialized)
* once the project is setup, close both unreal and visual studio
* move to the project directory (you can right click the project from the epic launcher and choose 'show in explorer')
* create a 'Plugins' directory into the project directory
* move into the just created Plugins directory and clone the repository:

```sh
git clone https://github.com/20tab/UnrealEnginePython
```

* from the explorer right click on the project main file and choose 'generate visual studio project files'
* open again visual studio, you should now see Plugins/UnrealEnginePython in your solution explorer
* before running the plugin build process you need to copy development headers and libs in the plugin directory (Plugins/UnrealEnginePython).
* create the directory Plugins/UnrealEnginePython/python35 (this is where the build script expects to find headers and static libs)
* copy "C:/Program Files/Python35/include" and "C:/Program Files/Python35/libs" into Plugins/UnrealEnginePython/python35
* run the compilation from visual studio
* once the compilation ends, copy the python35.dll (from "C:/Program Files/Python35" or from the embeded distribution) into the Binaries/Win64 directory of your project (you will find the plugin dll too in this directory)
* now you can re-run the unreal engine editor

If all goes well, open the output log and search for the string "Python". You should see the Python VM initialization message. It means your editor has now full python support.

# Installation on MacOSX

On the Mac the installation is easier, as the final user is currently forced to install python on its system (there are obviously dozens of workarounds but at this stage of the project we prefer focusing on the api).

* install the latest official python distribution from python.org (the installation will end in the "/Library/Frameworks/Python.framework/Versions/3.5" directory).
* create a new unreal engine blank c++ project (NOT a blueprint one, otherwise XCode will not be initialized)
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

    # this is called on game start
    def begin_play(self):
        ue.log('Begin Play on Hero class')
        
    # this is called at every 'tick'    
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

By default a 'begin_play' and a 'tick' method are expected (NOTE: currently you get an exception if you do not define them, this should be addressed soon). The following methods are defined for PyActor (they are triggered on specific events):

```py
def on_actor_begin_overlap(self, other_actor):
    pass

def on_actor_end_overlap(self, other_actor):
    pass
    
# *args is currently under investigation
def on_actor_hit(self, other, *args):
    pass
    
# maybe touched_actor is useless ?
def on_actor_clicked(self, touched_actor, button_pressed)
    pass
```

more events are going to be added

What is 'self.uobject' ?
------------------------

To allow seamless Python integration, each UObject of the engine is automatically mapped to a special Python Object (ue_PyUObject).

Whenever you want to access a UObject from python, you effectively get a reference to a ue_PyUObject exposing (via its methods) the features of the UObject (properties, functions, ....)

This special python object is cached into the UObject (via the __PyObject property) so you do not need to recreate it every time (and more important you can hold state informations into this python object)

To be more clear, a call to:

```py
text_render_component = unreal_engine.find_class('TextRenderComponent')
```

will internally search for the 'TextRenderComponent' class (via unreal c++ reflection) and when found will check if it exposes a '__PyObject' property. If available, it will return its value to the Python VM, otherwise a new one will be created and mapped to it.

From the previous example the 'text_render_component' maintains a mapping to the UObject (well a UClass in this example).

Pay attention: the python class you map to the PyActor class, is not a ue_PyUObject (it is a classic python class) but holds a reference (via the 'uobject' field) to a ue_PyUObject mapped to the AActor: PyActor exposes 'uobject' field mapped to the c++ UObject.

Note about 'uobject' from now on
---------------------------------

In the following lines, whenever you find a reference to 'uobject' it is meant as a ue_PyUObject object.

Adding a python component to an Actor
-------------------------------------

This works in the same way as the PyActor class, but it is, well, a component. You can attach it (search for the 'Python' component) to any actor.

Remember that for components, the self.uobject field point to the component itself, not the actor.

To access the actor you can use

```py
actor = self.uobject.get_class()
```

PyPawn
------

This works like PyActor, but this time you generate a new Pawn class (that you can posses with a controller)

Pythonizing a GameMode
-----------------------

You can attach a PythonComponent to a GameMode blueprint. Its tick method will be executed during the whole game lifetime.

The unreal_engine module
------------------------

This is a generic module, exposing functions not related to a particular 'World' (see below).

You can use these functions from an actor, a pawn, a component or directly from a module.

Just remember to import it:

```py
import unreal_engine
```

or (as an example)

```py
import unreal_engine as ue
```


---
```py
uclass = unreal_engine.find_class('name')
```

This function searches for a class in the engine with the specified name and returns its 'uobject' (a c++ UClass object internally).
You can use this uobject to search for objects of that type or to spawn new actors (and lot of more things)

---
```py
uclass = unreal_engine.find_object('name')
```

This is a more generic (and slower) variant of find_class that searches for every UObject. You can use it (for example) for getting references to assets (like materials, meshes...)

---
```py
unreal_engine.log('message')
```

log a string into the editor output log (under the LogPython category/class)


---
```py
unreal_engine.log_warning('message')
```

log a warning string (yellow) into the editor output log (under the LogPython category/class)


---
```py
unreal_engine.log_error('message')
```

log an error string (red) into the editor output log (under the LogPython category/class)


---
```py
unreal_engine.add_on_screen_debug_message(key, timeout, 'message')
```

low-level equivalent of blueprint 'print string' function. It disappear after 'timeout' seconds and can get a numeric key (use -1 for disabling keys feature)

TODO: support for colors


---
```py
unreal_engine.print_string('message')
```

python equivalent of the blueprint 'print string' function. It disappears after 2 seconds and it is wrote in cyan color.


---
```py
unreal_engine.vector_add_vector(x, y, z[, x1, y1, z1, ...])
```

optimized vector math function, allows adding 3 dimensions vectors exposed as 3 float values


---
```py
unreal_engine.vector_add_float(x, y, z[, d1, d2, ...])
```

optimized vector math function, allows adding a 3 dimensions vector with one or more floats


---
```py
unreal_engine.vector_mul_vector(x, y, z[, x1, y1, z1, ...])
```

optimized vector math function, allows multiplyging 3 dimensions vectors exposed as 3 float values


---
```py
unreal_engine.vector_mul_float(x, y, z[, d1, d2, ...])
```

optimized vector math function, allows multiplying a 3 dimensions vector with one or more floats

---
```py
editor = unreal_engine.get_editor_world()
```

(available only into the editor) it allows to get a reference to the editor world. This will allow in the near future to generate UObjects directly in the editor (for automating tasks or scripting the editor itself)


The 'World' concept
-------------------

Every uobject is mapped to a world (UWorld in c++). Generally when you play on a Level your objects all live in the same world, but at the same time there could be multiple worlds (for example while testing in the editor there is a world for the editor and one for the simulation)

While it is pretty rare to reference other worlds, you may need to compare the world of two uobject's (for example you may have a reference in your python module to a uobject of a hidden world and you want to check if you need to use it).

The uobject.get_world() function returns a uobject representing the world (the C++ UWorld class)

The uobject api
---------------

Each uobject represent a UObject class of the Engine. This C++ class is basically the root of all the other classes (Actors, Pawns, components, properties ...). Thanks to Unreal Engine reflection system we do not need to implement a python class for each unreal engine class, but for performance reason we expose the most common methods. The uobject system checks for the type of the mapped C++ UObject and will call the method only if it is safe to call it.

Sometime methods are implemented for automatically getting the right object. As an example get_actor_location() when called over a component will automatically retrieve the related actor and will call C++ AActor::GetActorLocation() method over it.

When this automagic approach is too risky, the method will check for the uobject type and will raise an exception in the case of inconsistencies.

Remember, there is no need to implement every single engine class method, the reflection system is powerful enough to be governed only via properties and function calls (check the uobject call() method)

Most-used methods are implemented directly as uobject methods for performance reasons.

---
```py
x, y, z = uobject.get_actor_location()
```

get the current actor location (automatically retrieve the actor from the component if needed)

---
```py
uobject.set_actor_location(x, y, z)
```

set the current actor location (automatically retrieve the actor from the component if needed)

---
```py
pitch, yaw, roll = uobject.get_actor_rotation()
```

get the current actor rotation (automatically retrieve the actor from the component if needed)


---
```py
uobject.set_actor_rotation(pitch, yaw, roll)
```
set the current actor rotation (automatically retrieve the actor from the component if needed)

---
```py
x, y, z = uobject.get_actor_forward()
```

get the current actor forward vector (automatically retrieve the actor from the component if needed)

---
```py
x, y, z = uobject.get_actor_right()
```

get the current actor right vector (automatically retrieve the actor from the component if needed)

---
```py
x, y, z = uobject.get_actor_up()
```

get the current actor up vector (automatically retrieve the actor from the component if needed)

---
```py
x, y, z = uobject.get_actor_velocity()
```
get the current actor velocity vector (automatically retrieve the actor from the component if needed)


---
```py
value = uobject.get_property('name')
```

get the property value of a uobject

NOTE: currently only floats and objects values are supported


---
```py
uobject.set_property('name', value)
```

set the property of a uobject

NOTE: currently only floats and strings values are supported

---
```py
properties_list = uobject.properties()
```

get the list of properties available for the uobject (as a list of string names)

---
```py
uobject.call('function arg0 arg1 argN....')
```

this is probably the most important functionality of the Unreal Engine reflection system.

It allows you to call a method/function exposed by a uobject passing it arguments.

For example you have an uobject mapped to a UTextRenderComponent and you want to change the text:

```py
text_render_component.call('SetText Hello')
```

will call the UTextRenderComponent::SetText(FString value) method passing 'Hello' as the string argument

This methods allows you to interface with basically every engine features, but is is obviously slower than directly calling C++ methods.

Use it for calling blueprint functions, or while waiting for the addition of new specific-methods in the uobject api ;)

---
```py
actor = uobject.get_owner()
```

get the owner of an object (generally called by a component to get its actor)

---
```py
world = uobject.get_world()
```

get the world containing an object. (currently useful only for testing and comparison)

---
```py
name = uobject.get_name()
```
get the name of the uobject

---
```py
full_name = uobject.get_full_name()
```

get the full name of the uobject

---
```py
# really slow !
found_uobject = uobject.find_object('name')
```

find an object (by name) in the same world of the caller (SLOW, do not use it unless you know what you are doing)

---
```py
# really really slow !
found_uobjects = uobject.all_objects()
```

get the list of all uobjects in the same world of the caller (REALLY SLOW,  do not use it unless you know what you are doing)

---
```py
found_actors = uobject.all_actors()
```

get the list of all actors available in the same world of the caller. A bit slow.

---
```py
uclass = uobject.get_class()
```

this is the same as calling ->GetClass() in c++. You can get the UClass of a uobject.

---
```py
uclass = uobject.actor_spawn(uclass[, x, y, z, pitch, yaw, roll])
```

spawn an actor. uclass is the reference you can get via get_class() or unreal_engine.find_class()

---
```py
uobject.actor_destroy()
```

destroy an actor

---
```py
yesno = uobject.actor_has_component_of_type(uclass)
```

return True if the actor has a component of the specified type

---
```py
yesno = uobject.get_actor_component_by_type(uclass)
```

return the first component (of an actor) of the specified type

---
```py
components = uobject.actor_components()
```

get the list of all components mapped to the actor

---
```py
uobject.enable_input()
```

enable the input system on an object

---
```py
uobject.bind_input_axis('axis')
```

---
```py
uobject.quit_game()
```

well, quit the game :)

---
```py
yesno = uobject.is_input_key_down('key')
```

---
```py
value = uobject.get_input_axis('axis')
```

---
```py
yesno = uobject.actor_has_tag('tagname')
```

check if an actor is tagged with the specific tag

---
```py
x, y, z, ex, ey, ez = uobject.get_actor_bounds()
```

get the bounds of an object (ex, ey and ez are the extents)

---
```py
hit_object, x, y, z, nx, ny, nz = uobject.line_trace_single_by_channel(x0, y1, z0, x1, y1, z1, channel)
```

---
```py
[(hit_object, x, y, z, nx, ny, nz), ...] = uobject.line_trace_multi_by_channel(x0, y1, z0, x1, y1, z1, channel)
```

---
```py
uobject.show_mouse_cursor()
```

---
```py
uobject.enable_click_events()
```

---
```py
uobject.enable_mouse_over_events()
```

---
```py
uobject.destructible_apply_damage(damage, impulse, dx, dy, dz, ix, iy, iz)
```

See Fracturing below

---
```py
uobject.set_view_target(target)
```

change the view target. The 'target' argument should be an actor with a camera component.

---
```py
uobject.set_simulate_physics()
```

---
```py
new_component = uobject.add_actor_component(uclass, 'name')
```

add a new component of the specified uclass (type) and set its name

---
```py
new_component = uobject.add_actor_root_component(uclass, 'name')
```

add a new component as the root one of the specified uclass (type) and set its name

---
```py
uobject.simple_move_to_location(x, y, z)
```

move to a location using navmesh (see Navigation below)


Automatic module reloading (Editor only)
----------------------------------------

When in the editor, you can change the code of your modules without restarting the project. The editor will reload the module every time a PyActor, PyPawn or PyComponent is instantiated. This is obviously not the best approach. In the future we would like to implement timestamp monitoring on the file to reload only when needed.

Math functions
--------------

The plugin does not expose FVector, FRotator or FQuat. You have optimized shortcuts in the unreal_engine module, but you can use any python math library if you want (included numpy)


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
        up_amount = unreal_engine.vector_mul_float(*up, self.speed, delta_time)
        # sum the direction to the position
        new_position = unreal_engine.vector_add_vector(position, up_amount)
        # set the new position
        self.uobject.set_actor_location(*new_position)
```

Navigation
---------

The only exposed navigation-related method is 'simple_move_to_location'. It expects a Pawn with a movement component (like a character)

```py
class MoveToTargetComponent:

    def begin_play(self):
        # get a 'target point' reference from a pawn public property
        target = self.uobject.get_owner().get_property('target')
        self.uobject.get_owner().simple_move_to_location(*target.get_actor_location())
        
    def tick(self, delta_time):
        pass
```

Physics
-------

The 'set_simulate_physics' method is exposed for enabling physic on PrimitiveComponent.

Remember that you cannot enable physics withotu setting the collision presetes accordingly:

```py

# PyActor with a staticmeshcomponent (a sphere)
# when overlapped it became a physic object
class Ball:

    def begin_play(self):
        self.sphere = self.uobject.get_actor_component_by_type(ue.find_class('StaticMeshComponent'))
        
    def tick(self, delta_time):
        pass
    
    def on_actor_begin_overlap(self, other_actor):
        # change collision profile
        self.sphere.call('SetCollisionProfileName BlockAll')
        # enable physics
        self.sphere.set_simulate_physics()
        
    # once the object became a physics one, hit event will start arriving
    def on_actor_hit(self, other, *args):
        ue.print_string('HIT with ' + other.get_name())
```

TODO: expose more physics functions, expecially the one for applying forces

Fracturing
----------

Fracturing is one of the best features you get for free in unreal engine.

You can apply damage to destructible objects directly from python (more kind of damages will be added soon)

```py
class DestroyMeComponent:

    def begin_play(self):
        # get a reference to a destructible component in the actor
        self.destructible = self.uobject.get_actor_component_by_type(ue.find_class('DestructibleComponent'))
        
    def tick(self, delta_time):
        pass
        
    def explode(self):
        # damage amount
        amount = 1000
        strength = 20000
        position = self.uobject.get_owner().get_actor_location()
        up = self.uobject.get_owner().get_actor_up()
        self.destructible.destructible_apply_damage(amount, strength, *position, *up)
    
```

you can now call the 'explode' method via blueprints using the 'Call Python Component Method' node

Blueprints integration
----------------------

You can call blueprints functions (or custom events) via the .call() method:

```py
your_funny_blueprint_object.call('AFunctionOrACustomEvent with_a_arg')
```

Whenever you need to reference external object, avoid using find_object() and similar. Instead add a public variable in your blueprint
pointing to the specific object. You can then reference this object easily getting the property value:

```py
the_other_object = self.uobject.get_property('target')
the_other_object.set_actor_location(0, 0, 0)
```

Events
------

Currently there is no support for defining custom events from python. The best thing to do is defining them from a blueprint and mapping each event to a python method:

![Alt text](screenshots/unreal_screenshot3.png?raw=true "Screenshot 3")

Packaging
---------

When you package your projects, remember to include the libpython (dll or dylib) in the binaries folder and the Scripts directory.

If you do not want to distribute pythn sources, you can include only the ```__pycache__``` directory with the bytecode.

Do not forget to include python third party modules (if you use any of them in your project)

Examples
--------

This is a PyActor destroying itself whenever another actor overlap it. Remember to add a mesh component to it (like a sphere) and set its collision behaviour as 'OverlapAll'. This could be tested with the third person official template.

```py
class Ball:
    def begin_play(self):
        ue.print_string('Hello')
    def tick(self, delta_time):
        pass
    def on_actor_begin_overlap(self, other_actor):
        ue.print_string('Collided with ' + other_actor.get_name())
        self.uobject.actor_destroy()
```

Now we create (at runtime !!!) a whole new PyActor:

```py
class SuperHero:
    def begin_play(self):
        # spawn a new PyActor
        new_actor = self.uobject.actor_spawn(ue.find_class('PyActor'), 0, 0, 0, 0, 0, 90)
        # add a sphere component as the root one
        static_mesh = new_actor.add_actor_root_component(ue.find_class('StaticMeshComponent'), 'SphereMesh')
        # set the mesh as the Sphere asset
        static_mesh.call('SetStaticMesh /Engine/EngineMeshes/Sphere.Sphere')
        # set the python module
        new_actor.set_property('PythonModule', 'gameclasses')
        # set the python class
        new_actor.set_property('PythonClass', 'Vertical')
        
    def tick(self, delta_time):
        pass
```

Memory management
-----------------

Dealing with 2 different GC's is really challenging.

PyActor, PyPawn and PythonComponent automatically DECREF's the mapped classes. This should be enough unless you hold references
in the python modules themselves. As this would be a bad programming practice, the current approach should be safe enough.

In addition to this, every time a uobject has to return its UObject mapping, it checks for its validity and safety:

```c
#define ue_py_check(py_u) if (!py_u->ue_object || !py_u->ue_object->IsValidLowLevel() || py_u->ue_object->IsPendingKillOrUnreachable())\
							return PyErr_Format(PyExc_Exception, "PyUObject is in invalid state")
```

Status and Known issues
-----------------------

The project could be considered in beta state.

Code should be organized way better as currently all of the uobject api is in a single huge file.

The property system is incomplete.

We would like to find a way to define and manage custom events directly from python

.py files are not recognized by the editor. This should be fixed soon

We still do not have a plugin icon ;)

The build system is not very robust. Maybe linking the python static library into the plugin dll could be a better approach.

Investigate what to do with threads (maybe disallowing unreal engine calls in python threads should be enough)

We need to find a way for automatic testing :)

Contacts
--------

If you want to contact us, drop a mail to info at 20tab.com
