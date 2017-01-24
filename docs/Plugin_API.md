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

The boolean argument is the bPluginTypeEnabledByDefault field (it is the 'Enabled' tick in the plugins menu of the editor)

```python
# get the current json descriptor of the plugin
json = paper2d.to_json(True)

# load a json in the plugin descriptor
paper2d.from_json(json_blob, True)
```

IPlugin properties (read only)
-

```python
.category
.can_contain_content
.enabled_by_default
.installed
.is_beta_version
.requires_build_platform
.created_by
.created_by_url
.description
.docs_url
.file_version
.friendly_name
.marketplace_url
.support_url
.version
.version_name
```
