# Managing Memory

## UObject

A UObject is one of the fundamental parts of Unreal Engine. It represents an engine/editor object tracked by Unreal Engine Garbage Collector.

Each UObject is constantly tracked, and whenever the Garbage Collector runs (generally every 60 seconds or in very specific parts of the engine/editor loop, like when entering PIE mode)
it checks if the UObject has still references to other UObjects, and in negative case (no references) it will be destroyed.

You can trigger a Garbage Collector run from python with:

```python
import unreal_engine as ue
ue.console_exec('obj gc')
```

## py_UEObject

this is the low-level C struct representing the python mapping between a PyObject (c struct representing a python object) and a UObject.
Whenever you create a py_UEObject (from the UE python api) another GC (related to the python plugin) will start tracking it.

Whenever the UE GC runs, the UnrealEnginePython GC will run too, checking if a UObject mapped to a py_UEObject is still alive.

If the UObject mapped to a python object is dead, an exception will be triggered.

This is an example:

```python
import unreal_engine as ue

from unreal_engine.classes import BlueprintFactory

factory = BlueprintFactory()
texture = ue.create_transient_texture(512, 512)
# run GC
ue.console_exec('obj gc')

print(factory)
# this will raise an exception as the UObject mapped to factory has been destroyed by the GC ru
print(texture)
```

By running this script you will end with something like this:

```
PyUObject is in invalid state
Traceback (most recent call last):
  File "<string>", line XX, in <module>
Exception: PyUObject is in invalid state
```

Here we are seeing two different behaviours between factory and texture. The first one survived the GC run, the second has been destroyed.

This is because only UObject's created explicitely by Python with a classic constructor (like BlueprintFactory()) are bound to the related ue_PyUObject. All of the others obey the Unreal GC rules. This is a pretty complex choice aimed at improving performance and avoiding too much competition between the two GCs.

Very long scripts, that do lot of stuff, often triggering UE4 GC, could be blocked in the middle of their execution by this kind of errors. In such a case (like you would do in C++) you need to inform the UE GC on how to deal with them (for avoiding their destruction).

Pay attention, as once you tell the UE GC to not destroy a UObject, that UObject (and its python mapping) will stay in memory (so you will end with a leak)

## Strategy 1: Setting UObject flags to govern the GC

When you create a UObject (from the C++ side, via the ```NewObject<T>``` api call) you can specify a bitmask of flags. By default the python api only use the RF_Public flag:
  
https://api.unrealengine.com/INT/API/Runtime/CoreUObject/UObject/EObjectFlags/index.html

You can change this bitmask with the set_obj_flags() python function:

```python
import unreal_engine as ue

texture = ue.create_transient_texture(512, 512)
# assign mask 0x00000001|0x00000002
texture.set_obj_flags(ue.RF_PUBLIC|ue.RF_STANDALONE)
# run GC
ue.console_exec('obj gc')
# this will normally print the UObject repr
print(texture)
```

The RF_Standalone flag (RF_STANDALONE in python api) will marks a UObject as 'standalone' so it will remain resident in memory forever.

Eventually you can reset/set the flags:

```python
import unreal_engine as ue

texture = ue.create_transient_texture(512, 512)
# assign mask 0x00000001|0x00000002
texture.set_obj_flags(ue.RF_PUBLIC|ue.RF_STANDALONE)

ue.console_exec('obj gc')

print(texture)

# the second True argument will reset the flags (otherwise set_obj_flags will work in append mode)
# eventually you can call factory.reset_obj_flags()
texture.set_obj_flags(ue.RF_PUBLIC, True)

ue.console_exec('obj gc')

print(texture)
```

The second print will raise the error.

This is a pretty raw approach (unless you are sure that you need a resident object). For having more control the second strategy will be way more better...

## Strategy 2: The Root Set

The root set is a very specific part of the GC tree. If you want to hold control of a UObject lifecycle in an efficient way, you can use the related python api:

```python
import unreal_engine as ue

texture = ue.create_transient_texture(512, 512)
texture.add_to_root()

ue.console_exec('obj gc')

print(texture)

texture.remove_from_root()

ue.console_exec('obj gc')

print(texture)
```

