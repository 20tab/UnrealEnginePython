The Python Console
------------------

This is a plugin module giving you a shell for running python commands. It is the main way you can script the editor itself:

![Alt text](../screenshots/unreal_screenshot4.png?raw=true "Screenshot 4")

In this screenshot you can see how to get access to the editor world, and how to add actors to it.

The console supports history

Running python scripts
----------------------

Albeit you can set your startup environment in ue_site.py or you can import module, they generally require you to restart the editor whenever you modify them.

The console allows you to run python scripts on-the-fly using the ue.exec(filename) command:

```py
import unreal_engine as ue
ue.exec('yourscript.py')
# now you can modify it, and re-run the new version
ue.exec('yourscript.py')
```

python scripts are searched from the /Content/Scripts directory

Note: on python2 ue.exec is not a valid function, you need to use ue.py_exec

Editor scripting example
------------------------

This is an example of editor scripting from the python console

```py
# get access to the editor world
editor = unreal_engine.get_editor_world()

# get the list of selected actors
selected_actors = unreal_engine.editor_get_selected_actors()

# deselect actors
unreal_engine.editor_deselect_actors()

# select an actor
unreal_engine.editor_select_actor(an_actor)

# import an asset
new_asset = unreal_engine.import_asset(filename, package[, factory_class])
```

Asset importing example
-----------------------

The following code shows how to import an asset from python (an image as a texture):

```py
import unreal_engine as ue

# get a factory class
factory = ue.find_class('TextureFactory')

# import the asset using the specified class
new_asset = ue.import_asset('C:/Users/Tester/Desktop/logo.png', '/Game/FooBar/Foo', factory)

# print the asset class (it will be a Texture2D)
ue.log(new_asset.get_class().get_name())
```

The last argument (the factory class) can be omitted, in such a case the factory will be detected by the file extension.

If you want to customize the factory (for modifying import options) you can directly generate a new factory object and set its properties:

```py
import unreal_engine as ue

factory = ue.find_class('TextureFactory')
# instantiate the factory
factory_obj = ue.new_object(factory)

# print the properties list
ue.log(str(factory_obj.properties()))

# set a property
factory_obj.set_property('bRGBToEmissive', True)

# import the asset (this time using a factory object instead of a factory class)
new_object = ue.import_asset('C:/Users/Tester/Desktop/logo.png', '/Game/FooBar/20tab', factory_obj)
```

Reimporting example
-------------------

You can reimport an object with the asset_reimport() method:

```py
uobject.asset_reimport([ask_for_new_file, show_notification, filename])
```

Assets listing example
----------------------

```py
# get the assets under the specified path
assets = unreal_engine.get_assets(path[, recursive])

# get all the assets implementing the specified class (class_name is a string, like 'SkeletalMesh')
assets = unreal_engine.get_assets_by_class(class_name[, includes_subclasses])

# get the assets currently selected in the content browser
assets = unreal_engine.get_selected_assets()
```


PyFbxFactory
------------

This is an ad-hoc factory subclassing the standard FbxFactory (many thanks to Chris Emerson for the idea). Its only objective is to disable the fbx importer dialog to allow easy scripting of fbx importing. You can set fbx importer options via the 'set_fbx_import_option()' function:

```py
import unreal_engine as ue
import os
import os.path
from unreal_engine.classes import PyFbxFactory, Skeleton
from unreal_engine.enums import EFBXImportType

"""
Scan a directory for FBX's and import them with a single skeleton
"""

# directory to scan for fbx's
fbx_directory = 'D:/FBXs'
# skeoton class to set
skeleton_name = 'UE4_Mannequin_Skeleton'

# get the skeleton reference (you could use get_asset() too)
skeleton = ue.find_object(skeleton_name)

# get the PyFbxFactory
factory = PyFbxFactory()

# setup fbx importer options
factory.ImportUI.MeshTypeToImport = EFBXImportType.FBXIT_Animation
factory.ImportUI.Skeleton = skeleton

# the fbx import procedure
def fbx_import(filename, asset_name):
    ue.log('importing ' + filename + ' to ' + asset_name)
    # import the file using PyFbxFactory
    asset = factory.factory_import_object(filename, asset_name)
    # print the skeleton name
    ue.log('FBX imported with skeleton: ' + asset.Skeleton.get_name())
    
    
# scan the directory and uatomatically assign a name using an incremental id
for _id, fbx in enumerate(os.listdir(fbx_directory)):
    if fbx.endswith('.FBX'):
        filename = os.path.join(fbx_directory, fbx)
        fbx_import(filename, '/Game/Skeletonz/Skel_{}'.format(_id))
```

Message Dialog
--------------

You can open a modal window (the Message Dialog) and wait for its response:

```py
ret = ue.message_dialog_open(ue.APP_MSG_TYPE_YES_NO, "Do you want to shot ?")
if ret == ue.APP_RETURN_TYPE_YES:
    ue.log('You choose "YES"')
```

Multiline mode
--------------

If a statement ends with ':' the python console will assume an 'incomplete' commands and will wait for the next line NOT starting with a space to issue the instructions. This mode allows you to enter functions, classes, cycles in the console.
