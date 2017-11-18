import unreal_engine as ue
from unreal_engine.classes import StaticMesh
from unreal_engine import SWindow, SScrollBox, SVerticalBox, SHorizontalBox, SImage, STextBlock, FARFilter
from unreal_engine.enums import EPixelFormat

_filter = FARFilter()
_filter.class_names = ['StaticMesh']

thumbnails = []

for static_mesh in ue.get_assets_by_filter(_filter):
    thumbnails.append((static_mesh.get_full_name(), static_mesh.get_thumbnail()))


window = SWindow(client_size=(1024, 512), title='StaticMeshes thumbnails')
scroll_box = SScrollBox()

vertical = SVerticalBox()

for name, thumbnail in thumbnails:
    texture = ue.create_transient_texture(thumbnail.get_image_width(), thumbnail.get_image_height())
    texture.texture_set_data(thumbnail.get_uncompressed_image_data())
    vertical.add_slot(
        SHorizontalBox()
        (
            SImage().set_texture(texture),
            max_width=64
        )
        (
            STextBlock(text=name),
            padding=4,
            auto_width=True,
        ),
        max_height=64
    )

scroll_box.add_slot(vertical)
window.set_content(scroll_box)
