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

Installing UnrealEnginePython
-

Obviously the first step is install the UnrealEnginePyton plugin.

Just take a binary release for your operating system/ue4 version combo and unzip it in the Plugins directory of your project (create it if it does not exist). It is highly suggested to get an embedded one so you do not need to install python in your system.

Importing the Mesh FBX
-

Once you have access to the PythonConsole, you can start issuing python commands.

```python
from unreal_engine.classe import PyFbxFactory
```

Creating the Materials
-

```python
from unreal_engine.classes import MaterialFactoryNew
```

Importing Animations
-

Creating the AnimationBlueprint
-

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

The Kaiju AIController
-

Testing it
-

Final notes
-

