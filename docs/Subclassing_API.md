The Native Subclassing API
--------------------------

This is the most advanced way to use Python for your gameplay logic, developing plugins or automating tasks.

It resembles the native way you would work with unreal and c++ but in a (more or less ;) pythonic way.

Anything you define in python subclasses of UObjects is automatically exposed to Blueprints. As python is a dynamic language without strong encapsulation concepts, this looks (at least currently) the best approach from a python-developer point of view.

The system is still under heavy development and new ideas are popping up constantly (the main objective is to simplify and speed up  end-users development)

NOTE: currently it works only on python3 as it heavily relies on function annotations.

Subclassing
-----------

You can define new classes using standard python subclassing:

```py
import unreal_engine as ue
from unreal_engine.classes import Character, PawnSensingComponent, Pawn

class Monster(Character):

    def __init__(self):
        self.sensor = self.add_actor_component(PawnSensingComponent, 'Sensor')
        
    # properties can only be set starting from begin play
    def ReceiveBeginPlay():
        self.sensor.SightRadius = 17000
        
    def OnSeePawn(self, pawn : Pawn):
        ue.print_string('seen {}'.format(pawn))
```

Another example for a custom trigger:

```py
import unreal_engine as ue
from unreal_engine.classes import TriggerBox, Actor

class ExplodeTrigger(TriggerBox):

    def ReceiveActorBeginOverlap(self, other : Actor):
        ue.log('TRIGGERED BY ' + str(other))
```

Properties
----------

Properties can be added to your class as static python class properties:

```py
import unreal_engine as ue
from unreal_engine.classes import Character, PawnSensingComponent, Pawn, FloatProperty

class Hero(Character):

    # you can set it from the editor
    UpSpeed = FloatProperty
    
    def ReceiveTick(self, DeltaSeconds: float):
        location = self.get_actor_location()
        location.z += self.UpSpeed * DeltaSeconds
        self.set_actor_location(location)
```

Array can be specified like this:

```py
import unreal_engine as ue
from unreal_engine.classes import Character, PawnSensingComponent, Pawn, FloatProperty

class Hero(Character):

    # you can set it from the editor as array
    UpSpeed = [FloatProperty]
    
    def ReceiveTick(self, DeltaSeconds: float):
        location = self.get_actor_location()
        location.z += self.UpSpeed[0] * DeltaSeconds
        self.set_actor_location(location)
```


Functions and Events
--------------------

Reloading
---------
