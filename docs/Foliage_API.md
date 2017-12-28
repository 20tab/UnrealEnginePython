# The Foliage API

Each UWorld can have its AInstancedFoliageActor dedicated to drawing foliage.

Basically the foliage system is massive instancing of static meshes.

UFoliageType is the representation of a StaticMesh that can be drawn as Foliage.

You can create a UFoliageType easily:

```python
from unreal_engine.classes import FoliageTypeFactory, StaticMesh
import unreal_engine as ue

factory = FoliageTypeFactory()
foliage_type = factory.factory_create_new('/Game/Foliage/FirstFoliageType')
foliage_type.Mesh = ue.load_object(StaticMesh, '/Game/Mesh/StaticMesh001')
foliage_type.save_package()
```

To add a UFoliageType to a UWorld just do:

```python

world.add_foliage_asset(foliage_type)
```

Eventually you can add a StaticMesh (instead of a FoliageType) directly to the AInstancedFoliageActor:

```python
world.add_foliage_asset(ue.load_object(StaticMesh, '/Game/Mesh/StaticMesh001'))
```

Note: unfortunately the foliage editor is not automatically updated, just switch the editor mode manually to the foliage mode to update the assets list.

You can get a reference to the AInstancedFoliageActor of a world using:

```python
foliage_actor = world.get_instanced_foliage_actor_for_current_level()
```
