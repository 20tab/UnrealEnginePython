# Snippets for Static and Skeletal Meshes

This is a collection of snippets/examples for StaticMeshes and SkeletalMeshes manipulation/creation.

Operations on animations, morph targets and LODs are included too

Code is heavily commented, most of the explanations are there !

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
    
    # get the raw data of the mesh, FRawMesh is a structure holding vertices, uvs, tangents, material indices of a LOD (by default lod 0)
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

The objective is to merge the following static meshes:

![Static Meshes to merge](https://github.com/20tab/UnrealEnginePython/blob/master/tutorials/SnippetsForStaticAndSkeletalMeshes_Assets/static_meshes_to_merge.PNG)

```python
import unreal_engine as ue
from unreal_engine import FRawMesh
from unreal_engine.classes import StaticMesh
from unreal_engine.structs import StaticMeshSourceModel, MeshBuildSettings

def mesh_merge(meshes, merge_materials=False):
    new_path = ue.create_modal_save_asset_dialog('Choose destination path', '/Game')
    package_name = ue.object_path_to_package_name(new_path)
    object_name = ue.get_base_filename(new_path)

    materials = []
    vertices = []
    texcoords = []
    normals = []
    indices = []
    material_indices = []
    for mesh in meshes:
        if not mesh.is_a(StaticMesh):
            raise DialogException('only StaticMeshes can be merged')

        raw_mesh = mesh.get_raw_mesh()
        
        # manage wedges
        original_indices = raw_mesh.get_wedge_indices()
        for i, index in enumerate(original_indices):
            original_indices[i] += len(vertices)
        indices += original_indices

        # manage materials
        original_material_indices = raw_mesh.get_face_material_indices()
        for i, index in enumerate(original_material_indices):
            if original_material_indices[i] >= 0:
                original_material_indices[i] += len(material_indices)
        material_indices += original_material_indices

        vertices += raw_mesh.get_vertex_positions()
        texcoords += raw_mesh.get_wedge_tex_coords()
        normals += raw_mesh.get_wedge_tangent_z()
        materials += mesh.StaticMaterials
        # give back control to ue to not block the interface too much
        ue.slate_tick()

    # assemble the new mesh
    sm = StaticMesh(object_name, ue.get_or_create_package(package_name))
    new_raw_mesh = FRawMesh()
    new_raw_mesh.set_vertex_positions(vertices)
    new_raw_mesh.set_wedge_indices(indices)
    new_raw_mesh.set_wedge_tex_coords(texcoords)
    new_raw_mesh.set_wedge_tangent_z(normals)
    if not merge_materials:
        new_raw_mesh.set_face_material_indices(material_indices)

    lod0 = StaticMeshSourceModel(BuildSettings=MeshBuildSettings(bRecomputeNormals=False, bRecomputeTangents=True, bUseMikkTSpace=True, bBuildAdjacencyBuffer=True, bRemoveDegenerates=True))
    
    new_raw_mesh.save_to_static_mesh_source_model(lod0)

    sm.SourceModels = [lod0]
    if merge_materials:
        materials = [materials[0]]
    sm.StaticMaterials = materials
    # rebuild the whole mesh
    sm.static_mesh_build()
    # re-do the body setup (collisions and friends)
    sm.static_mesh_create_body_setup()
    # notify the editor about the changes (not strictly required)
    sm.post_edit_change()

    sm.save_package()

    return sm
        

new_mesh = mesh_merge(ue.get_selected_assets())

ue.open_editor_for_asset(new_mesh)
    
```

![Merged](https://github.com/20tab/UnrealEnginePython/blob/master/tutorials/SnippetsForStaticAndSkeletalMeshes_Assets/mesh_merged.PNG)

In the screenshot you can note how highlighting a specific material slot will result in highlighting the specific part of the mesh (this will not work if you merge the materials, as all of the faces will map to the first material slot)

## Skeleton: Building a new tree

A Skeleton is an asset describing the tree of bones that influence a SkeletalMesh. While building a new skeleton (or adding a bone to it) is pretty easy, modifying or destroying a skeleton is always a risky operation. You should always generate a new Skeleton and the related SkeletalMesh whenever you need to change the bones tree.

In this example we create a simple skeleton:

```python
import unreal_engine as ue
from unreal_engine import FTransform, FVector, FRotator
from unreal_engine.classes import Skeleton

new_skeleton = Skeleton()

# -1 as parent means we are the root bone (THERE CAN BE ONLY ONE ROOT BONE !)
root_bone_index = new_skeleton.skeleton_add_bone('root', -1, FTransform())

# add two bones to the root one
bone_left_index = new_skeleton.skeleton_add_bone('bone_left', root_bone_index, FTransform(FVector(0, -100, 100), FRotator(0, 0, 90)))
bone_right_index = new_skeleton.skeleton_add_bone('bone_right', root_bone_index, FTransform(FVector(0, 100, 100), FRotator(0, 0, 90)))

# add another to the left one
bone_last_index = new_skeleton.skeleton_add_bone('bone_last', bone_left_index, FTransform(FVector(0, 0, 200)))

new_skeleton.save_package('/Game/CustomSkeletons/Skel001')

ue.open_editor_for_asset(new_skeleton)
```

![Bone Tree](https://github.com/20tab/UnrealEnginePython/blob/master/tutorials/SnippetsForStaticAndSkeletalMeshes_Assets/bone_tree.PNG)

Remember that you can have only one root bone (the python api will check for it, while the C++ will simply crash the editor). Each bone has its "pose" transform (mainly used for retargeting) that you specify when you create a bone

Once the Skeleton is built, the only way you can simply use it (read: without scripting) is by assigning it to a mesh with a skeleton with the same bone names. Well, a bit useless :)

Pay attention, if you manage to build an invalid skeleton, you could end in editor assert/crash. In case of emergency (your editor continues to crash), just manually delete the .uasset files containing broken skeletons from the Content/ directory

## SkeletalMesh: Bulding from static meshes

Before starting manipulating Skeletons, we want to be able to work with Skeletal Meshes, as without them, well, we cannot see our work :)

Skeletal Meshes are built by sequences of FSoftSkinVertex structs. These structures hold the classic vertex informations (positions, normals, uvs, material indices...) as well as the 'influences' of bones to each vertex. A vertex can be influenced by up to 8 bones.

In this snippet we take the UE4 cube static mesh, and we transform it to a SkeletalMesh by assigning a bone to each face of the cube.

```python
import unreal_engine as ue
from unreal_engine.classes import StaticMesh, SkeletalMesh, Skeleton
from unreal_engine import FVector, FTransform, FRotator, FSoftSkinVertex
import math

cube = ue.load_object(StaticMesh, '/Engine/BasicShapes/Cube')

skeleton = Skeleton()
# create the required bones (please always add a root bone first...)
root_bone = skeleton.skeleton_add_bone('root', -1, FTransform())
left_bone = skeleton.skeleton_add_bone('left', root_bone, FTransform(FVector(0, -50, 0), FRotator(0, 0, -90)))
right_bone = skeleton.skeleton_add_bone('right', root_bone, FTransform(FVector(0, 50, 0), FRotator(0, 0, 90)))
up_bone = skeleton.skeleton_add_bone('up', root_bone, FTransform(FVector(0, 0, 50), FRotator(0, 90, 0)))
down_bone = skeleton.skeleton_add_bone('down', root_bone, FTransform(FVector(0, 0, -50), FRotator(0, -90, 0)))
forward_bone = skeleton.skeleton_add_bone('forward', root_bone, FTransform(FVector(50, 0, 0), FRotator(0, 0, 0)))
backward_bone = skeleton.skeleton_add_bone('backward', root_bone, FTransform(FVector(-50, 0, 0), FRotator(0, 0, 180)))

skeleton.save_package('/Game/StaticToSkel/Cube_Skeleton')

mesh = SkeletalMesh()
mesh.skeletal_mesh_set_skeleton(skeleton)

# get raw data from static mesh
cube_raw = cube.get_raw_mesh()

# optimization: read the whole list of normals from the static mesh
# we will use normals to understand to which bone each vertex must be mapped
normals = cube_raw.get_wedge_tangent_z()

# the list of FSoftSkinVertex that will build the SkeletalMesh
vertices = []

# for 'wedge' UE4 means the group of infos (position, normals, uvs...) of a single vertex
for wedge in range(0, cube_raw.get_wedges_num()):
    # get the vertex normal using the wedge as the list index
    normal = normals[wedge]

    # we use the dot product to recognize the direction of the vertex/face
    dot_product_fwd = normal.dot(FVector(1, 0, 0))
    dot_product_right = normal.dot(FVector(0, 1, 0))
    dot_product_up = normal.dot(FVector(0, 0, 1))

    # create a new FSoftSkinvertex struct, we fill only the position, normal and influences
    # uvs are left to the reader :)
    v = FSoftSkinVertex()
    v.position = cube_raw.get_wedge_position(wedge)
    v.tangent_z = normal
    
    # by default a vertex has no infuences
    bone_index = 0
    influence = 0

    if math.isclose(dot_product_up, 1, abs_tol=0.0001):
        bone_index = up_bone
        influence = 255
    elif math.isclose(dot_product_up, -1, abs_tol=0.0001):
        bone_index = down_bone
        influence = 255
    elif math.isclose(dot_product_right, 1, abs_tol=0.0001):
        bone_index = right_bone
        influence = 255
    elif math.isclose(dot_product_right, -1, abs_tol=0.0001):
        bone_index = left_bone
        influence = 255
    elif math.isclose(dot_product_fwd, 1, abs_tol=0.0001):
        bone_index = forward_bone
        influence = 255
    elif math.isclose(dot_product_fwd, -1, abs_tol=0.0001):
        bone_index = backward_bone
        influence = 255

    v.influence_bones = (bone_index, 0, 0, 0, 0, 0, 0, 0)
    v.influence_weights = (influence, 0, 0, 0, 0, 0, 0, 0)

    vertices.append(v)

# build LOD0 for the StaticMesh using a list of FSoftSkinVertex
mesh.skeletal_mesh_build_lod(vertices)
mesh.save_package('/Game/StaticToSkel/Cube_Mesh')

ue.open_editor_for_asset(mesh)
```

![Cube Skel](https://github.com/20tab/UnrealEnginePython/blob/master/tutorials/SnippetsForStaticAndSkeletalMeshes_Assets/cube_skel.PNG)

Try translating/rotating each bone to see the result.

If you are confused about the dot_product part, it is a way to understand if two vectors points in the same direction. If the result of the dot product is near 1 the two vectors points (almost) in the same direction, -1 means they point to opposite directions.

In the scripts UVs are not honoured (they are all zeros). Remember a mesh can have multiple UV channels, so the get_wedge_tex_coords() method of FRawMesh returns a list of list, and the 'uvs' field of FSoftSkinVertex() expects a tuple of tuples: 

```
v.uvs = ((u0, v0), (u1, v1), ...)
```

## SkeletalMesh: Sections

Each LOD of a SkeletalMesh can be composed of multiple sections. Each section can have a different material assigned, so technically, each section costs a draw call.

Sections are automatically generated by honouring the material_index field of FSoftSkinVertex. Note: this behaviour is python-plugin specific, the C++ api works extremely differently.

We change the previous example (the skeletonized cube) to have a section for each face (so you can map a different material to each face):

```python
import unreal_engine as ue
from unreal_engine.classes import StaticMesh, SkeletalMesh, Skeleton
from unreal_engine import FVector, FTransform, FRotator, FSoftSkinVertex
from unreal_engine.structs import SkeletalMaterial, MeshUVChannelInfo
import math

cube = ue.load_object(StaticMesh, '/Engine/BasicShapes/Cube')

skeleton = Skeleton()
# create the required bones (please always add a root bone first...)
root_bone = skeleton.skeleton_add_bone('root', -1, FTransform())
left_bone = skeleton.skeleton_add_bone('left', root_bone, FTransform(FVector(0, -50, 0), FRotator(0, 0, -90)))
right_bone = skeleton.skeleton_add_bone('right', root_bone, FTransform(FVector(0, 50, 0), FRotator(0, 0, 90)))
up_bone = skeleton.skeleton_add_bone('up', root_bone, FTransform(FVector(0, 0, 50), FRotator(0, 90, 0)))
down_bone = skeleton.skeleton_add_bone('down', root_bone, FTransform(FVector(0, 0, -50), FRotator(0, -90, 0)))
forward_bone = skeleton.skeleton_add_bone('forward', root_bone, FTransform(FVector(50, 0, 0), FRotator(0, 0, 0)))
backward_bone = skeleton.skeleton_add_bone('backward', root_bone, FTransform(FVector(-50, 0, 0), FRotator(0, 0, 180)))

skeleton.save_package('/Game/StaticToSkel/Cube_Skeleton')

mesh = SkeletalMesh()
mesh.skeletal_mesh_set_skeleton(skeleton)

# get raw data from static mesh
cube_raw = cube.get_raw_mesh()

# optimization: read the whole list of normals from the static mesh
# we will use normals to understand to which bone each vertex must be mapped
normals = cube_raw.get_wedge_tangent_z()

# the list of FSoftSkinVertex that will build the SkeletalMesh
vertices = []

# for 'wedge' UE4 means the group of infos (position, normals, uvs...) of a single vertex
for wedge in range(0, cube_raw.get_wedges_num()):
    # get the vertex normal using the wedge as the list index
    normal = normals[wedge]

    # we use the dot product to recognize the direction of the vertex/face
    dot_product_fwd = normal.dot(FVector(1, 0, 0))
    dot_product_right = normal.dot(FVector(0, 1, 0))
    dot_product_up = normal.dot(FVector(0, 0, 1))

    # create a new FSoftSkinvertex struct, we fill only the position, normal and influences
    # uvs are left to the reader :)
    v = FSoftSkinVertex()
    v.position = cube_raw.get_wedge_position(wedge)
    v.tangent_z = normal
    
    # by default a vertex has no infuences
    bone_index = 0
    influence = 0
    section = 0

    if math.isclose(dot_product_up, 1, abs_tol=0.0001):
        bone_index = up_bone
        influence = 255
        section = 0
    elif math.isclose(dot_product_up, -1, abs_tol=0.0001):
        bone_index = down_bone
        influence = 255
        section = 1
    elif math.isclose(dot_product_right, 1, abs_tol=0.0001):
        bone_index = right_bone
        influence = 255
        section = 2
    elif math.isclose(dot_product_right, -1, abs_tol=0.0001):
        bone_index = left_bone
        influence = 255
        section = 3
    elif math.isclose(dot_product_fwd, 1, abs_tol=0.0001):
        bone_index = forward_bone
        influence = 255
        section = 4
    elif math.isclose(dot_product_fwd, -1, abs_tol=0.0001):
        bone_index = backward_bone
        influence = 255
        section = 5

    v.material_index = section
    v.influence_bones = (bone_index, 0, 0, 0, 0, 0, 0, 0)
    v.influence_weights = (influence, 0, 0, 0, 0, 0, 0, 0)

    vertices.append(v)

# build LOD0 for the StaticMesh using a list of FSoftSkinVertex
mesh.skeletal_mesh_build_lod(vertices)

# create material slots, UVChannelData field is required !
mesh.Materials = [
    SkeletalMaterial(MaterialSlotName='Up', UVChannelData=MeshUVChannelInfo(bInitialized=True)),
    SkeletalMaterial(MaterialSlotName='Down', UVChannelData=MeshUVChannelInfo(bInitialized=True)),
    SkeletalMaterial(MaterialSlotName='Right', UVChannelData=MeshUVChannelInfo(bInitialized=True)),
    SkeletalMaterial(MaterialSlotName='Left', UVChannelData=MeshUVChannelInfo(bInitialized=True)),
    SkeletalMaterial(MaterialSlotName='Forward', UVChannelData=MeshUVChannelInfo(bInitialized=True)),
    SkeletalMaterial(MaterialSlotName='Backward', UVChannelData=MeshUVChannelInfo(bInitialized=True)),
    ]

mesh.save_package('/Game/StaticToSkel/Cube_Mesh')

ue.open_editor_for_asset(mesh)
```

![Cube Skel Sections](https://github.com/20tab/UnrealEnginePython/blob/master/tutorials/SnippetsForStaticAndSkeletalMeshes_Assets/cube_skel_sections.PNG)

In addition to the material_index field, we create the material slots directly from the script (you can obivously do it manually), otherwise the 'sections' area in the editor will not appear until you create the related material slot.

## Skeleton: Renaming bones

As we are able now to build SkeletalMeshes, we can start renaming and manipulating bones without crashing the editor.

In this example we take the original mesh, duplicate it (by opening a wizard asking the user where to save the new asset) and we rename each bone by uppercasing it and reversing:


```python
import unreal_engine as ue
from unreal_engine.classes import Skeleton, SkeletalMesh

original_mesh = ue.get_selected_assets()[0]

if not original_mesh.is_a(SkeletalMesh):
    raise DialogException('the script only works with Skeletal Meshes')

# open a dialog for saving assets, it returns a full path
new_path = ue.create_modal_save_asset_dialog('Choose destination path', '/Game')
# get the package name part from the full path
package_name = ue.object_path_to_package_name(new_path)
# get the object name from the full path
object_name = ue.get_base_filename(new_path)

original_skeleton = original_mesh.Skeleton

# add the _Skeleton suffix to the asset, we use a package-per-asset like suggested by UE4 pipeline best-practices
new_skeleton = Skeleton('{0}_Skeleton'.format(object_name), ue.get_or_create_package('{0}_Skeleton'.format(package_name)))

# iterate each bone and rename it
for bone_index in range(0, original_skeleton.skeleton_bones_get_num()):
    # get the index of the parent bone
    parent_index = original_skeleton.skeleton_get_parent_index(bone_index)
    # get the transform of the bone
    bone_transform = original_skeleton.skeleton_get_ref_bone_pose(bone_index)
    # get the bone name
    bone_name = original_skeleton.skeleton_get_bone_name(bone_index)
    # create the new bone by uppercasing the original one and reversing it
    new_skeleton.skeleton_add_bone(bone_name.upper()[::-1], parent_index, bone_transform)

new_skeleton.save_package()

# the last True argument, allows for overwrite
new_mesh = original_mesh.duplicate(package_name, object_name, True)
# assign the new skeleton to the mesh. THIS WORKS ONLY BECAUSE THE NUMBER OF BONES IS >= than the original one
new_mesh.skeletal_mesh_set_skeleton(new_skeleton)

new_mesh.save_package()

ue.open_editor_for_asset(new_mesh)
```

![Renamed Bones](https://github.com/20tab/UnrealEnginePython/blob/master/tutorials/SnippetsForStaticAndSkeletalMeshes_Assets/renamed_bones.PNG)

The usage of the skeletal_mesh_set_skeleton(new_skeleton) method is more than enough as the number of bones in the new skeleton is equal (or higher, infact you could have added more bones to the script) to the original one.

if you need to remove bones, or more generally, you need to change the structure tree of the skeleton, you need to update the SkeletalMesh accordingly.

In the following example we reduce the skeleton to three bones (root, up, down) and we map each vertex to 'up' or 'down' based on its z value:

```python
import unreal_engine as ue
from unreal_engine.classes import Skeleton, SkeletalMesh
from unreal_engine import FTransform, FVector

original_mesh = ue.get_selected_assets()[0]

if not original_mesh.is_a(SkeletalMesh):
    raise DialogException('the script only works with Skeletal Meshes')

new_path = ue.create_modal_save_asset_dialog('Choose destination path', '/Game')
package_name = ue.object_path_to_package_name(new_path)
object_name = ue.get_base_filename(new_path)


new_skeleton = Skeleton('{0}_Skeleton'.format(object_name), ue.get_or_create_package('{0}_Skeleton'.format(package_name)))

root_bone = new_skeleton.skeleton_add_bone('root', -1, FTransform())
down_bone = new_skeleton.skeleton_add_bone('down', root_bone, FTransform(FVector(0, 0, 100)))
up_bone = new_skeleton.skeleton_add_bone('up', down_bone, FTransform(FVector(0, 0, 100)))

new_skeleton.save_package()

new_mesh = SkeletalMesh(object_name, ue.get_or_create_package(package_name))
# we can assign a skeleton safely as the mesh is empty
new_mesh.skeletal_mesh_set_skeleton(new_skeleton)

# iterate each LOD of the mesh
for lod_id in range(0, original_mesh.skeletal_mesh_lods_num()):
    # get the FSoftSkinVertex list of the whole LOD (the same structure you can pass to skeletal_mesh_build_lod())
    vertices = original_mesh.skeletal_mesh_get_lod(lod_id)
    # assign bone to vertex based on vertical position
    for v in vertices:
        if v.position.z <= 100.0:
            bone_index = down_bone
        else:
            bone_index = up_bone
            
        v.influence_bones = (bone_index, 0, 0, 0, 0, 0, 0, 0)
        v.influence_weights = (255, 0, 0, 0, 0, 0, 0, 0)

    new_mesh.skeletal_mesh_build_lod(vertices, lod_id)

# copy material slots
new_mesh.Materials = original_mesh.Materials

new_mesh.save_package()

ue.open_editor_for_asset(new_mesh)
```

![Mannequin Reskeleted](https://github.com/20tab/UnrealEnginePython/blob/master/tutorials/SnippetsForStaticAndSkeletalMeshes_Assets/mannequin_reskeleted.PNG)

## Skeleton: Sockets

Managing Sockets requires only to work with SkeletalMeshSocket class.

In this example, given a skeleton we add a socket for each bone (NOTE: this script works in place, so the original file will be modified):

```python
import unreal_engine as ue
from unreal_engine.classes import Skeleton, SkeletalMeshSocket
from unreal_engine import FVector, FRotator

skeleton = ue.get_selected_assets()[0]

current_sockets = skeleton.Sockets
new_sockets = []

if not skeleton.is_a(Skeleton):
    raise DialogException('the script only works with Skeletons')

# iterate each bone and add a random socket to it
for bone_index in range(0, skeleton.skeleton_bones_get_num()):
    bone_name = skeleton.skeleton_get_bone_name(bone_index)

    # SkeletalMeshSocket outer must be set to the related Skeleton
    new_socket = SkeletalMeshSocket('', skeleton)
    new_socket.SocketName = 'dumb_socket_for_bone_{0}'.format(bone_name)
    new_socket.BoneName = bone_name
    new_socket.RelativeLocation = FVector(17, 22, 30)
    new_socket.RelativeRotation = FRotator(0.17, 0.22, 0.30)
    new_socket.RelativeScale = FVector(1, 1, 1)
    
    new_sockets.append(new_socket)

# set the new sockets list
skeleton.Sockets = current_sockets + new_sockets

skeleton.save_package()

ue.open_editor_for_asset(skeleton)
```

![Sockets](https://github.com/20tab/UnrealEnginePython/blob/master/tutorials/SnippetsForStaticAndSkeletalMeshes_Assets/sockets.PNG)

## SkeletalMesh: Merging

This is a pretty crazy example, but will focus on a really important requirement: bone names must be unique in a skeleton

Here we take a SkeletalMesh and we "double it", paying attention to renaming each bone. The new tree will have a root, followed by two "relative roots" each with the whole bone tree. As it is pretty complex to explain, this time it is better to show the screenshot before:

![Merged Skel](https://github.com/20tab/UnrealEnginePython/blob/master/tutorials/SnippetsForStaticAndSkeletalMeshes_Assets/merged_skel.PNG)

```python
import unreal_engine as ue
from unreal_engine.classes import Skeleton, SkeletalMesh
from unreal_engine import FTransform

original_mesh = ue.get_selected_assets()[0]

if not original_mesh.is_a(SkeletalMesh):
    raise DialogException('the script only works with Skeletal Meshes')

new_path = ue.create_modal_save_asset_dialog('Choose destination path', '/Game')
package_name = ue.object_path_to_package_name(new_path)
object_name = ue.get_base_filename(new_path)

original_skeleton = original_mesh.Skeleton

new_skeleton = Skeleton('{0}_Skeleton'.format(object_name), ue.get_or_create_package('{0}_Skeleton'.format(package_name)))

root_bone = new_skeleton.skeleton_add_bone('root', -1, FTransform())

# add a whole new skeleton tree to the main one
# all of the old roots of sub-skeletons are attached to the main root
def add_tree(suffix):
    for bone_index in range(0, original_skeleton.skeleton_bones_get_num()):
        # get the bone name
        bone_name = original_skeleton.skeleton_get_bone_name(bone_index)
        # fix name
        bone_name = '{0}_{1}'.format(bone_name, suffix)
        # get the transform of the bone
        bone_transform = original_skeleton.skeleton_get_ref_bone_pose(bone_index)
        # get the index of the parent bone
        parent_index = original_skeleton.skeleton_get_parent_index(bone_index)
        # if the parent is -1, use 0, otherwise search in the current tree
        if parent_index == -1:
            parent_index = root_bone
        else:
            original_parent_name = original_skeleton.skeleton_get_bone_name(parent_index)
            parent_name = '{0}_{1}'.format(original_parent_name, suffix)
            parent_index = new_skeleton.skeleton_find_bone_index(parent_name)
        new_skeleton.skeleton_add_bone(bone_name, parent_index, bone_transform)


# add firt skeleton
add_tree('OfFirstModel')
# add the second skeleton
add_tree('OfSecondModel')

new_skeleton.save_package()

new_mesh = SkeletalMesh(object_name, ue.get_or_create_package(package_name))

new_mesh.skeletal_mesh_set_skeleton(new_skeleton)

vertices = original_mesh.skeletal_mesh_get_lod()
merged_vertices = []

# given the original index and a suffix, return the new bone index
def get_new_bone_index(index, suffix):
    bone_name = original_skeleton.skeleton_get_bone_name(index)
    # fix name
    bone_name = '{0}_{1}'.format(bone_name, suffix)
    return new_skeleton.skeleton_find_bone_index(bone_name)

# fix bones indices for the first mesh
for vertex in vertices:
    bones = list(vertex.influence_bones)
    for i in range(0, 8):
        bones[i] = get_new_bone_index(vertex.influence_bones[i], 'OfFirstModel')
    v = vertex.copy()
    v.influence_bones = bones
    merged_vertices.append(v)

# fix bones indices for the second mesh
for vertex in vertices:
    bones = list(vertex.influence_bones)
    for i in range(0, 8):
        bones[i] = get_new_bone_index(vertex.influence_bones[i], 'OfSecondModel')
    v = vertex.copy()
    v.influence_bones = bones
    merged_vertices.append(v)

new_mesh.skeletal_mesh_build_lod(merged_vertices)

new_mesh.save_package()

ue.open_editor_for_asset(new_mesh)
```

Note that all of the resulting bones will be "in use" when opening the Skeleton editor. If you want to mark only active bones, pass a list with all the valid indices to the skeletal_mesh_set_active_bones() method of a SkeletalMesh object.

## SkeletalMesh: Vertex Colors and LODs

You can have vertex colors even for skeletal meshes. Differently from StaticMesh they have to be explicitely enabled.

In this example we create a SkeletalMesh with 4 LODs, each with different vertex color values. Click on the 'Show' menu of the SkeletalMesh editor and select 'Vertex Colors' to see the colors.

```python
import unreal_engine as ue
from unreal_engine.classes import SkeletalMesh
from unreal_engine import FColor
import random

original_mesh = ue.get_selected_assets()[0]

if not original_mesh.is_a(SkeletalMesh):
    raise DialogException('the script only works with Skeletal Meshes')

new_path = ue.create_modal_save_asset_dialog('Choose destination path', '/Game')
package_name = ue.object_path_to_package_name(new_path)
object_name = ue.get_base_filename(new_path)

# get original vertex data
vertices = original_mesh.skeletal_mesh_get_lod()

# the last True argument, allows for overwrite
new_mesh = original_mesh.duplicate(package_name, object_name, True)

# inform UE4 that this SkeletalMesh has vertex colors
new_mesh.bHasVertexColors = True

# random colors (LOD 0, overwrites the original one)
for vertex in vertices:
    vertex.color = FColor(random.randrange(0, 256), random.randrange(0, 256), random.randrange(0, 256))
new_mesh.skeletal_mesh_build_lod(vertices, 0)

# blue gradient based on vertex z value (LOD 1)
for vertex in vertices:
    vertex.color = FColor(0, 0, int(vertex.position.z % 255))
new_mesh.skeletal_mesh_build_lod(vertices, 1)

# red (LOD 2)
for vertex in vertices:
    vertex.color = FColor(255, 0, 0)
new_mesh.skeletal_mesh_build_lod(vertices, 2)

# green (LOD 3)
for vertex in vertices:
    vertex.color = FColor(0, 255, 0)
new_mesh.skeletal_mesh_build_lod(vertices, 3)

new_mesh.save_package()

ue.open_editor_for_asset(new_mesh)
```

![Skel Colors](https://github.com/20tab/UnrealEnginePython/blob/master/tutorials/SnippetsForStaticAndSkeletalMeshes_Assets/skel_colors.PNG)

Note, that this example use the original skeleton for the mesh. For real-world case it is better to make a copy of the skeleton too and assign to the new mesh as seen before.

## SkeletalMesh: Building from Collada

There is already a tutorial for importing collada files as static meshes (https://github.com/20tab/UnrealEnginePython/blob/master/tutorials/WritingAColladaFactoryWithPython.md). This snippet shows how to import skeletal meshes. You can combine both to build a full-featured importer.

The main topic here is how to deal with matrices. Collada files expose bone infos as 4x4 column major matrices. FTransform objects can be built by passing them a 4x4 matrix in the UE4 convention (row-major). Collada obviously follows the OpenGL conventions (column-major), so we need to transpose all matrices. The pycollada modules returns matrices as numpy arrays, so in addition to transposing we need to flatten them (FTransform expects a simple iterator of 16 float elements).

Remember to install the pycollada module:

```
pip install pycollada
```

Here is the code, note that this time we do not save assets. All of the objects are transient (storing them is left as exercise).

Check how we need to fix UVs too, as UE4 do not use the OpenGL convention (damn, this is starting to become annoying ;) of texcoords origin on the left-bottom. More infos about switching conventions are in the below snippet about ThreeJS models.

```python
import unreal_engine as ue
from unreal_engine.classes import Skeleton, SkeletalMesh
from unreal_engine import FTransform, FSoftSkinVertex, FVector, FRotator, FQuat

from collada import Collada
import numpy

class ColladaLoader:

    def __init__(self, filename):
        self.dae = Collada(filename)
        self.controller = self.dae.controllers[0]
        # while i love quaternions, building them from euler rotations
        # looks generally easier to the reader...
        # this rotation is required for fixing the inverted Z axis on
        # bone positions. As bind pose should have rotations set to 0
        # we will apply this quaternion directly to bone positions
        self.base_quaternion = FRotator(0, 180, 180).quaternion()
        self.skeleton = self.get_skeleton()
        self.mesh = self.get_mesh(self.controller.geometry)
        
    def get_skeleton(self):
        # this mapping will allows us to fast retrieve a bone index, given its name
        # instead of relying on UE4 api
        self.bone_mapping = {}
        for scene in self.dae.scenes:
            # find the first node of type 'JOINT'
            for node in scene.nodes:
                if node.xmlnode.attrib.get('type', None) == 'JOINT':
                    self.skeleton = Skeleton()
                    self.traverse_hierarchy(node, FTransform(), -1)
                    return self.skeleton

    def traverse_hierarchy(self, node, parent_transform, parent_id):
        transform = FTransform(self.controller.joint_matrices[node.id].transpose().flatten())
        v0 = transform.translation
        q0 = transform.quaternion

        # fix axis from OpenGL to UE4 (note the quaternion multiplication that we use instead of inverting the z)
        transform.translation = FVector(v0.z, v0.x, v0.y) * self.base_quaternion
        transform.quaternion = FQuat(q0[2], q0[0] * -1, q0[1] * -1, q0[3])

        # each bone in collada is in world position (while unreal wants relative positions), we 'subtract' here the parent position
        relative_transform = transform * parent_transform.inverse()
        
        parent_id = self.skeleton.skeleton_add_bone(node.id, parent_id, relative_transform)
        # used for fast joint mapping
        self.bone_mapping[node.id] = parent_id
        for child in node.children:
            if child.xmlnode.attrib.get('type', None) == 'JOINT':
                self.traverse_hierarchy(child, transform, parent_id)

    def get_mesh(self, geometry):
        print(self.bone_mapping)
        self.mesh = SkeletalMesh()
        self.mesh.skeletal_mesh_set_skeleton(self.skeleton)
        triset = geometry.primitives[0]
        # the numpy.ravel function, completely flatten an array
        vertices = numpy.ravel(triset.vertex[triset.vertex_index])
        uvs = numpy.ravel(triset.texcoordset[0][triset.texcoord_indexset[0]])
        normals = numpy.ravel(triset.normal[triset.normal_index])
        # currently pycollada has no shortcuts for bones list
        bones = []
        weights = []
        for index in triset.vertex_index:
            influence_bones = []
            influence_weights = []
            for influence in self.controller.index[index]:
                influence_bones.append(self.bone_mapping[self.controller.weight_joints[influence[0]]])
                weight_value = int(self.controller.weights[influence[1]] * 255)
                # hack for avoiding required bones to be skipped
                # most of file weights will result in 254 instead of 255 (this is caused by float instability)
                if weight_value == 0:
                    weight_value = 1
                influence_weights.append(weight_value)
            bones.append(influence_bones)
            weights.append(influence_weights)
        soft_vertices = self.build_soft_vertices(vertices, uvs, normals, bones, weights)
        self.mesh.skeletal_mesh_build_lod(soft_vertices)
        return self.mesh
        
    def build_soft_vertices(self, vertices, uvs, normals, bones, weights):
        soft_vertices = []
        for i in range(0, len(vertices), 3):
           v = FSoftSkinVertex()
           xv, yv, zv = vertices[i], vertices[i+1], vertices[i+2]
           # invert forward
           v.position = FVector(zv * -1, xv, yv)
           xn, yn, zn = normals[i], normals[i+1], normals[i+2]
           # invert forward
           v.tangent_z = FVector(zn * -1, xn, yn)

           # get uv index
           uv_index = int(i / 3 * 2)
           # fix uvs from 0 on bottom to 0 on top
           v.uvs = [(uvs[uv_index], 1 - uvs[uv_index+1])]

           # get joint index
           joint_index = int(i / 3)
           # fix a special condition where the first bone has zero weight
           # but there are multiple influences. This is required as UE4 automatically
           # modify the first bone if weight normalization fails
           new_best_index = weights[joint_index].index(max(weights[joint_index]))
           if new_best_index > 0:
               first_bone = bones[joint_index][new_best_index]
               first_weight = weights[joint_index][new_best_index]
               bones[joint_index][new_best_index] = bones[joint_index][0]
               weights[joint_index][new_best_index] = weights[joint_index][0]
               bones[joint_index][0] = first_bone
               weights[joint_index][0] = first_weight
           v.influence_bones = bones[joint_index]
           v.influence_weights = weights[joint_index]
           soft_vertices.append(v)

        return soft_vertices

filename = ue.open_file_dialog('Choose a Collada file', '', '', 'Collada|*.dae;')[0]
loader = ColladaLoader(filename)
ue.open_editor_for_asset(loader.skeleton)
```

A note about joints management in UE4.

Unreal expects each vertex weights to be normalized. It means their sum must be 255 (maximum value for FSoftSkinVertex). If the sum
is not 255, UE4 will automatically add the required weight to the first bone influence.

This behaviour could lead to annoying errors generated by float errors (Collada exposes bone weights as float value in the range 0..1).

As an example, most of the vertices will result in a total weight of 254 (instead of 255), and (more bad), Collada does not enforce the first influence to be the one with the highest value.

For both reasons we add 1 to 0 weights (collada files does not report bone influences with a weight of 0, so we are safe) and, more important, we reorder bone influences in a way that the first one is always the one with the higher weight (more infos in the code comments):

```python
           new_best_index = weights[joint_index].index(max(weights[joint_index]))
           if new_best_index > 0:
               first_bone = bones[joint_index][new_best_index]
               first_weight = weights[joint_index][new_best_index]
               bones[joint_index][new_best_index] = bones[joint_index][0]
               weights[joint_index][new_best_index] = weights[joint_index][0]
               bones[joint_index][0] = first_bone
               weights[joint_index][0] = first_weight
           v.influence_bones = bones[joint_index]
           v.influence_weights = weights[joint_index]
```

If all goes well you wll end with your model correctly loaded (the screenshot shows a mixamo-exported model):

![Collada](https://github.com/20tab/UnrealEnginePython/blob/master/tutorials/SnippetsForStaticAndSkeletalMeshes_Assets/collada.PNG)

As you could have already noted in the comments, joints/bones matrices in collada files are world-relative, while UE4 expects each bone to be relative to the parent. For this reason we multiply each matrix for the inverse of its parent.

## SkeletalMesh: Morph Targets

Morph Targets (or Blend Shapes), are a simple form of animation where a specific vertex (and eventually its normal) is translated to a new position. By interpolating the transition from the default position to the new one defined by the morph target, you can generate an animation. Morph Targets are common in facial animations or, more generally, whenever an object must be heavily deformed.

The following screenshot shows a face morph target going (interpolated) from 0 to 1:

![Morph 0 to 1](https://github.com/20tab/UnrealEnginePython/blob/master/tutorials/SnippetsForStaticAndSkeletalMeshes_Assets/morph_0_to_1.PNG)

In the snippet we generate a simple triangle mapped to a single bone, and we assign to its vertex 1, a morph target moving the vertex 100 units up on the z axis:

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

Pay attention to the 'delta.source_idx' value as on bigger meshes it could not map to the original one (see next snippet)

![Morphed Triangle](https://github.com/20tab/UnrealEnginePython/blob/master/tutorials/SnippetsForStaticAndSkeletalMeshes_Assets/morph_target.PNG)

## Animations: Root Motion from SVG path

This funny example builds an animation from an SVG file.

The animation will only contain a motion track (for the root bone) and will set keys based on the first path curve found in a SVG file.

You can use the amazing Inkscape to draw your bezier curves:

![Bezier](https://github.com/20tab/UnrealEnginePython/blob/master/tutorials/SnippetsForStaticAndSkeletalMeshes_Assets/bezier.PNG)

To parse svg paths you need an external python module (https://pypi.python.org/pypi/svgpathtools).

```
pip install svgpathtools svgwrite numpy
```

The svgpathtools transforms SVG curves to a numpy polynomial array, so we can access point values at specific time (or gradients if you prefer):

```python
import unreal_engine as ue
from unreal_engine.classes import AnimSequenceFactory, Skeleton
from unreal_engine import FRawAnimSequenceTrack, FVector, FQuat, FRotator
from svgpathtools import svg2paths
import numpy

# use the currently selected skeleton
skeleton = ue.get_selected_assets()[0]

# open file dialog to choose the file (returns a list so we get only the first element)
paths, attributes = svg2paths(ue.open_file_dialog('Open SVG file', '', '', 'Scalable Vector Graphics|*.svg;')[0])

# an SVG file can contains multiple paths, just get the first one
path = paths[0]

# how many frame per second ?
fps = 30
# the duration of the animation
duration = 10
# scale size
scale = 3

factory = AnimSequenceFactory()
factory.TargetSkeleton = skeleton

anim = factory.factory_create_new('/Game/BezierAnimation001')
anim.NumFrames = duration * fps
anim.SequenceLength = duration
# enable root motion
anim.bEnableRootMotion = True

pos_keys = []
for t in numpy.arange(0, 1, 1.0/fps/duration):
    # get the point at specific time
    point = path.point(t)
    x, y = point.real, point.imag
    pos_keys.append(FVector(x, y, 0) * scale)

# add the track for the root bone
track = FRawAnimSequenceTrack()
track.pos_keys = pos_keys
track.rot_keys = [FQuat()]
anim.add_new_raw_track('root', track)

anim.save_package()
ue.open_editor_for_asset(anim)
```

As we have enabled root motion, once we play the animation (if yo uare using an animation blueprint remember to enable root motion there too) the character will move.

This is a debug session where the character has drawn a point under its position

![Root Motion](https://github.com/20tab/UnrealEnginePython/blob/master/tutorials/SnippetsForStaticAndSkeletalMeshes_Assets/root_motion.PNG)

You can download the example svg from here: https://raw.githubusercontent.com/20tab/UnrealEnginePython/master/tutorials/SnippetsForStaticAndSkeletalMeshes_Assets/bezier_path.svg

## Animations: Parsing ThreeJS json models (version 3)

This is a pretty big script that will import a ThreeJS json model file into Unreal Engine 4.

You are encouraged to implement it as a Factory like described here: https://github.com/20tab/UnrealEnginePython/blob/master/tutorials/WritingAColladaFactoryWithPython.md

You can download the script from here: https://raw.githubusercontent.com/20tab/UnrealEnginePython/master/tutorials/SnippetsForStaticAndSkeletalMeshes_Assets/threejs_importer.py

The example model is the classic knight.js (format 3): https://raw.githubusercontent.com/20tab/UnrealEnginePython/master/tutorials/SnippetsForStaticAndSkeletalMeshes_Assets/knight.js

Once you run the script, you will end with a SkeletalMesh with an animation and 3 Morph Targets:

![Knight](https://github.com/20tab/UnrealEnginePython/blob/master/tutorials/SnippetsForStaticAndSkeletalMeshes_Assets/knight.PNG)

![Knight Morph](https://github.com/20tab/UnrealEnginePython/blob/master/tutorials/SnippetsForStaticAndSkeletalMeshes_Assets/knight_morph.PNG)

The only relevant part in the code is the (a bit convoluted) way we move from OpenGL based conventions (y on top, x right, z forward right handed, counterwise backface culling) to the UE4 ones (z on top, x forward left handed, y right, clockwise backface culling)

This method is how we build the skeleton:

```python
    def build_skeleton(self, pkg_name, obj_name):
        pkg = ue.get_or_create_package('{0}_Skeleton'.format(pkg_name))
        skel = Skeleton('{0}_Skeleton'.format(obj_name), pkg)
        # add a root bone from which all of the others will descend 
        # (this trick will avoid generating an invalid skeleton [and a crash], as in UE4 only one root can exist)
        skel.skeleton_add_bone('root', -1, FTransform())
        # iterate bones in the json file, note that we move from opengl axis to UE4
        # (y on top, x right, z forward right-handed) to (y right, x forward left-handed, z on top)
        for bone in self.model['bones']:
            # assume no rotation
            quat = FQuat()
            # give priority to quaternions
            # remember to negate x and y axis, as we invert z on position
            if 'rotq' in bone:
                quat = FQuat(bone['rotq'][2], bone['rotq'][0] * -1,
                             bone['rotq'][1] * -1, bone['rotq'][3])
            elif 'rot' in bone:
                quat = FRotator(bone['rot'][2], bone['rot'][0] - 180
                                , bone['rot'][1] - 180).quaternion()
            pos = FVector(bone['pos'][2] * -1, bone['pos'][0],
                          bone['pos'][1]) * self.scale
            # always set parent+1 as we added the root bone before
            skel.skeleton_add_bone(
                bone['name'], bone['parent'] + 1, FTransform(pos, quat))
        skel.save_package()
        return skel
```

As you can see we change axis mapping for translations (x became y, y became z and z became inverted x), while we inverte pitch and roll for rotations. We always give priority to quaternions as they are more solid (albeit harder to grasp, the next snippet will show why they are extremely better than plain rotators/euler rotations)

Another important part is how we deal with morph targets. Internally the source_idx field of the MorphTarget object (like mentioned before) maps to an optimized index that is different from the one we used when we created the soft vertices array. Lucky enough, UE4 exposes an array that containes the mapping between the optimized index and the original one:

```python
...
# get the mapping
import_map = self.mesh.skeletal_mesh_to_import_vertex_map()

# iterate mapping
for idx, import_index in enumerate(import_map):
    # get the true index for accessing soft vertices data, use idx for source_idx
    vertex_index = original_vertices[import_index]
    ...
    morph_target.source_idx = idx
    ...
```

Finally, animations in Unreal have a fixed number of frames, while often we have only keyframes (tuple of time and value).

For this reason a common operation is to generate a new keyframe from a non linear series of keyframes. The numpy.interp() function is really handy, the following snippets show how we get a vector at specific time from a series of non-linear vector-based keyframes.

```python
    def interpolate_vector(self, timeline, t):
        keys = []
        x_values = []
        y_values = []
        z_values = []
        for key, value in timeline:
            keys.append(key)
            x_values.append(value[0])
            y_values.append(value[1])
            z_values.append(value[2])

        x = numpy.interp(t, keys, x_values)
        y = numpy.interp(t, keys, y_values)
        z = numpy.interp(t, keys, z_values)
        return FVector(x, y, z)
```

Remember, you can use the same approach with quaternions, but NOT with rotators !

## Animations: Getting curves from BVH files

BVH files are interesting for lot of reasons. First of all, BVH is basically the de-facto standard for motion capture devices. It is a textual human-readable format. And, maybe more important for this page, will heavily push your linear-algebra skills...

The BVH specs are described in this old document: https://research.cs.wisc.edu/graphics/Courses/cs-838-1999/Jeff/BVH.html

Lucky enough, here in 20tab we built a python module for parsing bvh files: https://github.com/20tab/bvh-python

```
pip install bvh
```

It is a simple parser that returns simple float lists/tuples that we can use to build vectors and quaternions.

BVH files only contain a skeleton definition (by specifying joint positions) and an animation.

This script will parse a bvh file and will return a transient skeleton (with an empty skeletal mesh associated) and an animation:

```python
import unreal_engine as ue
from unreal_engine.classes import Skeleton, SkeletalMesh, AnimSequence
from unreal_engine import FTransform, FVector, FRotator, FQuat, FRawAnimSequenceTrack

from bvh import Bvh

filename = ue.open_file_dialog('Choose your bvh file', '', '', 'Mocap BVH files|*.bvh;')[0]

with open(filename) as f:
    mocap = Bvh(f.read())

skeleton = Skeleton()

for joint_name in mocap.get_joints_names():
    offset = mocap.joint_offset(joint_name)
    parent_id = mocap.joint_parent_index(joint_name)
    # fix z value by inverting it, it will avoid left/right bone name mismatching
    position = FVector(*offset) * FVector(1, 1, -1)
    skeleton.skeleton_add_bone(joint_name, parent_id, FTransform(position))

mesh = SkeletalMesh()
mesh.skeletal_mesh_set_skeleton(skeleton)

# build an empty LOD, this is the minimal required to see bones
# in the editor
mesh.skeletal_mesh_build_lod([])

anim = AnimSequence()
anim.Skeleton = skeleton
anim.NumFrames = mocap.nframes
anim.SequenceLength = mocap.frame_time * mocap.nframes

# iterate each skeleton joint and create a track for each one
for joint_id, joint_name in enumerate(mocap.get_joints_names()):
    track = FRawAnimSequenceTrack()
    bone_transform = skeleton.skeleton_get_ref_bone_pose(joint_id)
    pos_keys = []
    rot_keys = []
    # the python bvh module allows us to choose in which order we want channels
    rotations = mocap.frames_joint_channels(joint_name, ('Xrotation', 'Yrotation', 'Zrotation'))
    positions = []

    # get root motion data for first bone
    if joint_id == 0:
        positions = mocap.frames_joint_channels(joint_name, ('Xposition', 'Yposition', 'Zposition'), 0)
   
    # quaternions do not suffer from gimbal lock as we can choose the rotation order
    for rotation in rotations:
        # BVH uses the ZXY rotation order (roll, pitch, yaw)
        roll = FRotator(rotation[0], 0, 0).quaternion()
        pitch = FRotator(0, rotation[1], 0).quaternion()
        yaw = FRotator(0, 0, rotation[2]).quaternion()
        # remember that quaternion multiplications are applied in the reverse
        q = yaw * pitch * roll

        # as we did not fix the bones axis, we rotate the root one accordingly
        if joint_id == 0:
            q = FRotator(-90, 0, 0).quaternion() * q

        rot_keys.append(q)

    # remember to always add the bind pose position to the animation track
    for position in positions:
        pos_keys.append(bone_transform.translation + FVector(position[0], position[2], position[1]))

    if not pos_keys:
        pos_keys = [bone_transform.translation]

    track.pos_keys = pos_keys
    track.rot_keys = rot_keys
    anim.add_new_raw_track(joint_name, track)

ue.open_editor_for_asset(anim)
```

Again, dealing with different convention here is the most complex part. In this example we do not swap axis, instead we rotate the root bone in the animation itelf:

![BVH](https://github.com/20tab/UnrealEnginePython/blob/master/tutorials/SnippetsForStaticAndSkeletalMeshes_Assets/bvh.PNG)


## Final Notes

The following two tutorials are good companions for the snippets:

https://github.com/20tab/UnrealEnginePython/blob/master/tutorials/FixingMixamoRootMotionWithPython.md (for fixing Mixamo root motion)

https://github.com/20tab/UnrealEnginePython/blob/master/tutorials/WritingAColladaFactoryWithPython.md (Collada StaticMesh importer)

Most of the snippets work on the currently selected asset/actors. Feel free to associate them to a context menu or an asset loader.

If you have a cool snippet that you want to share, just make a pull request !
