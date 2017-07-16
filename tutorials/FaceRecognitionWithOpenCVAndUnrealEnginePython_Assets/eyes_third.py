import unreal_engine as ue

from unreal_engine.classes import SceneCaptureComponent2D
from unreal_engine.enums import ESceneCaptureSource

class Sight:

    def __init__(self):
        self.what_i_am_seeing = ue.create_transient_texture_render_target2d(512, 512)

    def pre_initialize_components(self):
        # add a new root component (a SceneCaptureComponent2D one)
        self.scene_capturer = self.uobject.add_actor_root_component(SceneCaptureComponent2D, 'Scene Capture')
        # use the previously created texture as the render target
        self.scene_capturer.TextureTarget = self.what_i_am_seeing
        # store pixels as linear colors (non HDR)
        self.scene_capturer.CaptureSource = ESceneCaptureSource.SCS_FinalColorLDR

    def begin_play(self):
        # get a reference to the pawn currently controlled by the player
        mannequin = self.uobject.get_player_pawn()
        # attach myself to the 'head' bone of the mannequin Mesh component
        self.uobject.attach_to_component(mannequin.Mesh, 'head')