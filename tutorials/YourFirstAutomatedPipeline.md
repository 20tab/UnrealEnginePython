Your First Automated Pipeline with UnrealEnginePython
=

In this tutorial i will try to show you how to build a python script that can generate
a new Unreal Engine 4 Blueprint implementing a Kaiju (a big Japanese monster) with its materials, animations and a simple AI based on Behavior Trees.

Running the script from the Unreal Engine Python Console will result in a native Blueprint (as well as meshes, animations, a blackboard and a behaviour tree graph) that does not require the python plugin to work.

Technically i am showing the "editor scripting" features of the plugin, not its "gameplay scripting" mode.

If this is the first time you use the UnrealEnginePython plugin you should take in account the following notes:

* when you see CamelCase in the python code (for attributes and function calls) it means the UE4 reflection system is being used. Albeit CamelCase for variables and functions is not 'pythonic', you should see it as a 'signal' of jumping into UE4 reflection system.

* in this tutorial i use the embedded python editor. It is pretty raw, if you want you can use your favourite editor (Vim, Sublime, Emacs...). Anything that can edit python files will be good.

* Python scripts are stored in the '/Game/Scripts' folder of the project

* The Python code is pretty verbose and repeat itself constantly to show the biggest possible number of api features, i strongly suggest you to reorganize it if you plan to build something for your own pipeline

Installing UnrealEnginePython
-

Obviously the first step is installing the UnrealEnginePyton plugin.

Just take a binary release for your operating system/ue4 version combo and unzip it in the Plugins directory of your project (create it if it does not exist). 

You can download binary versions from here:

https://github.com/20tab/UnrealEnginePython/releases

It is highly suggested to get an embedded one so you do not need to install python in your system. You can start with a Blueprint or a C++ project, both will work.

Once you have unzipped the zip file into the Plugins directory, you can run your project

Ensure the plugin is enabled by opening the Edit/Plugins menu:

![Check Plugin](https://github.com/20tab/UnrealEnginePython/blob/master/tutorials/YourFirstAutomatedPipeline_Assets/check_plugin.png)

tick the 'Enabled' checkbox and confirm the editor restart.

If all goes well, you will get a new option under the Window menu, called the 'Python Console':

![Screenshot of Window menu](https://github.com/20tab/UnrealEnginePython/blob/master/tutorials/YourFirstAutomatedPipeline_Assets/screenshot_window_menu.png)

Run it, and ensure your python environment works:

![Python Console](https://github.com/20tab/UnrealEnginePython/blob/master/tutorials/YourFirstAutomatedPipeline_Assets/python_console.png)

In the Window menu, you will find another tool (optional), the Python Editor. It is a very simple editor for managing your project scripts. Obviously feel free to use your favourite editor.

![Python Editor](https://github.com/20tab/UnrealEnginePython/blob/master/tutorials/YourFirstAutomatedPipeline_Assets/python_editor.png)

Initializing the environment
-

Create a new python script (just click New in the python editor, or just create a new file in your favourite editor under the Scripts project directory), call it kaiju_slicer_pipeline.py with the following content

```python
import unreal_engine as ue

print('Hello i am your pipeline automator')
```

![First script in editor](https://github.com/20tab/UnrealEnginePython/blob/master/tutorials/YourFirstAutomatedPipeline_Assets/first_script_editor.png)

save thh script and click on 'Execute' to run the script.

If you are not using the embedded editor, you can run python scripts from the console with the py_exec command

```python
import unreal_engine as ue
ue.py_exec('name_of_script.py')
```

![First script in console](https://github.com/20tab/UnrealEnginePython/blob/master/tutorials/YourFirstAutomatedPipeline_Assets/first_script_console.png)

Finally, download the https://github.com/20tab/UnrealEnginePython/blob/master/tutorials/YourFirstAutomatedPipeline_Assets/Kaiju_Assets.zip file and unzip it in your Desktop folder. These are the original files (fbx, tga, ...) we will import in the project using a python script.

Importing the Mesh FBX
-

Once you have access to the PythonConsole (or the Editor), you can start issuing python commands.

```python
from unreal_engine.classe import PyFbxFactory
```

Creating the Materials
-

We are going to create two different materials, one for the blades and the other for the kaiju body.

Both materials are based on Substance Designer textures, the second one will include the ability to 'blink' the emissive texture using a sin function/node.

```python
from unreal_engine.classes import MaterialFactoryNew

material_factory = MaterialFactoryNew()

material_blades = material_factory.factory_create_new('/Game/Kaiju/Slicer/Blades_Material')

material_body = material_factory.factory_create_new('/Game/Kaiju/Slicer/Body_Material')
```

Importing Animations
-

Importing Animations uses the same factory for Fbx meshes.

Now we want to create a BlendShape1D asset. It will be composed by the 3 locomotion-related animations (idle, walk, run) and it will be governed by a variable (the X) called Speed, with a minimum value of 0 and a max of 300.



Creating the AnimationBlueprint
-


Once our assets are ready we can start creating the Animation blueprint.

The animation blueprint wil contain a state machine switching between:

* Locomotion (the blend space we created before)

* Attack (attack with blades)

* Roar (a kind of taunt)

* Bored (when idle for more than 10 seconds, starts looking around)

Its event graph manages the Speed variable for the blend space and the idle timer triggering the 'Bored' state.

Put it all in a new Blueprint
-

Now it is time to create a Character Blueprint

Filling the Event Graph
-

The BlackBoard
-

```python
from unreal_engine.classes import BlackBoardDataFactory
from unreal_engine.classes import BlackboardKeyType_Bool, BlackboardKeyType_String

from unreal_engine.structs import BlackboardEntry

```

The Behavior Tree Graph
-

The Kaiju Brain
-

Testing it
-

Final notes
-

