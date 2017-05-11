# Your First Automated Pipeline with UnrealEnginePython (Part 2)

In this tutorial we will see how to reference the previously created assets, and how to create an AI using Behavior Trees and a BlackBoard.

Finally we will add a unit test for the Kaiju.

WORK IN PROGRESS


## Filling the Blueprint Event Graph

TODO: implement the idle timer triggering the bored state

## The BlackBoard

```python
from unreal_engine.classes import BlackBoardDataFactory
from unreal_engine.classes import BlackboardKeyType_Bool, BlackboardKeyType_String

from unreal_engine.structs import BlackboardEntry

```

## The Behavior Tree Graph

TODO: Implement a decorator checking for pawn in a sphere, create a bt with a sequence of 'move to', 'raise event (attack)', 'wait'

## The Kaiju Brain

TODO: assign the bt to the AIController of the Kaiju

## Testing it

TODO: write a unit test instantiating the Kaiju with a dumb pawn and checking for pawn in its virtual sphere.
