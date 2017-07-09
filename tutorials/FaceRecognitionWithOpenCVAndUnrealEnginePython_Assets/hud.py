import unreal_engine as ue

from unreal_engine import FColor, FLinearColor

from unreal_engine.enums import EBlendMode, EPixelFormat


import os
import cv2

class FacesDetector:

    def __init__(self):
        self.texture_to_draw = None
        self.gray_data = None
        
        self.texture = ue.create_transient_texture(512, 512, EPixelFormat.PF_G8)
        self.texture.auto_root()
        filename = os.path.join(ue.get_content_dir(), 'haarcascade_frontalface_default.xml')
        self.cascade = cv2.CascadeClassifier(filename)
        
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
    
        # detect faces
        faces = self.cascade.detectMultiScale(self.gray_data)
        # highlight detected areas
        for x, y, w, h in faces:
            self.uobject.hud_draw_rect(x/2.0, 256 + y/2.0, w/2.0, h/2.0, FLinearColor(0, 1, 0, 0.5))
        # report the number of detected faces
        self.uobject.hud_draw_text('I am seeing {0} faces'.format(len(faces)), 500, 50, FLinearColor(0, 1, 1, 1), 2)