Your First Automated Pipeline with UnrealEnginePython (Part 1)
=

In this tutorial i will try to show you how to build a python script that can generate
a new Unreal Engine 4 Blueprint implementing a Kaiju (a big Japanese monster) with its materials and animations. In the second part a simple AI based on Behavior Trees will be added to the Blueprint as well as a bunch of simple unit tests..

Running the script from the Unreal Engine Python Console will result in a native Blueprint (as well as meshes, animations, a blackboard and a behaviour tree graph) that does not require the python plugin to work.

Technically i am showing the "editor scripting" features of the plugin, not its "gameplay scripting" mode.

If this is the first time you use the UnrealEnginePython plugin you should take in account the following notes:

* when you see CamelCase in the python code (for attributes and function calls) it means the UE4 reflection system is being used. Albeit CamelCase for variables and functions is not 'pythonic', you should see it as a 'signal' of jumping into UE4 reflection system.

* in this tutorial i use the embedded python editor. It is pretty raw, if you want you can use your favourite editor (Vim, Sublime, Emacs...). Anything that can edit python files will be good.

* Python scripts are stored in the '/Game/Scripts' folder of the project

* The Python code is pretty verbose and repeat itself constantly to show the biggest possible number of api features, i strongly suggest you to reorganize/refactor it if you plan to build something for your own pipeline

* It has been tested with version 4.15 of the editor and python 3.6

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

Finally, download the https://github.com/20tab/UnrealEnginePython/blob/master/tutorials/YourFirstAutomatedPipeline_Assets/Kaiju_Assets.zip file and unzip it in your Desktop folder. These are the original files (fbx, tga, ...) we will import in the project using a python script. The files have been 'donated' to me by Matteo Baccaro and Giacomo Caneschi of AIV (Italian Videogame Academy, http://aiv01.it)

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

The model looks good from a geometry point of view, but you can note (on the top left of the previous screenshot) that no material is assigned to the mesh.

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

Let's start with the blades. The only two things to take into account is that the OcclusionRoughnessMetallic textures must have the sRGB flag turned off, and each of its color channel will be mapped to a different material component:

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

# run material compilation
material_body.post_edit_change()
```

And this is the body material result:

![The Kaiju Body Material](https://github.com/20tab/UnrealEnginePython/blob/master/tutorials/YourFirstAutomatedPipeline_Assets/slicer_body_material.png)

one last step missing: assigning materials to the slicer mesh:

```python
from unreal_engine.structs import SkeletalMaterial, MeshUVChannelInfo

# SkeletalMaterial is a struct holding information about material mappings
# the Materials property is the array of assigned materials (setting UVChannelData is required, otherwise you will get assertions)
slicer_mesh.Materials = [SkeletalMaterial(MaterialInterface=material_blades, MaterialSlotName='Blades', UVChannelData=MeshUVChannelInfo(bInitialized=True)),SkeletalMaterial(MaterialInterface=material_body, MaterialSlotName='Body', UVChannelData=MeshUVChannelInfo(bInitialized=True))]

# and save them all
slicer_mesh.save_package()
material_blades.save_package()
material_body.save_package()
```

The final result:

![The Kaiju With Material](https://github.com/20tab/UnrealEnginePython/blob/master/tutorials/YourFirstAutomatedPipeline_Assets/slicer_with_materials.png)

Importing Animations and creating a new monodimensional BlendSpace for locomotion
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

Now we want to create a BlendSpace1D asset. It will be composed by the 3 locomotion-related animations (idle, walk, run) and it will be governed by a variable (the X) called Speed, with a minimum value of 0 and a max of 300.

```python
from unreal_engine.classes import BlendSpaceFactory1D
from unreal_engine.structs import BlendSample, BlendParameter
from unreal_engine import FVector

blend_space_factory = BlendSpaceFactory1D()
# you need to assign the related Skeleton
blend_space_factory.TargetSkeleton = slicer_mesh.Skeleton

# create the asset
slicer_locomotion = blend_space_factory.factory_create_new('/Game/Kaiju/Slicer/Animations/slicer_locomotion')

