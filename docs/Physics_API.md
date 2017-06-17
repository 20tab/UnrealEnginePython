# The Physics API

The 'set_simulate_physics' method is exposed for enabling physic on PrimitiveComponent.

Remember that you cannot enable physics withotu setting the collision presetes accordingly:

```py

# PyActor with a staticmeshcomponent (a sphere)
# when overlapped it became a physic object
class Ball:

    def begin_play(self):
        self.sphere = self.uobject.get_actor_component_by_type(ue.find_class('StaticMeshComponent'))
        
    def tick(self, delta_time):
        pass
    
    def on_actor_begin_overlap(self, me, other_actor):
        # change collision profile
        self.sphere.call('SetCollisionProfileName BlockAll')
        # enable physics
        self.sphere.set_simulate_physics()
        
    # once the object became a physics one, hit event will start arriving
    def on_actor_hit(self, me, other, *args):
        ue.print_string('HIT with ' + other.get_name())
```

TODO: expose more physics functions, expecially the one for applying forces
