# The Timer API

A custom python class is exposed (unreal_engine.FTimerHandler) to support UE4 timers:

```py
# create a timer
timer = self.uobject.set_timer(frequency, callable[, loop, initial])
# clear a timer
timer.clear()
# pause a timer
timer.pause()
# unpause a timer
timer.unpause()
```
