# The Landscape/Terrain API

Terrains in UE4 are special actors built over the 'heightmap' concept.

Each Terrain is a grid of components (ULandscapeComponent). Each component is mapped to a texture holding heights data.

The landscape component concept is really important as it impacts performance and quality of the result. A component is the minimal render unit of a terrain
(so it is the minimal geometry that can be culled both from the rendering and collisions point of view).

A brief explanaton on landscape components is available here: https://docs.unrealengine.com/latest/INT/Engine/Landscape/TechnicalGuide/#componentsections

To build a new terrain (or Landscape in UE4), you need to get a heightmap. From this heightmap, the UE4 api will generate
the textures mapped to components.

Heightmaps are simple arrays of unsigned 16bit values (0 to 65535 with 32768 value considered the 'sea level').

In Python (for performance reason, and to simplify integration with numpy) heightsmap are represented as bytearray's (so you eventually need to recast them).

# Creating a new Landscape

We start by creating a heightmap with random values:

```python
import unreal_engine as ue
import struct
import random

width = 1024
height = 1024
heightmap = []

# fill the heightmap with random values
for y in range(0, height):
    for x in range(0, width):
        heightmap.append(random.randint(0, 65535))

data = struct.pack('{0}H'.format(width * height), *heightmap)
```

Now 'data' is something we can use for the landscape api

Before filling up a landscape, we need to spawn it:

```python
from unreal_engine.classes import Landscape

new_landscape = ue.get_editor_world().actor_spawn(Landscape)
```

Note: do not run the previous script, as the editor does not like uninitialized terrains. (read: it will brutally crash)

Now it is time to fill the terrain with the heightmap data we created before. We need to choose how many components we need (the grid resolution)and how many quads are required for each component
(each component geometry is formed by simple quads primitives).

Once we know how big the terrain will be, we can expand/adapt the heightmap accordingly using a special UE4 api function:

```
unreal_engine.heightmap_expand(data, original_width, original_height, terrain_width, terrain_height)
```

This function will generate a new heightmap with the optimal dimension for the landscape.


```python
import unreal_engine as ue
import struct
import random
from unreal_engine.classes import Landscape

width = 1024
height = 1024
heightmap = []

for y in range(0, height):
    for x in range(0, width):
        heightmap.append(random.randint(0, 65535))

data = struct.pack('{0}H'.format(width * height), *heightmap)

quads_per_section = 63
number_of_sections = 1
components_x = 8
components_y = 8

fixed_data = ue.heightmap_expand(data, width, height, quads_per_section * number_of_sections * components_x + 1, quads_per_section * number_of_sections * components_y + 1)

landscape = ue.get_editor_world().actor_spawn(Landscape)
landscape.landscape_import(quads_per_section, number_of_sections, components_x, components_y, fixed_data)
landscape.set_actor_scale(1,1,1)
```

You should have noted that instead of specifying the quads per component we are using the 'section' concept.

The truth is that UE4 allows another level of subdivision for giving better control over optimizations (LOD, mipmapping...). More details here:

https://docs.unrealengine.com/latest/INT/Engine/Landscape/TechnicalGuide/#componentsections

You can have 1 section (1x1 quad) or 2 (2x2 quads). Other values are not supported.

Even the number of quads is related to textures size, so valid values are: 7x7, 15x15, 31x31, 63x63, 127x127, 255x255 (note the off-by-one weirdness, as all of the terrain tools works with max value and not the size)

Note that you need to carefully choose the size of the terrain as well as the heightmap:

https://docs.unrealengine.com/latest/INT/Engine/Landscape/TechnicalGuide/index.html#calculatingheightmapdimensions


## Getting/Creating ULandscapeInfo

Informations about a Landscape/Terrain are stored in a special uobject called ULandscapeInfo.

To retrieve it (or eventually create a new one if you are making weird operations), you have the following two functions:

```python
landscape_info = landscape.get_landscape_info()

# create a new ULandscapeInfo, required if you do not import an heightmap in a manually spawned landscape
landscape_info = landscape.create_landscape_info()
```

## Retrieving ULandscapeComponent's textures

If you want to get access to the height values of a terrain, you need to retrieve them from each component:

```python
import unreal_engine as ue

for component in landscape.LandscapeComponents:
    heightmap_texture = component.HeightmapTexture
    print('{0}x{1} -> {2}'.format(heightmap_texture.texture_get_width(), heightmap_texture.texture_get_height(), len(heightmap_texture.texture_get_source_data())))
```

this will print the texture width, height and data size of each landscape component.

## Exporting the Terrain to a FRawMesh

FRawMesh is a special structure representing a mesh. You can use it to generate new StaticMesh in UE4.

More infos can be retrieved here:

https://github.com/20tab/UnrealEnginePython/blob/master/tutorials/SnippetsForStaticAndSkeletalMeshes.md

You can generate a new FRawMesh from a landscape with this function:


```python
# lod is optional, by default it is 0
raw_mesh = landscape.landscape_export_to_raw_mesh([lod])
```

Remember that terrains are generally huge in size :)

## The Heightmap api

A heightmap high-level api is exposed to simplify heightmap manipulation

```python
# expand the heightmap to fit the new size
expanded_data = ue.heightmap_expand(data, data_width, data_height, new_width, new_height)
```

```python
# import a heightmap file (r16 or grayscale 16bit png) and returns a bytearray
data = ue.heightmap_import(filename[,width, height])
```

if width and height are not specified, the system will try to retrieve them from the file
