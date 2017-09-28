# The Thumbnail API

The UE4 python api exposes two main functions to access thumbnails:

```python
thumbnail = uobject.get_thumbnail([force_create])
```

This function tries to get the thumbnail of an object from the cache, or (if not found in the cache) from the package file.

The boolean argument, if True, forces the function to generate a new thumbnail (with the default size) if not available in the asset/package

The returned object is of type FObjectThumbail (see below)

```python
thumbnail = uobject.render_thumbnail([width, height, no_flush])
```

This function generates a new FObjectThumbail from an object. By default texture streaming is flushed after the creation of the thumbnail. By specyfing 'no_flush' the streaming will not be flushed.

## The FObjectThumbnail object

This class represents a thumbnail


The following methods are available:

```python
width = thumbnail.get_image_width()
```

```python
height = thumbnail.get_image_height()
```

```python
# caould return zero if the thumbnail is not compressed
size = thumbnail.get_compressed_data_size()
```

```python
# returns BGRA bytearray of pixels
data = thumbnail.get_uncompressed_image_data()
```

```python
# if this is a newly created thumbnail, force compression
thumbnail.compress_image_data()
```

For a handy example about the thumbnail api check https://github.com/20tab/UnrealEnginePython/blob/master/examples/thumbnail_viewer.py
