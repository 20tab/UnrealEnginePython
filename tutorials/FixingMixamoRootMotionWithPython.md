# Fixing Mixamo Root Motion with Python

Mixamo.com is one of the biggest source for free (almost) characters animations. Unfortunately its root motion support works really differently from what Unreal Engine 4 expects.

Fixing it is a pretty heavy (and manual) task: you need to add a new bone to the skeleton, fix bone influences and update animation curves for the new bone. You can dot it from Blender, Maya or whichever tool is able to modify fbx files. (and i am pretty sure there is a UE4 plugin somewhere to do it)

The following tutorial shows how you can accomplish the same task with a simple python script.

# Steps

Once our mixamo assets are imported we need to do the following steps:

* Add a new bone to the skeleton that will be the root of the whole tree (the bone will be called 'root')

* Modify the SkeletalMesh bone influences as indexes will be shifted after the adding of a new bone

* Split the 'Hips' related animation curve in two other curves, one containing the root motion (translations, relative to axis zero) that will be mapped to the 'root' track, and the other mapped to the 'Hips' track that will contain only rotations.

To avoid damages, we will generate a copy of each asset, so you will be able to always use the original assets.

# Step 1: importing mixamo animations

# Step 2: generating a new skeleton

# Step 3: fixing skeletal mesh bone influences

# Step 4: splitting 'Hips' track in animation

# Adding a context menu

# Final Notes
