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
