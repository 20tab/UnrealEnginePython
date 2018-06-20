The Native Subclassing API
--------------------------

This is the most advanced way to use Python for your gameplay logic, developing plugins or automating tasks.

It resembles the native way you would work with unreal and c++ but in a (more or less ;) pythonic way.

Anything you define in python subclasses of UObjects is automatically exposed to Blueprints. As python is a dynamic language without strong encapsulation concepts, this looks (at least currently) the best approach from a python-developer point of view.

The system is still under heavy development and new ideas are popping up constantly (the main objective is to simplify and speed up  end-users development)

Before starting using it, ensure to understand the exposed reflection system: https://github.com/20tab/UnrealEnginePython#native-methods-vs-reflection

NOTE: currently it works only on python3 as it heavily relies on function annotations.

Subclassing
-----------

You can define new classes using standard python subclassing:

```py
import unreal_engine as ue
from unreal_engine.classes import Character, PawnSensingComponent, Pawn

class Monster(Character):

    # constructor adding a component
    def __init__(self):
        self.sensor = self.add_actor_component(PawnSensingComponent, 'Sensor')
        
    # properties can only be set starting from begin play
    def ReceiveBeginPlay():
        self.sensor.SightRadius = 17000
    
    # this will automatically override the OnSeePawn event
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

To map object references to properties:


```py
import unreal_engine as ue
from unreal_engine.classes import Character, PawnSensingComponent, Pawn, FloatProperty

class Hero(Character):

    # you can set it from the editor
    AnotherCharacter = Character 
    
    def ReceiveTick(self, DeltaSeconds: float):
        ue.log('Hello ' + str(AnotherCharacter))
```


Functions and Events
--------------------

By default any methods having the same signature of an internal virtual one (like ReceiveBeginPlay in the previous examples) will became 
automagically an override. Pay attention to the function annotation style for helping the blueprint system discovering the type of arguments. Except from self, any argument (or return value) without proper type will result in ignoring the argument:

```py
import unreal_engine as ue
from unreal_engine.classes import Character, PawnSensingComponent, Pawn, FloatProperty

class Hero(Character):

    # you can set it from the editor as array
    UpSpeed = [FloatProperty]
    
    # automatic override
    def ReceiveTick(self, DeltaSeconds: float):
        location = self.get_actor_location()
        location.z += self.UpSpeed[0] * DeltaSeconds
        self.set_actor_location(location)
        
    # this new method will be available to blueprints
    def FunnyNewMethod(self, a_word: str):
        ue.print_string('This is a word from blueprint: ' + a_word)
```

you can do manual override too:

```py
import unreal_engine as ue
from unreal_engine.classes import Character, PawnSensingComponent, Pawn, FloatProperty

class Hero(Character):

    # you can set it from the editor as array
    UpSpeed = [FloatProperty]
    
    # manual override
    def funny_receive_tick(self, DeltaSeconds: float):
        location = self.get_actor_location()
        location.z += self.UpSpeed[0] * DeltaSeconds
        self.set_actor_location(location)
    funny_receive_tick.override = 'ReceiveTick'
```

To mark a method as 'pure' (in the Blueprint sense):

```py
import unreal_engine as ue
from unreal_engine.classes import Character

class Hero(Character):
        
    # this new method will be available to blueprints
    def FunnyNewMethod(self, a_word: str):
        ue.print_string('This is a word from blueprint: ' + a_word)
    FunnyNewMethod.pure = True
```

or static:

```py
import unreal_engine as ue
from unreal_engine.classes import Character

class Hero(Character):
        
    # this new static method will be available to blueprints
    def FunnyStaticMethod():
        ue.print_string('I am a static method')
    FunnyStaticMethod.static = True
```

Events can be easily exposed like this:

```py
class ExplodeTrigger(TriggerBox):

    def WorldDestroyedByDaemons(self):
        ue.log_warning('WORLD DESTROYED')
    WorldDestroyedByDaemons.event = True
```

You can make your events networkedm using the 'multicast', 'server', 'client' and 'reliable' attributes:

```py
class ExplodeTrigger(TriggerBox):

    # this event will be run on the server and in reliable mode
    def WorldDestroyedByDaemons(self):
        ue.log_warning('WORLD DESTROYED')
    WorldDestroyedByDaemons.event = True
    WorldDestroyedByDaemons.server = True
    WorldDestroyedByDaemons.reliable = True
```


Reloading
---------

The hot-reloading system is still under heavy testing, expect a bunch of crashes when you heavily redefine classes.

By the way, the idea is that simply redefining a class in python (for example via unreal_engine.exec) will update the internal unreal definition. Try to stress-test it and report crash backtraces. Thanks a lot !
