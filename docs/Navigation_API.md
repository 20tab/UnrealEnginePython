# The Navigation API

The only exposed navigation-related method is 'simple_move_to_location'. It expects a Pawn with a movement component (like a Character)

```py
class MoveToTargetComponent:

    def begin_play(self):
        # get a 'target point' reference from a pawn public property
        target = self.uobject.get_owner().get_property('target')
        self.uobject.get_owner().simple_move_to_location(target.get_actor_location())
        
    def tick(self, delta_time):
        pass
```

Another example for diablo-like movements (click to move, add this component to a character)

```py
class Walker:
    def begin_play(self):
        self.uobject.show_mouse_cursor()
    def tick(self, delta_time):
        if not self.uobject.is_input_key_down('LeftMouseButton'):
            return
        hit = self.uobject.get_hit_result_under_cursor(ue.COLLISION_CHANNEL_VISIBILITY)
        if not hit:
            return
        # hit is a unreal_engine.FHitResult object
        self.uobject.simple_move_to_location(hit.impact_point)
```
