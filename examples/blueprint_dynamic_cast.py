import unreal_engine as ue
from unreal_engine.classes import Blueprint, K2Node_DynamicCast, Actor, Object
from unreal_engine.structs import EdGraphPinType
from unreal_engine.enums import EEdGraphPinDirection

bp_foo = ue.load_object(Blueprint, '/Game/Foo.Foo')
bp_bar = ue.load_object(Blueprint, '/Game/Bar.Bar')

cast_node = K2Node_DynamicCast()
cast_node.TargetType = bp_bar.GeneratedClass

graph = ue.blueprint_add_function(bp_foo, 'FooCaster')
func = graph.Nodes[0]

pin_type = EdGraphPinType(PinCategory = 'object', PinSubCategoryObject=Actor)
pin = func.node_create_pin(EEdGraphPinDirection.EGPD_Input, pin_type, 'Arg001')


graph.graph_add_node(cast_node, 600, 0)

cast_node.node_find_pin('Object').category = 'object'
cast_node.node_find_pin('Object').sub_category = Object

pin.make_link_to(cast_node.node_find_pin('Object'))
func.node_find_pin('then').make_link_to(cast_node.node_find_pin('execute'))

ue.compile_blueprint(bp_foo)