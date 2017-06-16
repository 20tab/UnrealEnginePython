from unreal_engine import SWindow, SBox, SPythonComboBox, STextBlock, SBorder, SVerticalBox, SEditableTextBox, SHorizontalBox, SButton
from unreal_engine.enums import EHorizontalAlignment, EVerticalAlignment
from unreal_engine import FLinearColor
from unreal_engine.structs import SlateColor, Margin

class DynamicComboBox:

    def __init__(self, items):
        self.box = SBox(height_override=100, min_desired_width=400)
        self.items = items
        self.build_combo_box()
        self.combo_box.set_selected_item(self.items[0])

    def get_widget(self):
        return self.box

    def generate_combo_box_widget(self, item):
        return STextBlock(text=item)

    def append(self, item, committed):
        if item and item not in self.items:
            self.items.append(item)
            self.build_combo_box()
            self.combo_box.set_selected_item(item)

    def get_current_item(self):
        return self.combo_box.get_selected_item()

    def build_combo_box(self):
        self.combo_box = SPythonComboBox(options_source=self.items, on_generate_widget=self.generate_combo_box_widget, content=STextBlock(text=self.get_current_item))
        self.box.set_content(self.combo_box)

    

dynamic_combo_box = DynamicComboBox(['one','two', 'three'])

# the final backslash is required for the 'pretty syntax'
SWindow(client_size=(1024, 576), title='DynamicComboBox')\
(
    SBorder(color_and_opacity=FLinearColor(0, 1, 0, 1), border_background_color=SlateColor(SpecifiedColor=FLinearColor(1, 0, 0, 1)))
    (
        SBox(h_align=EHorizontalAlignment.HAlign_Center, v_align=EVerticalAlignment.VAlign_Center)
        (
            SBorder(color_and_opacity=FLinearColor(0, 1, 0, 1), border_background_color=SlateColor(SpecifiedColor=FLinearColor(0, 1, 0, 1)))
            (
                SVerticalBox()
                (
                    STextBlock(text='Hello 1', tool_tip_text='Test Tool Tip')
                )
                (
                    STextBlock(text='Hello 2')
                )
                (
                    dynamic_combo_box.get_widget()
                )
                (
                    SHorizontalBox()
                    (
                        SEditableTextBox(text=lambda: '', on_text_committed=dynamic_combo_box.append),
                        fill_width=0.8
                    )
                    (
                        SButton(text='Ok'),
                        fill_width=0.2
                    )
                )
            )
        )
    )
)