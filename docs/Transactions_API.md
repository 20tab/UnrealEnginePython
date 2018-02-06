The Trasaction API
=

Transactions in Unreal Engine 4 are sequences of recorded editor commands that can be re-executed or reverted. It is the base of the undo/redo system of Unreal Engine 4. 

If you want to check how transactions are managed, open select the  "Edit->Undo History" menu item.

Managing transactions
-

It is highly suggested to execute the commands with the "Undo History" panel opened:

```python
from unreal_engine.classes import Actor, Character
import unreal_engine as ue

# get the editor world
world = ue.get_editor_world()

# begin a new transaction (can be reverted/reapplied)
ue.begin_transaction('My first transaction')
actor1 = world.actor_spawn(Actor)
actor2 = world.actor_spawn(Actor)
actor3 = world.actor_spawn(Actor)
ue.end_transaction()

# begin a new transaction (can be reverted/reapplied)
ue.begin_transaction('My second transaction')
character1 = world.actor_spawn(Character)
character2 = world.actor_spawn(Character)
character3 = world.actor_spawn(Character)
ue.end_transaction()

# undo the two transactions
ue.editor_undo()
ue.editor_undo()

# redo the last one
ue.editor_redo()
```


Managing transactions with property modification
-
In this little example we are creating a cone using a static mesh object from the engine content. We start a transaction and move the cone by 15 units and use the `modify` call to keep our property modifications pushed to the undo stack.

```python

import unreal_engine as ue
from unreal_engine import FVector
from unreal_engine.classes import StaticMesh, StaticMeshActor

#Create procedurally a Cone in the Editor
world = ue.get_editor_world()
cone2 = world.actor_spawn(StaticMeshActor)
cone2.StaticMeshComponent.StaticMesh = ue.load_object(StaticMesh, '/Engine/BasicShapes/Cone')

#Start Transaction 
ue.begin_transaction("Move Up +15")
position = cone2.get_actor_location()
position.y += 15

#Call modify to track property changes before setting the position
cone2.modify()
cone2.set_actor_location(position)
ue.end_transaction()

#Undo "Move Up +15"
##ue.editor_undo()
```




Functions
-

```python
# begin a new transaction with the psecified description (returns the transaction id)
transaction_id = unreal_engine.begin_transaction(description)

# cancel the current transaction
unreal_engine.cancel_transaction()

# end the transaction (returns the next transaction id)
tid = unreal_engine.end_transaction()

# returns True if we are currently into a transaction
is_active = unreal_engine.is_transaction_active()

# get the current transaction name
name = unreal_engine.get_transaction_name()

# redo the current transaction
unreal_engine.redo_transaction()

# reset the current transaction
unreal_engine.reset_transaction()

# get the list of transactions
transactions = unreal_engine.transactions()

# undo (return True on success)
success = unreal_engine.editor_undo()

# red (return True on success)
success = unreal_engine.editor_redo()
```
