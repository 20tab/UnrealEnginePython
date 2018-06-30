from PIL import Image
import unreal_engine as ue
from unreal_engine.classes import Texture2D

def rotate_selected_assets_data(degrees):
    for asset in ue.get_selected_assets():
        if asset.is_a(Texture2D):
            tex = ue.load_object(Texture2D, asset.get_path_name())
            data = tex.texture_get_data()
            size = (asset.Blueprint_GetSizeX(), asset.Blueprint_GetSizeY())

            img = Image.frombytes("RGB", size, data)
            img.show()
            out = img.rotate(degrees, expand=1)
            tex.texture_set_data(out.tobytes())
