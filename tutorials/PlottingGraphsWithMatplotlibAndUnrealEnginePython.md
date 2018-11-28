# Plotting Graphs with Matplotlib and Unreal Engine Python

In this tutorial we will see how to use third party python modules in your Unreal Engine projects.

The module of choice is 'matplotlib' https://matplotlib.org/.

From the official site:

>Matplotlib is a Python 2D plotting library which produces publication quality figures in a variety of hardcopy formats and interactive environments across platforms. Matplotlib can be used in Python scripts, the Python and IPython shell, the jupyter notebook, web application servers, and four graphical user interface toolkits.

So our objective will be plotting graphs into Unreal Engine textures asset, both in the editor (to pre-generate assets) and during gameplay (to dynamically update texture-graph data)

## Creating the project

First step is obviously creating a new project.

We will use the third person template

![ThirdPerson Template](https://github.com/20tab/UnrealEnginePython/blob/master/tutorials/PlottingGraphsWithMatplotlibAndUnrealEnginePython_Assets/third_person.png)

## Installing the Python plugin

Ensure your project/editor has been closed.

Download the most recent embedded binary release from here: https://github.com/20tab/UnrealEnginePython/releases

You can use a non-embedded version too if you already have python installed in your system and you are confident with it.

The only difference in the tutorial is in where the matplotlib module will be installed:

For embedded version, the installation will happen in the plugin directory itself. For non-embedded it will be in the python system path.

Obviously if your system python installation already includes matplotlib, you can simply skip the related paragraph below.

>Note: on Linux and Mac system, you will use the system installation (no embedded distributions are provided) or directly the source one. The rest of the tutorial will be platform independent.

Create a Plugins/ directory in your project and unzip the plugin archive into it.

Re-start your project and in the Edit/Plugins menu you will be able to enable the python plugin:

![Plugin Installation](https://github.com/20tab/UnrealEnginePython/blob/master/tutorials/PlottingGraphsWithMatplotlibAndUnrealEnginePython_Assets/enable_plugin.png)

## pip-installing matplotlib

If you installed the embedded distribution, just move to the Plugins/UnrealEnginePython/Binaries/Win64 directory in your terminal, and run:

```sh
./python.exe -m pip install --target . matplotlib
```

(the --target . ensures the modules are not installed into the Lib/site-packages subdirectory)

![pip install matplotlib](https://github.com/20tab/UnrealEnginePython/blob/master/tutorials/PlottingGraphsWithMatplotlibAndUnrealEnginePython_Assets/pip_install.png)

If you are using a system python installation (or you are on Linux/Mac) just run:

```sh
pip3 install matplotlib
```

or (based on your config)

```sh
pip install matplotlib
```

In the Unreal Engine Python Console (under the 'Window/Developer Tools/Python Console' menu entry) run

```python
import matplotlib
```

if all is ok, you will not get an import error

## Testing matplotlib in Unreal Engine: generating a graph-texture

Time to check if all is working well.

We will start by generating a texture asset. The content of this texture will be managed by matplotlib.

You can use the included python editor to edit your scripts (it is under the 'Window/Python Editor' menu entry), or your favourite one (SublimeText, Vim, PyCharm...) just ensure scripts are under the Content/Scripts directory of your project.

The steps are: generating a texture in memory, plotting the graph, transferring the graph data into texture memory, save it as an asset and opening the related editor.

```python
import unreal_engine as ue
# EPixelFormat defines the various pixel formats for a texture/image, we will use RGBA with 8bit per channel
from unreal_engine.enums import EPixelFormat

import matplotlib
# set the Agg renderer as we do not need any toolkit
matplotlib.use('Agg')

import matplotlib.pyplot as plt

# set texture/plot dimensions and dpi, ensure dpi is a float !
width = 1024
height = 1024
dpi = 72.0

# create a new figure with the specified sizes
fig = plt.figure(1)
fig.set_dpi(dpi)
fig.set_figwidth(width/dpi)
fig.set_figheight(height/dpi)

# plot a simple graph with a label on the y axis
plt.plot([1, 2, 3, 4])
plt.ylabel('some numbers')

# draw the graph (in memory)
fig.canvas.draw()

# create a texture in memory (will be saved later)
texture = ue.create_transient_texture(width, height, EPixelFormat.PF_R8G8B8A8)
# copy pixels from matplotlib canvas to the texture as RGBA
texture.texture_set_data(fig.canvas.buffer_rgba())

# save the texture
texture.save_package('/Game/FirstGraphTexture')

# open its editor
ue.open_editor_for_asset(texture)
```

if you are using the included Python Editor, click on 'Save' and then on 'Execute'

if you are using an external editor you can run the script form the python console with:

```python
import unreal_engine as ue
ue.py_exec('name_of_your_script.py')
```

![Texture Created](https://github.com/20tab/UnrealEnginePython/blob/master/tutorials/PlottingGraphsWithMatplotlibAndUnrealEnginePython_Assets/texture_created.png)

if you ended up with the texture editor opened on your new graph, you are ready for the next steps



## Our project: Plotting a Pie chart tracking overlap events

Our objective is to have a special blueprint in our level exposing 3 cubes in 3 different colors.

Whenever you overlap one of the cubes with your character, their internal counter is incremented.

A pie chart "carpet" is on the floor, and it is updated with overlap-counters data (the more you overlap a cube, the bigger will be its slice in the chart)

At this point, i strongly suggest you to restart the editor so your python environment will be correctly setup to search for modules in the /Content/Scripts directory (this is because the python modules search path is not populated with non existent dirctories on startup, and unfortunately /Content/Scripts still does not exist when you run the plugin for the first time)

### Setting up materials

We need 4 materials and 1 material instance for the project:

* a completely red material (just link a constant vector3 to base color)

* a completely green material

* a completely blue material

* a material with a texture parameter named 'Graph' linked to base color (use any texture you have, it will be substituted at runtime)

* an instance of the previous material exposing the texture parameter (so it can be updated at runtime, again any texture will be good, even the same of the parent material)

### The "pie chart carpet" blueprint

The first blueprint we are about to create, is the 'carpet' (we call it Graph_Blueprint)

![Graph Blueprint](https://github.com/20tab/UnrealEnginePython/blob/master/tutorials/PlottingGraphsWithMatplotlibAndUnrealEnginePython_Assets/graph_blueprint.png)

As you can see it is really simple:

* inherit from standard Actor
* add a StaticMeshComponent (as root if you want) with a plane as the mesh (scale it to 10 on all axis) and the material instance you created before
* add a Python component with the module as 'plotter' and the class as 'PlotComponent' (this is where the matplotlib part will be)
* add an event dispatcher called 'OnGraphDataUpdated' (this will be triggered whenever the character steps over a cube) taking a single input argument named 'Platform' of type 'Actor'

You can now create (again into Content/Scripts) the plotter.py python script

```python
import unreal_engine as ue
from unreal_engine.enums import EPixelFormat

import matplotlib
matplotlib.use('Agg')

import matplotlib.pyplot as plt

class PlotComponent:
  
    def begin_play(self):
        width = 1024
        height = 1024
        dpi = 72.0
        self.texture = ue.create_transient_texture(width, height, EPixelFormat.PF_R8G8B8A8)

        self.uobject.get_owner().StaticMeshComponent.OverrideMaterials[0].set_material_texture_parameter('Graph', self.texture)

        self.fig = plt.figure(1)
        self.fig.set_dpi(dpi)
        self.fig.set_figwidth(width/dpi)
        self.fig.set_figheight(height/dpi)

        self.uobject.get_owner().bind_event('OnGraphDataUpdated', self.update_graph)

    def update_graph(self, platform):
        # clear the current plot data
        plt.clf()
        # draw a pie chart        
        plt.pie([platform.RedCubeCounter, platform.GreenCubeCounter, platform.BlueCubeCounter], colors=['r', 'g', 'b'], labels=['RedCube', 'GreenCube', 'BlueCube'], shadow=True)

        self.fig.canvas.draw()
        self.texture.texture_set_data(self.fig.canvas.buffer_rgba())
```

The only relevant part is the update_graph function that is triggered by the 'OnGraphDataUpdated' event dispatcher.

This function receives as the 'platform' argument the Actor triggering the event. This Actor (that we will create in the next phase), exposes the counters of the overlapping cubes. We use that values to re-generate our pie chart and uploading it into texture memory whenever the event is triggered 

### The "Plotter Platforms"

The 'PlotterPlatforms' blueprint, implements the actor containing the 3 cubes and manages the overlapping events as well as incrementing the 3 counters.

Let's start with the viewport, you need to add 3 static meshes, each with a cube and the related solid-color material we created earlier. Ensure their collision mode is set to OverlapAll:

![PlotterPlatforms Blueprint Viewport](https://github.com/20tab/UnrealEnginePython/blob/master/tutorials/PlottingGraphsWithMatplotlibAndUnrealEnginePython_Assets/platforms_viewport.png)

then we define a blueprint function 'NotifyPlotter', that will trigger the event dispatcher on the Graph_Blueprint (our carpet):

![PlotterPlatforms Blueprint Notify Plotter](https://github.com/20tab/UnrealEnginePython/blob/master/tutorials/PlottingGraphsWithMatplotlibAndUnrealEnginePython_Assets/platforms_notify_plotter.png)

finally we have the Event Graph, here we simply increment the related counter variables and call the 'NotifyPlotter' function:

![PlotterPlatforms Blueprint Event Graph](https://github.com/20tab/UnrealEnginePython/blob/master/tutorials/PlottingGraphsWithMatplotlibAndUnrealEnginePython_Assets/platforms_event_graph.png)

Ensure to add the 3 Integer variables (check the lower-left side of the screenshots) for the counters !

### Playing it

Drag the two blueprints (Graph_Blueprint and PlotterPlatforms) into the Level (choose good positions for them ;) and hit Play in your editor:

![Play](https://github.com/20tab/UnrealEnginePython/blob/master/tutorials/PlottingGraphsWithMatplotlibAndUnrealEnginePython_Assets/play.png)

walk over the 3 cubes and see your pie chart updating

### Writing a simple unit test

Sorry, i will never stop telling you how important unit tests are, so, even if we have developed a really simple script, we will write a unit test too :)

We need to test that when a 'ComponentOverlap' event is triggered on a cube, the right counter is incremented:

```python
import unittest
import unreal_engine as ue
from unreal_engine.classes import Blueprint

class TestPlotterPlatform(unittest.TestCase):

    def setUp(self):
        ue.allow_actor_script_execution_in_editor(True)
        ue.begin_transaction('test')
        self.world = ue.get_editor_world()
        self.blueprint = ue.load_object(Blueprint, '/Game/PlotterPlatforms.PlotterPlatforms')

    def tearDown(self):
        ue.end_transaction()
        ue.editor_undo()
        ue.allow_actor_script_execution_in_editor(False)

    def test_red_cube_overlap(self):
        actor = self.world.actor_spawn(self.blueprint.GeneratedClass)
        self.assertEqual(actor.RedCubeCounter, 0)
        actor.RedCube.broadcast('OnComponentBeginOverlap')
        self.assertEqual(actor.RedCubeCounter, 1)
        self.assertEqual(actor.GreenCubeCounter, 0)
        self.assertEqual(actor.BlueCubeCounter, 0)

    def test_green_cube_overlap(self):
        actor = self.world.actor_spawn(self.blueprint.GeneratedClass)
        self.assertEqual(actor.GreenCubeCounter, 0)
        actor.GreenCube.broadcast('OnComponentBeginOverlap')
        self.assertEqual(actor.RedCubeCounter, 0)
        self.assertEqual(actor.GreenCubeCounter, 1)
        self.assertEqual(actor.BlueCubeCounter, 0)

    def test_blue_cube_overlap(self):
        actor = self.world.actor_spawn(self.blueprint.GeneratedClass)
        self.assertEqual(actor.BlueCubeCounter, 0)
        actor.BlueCube.broadcast('OnComponentBeginOverlap')
        self.assertEqual(actor.RedCubeCounter, 0)
        self.assertEqual(actor.GreenCubeCounter, 0)
        self.assertEqual(actor.BlueCubeCounter, 1)

if __name__ == '__main__':
    unittest.main(exit=False)
```

You can run the unit test with:

```python
ue.sandbox_exec('test_plotter_platforms.py')
```

the 'sanbox' execution, ensures a clean python subinterpeter is initialized, instead of clobberign the main one.

Check how the test setUp and tearDown methods, ensure your world is cleaned up at the end (using the transaction api of unreal engine: https://github.com/20tab/UnrealEnginePython/blob/master/docs/Transactions_API.md)

### Downloading scripts:

the plotter.py Python Component: https://github.com/20tab/UnrealEnginePython/blob/master/tutorials/PlottingGraphsWithMatplotlibAndUnrealEnginePython_Assets/plotter.py

the graph_texture.py example script: https://github.com/20tab/UnrealEnginePython/blob/master/tutorials/PlottingGraphsWithMatplotlibAndUnrealEnginePython_Assets/graph_texture.py

the unit test for the 'Plotter Platforms' blueprint: https://github.com/20tab/UnrealEnginePython/blob/master/tutorials/PlottingGraphsWithMatplotlibAndUnrealEnginePython_Assets/test_plotter_platforms.py
