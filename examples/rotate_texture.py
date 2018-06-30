from PIL import Image
import unreal_engine as ue
from unreal_engine.classes import Texture2D

def rotate_selected_assets_data(degrees):
    for asset in ue.get_selected_assets():
        if asset.is_a(Texture2D):
            data = asset.texture_get_source_data()
            
            size = (asset.texture_get_width(), asset.texture_get_height())
           
            img = Image.frombytes('RGBA', size, bytes(data))
            
            out = img.rotate(degrees)

            asset.texture_set_source_data(out.tobytes())

            
            

rotate_selected_assets_data(45)