ue.open_editor_for_asset(slicer_locomotion)

# set blend parameters
slicer_locomotion.modify()
slicer_locomotion.BlendParameters = BlendParameter(DisplayName='Speed', Min=0, Max=300, GridNum=2)

# assign animations
# 0 -> idle
# 150 -> walk
# 300 -> run
# mark them as 'valid' explicitely !
slicer_locomotion.SampleData = [BlendSample(Animation=animation_idle, SampleValue=FVector(0, 0, 0), bIsValid=True, RateScale=1), BlendSample(Animation=animation_walk, SampleValue=FVector(150, 0, 0), bIsValid=True, RateScale=1), BlendSample(Animation=animation_run, SampleValue=FVector(300, 0, 0), bIsValid=True, RateScale=1)]

# compute blend space and update the editor preview
slicer_locomotion.post_edit_change()

slicer_locomotion.save_package()

ue.close_editor_for_asset(slicer_locomotion)
```

Note the ue.open_editor_for_asset(slicer_locomotion) and ue.close_editor_for_asset(slicer_locomotion) calls. They allow the blend space to be rebuilt after modifications (there is no api out of the slate interface to do it currently in UE4)


![The Kaiju Locomotion BlendSpace](https://github.com/20tab/UnrealEnginePython/blob/master/tutorials/YourFirstAutomatedPipeline_Assets/slicer_locomotion.png)

Creating the AnimationBlueprint
-

Our animation assets are ready, so we can start creating the Animation blueprint.

It will contain a state machine switching between:

* Locomotion (the blend space we created before)

* Attack (attack with blades or a taunting roar happining randomly 30% of the times)

* Bored (when idle for more than 10 seconds, starts looking around)

Its event graph manages the Speed variable for the blend space and the idle timer triggering the 'Bored' state.

```python
from unreal_engine.classes import AnimBlueprintFactory


# ensure no blueprint with the same name exists
# find_asset() returns None if the asset does not exist
anim_bp = ue.find_asset('/Game/Kaiju/Slicer/slicer_AnimBP.slicer_AnimBP')
if anim_bp:    
    ue.delete_asset(anim_bp.get_path_name())

anim_bp_factory = AnimBlueprintFactory()
anim_bp_factory.TargetSkeleton = slicer_mesh.Skeleton

anim_bp = anim_bp_factory.factory_create_new('/Game/Kaiju/Slicer/slicer_AnimBP')
```

Always take into account that creating a Blueprint (of any kind)  with a name of an existing one, will trigger an error.
If ue.find_asset() succeeds, we destroy the already existing asset. 

Once the blueprint is created, we need to assign a custom event 'DoAttack' (triggered when the Kaiju is near a Pawn), anpther one called 'Boring' (triggered when the kaiju is stationary/idle for more than 10 seconds), 2 bool variables (Attack, Bored) and a float one (Speed):

```python
# DoAttack custom event
node_do_attack = anim_bp.UberGraphPages[0].graph_add_node_custom_event('DoAttack', 0, -200)

# Boring custom event
node_boring = anim_bp.UberGraphPages[0].graph_add_node_custom_event('Boring', 0, -400)

# bool variables
ue.blueprint_add_member_variable(anim_bp, 'Attack', 'bool')
ue.blueprint_add_member_variable(anim_bp, 'Bored', 'bool')

# float variable
ue.blueprint_add_member_variable(anim_bp, 'Speed', 'float')
```

![The Kaiju Animation Blueprint](https://github.com/20tab/UnrealEnginePython/blob/master/tutorials/YourFirstAutomatedPipeline_Assets/slicer_anim_blueprint.png)

We can now add the graph nodes:

```python
# add nodes to the graph
from unreal_engine.classes import KismetSystemLibrary, KismetMathLibrary

# set Attack to True
node_set_attack001 = anim_bp.UberGraphPages[0].graph_add_node_variable_set('Attack', None, 300, -200)
node_set_attack001.node_find_pin('Attack').default_value = 'true'

