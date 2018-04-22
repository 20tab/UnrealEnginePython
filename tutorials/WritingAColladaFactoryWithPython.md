# Writing a Collada StaticMesh importer/factory with Python

In this tutorial we will see how to use the advanced subclassing api available in the Unreal Engine 4 Python plugin, as well as how to build Slate GUIs

This tutorial has been tested with Unreal Engine 4.16 and Python 3.6 and assumes you are already familiar with installing the plugin into the engine and editing Python scripts.

If this is your first experience with the Unreal Engine Python plugin, this link is a good place to start: https://github.com/20tab/UnrealEnginePython/blob/master/tutorials/PlottingGraphsWithMatplotlibAndUnrealEnginePython.md#installing-the-python-plugin


The objective is to instruct the editor to import .dae (collada) files as static meshes, by building a new Factory.
A Factory is a special class in Unreal Engine 4 allowing to create and import assets. There are factories for fbx's, wav's, images and all of the media files supported by UE4.

You generally create new factories by subclassing the UFactory class in C++, but in this tutorial we will see how to subclass UE4 C++ classes in pure python.



## Subclassing UFactory: PyFactory

The subclassing api allows the developer to subclass any UClass (classes exposed to the UE4 reflection system). Unfortunately the UFactory class does not expose
its methods to the reflection system, so an handy class named 'UPyFactory' (it is a simple subclass of UFactory) is exposed by the python plugin:

```python
from unreal_engine.classes import PyFactory

import unreal_engine as ue

class ColladaFactory(PyFactory):

    def __init__(self):
        ue.log_error('Hello World, i am a useless factory')
```

save the file as collada_factory.py into the /Content/Scripts directory and run it from the Unreal Engine Python Console:

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

once you re-run the collada_factory.py script you will be able to click on 'Import' in the Countent Browser and now the 'dae' extension will show in the list of supported ones. Select the duck file and just look at the python console:

