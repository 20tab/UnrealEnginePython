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
