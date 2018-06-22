import unreal_engine as ue
from unreal_engine import SFilePathPicker, SWindow, FLinearColor
from unreal_engine.structs import ButtonStyle, SlateBrush, SlateColor

# a style is required for the file picker
style = ButtonStyle(Normal=SlateBrush(TintColor=SlateColor(SpecifiedColor=FLinearColor(1, 0, 0))))


window = SWindow(client_size=(576,576), title='Hello', modal=True)

def path_picked(path):
    print(path)
    window.request_destroy()

picker = SFilePathPicker(browse_title='Hello', browse_button_style=style, on_path_picked=path_picked)
window.set_content(picker)

window.add_modal()