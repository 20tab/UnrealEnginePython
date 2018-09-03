import unreal_engine as ue
from unreal_engine.classes import GroupActor

import time

world = ue.get_editor_world()
group_actor = world.actor_spawn(GroupActor)

for folder in world.world_folders():
    print(folder)

new_folder = 'FooBar_{}'.format(int(time.time()))

world.world_create_folder(new_folder)

world.world_rename_folder(new_folder, new_folder + '__hello')

world.world_delete_folder(new_folder + '__hello')

actor = ue.editor_get_selected_actors()[0]

# folder tree will be automatically created
actor.set_folder_path('Test1/Test2/Test3')

# assign actors to the group
group_actor.GroupActors = [actor]