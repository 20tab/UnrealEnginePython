# Fixing Mixamo Root Motion with Python

Mixamo.com is one of the biggest source for free (almost) characters animations. Unfortunately its root motion support works really differently from what Unreal Engine 4 expects.

Fixing it is a pretty heavy (and manual) task: you need to add a new bone to the skeleton, fix bone influences and update animation curves for the new bone. You can dot it from Blender, Maya or whichever tool is able to modify fbx files. (and i am pretty sure there is a UE4 plugin somewhere to do it)

The following tutorial shows how you can accomplish the same task with a simple python script.

The scripts have been tested with Unreal Engine 4.16 and python 3.6. If this is the first time you use the Unreal Engine Python plugin i strongly suggest you to start with a more gentle tutorial: https://github.com/20tab/UnrealEnginePython/blob/master/tutorials/PlottingGraphsWithMatplotlibAndUnrealEnginePython.md#installing-the-python-plugin

## Steps

Once our mixamo assets are imported we need to do the following steps:

* Add a new bone to the skeleton that will be the root of the whole tree (the bone will be called 'root')

* Modify the SkeletalMesh bone influences as indexes will be shifted after the adding of a new bone

* Split the 'Hips' related animation curve in two other curves, one containing the root motion (translations, relative to local axis origin) that will be mapped to the 'root' track, and the other mapped to the 'Hips' track that will contain only rotations.

To avoid damages, we will generate a copy of each asset, so you will be able to always use the original ones.

## Step 1: importing mixamo animations

Create a folder, and import a bunch of root-motion based animations from mixamo.

In this example i have choosen the vampire as the mesh and the run, climbing and hurricane kick animations

