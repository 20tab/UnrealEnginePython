# Fixing Mixamo Root Motion with Python

Mixamo.com is one of the biggest source for free (almost) characters animations. Unfortunately its root motion support works really differently from what Unreal Engine 4 expects.

Fixing it is a pretty heavy (and manual) task: you need to add a new bone to the skeleton, fix bone influences and update animation curves for the new bone. You can dot it from Blender, Maya or whichever tool is able to modify fbx files. (and i am pretty sure there is a UE4 plugin somewhere to do it)

The following tutorial shows how you can accomplish the same task with a simple python script.

The scripts have been tested with Unreal Engine 4.16 and python 3.6. If this is the first time you use the Unreal Engine Python plugin i strongly suggest you to start with a more gentle tutorial: https://github.com/20tab/UnrealEnginePython/blob/master/tutorials/PlottingGraphsWithMatplotlibAndUnrealEnginePython.md#installing-the-python-plugin

## Steps

Once our mixamo assets are imported we need to do the following steps:

* Add a new bone to the skeleton that will be the root of the whole tree (the bone will be called 'root')

* Modify the SkeletalMesh bone influences as indexes will be shifted after the adding of a new bone

* Split the 'Hips' related animation curve in two other curves, one containing the root motion (translations, relative to local axis origin) that will be mapped to the 'root' track, and the other mapped to the 'Hips' track that will contain only rotations. The 'root' track must be the first one.

To avoid damages, we will generate a copy of each asset, so you will be able to always use the original ones.

## Step 1: importing mixamo animations

Create a folder, and import a bunch of root-motion based animations from mixamo. (you need to download the plain fbx version)

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

The `DialogException` class is a useful trick for generating a modal window on error. This is more UE4-friendly than simply spitting out errors in the console. Whenever you raises it, the dialog will appear.

The `ue.create_modal_save_asset_dialog(title, default_package_name, default_object_name)` function opens the UE4 asset manager for allowing the user to specify where to save the SkeletalMesh/Skeleton copies. Obviously, in case of massive conversions you can avoid it and simply specify an automatic file pattern in the code.

Pay attention to the way bones are structured. They are basically referenced as numbers, with each bone mapped to a parent index (-1 for the first bone in the chain).

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

Each skeletal mesh LOD has a series of 'soft skin vertices' mapped to it. Each vertex data structure contains the classic position, tangents (included normals) and uvs values, as well as 2 additional fields for storing up to 8 'bone influences'. A bone influence is composed by a bone index (note: not the same index of the skeleton, see below) and a bone weight. Weight defines how much the bone deforms the vertex.

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
mesh.skeletal_mesh_set_bone_map([17, 22, 26, 30, ...])
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

Note the calls to `skeletal_mesh_set_active_bone_indices()` and `skeletal_mesh_set_required_bones()`: they are required to complete the build of the new mapping.

Now just add the following two lines at the end of the `add_root_to_skeleton()` method:

```python
class RootMotionFixer:

    def add_root_to_skeleton(self, mesh, bone='root'):
        ...
        # fix bone influences
        self.fix_bones_influences(new_mesh, mesh.Skeleton)

        new_mesh.save_package()
    
    ...
```

You can now delete the previously generated assets (the SeletalMesh and the Skeleton) and re-run the script (remember to select at least a Skeletal Mesh asset).

If all goes well, open the new skeleton and rotate again the left shoulder. Now it should work flawlessly:

![fixed influences](https://github.com/20tab/UnrealEnginePython/raw/master/tutorials/FixingMixamoRootMotionWithPython_Assets/fixed_influences.png)

No more arms deformation :)

## Step 4: splitting 'Hips' track in animation

The final step is fixing animations. We basically need to add a new track for the root motion, and remove translations from the track of the 'Hips' bone (remember, each bone is mapped to a different track in the animation).

We will create a new empty animation for the 'rooted' skeleton, and we will copy (and eventually fix) each track from the selected animation.

