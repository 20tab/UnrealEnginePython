import unreal_engine as ue
from unreal_engine import FSlateIcon, FSlateStyleSet, FLinearColor
from unreal_engine.structs import SlateBrush, SlateColor, Vector2D

# this code must be in ue_site.py, extenders must be  registered during editor startup phase !!!

# we create a new StyleSet to register a new Icon

# create a new styleset
style = FSlateStyleSet('PyStyle')
# allocate a solid color brush (check here https://docs.unrealengine.com/latest/INT/API/Runtime/SlateCore/Styling/FSlateBrush/index.html)
brush = SlateBrush(TintColor=SlateColor(SpecifiedColor=FLinearColor(1, 0, 0, 1)))
# register the brush in the 'PyStyle' StyleSet
style.set('SolidBrush001', brush)
# allocate a brush image (best image size for toolbar is 64x64)
# Note: pass a path to a valid image file
brush2 = SlateBrush(ResourceName='C:/Users/Roberto/Desktop/chessboard.png', ImageSize=Vector2D(X=128, Y=128))
# register the brush
style.set('ImageBrush001', brush2)
# register the new style
style.register()

# this is called whenever you select the SimpleMenuExtension entry
def dumb():
   ue.log('HELLO WORLD')

# this generates the menu entries
def open_menu(menu):
   menu.begin_section('test1', 'test2')
   menu.add_menu_entry('one', 'two', lambda: ue.log('ciao 1'))
   menu.add_menu_entry('three', 'four', lambda: ue.log('ciao 2'))
   menu.end_section()

# this fills the toolbar
def fill_toolbar(toolbar):
	icon1 = FSlateIcon('PyStyle', 'SolidBrush001')
	icon2 = FSlateIcon('PyStyle', 'ImageBrush001')
	toolbar.add_tool_bar_button('button001', 'Button001', 'Button001 tooltip', icon1, lambda: ue.log('Button001'))
	toolbar.add_tool_bar_button('button002', 'Button002', 'Button002 tooltip', icon2, lambda: ue.log('Button002'))

ue.add_menu_extension('SimpleMenuExtension', dumb)

ue.add_menu_bar_extension('SimpleMenuBarExtension', open_menu)

ue.add_tool_bar_extension('SimpleToolBarExtension', fill_toolbar)
