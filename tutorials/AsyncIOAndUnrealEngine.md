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

This is a more complex (but more useful) example, showing how to open a tcp server in your UE instance, to allow external communication.

Here, the protocol is pretty dumb: each lien sent by a client will spawn a new Character with the name set to the string passed by the client.

```python
import asyncio
import ue_asyncio
from unreal_engine.classes import Character

# cleanup previous tasks
for task in asyncio.Task.all_tasks():
    task.cancel()

# this is called whenever a new client connects
async def new_client_connected(reader, writer):
    name = writer.get_extra_info('peername')
    ue.log('new client connection from {0}'.format(name))
    while True:
        # wait for a line
        data = await reader.readline()
        if not data:
            break
        ue.log('client {0} issued: {1}'.format(name, data.decode()))
        # spawn a new Character and set its label
        new_actor = ue.get_editor_world().actor_spawn(Character)
        new_actor.set_actor_label(data.decode())
    ue.log('client {0} disconnected'.format(name))

# this spawns the server
# the try/finally trick allows for gentle shutdown of the server
# see below for more infos about exception management
async def spawn_server(host, port):
    try:
        coro = await asyncio.start_server(new_client_connected, host, port, reuse_address=True)
        ue.log('tcp server spawned on {0}:{1}'.format(host, port))
        await coro.wait_closed()
    finally:
        coro.close()
        ue.log('tcp server ended')
    
asyncio.ensure_future(spawn_server('192.168.173.45', 8885))
```

## Redis pubsub

Another useful example, here we subscribe to a remote redis queue, and we wait for messages

```python
import asyncio
import ue_asyncio
import unreal_engine as ue
import aioredis

# cleanup previous tasks
for task in asyncio.Task.all_tasks():
    task.cancel()

async def wait_for_redis():
    redis = await aioredis.create_redis(('192.168.173.27', 6379))
    channel, = await redis.subscribe('foobar')
    ue.log('subscribed to redis queue, waiting for messages ...')
    while await channel.wait_message():
        msg = await channel.get()
        ue.log_warning(msg)
    redis.close()
    ue.log_warning('done with redis')

asyncio.ensure_future(wait_for_redis())
```

## Managinx exceptions in non-awaited coroutines

By default exceptions thrown in a non-awaited coroutine (like the ones you spawn with asyncio.ensure_future) are not propagated.

There are various tricks you can use to manage them. The simplest one is trapping exceptions in the coroutine itself and reporting them as UE errors:

```python
async def connecto_to_a_broken_redis():
    try:
        redis = await aioredis.create_redis(('192.168.173.27', 6379))
        ...
    except Exception as e:
        ue.log_error(e)
    finally:
        # here you can run important cleanup tasks
        redis.close()
```

A more elegant approach is adding a callback whenever a coroutine ends, in which we will check if it died for an exception or it has been cancelled (or obviously it has simply finished its work):

```python

async def broken_coroutine():
    raise Exception('OOOOOOPS')
    
def check_exception(coro):
    # if the coroutine has been cancelled, just ignore it
    if coro.cancelled():
            return
    # check if the coroutine ended with an exception
    exc = coro.exception()
    # in such a case raise/propagate it
    if exc:
        raise exc

coroutine = asyncio.ensure_future(broken_coroutine())
coroutine.add_done_callback(check_exception)
```

Remember, you only need to pay attention to non-awaited coroutines, the others will correctly propagate the exception to the task awaiting it (obviously this task could be a not-awaited one, so as a general rule, add a callback every time you use asyncio.ensure_future())

## aiohttp

This is probably the most known third-party asyncio module. It basically interact with every HTTP aspect (server, client, websockets....)

## asyncio in your actors

### A simple Icecast/Shoutcast streaming radio Component

