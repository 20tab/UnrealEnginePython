import unreal_engine as ue

from unreal_engine.classes import Material
from unreal_engine.enums import EPixelFormat

import cv2

class WebCam:

    def begin_play(self):
        # open the first video capture device
        self.capture = cv2.VideoCapture(0)
        # get its size
        width = int(self.capture.get(cv2.CAP_PROP_FRAME_WIDTH))
        height = int(self.capture.get(cv2.CAP_PROP_FRAME_HEIGHT))
        # create a new texture with the desired size
        self.texture = ue.create_transient_texture(width, height, EPixelFormat.PF_R8G8B8A8)

        # assign the texture to the material mapped to the cube
        self.material = self.uobject.create_material_instance_dynamic(ue.load_object(Material, '/Game/DumbMaterial'))
        self.uobject.get_owner().StaticMeshComponent.set_material(0, self.material)
        self.material.set_material_texture_parameter('Texture', self.texture)

    def tick(self, delta_time):
        # get webcam frame
        ret, frame = self.capture.read()
        if not ret:
            return
        # frames are in plain RGB, convert them to a texture-useful format
        frame = cv2.cvtColor(frame, cv2.COLOR_BGR2RGBA)
        self.texture.texture_set_data(frame)