This time we will ask the user to select a skeleton asset using a slate window, but we will not ask where to save the new copy, instead we will automatically generate a name (obviously feel free to add your asset save dialog).

```python
import unreal_engine as ue
from unreal_engine.classes import SkeletalMesh, Skeleton, AnimSequence, AnimSequenceFactory
from unreal_engine import FTransform, FRawAnimSequenceTrack, FQuat
from unreal_engine import SWindow, SObjectPropertyEntryBox

class RootMotionFixer:

    ...
    def set_skeleton(self, asset_data):
        """
        This hook will be called after you selected a skeleton from the slate wizard
        """
        self.choosen_skeleton = asset_data.get_asset()
        self.window.request_destroy()

    def split_hips(self, animation, bone='Hips'):
        """
        SObjectPropertyEntryBox is the asset selector widget (we limit it to the Skeleton class)
        """
        # this will contain the user-selected skeleton (if-any)
        self.choosen_skeleton = None
        # first ask for which skeleton to use:
        self.window = SWindow(title='Choose your new Skeleton', modal=True, sizing_rule=1)(
                     SObjectPropertyEntryBox(allowed_class=Skeleton, on_object_changed=self.set_skeleton)
                 )
        # add_modal() will block the script until the user make some kind of input in the window
        self.window.add_modal()
        
        # ensure a skeleton has been selected
        if not self.choosen_skeleton:
            raise DialogException('Please specify a Skeleton for retargeting')

        # create a new empty animation from the skeleton
        factory = AnimSequenceFactory()
        factory.TargetSkeleton = self.choosen_skeleton
        
        # automatically build its new path
        base_path = animation.get_path_name()
        package_name = ue.get_path(base_path)
        object_name = ue.get_base_filename(base_path)

        new_anim = factory.factory_create_new(package_name + '/' + object_name + '_rooted')

        # copy the number of frames and duration
        new_anim.NumFrames = animation.NumFrames
        new_anim.SequenceLength = animation.SequenceLength

        # first step is generatin the 'root' track
        # we need to do it before anything else, as the 'root' track must be the 0 one
        for index, name in enumerate(animation.AnimationTrackNames):
            if name == bone:
                data = animation.get_raw_animation_track(index)
                # extract root motion
                root_motion = [(position - data.pos_keys[0]) for position in data.pos_keys]

                # create a new track (the root motion one)
                root_data = FRawAnimSequenceTrack()
                root_data.pos_keys = root_motion
                # ensure empty rotations !
                root_data.rot_keys = [FQuat()]
        
                # add  the track
                new_anim.add_new_raw_track('root', root_data)
                break
        else:
            raise DialogException('Unable to find bone {0}'.format(bone))
           
        # now append the original tracks, but removes the position keys
        # from the original root bone
        for index, name in enumerate(animation.AnimationTrackNames):
            data = animation.get_raw_animation_track(index)
            if name == bone:
                # remove root motion from original track
                data.pos_keys = [data.pos_keys[0]]
                new_anim.add_new_raw_track(name, data)
            else:
                new_anim.add_new_raw_track(name, data)

        new_anim.save_package()
```

The two 'for loops' is where the fix happens. Take into account that some animation could require additional manipulation,
as an example you may want to remove z and x transformations for a running loop:

```python
# extract root motion
root_motion = [((position - data.pos_keys[0]) * FVector(0, 1, 0)) for position in data.pos_keys]
```

And always remember that modifying the z axis in root motion, requires your Character to be in 'Flying' movement mode.

Now add support for AnimSequence in your final loop:

```python
for uobject in ue.get_selected_assets():
    if uobject.is_a(SkeletalMesh):
        root_motion_fixer.add_root_to_skeleton(uobject)
    elif uobject.is_a(AnimSequence):
        root_motion_fixer.split_hips(uobject)
    else:
        raise DialogException('Only Skeletal Meshes and Anim Sequences are supported')
```

Select an animation from the content browser and run the script.


