import unreal_engine as ue
from unreal_engine.enums import EPixelFormat

import matplotlib
matplotlib.use('Agg')

import matplotlib.pyplot as plt

class PlotComponent:
  
    def begin_play(self):
        width = 1024
        height = 1024
        dpi = 72.0
        self.texture = ue.create_transient_texture(width, height, EPixelFormat.PF_R8G8B8A8)

        self.uobject.get_owner().StaticMeshComponent.OverrideMaterials[0].set_material_texture_parameter('Graph', self.texture)

        self.fig = plt.figure(1)
        self.fig.set_dpi(dpi)
        self.fig.set_figwidth(width/dpi)
        self.fig.set_figheight(height/dpi)

        self.uobject.get_owner().bind_event('OnGraphDataUpdated', self.update_graph)

    def update_graph(self, platform):
        # clear the current plot data
        plt.clf()
        # draw a pie chart        
        plt.pie([platform.RedCubeCounter, platform.GreenCubeCounter, platform.BlueCubeCounter], colors=['r', 'g', 'b'], labels=['RedCube', 'GreenCube', 'BlueCube'], shadow=True)

        self.fig.canvas.draw()
        self.texture.texture_set_data(self.fig.canvas.buffer_rgba())
        

        
