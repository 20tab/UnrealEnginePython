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

```py
import unreal_engine as ue
# get the Mat001.Foobar asset
material = ue.get_asset('/Game/Materials/Mat001.Foobar')
# print material repr
ue.log(material)
# print material properties
ue.log(material.properties())
```

To get a list of all the assets under a specific path:

```py
materials = ue.get_assets('/Game/Materials')
```

by default the call will not be recursive (read: it will not scan subdirectories), to enable recursive mode just pass True as the second argument:

```py
materials = ue.get_assets('/Game/Materials', True)
```

You can get a list of all the assets inheriting for a specified class (passed as a string !). As get_assets(), the second argument specify recursion.

```py
# get all assets of the class Material
materials = ue.get_assets_by_class('Material')
```


