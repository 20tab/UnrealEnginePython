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

# Installation on MacOSX

# Installation on other platforms

Currently only Windows and MacOSX are supported, Linux support should be available soon. We are investigating Android support too via the kivy project.
