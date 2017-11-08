import unreal_engine as ue
from unreal_engine import FARFilter

_filter = FARFilter()
_filter.class_names = ['StaticMesh']

# when passing True to the second argument of get_assets_by_filter(), you will get FAssetData instead of UObject
for asset_data in ue.get_assets_by_filter(_filter, True):
    has_custom_thumbnail = asset_data.has_custom_thumbnail()
    has_cached_thumbnail = asset_data.has_cached_thumbnail()
    try:
        thumbnail = asset_data.get_thumbnail()
    except:
        thumbnail = None
    ue.log('Asset: {0} Loaded: {1} CustomThumbnail: {2} CachedThumbnail: {3} Thumbnail: {4}'.format(asset_data.object_path, asset_data.is_asset_loaded(), has_custom_thumbnail, has_cached_thumbnail, thumbnail)) 