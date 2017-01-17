Managing your Assets with python
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


