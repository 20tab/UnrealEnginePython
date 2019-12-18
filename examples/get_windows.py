import unreal_engine as ue
from unreal_engine import FSlateApplication
from unreal_engine.classes import GameViewportClient

def iterate_window(window):
    print(window.get_title())
    for child in window.get_child_windows():
        iterate_window(child)

# get the active top level window
top_window = FSlateApplication.get_active_top_level_window()
iterate_window(top_window)

# get Game GameViewportClient
try:
    iterate_window(ue.get_game_viewport_client().game_viewport_client_get_window())
except:
    pass

# get PIE GameViewportClient
try:
    iterate_window(ue.get_editor_pie_game_viewport_client().game_viewport_client_get_window())
except:
    pass

# iterate all GameViewportClient uobject's
for game_viewport_client in ue.tobject_iterator(GameViewportClient):
    iterate_window(game_viewport_client.game_viewport_client_get_window())