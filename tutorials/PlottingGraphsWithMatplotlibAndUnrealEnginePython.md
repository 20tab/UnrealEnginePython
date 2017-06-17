# Plotting Graphs with Matplotlib and Unreal Engine Python


## Creating the project

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
