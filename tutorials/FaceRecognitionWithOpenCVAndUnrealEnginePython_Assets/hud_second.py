import unreal_engine as ue

from unreal_engine import FColor, FLinearColor

from unreal_engine.enums import EBlendMode, EPixelFormat


import os
import cv2

class FacesDetector:

    def __init__(self):
        self.texture_to_draw = None
        self.gray_data = None
        
        # create a transient texture with a single color channel
        self.texture = ue.create_transient_texture(512, 512, EPixelFormat.PF_G8)
        # this automatically destroys the texture when self.texture dies (this is required as UE does not know about this texture object)
        self.texture.auto_root()
                
    def draw_hud(self):
        # exit if we do not have enough data
        if not self.texture_to_draw:
            return

        # upload grayscale data into the GPU
        self.texture.texture_set_data(self.gray_data)
        
        # draw what the player pawn is seeing
        self.uobject.hud_draw_texture(self.texture_to_draw, 0, 0, 256, 256)

        # draw in opaque mode (ignore transparency)
        self.uobject.hud_draw_texture(self.texture, 0, 256, 256, 256, 0, 0, 1, 1, FLinearColor(1, 1, 1, 1), EBlendMode.BLEND_Opaque)