# Delay of 2 seconds (cooldown)
from unreal_engine.classes import KismetSystemLibrary
node_delay_2_seconds = anim_bp.UberGraphPages[0].graph_add_node_call_function(KismetSystemLibrary.Delay, 600, -200)
node_delay_2_seconds.node_find_pin('Duration').default_value = '2.0'

# set Attack to False
node_set_attack002 = anim_bp.UberGraphPages[0].graph_add_node_variable_set('Attack', None, 900, -200)
node_set_attack002.node_find_pin('Attack').default_value = 'false'

# set Bored to True
node_set_bored001 = anim_bp.UberGraphPages[0].graph_add_node_variable_set('Bored', None, 300, -400)
node_set_bored001.node_find_pin('Bored').default_value = 'true'

# Delay of 5 seconds (cooldown)
from unreal_engine.classes import KismetSystemLibrary
node_delay_5_seconds = anim_bp.UberGraphPages[0].graph_add_node_call_function(KismetSystemLibrary.Delay, 600, -400)
node_delay_5_seconds.node_find_pin('Duration').default_value = '5.0'

# set Attack to False
node_set_bored002 = anim_bp.UberGraphPages[0].graph_add_node_variable_set('Bored', None, 900, -400)
node_set_bored002.node_find_pin('Bored').default_value = 'false'
```

... and link them

```python
# link nodes

# DoAttack to Set Attack True
node_do_attack.node_find_pin('then').make_link_to(node_set_attack001.node_find_pin('execute'))
# Set Attack True to Delay
node_set_attack001.node_find_pin('then').make_link_to(node_delay_2_seconds.node_find_pin('execute'))
# Delay to Set Attack False
node_delay_2_seconds.node_find_pin('then').make_link_to(node_set_attack002.node_find_pin('execute'))

# Boring to Set Bored True
node_boring.node_find_pin('then').make_link_to(node_set_bored001.node_find_pin('execute'))
# Set Attack True to Delay
node_set_bored001.node_find_pin('then').make_link_to(node_delay_5_seconds.node_find_pin('execute'))
# Delay to Set Attack False
node_delay_5_seconds.node_find_pin('then').make_link_to(node_set_bored002.node_find_pin('execute'))
```

![The Kaiju Animation Blueprint with Nodes](https://github.com/20tab/UnrealEnginePython/blob/master/tutorials/YourFirstAutomatedPipeline_Assets/slicer_anim_blueprint_with_nodes.png)

The last piece missing for the event graph is assigning speed to the actor velocity.

As you can see the 'BlueprintUpdateAnimation' event and the 'TryGetPawnOwner' pure function are automatically added to the animation graph. For this reason we need to commodity functions to find a reference to them:

```python
from unreal_engine.classes import K2Node_Event, K2Node_CallFunction, Actor, AnimInstance

def find_event_node(graph, name):
    for node in graph.Nodes:
        if node.is_a(K2Node_Event):
            if node.EventReference.MemberName == name:
                return node
    return None
    
def find_function_node(graph, name):
    for node in graph.Nodes:
        if node.is_a(K2Node_CallFunction):
            if node.FunctionReference.MemberName == name:
                return node
    return None

update_animation_event = find_event_node(anim_bp.UberGraphPages[0], 'BlueprintUpdateAnimation')
try_get_pawn_owner = find_function_node(anim_bp.UberGraphPages[0], 'TryGetPawnOwner')
# weird enough, sometimes the TryGetPawnOwner node is not automatically added ... (is it a race condition ?)
if not try_get_pawn_owner:
    try_get_pawn_owner = anim_bp.UberGraphPages[0].graph_add_node_call_function(AnimInstance.TryGetPawnOwner, 0, 200)
```

We can now add the 'GetVelocity' node and the 'VectorLength' one. Its return value will be stored into the Speed variable

```python
node_get_velocity = anim_bp.UberGraphPages[0].graph_add_node_call_function(Actor.GetVelocity, 300, 200)
# note the functions is called VSize albeit the graph editor reports it as VectorLength
node_vector_length = anim_bp.UberGraphPages[0].graph_add_node_call_function(KismetMathLibrary.VSize, 600, 200)
node_speed_set = anim_bp.UberGraphPages[0].graph_add_node_variable_set('Speed', None, 900, 0)


