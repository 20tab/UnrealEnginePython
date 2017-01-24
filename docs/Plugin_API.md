The Plugin API
=

This API allows access to the Unreal Engine 4 plugin system (IPluginManager, and IPlugin interfaces)

Discovering/Listing/Referencing plugins
-

```py
plugins_list = unreal_engine.get_discovered_plugins()
plugins_list = unreal_engine.get_enabled_plugins()
# name is the string name of the plugin (like 'UnrealEnginePython' or 'Paper2D')
plugin = unreal_engine.find_plugin(name)
```
