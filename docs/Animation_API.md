
# The Animation API

You can control animation blueprints variables and events easily:

```py

from unreal_engine.classes import SkeletalMeshComponent

# get a reference to the skeletal mesh
skeletal = self.uobject.get_component_by_type(SkeletalMeshComponent)
# get a reference to the animation class
animation = skeletal.get_anim_instance()

# set a variable
animation.set_property('Speed', 17.0)

# trigger a custom event
animation.call('AttackWithSword')
```