![First import](https://github.com/20tab/UnrealEnginePython/blob/master/tutorials/WritingAColladaFactoryWithPython_Assets/first_import.png)

obviously we still have not added data to our StaticMesh, so in our Content Browser an empty Static Mesh will appear:

![Empty mesh](https://github.com/20tab/UnrealEnginePython/blob/master/tutorials/WritingAColladaFactoryWithPython_Assets/empty_mesh.png)

## Building the new mesh

The following part is a bit complex and requires heavy understanding of the UE4 internals.

Just read the code comments, we now need to extract the data in the dae file (vertices, uvs and normals) and build a LOD (Level of Detail) for the StaticMesh (a StaticMesh can have multiple LOD's, each one is a different Mesh).

In this case we use a single LOD (the LOD0). Pay attention to the data manipulation (using numpy) in FixMeshData(). The collada format has different conventions in respect to UE4. We flip UVs vertically and we swap axis to have Z on top (instead of y)

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
        
        # create a new mesh, FRawMesh is an optimized wrapper exposed by the python plugin. read: no reflection involved
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

it is the Vampire from mixamo.com.

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
        for i, uv in enumerate(self.uvs):
            if i % 2 != 0:
                self.uvs[i] = 1 - uv
```

re-run and re-import the vampire, it should be correctly rotated now.

Obviously if we reimport the duck with the current code, its rotation will be wrong. So, time to add a configurator GUI for the factory.

## Adding a GUI to the importer: The Slate API

Slate is the GUI technology of Unreal Engine. It is based on tons of preprocessor macros to simplify readability to the developer building graphical interfaces. Its python wrapper tries to resemble this spirit by adapting it to the pythonic style. Note: the slate python api wrapper development has been completely sponsored by Kite & Lightning (http://kiteandlightning.la/). Big thanks to them.

A Python Slate interface looks like this:

```python
from unreal_engine import SWindow, SVerticalBox, SHorizontalBox, SButton, STextBlock, SBorder, FLinearColor
from unreal_engine.enums import EHorizontalAlignment, EVerticalAlignment

import unreal_engine as ue

SWindow(title='Hello I am Slate', client_size=(1024, 512))(
    SVerticalBox()
    (
        STextBlock(text='I am the first top vertical item, without alignment'),
    )
    (
        SBorder(color_and_opacity=FLinearColor(1, 0, 0, 1))
        (
            STextBlock(text='I am the second top vertical item with center/bottom alignment'),      
        ),
        h_align = EHorizontalAlignment.HAlign_Center,
        v_align = EVerticalAlignment.VAlign_Bottom
    )
    (
        SHorizontalBox()
        (
            SButton(text='Button Left', on_clicked=lambda: ue.log('Hello from left'), h_align=EHorizontalAlignment.HAlign_Center)
        )
        (
            SButton(text='Button Right', on_clicked=lambda: ue.log('Hello from right'), v_align=EVerticalAlignment.VAlign_Center)
        ),
        fill_height=0.2
    )
)
```

feel free to run the previous code to obtain something like this (click on the buttons, they will write to the console !):

![Slate demo](https://github.com/20tab/UnrealEnginePython/blob/master/tutorials/WritingAColladaFactoryWithPython_Assets/slate_demo.png)

Pay attention to the opening parenthesis at the end of the SWindow() line. It is important the first opening is on the same line to inform the python parser that the instruction is not ended. (this is only required for the first opening)

Now we want to create a modal window just after the dae file is parsed to ask the user to confirm the import and to specify the rotation to apply to the mesh:

```python

...

from unreal_engine import SWindow, SVerticalBox, SHorizontalBox, SButton, SRotatorInputBox
from unreal_engine.enums import EHorizontalAlignment

...
def PyFactoryCreateFile(self, uclass: Class, parent: Object, name: str, filename: str) -> Object:

    ...

    def open_collada_wizard(self):

        def cancel_import():
            self.wizard.request_destroy()

        def confirm_import():
            self.do_import = True
            self.wizard.request_destroy()

        def update_roll(roll):
            self.force_rotation.roll=roll

        def update_pitch(pitch):
            self.force_rotation.pitch=pitch

        def update_yaw(yaw):
            self.force_rotation.yaw=yaw

        self.wizard = SWindow(title='Collada Import Options', modal=True, sizing_rule=1)(
                          SVerticalBox()
                          (
                              SRotatorInputBox(roll=lambda:self.force_rotation.roll,
                                               pitch=lambda:self.force_rotation.pitch,
                                               yaw=lambda:self.force_rotation.yaw,
                                               on_roll_changed=update_roll,
                                               on_pitch_changed=update_pitch,
                                               on_yaw_changed=update_yaw,
                                               ),
                              auto_height=True,
                              padding = 10
                          )
                          (
                              SHorizontalBox()
                              (
                                  SButton(text='Cancel', on_clicked=cancel_import, h_align = EHorizontalAlignment.HAlign_Center)
                              )
                              (
                                  SButton(text='Import', on_clicked=confirm_import, h_align = EHorizontalAlignment.HAlign_Center)
                              ),
                              auto_height=True,
                              padding = 4,
                          ),
                          
                      )
        self.wizard.add_modal()

```

add the method to the ColladaFactory and modify the PyFactoryCreateFile function like this:

```python
def PyFactoryCreateFile(self, uclass: Class, parent: Object, name: str, filename: str) -> Object:
        # load the collada file
        dae = Collada(filename)
        ue.log_warning(dae)

        self.do_import = False
        self.force_rotation = FRotator()
        self.open_collada_wizard()

        # if the user click on 'Cancel', return !
        if not self.do_import:
            return None
        ....
```

Re running the import will result in this wizard:

![Import options](https://github.com/20tab/UnrealEnginePython/blob/master/tutorials/WritingAColladaFactoryWithPython_Assets/import_options.png)

Remember to change the FixMeshData function to honour the self.force_rotation value:

```python
# this functions starts with an uppercase letter, so it will be visible to the UE system
    # not required obviously, but it will be a good example
    def FixMeshData(self):
        # move from collada system (y on top) to ue4 one (z on top, forward decreases over viewer)
        for i in range(0, len(self.vertices), 3):
           xv, yv, zv = self.vertices[i], self.vertices[i+1], self.vertices[i+2]
           # invert forward
           vec = FVector(zv * -1, xv, yv) * self.force_rotation
           self.vertices[i] = vec.x
           self.vertices[i+1] = vec.y
           self.vertices[i+2] = vec.z
           xn, yn, zn = self.normals[i], self.normals[i+1], self.normals[i+2]
           nor = FVector(zn * -1, xn, yn) * self.force_rotation
           # invert forward
           self.normals[i] = nor.x
           self.normals[i+1] = nor.y
           self.normals[i+2] = nor.z
        
        # fix uvs from 0 on bottom to 0 on top
        for i, uv in enumerate(self.uvs):
            if i % 2 != 0:
                self.uvs[i] = 1 - uv
```

## Persistent importer options: more subclassing

All should work fine, but it would be way cooler if the options we set in the importer wizard are remembered between imports.

We could obviously create a python class for storing values and mapping an instance of it to the ColladaFactory, but there is a better approach that will allow us to generate slate widgets automatically (without describing every single part of the interface).

Remember that you can inherit from every UClass, even UObject !

```python
from unreal_engine.classes import Material, Object
from unreal_engine.struct import Rotator

class ColladaImportOptions(Object):

    # this is exposed as a UProperty
    DefaultRotation = Rotator
    # this is exposed as a UProperty
    DefaultMaterial = Material
    
...

class ColladaFactory(PyFactory):

    ImportOptions = ColladaImportOptions()
    
    ...

```

Now every time a ColladaFactory is spawned it will hold a reference to the ColladaImportOptions() instance. In this way the values we enter will be remembered between runs.

Note that we have set a default material to assign to the mesh.

The new open_collada_wizard function is now super-easy as the widget will be automatically generated by the `ue.create_detail_view(obj)` function:

```python
def open_collada_wizard(self):

        def cancel_import():
            self.wizard.request_destroy()

        def confirm_import():
            self.do_import = True
            self.wizard.request_destroy()

        self.wizard = SWindow(title='Collada Import Options', modal=True, sizing_rule=1)(
                          SVerticalBox()
                          (
                              ue.create_detail_view(self.ImportOptions),
                              auto_height=True,
                              padding = 10
                          )
                          (
                              SHorizontalBox()
                              (
                                  SButton(text='Cancel', on_clicked=cancel_import, h_align = EHorizontalAlignment.HAlign_Center)
                              )
                              (
                                  SButton(text='Import', on_clicked=confirm_import, h_align = EHorizontalAlignment.HAlign_Center)
                              ),
                              auto_height=True,
                              padding = 4,
                          ),
                          
                      )
        self.wizard.add_modal()
```

To assign the material to the mesh just add this call before returning the static_mesh:

```python
static_mesh.StaticMaterials = [StaticMaterial(MaterialInterface=self.ImportOptions.DefaultMaterial, MaterialSlotName='Main')]
```

You can download the full updated script here:

https://github.com/20tab/UnrealEnginePython/blob/master/tutorials/WritingAColladaFactoryWithPython_Assets/collada_factory.py

## Automatically add the ColladaFactory on editor startup

When you are satisfied with your new class, you may want to automatically load it at editor/engine boot.

Just remember that the /Content/Scripts/ue_site.py script is automatically imported (if found) on boot, so just create an empty one and insert:

```python
import collada_factory
```


## Improvements and final notes

This is only a brief introduction to the subclassing and slate API's. Both are still in-development areas so expect more improvements and shortcuts.

A great (future) addition would be loading skeletal meshes and animations... more to come soon
