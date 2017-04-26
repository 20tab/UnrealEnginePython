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

* The Python code is pretty verbose and repeat itself constantly to show the biggest possible number of api features, i strongly suggest you to reorganize/refactor it if you plan to build something for your own pipeline

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

Create a new python script (just click `New` in the python editor, or just create a new file in your favourite editor under the `Scripts` project directory), call it kaiju_slicer_pipeline.py with the following content

```python
import unreal_engine as ue

print('Hello i am your pipeline automator')
```

![First script in editor](https://github.com/20tab/UnrealEnginePython/blob/master/tutorials/YourFirstAutomatedPipeline_Assets/first_script_editor.png)

save the script and click on 'Execute' to run the script.

If you are not using the embedded editor, you can run python scripts from the console with the py_exec command

```python
import unreal_engine as ue
ue.py_exec('name_of_script.py')
```

![First script in console](https://github.com/20tab/UnrealEnginePython/blob/master/tutorials/YourFirstAutomatedPipeline_Assets/first_script_console.png)

Finally, download the https://github.com/20tab/UnrealEnginePython/blob/master/tutorials/YourFirstAutomatedPipeline_Assets/Kaiju_Assets.zip file and unzip it in your Desktop folder. These are the original files (fbx, tga, ...) we will import in the project using a python script.

Importing the Mesh FBX
-

Let's start with importing our kaiju mesh (~/Desktop/Kaiju_Assets/Slicer/slicer.fbx) into Unreal Engine. For importing assets we need a 'Factory' (it is a UE4 class).

There is a basically a Factory for each asset type. The FbxFactory is the one dedicated at importing Fbx files.

Unfortunately, by default this factory opens a configuration wizard whenever you try to import an fbx, so an alternative class (PyFbxFactory) will be used.

It is a factory included in the UnrealUnginePython plugin, and it is a simple subclass of FbxFactory, but without the wizard. 

```python
import os.path
from unreal_engine.classes import PyFbxFactory

# instantiate a new factory
fbx_factory = PyFbxFactory()

# build the path for the fbx file
kaiju_assets_dir = os.path.join(os.path.expanduser('~/Desktop'), 'Kaiju_Assets/Slicer')

slicer_fbx = os.path.join(kaiju_assets_dir, 'slicer.fbx')

# configure the factory
fbx_factory.ImportUI.bCreatePhysicsAsset = False
fbx_factory.ImportUI.bImportMaterials = False
fbx_factory.ImportUI.bImportTextures = False
fbx_factory.ImportUI.bImportAnimations = False
# scale the mesh (the Kaiju is 30 meters high !)
fbx_factory.ImportUI.SkeletalMeshImportData.ImportUniformScale = 0.1;

# import the mesh
slicer_mesh = fbx_factory.factory_import_object(slicer_fbx, '/Game/Kaiju/Slicer')
```

Run the script, and if all goes well your Kaiju will be imported in your content browser:

![Kaiju in ContentBrowser](https://github.com/20tab/UnrealEnginePython/blob/master/tutorials/YourFirstAutomatedPipeline_Assets/slicer_mesh.png)

Scary, isn't it ?

![The Kaiju model](https://github.com/20tab/UnrealEnginePython/blob/master/tutorials/YourFirstAutomatedPipeline_Assets/slicer_without_materials.png)

Just a note about this line:

```python
fbx_factory.ImportUI.SkeletalMeshImportData.ImportUniformScale = 0.1;
```

The Kaiju original model is 30 meters high, so we scale it down to use it in a context different from "Tokyo under attack".

Creating the Materials
-

The models looks good from a geometry point of view, but you can not (on the top left of the previous screenshot) that not material is assigned to the mesh.

We are going to create two different materials, one for the blades (Element 0) and the other for the kaiju body (Element 1)

Both materials are based on `Substance Designer` textures, the second one will include the ability to 'blink' the emissive texture using a sin function/node.

Add this code to your script

```python
from unreal_engine.classes import MaterialFactoryNew

material_factory = MaterialFactoryNew()

material_blades = material_factory.factory_create_new('/Game/Kaiju/Slicer/Blades_Material')

material_body = material_factory.factory_create_new('/Game/Kaiju/Slicer/Body_Material')
```

and run it, you will end with two new assets:

![The Kaiju materials](https://github.com/20tab/UnrealEnginePython/blob/master/tutorials/YourFirstAutomatedPipeline_Assets/slicer_materials.png)

Before editing materials, we need to import the 8 required Textures (4 for the blades, 4 for the body):

```python
from unreal_engine.classes import TextureFactory

# instantiate a factory for importing textures
texture_factory = TextureFactory()
# ensures textures are overwritten (2 means, YesAll, defined in Engine/Source/Runtime/Core/Public/GenericPlatform/GenericPlatformMisc.h, EAppReturnType::YesAll)
texture_factory.OverwriteYesOrNoToAllState = 2

slicer_blade_texture_base_color_tga = os.path.join(kaiju_assets_dir, 'Textures/slicer_blade_BaseColor.tga')
slicer_blade_texture_base_color = texture_factory.factory_import_object(slicer_blade_texture_base_color_tga, '/Game/Kaiju/Slicer/Textures')

slicer_blade_texture_normal_tga = os.path.join(kaiju_assets_dir, 'Textures/slicer_blade_Normal.tga')
slicer_blade_texture_normal = texture_factory.factory_import_object(slicer_blade_texture_normal_tga, '/Game/Kaiju/Slicer/Textures')

slicer_blade_texture_emissive_tga = os.path.join(kaiju_assets_dir, 'Textures/slicer_blade_Emissive.tga')
slicer_blade_texture_emissive = texture_factory.factory_import_object(slicer_blade_texture_emissive_tga, '/Game/Kaiju/Slicer/Textures')

# orm stands for OcclusionRoughnessMetallic
slicer_blade_texture_orm_tga = os.path.join(kaiju_assets_dir, 'Textures/slicer_blade_OcclusionRoughnessMetallic.tga')
slicer_blade_texture_orm = texture_factory.factory_import_object(slicer_blade_texture_orm_tga, '/Game/Kaiju/Slicer/Textures')

slicer_texture_base_color_tga = os.path.join(kaiju_assets_dir, 'Textures/slicer_BaseColor.tga')
slicer_texture_base_color = texture_factory.factory_import_object(slicer_texture_base_color_tga, '/Game/Kaiju/Slicer/Textures')

slicer_texture_normal_tga = os.path.join(kaiju_assets_dir, 'Textures/slicer_Normal.tga')
slicer_texture_normal = texture_factory.factory_import_object(slicer_texture_normal_tga, '/Game/Kaiju/Slicer/Textures')

slicer_texture_emissive_tga = os.path.join(kaiju_assets_dir, 'Textures/slicer_Emissive.tga')
slicer_texture_emissive = texture_factory.factory_import_object(slicer_texture_emissive_tga, '/Game/Kaiju/Slicer/Textures')

# orm stands for OcclusionRoughnessMetallic
slicer_texture_orm_tga = os.path.join(kaiju_assets_dir, 'Textures/slicer_OcclusionRoughnessMetallic.tga')
slicer_texture_orm = texture_factory.factory_import_object(slicer_texture_orm_tga, '/Game/Kaiju/Slicer/Textures')
```

![The Kaiju textures](https://github.com/20tab/UnrealEnginePython/blob/master/tutorials/YourFirstAutomatedPipeline_Assets/slicer_textures.png)

The textures are ready, we can start "programming" our materials adding nodes to the graphs.

Let's start with the blades. The only two things to take into account is that the OcclusionRoughnessMetallic textures must have the sRGB flag turned off, and each of its channel will be mapped to a different channel:

* Green -> Roughness
* Blue -> Metallic
* Red -> Ambient Occlusion

```python
# setup the slicer blades material

# turn sRGB off for orm textures
slicer_blade_texture_orm.SRGB = False

from unreal_engine.classes import MaterialExpressionTextureSample
from unreal_engine.enums import EMaterialSamplerType
from unreal_engine.structs import ColorMaterialInput, VectorMaterialInput, ScalarMaterialInput

# notify the editor we are about to modify the material
material_blades.modify()

# create the BaseColor node
material_blades_base_color = MaterialExpressionTextureSample('', material_blades)
material_blades_base_color.Texture = slicer_blade_texture_base_color
material_blades_base_color.MaterialExpressionEditorX = -400
material_blades_base_color.MaterialExpressionEditorY = 0

# create the Normal node
material_blades_normal = MaterialExpressionTextureSample('', material_blades)
material_blades_normal.Texture = slicer_blade_texture_normal
# force the sampler to 'Normal'
material_blades_normal.SamplerType = EMaterialSamplerType.SAMPLERTYPE_Normal
material_blades_normal.MaterialExpressionEditorX = -400
material_blades_normal.MaterialExpressionEditorY = 200

# create the Emissive node
material_blades_emissive = MaterialExpressionTextureSample('', material_blades)
material_blades_emissive.Texture = slicer_blade_texture_emissive
material_blades_emissive.MaterialExpressionEditorX = -400
material_blades_emissive.MaterialExpressionEditorY = 400

# create the OcclusionRoughnessMetallic node
material_blades_orm = MaterialExpressionTextureSample('', material_blades)
material_blades_orm.Texture = slicer_blade_texture_orm
# force the sampler to 'LinearColor' (remember you turned off sRGB)
material_blades_orm.SamplerType = EMaterialSamplerType.SAMPLERTYPE_LinearColor
material_blades_orm.MaterialExpressionEditorX = -400
material_blades_orm.MaterialExpressionEditorY = 600

# assign nodes to the material
material_blades.Expressions = [material_blades_base_color, material_blades_normal, material_blades_emissive, material_blades_orm]

# link nodes
material_blades.BaseColor = ColorMaterialInput(Expression=material_blades_base_color)
# normals are vectors
material_blades.Normal = VectorMaterialInput(Expression=material_blades_normal)
material_blades.EmissiveColor = ColorMaterialInput(Expression=material_blades_emissive)
# use masking for orm nodes, it turns on/off specific channels, all of the items are scalars
material_blades.Roughness = ScalarMaterialInput(Expression=material_blades_orm, Mask=1, MaskG=1)
material_blades.Metallic = ScalarMaterialInput(Expression=material_blades_orm, Mask=1, MaskB=1)
material_blades.AmbientOcclusion = ScalarMaterialInput(Expression=material_blades_orm, Mask=1, MaskR=1)

# run material compilation
material_blades.post_edit_change()
```

This is what we have just generated:

![The Kaiju Blades Material](https://github.com/20tab/UnrealEnginePython/blob/master/tutorials/YourFirstAutomatedPipeline_Assets/slicer_blades_material.png)

The body material will be a bit more complex as we want to 'blink/beam' the emissive texture

```python
from unreal_engine.classes import MaterialExpressionSine, MaterialExpressionMultiply, MaterialExpressionTime
from unreal_engine.structs import ExpressionInput

# turn sRGB off for orm textures
slicer_texture_orm.SRGB = False

# inform the editor we want to modify the material
material_body.modify()

# build the material graph

# start with texture samples (as we did with the blades material)
material_body_base_color = MaterialExpressionTextureSample('', material_body)
material_body_base_color.Texture = slicer_texture_base_color
material_body_base_color.MaterialExpressionEditorX = -400
material_body_base_color.MaterialExpressionEditorY = 0

material_body_normal = MaterialExpressionTextureSample('', material_body)
material_body_normal.Texture = slicer_texture_normal
material_body_normal.SamplerType = EMaterialSamplerType.SAMPLERTYPE_Normal
material_body_normal.MaterialExpressionEditorX = -400
material_body_normal.MaterialExpressionEditorY = 200

material_body_emissive = MaterialExpressionTextureSample('', material_body)
material_body_emissive.Texture = slicer_texture_emissive
material_body_emissive.MaterialExpressionEditorX = -400
material_body_emissive.MaterialExpressionEditorY = 400

material_body_orm = MaterialExpressionTextureSample('', material_body)
material_body_orm.Texture = slicer_texture_orm
material_body_orm.SamplerType = EMaterialSamplerType.SAMPLERTYPE_LinearColor
material_body_orm.MaterialExpressionEditorX = -400
material_body_orm.MaterialExpressionEditorY = 600

# create a Sine node
material_body_sine = MaterialExpressionSine('', material_body)
material_body_sine.MaterialExpressionEditorX = -1000
material_body_sine.MaterialExpressionEditorY = 0

# create a Time node
material_body_time = MaterialExpressionTime('', material_body)
material_body_time.MaterialExpressionEditorX = -1200
material_body_time.MaterialExpressionEditorY = 0

# create a Multiply node
material_body_multiply = MaterialExpressionMultiply('', material_body)
material_body_multiply.MaterialExpressionEditorX = -800
material_body_multiply.MaterialExpressionEditorY = 0

# assign nodes to the graph
material_body.Expressions = [material_body_base_color, material_body_normal, material_body_emissive, material_body_orm, material_body_time, material_body_sine, material_body_multiply]

# connect Time to Sine Input
material_body_sine.Input = ExpressionInput(Expression=material_body_time)
# connect the Emissive Texture to the Multiply pin A
material_body_multiply.A = ExpressionInput(Expression=material_body_emissive)
# connect the Sine to the Multiply pin B
material_body_multiply.B = ExpressionInput(Expression=material_body_sine)

# link nodes 
material_body.BaseColor = ColorMaterialInput(Expression=material_body_base_color)
material_body.EmissiveColor = ColorMaterialInput(Expression=material_body_multiply)
material_body.Normal = VectorMaterialInput(Expression=material_body_normal)
material_body.Roughness = ScalarMaterialInput(Expression=material_body_orm, Mask=1, MaskG=1)
material_body.Metallic = ScalarMaterialInput(Expression=material_body_orm, Mask=1, MaskB=1)
material_body.AmbientOcclusion = ScalarMaterialInput(Expression=material_body_orm, Mask=1, MaskR=1)

# run material compilatiom
material_body.post_edit_change()
```

And this is the body material result:

![The Kaiju Body Material](https://github.com/20tab/UnrealEnginePython/blob/master/tutorials/YourFirstAutomatedPipeline_Assets/slicer_body_material.png)

one last step missing: assigning materials to the slicer mesh:

```python
from unreal_engine.structs import SkeletalMaterial

# SkeletalMaterial is a struct holding information about material mappings
# the Materials property is the array of assigned materials
slicer_mesh.Materials = [SkeletalMaterial(MaterialInterface=material_blades, MaterialSlotName='Blades'), SkeletalMaterial(MaterialInterface=material_body, MaterialSlotName='Body')]

# and save them all
slicer_mesh.save_package()
material_blades.save_package()
material_body.save_package()
```

The final result:

![The Kaiju With Material](https://github.com/20tab/UnrealEnginePython/blob/master/tutorials/YourFirstAutomatedPipeline_Assets/slicer_with_materials.png)

Importing Animations
-

Importing Animations uses the same factory for Fbx meshes.

We have 6 animations to import: idle, bored, attack, roaring, run and walk

It will be an easy task:

```python
anim_factory = PyFbxFactory()
# you need to set the skeleton before importing animations
anim_factory.ImportUI.Skeleton = slicer_mesh.Skeleton
anim_factory.ImportUI.bImportMesh = False
anim_factory.ImportUI.bImportMaterials = False
anim_factory.ImportUI.bImportTextures = False
# remember to scale them as you did with the base mesh
anim_factory.ImportUI.AnimSequenceImportData.ImportUniformScale = 0.1;

slicer_idle_fbx = os.path.join(kaiju_assets_dir, 'Animations/slicer_idle.fbx')
animation_idle = anim_factory.factory_import_object(slicer_idle_fbx, '/Game/Kaiju/Slicer/Animations')

slicer_bored_fbx = os.path.join(kaiju_assets_dir, 'Animations/slicer_bored.fbx')
animation_bored = anim_factory.factory_import_object(slicer_bored_fbx, '/Game/Kaiju/Slicer/Animations')

slicer_attack_fbx = os.path.join(kaiju_assets_dir, 'Animations/slicer_attack.fbx')
animation_attack = anim_factory.factory_import_object(slicer_attack_fbx, '/Game/Kaiju/Slicer/Animations')

slicer_roaring_fbx = os.path.join(kaiju_assets_dir, 'Animations/slicer_roaring.fbx')
animation_roaring = anim_factory.factory_import_object(slicer_roaring_fbx, '/Game/Kaiju/Slicer/Animations')

slicer_run_fbx = os.path.join(kaiju_assets_dir, 'Animations/slicer_run.fbx')
animation_run = anim_factory.factory_import_object(slicer_run_fbx, '/Game/Kaiju/Slicer/Animations')

slicer_walk_fbx = os.path.join(kaiju_assets_dir, 'Animations/slicer_walk.fbx')
animation_walk = anim_factory.factory_import_object(slicer_walk_fbx, '/Game/Kaiju/Slicer/Animations')
```

![The Kaiju With Material](https://github.com/20tab/UnrealEnginePython/blob/master/tutorials/YourFirstAutomatedPipeline_Assets/slicer_animations.png)

Now we want to create a BlendShape1D asset. It will be composed by the 3 locomotion-related animations (idle, walk, run) and it will be governed by a variable (the X) called Speed, with a minimum value of 0 and a max of 300.

```python
from unreal_engine.classes import BlendSpaceFactory1D
from unreal_engine.structs import BlendSample, BlendParameter
from unreal_engine import FVector

blend_space_factory = BlendSpaceFactory1D()
# you need to assign the related Skeleton
blend_space_factory.TargetSkeleton = slicer_mesh.Skeleton

# create the asset
slicer_locomotion = blend_space_factory.factory_create_new('/Game/Kaiju/Slicer/Animations/slicer_locomotion')

# prepare for modification
slicer_locomotion.modify()

# set blend parameters
slicer_locomotion.BlendParameters = BlendParameter(DisplayName='Speed', Min=0, Max=300, GridNum=3)

# assign animations
# 0 -> idle
# 150 -> walk
# 300 -> run
# mark them as 'valid' explicitely !
slicer_locomotion.SampleData = [BlendSample(Animation=animation_idle, SampleValue=FVector(0, 0, 0), bIsValid=True, RateScale=1), BlendSample(Animation=animation_walk, SampleValue=FVector(150, 0, 0), bIsValid=True, RateScale=1), BlendSample(Animation=animation_run, SampleValue=FVector(300, 0, 0), bIsValid=True, RateScale=1)]

# compute blend space
slicer_locomotion.post_edit_change()

# save
slicer_locomotion.save_package()
```


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