```python
import ue_asyncio
import asyncio
import aiohttp
import unreal_engine as ue
from unreal_engine.classes import SoundWaveProcedural, AudioComponent
import mpg123
import os

class RadioStreaming:

    def initialize_component(self):
        ue.print_string('Hello')
        self.actor = self.uobject.get_owner()
        self.audio = self.actor.get_actor_component_by_type(AudioComponent)
        self.audio.Sound = SoundWaveProcedural()
        self.audio.Sound.Duration = 10000
        # if you do not have libmpg123 in the system path, specify its absolute location here
        # self.mp3 = mpg123.Mpg123(library_path=os.path.join(ue.get_content_dir(), 'libmpg123-0.dll'))
        self.mp3 = mpg123.Mpg123()
        self.coroutine = asyncio.ensure_future(self.stream('http://178.32.136.160:8050'))
        self.coroutine.add_done_callback(self.check_exception)

    def check_exception(self, coro):
        if coro.cancelled():
            return
        exc = coro.exception()
        if exc:
            raise exc

    async def stream(self, url):
        async with aiohttp.ClientSession() as session:
            async with session.get(url) as response:
                ue.log_warning('start of stream')
                while True:
                    chunk = await response.content.read(4096)
                    if not chunk:
                        break

                    self.mp3.feed(chunk)

                    if not self.audio.IsPlaying():
                        rate, channels, encoding = self.mp3.get_format()
                        self.audio.Sound.SampleRate = rate
                        self.audio.Sound.NumChannels = channels
                        self.audio.Play()

                    for frame in self.mp3.iter_frames():
                        self.audio.Sound.queue_audio(frame)
                        

    def end_play(self, reason):
        ue.log_warning('end of stream')
        self.coroutine.cancel()
```

### PyUserWidget WebSocket Echo test

![PyUserWidget Designer](https://github.com/20tab/UnrealEnginePython/blob/master/tutorials/AsyncIO_Assets/py_user_widget_designer.PNG)


![PyUserWidget Graph](https://github.com/20tab/UnrealEnginePython/blob/master/tutorials/AsyncIO_Assets/py_user_widget_graph.PNG)

![PyUserWidget Level Blueprint](https://github.com/20tab/UnrealEnginePython/blob/master/tutorials/AsyncIO_Assets/py_user_widget_level_blueprint.PNG)

```python
import unreal_engine as ue
import ue_asyncio
import asyncio
import aiohttp

class Echo:

    def construct(self):
        self.coroutine = asyncio.ensure_future(self.ws_connect('ws://echo.websocket.org'))
        self.coroutine.add_done_callback(self.check_exception)
        self.ws = None


    def button_clicked(self):
        if self.ws:
            self.ws.send_str(self.uobject.Input.GetText())

    def check_exception(self, coro):
        if coro.cancelled():
            return
        exc = coro.exception()
        if exc:
            raise exc

    async def ws_connect(self, url):
        session = aiohttp.ClientSession()
        async with session.ws_connect(url) as ws:
            self.ws = ws
            async for msg in ws:
                orig_text = self.uobject.WSData.GetText()
                self.uobject.WSData.SetText(orig_text + '\n' + msg.data)
        await ws.close()              

```

## Additional 'transient' loop engines

## Note: concurrency vs parallelism

In the whole tutorial i have never used the term 'parallelism'.

It is important to distinguish between 'concurrency' and 'parallelism'. By 'concurrency' we mean the possibility for a software to manage multiple tasks, 'parallelism' means a way for execute those tasks in parallel (one per cpu-core). Threads are a way to obtain parallelism (albeit not always true in Python) as well as processes.

Coroutines, works very similar as a classic game engine (included unreal): we split the code paths in multiple tiny logical abstractions (like Actors in UE) and we suspend/resume each of them continuously, giving the user the illusion that multiple components works together at the same time. (effectively this is how a kernel scheduler works too, your computer processes never run in parallel if you have a single cpu core !).

For more informations about cooperation between python threads and asyncio coroutines, check here: https://docs.python.org/3/library/asyncio-dev.html#concurrency-and-multithreading (TLDR, do not mix them if you do not know what you are doing :P)
