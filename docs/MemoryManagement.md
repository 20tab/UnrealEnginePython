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

When you create a UObject (from the C++ side, via the NewObject<T> api call) you can specify a bitmask of flags. By default the python api only use the RF_Public flag:
  
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

As an example when running a script multiple times, the 'tracker' id will be overwritten, triggering the destruction of the mapped python object (and its __del__ method)

