Add a ForEachLoop Macro node
-

```python
import unreal_engine as ue
from unreal_engine.classes import K2Node_MacroInstance, EdGraph
from unreal_engine.structs import GraphReference

# for_each_loop = ue.load_object(EdGraph, '/Engine/EditorBlueprintResources/StandardMacros.StandardMacros:ForEachLoop')
for_each_loop = ue.find_object('ForEachLoop')

# get a reference to your blueprint
blueprint = ...

# add the node
node = blueprint.UberGraphPages[0].graph_add_node(K2Node_MacroInstance)
# assign the macro graph to the node
node.MacroGraphReference = GraphReference(MacroGraph=for_each_loop)
# allocate pins
node.node_allocate_default_pins()

# update the blueprint
ue.blueprint_mark_as_structurally_modified(bp)
```
