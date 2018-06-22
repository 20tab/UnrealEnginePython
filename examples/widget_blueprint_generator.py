import unreal_engine as ue

from unreal_engine.classes import Image, CanvasPanelSlot, TextBlock, WidgetBlueprintFactory
from unreal_engine.structs import AnchorData, Anchors, Vector2D, Margin

import time

widget = WidgetBlueprintFactory().factory_create_new( "/Game/W_MyWidgetBlueprint" + str(int(time.time())) )

widget.modify()

widgetTree = widget.WidgetTree


slot = CanvasPanelSlot('', widgetTree)
image = Image('', widgetTree)

slot.Content = image

slot2 = CanvasPanelSlot('', widgetTree)
text_block = TextBlock('', widgetTree)
text_block.Text = 'Hello World'

slot2.Content = text_block

widgetTree.RootWidget.Slots = [slot, slot2]
widgetTree.AllWidgets = [ widgetTree.RootWidget, image, text_block ]

slot.LayoutData = AnchorData(Offsets=Margin(Left=0, Top=0, Right=300, Bottom=300))

slot2.LayoutData = AnchorData(Anchors=Anchors(Minimum=Vector2D(X=0.5, Y=0.5), Maximum=Vector2D(X=1, Y=1)))

widget.post_edit_change()

ue.compile_blueprint(widget)

ue.open_editor_for_asset(widget)