#link nodes

# BlueprintUpdateAnimation to Speed Set
update_animation_event.node_find_pin('then').make_link_to(node_speed_set.node_find_pin('execute'))

# TryGetPawnOwner to GetVelocity
try_get_pawn_owner.node_find_pin('ReturnValue').make_link_to(node_get_velocity.node_find_pin('self'))

# GetVelocity to VectorLength
node_get_velocity.node_find_pin('ReturnValue').make_link_to(node_vector_length.node_find_pin('A'))

# VectorLength to Speed Set
node_vector_length.node_find_pin('ReturnValue').make_link_to(node_speed_set.node_find_pin('Speed'))
```

![The Kaiju Animation Blueprint Event Graph](https://github.com/20tab/UnrealEnginePython/blob/master/tutorials/YourFirstAutomatedPipeline_Assets/slicer_anim_blueprint_event_graph.png)

We have done with the event graph, we can now move to the AnimGraph and define a State Machine

```python
# create the State Machine
from unreal_engine.classes import AnimGraphNode_StateMachine

state_machine = anim_bp.FunctionGraphs[0].graph_add_node(AnimGraphNode_StateMachine, -300, 0)
# assign a name to the state machine
state_machine.EditorStateMachineGraph.set_name('Slicer State Machine')

# connect the state machine to the first node of the anim graph
state_machine.node_find_pin('Pose').make_link_to(anim_bp.FunctionGraphs[0].Nodes[0].node_find_pin('Result'))
```

![The Kaiju Animation Blueprint State Machine](https://github.com/20tab/UnrealEnginePython/blob/master/tutorials/YourFirstAutomatedPipeline_Assets/slicer_state_machine.png)

(the Warning you get is because the state machine is empty)

Time to add 'States':

```python
# adding states
from unreal_engine.classes import AnimStateNode

# Locomotion
locomotion_state = state_machine.EditorStateMachineGraph.graph_add_node(AnimStateNode, 200, 0)
locomotion_state.BoundGraph.set_name('Locomotion')

# Attack
attack_state = state_machine.EditorStateMachineGraph.graph_add_node(AnimStateNode, 400, -150)
attack_state.BoundGraph.set_name('Attack')

# Bored
bored_state = state_machine.EditorStateMachineGraph.graph_add_node(AnimStateNode, 400, 150)
bored_state.BoundGraph.set_name('Bored')
```

![The Kaiju Animation Blueprint States](https://github.com/20tab/UnrealEnginePython/blob/master/tutorials/YourFirstAutomatedPipeline_Assets/slicer_states.png)


We start with the 'Bored' state, as it is a simple one (just play an animation looply)

```python
# The Bored State -> simply play an animation
from unreal_engine.classes import AnimGraphNode_SequencePlayer
from unreal_engine.structs import AnimNode_SequencePlayer

bored_player = bored_state.BoundGraph.graph_add_node(AnimGraphNode_SequencePlayer, -300, 0)
bored_player.Node = AnimNode_SequencePlayer(Sequence=animation_bored, bLoopAnimation=True)
bored_player.node_find_pin('Pose').make_link_to(bored_state.BoundGraph.Nodes[0].node_find_pin('Result'))
```

![The Kaiju Animation Blueprint State Bored](https://github.com/20tab/UnrealEnginePython/blob/master/tutorials/YourFirstAutomatedPipeline_Assets/slicer_states_bored.png)

Next is the 'Locomotion' state, it is a simple play of the blend space governed by the 'Speed' vairable:

```python
# The Locomotion State -> play the blendspace driven by the 'Speed' variable
from unreal_engine.classes import AnimGraphNode_BlendSpacePlayer
from unreal_engine.structs import AnimNode_BlendSpacePlayer

