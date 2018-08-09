# The Audio API

The uobject.play_sound_at_location(sound, position[, volume_multiplier, pitch_multiplier, start_time]) api method is exposed:

```py
# get a reference to asound
sound = ue.find_object('my_sound')
# play the sound at position 0,0,0
self.uobject.play_sound_at_location(sound, FVector(0, 0, 0))
```

If you prefer to work with AudioComponent:

```py
from unreal_engine.classes import AudioComponent

class Sounder:
    def begin_play(self):
        # find the AudioComponent of this actor
        self.audio = self.uobject.get_component_by_type(AudioComponent)
        self.audio.call('Stop')
    def tick(self, delta_time):
        # start the sound when pressing 'A'
        if self.uobject.is_input_key_down('A'):
            self.audio.call('Play')
```
