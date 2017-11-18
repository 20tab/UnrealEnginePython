import unreal_engine as ue

for viewport_client in ue.all_viewport_clients():
    print(viewport_client.get_world().get_name())
    origin, size = viewport_client.get_viewport_dimensions()
    print(viewport_client.get_look_at_location())
    print(viewport_client.get_view_location())
    