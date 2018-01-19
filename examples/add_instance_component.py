import unreal_engine as ue
from unreal_engine.classes import PawnSensingComponent

actor = ue.editor_get_selected_actors()[0]
component = actor.add_actor_component(PawnSensingComponent, 'Foo')
actor.add_instance_component(component)
actor.modify()
