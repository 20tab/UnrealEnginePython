Managing your Assets with Python
=

Naming conventions
-

Before starting, you should be confortable with unreal naming conventions regarding assets:

* UObject, is the root of the vast majority of Unreal C++ classes. Materials, Textures, Blueprints, all inherit from UObject
* Asset is a UObject that has been serialized in some form. You see Assets in the content browser
* Package is a file containing one or more Assets. The blessed Unreal pattern is to have one Asset per Package, but you can have more than one in a single one. Packages are stored in the .uasset files in the Content directory.


Referencing assets
-

To load/reference an asset, you need to use a special naming convention:

```
PackagePath.AssetName
```

As an example a Material named 'FooBar' packaged in '/Game/Materials/Mat001' can be referenced as:

```
/Game/Materials/Mat001.Foobar
```

(this means a Mat001.uasset will be available in the /Content/Materials directory of the project filesystem)


To get a reference to the asset from python you can use:

```python
import unreal_engine as ue
# get the Mat001.Foobar asset
material = ue.get_asset('/Game/Materials/Mat001.Foobar')
# print material repr
ue.log(material)
# print material properties
ue.log(material.properties())
```

To get a list of all the assets under a specific path:

```python
materials = ue.get_assets('/Game/Materials')
```

by default the call will not be recursive (read: it will not scan subdirectories), to enable recursive mode just pass True as the second argument:

```python
materials = ue.get_assets('/Game/Materials', True)
```

You can get a list of all the assets inheriting for a specified class (passed as a string !). As get_assets(), the second argument specify recursion.

```python
# get all assets of the class Material
materials = ue.get_assets_by_class('Material')
```

Moving/Renaming assets
-

The rename_asset() function allows you to change the name of an asset:

```python
ue.rename_asset('/Game/Materials/Mat001.Foobar', 'FooBarUpdated')
```

this will result in the /Game/Materials/FooBarUpdated.FooBarUpdated asset. Beware, as the unreal editor retain the so called 'Redirector Object' that is basically a reference to the original object, so you will not be able to re-use the old name until you reload the editor.

If you specify a full path as the destination, you can 'move' the asset:

```python
ue.rename_asset('/Game/Materials/Mat001.Foobar', '/Game/Ops/FooBarUpdated')
```

will results in /Game/Ops/FooBarUpdated.FooBarUpdated asset

Duplicating assets
-

Works like renaming:

```python
ue.duplicate_asset('/Game/Materials/Mat001.Foobar', '/Game/NewMaterials/Mat001', 'FooBarUpdated')
```

This will result in /Game/NewMaterials/Mat001.FooBarUpdated being a copy of the original asset without destroying it

Deleting assets
-

```python
ue.delete_asset('/Game/NewMaterials/Mat001.FooBarUpdated')
```

will remove the /Game/NewMaterials/Mat001.FooBarUpdated asset


Importing Assets
-

This is probably the most important feature for task automation. It allows you to import an external resource (like textures, meshes, animation ...) into the editor.

To import a file in an asset folder:

```python
asset = ue.import_asset('/Users/FooBar/Desktop/texture001.png', '/Game/Textures')
```

this will result in the texture001 asset in the /Game/Textures folder

You can even import multiple assets in a single round:

```python
files_to_import = ['/Users/FooBar/Desktop/texture001.png', '/Users/FooBar/Desktop/texture002.png', '/Users/FooBar/Desktop/texture001.png', '/Users/FooBar/Desktop/texture003,png']
assets = ue.import_asset(files_to_import,  '/Game/Textures')
```

The previous functions trigger auto-discovery of the UFactory (a factory describes how to load a file, there are factories for each supported file format). If auto-discovery of the right factory fails, you can specify it

```python
from unreal_engine.classes import TextureFactory, FbxFactory
asset001 = ue.import_asset('/Users/FooBar/Desktop/texture001.png', '/Game/Textures', TextureFactory)
asset002 = ue.import_asset('/Users/FooBar/Desktop/warrior001.fbx', '/Game/Meshes', FbxFactory)
```

Note that FbxFactory is the default importer for fbx, but it will trigger a dialog. For automating tasks a better factory would be the included PyFbxFactory

```python
from unreal_engine.classes import PyFbxFactory
asset002 = ue.import_asset('/Users/FooBar/Desktop/warrior001.fbx', '/Game/Meshes', PyFbxFactory)
```

If the factory supports options, you can instantiate a factory before use:

```python
from unreal_engine.classes import PyFbxFactory

factory = PyFbxFactory()
factory.ImportUI.bImportAsSkeletal = True

asset002 = factory.factory_import_object('/Users/FooBar/Desktop/warrior001.fbx', '/Game/Meshes')
```

Reimporting assets
-

There are assets that could require re-importing (for example when you have a new texture image or fbx). In such a case you can do:

```python
asset002.asset_reimport()
```

Before reimporting you can check if an asset supports it:

```python
if asset002.asset_can_reimport():
   asset002.asset_reimport()
```

The signature of asset_reimport() is the following:

```python
success = asset_reimport(open_file_dialog, report_dialog, filename_to_import)
```

open_file_dialog will trigger an open file widget for selecting the file to import if the object has no file mapped to it.

report_dialog will show the notify widget reporting the status of the operation

filename_to_import allows you to specify which file to import for the asset

The function returns a boolean value

You can programmatically change the filename assigned to an asset in this way:

```python
mesh.asset_import_data_set_sources('D:/sword.fbx')
```


Creating Assets
-

You can create new asset manually or via a factory (if required):

```python
from unreal_engine.classes import ParticleSystem, Material

particle_system = ParticleSystem()
particle_system.set_name('ParticleSystemForDummies')

# special form of the constructor taking the object name
material = Material('FunnyMaterial')

# this will save particle_system into /Game/Particles.ParticleSystemForDummies
particle_system.save_package('/Game/Particles')

# this will save material into /Game/FunnyMaterials.FunnyMaterial
material.save_package('/Game/FunnyMaterials')
```

the previous approach is the blessed one where each asset is stored in a different package.

If you want to store multiple assets in the same package you can simply pass the same name in save_package

```python
# this will save particle_system into /Game/Funny.ParticleSystemForDummies
particle_system.save_package('/Game/Funny')

# this will save material into /Game/Funny.FunnyMaterial
material.save_package('/Game/Funny')
```

Take into account that if you call save_package() on a transient object (read: an object that is marked as not being storable to disk), the object flags will be overwritten to include RF_Public|RF_Standalone, while RF_Transient will be removed

Using a factory allows to create empty assets:

```python
from unreal_engine.classes import AnimBlueprintFactory

# get a reference to a skeleton asset (required for anim blueprint)
skeleton = ue.get_asset('/Game/Skeleton.Skeleton001')
anim_blueprint_factory = AnimBlueprintFactory()

anim_blueprint_factory.TargetSkeleton = skeleton

# create the asset
anim_blueprint = anim_blueprint_factory.factory_create_new('/Game/anim001')
```

Asset dependencies/referencers
-

Each Asset has a list of other assets referencing to it (referencers) and asset on which it depends (dependencies).

You can access those lists with:

```python
list_of_referencers = ue.get_asset_referencers('/Game/FooBar')
list_of_dependencies = ue.get_asset_dependencies('/Game/FooBar')
```
