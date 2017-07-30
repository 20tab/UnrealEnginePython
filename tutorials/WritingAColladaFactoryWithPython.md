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

as you can see from the screenshot, the address of the UClass as well as its python UObject counterpart remains the same, while the `__init__` call is updated. Remember that whenever you create a new UClass from python, the `__init__` method is automatically called by the ClassDefaultObject (this information is useful only if you are into unreal engine internals, otherwise you can simply ignore it ;).

Remember: only functions starting with an uppercase letter will be exposed to unreal, the others will be usable only from python.

## Parse .dae files with pycollada

The collada format is based on XML, so we can easily parse it from python, but instead of doing the hard work by hand, we can use the amazing pycollada module (https://github.com/pycollada/pycollada). Just pip install it and update your class to import and use it:

```python
from unreal_engine.classes import PyFactory, StaticMesh, Object, Class

import unreal_engine as ue

from collada import Collada

class ColladaFactory(PyFactory):

    def __init__(self):
        # inform the editor that this class is able to import assets
        self.bEditorImport = True
        # register the .dae extension as supported
        self.Formats = ['dae;Collada']
        # set the UClass this UFactory will generate
        self.SupportedClass = StaticMesh

    def PyFactoryCreateFile(self, uclass: Class, parent: Object, name: str, filename: str) -> Object:
        # load the collada file
        dae = Collada(filename)
        ue.log_warning(dae)
        # create a new UStaticMesh with the specified name and parent
        static_mesh = StaticMesh(name, parent)
        return static_mesh
```

running this new script will show that the ColladaFactory got a new method:


![Added pycollada](https://github.com/20tab/UnrealEnginePython/blob/master/tutorials/WritingAColladaFactoryWithPython_Assets/added_pycollada.png)


PyFactoryCreateFile, is the virtual C++ method exposed by UPyFactory and that we are overriding via python. Note that we need to use python3 annotations to inform unreal engine (that is obviously strongly typed) about the type of the functions. PyFactoryCreateFile is called by the editor whenever we try to import a file with the ColladaFactory

Now download the example .dae file from here:

https://github.com/20tab/UnrealEnginePython/raw/master/tutorials/WritingAColladaFactoryWithPython_Assets/duck_triangles.dae

(it is a simple duck)

once you re-run the collada_factory.py script you will be able to click on 'Import' in the Countent Browser and now you the 'dae' extension will show in the list of supported ones. Select the duck file and just look at the python console:

![First import](https://github.com/20tab/UnrealEnginePython/blob/master/tutorials/WritingAColladaFactoryWithPython_Assets/first_import.png)

obviously we still have not added data to our StaticMesh, so in our Content Browser an empty Static Mesh will appear:

![Empty mesh](https://github.com/20tab/UnrealEnginePython/blob/master/tutorials/WritingAColladaFactoryWithPython_Assets/empty_mesh.png)

## Bulding the new mesh

The following part is a bit complex and requires heavy understanding of the UE4 internals.

Just read the code comments, we now need to extract the data in the dae file (vertices, uvs and normals) and build a LOD (Level of Detail) for the StaticMesh (a StaticMesh can have multiple LOD's, each one is a different Mesh).

In this case we use a single LOD (the LOD0). Pay attention to the data manipulation (using numpy). The collada format has different convention in respect to UE4. We flip UVs vertically and we swap axis to have Z on top (instead of y)

```python
from unreal_engine.classes import PyFactory, StaticMesh, Object, Class

import unreal_engine as ue

from collada import Collada

from unreal_engine.structs import StaticMeshSourceModel, MeshBuildSettings
from unreal_engine import FRawMesh
import numpy

class ColladaFactory(PyFactory):

    def __init__(self):
        # inform the editor that this class is able to import assets
        self.bEditorImport = True
        # register the .dae extension as supported
        self.Formats = ['dae;Collada']
        # set the UClass this UFactory will generate
        self.SupportedClass = StaticMesh
        ue.log(dir(self))
    
    # this functions starts with an uppercase letter, so it will be visible to the UE system
    # not required obviously, but it will be a good example
    def FixMeshData(self):
        # move from collada system (y on top) to ue4 one (z on top, forward decreases over viewer)
        for i in range(0, len(self.vertices), 3):
           xv, yv, zv = self.vertices[i], self.vertices[i+1], self.vertices[i+2]
           # invert forward
           self.vertices[i] = zv * -1
           self.vertices[i+1] = xv
           self.vertices[i+2] = yv
           xn, yn, zn = self.normals[i], self.normals[i+1], self.normals[i+2]
           # invert forward
           self.normals[i] = zn * -1
           self.normals[i+1] = xn
           self.normals[i+2] = yn
        
        # fix uvs from 0 on bottom to 0 on top
        for i, uv in enumerate(uvs):
            if i % 2 != 0:
                uvs[i] = 1 - uv

    def PyFactoryCreateFile(self, uclass: Class, parent: Object, name: str, filename: str) -> Object:
        # load the collada file
        dae = Collada(filename)
        ue.log_warning(dae)
        # create a new UStaticMesh with the specified name and parent
        static_mesh = StaticMesh(name, parent)

        # prepare a new model with the specified build settings
        source_model = StaticMeshSourceModel(BuildSettings=MeshBuildSettings(bRecomputeNormals=False, bRecomputeTangents=True, bUseMikkTSpace=True, bBuildAdjacencyBuffer=True, bRemoveDegenerates=True))

        # extract vertices, uvs and normals from the da file (numpy.ravel will flatten the arrays to simple array of floats)
        triset = dae.geometries[0].primitives[0]
        self.vertices = numpy.ravel(triset.vertex[triset.vertex_index])
        # take the first uv channel (there could be multiple channels, like the one for lightmapping)
        self.uvs = numpy.ravel(triset.texcoordset[0][triset.texcoord_indexset[0]])
        self.normals = numpy.ravel(triset.normal[triset.normal_index])

        # fix mesh data
        self.FixMeshData()
        
        # create a new mesh, FRawMesh is an ptopmized wrapper exposed by the python plugin. read: no reflection involved
        mesh = FRawMesh()
        # assign vertices
        mesh.set_vertex_positions(self.vertices)
        # uvs are required
        mesh.set_wedge_tex_coords(self.uvs)
        # normals are optionals
        mesh.set_wedge_tangent_z(self.normals)
        
        # assign indices (not optimized, just return the list of triangles * 3...)
        mesh.set_wedge_indices(numpy.arange(0, len(triset) * 3))

        # assign the FRawMesh to the LOD0 (the model we created before)
        mesh.save_to_static_mesh_source_model(source_model)

        # assign LOD0 to the SataticMesh and build it
        static_mesh.SourceModels = [source_model]
        static_mesh.static_mesh_build()
        static_mesh.static_mesh_create_body_setup()

        return static_mesh
```

re-run the script and import the same file again (by clicking Import in the Content Browser), if all goes well you will end with the duck mesh:

![The Duck](https://github.com/20tab/UnrealEnginePython/blob/master/tutorials/WritingAColladaFactoryWithPython_Assets/the_duck.png)

Try to play a bit with MeshBuildSettings values in the code (expecially note the difference between normals in the file and the autogenerated ones ;)

## A more complex model: Mixamo's Vampire

Download this file: https://github.com/20tab/UnrealEnginePython/raw/master/tutorials/WritingAColladaFactoryWithPython_Assets/vampire.dae

it is the Vampire from mixmamo.com.

Technically it is a skeletal mesh with an animation (something we will try to import in a future tutorial), if we try to import it we will end with a dead vampire :P

![Broken vampire](https://github.com/20tab/UnrealEnginePython/blob/master/tutorials/WritingAColladaFactoryWithPython_Assets/broken_vampire.png)

time to use some vector math to fix the mesh rotation:

```python
...

from unreal_engine import FVector, FRotator
...
def FixMeshData(self):
        # move from collada system (y on top) to ue4 one (z on top, forward decreases over viewer)
        for i in range(0, len(self.vertices), 3):
           xv, yv, zv = self.vertices[i], self.vertices[i+1], self.vertices[i+2]
           # invert forward
           vec = FVector(zv * -1, xv, yv) * FRotator(0, -90, 180)
           self.vertices[i] = vec.x
           self.vertices[i+1] = vec.y
           self.vertices[i+2] = vec.z
           xn, yn, zn = self.normals[i], self.normals[i+1], self.normals[i+2]
           nor = FVector(zn * -1, xn, yn) * FRotator(0, -90, 180)
           # invert forward
           self.normals[i] = nor.x
           self.normals[i+1] = nor.y
           self.normals[i+2] = nor.z
        
        # fix uvs from 0 on bottom to 0 on top
        for i, uv in enumerate(uvs):
            if i % 2 != 0:
                uvs[i] = 1 - uv
```

re-run and re-import the vampire, it should be correctly rotated now.

Obviously if we reimport the duck with the current code, its rotation will be wrong. So, time to add a configurator GUI for the factory.

## Adding a GUI to the importer: The Slate API

## Persistent importer options: more subclassing

## Automatically add the ColladaFactory on editor startup

## Improvements and final notes
