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

Whenever the UE GC runs, the UNrealEnginePython GC will run too, checking if a UObject mapped to a py_UEObject is still alive.

If the UObject mapped to a python object is dead, an exception will be triggered.

This is an example:

```python
import unreal_engine as ue

from unreal_engine.classes import BlueprintFactory

factory = BlueprintFactory()
# run GC
ue.console_exec('obj gc')
# this will raise an exception as the UObject mapped to factory has been destroyed by the GC run
print(factory)
```

By running this script you will end with something like this:

```
PyUObject is in invalid state
Traceback (most recent call last):
  File "<string>", line XX, in <module>
Exception: PyUObject is in invalid state
```

Very long scripts, that do lot of stuff, often triggering UE4 GC, could be blocked in the middle of their execution by this kind of errors. In such a case (like you would do in C++) you need to inform the UE GC on how to deal with them (for avoiding their destruction).

Pay attention, as once you tell the UE GC to not destroy a UObject, that UObject (and its python mapping) will stay in memory (so you will end with a leak)

## Strategy 1: Setting UObject flags to govern the GC

When you create a UObject (from the C++ side, via the ```NewObject<T>``` api call) you can specify a bitmask of flags. By default the python api only use the RF_Public flag:
  
https://api.unrealengine.com/INT/API/Runtime/CoreUObject/UObject/EObjectFlags/index.html

You can change this bitmask with the set_obj_flags() python function:

```python
import unreal_engine as ue

from unreal_engine.classes import BlueprintFactory

factory = BlueprintFactory()
# assign mask 0x00000001|0x00000002
factory.set_obj_flags(ue.RF_PUBLIC|ue.RF_STANDALONE)
# run GC
ue.console_exec('obj gc')
# this will normally print the UObject repr
print(factory)
```

The RF_Standalone flag (RF_STANDALONE in python api) will marks a UObject as 'standalone' so it will remain resident in memory forever.

Eventually you can reset/set the flags:

```python
import unreal_engine as ue

from unreal_engine.classes import BlueprintFactory

factory = BlueprintFactory()
factory.set_obj_flags(ue.RF_PUBLIC|ue.RF_STANDALONE)


ue.console_exec('obj gc')

print(factory)

# the second True argument will reset the flags (otherwise set_obj_flags will work in append mode)
# eventually you can call factory.reset_obj_flags()
factory.set_obj_flags(ue.RF_PUBLIC, True)

ue.console_exec('obj gc')

print(factory)
```

The second print will raise the error.

This is a pretty raw approach (unless you are sure that you need a resident object). For having more control the second strategy will be way more better...

## Strategy 2: The Root Set

The root set is a very specific part of the GC tree. If you want to hold control of a UObject lifecycle in an efficient way, you can use the related python api:

```python
import unreal_engine as ue

from unreal_engine.classes import BlueprintFactory

factory = BlueprintFactory()
factory.add_to_root()

ue.console_exec('obj gc')

print(factory)

factory.remove_from_root()

ue.console_exec('obj gc')

print(factory)
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
factory = tracker.track(BlueprintFactory())
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


## UStruct

UStruct's are the UE representation of low-level C/C++ structs. They work both as POD (Plain Old Data, like in C) and as class-like objects (with methods, but no encapsulation). From the Blueprint point of view, UStruct's are POD (generally in the form of User Defined Structs), while in the C++ api, most of them have regular methods.

UStruct in the python api are passed by value (as there is no way to track them safely, so dangling pointers could spawn up all over the place), and this leads to some common headache:

```python
from unreal_engine.structs import StaticMeshSourceModel, MeshBuildSettings
lod1 = StaticMeshSourceModel(BuildSettings=MeshBuildSettings(bRecomputeNormals=False, bRecomputeTangents=True, bUseMikkTSpace=True, bBuildAdjacencyBuffer=True, bRemoveDegenerates=True))
```

In this example we are generating a new LOD for a StaticMesh using the StaticMeshSourceModel UStruct (https://api.unrealengine.com/INT/API/Runtime/Engine/Engine/FStaticMeshSourceModel/index.html)

now you may think that the following code will modify the bRecomputeNormals of lod1:

```python
# WRONG you are working on a copy !!!
lod1.BuildSettings.bRecomputeNormals = True
```

but (as you read in the comment) it is wrong as lod1.BuildSettings will return a copy of the original MeshBuildSettings UStruct, so technically you are updating a brand new structure that will be destroyed soon after.

Instead you should recreate and assign a whole new MeshBuildSettings:

```python
lod1.BuildSettings = MeshBuildSettings(bRecomputeNormals=True, bRecomputeTangents=True, bUseMikkTSpace=True, bBuildAdjacencyBuffer=True, bRemoveDegenerates=True)
```

If you need you can make a copy/clone of a struct to avoid copy/paste code:

```python
mesh_build_settings = lod1.BuildSettings.clone()
mesh_build_settings.bRecomputeNormals = True
lod1.BuildSettings = mesh_build_settings
```

This kind of work-mode is not very pythonic (and generally unintuitive) but it is the safest way to avoid very-hard-to-debug crashes triggered by dangling pointers generated by the way structures works in UE.

If instead, you know what you are doing, you can work in pass-by-ref mode with structures:

```python
lod1.BuildSettings.ref().bRecomputeNormals = True
```

The ref() method will return a new structure that contains a reference/pointer to the original one. This is possible as whenever we create a new UStruct we save its value as well as its pointer. Obviously ref() can point to a non valid memory area, in such a case expect any kind of evilness ;)
