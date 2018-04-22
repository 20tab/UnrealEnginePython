import unreal_engine as ue
from unreal_engine.classes import BlueprintFactory, DirectionalLightComponent, K2Node_Event

import time

# create new blueprint from factory
bpFactory = BlueprintFactory()
bp = bpFactory.factory_create_new('/Game/test' + str(int(time.time())))

# add intensity variable
intensity = ue.blueprint_add_member_variable(bp, 'intensity', 'float')
# set its visibility to True
ue.blueprint_set_variable_visibility(bp, 'intensity', True)

# add directional light component
directLightComponent = ue.add_component_to_blueprint(bp,DirectionalLightComponent, "Directional_light")

# add node variables (get) to the graph
intensity_node = bp.UberGraphPages[0].graph_add_node_variable_get('intensity', None, 200, 100)
directional_light_node = bp.UberGraphPages[0].graph_add_node_variable_get('Directional_light', None, 200, 0)

# add the SetIntensity node (from DirectionalLightComponent)
directional_light_set_intensity = bp.UberGraphPages[0].graph_add_node_call_function(DirectionalLightComponent.SetIntensity, 400, 0)

# link variables
intensity_node.node_find_pin('intensity').make_link_to(directional_light_set_intensity.node_find_pin('NewIntensity'))
directional_light_node.node_find_pin('Directional_light').make_link_to(directional_light_set_intensity.node_find_pin('self'))

# a commodity function for finding an event node
def get_event_node(event_name):
    for node in bp.UberGraphPages[0].Nodes:
        if node.is_a(K2Node_Event):
            if node.EventReference.MemberName == event_name:
                return node

# get the ReceiveBeginPlay event node
begin_play_node = get_event_node('ReceiveBeginPlay')

# link BeginPlay to SetIntensity
begin_play_node.node_find_pin('then').make_link_to(directional_light_set_intensity.node_find_pin('execute'))

# compile the blueprint
ue.compile_blueprint(bp)

# open related editor
ue.open_editor_for_asset(bp)

# spawn it
ue.get_editor_world().actor_spawn(bp.GeneratedClass)