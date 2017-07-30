# Writing a Collada StaticMesh importer/factory with Python

In this tutorial we will see how to use the advanced subclassing api available in the Unreal Engine 4 Python plugin, as well as how to build Slate GUIs

This tutorial has been tested with Unreal Engine 4.16 and Python 3.6 and assumes you are already familiar with installing the plugin into the engine and editing Python scripts.

If this is your first experience with the Unreal Engine Python plugin, this link is a good place to start: https://github.com/20tab/UnrealEnginePython/blob/master/tutorials/PlottingGraphsWithMatplotlibAndUnrealEnginePython.md#installing-the-python-plugin


The objective is to instruct the editor to import .dae (collada) files as static meshes., by building a new Factory. A Factory is a special class
in Unreal Engine 4 allowing to create and import assets. There are factories for fbx's, wav's, images and all of the media files supported by UE4.

You generally create new factories by subclassing the UFactory class in C++, but in this tutorial we will see how to subclass UE4 C++ classes in pure python.



## Subclassing UFactory: PyFactory

The subclassing api allows the developer to subclass any UClass (classes exposed to the UE4 reflection system). Unfortunately the UFactory class does not expose
its methods to the reflection system, so an handy class named 'UPyFactory' is exposed by the python plugin:

```python
```