![mixamo import](https://github.com/20tab/UnrealEnginePython/raw/master/tutorials/FixingMixamoRootMotionWithPython_Assets/mixamo.png)

if you open the 'run' animation and enable root motion you will end with this weird pose:

![broken run](https://github.com/20tab/UnrealEnginePython/raw/master/tutorials/FixingMixamoRootMotionWithPython_Assets/broken_run.png)

The reason is in how the character is rigged:

![default_skeleton](https://github.com/20tab/UnrealEnginePython/raw/master/tutorials/FixingMixamoRootMotionWithPython_Assets/default_skeleton.png)

as you can see the root of the skeleton tree is the 'Hips' joint/bone. Animations use this bone for both translations and rotations, breaking the way UE4 works. Unreal Engine expects the root of the skeleton to be at the local origin and all of the other bones/joints (weighted to vertices) below it.

So our first step will be building a new skeleton with this specific configuration.

## Step 2: generating a new skeleton

Open your favourite python editor (or the embedded one) and create the /Game/Scripts/mixamo.py file:

```python
import unreal_engine as ue
from unreal_engine.classes import SkeletalMesh, Skeleton
from unreal_engine import FTransform


class DialogException(Exception):
    """
    Handy exception class for spawning a message dialog on error
    """
    def __init__(self, message):
        ue.message_dialog_open(0, message)

class RootMotionFixer:

    def add_root_to_skeleton(self, mesh, bone='root'):
        # retrieve the mesh path, so we can give the user a handy default for the file picker
        base_path = mesh.get_path_name()
        # open the save asset dialog with handy defaults
        new_path = ue.create_modal_save_asset_dialog('Choose destination path', ue.get_path(base_path), ue.get_base_filename(base_path) + '_rooted')
        if not new_path:
            raise DialogException('Please specify a new path for the Skeletal Mesh copy')

        # build package and object names for the mesh copy
        package_name = ue.object_path_to_package_name(new_path)
        object_name = ue.get_base_filename(new_path)
        # the last True allows for overwrites
        new_mesh = mesh.duplicate(package_name, object_name, True)

        # generate a new skeleton
        new_skel = self.build_new_skeleton(mesh.Skeleton, object_name + '_Skeleton', bone)
        # save the new skeleton in the same package directory of the new skeletal mesh
        new_skel.save_package(package_name)

        # assign the new skeleton to the new mesh
        new_mesh.skeletal_mesh_set_skeleton(new_skel)

        new_skel.save_package()

    def build_new_skeleton(self, skeleton, name, root):
        """
        Generate a new skeleton with a 'root' bone
        """
        new_skel = Skeleton(name)
        # add the root bone with -1 as parent
        new_skel.skeleton_add_bone(root, -1, FTransform())

        # copy bone indices from the original skeleton
        for index in range(0, skeleton.skeleton_bones_get_num()):
            # get bone name, parent index and ref pose
            bone_name = skeleton.skeleton_get_bone_name(index)
            bone_parent = skeleton.skeleton_get_parent_index(index)
            bone_transform = skeleton.skeleton_get_ref_bone_pose(index)

            # if this is the old 'root' bone (Hips), change its parent to the new 'root'
            if bone_parent == -1:
                bone_parent_name = root
            else:
                bone_parent_name = skeleton.skeleton_get_bone_name(bone_parent)

            # get new bone parent id
            new_bone_parent = new_skel.skeleton_find_bone_index(bone_parent_name)
            # add the new bone
            new_skel.skeleton_add_bone(bone_name, new_bone_parent, bone_transform)
        return new_skel


root_motion_fixer = RootMotionFixer()

# get the list of currently selected assets in the content browser
for uobject in ue.get_selected_assets():
    # check if uobject is a SkeletlMesh
    if uobject.is_a(SkeletalMesh):
        root_motion_fixer.add_root_to_skeleton(uobject)
    else:
        raise DialogException('Only Skeletal Meshes are supported')
```

The `DialogException` class is a useful trick for generating a modal window on error. This is more UE4-friendly than simply spitting out erors in the console. Whenever you raises it, the dialog will appear.

The `ue.create_modal_save_asset_dialog(title, default_package_name, default_object_name)` function opens the UE4 asset manager for allowing the user to specify where to save the SkeletalMesh/Skeleton copies. Obviously, in case of massive conversions you can avoid it and simply specify an automatic file pattern in the code.

Pay attention to the way bones are structured. They are basically referenced as numbers, with each bone mapped to a parent index (-1 fror the first bone in the chain).

The last part of the script runs the `add_root_to_skeleton()` method for each selected asset in the content browser (but will raise an exception if one of the assets is not a Skeletal Mesh).

So, select the vampire Skeletal Mesh and run the `mixamo.py` script from the UE4 python console (or click execute in the embedded python editor):

```python
ue.py_exec('mixamo.py')
```

You will end with two new assets, one for the SkeletalMesh and another for the Skeleton.

Double click on the skeleton to open its editor and you should see the new bone hierarchy with 'root' as the parent. Now rotate the left shoulder and you should note some scary result:

![no influences](https://github.com/20tab/UnrealEnginePython/raw/master/tutorials/FixingMixamoRootMotionWithPython_Assets/no_influences.png)

## Step 3: fixing skeletal mesh bone influences

In the last screenshot we have seen a broken deformation. What happened to our vampire ?

Each skeletal mesh LOD has a series of 'soft skin vertices' mapped to it. Each vertex data structure contains the classic position, tangents (included normals) and uvs values, as well as 2 additional fields for storing up to 8 'bone influences'. A bone influence is composed by a bone index (note: not the same index of the skeleton, see below) and a bone weight. Weight define how much the bone deforms the vertex.

Changing the bone hierarchy (remember, we have added the 'root' bone), resulted in different bone indices, so each vertex is potentially pointing to the wrong bone.

In addition to this, soft skin vertices do not use the skeleton hierarchy, but point to an additional map (the BoneMap) that contains a simple array of the used bones.

A BoneMap looks like this:

```python
[17, 22, 26, 30]
```

so index 0 means bone 17, index 1 means bone 22 and so on. This additional mapping is for performance reasons, but we need to deal with it.

To retrieve the BoneMap of a skeletal mesh you can use:

```python
bone_map = mesh.skeletal_mesh_get_bone_map()
```

and to assign a new one:

```python
mesh.skeletal_mesh_get_bone_map([17, 22, 26, 30, ...])
```

In the same way you can retrieve soft skin vertices:

```python
vertices = mesh.skeletal_mesh_get_soft_vertices()
```

each vertex is a FSoftSkinVertex class with the following fields:

* position (the vertex position vector)
* color (the vertex color, can be used by materials)
* tangent_x, tangent_y, tangent_z (tangents/normal)
* uvs (texture coordinates)
* influence_bones (tuple with up to 8 BoneMap indices)
* influence_weights (bone weight as integer, 0 to 255)

Our objective is to update the influence_bones tuple for each vertex and to regenerate the BoneMap:

```python

class RootMotionFixer:

    ...
    
    def fix_bones_influences(self, mesh, old_skeleton):
        # get current soft vertices
        vertices = mesh.skeletal_mesh_get_soft_vertices()
        new_vertices = []
        # get current bone map
        old_bone_map = mesh.skeletal_mesh_get_bone_map()
        new_bone_map = []

        # fix bones influence for each vertex
        for vertex in vertices:
            bone_ids = list(vertex.influence_bones)
            for index, bone_id in enumerate(bone_ids):
                # only fix bone with weight > 0
                if vertex.influence_weights[index] > 0:
                    bone_ids[index] = self.get_updated_bone_index(old_skeleton, mesh.Skeleton, old_bone_map, new_bone_map, bone_id)
            vertex.influence_bones = bone_ids
            new_vertices.append(vertex)

        # assign new vertices
        mesh.skeletal_mesh_set_soft_vertices(new_vertices)
        # add the new bone mapping
        mesh.skeletal_mesh_set_bone_map(new_bone_map)

        # specify which bones are active and required (ensure root is added to required bones)
        mesh.skeletal_mesh_set_active_bone_indices(new_bone_map)
        if 0 not in new_bone_map:
            new_bone_map += [0]
        mesh.skeletal_mesh_set_required_bones(new_bone_map)

    def get_updated_bone_index(self, old_skeleton, new_skeleton, old_bone_map, new_bone_map, index):
        """
        Convert indices betwen skeletons
        """
        # get the skeleton bone_id from the map
        true_bone_id = old_bone_map[index]

        # get the bone name
        bone_name = old_skeleton.skeleton_get_bone_name(true_bone_id)

        # get the new index
        new_bone_id = new_skeleton.skeleton_find_bone_index(bone_name)

        # check if a new mapping is available
        if new_bone_id in new_bone_map:
            return new_bone_map.index(new_bone_id)

        new_bone_map.append(new_bone_id)
        return len(new_bone_map)-1
```

## Step 4: splitting 'Hips' track in animation

## Adding a context menu

## Final Notes
