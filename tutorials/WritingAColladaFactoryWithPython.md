# Writing a Collada StaticMesh importer/factory with Python

In this tutorial we will see how to use the advanced subclassing api available in the Unreal Engine 4 Python plugin, as well as how to build Slate GUIs

This tutorial has been tested with Unreal Engine 4.16 and Python 3.6 and assumes you are already familiar with installing the plugin into the engine and editing Python scripts.

If this is your first experience with the Unreal Engine Python plugin, this link is a good place to start: https://github.com/20tab/UnrealEnginePython/blob/master/tutorials/PlottingGraphsWithMatplotlibAndUnrealEnginePython.md#installing-the-python-plugin


The objective is to instruct the editor to import .dae (collada) files as static meshes, by building a new Factory.
A Factory is a special class in Unreal Engine 4 allowing to create and import assets. There are factories for fbx's, wav's, images and all of the media files supported by UE4.

You generally create new factories by subclassing the UFactory class in C++, but in this tutorial we will see how to subclass UE4 C++ classes in pure python.



## Subclassing UFactory: PyFactory

The subclassing api allows the developer to subclass any UClass (classes exposed to the UE4 reflection system). Unfortunately the UFactory class does not expose
its methods to the reflection system, so an handy class named 'UPyFactory' (it ia simple subclass of UFactory) is exposed by the python plugin:

```python
from unreal_engine.classes import PyFactory

import unreal_engine as ue

class ColladaFactory(PyFactory):

    def __init__(self):
        ue.log_error('Hello World, i am a useless factory')
```

save the file as collada_factory.py into the /Content/Scripts directory and run it rom the Unreal Engine Python Console:

![Class prototype](https://github.com/20tab/UnrealEnginePython/blob/master/tutorials/WritingAColladaFactoryWithPython_Assets/class_prototype.png)

Note that whenever you re-run the scripts, the internal Unreal Engine reflection system is updated, so you do not need to restart the editor (or launch slow compilations) when you make changes to the script code:

![Multiple runs](https://github.com/20tab/UnrealEnginePython/blob/master/tutorials/WritingAColladaFactoryWithPython_Assets/multiple_runs.png)

as you can see from the screenshot, the address of the UClass as well as its python UObject counterpart remains the same, while the __init__ call is updated. Remember that whenever you create a new UClass from python, the __init__ method is automatically called by the ClassDefaultObject (this information is useful only if you are into unreal engine internals, otherwise you can simply ignore it ;).

## Parse .dae files with pycollada

## Adding a GUI to the importer: The Slate API

## Persistent importer options: more subclassing

## Automatically add the ColladaFactory on editor startup

## Improvements and final notes
