The Material API
----------------

Unreal has 3 "kind" of materials:

* Material - this is the base for the following two types, this is where you define material nodes and properties, it must be compiled to generate the required shaders.

* Material Instance - when you want to modify parameters of a material you do not need to create a whole new material, but you can create an 'instance' of it. The instance shares the logic of the parent material but changes specific properties (like textures).

* Material Instance Dynamic - sometime you may want to change the properties of a material instance at runtime, in such a case you need to generate a special matrial instance that is mapped to a specific object while your game runs.

Assigning a material to a primitive component
---------------------------------------------

Once your material (instanced or non instanced, constant or dynamic) is ready, you want to assign it to a PrimitiveComponent:


```python
component = self.uobject.get_actor_component('Mesh')
material = ue.load_object(Material, '/Game/Materials/Iron')
component.set_material(index, material);
```

Creating a Material (editor only)
---------------------------------

This is the 'raw' way for creating a Material

```python
from unreal_engine.classes import Material
new_material = Material()
new_material.set_name('New Funny Material')
new_material.save_package('/Game/Materials/NewFunnyMaterial')
```

Even better, you can use the MaterialFactoryNew class

```python
from unreal_engine.classes import MaterialFactoryNew
import unreal_engine as ue

factory = MaterialFactoryNew()
new_material = factory.factory_create_new('/Game/Materials/NewFunnyMaterial')

# destroy the asset
ue.delete_asset(new_material.get_path_name())
```

Creating a Material Instance (editor only)
------------------------------------------

You have two ways to create a instanced material:

(new_material is a reference to a previously created/loaded material)

```python
from unreal_engine.classes import MaterialInstanceConstant

material_instance = MaterialInstanceConstant()
material_instance.set_name('New Funny Material Instance')
material_instance.set_material_parent(new_material)
material_instance.save_package('/Game/Materials/instanced')
```

or the shortcut:

```python
import unreal_engine as ue
# the material instance will get the name of the parent with the _inst suffix
material_instance = ue.create_material_instance(new_material)
```

```python
import unreal_engine as ue
# you can also specify a path and a name to the new material instance.
material_instance = ue.create_material_instance(new_material, '/Game/Materials/', 'New Funny Material Instance')
```


Or the factory way:

```python
from unreal_engine.classes import Material, MaterialInstanceConstantFactoryNew

factory = MaterialInstanceConstantFactoryNew()

# get a reference to a parent material
parent_material = ue.load_object(Material, '/Game/MainMaterial')

factory.InitialParent = parent_material

child_material = factory.factory_create_new('/Game/ChildMaterial')
```

Creating a Material Instance Dynamic
------------------------------------

You can create a MID (Material Instance Dynamic) from a material (or material instance):

```python
mid = self.uobject.create_material_instance_dynamic(material_instance)
```


Listing, getting and changing available material properties
-----------------------------------------------------------

To modify a material's property you need to know its name.

You cannot access the property list from a material instance, you need to get it from the parent:

```python
parent_material = material_instance.Parent

for expression in parent_material.Expressions:
    parameter_name = expression.ParameterName
    parameter_group = expression.Group
```


Once you have the property name you can get/set parameters values

```py
# retuns a float
material_instance.get_material_scalar_parameter('Parameter name')
# returns a FVector
material_instance.get_material_vector_parameter('Parameter name')
# returns a Texture
material_instance.get_material_texture_parameter('Parameter name')

material_instance.set_material_scalar_parameter('Parameter name', float)
material_instance.set_material_vector_parameter('Parameter name', FVector)
material_instance.set_material_texture_parameter('Parameter name', Texture)
```
