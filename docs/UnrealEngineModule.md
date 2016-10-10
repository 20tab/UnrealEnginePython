The unreal_engine module
------------------------

This is a generic module, exposing functions not related to a particular 'World' (see below).

You can use these functions from an actor, a pawn, a component or directly from a module.

Just remember to import it:

```py
import unreal_engine
```

or (as an example)

```py
import unreal_engine as ue
```


---
```py
uclass = unreal_engine.find_class('name')
```

This function searches for a class in the engine with the specified name and returns its 'uobject' (a c++ UClass object internally).
You can use this uobject to search for objects of that type or to spawn new actors (and lot of more things)

---
```py
uclass = unreal_engine.find_object('name')
```

This is a more generic (and slower) variant of find_class that searches for every UObject. You can use it (for example) for getting references to assets (like materials, meshes...)

---
```py
unreal_engine.log('message')
```

log a string into the editor output log (under the LogPython category/class)


---
```py
unreal_engine.log_warning('message')
```

log a warning string (yellow) into the editor output log (under the LogPython category/class)


---
```py
unreal_engine.log_error('message')
```

log an error string (red) into the editor output log (under the LogPython category/class)


---
```py
unreal_engine.add_on_screen_debug_message(key, timeout, 'message')
```

low-level equivalent of blueprint 'print string' function. It disappear after 'timeout' seconds and can get a numeric key (use -1 for disabling keys feature)

TODO: support for colors


---
```py
unreal_engine.print_string('message')
```

python equivalent of the blueprint 'print string' function. It disappears after 2 seconds and it is wrote in cyan color.


---
```py
editor = unreal_engine.get_editor_world()
```

(available only into the editor) it allows to get a reference to the editor world. This will allow in the near future to generate UObjects directly in the editor (for automating tasks or scripting the editor itself)

