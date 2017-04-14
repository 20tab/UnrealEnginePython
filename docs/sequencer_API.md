The Sequencer API
-----------------

The sequencer API development has been sponsored by Matthew Whelan (http://www.mattwhelan.com/)

Credits should go to Ron Fischer too, for having explained me the sequencer api ;)

Creating a Sequencer Asset
--------------------------

You can create a new 'Level Sequence' asset using the LevelSequencerFactoryNew class:

```python
from unreal_engine.classes import LevelSequenceFactoryNew

factory = LevelSequenceFactoryNew()
seq = factory.factory_create_new('/Game/MovieMaster')
```
