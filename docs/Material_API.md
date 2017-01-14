The Material API
----------------

Unreal has 3 "kind" of materials:

* Material (this is the base for the following two types, this is where you define material nodes and properties, it must be compiled to generate the required shaders)

* Material Instance (when you want to modify parameters of a material you do not need to create a whole new material, but you can create an 'instance' of it. The instance shared the logic of the material but changes specific properties (like textures)

* Material Instance Dynamic (sometime you want to change the properties of a material instance at runtime, in such a case you need to generate a special matrial instance that is mapped to a specific object while your game runs)