locomotion_player = locomotion_state.BoundGraph.graph_add_node(AnimGraphNode_BlendSpacePlayer, -300, 0)
locomotion_player.Node = AnimNode_BlendSpacePlayer(BlendSpace=slicer_locomotion, bLoop=True)

locomotion_speed_get = locomotion_state.BoundGraph.graph_add_node_variable_get('Speed', None, -500, 0)

# link nodes
locomotion_speed_get.node_find_pin('Speed').make_link_to(locomotion_player.node_find_pin('X'))
locomotion_player.node_find_pin('Pose').make_link_to(locomotion_state.BoundGraph.Nodes[0].node_find_pin('Result'))
```

![The Kaiju Animation Blueprint State Locomotion](https://github.com/20tab/UnrealEnginePython/blob/master/tutorials/YourFirstAutomatedPipeline_Assets/slicer_states_locomotion.png)

The last state is the 'Attack' one, it uses a random player, playing 70% of the time the attack animation, and 30% fallbacks to a taunt (the roaring animation)

```python
# The Attack State -> 70% of the time it will run the blades animation, the other time will run the roar one (like a taunt)
from unreal_engine.classes import AnimGraphNode_RandomPlayer
from unreal_engine.structs import AnimNode_RandomPlayer, RandomPlayerSequenceEntry

attack_player = attack_state.BoundGraph.graph_add_node(AnimGraphNode_RandomPlayer, -300, 0)
attack_player.Node = AnimNode_RandomPlayer(Entries=[RandomPlayerSequenceEntry(Sequence=animation_attack, ChanceToPlay=0.7), RandomPlayerSequenceEntry(Sequence=animation_roaring, ChanceToPlay=0.3)])

attack_player.node_find_pin('Pose').make_link_to(attack_state.BoundGraph.Nodes[0].node_find_pin('Result'))
```

![The Kaiju Animation Blueprint State Attack](https://github.com/20tab/UnrealEnginePython/blob/master/tutorials/YourFirstAutomatedPipeline_Assets/slicer_states_attack.png)

Now we can make connections, note that we use the 'connect()' method of the pin instead of 'make_link_to' as 'connect' will honour the node features (for state node it automatically generates a condition graph)

```python
# time to make states connections
entry_state = state_machine.EditorStateMachineGraph.Nodes[0]

# Entry to Locomotion
entry_state.node_find_pin('Entry').connect(locomotion_state.node_find_pin('In'))
# Locomotion to Bored
locomotion_state.node_find_pin('Out').connect(bored_state.node_find_pin('In'))
# Bored to Locomotion
bored_state.node_find_pin('Out').connect(locomotion_state.node_find_pin('In'))
# Locomotion to Attack
locomotion_state.node_find_pin('Out').connect(attack_state.node_find_pin('In'))
# Attack to Locomotion
attack_state.node_find_pin('Out').connect(locomotion_state.node_find_pin('In'))
```

![The Kaiju Animation Blueprint State Machine Connections](https://github.com/20tab/UnrealEnginePython/blob/master/tutorials/YourFirstAutomatedPipeline_Assets/slicer_state_machine_connections.png)

Once the connections are made, we can define the transition graphs. Unfortunately the current binary release of UnrealEnginePython does not contain the node_get_title() method, so we need to reference the transition graphs by their id. Lucky enough, they are indexed in the order they are created:

```python
# get transitions graphs
locomotion_to_bored = state_machine.EditorStateMachineGraph.SubGraphs[3]
bored_to_locomotion = state_machine.EditorStateMachineGraph.SubGraphs[4]
locomotion_to_attack = state_machine.EditorStateMachineGraph.SubGraphs[5]
attack_to_locomotion = state_machine.EditorStateMachineGraph.SubGraphs[6]

# locomotion to bored
check_bored = locomotion_to_bored.graph_add_node_variable_get('Bored', None, -200, 0)
check_bored.node_find_pin('Bored').make_link_to(locomotion_to_bored.Nodes[0].node_find_pin('bCanEnterTransition'))

