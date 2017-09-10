# Snippets for Static and Skeletal Meshes

This is a collection of snippets/examples for StaticMeshes and SkeletalMeshes manipulation/creation.

Operations on animations, morph targets and LODs are included too

## Introduction

Before starting with snippets, we should introduce some convention:

Each script (unless differently specified) should be run manually from the console or the embedded python editor, and will be executed to the currently selected objects in the content browser or the world outliner.

To access the currently selected assets you use the following functions (it returns a list of UObjects):

```python
import unreal_engine as ue
uobjects = ue.get_selected_assets()
```

To access the currently selected actors in the editor world outliner:

```python
actors = ue.editor_get_selected_actors()
```

This is a screenshot of getting the name of the selected assets using python list comprehension:

![Content Browser](https://github.com/20tab/UnrealEnginePython/blob/master/tutorials/SnippetsForStaticAndSkeletalMeshes_Assets/content_browser.PNG)

![get_selected_assets](https://github.com/20tab/UnrealEnginePython/blob/master/tutorials/SnippetsForStaticAndSkeletalMeshes_Assets/get_selected_assets.PNG)

Another convention used in the snippets will be using a custom python Exception whenever an error must be returned to the user:

```python
import unreal_engine as ue

class DialogException(Exception):
    """
    Handy exception class for spawning a message dialog on error
    """
    def __init__(self, message):
        # 0 here, means "show only the Ok button", for other values
        # check https://docs.unrealengine.com/latest/INT/API/Runtime/Core/GenericPlatform/EAppMsgType__Type/index.html
        ue.message_dialog_open(0, message)
```

raising DialogException(message) will trigger something like this:

![Python Exception](https://github.com/20tab/UnrealEnginePython/blob/master/tutorials/SnippetsForStaticAndSkeletalMeshes_Assets/python_exception.PNG)

Finally, most of the scripts (for safety reasons) generate a copy of the original object. You are obviously free to update the original objects (at your risk !)


## StaticMesh: Centering pivot

One of the most annoying issues (expecially when working with non-experienced modelers) are totally meaningless pivots. Remember the pivot of a mesh is its origin axis (0X, 0Y, 0Z), nothing more nothing less.

This is a mesh with a "weird" pivot:

![Weird Pivot](https://github.com/20tab/UnrealEnginePython/blob/master/tutorials/SnippetsForStaticAndSkeletalMeshes_Assets/broken_pivot.PNG)

for fixing it, we need to get the mesh bounds (you can see them by clicking the button "Bounds" in the toolbar ot the StaticMeshEditor) and its origin and then subtract this vector to each mesh vertex (in this way the origin of the bounds will became the origin of the vertices too):

![Weird Pivot](https://github.com/20tab/UnrealEnginePython/blob/master/tutorials/SnippetsForStaticAndSkeletalMeshes_Assets/mesh_bounds.PNG)


```python
import unreal_engine as ue
from unreal_engine.classes import StaticMesh

def fix_pivot(static_mesh):
    if not static_mesh.is_a(StaticMesh):
        raise DialogException('Asset is not a StaticMesh')    
           
    # get the origin of the mesh bounds
    center = static_mesh.ExtendedBounds.Origin
    
    # get the raw data of the mesh, FRawMesh is a strcture holding vertices, uvs, tangents, material indices of a LOD (by default lod 0)
    raw_mesh = static_mesh.get_raw_mesh()   
    
    # now we create a duplicate of the mesh in the /Game/RePivoted_Meshes folder with the same name suffixed with _RePivoted
    new_asset_name = '{0}_RePivoted'.format(static_mesh.get_name())
    package_name = '/Game/RePivoted_Meshes/{0}'.format(new_asset_name)
    new_static_mesh = static_mesh.duplicate(package_name, new_asset_name)
    
    # time to fix each vertex
    updated_positions = []

    for vertex in raw_mesh.get_vertex_positions():
        updated_positions.append(vertex - center)
        # this is a trick for giving back control to Unreal Engine avoiding python to block it on slow operations
        ue.slate_tick()

    # update the FRawMesh structure with new vertices
    raw_mesh.set_vertex_positions(updated_positions)
        
    # assign the mesh data to the first LOD (LODs are exposed in SourceModels array)
    raw_mesh.save_to_static_mesh_source_model(new_static_mesh.SourceModels[0])
    # rebuild the whole mesh
    new_static_mesh.static_mesh_build()
    # re-do the body setup (collisions and friends)
    new_static_mesh.static_mesh_create_body_setup()
    # notify the editor about the changes
    new_static_mesh.post_edit_change()
    # save the new asset
    new_static_mesh.save_package()

for uobject in ue.get_selected_assets():
    fix_pivot(uobject)
```

Once the script is executed your new asset will have the pivot on its center:

![Fixed Pivot](https://github.com/20tab/UnrealEnginePython/blob/master/tutorials/SnippetsForStaticAndSkeletalMeshes_Assets/fixed_pivot.PNG)


## StaticMesh: Adding LODs

In the previous snippet we worked on the already available LOD of a StaticMesh. This time (assuming a mesh with a single LOD), we will add two new LODS. You can put any kind of mesh data in each lod (they can be completely unrelated meshes). In this example we will do something not-so-useful by setting vertex colors to a different value for each vertex).

Here we use the UE4 Mannequin skeletal mesh, converted to a static one, with both the materials slots mapped to the same "dumb" material that simply write the value of the vertex color to the fragment base color channel

![Mannequin](https://github.com/20tab/UnrealEnginePython/blob/master/tutorials/SnippetsForStaticAndSkeletalMeshes_Assets/mannequin.PNG)

![Vertex Color Material](https://github.com/20tab/UnrealEnginePython/blob/master/tutorials/SnippetsForStaticAndSkeletalMeshes_Assets/vertex_color_material.PNG)

```python
import unreal_engine as ue
from unreal_engine.classes import StaticMesh
from unreal_engine.structs import StaticMeshSourceModel, MeshBuildSettings
from unreal_engine import FColor

def generate_vertex_colors(raw_mesh, color0, color1):
    # generate a list of new vertex colors
    # iterate each face to check which material slot is in use, and generate 3 new vertex colors
    updated_colors = []
    for material_index in raw_mesh.get_face_material_indices():
        color = color0
        if material_index == 1:
            color = color1
        updated_colors.append(color)
        updated_colors.append(color)
        updated_colors.append(color)

    # update the FRawMesh structure with new vertex colors
    raw_mesh.set_wedge_colors(updated_colors)

def add_lods(static_mesh):
    if not static_mesh.is_a(StaticMesh):
        raise DialogException('Asset is not a StaticMesh')    
               
    # get the raw data of the mesh, FRawMesh is a strcture holding vertices, uvs, tangents, material indices of a LOD (by default lod 0)
    raw_mesh = static_mesh.get_raw_mesh()   
    
    # now we create a duplicate of the mesh in the /Game/ReLODed_Meshes folder with the same name suffixed with _ReLODed
    new_asset_name = '{0}_ReLODed'.format(static_mesh.get_name())
    package_name = '/Game/ReLODed_Meshes/{0}'.format(new_asset_name)
    new_static_mesh = static_mesh.duplicate(package_name, new_asset_name)
    
    
    # generate LOD1
    lod1 = StaticMeshSourceModel(BuildSettings=MeshBuildSettings(bRecomputeNormals=False, bRecomputeTangents=True, bUseMikkTSpace=True, bBuildAdjacencyBuffer=True, bRemoveDegenerates=True))
    generate_vertex_colors(raw_mesh, FColor(0, 0, 255), FColor(0, 255, 255))
    raw_mesh.save_to_static_mesh_source_model(lod1)

    # generate LOD2
    lod2 = StaticMeshSourceModel(BuildSettings=MeshBuildSettings(bRecomputeNormals=False, bRecomputeTangents=True, bUseMikkTSpace=True, bBuildAdjacencyBuffer=True, bRemoveDegenerates=True))
    generate_vertex_colors(raw_mesh, FColor(255, 0, 0), FColor(0, 255, 0))
    raw_mesh.save_to_static_mesh_source_model(lod2)
        
    # assign the new LODs (leaving the first one untouched)
    new_static_mesh.SourceModels = [new_static_mesh.SourceModels[0], lod1, lod2]
    # rebuild the whole mesh
    new_static_mesh.static_mesh_build()
    # re-do the body setup (collisions and friends)
    new_static_mesh.static_mesh_create_body_setup()
    # notify the editor about the changes
    new_static_mesh.post_edit_change()
    # save the new asset
    new_static_mesh.save_package()

for uobject in ue.get_selected_assets():
    add_lods(uobject)
```

The key concepts here are the usage of the StaticMeshSourceModel and MeshBuildSettings structures, requred to build the new LOD

The result will be the mannequin changing color based on the distance from the viewer:

![Mannequin LODs](https://github.com/20tab/UnrealEnginePython/blob/master/tutorials/SnippetsForStaticAndSkeletalMeshes_Assets/mannequin_lods.PNG)

## StaticMesh: Merging

This snippet shows how to build a new StaticMesh by combining multiple ones. It could be very handy in case of wrongly exported models, or as an optimization. The mesh_merge() function takes an optional parameter 'merge_materials' to instruct the script to generate a material slot for each mesh, or to use a single material for all of them (reducing draw calls, but very probably breaking the result).

Instead of automatically generating the asset name, a dialog will open asking for the path. Note: all of the selected Static Meshes will be merged in a single new one.

## Skeleton: Building a new tree

## Skeleton: Renaming bones

## SkeletalMesh: Bulding from static meshes

## SkeletalMesh: Merging

## SkeletalMesh: Building from Collada

## SkeletalMesh: Morph Targets

Morph Targets (or Blend Shapes), are a simple form of animation where a specific vertex (and eventually its normal) is translated to a new position. By interpolating the transition from the default position to the new one defined by the morph target, you can generate an animation. Morph Targets are common in facial animations or, more generally, whenever an object must be heavily deformed.

The following screenshots shows a face morph target going (interpolated) from 0 to 1:

![Morph0](https://github.com/20tab/UnrealEnginePython/blob/master/tutorials/SnippetsForStaticAndSkeletalMeshes_Assets/morph1.PNG)

![Morph1](https://github.com/20tab/UnrealEnginePython/blob/master/tutorials/SnippetsForStaticAndSkeletalMeshes_Assets/morph2.PNG)

```python
import unreal_engine as ue
from unreal_engine.classes import Skeleton, SkeletalMesh, MorphTarget
from unreal_engine import FTransform, FVector, FSoftSkinVertex, FMorphTargetDelta

skel = Skeleton()

root_bone_index = skel.skeleton_add_bone('root', -1, FTransform())

skel.save_package('/Game/Snippets/Triangle001_Skeleton')

vertices = []

v = FSoftSkinVertex()
v.position = FVector(0, 0, 0)
# UE4 supports up to 8 bone influences
# the influence value goes from 0 (no influence) to 255 (full influence)
v.influence_weights = (255, 0, 0, 0, 0, 0, 0)
v.influence_bones = (root_bone_index, 0, 0, 0, 0, 0, 0)

vertices.append(v)

v = FSoftSkinVertex()
v.position = FVector(100, 100, 0)
v.influence_weights = (255, 0, 0, 0, 0, 0, 0)
v.influence_bones = (root_bone_index, 0, 0, 0, 0, 0, 0)

vertices.append(v)

v = FSoftSkinVertex()
v.position = FVector(100, 0, 0)
v.influence_weights = (255, 0, 0, 0, 0, 0, 0)
v.influence_bones = (root_bone_index, 0, 0, 0, 0, 0, 0)

vertices.append(v)

mesh = SkeletalMesh()
mesh.skeletal_mesh_set_skeleton(skel)

# build a new skeletal mesh from the vertices list
mesh.skeletal_mesh_build_lod(vertices)

# a MorphTarget must have its SkeletalMesh as outer
# leaving the first string as empty will generate a unique name (often the safest choice)
morph = MorphTarget('', mesh)

# a FMorphTargetDelta structure describes how much a vertex (and/or its normal) must be translated
delta = FMorphTargetDelta()
delta.position_delta = FVector(0, 0, 100)
# this is the id of the vertex to morph, pay attention, this is the internal vertex id
# that could be different from the original one (its index in the 'vertices' list)
# to get the mapping between original and internal id, you can use skeletal_mesh_to_import_vertex_map()
delta.source_idx = 1

# always check for True return value, UE4 could decide to not
# import a morph delta if the translation is too little
if morph.morph_target_populate_deltas([delta]):
    mesh.skeletal_mesh_register_morph_target(morph)

mesh.save_package('/Game/Snippets/Triangle001_Mesh')

# open the asset editor for SkeletalMesh
ue.open_editor_for_asset(mesh)

```

## Animations: Parsing ThreeJS json models (version 3)

## Animations: Getting curves from BVH files