![animation fixed](https://github.com/20tab/UnrealEnginePython/raw/master/tutorials/FixingMixamoRootMotionWithPython_Assets/animation_fixed.png)

You can downlod the full code here:

https://github.com/20tab/UnrealEnginePython/blob/master/tutorials/FixingMixamoRootMotionWithPython_Assets/mixamo.py

## Bonus step: adding a context menu

Running the script from the command line is not very handy (unless you are doing some kind of massive conversion).

It would be way cooler to execute it by right clicking the asset from the content browser. You can add a context menu extension:

```python
class RootMotionFixer:

    ...
    def run_tasks(self, selected_assets):
        # asset_data is an FAssetData instance.It is not a UObject !
        for asset_data in selected_assets:
            if asset_data.asset_class == 'SkeletalMesh':
                self.add_root_to_skeleton(asset_data.get_asset())
            elif asset_data.asset_class == 'AnimSequence':
                self.split_hips(asset_data.get_asset())
            else:
                raise DialogException('Only Skeletal Meshes and Anim Sequences are supported')

    def __call__(self, menu, selected_assets):
        menu.begin_section('mixamo', 'mixamo')
        menu.add_menu_entry('fix root motion', 'fix root motion', self.run_tasks, selected_assets)
        menu.end_section()
    
    
# add a context menu
ue.add_asset_view_context_menu_extension(RootMotionFixer())
ue.log('Mixamo Root Motion Fixer registered')
```

(remember to remove the ue.get_selected_assets() loop and the RootMotionFixer() instance from the previous code !)

The `__call__` method will be called whenever the user right-click the asset in the content browser and will generate the menu

To automatically register the mixamo module at editor startup, just import it in /Game/Scripts/ue_site.py (create it if it does not exist):

```python
import mixamo
```

## Final Notes

Always pay attention when choosing asset names (expecially if you are automatically generating them). Attempting to create a UObject with a conflicting name could lead to an editor crash.

Remember that you can open the asset-related editor by simply calling ```ue.open_editor_for_asset(uobject)```

Note that calling ```ue.add_asset_view_context_menu_extension(callable)``` will generate a new context menu every time. There is currently no support for cleaning the context menu (well, except for restarting the editor :P)

## Update 20170927 by issue 228

https://github.com/20tab/UnrealEnginePython/issues/228

Marat Yakupov reported in issue 228 a problem with the mixamo ybot model. The problem is caused by the multiple sections available in the mesh. The mixamo.py has been updated to support multiple sections:

```python
    def fix_bones_influences(self, mesh, old_skeleton):
        active_bones = []
        for section in range(0, mesh.skeletal_mesh_sections_num()):
            vertices = mesh.skeletal_mesh_get_soft_vertices(0, section)
            ue.log_warning(len(vertices))
            new_vertices = []
            old_bone_map = mesh.skeletal_mesh_get_bone_map(0, section)
            new_bone_map = []

            for vertex in vertices:
                bone_ids = list(vertex.influence_bones)
                for index, bone_id in enumerate(bone_ids):
                    if vertex.influence_weights[index] > 0:
                        bone_ids[index] = self.get_updated_bone_index(old_skeleton, mesh.Skeleton, old_bone_map, new_bone_map, bone_id)
                        if new_bone_map[bone_ids[index]] not in active_bones:
                            active_bones.append(new_bone_map[bone_ids[index]])
                vertex.influence_bones = bone_ids
                new_vertices.append(vertex)

            # assign new vertices
            mesh.skeletal_mesh_set_soft_vertices(new_vertices, 0, section)
            # add the new bone mapping
            mesh.skeletal_mesh_set_bone_map(new_bone_map, 0, section)

        # specify which bones are active and required (ensure root is added to required bones)
        mesh.skeletal_mesh_set_active_bone_indices(active_bones)
        # mark all the bones as required (eventually you can be more selective)
        mesh.skeletal_mesh_set_required_bones(list(range(0, mesh.Skeleton.skeleton_bones_get_num())))
```

As you can see we still assume a single LOD (the 0 before the section is always the lod)
