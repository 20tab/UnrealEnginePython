The Python Console
------------------

This is a plugin module giving you a shell for running python commands. It is the main way you can script the editor itself:

![Alt text](screenshots/unreal_screenshot4.png?raw=true "Screenshot 4")

In this screenshot you can see how to get access to the editor world, and how to add actors to it.

The console is still in early stage of development (it is really raw, do not expected a good user experience)

Editor scripting
----------------

The following functions allows access to editor internals:

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

Asset importing
---------------

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

Message Dialog
--------------

You can open a modal window (the Message Dialog) and wait for its response:

```py
ret = message_dialog_open(ue.APP_MSG_TYPE_YES_NO, "Do you want to shot ?")
if ret == ue.APP_RETURN_TYPE_YES:
    ue.log('You choose "YES"')
```
