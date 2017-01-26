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
