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
```
