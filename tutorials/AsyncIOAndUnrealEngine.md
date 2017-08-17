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

asyncio.ensure_future(simple_timer(1))
asyncio.ensure_future(simple_timer(3))

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
import asyncio

async def simple_timer(frequency):
    while True:
        await asyncio.sleep(frequency)
        print('{0} seconds elapsed'.format(frequency))

async def star_wars():
    reader, writer = await asyncio.open_connection('towel.blinkenlights.nl', 23)
    while True:
        data = await reader.read(1024)
        if not data:
            break
        print(data.decode())

loop = asyncio.get_event_loop()
asyncio.ensure_future(simple_timer(1))
asyncio.ensure_future(simple_timer(3))

asyncio.ensure_future(star_wars())

loop.run_forever()
```

A new coroutine has been spawned, it suspend itself while waiting for connection to the host, and once the connection is made, the loop engine gives back control to it. In the while loop the coroutine suspends itself whenever it needs to wait for data from the socket.

When there are no more data, the coroutine ends.

Running the script will print the ascii art along the 2 timers we have seen before. All concurrently.

## Why using asyncio in Unreal Engine 4 ?

Lot of asyncio third party libraries and frameworks are available.

There are asyncio modules for databases interaction, http and websockets services, dns and whatever you can think of (both client and servers).

These wrappers are extremely easy to use and completely abstract you from the concurrency problems. All of them will work in pair with the UE4 main loop (both in editor and in game), so you can basically do anything asyncio allows into Unreal Engine 4.

Want to integrate an HTTP server into UE ? doable. Want to wait for a redis pubsub queue ? doable. Want to issue commands for your editor via ssh ? doable. Well, there are really limitless possibilities :)

## Adding an asyncio loop engine in the UE4 core

We will start by adding a new (pretty simple) module to our project (call it ue_asyncio.py):

```python
import asyncio
import unreal_engine as ue

loop = asyncio.new_event_loop()

asyncio.set_event_loop(loop)

def ticker_loop(delta_time):
    try:
        loop.stop()
        loop.run_forever()
    except Exception as e:
        ue.log_error(e)
    return True


ticker = ue.add_ticker(ticker_loop)
```

Do not run this module, it will be just the 'hub' the other parts of our project will connect to.

Basically when you first import this module a new 'ticker' will be registered into Unreal Engine. A ticker is a function that periodically 'ticks'. In this case Unreal Engine will use this ticker function to wake up the asyncio loop engine to manage all of the coroutines registered to it.

Let's start with the previous timer example:

```python
import asyncio
import unreal_engine as ue

import ue_asyncio

async def simple_timer(frequency):
    while True:
        await asyncio.sleep(frequency)
        ue.log('{0} seconds elapsed'.format(frequency))

asyncio.ensure_future(simple_timer(2))
```

If you run this script in the UE Python console you will start getting messages every 2 seconds.

If you re-run the script, a new coroutine will be spawned, but the old one is still here...

Generally this is not what you want, lucky enouch each asyncio loop allows you to access the list of currently available coroutines, and eventually cancel them:

```python
for task in asyncio.Task.all_tasks():
    task.cancel()
```

## A simple tcp server spawning Character's in the editor

```python
import asyncio
import ue_asyncio
from unreal_engine.classes import Character

for task in asyncio.Task.all_tasks():
    task.cancel()

async def new_client_connected(reader, writer):
    name = writer.get_extra_info('peername')
    ue.log('new client connection from {0}'.format(name))
    while True:
        data = await reader.readline()
        if not data:
            break
        ue.log('client {0} issued: {1}'.format(name, data.decode()))
        new_actor = ue.get_editor_world().actor_spawn(Character)
        new_actor.set_actor_label(data.decode())
    ue.log('client {0} disconnected'.format(name))

async def spawn_server(host, port):
    try:
        coro = await asyncio.start_server(new_client_connected, host, port)
        ue.log('tcp server spawned on {0}:{1}'.format(host, port))
        await coro.wait_closed()
    finally:
        coro.close()
        ue.log('tcp server ended')
    
asyncio.ensure_future(spawn_server('192.168.173.45', 8885))
```

## Redis pubsub

## asyncio in your actors

## Additional 'transient' loop engines

## Note: concurrency vs parallelism