# bored to locomotion
check_not_bored = bored_to_locomotion.graph_add_node_variable_get('Bored', None, -300, 0)
check_not_bored_not_bool = bored_to_locomotion.graph_add_node_call_function(KismetMathLibrary.Not_PreBool, -150, 0)
check_not_bored.node_find_pin('Bored').make_link_to(check_not_bored_not_bool.node_find_pin('A'))
check_not_bored_not_bool.node_find_pin('ReturnValue').make_link_to(bored_to_locomotion.Nodes[0].node_find_pin('bCanEnterTransition'))

# locomotion to attack
check_attack = locomotion_to_attack.graph_add_node_variable_get('Attack', None, -200, 0)
check_attack.node_find_pin('Attack').make_link_to(locomotion_to_attack.Nodes[0].node_find_pin('bCanEnterTransition'))

# attack to locomotion
check_not_attack = attack_to_locomotion.graph_add_node_variable_get('Attack', None, -300, 0)
check_not_attack_not_bool = attack_to_locomotion.graph_add_node_call_function(KismetMathLibrary.Not_PreBool, -150, 0)
check_not_attack.node_find_pin('Attack').make_link_to(check_not_attack_not_bool.node_find_pin('A'))
check_not_attack_not_bool.node_find_pin('ReturnValue').make_link_to(attack_to_locomotion.Nodes[0].node_find_pin('bCanEnterTransition'))
```

![The Kaiju Animation Blueprint State Machine Transitions](https://github.com/20tab/UnrealEnginePython/blob/master/tutorials/YourFirstAutomatedPipeline_Assets/slicer_state_machine_transitions.png)

FInally we can compile the Animation Blueprint and save it

```python
# compile and save
ue.compile_blueprint(anim_bp)
anim_bp.save_package()
```

Put it all in a new Blueprint
-

Now it is time to create The Character Blueprint at which we will attach the previously created assets.

```python
from unreal_engine.classes import Character

# always check for already existing blueprints
slicer_bp = ue.find_asset('/Game/Kaiju/Slicer/slicer_Blueprint.slicer_Blueprint')
if slicer_bp:
    ue.delete_asset(slicer_bp.get_path_name())
    
slicer_bp = ue.create_blueprint(Character, '/Game/Kaiju/Slicer/slicer_Blueprint')
```

time to assign values:

```python

# configure the capsule
slicer_bp.GeneratedClass.get_cdo().CapsuleComponent.CapsuleHalfHeight = 150
slicer_bp.GeneratedClass.get_cdo().CapsuleComponent.CapsuleRadius = 60

# assign the the skeletal mesh and fix its relative position
slicer_bp.GeneratedClass.get_cdo().Mesh.SkeletalMesh = slicer_mesh
slicer_bp.GeneratedClass.get_cdo().Mesh.RelativeLocation = FVector(10, -3, -144)

# assign the animation blueprint
slicer_bp.GeneratedClass.get_cdo().Mesh.AnimClass = anim_bp.GeneratedClass
```

the get_cdo() method returns the 'Class Default Object', it is a special instance of a class defining the default properties and components that following instances should inherit. When you edit a blueprint class you can effectively editing its 'cdo'.

We can now complete the first part of the tutorial by compiling the blueprint:

```python
ue.compile_blueprint(slicer_bp)
slicer_bp.save_package()
```

![The Kaiju Blueprint](https://github.com/20tab/UnrealEnginePython/blob/master/tutorials/YourFirstAutomatedPipeline_Assets/slicer_blueprint.png)

Final notes
-

You can get the whole script here: https://github.com/20tab/UnrealEnginePython/blob/master/tutorials/YourFirstAutomatedPipeline_Assets/kaiju_slicer_pipeline.py

You can now drag and drop the blueprint in your scene or subclass it.

You can eventually spawn it directly from python:

```python
# get a reference to the editor world
world = ue.get_editor_world()
world.actor_spawn(slicer_bp.GeneratedClass, FVector(17, 22, 30))
```

In part2 we will see how to give a 'brain' to our Kaiju.
