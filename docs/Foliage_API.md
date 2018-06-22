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

## Iterating instances

You can get a FFoliageInstance struct for each instance (grouped by UFoliageType):

```python
import unreal_engine as ue

foliage_actor = ue.get_editor_world().get_instanced_foliage_actor_for_current_level()

for foliage_type in foliage_actor.get_foliage_types():
   print('Foliage Type: {0}'.format(foliage_type.get_name()))
   for foliage_instance in foliage_actor.get_foliage_instances(foliage_type):
       print(foliage_instance.location)
       print(foliage_instance.draw_scale3d)
       print(foliage_instance.pre_align_rotation)
       print(foliage_instance.rotation)
       print(foliage_instance.flags)
       print(foliage_instance.zoffset)
       print('*' * 20)
```
