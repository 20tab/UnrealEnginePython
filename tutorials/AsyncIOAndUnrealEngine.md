# Using AsyncIO with Unreal Engine

Available starting from python 3.4, asyncio (https://docs.python.org/3/library/asyncio.html) is 'the way to go' when dealing with high concurrency
in modern python applications.

It mixes basic non-blocking programming patterns with the 'coroutine' concept. You can see a coroutine as a function that can be 'suspended' and 'resumed' even in the middle of its execution flow

Consider the following example (run it from a standard shell with python >= 3.6):

```python

import asyncio

async def simple_timer(frequency):
    while True:
        await asyncio.sleep(frequency)
        print('{0} seconds elapsed'.format(frequency))

loop = asyncio.get_event_loop()
loop.create_task(simple_timer(1))
loop.create_task(simple_timer(3))

loop.run_forever()
```

Running the example will result in this infinite output:

```
1 seconds elapsed
1 seconds elapsed
3 seconds elapsed
1 seconds elapsed
1 seconds elapsed
1 seconds elapsed
3 seconds elapsed
1 seconds elapsed
1 seconds elapsed
...
```

What happened ?

the `simple_timer` function is effectively a 'coroutine' (defining it with 'async def' informs the python vm that this function can be suspended).

Albeit the function includes a `while True:` loop, the `await` call allows to suspend the coroutine and pass back the control to a sort of 'hub' (the loop engine) that will give back the control to the simple_timer function after the specified amount of seconds.

In addition to waiting for time, the loop engine is able to wait for file descriptors readyness and for the so called 'futures' (consider them as object holding a value that can be monitored for changes)


