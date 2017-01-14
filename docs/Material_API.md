The Material API
----------------

Unreal has 3 "kind" of materials:

* Material - this is the base for the following two types, this is where you define material nodes and properties, it must be compiled to generate the required shaders.

* Material Instance - when you want to modify parameters of a material you do not need to create a whole new material, but you can create an 'instance' of it. The instance shares the logic of the parent material but changes specific properties (like textures).

* Material Instance Dynamic - sometime you may want to change the properties of a material instance at runtime, in such a case you need to generate a special matrial instance that is mapped to a specific object while your game runs.

Assigning a material to a primitive component
---------------------------------------------

Once your material (instanced or non instanced, constant or dynamic) is ready, you want to assign it to a PrimitiveComponent:


```py
component = self.uobject.get_actor_component('Mesh')
material = ue.load_object(Material, '/Game/Materials/Iron')
component.set_material(index, material);
```

Creating a Material (editor only)
---------------------------------

```py
from unreal_engine.classes import Material
new_material = Material()
new_material.set_name('New Funny Material')
new_material.save_package('/Game/Materials/NewFunnyMaterial')
```

Creating a Material Instance (editor only)
------------------------------------------

You have two ways to create a instanced material:

(new_material is a reference to a previously created/loaded material)

```py
from unreal_engine.classes import MaterialInstancedConstant

material_instance = MaterialInstancedConstant()
material_instance.set_name('New Funny Material Instance')
material_instance.set_material_parent(new_material)
material_instance.save_package('/Game/Materials/instanced')
```

or the shortcut:

```py
import unreal_engine as ue
# the material instance will get the name of the parent with the _inst suffix
material_instance = ue.create_material_instance(new_material)
```

Creating a Material Instance Dynamic
------------------------------------

You can create a MID (Material Instance Dynamic) from a ,aterial instance:

```py
mid = self.uobject.create_material_instance_dynamic(material_instance)
```


Listing, getting and chaning available material properties
----------------------------------------------------------
