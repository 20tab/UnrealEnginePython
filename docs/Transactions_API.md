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
