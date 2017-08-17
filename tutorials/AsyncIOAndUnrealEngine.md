# Using asyncio with Unreal Engine

# asyncio ? What is it ?

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

the `simple_timer` function is effectively a 'coroutine' (defining it with `async def` informs the python vm that this function can be suspended).

Albeit the function includes a `while True:` loop, the `await` call allows to suspend the coroutine and pass back the control to a sort of 'hub' (the loop engine) that will give back the control to the simple_timer function after the specified amount of seconds.

In addition to waiting for time, the loop engine is able to wait for file descriptors readyness and for the so called 'futures' (consider them as object holding a value that can be monitored for changes)

This is another example waiting for data from the popular telnet site `towel.blinkenlights.nl` returning you the ascii-art version of Star Wars Episode IV:

```python

```


## Why using asyncio in Unreal Engine 4 ?

Lot of asyncio third party libraries and frameworks are available.

There are asyncio modules for databases interaction, http and websockets services, dns and whatever you can think of (both client and servers).

These wrappers are extremely easy to use and completely abstract you from the concurrency problems. All of them will work in pair with the UE4 main loop (both in editor and in game), so you can basically do anything asyncio allows into Unreal Engine 4.

Want to integrate an HTTP server into UE ? doable. Want to wait for a redis pubsub queue ? doable. Want to issue commands for your editor via ssh ? doable. Well, there are really limitless possibilities :)

## Adding an asyncio loop engine in the UE4 core

## asyncio in your actors

## Additional 'transient' loop engines

