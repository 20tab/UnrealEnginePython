import unreal_engine as ue
from unreal_engine.classes import ActorComponent


class FooComponent(ActorComponent):
    def __init__(self):
        ue.log('Ctor')

    def DoSomething():
        ue.print_string('TEST !!!')

FooComponent.set_metadata('BlueprintType', 'true')
FooComponent.set_metadata('BlueprintSpawnableComponent', 'true')
FooComponent.set_metadata('IsBlueprintBase', 'true')
FooComponent.component_type_registry_invalidate_class()
