# Plotting Graphs with Matplotlib and Unreal Engine Python

In this tutorial we will see how to use third party python modules in your Unreal Engine projects.

The module of choice is 'matplotlib' https://matplotlib.org/.

From the official site:

```
Matplotlib is a Python 2D plotting library which produces publication quality figures in a variety of hardcopy formats and interactive environments across platforms. Matplotlib can be used in Python scripts, the Python and IPython shell, the jupyter notebook, web application servers, and four graphical user interface toolkits.
```

So our objective will be plotting graphs into Unreal Engine textures asset, both in the editor (to pre-generate assets) and during gameplay (to dynamically update texture-graph data)

## Creating the project

First step is obviously creating a new project.

We will use the third person template

![ThirdPerson Template](https://github.com/20tab/UnrealEnginePython/blob/master/tutorials/PlottingGraphsWithMatplotlibAndUnrealEnginePython_Assets/third_person.png)

## Installing the Python plugin

![Plugin Installation](https://github.com/20tab/UnrealEnginePython/blob/master/tutorials/PlottingGraphsWithMatplotlibAndUnrealEnginePython_Assets/enable_plugin.png)

## pip-installing matplotlib

![pip install matplotlib](https://github.com/20tab/UnrealEnginePython/blob/master/tutorials/PlottingGraphsWithMatplotlibAndUnrealEnginePython_Assets/pip_install.png)

## Testing matplotlib in Unreal Engine: generating a graph-texture

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

![Texture Created](https://github.com/20tab/UnrealEnginePython/blob/master/tutorials/PlottingGraphsWithMatplotlibAndUnrealEnginePython_Assets/texture_created.png)

## Our project: Plotting a Pie chart tracking overlap events

Our objective is to have a special blueprint in our level exposing 3 cube in 3 different colors.

Whenever you overlap one of the cubes with your character, their internal counter is incremented.

A pie chart "carpet" is on the floor, and it is updated with overlap-counters data (the more you overlap a cube, the bigger will be its slice in the chart)

### Setting up materials

We need 4 materials and 1 material instance for the project:

* a completely red material (just link a constant vector3 to base color)

* a completely green material

* a completely blue material

* a material with a texture parameter linked to base color

* an instance of the previous material exoosing the texture parameter (so it can be updated at runtime)

### The "pie chart carpet" blueprint



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

### The "Plotter Platforms"

### Playing it

### Writing a simple unit test

Sorry, i will never stop telling you how important unit tests are, so, even if we have deloped a really simple script, we will write a unit test too :)

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
