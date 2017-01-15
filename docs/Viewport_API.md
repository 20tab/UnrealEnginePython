Working with Viewports
----------------------

The following functions are exposed by the unreal_engine module

Check https://github.com/20tab/UnrealEnginePython/blob/master/examples/pie_screenshotter.py for an example of a PIE viewport screenshot

```py
# returns a tuple of FColor objects, None if no game viewport is active
pixels = ue.get_viewport_screenshot()

# returns None if no game viewport is active
width, height = ue.get_viewport_size()

# returns a tuple of FColor objects, None if no viewport is active in the editor
pixels = ue.editor_get_active_viewport_screenshot()

# returns None if no viewport is active in the editor
width, height = ue.editor_get_active_viewport_size()

# returns a tuple of FColor objects, None if no PIE is active in the editor
pixels = ue.editor_get_pie_viewport_screenshot()

# returns None if no PIE is active in the editor
width, height = ue.editor_get_pie_viewport_size()

# trigger highres screenshot in the editor
ue.editor_take_high_res_screen_shots()
```
