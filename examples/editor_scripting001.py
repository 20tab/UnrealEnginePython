import unreal_engine as ue
from unreal_engine import FVector

from unreal_engine.classes import Actor, Pawn, Character, ProjectileMovementComponent, PawnSensingComponent, StaticMesh
from unreal_engine.classes import StaticMeshComponent, StaticMeshActor, PointLightComponent

world = ue.get_editor_world()

# instantiate 10 pawns with projectile component
for i in range(0, 10):
	new_pawn = world.actor_spawn(Pawn, FVector(0, 0, i * 1000))
	new_pawn.set_actor_label("FooBarPawn {0}".format(i))
	projectile = new_pawn.add_actor_component(ProjectileMovementComponent, 'Projectile Movement')
	projectile.InitialSpeed = 173.0
	projectile.MaxSpeed = 999.9

# instantiate 10 characters with sensing component
for i in range(0, 10):
	new_character = world.actor_spawn(Character, FVector(i * 100, 0, i * 1000))
	new_character.set_actor_label("FooBarCharacter {0}".format(i))
	sensing = new_character.add_actor_component(PawnSensingComponent, 'Ears')

# add a cone
cone = world.actor_spawn(Actor)
cone.set_actor_label('A Cone')
cone_mesh = cone.add_actor_component(StaticMeshComponent, 'Cone')
cone_mesh.StaticMesh = ue.load_object(StaticMesh, '/Engine/BasicShapes/Cone')
cone.InitialLifeSpan = 30.0

# add a better cone
cone2 = world.actor_spawn(StaticMeshActor)
cone2.StaticMeshComponent.StaticMesh = ue.load_object(StaticMesh, '/Engine/BasicShapes/Cone')
cone2.set_actor_label('A Better Cone')

# create a new bleprint
bp = ue.create_blueprint(Pawn, '/Game/SuperEvilPawn')
ue.add_component_to_blueprint(bp, PawnSensingComponent, 'Eyes')
point_light = ue.add_component_to_blueprint(bp, PointLightComponent, 'Light')
point_light.Intensity = 9999.9
ue.blueprint_add_member_variable(bp, 'SightPower', 'float')
ue.blueprint_set_variable_visibility(bp, 'SightPower', False)

ue.compile_blueprint(bp)

# instantiate a new actor using the previously created blueprint
super_evil_pawn = world.actor_spawn(bp.GeneratedClass)
super_evil_pawn.set_actor_label('Super Evil Pawn')
super_evil_pawn.SightPower = 2217.30

# save them all
ue.editor_save_all()


