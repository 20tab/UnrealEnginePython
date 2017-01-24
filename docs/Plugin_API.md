The Plugin API
=

This API allows access to the Unreal Engine 4 plugin system (IPluginManager, and IPlugin interfaces)

Discovering/Listing/Referencing plugins
-

```python
import unreal_engine

# get the list of all discovered plugins
plugins_list = unreal_engine.get_discovered_plugins()

# get the list of enabled plugins
plugins_list = unreal_engine.get_enabled_plugins()

# name is the string name of the plugin (like 'UnrealEnginePython' or 'Paper2D')
plugin = unreal_engine.find_plugin(name)
```

The unreal_engine.IPlugin class represents a plugin


IPlugin methods
-

```python
import unreal_engine

paper2d = ue.find_plugin('Paper2D')

# the name of the plugin
name = paper2d.get_name()

base_dir = paper2d.get_base_dir()

content_dir =  paper2d.get_content_dir()

descriptor_file_name = paper2d.get_descriptor_file_name()

mounted_asset_path = paper2d.get_mounted_asset_path()

can_contain_content = paper2d.can_contain_content()

is_enabled = paper2d.is_enabled()
```

Managing the descriptor
-

You can load/write a json string from/to the plugin descriptor