as before, the first GC run will not destroy the UObject (as it is in the root set), while the second one will remove if from the memory as it is no more in the root set.

A funny approach to memory management of UObject from python is by using a Tracker object:

```python
class Tracker:

    def __init__(self):
        self.uobjects = []

    def track(self, uobject):
         uobject.add_to_root()
         self.uobjects.append(uobject)
         return uobject

    def __del__(self):
         for uobject in self.uobjects:
             uobject.remove_from_root()

tracker = Tracker()
```

Now you can create UObject from python and track them automatically. When the python GC destroys the tracker object, all of the UObject's tracked by it will be destroyed too:

```python
texture = tracker.track(ue.create_transient_texture(512, 512))
```

As an example when running a script multiple times, the 'tracker' id will be overwritten, triggering the destruction of the mapped python object (and its ```__del__``` method)

## Low-level UObject creation api

Til now you have seen how to create new UObject's in a very pythonic way:

```python
from unreal_engine.classes import BlueprintFactory, Material, MaterialFactoryNew

bp_factory = BlueprintFactory()
material = Material()
# the first argument here (None) is the outer UObject
material_with_a_name = Material(None, 'FooBar001')
mat_factory = MaterialFactoryNew()
```

While the automagic python UObject creation api is really handy, sometime you want lower-level access to the ```NewObject<T>``` C++ api:
  
```python
import unreal_engine as ue
from unreal_engine.classes import Material

material = ue.new_object(Material, None, 'DumbMaterial001', ue.RF_PUBLIC|ue.RF_STANDALONE)
```

or for more dynamic class specification:

```python
import unreal_engine as ue

# you can reference to Unreal classes with a string
material = ue.new_object(ue.find_class('Material'), None, 'DumbMaterial001', ue.RF_PUBLIC|ue.RF_STANDALONE)
```

## Owning

We have seen how a UObject is differently managed based on the way it has been created:

```python
# owned by python
material = Material()


# owned by unreal
material2 = ue.new_object(Material)
```

The interesting thing is that we are allowed to change the owner using the .own() and .disown() methods:

```python
# owned by unreal
material2 = ue.new_object(Material)

# now owned by python
material2.own()

# owned again by unreal

material2.disown()
```

You can check if an object is owned or not by using the .is_owned() method (returns a bool)

## UStruct

UStruct's are the UE representation of low-level C/C++ structs. They work both as POD (Plain Old Data, like in C) and as class-like objects (with methods, but no encapsulation). From the Blueprint point of view, UStruct's are POD (generally in the form of User Defined Structs), while in the C++ api, most of them have regular methods.

```python
from unreal_engine.structs import StaticMeshSourceModel, MeshBuildSettings
lod1 = StaticMeshSourceModel(BuildSettings=MeshBuildSettings(bRecomputeNormals=False, bRecomputeTangents=True, bUseMikkTSpace=True, bBuildAdjacencyBuffer=True, bRemoveDegenerates=True))
```

In this example we are generating a new LOD for a StaticMesh using the StaticMeshSourceModel UStruct (https://api.unrealengine.com/INT/API/Runtime/Engine/Engine/FStaticMeshSourceModel/index.html)

you can now modify the bRecomputeNormals of lod1:

```python
lod1.BuildSettings.bRecomputeNormals = True
```

This is very python and user-friendly, but note that if the structures you are working with have been not created by python, they could became invalid as the internal memory pointer became a dangling pointer.

So, to be more clear, there are 2 kind of UStruct's: owned and not-owned. Owned structs are those created by the python api, and are generally safe. Unowned UStruct's are those created by UE4 itself, so it could destroy them at any time (albeit this rarely happens a the vast majority of structs in unreal are passed by value)

If you want to build an owned struct from an unowned one (or you want to make a simple copy), you can call the .clone() method:

```python
mesh_build_settings = lod1.BuildSettings.clone()
mesh_build_settings.bRecomputeNormals = True
lod1.BuildSettings = mesh_build_settings
```


