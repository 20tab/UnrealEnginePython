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


## StaticMesh: Centering pivot

One of the most annoying issues (expecially when working with non-experienced modelers) are totally meaningless pivots. Remember the pivot of a mesh is its origin axis (0X, 0Y, 0Z), nothing more nothing less.

This is a mesh with a "weird" pivot:

![Weird Pivot](https://github.com/20tab/UnrealEnginePython/blob/master/tutorials/SnippetsForStaticAndSkeletalMeshes_Assets/broken_pivot.PNG)

for fixing it, we need to get the mesh bounds (you can see them by clicking the button "Bounds" in the toolbar ot the StaticMeshEditor) and its origin and then subtract this vector to each mesh vertex (in this way the origin of the bounds will became the origin of the vertices too):

![Weird Pivot](https://github.com/20tab/UnrealEnginePython/blob/master/tutorials/SnippetsForStaticAndSkeletalMeshes_Assets/mesh_bounds.PNG)


```python
import unreal_engine as ue
from unreal_engine.classes import StaticMesh
from unreal_engine.structs import StaticMeshSourceModel, MeshBuildSettings

def fix_pivot(static_mesh):
    if not static_mesh.is_a(StaticMesh):
        raise DialogException('Asset is not a StaticMesh')
        
    # get the package name of the asset (will be used for duplication in the same folder)
    package_name = static_mesh.get_outermost().get_name()
    new_asset_name = '{0}_RePivoted'.format(static_mesg.get_name())
        
    # get the origin of the mesh bounds
    center = static_mesh.ExtendedBounds.Origin
    
    # get the raw data of the mesh, FRawMesh is a strcture holding vertices, uvs, tangents, material indices of a LOD (by default lod 0)
    raw_mesh = sm.get_raw_mesh()   
    
    # now we create a duplicate of the mesh in the same package of the original one, but adding the suffix _RePivoted to the name
    new_static_mesh = static_mesh.duplicate(package_name, new_asset_name)
    
    # time to fix each vertex
    updated_positions = []

    for vertex in raw_mesh.get_vertex_positions():
        updated_positions.append(vertex - center)
        # this is a trick for giving back control to Unreal Engine avoiding python to block it on slow operations
        ue.slate_tick()
        
    raw_mesh.save_to_static_mesh_source_model(new_static_mesh.SourceModels[0])
    new_static_mesh.static_mesh_build()
    new_static_mesh.static_mesh_create_body_setup()
    new_static_mesh.save_package()

for uobject in ue.get_selected_assets():
    fix_pivot(uobject)
```
