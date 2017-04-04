import unreal_engine as ue
from unreal_engine.classes import PyFbxFactory, TextureFactory, Material, Actor
from unreal_engine import FVector, FRotator
from unreal_engine.enums import EFBXImportType
import os.path
import time

# in which content folder to store the new assets
kaiju_destination = '/Game/Kaiju'
# the path from where new assets are loaded
kaiju_source = os.path.expanduser('~/Desktop')

# an fbx skeletal mesh
kaiju_mesh = 'slicer_tpose.fbx'
# textures (from substance designer)
kaiju_textures = ['slicer_BaseColor.tga', 'slicer_Emissive.tga', 'slicer_Normal.tga', 'slicer_OcclusionRoughnessMetallic.tga']
# an fbx skeletal animation
kaiju_animation = 'slicer_walking.fbx'

# start configuring the mesh importer
factory = PyFbxFactory()
factory.ImportUI.bCreatePhysicsAsset = False
factory.ImportUI.bImportMaterials = False
factory.ImportUI.bImportTextures = False
factory.ImportUI.bImportAnimations = False
# scale the mesh
factory.ImportUI.SkeletalMeshImportData.ImportUniformScale = 0.1;

# import the mesh
mesh = factory.factory_import_object(os.path.join(kaiju_source, kaiju_mesh), kaiju_destination)


# start configurint the texture importer
texture_factory = TextureFactory()

imported_textures = []

for texture in kaiju_textures:
    imported_texture = texture_factory.factory_import_object(os.path.join(kaiju_source, texture), kaiju_destination)
    imported_textures.append(imported_texture)
    

mat = Material()
#mat.set_name('kaiju_Material_' + str(time.time()))
mat.save_package(kaiju_destination + '/kaiju_Material')

from unreal_engine.classes import MaterialExpressionTextureSample, Texture2D, MaterialExpressionConstant3Vector, MaterialExpressionSine, MaterialExpressionMultiply, MaterialExpressionTime, Skeleton, Character
from unreal_engine.structs import ColorMaterialInput, ScalarMaterialInput, ExpressionInput, VectorMaterialInput, SingleAnimationPlayData
from unreal_engine.enums import EMaterialSamplerType, EAnimationMode

# inform the editor you want to modify the new material
mat.modify()

# build the material graph
sample_base_color = MaterialExpressionTextureSample('', mat)
sample_base_color.Texture = imported_textures[0]
sample_base_color.MaterialExpressionEditorX = -400
sample_base_color.MaterialExpressionEditorY = 0

sample_emissive = MaterialExpressionTextureSample('', mat)
sample_emissive.Texture = imported_textures[1]
sample_emissive.MaterialExpressionEditorX = -600
sample_emissive.MaterialExpressionEditorY = 0

sample_normal = MaterialExpressionTextureSample('', mat)
sample_normal.Texture = imported_textures[2]
sample_normal.SamplerType = EMaterialSamplerType.SAMPLERTYPE_Normal
sample_normal.MaterialExpressionEditorX = -400
sample_normal.MaterialExpressionEditorY = 400

sample_orm = MaterialExpressionTextureSample('', mat)
sample_orm.Texture = imported_textures[3]
sample_orm.MaterialExpressionEditorX = -600
sample_orm.MaterialExpressionEditorY = 400

sine = MaterialExpressionSine('', mat)
sine.MaterialExpressionEditorX = -1000
sine.MaterialExpressionEditorY = 0

time = MaterialExpressionTime('', mat)
time.MaterialExpressionEditorX = -1200
time.MaterialExpressionEditorY = 0

multiply = MaterialExpressionMultiply('', mat)
multiply.MaterialExpressionEditorX = -800
multiply.MaterialExpressionEditorY = 0

mat.Expressions = [sample_base_color, sample_emissive, sample_normal, sample_orm, time, sine, multiply]

sine.Input = ExpressionInput(Expression=time)
multiply.A = ExpressionInput(Expression=sample_emissive)
multiply.B = ExpressionInput(Expression=sine)

mat.BaseColor = ColorMaterialInput(Expression=sample_base_color)
mat.EmissiveColor = ColorMaterialInput(Expression=multiply)
mat.Normal = VectorMaterialInput(Expression=sample_normal)
mat.Roughness = ScalarMaterialInput(Expression=sample_orm, Mask=1, MaskG=1)
mat.Metallic = ScalarMaterialInput(Expression=sample_orm, Mask=1, MaskB=1)
mat.AmbientOcclusion = ScalarMaterialInput(Expression=sample_orm, Mask=1, MaskR=1)

# run material compilatiom
mat.post_edit_change()
# re-save it
mat.save_package()


# import the animation

anim_factory = PyFbxFactory()
anim_factory.ImportUI.Skeleton = mesh.Skeleton
anim_factory.ImportUI.bImportMesh = False
anim_factory.ImportUI.bImportMaterials = False
anim_factory.ImportUI.bImportTextures = False
anim_factory.ImportUI.AnimSequenceImportData.ImportUniformScale = 0.1;

animation = anim_factory.factory_import_object(os.path.join(kaiju_source, kaiju_animation), kaiju_destination)

new_blueprint = ue.create_blueprint(Character, kaiju_destination + '/Kaiju_BP')

new_blueprint.GeneratedClass.get_cdo().Mesh.SkeletalMesh = mesh
new_blueprint.GeneratedClass.get_cdo().Mesh.RelativeLocation = FVector(0, 0, -140)
new_blueprint.GeneratedClass.get_cdo().Mesh.RelativeRotation = FRotator(0, 0, -90)
new_blueprint.GeneratedClass.get_cdo().CapsuleComponent.CapsuleHalfHeight = 150
new_blueprint.GeneratedClass.get_cdo().CapsuleComponent.CapsuleRadius = 50

new_blueprint.GeneratedClass.get_cdo().Mesh.OverrideMaterials = [None, mat]

new_blueprint.GeneratedClass.get_cdo().Mesh.AnimationMode = EAnimationMode.AnimationSingleNode
new_blueprint.GeneratedClass.get_cdo().Mesh.AnimationData = SingleAnimationPlayData(AnimToPlay=animation)

# move forward
tick = new_blueprint.UberGraphPages[0].graph_add_node_event(Actor, 'ReceiveTick')
add_movement_input = new_blueprint.UberGraphPages[0].graph_add_node_call_function(Character.AddMovementInput)
add_movement_input.node_find_pin('WorldDirection').default_value = '1,0,0'
tick.node_find_pin('then').make_link_to(add_movement_input.node_find_pin('execute'))

ue.compile_blueprint(new_blueprint)

world = ue.get_editor_world()
new_actor = world.actor_spawn(new_blueprint.GeneratedClass, FVector(0, 0, 150))

ue.editor_save_all()