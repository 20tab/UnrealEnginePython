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
