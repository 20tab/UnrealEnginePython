import unreal_engine as ue
import os
import png

# run play in the editor and execute this script (with ue.exec) from the python console

width, height = ue.editor_get_pie_viewport_size()
pixels = ue.editor_get_pie_viewport_screenshot()

ue.log("{0} {1} {2}".format(width, height, len(pixels)))

png_pixels = []

for y in range(0, height):
	line = []
	for x in range(0, width):
		index = y * width + x
		pixel = pixels[index]
		line.append([pixel.r, pixel.g, pixel.b, pixel.a])
	png_pixels.append(line)


path = os.path.expanduser("~/Desktop/pie_screenshot.png")

png.from_array(png_pixels, 'RGBA').save(path)
