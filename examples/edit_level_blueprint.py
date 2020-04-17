import unreal_engine as ue
from unreal_engine.structs import EdGraphPinType

world = ue.get_editor_world()
level_bp = world.CurrentLevel.get_level_script_blueprint()
pin = EdGraphPinType(PinCategory='string')
ue.blueprint_add_member_variable(level_bp, 'TestString', pin)
ue.open_editor_for_asset(level_bp)
