import unreal_engine as ue

class FacesDetector:

    def __init__(self):
        self.texture_to_draw = None
               
    def draw_hud(self):
        # exit if we do not have enough data
        if not self.texture_to_draw:
            return

        # draw what the player pawn is seeing
        self.uobject.hud_draw_texture(self.texture_to_draw, 0, 0, 256, 256)