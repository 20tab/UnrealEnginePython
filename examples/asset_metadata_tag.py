import unreal_engine as ue
from unreal_engine.classes import EditorAssetLibrary

asset = ue.get_selected_assets()[0]

# reflection-based api
EditorAssetLibrary.SetMetadataTag(asset, 'Foo', 'Bar')
for value in EditorAssetLibrary.GetMetadataTagValues(asset):
    print(value)
print(EditorAssetLibrary.GetMetadataTag(asset, 'Foo'))


# native api
asset.set_metadata_tag('Test001', 'Text002')
asset.set_metadata_tag('Test003', 'Text004')
for key in asset.metadata_tags():
    print(key)
print(asset.has_metadata_tag('Foo')) # bool
print(asset.get_metadata_tag('Test001'))
