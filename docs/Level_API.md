# The Level API

## ULevel vs UWorld

Before dealing with the Level api, we need to focus on the thin difference between a level and a world.

To be rough, the matter is at the implementation level.

A UWorld is a container for actors, but for allowing features like 'level streaming' (common in open worlds game, where you cannot completely load a huge level but instead you load only the currently-relevant fractions of it), a UWorld can be mapped to one or more "levels".

To be more clear: when you create a new Level (or a new 'map' in some naming convention) you are effectively creating a new UWorld asset (it has even a factory !):

```python
from unreal_engine.classes import WorldFactory

factory = WorldFactory()
new_world = factory.factory_create_new('/Game/Maps/FooLevel')
```

The new_world object is effectively a UWorld. Whenever you create a level from the editor you are creating a world (yes, it sounds weird...)

Now, sometimes (like in the open world example before), you want multiple of this world to be active at the same time in the same viewport. But a viewport is mapped to a single world, so an additional abstraction (ULevel) is required.

So by default, for each UWorld you have a ULevel object representing it, but you can load additional UWorld's and 'combine' them with the 'main' one.

Let's try to clarify things wth another example (read comments carefully):


```python
import unreal_engine as ue
from unreal_engine.classes import WorldFactory, Actor, PlayerStart

factory = WorldFactory()

# create a world (it will be the main one, the one you load into the editor by double clicking it)
main_world = factory.factory_create_new('/Game/MapsTest/MainWorld001')
# spawn actors in the world
actor0 = main_world.actor_spawn(PlayerStart)

# create another world
child_world1 = factory.factory_create_new('/Game/MapsTest/ChildWorld001')
# spawn actors in the world
actor1 = child_world1.actor_spawn(Actor)
actor2 = child_world1.actor_spawn(Actor)
actor3 = child_world1.actor_spawn(Actor)

# create another world
child_world2 = factory.factory_create_new('/Game/MapsTest/ChildWorld002')
# spawn actors in the world
actor4 = child_world2.actor_spawn(Actor)

# now the important part, each UWorld, has a ULevel mapped to it (the PersistentLevel):
main_level = main_world.PersistentLevel
child_level1 = child_world1.PersistentLevel
child_level2 = child_world2.PersistentLevel

# open main world in the editor
ue.open_editor_for_asset(main_world)
```

## Joining Levels to a single world

So we now know that each world asset has its level counterpart, and this level is the 'joint' a world can use to attach to another world to implement composition (useful if multiple developers works in different areas of the scene) or streaming.

To attach a level to a world you can use the following function:

```python
import unreal_engine as ue

# note we pass the name of the asset instead of the uobject !
# returns a ULevelStreaming, if you want to retrieve a ULevel from ULevelStreaming, use the 'LoadedLevel' property
added_streaming_level = ue.add_level_to_world(main_world, child_world1.get_path_name()[, always_loaded])
```

Remember that you need to pass the asset path of the world you want to attach !

The always_loaded optional boolean parameter, if set, will add a persistent level instead of a streaming one (it means whenever you load the main_world even child_world1 will be enabled too)

# The "CurrentLevel" concept

Each world has the concept of the "current level". As an example, when you call actor_spawn() on a world, the actor will be effectively spawned in the world that is marked as the current level:

```python
# get a reference to the current level
current_level = ue.get_editor_world().get_current_level()

# change the current level
ue.get_editor_world().set_current_level(child_level1)

# spawn an actor in editor world, but effectively it will be spawned
# in a child level
actor001 = ue.get_editor_world().actor_spawn(actor001)

# back to the original level
ue.get_editor_world().set_current_level(current_level)
```
