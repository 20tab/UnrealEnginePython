import unreal_engine as ue

from unreal_engine import SWindow, SButton, STextBlock, SEditableTextBox, SMultiLineEditableText, SGridPanel, SHorizontalBox, SPythonEditorViewport
from unreal_engine.classes import Blueprint

def clicked(geometry, pointer_event):
    ue.log('Hello')
    ue.log(geometry.fields())
    ue.log(pointer_event.fields())
    return True

def clicked2():
    ue.log('Test')
    return True

window = SWindow().resize(1024, 576).set_title('Hello World')

horizontal = SHorizontalBox()

button = SButton().set_content(STextBlock().set_text('Click ME !'))
button.bind_on_mouse_button_down(clicked)
button.bind_on_clicked(clicked2)

box = SEditableTextBox()

editor = SMultiLineEditableText()
editor.set_text('Hello')

grid = SGridPanel()

grid.add_slot(button, 0, 0)
grid.add_slot(box, 0, 1)
grid.add_slot(editor, 0, 2)

horizontal.add_slot(grid, v_align=2, h_align=2).add_slot(STextBlock().set_text('Second'), v_align=2, h_align=2)

viewport = SPythonEditorViewport()

world = viewport.get_world()
bear_bp = ue.load_object(Blueprint, '/Game/Character3_Blueprint')

bear = world.actor_spawn(bear_bp.GeneratedClass)

horizontal.add_slot(viewport)

window.set_content(horizontal)
