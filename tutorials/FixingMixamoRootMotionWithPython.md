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

## Step 3: fixing skeletal mesh bone influences

## Step 4: splitting 'Hips' track in animation

## Adding a context menu

## Final Notes
