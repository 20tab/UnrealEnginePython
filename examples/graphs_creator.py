import unreal_engine as ue
from unreal_engine.classes import K2Node_InputKey, K2Node_SpawnActorFromClass, Actor, Character, KismetMathLibrary
from unreal_engine.structs import Key

# create  a new blueprint
new_blueprint = ue.create_blueprint(Actor, '/Game/StrangeBlueprint')

# add a member float variable
ue.blueprint_add_member_variable(new_blueprint, 'Speed', 'float')

# get a reference to the first graph page
uber_page = new_blueprint.UberGraphPages[0]

# get good coordinates for a new node
x, y = uber_page.graph_get_good_place_for_new_node()
# add the Speed variable to the graph
uber_page.graph_add_node_variable_get('Speed', None, x, y)

x, y = uber_page.graph_get_good_place_for_new_node()
# add a custom event to the graph
hello_world = uber_page.graph_add_node_custom_event('Hello World', x, y)

# get a reference to the 'then' pin
hello_world_then = hello_world.node_find_pin('then')

x, y = uber_page.graph_get_good_place_for_new_node()
# add a 'Spawn Actor From Class' node
spawn_actor_node = uber_page.graph_add_node(K2Node_SpawnActorFromClass, x, y)

# set its Class pin
pin_class = spawn_actor_node.node_find_pin('Class')
pin_class.default_object = Character

# get a reference to its 'exec' pin
spawn_actor_node_exec = spawn_actor_node.node_find_pin('execute')

# link the hello world event to the spawn actor node
hello_world_then.make_link_to(spawn_actor_node_exec)

x, y = uber_page.graph_get_good_place_for_new_node()
# create a 'make transform' node
make_transform = uber_page.graph_add_node_call_function(KismetMathLibrary.MakeTransform, x, y)

# link the return value of 'make transform' to the 'SpawnTransform' pin of the spawn actor node
make_transform.node_find_pin('ReturnValue').make_link_to(spawn_actor_node.node_find_pin('SpawnTransform'))

input_key = K2Node_InputKey()
input_key.InputKey = Key(KeyName='SpaceBar')
input_key_node = uber_page.graph_add_node(input_key, 400, 400)

# compile the blueprint
ue.compile_blueprint(new_blueprint)

# save it
ue.editor_save_all()
