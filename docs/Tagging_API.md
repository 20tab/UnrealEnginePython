# The Tagging API

You can 'tag' actor and components.

A tag is a string (well, an FName internally) you assign to your actors and components. Each of them can have multiple tags.

## Tagging Actors

The 'Tags' properties allows you to tag actors:

```python
your_actor.Tags = ['foo', 'Bar', tEsT']
```

You can eventually fast-check an actor for the availability of a tag:

```python
if your_actor.actor_has_tag('foo'):
    ue.log('Success')
```

Finding all actors with a tag is unfortuntaley a slow operation (in C++, Blueprint and obviously python):

```python
import unreal_engine as ue

def find_all_actors_with_tag(world, tag):
    for actor in world.all_actors():
        if actor.actor_has_tag(tag):
            yield actor


world = ue.get_editor_world()

for actor in find_all_actors_with_tag(world, 'foo'):
    print(actor)
```

Eventually you can use the blueprint api:

```python
import unreal_engine as ue
from unreal_engine.classes import GameplayStatics

world = ue.get_editor_world()

# blueprint ufunctions returns a tuple (as UFunctions can returns multiple values)
actors, = GameplayStatics.GetAllActorsWithTag(world, 'one')
for actor in actors:
    print(actor)
```

## Tagging Components

You can tag components too using the property 'ComponentTags':

```python
your_component.ComponentTags = ['one', 'two', 'three']
```

Fast-check of tag availability can be done with the component_has_tag() function:

```python
if your_component.component_has_tag('one'):
    print('yes')
```

While you can get the list of actor's component tagged with the specific value:

```python
component_list = your_actor.get_components_by_tag('two')
```

The function can take an optional argument specifying which classes of ActorComponent must be taken into account:

```python
from unreal_engine.classes import SceneComponent
# search all the components children of the class SceneComponent and tagged with 'two'
component_list = your_actor.get_components_by_tag('two', SceneComponent)
```
