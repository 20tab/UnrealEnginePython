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
